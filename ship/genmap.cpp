//
//  genmap.cpp
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "genmap.h"


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

//GenmapInfo::~GenmapInfo()
//{}

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
, good(false)
, size_(0)
{}

Genmap::Genmap(const Genmap & other)
: genmap(other.genmap)
, median_rate_(other.median_rate_)
, median_dist_(other.median_dist_)
, good(other.good)
, size_(other.size_)
{}

Genmap::Genmap(Genmap && other)
: genmap(std::move(other.genmap))
, median_rate_(other.median_rate_)
, median_dist_(other.median_dist_)
, good(other.good)
, size_(other.size_)
{}

Genmap & Genmap::operator = (const Genmap & other)
{
	if (this != &other)
	{
		this->genmap = other.genmap;
		this->median_rate_ = other.median_rate_;
		this->median_dist_ = other.median_dist_;
		this->good = other.good;
		this->size_ = other.size_;
	}
	return *this;
}

Genmap & Genmap::operator = (Genmap && other)
{
	if (this != &other)
	{
		this->genmap = std::move(other.genmap);
		this->median_rate_ = other.median_rate_;
		this->median_dist_ = other.median_dist_;
		this->good = other.good;
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
	const unsigned long n = this->genmap.size();
	
	if (n == 0)
	{
		this->median_rate_ = 0;
		return;
	}
	
	std::vector<double> r;
	r.reserve(n);
	
	// collect rates
	for (std::map<unsigned long, GenmapInfo>::const_iterator it = this->genmap.begin(), end = this->genmap.end(); it != end; ++it)
	{
		r.push_back(it->second.rate);
	}
	
	std::sort(r.begin(), r.end()); // sort rates
	
	this->median_rate_ = (n % 2 == 0) ? (r[ (n / 2) - 1 ] + r[ n / 2 ]) / 2: r[ n / 2 ];
}

void Genmap::median_dist()
{
	const unsigned long n = this->genmap.size();
	
	if (n == 0)
	{
		this->median_dist_ = 0;
		return;
	}
	
	std::vector<double> d;
	d.reserve(n);
	
	std::map<unsigned long, GenmapInfo>::const_iterator prev, curr, end = this->genmap.end();
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
	
	this->median_dist_ = (n % 2 == 0) ? (d[ (n / 2) - 1 ] + d[ n / 2 ]) / 2: d[ n / 2 ];
}

void Genmap::insert(const GenmapInfo & info)
{
	if (this->good)
	{
		throw std::logic_error("Loading of genetic map already completed");
		return;
	}
	
	// insert if first
	if (this->genmap.size() == 0)
	{
		this->genmap[ info.pos ] = info;
		this->size_ += 1;
		return;
	}
	
	// check duplicate
	if (this->genmap.count(info.pos) != 0)
	{
		throw std::invalid_argument("Duplicate position in genetic map");
		return;
	}
	
	std::map<unsigned long, GenmapInfo>::const_iterator next = this->genmap.upper_bound(info.pos);
	
	// check upper bound distance
	if (next != this->genmap.end())
	{
		if (next->second.dist < info.dist)
		{
			throw std::invalid_argument("Genetic distance in genetic map is lower at following physical position");
			return;
		}
		
		// check lower bound distance
		if (next != this->genmap.begin())
		{
			std::map<unsigned long, GenmapInfo>::const_iterator prev = --next;
			
			if (prev->second.dist > info.dist)
			{
				throw std::invalid_argument("Genetic distance in genetic map is larger at previous physical position");
				return;
			}
		}
	}
	
	this->genmap[ info.pos ] = info;
	this->size_ += 1;
}

void Genmap::insert(GenmapInfo && info)
{
	if (this->good)
	{
		throw std::logic_error("Loading of genetic map already completed");
		return;
	}
	
	// insert if first
	if (this->genmap.size() == 0)
	{
		this->genmap[ info.pos ] = std::move(info);
		this->size_ += 1;
		return;
	}
	
	// check duplicate
	if (this->genmap.count(info.pos) != 0)
	{
		throw std::invalid_argument("Duplicate position in genetic map");
		return;
	}
	
	std::map<unsigned long, GenmapInfo>::const_iterator next = this->genmap.upper_bound(info.pos);
	
	// check upper bound distance
	if (next != this->genmap.end())
	{
		if (next->second.dist < info.dist)
		{
			throw std::invalid_argument("Genetic distance in genetic map is lower at following physical position");
			return;
		}
		
		// check lower bound distance
		if (next != this->genmap.begin())
		{
			std::map<unsigned long, GenmapInfo>::const_iterator prev = --next;
			
			if (prev->second.dist > info.dist)
			{
				throw std::invalid_argument("Genetic distance in genetic map is larger at previous physical position");
				return;
			}
		}
	}
	
	this->genmap[ info.pos ] = std::move(info);
	this->size_ += 1;
}

void Genmap::finish()
{
	if (this->good)
		throw std::logic_error("Loading of genetic map already completed");
	
	this->median_rate();
	this->median_dist();
	this->good = true;
}

MarkerGmap Genmap::approx(const MarkerInfo & marker) const
{
	if (! this->good)
		throw std::logic_error("Loading of genetic map not finished");
	
	MarkerGmap gmap; // return type
	
	// return empty, if no mapped positions were provided
	if (this->genmap.size() == 0)
	{
		return gmap;
	}
	
	// return without approximating, if present
	std::map<unsigned long, GenmapInfo>::const_iterator match = this->genmap.find(marker.pos);
	if (match != this->genmap.end())
	{
		gmap.rate = match->second.rate;
		gmap.dist = match->second.dist;
		gmap.source = MarkerGmap::mapped;
		return gmap;
	}
	
	// extrapolation
	std::map<unsigned long, GenmapInfo>::const_iterator upper = this->genmap.upper_bound(marker.pos);
	
	// extrapolate, after end
	if (upper == this->genmap.end())
	{
		gmap.rate = this->median_rate_;
		gmap.dist = this->median_dist_ * (marker.pos - std::prev(upper)->second.pos);
		gmap.source = MarkerGmap::extrapolated;
		return gmap;
	}
	
	// extrapolate, before begin
	if (upper == this->genmap.begin())
	{
		gmap.rate = this->median_rate_;
		gmap.dist = this->median_dist_ * (upper->second.pos - marker.pos);
		gmap.source = MarkerGmap::extrapolated;
		return gmap;
	}
	
	// linear interpolation
	std::map<unsigned long, GenmapInfo>::const_iterator lower;
	lower = std::prev(upper);
	
	double inter_pos = static_cast<double>(marker.pos);
	double lower_pos = static_cast<double>(lower->second.pos);
	double upper_pos = static_cast<double>(upper->second.pos);
	double f = ((inter_pos - lower_pos) / (upper_pos - lower_pos));
	
	gmap.rate = (lower->second.rate * (upper->second.rate - lower->second.rate)) / f;
	gmap.dist = (lower->second.dist * (upper->second.dist - lower->second.dist)) / f;
	gmap.source = MarkerGmap::interpolated;
	
	return gmap;
}

unsigned long Genmap::size() const
{
	return this->size_;
}


