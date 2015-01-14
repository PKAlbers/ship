//
//  genmap.cpp
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "genmap.h"

#define DEBUG_GENMAP

//******************************************************************************
// Genetic map container
//******************************************************************************

//
// Genetic map site information
//

GenmapInfo::GenmapInfo()
: pos(0)
, rate(0)
, dist(0)
{}

GenmapInfo::GenmapInfo(const GenmapInfo & other)
: chr(other.chr)
, pos(other.pos)
, rate(other.rate)
, dist(other.dist)
{}

GenmapInfo::GenmapInfo(GenmapInfo && other)
: chr(other.chr)
, pos(other.pos)
, rate(other.rate)
, dist(other.dist)
{}

GenmapInfo & GenmapInfo::operator = (const GenmapInfo & other)
{
	this->chr = other.chr;
	this->pos = other.pos;
	this->rate = other.rate;
	this->dist = other.dist;
	return *this;
}

GenmapInfo & GenmapInfo::operator = (GenmapInfo && other)
{
	this->chr = other.chr;
	this->pos = other.pos;
	this->rate = other.rate;
	this->dist = other.dist;
	return *this;
}

bool GenmapInfo::operator <  (const GenmapInfo & other) const { return (this->pos <  other.pos); }
bool GenmapInfo::operator >  (const GenmapInfo & other) const { return (this->pos >  other.pos); }
bool GenmapInfo::operator == (const GenmapInfo & other) const { return (this->pos == other.pos); }
bool GenmapInfo::operator != (const GenmapInfo & other) const { return (this->pos != other.pos); }


//
// Genetic map container
//

Genmap::Genmap()
: median_rate_(0)
, median_dist_(0)
, finished(false)
, size_(0)
{}

Genmap::Genmap(const Genmap & other)
: chromosome(other.chromosome)
, genmap(other.genmap)
, median_rate_(other.median_rate_)
, median_dist_(other.median_dist_)
, finished(other.finished)
, size_(other.size_)
{}

Genmap::Genmap(Genmap && other)
: chromosome(other.chromosome)
, genmap(std::move(other.genmap))
, median_rate_(other.median_rate_)
, median_dist_(other.median_dist_)
, finished(other.finished)
, size_(other.size_)
{}

Genmap & Genmap::operator = (const Genmap & other)
{
	if (this != &other)
	{
		this->chromosome = other.chromosome;
		this->genmap = other.genmap;
		this->median_rate_ = other.median_rate_;
		this->median_dist_ = other.median_dist_;
		this->finished = other.finished;
		this->size_ = other.size_;
	}
	return *this;
}

Genmap & Genmap::operator = (Genmap && other)
{
	if (this != &other)
	{
		this->chromosome = other.chromosome;
		this->genmap = std::move(other.genmap);
		this->median_rate_ = other.median_rate_;
		this->median_dist_ = other.median_dist_;
		this->finished = other.finished;
		this->size_ = other.size_;
	}
	return *this;
}

Genmap::~Genmap()
{
	this->genmap.clear();
}

void Genmap::median_rate()
{
	const size_t n = this->genmap.size();
	
	if (n == 0)
	{
		this->median_rate_ = 0;
		return;
	}
	
	std::vector<double> r;
	r.reserve(n);
	
	// collect rates
	for (std::map<size_t, GenmapInfo>::const_iterator it = this->genmap.begin(), end = this->genmap.end(); it != end; ++it)
	{
		r.push_back(it->second.rate);
	}
	
	std::sort(r.begin(), r.end()); // sort rates
	
	// calculate median rate
	this->median_rate_ = (n % 2 == 0) ? (r[ (n / 2) - 1 ] + r[ n / 2 ]) / 2: r[ n / 2 ];
}

void Genmap::median_dist()
{
	const size_t n = this->genmap.size();
	
	if (n == 0)
	{
		this->median_dist_ = 0;
		return;
	}
	
	std::vector<double> d;
	d.reserve(n);
	
	std::map<size_t, GenmapInfo>::const_iterator prev, curr, end = this->genmap.end();
	curr = this->genmap.begin();
	prev = curr++;
	
	// calculate distances between positions
	while (curr != end)
	{
		d.push_back((curr->second.dist - prev->second.dist) / static_cast<double>(curr->second.pos - prev->second.pos));
		++prev;
		++curr;
	}
	
	if (d.size() == 0)
	{
		this->median_dist_ = 0;
		return;
	}
	
	std::sort(d.begin(), d.end()); // sort distances
	
	// calculate median distance
	this->median_dist_ = (n % 2 == 0) ? (d[ (n / 2) - 1 ] + d[ n / 2 ]) / 2: d[ n / 2 ];
}

void Genmap::insert(const GenmapInfo & info)
{
#ifdef DEBUG_GENMAP
	if (this->finished)
	{
		throw std::logic_error("Loading of genetic map already completed");
	}
#endif
	
	// insert if first
	if (this->genmap.size() == 0)
	{
		this->chromosome = info.chr;
		this->genmap[ info.pos ] = info;
		this->size_ += 1;
		return;
	}
	
	// replace unknown chromosome with first known
	if (this->chromosome.is_unknown() && !info.chr.is_unknown())
	{
		this->chromosome = info.chr;
	}
	
	// match chromosome
	if (! this->chromosome.match(info.chr))
	{
		throw std::invalid_argument("Genetic map contains multiple chromosomes\n"
									"Expected chromosome: " + std::to_string((int)this->chromosome) + "\n"
									"Detected chromosome: " + std::to_string((int)info.chr) + " "
									"(at position '" + std::to_string(info.pos) + "')");
	}
	
	// check duplicate
	if (this->genmap.count(info.pos) != 0)
	{
		throw std::invalid_argument("Duplicate position '" + std::to_string(info.pos) + "' in genetic map");
	}
	
	std::map<size_t, GenmapInfo>::const_iterator next = this->genmap.upper_bound(info.pos);
	
	// check upper bound distance
	if (next != this->genmap.end())
	{
		if (next->second.dist < info.dist)
		{
			throw std::invalid_argument("Genetic distance at position '" +
										std::to_string(info.pos) +
										"' is lower at following physical position '" +
										std::to_string(next->second.pos) +
										"'");
		}
		
		// check lower bound distance
		if (next != this->genmap.begin())
		{
			std::map<size_t, GenmapInfo>::const_iterator prev = --next;
			
			if (prev->second.dist > info.dist)
			{
				throw std::invalid_argument("Genetic distance at position '" +
											std::to_string(info.pos) +
											"' is greater at previous physical position '" +
											std::to_string(prev->second.pos) +
											"'");
			}
		}
	}
	
	this->genmap[ info.pos ] = info;
	this->size_ += 1;
}

void Genmap::finish()
{
#ifdef DEBUG_GENMAP
	if (this->finished)
	{
		throw std::logic_error("Loading of genetic map already completed");
	}
#endif
	
	this->median_rate();
	this->median_dist();
	this->finished = true;
}

MarkerGmap Genmap::approx(const MarkerInfo & marker) const
{
#ifdef DEBUG_GENMAP
	if (! this->finished)
	{
		throw std::logic_error("Loading of genetic map not completed");
	}
#endif
	
	// check chromosome
	if (! this->chromosome.match(marker.chr))
	{
		throw std::invalid_argument("Genetic map has different chromosome\n"
									"Expected chromosome: " + std::to_string(this->chromosome) +
									"Detected chromosome: " + std::to_string(marker.chr));
	}
	
	MarkerGmap gmap;
	
	// return empty, if no mapped positions were provided
	if (this->size_ == 0)
	{
		return gmap;
	}
	
	// return without approximating, if present
	std::map<size_t, GenmapInfo>::const_iterator match = this->genmap.find(marker.pos);
	if (match != this->genmap.end())
	{
		gmap.rate = match->second.rate;
		gmap.dist = match->second.dist;
		gmap.source = MarkerGmap::mapped;
		return gmap;
	}
	
	// extrapolation
	std::map<size_t, GenmapInfo>::const_iterator upper = this->genmap.upper_bound(marker.pos);
	
	// extrapolate, after end
	if (upper == this->genmap.end())
	{
		gmap.rate = this->median_rate_;
		gmap.dist = this->median_dist_ * (static_cast<double>(marker.pos) - static_cast<double>(std::prev(upper)->second.pos));
		gmap.source = MarkerGmap::extrapolated;
		return gmap;
	}
	
	// extrapolate, before begin
	if (upper == this->genmap.begin())
	{
		gmap.rate = this->median_rate_;
		gmap.dist = this->median_dist_ * (static_cast<double>(marker.pos) - static_cast<double>(upper->second.pos));
		gmap.source = MarkerGmap::extrapolated;
		return gmap;
	}
	
	// linear interpolation
	upper = this->genmap.lower_bound(marker.pos);
	std::map<size_t, GenmapInfo>::const_iterator lower = std::prev(upper);
	
	double inter_pos = static_cast<double>(marker.pos);
	double lower_pos = static_cast<double>(lower->second.pos);
	double upper_pos = static_cast<double>(upper->second.pos);
	double f = ((inter_pos - lower_pos) / (upper_pos - lower_pos));
	
	gmap.rate = lower->second.rate + ((upper->second.rate - lower->second.rate) * f);
	gmap.dist = lower->second.dist + ((upper->second.dist - lower->second.dist) * f);
	gmap.source = MarkerGmap::interpolated;
	
	return gmap;
}

size_t Genmap::size() const
{
	return this->size_;
}


