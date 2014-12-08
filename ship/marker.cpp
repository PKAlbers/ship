//
//  marker.cpp
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "marker.h"


#define DEBUG_MARKER


//******************************************************************************
// Marker containers
//******************************************************************************

//
// Marker data
//


MarkerData::MarkerData(const size_t _size)
: data_(new Genotype[_size])
, size_(_size)
, i(0)
, contains_unknown_(false)
{}

MarkerData::MarkerData(const MarkerData & other)
: data_(new Genotype[other.size_])
, size_(other.size_)
, i(other.i)
, contains_unknown_(other.contains_unknown_)
{
	std::copy(other.data_, other.data_ + other.size_, this->data_);
}

MarkerData::MarkerData(MarkerData && other)
: data_(nullptr)
, size_(other.size_)
, i(other.i)
, contains_unknown_(other.contains_unknown_)
{
	this->data_ = other.data_;
	other.data_ = nullptr;
}

MarkerData::~MarkerData()
{
	if (this->data_ != nullptr)
		delete [] this->data_;
}

MarkerData & MarkerData::operator = (const MarkerData & other)
{
	if (this != &other)
	{
		delete [] this->data_;
		
		this->data_ = new Genotype[other.size_];
		std::copy(other.data_, other.data_ + other.size_, this->data_);
		
		this->size_ = other.size_;
		this->i = other.i;
		this->contains_unknown_ = other.contains_unknown_;
	}
	return *this;
}

MarkerData & MarkerData::operator = (MarkerData && other)
{
	if (this != &other)
	{
		delete [] this->data_;
			
		this->data_ = other.data_;
		other.data_ = nullptr;
		
		this->size_ = other.size_;
		this->i = other.i;
		this->contains_unknown_ = other.contains_unknown_;
	}
	return *this;
}

bool MarkerData::append(const Genotype & g)
{
	if (this->i < this->size_)
	{
		if (g.h0.is_unknown() || g.h1.is_unknown())
		{
			this->contains_unknown_ = true;
		}
		
		this->data_[this->i++] = g; // append
		
		return true;
	}
	
	return false;
}

void MarkerData::remove()
{
	if (this->data_ != nullptr)
		delete [] this->data_;
	
	this->data_ = nullptr;
	this->size_ = 0;
	this->i = 0;
	this->contains_unknown_ = false;
}

size_t MarkerData::size() const
{
	return this->size_;
}

size_t MarkerData::count() const
{
	return this->i;
}

bool MarkerData::is_complete() const
{
	return (this->i == this->size_);
}

bool MarkerData::contains_unknown() const
{
	return this->contains_unknown_;
}

Genotype MarkerData::operator [] (const size_t _i) const
{
#ifdef DEBUG_MARKER
	if (_i >= this->i)
	{
		throw std::out_of_range("Marker data out of range for index: " + std::to_string(_i));
	}
#endif
	return this->data_[_i];
}

const Genotype & MarkerData::at(const size_t _i) const
{
#ifdef DEBUG_MARKER
	if (_i >= this->i)
	{
		throw std::out_of_range("Marker data out of range for index: " + std::to_string(_i));
	}
#endif
	return this->data_[_i];
}


//
// Marker information
//

MarkerInfo::MarkerInfo()
: pos(0)
{}

MarkerInfo::MarkerInfo(const MarkerInfo & other)
: chr(other.chr)
, pos(other.pos)
, key(other.key)
, allele(other.allele)
{}

MarkerInfo::MarkerInfo(MarkerInfo && other)
: chr(other.chr)
, pos(other.pos)
, key(std::move(other.key))
, allele(std::move(other.allele))
{}

MarkerInfo::~MarkerInfo()
{
	this->key.clear();
}

MarkerInfo & MarkerInfo::operator = (const MarkerInfo & other)
{
	if (this != &other)
	{
        this->chr = other.chr;
        this->pos = other.pos;
        this->key = other.key;
        this->allele = other.allele;
	}
	return *this;
}

MarkerInfo & MarkerInfo::operator = (MarkerInfo && other)
{
	if (this != &other)
	{
        this->chr = other.chr;
        this->pos = other.pos;
        this->key = std::move(other.key);
        this->allele = std::move(other.allele);
	}
	return *this;
}

bool MarkerInfo::operator <  (const MarkerInfo & other) const { return (this->pos <  other.pos); }
bool MarkerInfo::operator >  (const MarkerInfo & other) const { return (this->pos >  other.pos); }
bool MarkerInfo::operator == (const MarkerInfo & other) const { return (this->pos == other.pos); }
bool MarkerInfo::operator != (const MarkerInfo & other) const { return (this->pos != other.pos); }

void MarkerInfo::print(std::ostream & stream, const char last) const
{
	stream << (int)this->chr << ' ' << this->pos << ' ' << this->key << ' ' << this->allele.size() << ' ';
    this->allele.print(stream, last);
}

void MarkerInfo::print(FILE * fp, const char last) const
{
	fprintf(fp, "%d %lu %s %du ", (int)this->chr, this->pos, this->key.c_str(), this->allele.size());
	this->allele.print(fp, last);
}

std::string MarkerInfo::str() const
{
	std::ostringstream oss;
	this->print(oss);
	return oss.str();
}

const std::string MarkerInfo::header = "chromosome position marker_id n_alleles alleles";


//
// Marker statistics
//

MarkerStat::MarkerStat()
: evaluated(false)
{}

MarkerStat::MarkerStat(const MarkerStat & other)
: evaluated(other.evaluated)
, stat_h(other.stat_h)
, stat_g(other.stat_g)
, index_h(other.index_h)
, index_g(other.index_g)
{}

MarkerStat::MarkerStat(MarkerStat && other)
: evaluated(other.evaluated)
, stat_h(std::move(other.stat_h))
, stat_g(std::move(other.stat_g))
, index_h(std::move(other.index_h))
, index_g(std::move(other.index_g))
{}

MarkerStat::~MarkerStat()
{
	this->stat_h.clear();
	this->stat_g.clear();
	this->index_h.clear();
	this->index_g.clear();
}

MarkerStat & MarkerStat::operator = (const MarkerStat & other)
{
	if (this != &other)
	{
		this->evaluated = other.evaluated;
		this->stat_h = other.stat_h;
		this->stat_g = other.stat_g;
		this->index_h = other.index_h;
		this->index_g = other.index_g;
	}
	return *this;
}

MarkerStat & MarkerStat::operator = (MarkerStat && other)
{
	this->evaluated = other.evaluated;
	this->stat_h = std::move(other.stat_h);
	this->stat_g = std::move(other.stat_g);
	this->index_h = std::move(other.index_h);
	this->index_g = std::move(other.index_g);
	return *this;
}

void MarkerStat::evaluate(const MarkerInfo & info, const MarkerData & data)
{
#ifdef DEBUG_MARKER
	if (this->evaluated)
	{
		throw std::logic_error("Marker statistics already calculated");
	}
#endif
	const size_t size = data.size();
	
	// fill expected haplotype stats
	for (int i = 0; i < info.allele.size(); ++i)
	{
		this->haplostat[ Haplotype(i) ] = Census();
	}
	
	// add missing haplotype
	this->haplostat[ Haplotype() ] = Census();
	
	// fill expected genotypes (missing already included)
	for (std::unordered_map<Haplotype, Census>::const_iterator it0 = this->haplostat.cbegin(), end = this->haplostat.cend(); it0 != end; ++it0)
	{
		Haplotype h0 = it0->first;
		
		for (std::unordered_map<Haplotype, Census>::const_iterator it1 = it0; it1 != end; ++it1)
		{
			Haplotype h1 = it1->first;
			
			this->genostat[ Genotype(h0, h1) ] = Census();
		}
	}
	
	// walkabout data
	for (unsigned long i = 0; i < size; ++i)
	{
		Genotype g = data[i];
		
		// count haplotypes and genotype
		try
		{
			++this->haplostat.at(g.h0);
		}
		catch (std::out_of_range &)
		{
			throw std::out_of_range("Haplotype '" + std::string(1, static_cast<char>(g.h0)) + "' not defined in marker information");
		}
		
		try
		{
			++this->haplostat.at(g.h1);
		}
		catch (std::out_of_range &)
		{
			throw std::out_of_range("Haplotype '" + std::string(1, static_cast<char>(g.h1)) + "' not defined in marker information");
		}
		
		// count genotype
		g = Genotype(std::min(g.h0, g.h1), std::max(g.h0, g.h1)); // re-order genotype
		++this->genostat.at(g);
	}
	
	// scale counts to get frequencies
	for (std::unordered_map<Haplotype, Census>::iterator it = this->haplostat.begin(), end = this->haplostat.end(); it != end; ++it)
	{
		it->second.scale(size * 2); // two haplotypes per genotype
	}
	for (std::unordered_map<Genotype, Census>::iterator it = this->genostat.begin(), end = this->genostat.end(); it != end; ++it)
	{
		it->second.scale(size);
	}
}

const Census & MarkerStat::operator [] (const Haplotype & h) const
{
#ifdef DEBUG_MARKER
	if (!this->evaluated)
	{
		throw std::logic_error("Marker statistics not calculated");
	}
#endif
	
	try
	{
		return this->haplostat.at(h);
	}
	catch (std::out_of_range &)
	{
		throw std::out_of_range("Haplotype '" + std::to_string((int)h) + "' out of range");
	}
	return this->haplostat.at(h); // satisfy compiler
}

const Census & MarkerStat::operator [] (const Genotype & g) const
{
#ifdef DEBUG_MARKER
	if (!this->evaluated)
	{
		throw std::logic_error("Marker statistics not calculated");
	}
#endif
	
	try
	{
		const Genotype G(std::min(g.h0, g.h1), std::max(g.h0, g.h1));
		return this->genostat.at(G);
	}
	catch (std::out_of_range &)
	{
		throw std::out_of_range("Genotype (" + std::to_string((int)g.h0) + ", " + std::to_string((int)g.h1) + ") out of range");
	}
	return this->genostat.at(g); // satisfy compiler
}

void MarkerStat::print(std::ostream & stream, const char last) const
{
#ifdef DEBUG_MARKER
	if (!this->evaluated)
	{
		throw std::logic_error("Marker statistics not calculated");
	}
#endif
	
	char sep;
	
	// allele_count
	sep = NULL;
	for (std::unordered_map<Haplotype, Census>::const_iterator it = this->haplostat.cbegin(), end = this->haplostat.cend(); it != end; ++it)
	{
		stream << sep << (int)it->first << ':' << (size_t)it->second;
		sep = ',';
	}
	stream << " ";
	
	// allele_freq
	sep = NULL;
	for (std::unordered_map<Haplotype, Census>::const_iterator it = this->haplostat.cbegin(), end = this->haplostat.cend(); it != end; ++it)
	{
		stream << sep << static_cast<char>(it->first) << ':' << std::setprecision(8) << static_cast<double>(it->second);
		sep = ',';
	}
	stream << " ";
	
	// genotype_count
	sep = NULL;
	for (std::unordered_map<Genotype, Census>::const_iterator it = this->genostat.cbegin(), end = this->genostat.cend(); it != end; ++it)
	{
		stream << sep << static_cast<char>(it->first.h0) << ((it->first.phased) ? '|': '/') << static_cast<char>(it->first.h1) << ':' << static_cast<unsigned long>(it->second);
		sep = ',';
	}
	stream << " ";
	
	// genotype_freq
	sep = NULL;
	for (std::unordered_map<Genotype, Census>::const_iterator it = this->genostat.cbegin(), end = this->genostat.cend(); it != end; ++it)
	{
		stream << sep << static_cast<char>(it->first.h0) << ((it->first.phased) ? '|': '/') << static_cast<char>(it->first.h1) << ':' << std::setprecision(8) << static_cast<double>(it->second);
		sep = ',';
	}
	stream << last;
}

void MarkerStat::print(FILE * fp, const char last) const
{
	if (!this->good)
		throw std::logic_error("Marker statistics not calculated");
	
	char sep;
	
	// allele_count
	sep = NULL;
	for (std::map<Haplotype, Census>::const_iterator it = this->haplostat.cbegin(), end = this->haplostat.cend(); it != end; ++it)
	{
		fprintf(fp, "%c%c:%lu", sep, static_cast<char>(it->first), static_cast<unsigned long>(it->second));
		sep = ',';
	}
	fprintf(fp, " ");
	
	// allele_freq
	sep = NULL;
	for (std::map<Haplotype, Census>::const_iterator it = this->haplostat.cbegin(), end = this->haplostat.cend(); it != end; ++it)
	{
		fprintf(fp, "%c%c:%0.8f", sep, static_cast<char>(it->first), static_cast<double>(it->second));
		sep = ',';
	}
	fprintf(fp, " ");
	
	// genotype_count
	sep = NULL;
	for (std::map<Genotype, Census>::const_iterator it = this->genostat.cbegin(), end = this->genostat.cend(); it != end; ++it)
	{
		fprintf(fp, "%c%c%c%c:%lu", sep, static_cast<char>(it->first.h0), ((it->first.phased) ? '|': '/'), static_cast<char>(it->first.h1), static_cast<unsigned long>(it->second));
		sep = ',';
	}
	fprintf(fp, " ");
	
	// genotype_freq
	sep = NULL;
	for (std::map<Genotype, Census>::const_iterator it = this->genostat.cbegin(), end = this->genostat.cend(); it != end; ++it)
	{
		fprintf(fp, "%c%c%c%c:%0.8f", sep, static_cast<char>(it->first.h0), ((it->first.phased) ? '|': '/'), static_cast<char>(it->first.h1), static_cast<double>(it->second));
		sep = ',';
	}
	fprintf(fp, "%c", last);
}

std::string MarkerStat::str() const
{
	std::ostringstream oss;
	this->print(oss);
	return oss.str();
}

const std::string MarkerStat::header = "allele_count allele_freq genotype_count genotype_freq";


//
// Marker in genetic map
//

MarkerGmap::MarkerGmap()
: rate(0)
, dist(0)
, source(MarkerGmap::unknown)
{}

MarkerGmap::MarkerGmap(const MarkerGmap & other)
: rate(other.rate)
, dist(other.rate)
, source(other.source)
{}

MarkerGmap::MarkerGmap(MarkerGmap && other)
: rate(other.rate)
, dist(other.rate)
, source(other.source)
{}

//MarkerGmap::~MarkerGmap()
//{}

MarkerGmap & MarkerGmap::operator = (const MarkerGmap & other)
{
	if (this != &other)
	{
		this->rate   = other.rate;
		this->dist   = other.dist;
		this->source = other.source;
	}
	return *this;
}

MarkerGmap & MarkerGmap::operator = (MarkerGmap && other)
{
	this->rate   = other.rate;
	this->dist   = other.dist;
	this->source = other.source;
	return *this;
}

void MarkerGmap::print(std::ostream & stream, const char last) const
{
	stream << std::setprecision(8) << this->rate << ' ' << std::setprecision(8) << this->dist << ' ' << this->source << last;
}

void MarkerGmap::print(FILE * fp, const char last) const
{
	fprintf(fp, "%0.8f %0.8f %c%c", this->rate, this->dist, this->source, last);
}

std::string MarkerGmap::str() const
{
	std::ostringstream oss;
	this->print(oss);
	return oss.str();
}

const char MarkerGmap::mapped       = 'm';
const char MarkerGmap::interpolated = 'i';
const char MarkerGmap::extrapolated = 'e';
const char MarkerGmap::unknown      = '.';
const std::string MarkerGmap::header = "map_rate map_dist map_source";


//
// Genetic marker
//

Marker::Marker(const unsigned long size)
: data(size)
{}

Marker::Marker(const Marker & other)
: info(other.info)
, stat(other.stat)
, gmap(other.gmap)
, data(other.data)
{}

Marker::Marker(Marker && other)
: info(std::move(other.info))
, stat(std::move(other.stat))
, gmap(std::move(other.gmap))
, data(std::move(other.data))
{}

//Marker::~Marker()
//{}

Marker & Marker::operator = (const Marker & other)
{
	if (this != &other)
	{
		this->info = other.info;
		this->stat = other.stat;
		this->gmap = other.gmap;
		this->data = other.data;
	}
	return *this;
}

Marker & Marker::operator = (Marker && other)
{
	this->info = std::move(other.info);
	this->stat = std::move(other.stat);
	this->gmap = std::move(other.gmap);
	this->data = std::move(other.data);
	return *this;
}

bool Marker::operator <  (const Marker & other) const { return (this->info <  other.info); }
bool Marker::operator >  (const Marker & other) const { return (this->info >  other.info); }
bool Marker::operator == (const Marker & other) const { return (this->info == other.info); }
bool Marker::operator != (const Marker & other) const { return (this->info != other.info); }


