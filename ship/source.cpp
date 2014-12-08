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
: marker_size_(0)
, sample_size_(0)
, good(false)
{}

Source::Source(const Source & other)
: marker_size_(other.marker_size_)
, sample_size_(other.sample_size_)
, marker_(other.marker_)
, sample_(other.sample_)
, good(other.good)
{}

Source::Source(Source && other)
: marker_size_(other.marker_size_)
, sample_size_(other.sample_size_)
, marker_(std::move(other.marker_))
, sample_(std::move(other.sample_))
, good(other.good)
{}

Source::~Source()
{
	this->marker_.clear();
	this->sample_.clear();
}

Source & Source::operator = (const Source & other)
{
	if (this != &other)
	{
		this->marker_size_ = other.marker_size_;
		this->sample_size_ = other.sample_size_;
		this->marker_ = other.marker_;
		this->sample_ = other.sample_;
		this->good = other.good;
	}
	return *this;
}

Source & Source::operator = (Source && other)
{
	if (this != &other)
	{
		this->marker_size_ = other.marker_size_;
		this->sample_size_ = other.sample_size_;
		this->marker_.swap(other.marker_);
		this->sample_.swap(other.sample_);
		this->good = other.good;
	}
	return *this;
}

void Source::append(Marker && marker)
{
	if (this->good)
	{
		throw std::logic_error("Cannot add marker, because appending was finished");
	}
	
//	// check for duplicates
//	std::vector<Marker>::const_iterator it;
//	it = find_if(this->marker_.cbegin(), this->marker_.cend(),
//				 [& marker] (const Marker & _marker)
//				 { return _marker == marker; }
//				 );
//	if (it != this->marker_.cend())
//	{
//		throw std::invalid_argument("Duplicate marker detected\n"
//						            "Marker information inserted: " + marker.info.str() + "\n"
//						            "Marker information detected: " + it->info.str());
//	}
	
	// append data
	if (this->sample_size_ != marker.data.size())
	{
		throw std::domain_error("Different sample size detected\n"
						        "Sample size at marker: " + std::to_string(marker.data.size()) + "\n"
						        "Sample size expected:  " + std::to_string(this->sample_size_));
	}
	for (unsigned long i = 0; i < this->sample_size_; ++i)
	{
		this->sample_[i].data.append(marker.data[i]);
	}
	
	// remove marker data
	marker.data.remove();
	
	// append marker
	this->marker_.push_back(std::move(marker)); // move
	this->marker_size_ += 1;
}

void Source::append(Sample && sample)
{
	if (this->good)
	{
		throw std::logic_error("Cannot add sample, because appending was finished");
	}
	
//	// check for duplicates
//	std::vector<Sample>::const_iterator it;
//	it = find_if(this->sample_.cbegin(), this->sample_.cend(),
//				 [& sample] (const Sample & _sample)
//				 { return _sample == sample; }
//				 );
//	if (it != this->sample_.cend())
//	{
//		throw std::invalid_argument("Duplicate sample detected\n"
//						            "Sample information inserted: " + sample.info.str() + "\n"
//						            "Sample information detected: " + it->info.str());
//	}
	
	this->sample_.push_back(std::move(sample));
	this->sample_size_ += 1;
}

const Marker & Source::marker(const unsigned long i) const
{
	if (i >= this->marker_size_)
	{
		throw std::out_of_range("Marker out of range\n"
						        "Marker requested at index '" + std::to_string(i) + "'\n"
						        "Range maximum is at index '" + std::to_string(this->marker_size_ - 1));
	}
	return this->marker_[i];
}

const Sample & Source::sample(const unsigned long i) const
{
	if (i >= this->sample_size_)
	{
		throw std::out_of_range("Sample out of range\n"
						        "Sample requested at index '" + std::to_string(i) + "'\n"
						        "Range maximum is at index '" + std::to_string(this->sample_size_ - 1));
	}
	return this->sample_[i];
}

unsigned long Source::marker_size() const { return this->marker_size_; }
unsigned long Source::sample_size() const { return this->sample_size_; }

void Source::finish()
{
	if (this->good)
	{
		throw std::logic_error("Source already finished");
	}
	if (this->marker_size_ == 0)
	{
		throw std::runtime_error("No marker data provided");
	}
	if (this->sample_size_ == 0)
	{
		throw std::runtime_error("No sample data provided");
	}
	
	this->sort();
	
	this->marker_.shrink_to_fit();
	this->sample_.shrink_to_fit();
	
	this->good = true;
}

void Source::sort()
{
	std::vector< std::pair<unsigned long, std::vector<Marker>::iterator> > index; // index of current order
	std::vector<unsigned long> order, check; // index order & check index for expected order
	
	index.resize(this->marker_size_);
	order.resize(this->marker_size_);
	check.resize(this->marker_size_);
	
	std::iota(check.begin(), check.end(), 0); // fill vector with increasing index
	
	// determine order for index
	unsigned long i = 0;
	for (std::vector<Marker>::iterator it = this->marker_.begin(), end = this->marker_.end(); it != end; ++it)
	{
		index[i] = std::pair<unsigned long, std::vector<Marker>::iterator>(i, it);
		i += 1;
	}
	
	// sort index by marker
	std::sort(index.begin(), index.end(),
			  [] (const std::pair<unsigned long, std::vector<Marker>::iterator> & a,
				  const std::pair<unsigned long, std::vector<Marker>::iterator> & b) -> bool
			  { return a.second < b.second; }
			  );
	
	// determine order
	std::transform(index.begin(), index.end(), order.begin(),
				   [] (const std::pair<unsigned long, std::vector<Marker>::iterator> & a) -> unsigned long
				   { return a.first ; }
				   );
	
	// stop if sorting is not necessary
	if (std::equal(order.begin(), order.end(), check.begin()))
		return;
	
	index.clear();
	check.clear();
	
	// sort markers
	std::sort(this->marker_.begin(), this->marker_.end());
	
	// sort data in each sample
	for (std::vector<Sample>::iterator it = this->sample_.begin(), end = this->sample_.end(); it != end; ++it)
	{
		SampleData data;
		
		for (unsigned long i = 0, n = this->marker_size_; i < n; ++i)
		{
			data.append(it->data[ order[i] ]);
		}
		data.finish();
		
		it->data = std::move(data);
	}
}



