//
//  shared.cpp
//  ship
//
//  Created by Patrick Albers on 11.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "shared.h"



//******************************************************************************
// Shared haplotype container
//******************************************************************************

SharedHaplotype::SharedHaplotype(const size_t _marker_id, const Haplotype _haplotype)
: size_(0)
, marker_id(_marker_id)
, haplotype(_haplotype)
{}

void SharedHaplotype::detect(const Source & source)
{
	for (size_t i = 0, n = source.sample_size(); i < n; ++i)
	{
		const Genotype g = source.sample(i).data[this->marker_id];
		
		if (g.h0 == this->haplotype || g.h1 == this->haplotype)
		{
			this->sample_id.push_back(i);
			++this->size_;
		}
	}
}

size_t SharedHaplotype::subsample(const size_t _i) const
{
#ifdef DEBUG_SHARED
	if (_i > this->size_)
	{
		throw std::out_of_range("Subsample out of range");
	}
#endif
	
	return this->sample_id[_i];
}

size_t SharedHaplotype::size() const
{
	return this->size_;
}



//******************************************************************************
// Shared haplotype list
//******************************************************************************

Shared::Shared()
: size_(0)
, marker_count_(0)
{}

void Shared::identify(const Source & source, const Census & cutoff)
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
				this->list.push_back(SharedHaplotype(i, mptr->stat.haplotype.type(k)));
				++this->size_;
				flag = true;
			}
		}
		
		if (flag)
			++this->marker_count_;
	}
}

const SharedHaplotype & Shared::operator [] (const size_t _i) const
{
#ifdef DEBUG_SHARED
	if (_i > this->size_)
	{
		throw std::out_of_range("Shared haplotype out of range");
	}
#endif
	
	return this->list[_i];
}

SharedHaplotype & Shared::at(const size_t _i)
{
#ifdef DEBUG_SHARED
	if (_i > this->size_)
	{
		throw std::out_of_range("Shared haplotype out of range");
	}
#endif
	
	return this->list[_i];
}

size_t Shared::size() const
{
	return this->size_;
}

size_t Shared::marker_count() const
{
	return this->marker_count_;
}


