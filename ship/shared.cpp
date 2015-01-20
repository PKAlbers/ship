//
//  shared.cpp
//  ship
//
//  Created by Patrick Albers on 11.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "shared.h"



//******************************************************************************
// Shared haplotype containers
//******************************************************************************

//
// Shared haplotype
//

SharedType::SharedType(const Haplotype _haplotype, const size_t _marker_id)
: haplotype(_haplotype)
, marker_id(_marker_id)
{}


//
// Shared haplotype tree structure
//

SharedTree::SharedTree(const bool _side)
: side(_side)
{}

void SharedTree::scan(const Source & source, const SharedType & type)
{
	static const int hmax = Haplotype::unknown + 1;
	const size_t n_marker = source.marker_size(); // right hand side bound
	const size_t n_sample = type.sample_id.size(); // number of subsamples
	
	this->stop = type.marker_id;
	
	if (n_sample < 2)
		return;
	
	size_t marker_id = type.marker_id; // current marker ID
	
	// walkabout
	while (true)
	{
		if (this->side) // right scan
		{
			++marker_id;
			
			if (marker_id == n_marker)
				break;
		}
		else // left scan
		{
			if (marker_id == 0)
				break;
			
			--marker_id;
		}
		
		bool flag = true; // flag that all are heterozygous
		
		const Marker * mptr = &source.marker(marker_id);
		
		int G0[ n_sample ];
		int G1[ n_sample ];
		
		bool H[ hmax ] = { false };
		
		// collect genotypes & haplotypes
		for (size_t i = 0; i < n_sample; ++i)
		{
			const Genotype g = mptr->data[ type.sample_id[i] ];
			const int x0 = (int)g.h0;
			const int x1 = (int)g.h1;
			
			G0[i] = x0;
			G1[i] = x1;
			
			H[x0] = true;
			
			if (g.h0 != g.h1)
			{
				H[x1] = true;
			}
			else
			{
				flag = false;
			}
		}
		
		int n_haplotypes = 0;
		
		for (int i = 0; i < hmax; ++i)
		{
			if (H[i])
				++n_haplotypes;
		}
		
		if ( n_haplotypes == 1 ||        // all homozygous
			(n_haplotypes == 2 && flag)) // all heterozygous with same two haplotypes
		{
			this->stop = marker_id;
			continue; // no breakpoint
		}
		else // check each haplotype to be shared by all
		{
			flag = false; // flag that haplotype is shared by none
			
			for (int h = 0; h < hmax; ++h)
			{
				if (!H[h])
					continue;
				
				int n_shared = 0;
				
				for (size_t i = 0; i < n_sample; ++i)
				{
					if (G0[i] == h || G1[i] == h)
						++n_shared;
				}
				
				if (n_shared == n_sample)
				{
					flag = true;
					break;
				}
			}
			
			if (flag) // haplotype is shared by all
			{
				this->stop = marker_id;
				continue; // no breakpoint
			}
			else // breakpoint
			{
				this->node.reserve(n_haplotypes);
				
				for (int h = 0; h < hmax; ++h)
				{
					if (!H[h])
						continue;

					std::vector<size_t> subsample_id;
					int n_subsample = 0;
					
					for (size_t i = 0; i < n_sample; ++i)
					{
						if (G0[i] == h || G1[i] == h)
						{
							subsample_id.push_back(type.sample_id[i]);
							++n_subsample;
						}
					}
					
					if (n_subsample > 2)
					{
						SharedNode node(Haplotype(h), marker_id, this->side); // new sub node
						
						node.type.sample_id = std::move(subsample_id); // insert subsample

						this->node.push_back(std::move(node)); // insert node
					}
				}
				
				// scan trees for each node
				for (std::vector<SharedNode>::iterator it = this->node.begin(), end = this->node.end(); it != end; ++it)
				{
					it->tree.scan(source, it->type);
				}
				
				break;
			}
		}
	}
}

size_t SharedTree::count() const
{
	size_t sum, n = this->node.size();
	
	sum = n;
	
	for (int i = 0; i < n; ++i)
	{
		sum += this->node[i].tree.count();
	}
	
	return sum;
}


//
// Shared haplotype node in tree
//

SharedNode::SharedNode(const Haplotype _haplotype, const size_t _marker_id, const bool _side)
: type(_haplotype, _marker_id)
, tree(_side)
{}


//
// Shared haplotype root of tree
//

SharedRoot::SharedRoot(const Haplotype _haplotype, const size_t _marker_id)
: type(_haplotype, _marker_id)
, ltree(false)
, rtree(true)
{}

void SharedRoot::subsample(const Source & source)
{
	const Marker * mptr = &source.marker(this->type.marker_id);
	
	for (size_t sample_id = 0, n = source.sample_size(); sample_id < n; ++sample_id)
	{
		const Genotype g = mptr->data[sample_id];
		
		if (g.h0 == this->type.haplotype || g.h1 == this->type.haplotype)
		{
			this->type.sample_id.push_back(sample_id);
		}
	}
}

void SharedRoot::scan(const Source & source)
{
	this->ltree.scan(source, this->type);
	this->rtree.scan(source, this->type);
}


//
// All shared haplotypes
//

Shared::Shared(const Source & source, const Census & cutoff)
: size_(0)
, marker_count_(0)
{
	for (size_t i = 0; i < source.marker_size(); ++i)
	{
		const Marker * mptr = &source.marker(i);
		bool flag = false;
		
		for (int k = 0; k < mptr->stat.haplotype.size(); ++k)
		{
			if (mptr->stat.haplotype.census(k) >  size_t(1) && // exclude singletons
				mptr->stat.haplotype.census(k) <= cutoff) // below/equal specified threshold
			{
				this->root.push_back(SharedRoot(mptr->stat.haplotype.type(k), i));
				++this->size_;
				flag = true;
			}
		}
		
		if (flag)
			++this->marker_count_;
	}
}

const SharedRoot & Shared::operator [] (const size_t _i) const
{
#ifdef DEBUG_SHARED
	if (_i > this->size_)
	{
		throw std::out_of_range("Shared haplotype out of range");
	}
#endif
	
	return this->root[_i];
}

SharedRoot & Shared::at(const size_t _i)
{
#ifdef DEBUG_SHARED
	if (_i > this->size_)
	{
		throw std::out_of_range("Shared haplotype out of range");
	}
#endif
	
	return this->root[_i];
}

size_t Shared::size() const
{
	return this->size_;
}

size_t Shared::marker_count() const
{
	return this->marker_count_;
}

void Shared::scan_parallel(const std::vector<size_t> & index, const Source & source, ProgressBar & progress)
{
	for (size_t i : index)
	{
		this->ex_scan.lock();
		progress.update();
		this->ex_scan.unlock();
		
		this->root[i].scan(source);
	}
}

void Shared::scan(const Source & source, const int threads)
{
	ProgressBar progress(this->size_);
	
	std::vector<std::thread> t;
	std::vector< std::vector<size_t> > index(threads);
	
	int k = 0;
	for (size_t i = 0; i < this->size_; ++i)
	{
		index[k++].push_back(i);
		
		if (k == threads)
			k = 0;
	}
	
	for (k = 1; k < threads; ++k)
	{
		t.push_back(std::thread(&Shared::scan_parallel, this, std::ref(index[k]), std::ref(source), std::ref(progress)));
	}
	
	this->scan_parallel(index[0], source, progress); // run on this thread
	
	for (std::thread & _t : t)
	{
		_t.join();
	}
	
	progress.finish();
}





