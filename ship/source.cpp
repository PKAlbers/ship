//
//  source.cpp
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "source.h"


//******************************************************************************
// Marker & sample (data matrix) container
//******************************************************************************


Source::Source()
: sample_size_(0)
, marker_size_(0)
, finished(false)
{}

Source::Source(const Source & other)
: sample_(other.sample_)
, marker_(other.marker_)
, sample_size_(other.sample_size_)
, marker_size_(other.marker_size_)
, finished(other.finished)
{}

Source::Source(Source && other)
: sample_(std::move(other.sample_))
, marker_(std::move(other.marker_))
, sample_size_(other.sample_size_)
, marker_size_(other.marker_size_)
, finished(other.finished)
{}

Source::~Source()
{
	this->sample_.clear();
	this->marker_.clear();
}

Source & Source::operator = (const Source & other)
{
	if (this != &other)
	{
		this->sample_ = other.sample_;
		this->marker_ = other.marker_;
		this->sample_size_ = other.sample_size_;
		this->marker_size_ = other.marker_size_;
		this->finished = other.finished;
	}
	
	return *this;
}

Source & Source::operator = (Source && other)
{
	if (this != &other)
	{
		this->sample_.swap(other.sample_);
		this->marker_.swap(other.marker_);
		this->sample_size_ = other.sample_size_;
		this->marker_size_ = other.marker_size_;
		this->finished = other.finished;
	}
	
	return *this;
}

void Source::append(Sample && sample)
{
#ifdef DEBUG_SOURCE
	if (this->finished)
	{
		throw std::runtime_error("Appending of markers already completed");
	}
#endif
	
	this->sample_.push_back(std::move(sample));
	this->sample_size_ += 1;
}

void Source::append(Marker && marker)
{
#ifdef DEBUG_SOURCE
	if (this->finished)
	{
		throw std::runtime_error("Appending of markers already completed");
	}
	if (this->sample_size_ != marker.data.size())
	{
		throw std::domain_error("Different sample size detected\n"
								"Sample size at marker: " + std::to_string(marker.data.size()) + "\n"
								"Sample size expected:  " + std::to_string(this->sample_size_));
	}
#endif
	
	// replace unknown chromosome with first known
	if (this->chromosome.is_unknown() && !marker.info.chr.is_unknown())
	{
		this->chromosome = marker.info.chr;
	}
	
	// match chromosome
	if (! this->chromosome.match(marker.info.chr))
	{
		throw std::invalid_argument("Marker has different chromosome\n"
									"Expected chromosome: " + std::to_string((int)this->chromosome) + "\n"
									"Detected chromosome: " + std::to_string((int)marker.info.chr) + " "
									"(at position '" + std::to_string(marker.info.pos) + "')");
	}
	
	// append data
	for (size_t i = 0; i < this->sample_size_; ++i)
	{
		this->sample_[i].data.append(marker.data[i]);
	}
	
	// remove marker data
	marker.data.remove();
	
	// append marker
	this->marker_.push_back(std::move(marker)); // move
	this->marker_size_ += 1;
}

const Sample & Source::sample(const size_t i) const
{
#ifdef DEBUG_SOURCE
	if (i >= this->sample_size_)
	{
		throw std::out_of_range("Sample out of range\n"
								"Sample requested at index '" + std::to_string(i) + "'\n"
								"Range maximum is at index '" + std::to_string(this->sample_size_ - 1));
	}
#endif
	
	return this->sample_[i];
}

const Marker & Source::marker(const size_t i) const
{
#ifdef DEBUG_SOURCE
	if (i >= this->marker_size_)
	{
		throw std::out_of_range("Marker out of range\n"
								"Marker requested at index '" + std::to_string(i) + "'\n"
								"Range maximum is at index '" + std::to_string(this->marker_size_ - 1));
	}
#endif
	
	return this->marker_[i];
}

const std::vector<Sample> & Source::sample() const
{
#ifdef DEBUG_SOURCE
	if (! this->finished)
	{
		throw std::runtime_error("Appending of samples not completed");
	}
#endif
	
	return this->sample_;
}

const std::vector<Marker> & Source::marker() const
{
#ifdef DEBUG_SOURCE
	if (! this->finished)
	{
		throw std::runtime_error("Appending of markers not completed");
	}
#endif
	
	return this->marker_;
}

size_t Source::sample_size() const
{
	return this->sample_size_;
}

size_t Source::marker_size() const
{
	return this->marker_size_;
}

void Source::finish(const int threads)
{
#ifdef DEBUG_SOURCE
	if (this->finished)
	{
		throw std::runtime_error("Source already finished");
	}
#endif
	
	if (this->marker_size_ == 0)
	{
		throw std::runtime_error("No marker data provided");
	}
	if (this->sample_size_ == 0)
	{
		throw std::runtime_error("No sample data provided");
	}
	
	std::cout << std::endl << "Allocating data" << std::endl;
	ProgressBar progress(this->sample_size_);
	
	// finish sample data
	for (std::vector<Sample>::iterator it = this->sample_.begin(), end = this->sample_.end(); it != end; ++it)
	{
		progress.update();
		
		it->data.finish();
	}
	
	progress.finish();
	
	// sort marker data
	this->sort(threads);
	
	this->finished = true;
}

void Source::sort_subsample(const std::vector<size_t> & subsample, const std::vector<size_t> & order, ProgressBar & progress)
{
	thread_local const size_t n_subsample = subsample.size();
	thread_local const size_t n_order     = order.size();
	thread_local SampleData data;
	
	for (size_t i = 0; i < n_subsample; ++i)
	{
		this->ex_sort.lock();
		progress.update();
		this->ex_sort.unlock();
		
		data = SampleData();
		
		for (size_t k = 0; k < n_order; ++k)
		{
			data.append(this->sample_[subsample[i]].data[ order[k] ]);
		}
		data.finish();
		
		this->sample_[subsample[i]].data = std::move(data);
	}
}

void Source::sort(const int threads)
{
	std::vector< std::pair<size_t, std::vector<Marker>::iterator> > index; // index of current order
	std::vector<size_t> order, check; // index order & check index for expected order
	size_t i = 0;
	
	index.resize(this->marker_size_);
	order.resize(this->marker_size_);
	check.resize(this->marker_size_);
	
	std::iota(check.begin(), check.end(), 0); // fill vector with increasing index
	
	// determine order for index
	for (std::vector<Marker>::iterator it = this->marker_.begin(), end = this->marker_.end(); it != end; ++it)
	{
		index[i] = std::pair<size_t, std::vector<Marker>::iterator>(i, it);
		++i;
	}
	
	// sort index by marker position
	std::sort(index.begin(), index.end(),
			  [] (const std::pair<size_t, std::vector<Marker>::iterator> & a,
				  const std::pair<size_t, std::vector<Marker>::iterator> & b) -> bool
			  { return a.second->info.pos < b.second->info.pos; }
			  );
	
	// determine order
	std::transform(index.begin(), index.end(), order.begin(),
				   [] (const std::pair<size_t, std::vector<Marker>::iterator> & a) -> size_t
				   { return a.first ; }
				   );
	
	// stop if sorting is not necessary
	if (std::equal(order.begin(), order.end(), check.begin()))
		return;
	
	// sort markers
	std::sort(this->marker_.begin(), this->marker_.end());
	
	// sort data in each sample
	std::vector<std::thread> t;
	std::vector< std::vector<size_t> > subsample(threads);
	
	std::cout << std::endl << "Sorting data" << std::endl;
	ProgressBar progress(this->sample_size_);
	
	int k = 0;
	for (i = 0; i < this->sample_size_; ++i)
	{
		subsample[k++].push_back(i);
		
		if (k == threads)
			k = 0;
	}
	
	for (k = 1; k < threads; ++k)
	{
		t.push_back(std::thread(&Source::sort_subsample, this, subsample[k], order, std::ref(progress)));
	}
	
	this->sort_subsample(subsample[0], order, progress);
	
	for (std::thread & _t : t)
	{
		_t.join();
	}
	
	progress.finish();
}



