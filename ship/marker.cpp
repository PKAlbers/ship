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
, cleared(false)
{}

MarkerData::MarkerData(const MarkerData & other)
: data_(new Genotype[other.size_])
, size_(other.size_)
, i(other.i)
, contains_unknown_(other.contains_unknown_)
, cleared(other.cleared)
{
	std::copy(other.data_, other.data_ + other.size_, this->data_);
}

MarkerData::MarkerData(MarkerData && other)
: data_(nullptr)
, size_(other.size_)
, i(other.i)
, contains_unknown_(other.contains_unknown_)
, cleared(other.cleared)
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
		if (this->data_ != nullptr)
			delete [] this->data_;
		
		this->data_ = new Genotype[other.size_];
		std::copy(other.data_, other.data_ + other.size_, this->data_);
		
		this->size_ = other.size_;
		this->i = other.i;
		this->contains_unknown_ = other.contains_unknown_;
		this->cleared = other.cleared;
	}
	return *this;
}

MarkerData & MarkerData::operator = (MarkerData && other)
{
	if (this != &other)
	{
		if (this->data_ != nullptr)
			delete [] this->data_;
			
		this->data_ = other.data_;
		other.data_ = nullptr;
		
		this->size_ = other.size_;
		this->i = other.i;
		this->contains_unknown_ = other.contains_unknown_;
		this->cleared = other.cleared;
	}
	return *this;
}

bool MarkerData::append(const Genotype & g)
{
#ifdef DEBUG_MARKER
	if (this->cleared)
	{
		throw std::runtime_error("Marker data was cleared");
	}
#endif
	
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

void MarkerData::clear()
{
#ifdef DEBUG_MARKER
	if (this->cleared)
	{
		throw std::runtime_error("Marker data was cleared");
	}
#endif
	
	if (this->data_ != nullptr)
		delete [] this->data_;
	
	this->data_ = nullptr;
	this->size_ = 0;
	this->i = 0;
	this->contains_unknown_ = false;
	this->cleared = true;
}

size_t MarkerData::size() const
{
#ifdef DEBUG_MARKER
	if (this->cleared)
	{
		throw std::runtime_error("Marker data was cleared");
	}
#endif
	return this->size_;
}

size_t MarkerData::count() const
{
#ifdef DEBUG_MARKER
	if (this->cleared)
	{
		throw std::runtime_error("Marker data was cleared");
	}
#endif
	return this->i;
}

bool MarkerData::is_complete() const
{
#ifdef DEBUG_MARKER
	if (this->cleared)
	{
		throw std::runtime_error("Marker data was cleared");
	}
#endif
	return (this->i == this->size_);
}

bool MarkerData::contains_unknown() const
{
#ifdef DEBUG_MARKER
	if (this->cleared)
	{
		throw std::runtime_error("Marker data was cleared");
	}
#endif
	return this->contains_unknown_;
}

Genotype MarkerData::operator [] (const size_t _i) const
{
#ifdef DEBUG_MARKER
	if (this->cleared)
	{
		throw std::runtime_error("Marker data was cleared");
	}
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
	if (this->cleared)
	{
		throw std::runtime_error("Marker data was cleared");
	}
	if (_i >= this->i)
	{
		throw std::out_of_range("Marker data out of range for index: " + std::to_string(_i));
	}
#endif
	return this->data_[_i];
}

void MarkerData::print(std::ostream & stream, const char last) const
{
#ifdef DEBUG_MARKER
	if (this->cleared)
	{
		throw std::runtime_error("Marker data was cleared");
	}
#endif
	
	char sep = NULL;
	
	for (size_t i = 0; i < this->size_; ++i)
	{
		const Genotype g = this->data_[i];
		stream << sep << (int)g.h0 << ' ' << (int)g.h1;
		sep = ' ';
	}
	
	if (last != '\0')
		stream << last;
}

void MarkerData::print(FILE * fp, const char last) const
{
#ifdef DEBUG_MARKER
	if (this->cleared)
	{
		throw std::runtime_error("Marker data was cleared");
	}
#endif
	
	char sep = NULL;
	
	for (size_t i = 0; i < this->size_; ++i)
	{
		const Genotype g = (Genotype)this->data_[i];
		fprintf(fp, "%c%d %d", sep, (int)g.h0, (int)g.h1);
		sep = ' ';
	}
	
	if (last != '\0')
		fprintf(fp, "%c", last);
}

std::string MarkerData::str() const
{
	std::ostringstream oss;
	this->print(oss);
	return oss.str();
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
, haplostat(other.haplostat)
, genostat(other.genostat)
{}

MarkerStat::MarkerStat(MarkerStat && other)
: evaluated(other.evaluated)
, haplostat(std::move(other.haplostat))
, genostat(std::move(other.genostat))
{}

MarkerStat::~MarkerStat()
{
	this->haplostat.clear();
	this->genostat.clear();
}

MarkerStat & MarkerStat::operator = (const MarkerStat & other)
{
	if (this != &other)
	{
		this->evaluated = other.evaluated;
		this->haplostat = other.haplostat;
		this->genostat = other.genostat;
	}
	return *this;
}

MarkerStat & MarkerStat::operator = (MarkerStat && other)
{
	this->evaluated = other.evaluated;
	this->haplostat = std::move(other.haplostat);
	this->genostat = std::move(other.genostat);
	return *this;
}

bool MarkerStat::evaluate(const MarkerInfo & info, const MarkerData & data)
{
#ifdef DEBUG_MARKER
	if (this->evaluated)
	{
		throw std::logic_error("Marker statistics already calculated");
	}
#endif
	
	const size_t size = data.size();
	
	size_t table_h[ HAPLOTYPE_MAX + 2 ] = { 0 }; // haplotype counts
	size_t table_g[ HAPLOTYPE_MAX + 2 ][ HAPLOTYPE_MAX + 2 ] = { 0 }; // genotype counts
	
	// walkabout data
	for (size_t i = 0; i < size; ++i)
	{
		const Genotype g = data[i];
		
		// check if haplotypes are defined in allele list
		if ((!g.h0.is_unknown() && !info.allele( (int)g.h0 )) ||
			(!g.h1.is_unknown() && !info.allele( (int)g.h1 )) )
		{
			return false;
		}
		
		// count haplotypes
		++table_h[ ((g.h0.is_unknown()) ? Haplotype::unknown: (int)g.h0) ];
		++table_h[ ((g.h1.is_unknown()) ? Haplotype::unknown: (int)g.h1) ];
		
		// count genotype (in sorted order)
		const Haplotype hX = std::min(g.h0, g.h1);
		const Haplotype hY = std::max(g.h0, g.h1);
		++table_g[ ((hX.is_unknown()) ? Haplotype::unknown: (int)hX) ][ ((hY.is_unknown()) ? Haplotype::unknown: (int)hY) ];
	}
	
	
	// insert expected haplotypes
	for (int i = 0; i < info.allele.size(); ++i)
	{
		this->haplostat[ Haplotype(i) ] = Census();
	}
	
	// insert counted haplotypes
	for (int i = 0; i < HAPLOTYPE_MAX + 2; ++i)
	{
		if (table_h[i] != 0)
		{
			this->haplostat[ Haplotype(i) ] = table_h[i];
		}
	}
	
	// insert expected genotypes
	for (int i0 = 0; i0 < info.allele.size(); ++i0)
	{
		const Haplotype h0(i0);
		
		for (int i1 = i0; i1 < info.allele.size(); ++i1)
		{
			const Haplotype h1(i1);
			
			this->genostat[ Genotype(h0, h1) ] = Census();
		}
	}
	
	// insert counted genotypes
	for (int i0 = 0; i0 < HAPLOTYPE_MAX + 2; ++i0)
	{
		const Haplotype h0(i0);
		
		for (int i1 = i0; i1 < HAPLOTYPE_MAX + 2; ++i1)
		{
			if (table_g[i0][i1] != 0)
			{
				const Haplotype h1(i1);
			
				this->genostat[ Genotype(h0, h1) ] = table_g[i0][i1];
			}
		}
	}
	
	// scale counts to get frequencies
	for (std::map<Haplotype, Census>::iterator it = this->haplostat.begin(), end = this->haplostat.end(); it != end; ++it)
	{
		it->second.scale(size * 2); // two haplotypes per genotype
	}
	for (std::map<Genotype, Census>::iterator it = this->genostat.begin(), end = this->genostat.end(); it != end; ++it)
	{
		it->second.scale(size);
	}
	
	this->evaluated = true;
	
	return true;
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
	
	const Haplotype hX = std::min(g.h0, g.h1);
	const Haplotype hY = std::max(g.h0, g.h1);
	
	try
	{
		return this->genostat.at( Genotype(hX, hY) );
	}
	catch (std::out_of_range &)
	{
		throw std::out_of_range("Genotype (" + std::to_string((int)g.h0) + ", " + std::to_string((int)g.h1) + ") out of range");
	}
	return this->genostat.at( Genotype(hX, hY) ); // satisfy compiler
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
	for (std::map<Haplotype, Census>::const_iterator it = this->haplostat.cbegin(), end = this->haplostat.cend(); it != end; ++it)
	{
		stream << sep << (int)it->first << ':' << (size_t)it->second;
		sep = ',';
	}
	stream << " ";
	
	// allele_freq
	sep = NULL;
	for (std::map<Haplotype, Census>::const_iterator it = this->haplostat.cbegin(), end = this->haplostat.cend(); it != end; ++it)
	{
		stream << sep << (int)it->first << ':' << std::setprecision(8) << (double)it->second;
		sep = ',';
	}
	stream << " ";
	
	// genotype_count
	sep = NULL;
	for (std::map<Genotype, Census>::const_iterator it = this->genostat.cbegin(), end = this->genostat.cend(); it != end; ++it)
	{
		stream << sep << (int)it->first.h0 << '/' << (int)it->first.h1 << ':' << (size_t)it->second;
		sep = ',';
	}
	stream << " ";
	
	// genotype_freq
	sep = NULL;
	for (std::map<Genotype, Census>::const_iterator it = this->genostat.cbegin(), end = this->genostat.cend(); it != end; ++it)
	{
		stream << sep << (int)it->first.h0 << '/' << (int)it->first.h1 << ':' << std::setprecision(8) << (double)it->second;
		sep = ',';
	}
	
	if (last != '\0')
		stream << last;
}

void MarkerStat::print(FILE * fp, const char last) const
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
	for (std::map<Haplotype, Census>::const_iterator it = this->haplostat.cbegin(), end = this->haplostat.cend(); it != end; ++it)
	{
		fprintf(fp, "%c%c:%lu", sep, (int)it->first, (size_t)it->second);
		sep = ',';
	}
	fprintf(fp, " ");
	
	// allele_freq
	sep = NULL;
	for (std::map<Haplotype, Census>::const_iterator it = this->haplostat.cbegin(), end = this->haplostat.cend(); it != end; ++it)
	{
		fprintf(fp, "%c%c:%0.8f", sep, (int)it->first, (double)it->second);
		sep = ',';
	}
	fprintf(fp, " ");
	
	// genotype_count
	sep = NULL;
	for (std::map<Genotype, Census>::const_iterator it = this->genostat.cbegin(), end = this->genostat.cend(); it != end; ++it)
	{
		fprintf(fp, "%c%c/%c:%lu", sep, (int)it->first.h0,  (int)it->first.h1, (size_t)it->second);
		sep = ',';
	}
	fprintf(fp, " ");
	
	// genotype_freq
	sep = NULL;
	for (std::map<Genotype, Census>::const_iterator it = this->genostat.cbegin(), end = this->genostat.cend(); it != end; ++it)
	{
		fprintf(fp, "%c%c/%c:%0.8f", sep, (int)it->first.h0,  (int)it->first.h1, (double)it->second);
		sep = ',';
	}
	
	if (last != '\0')
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
	if (this != &other)
	{
		this->rate   = other.rate;
		this->dist   = other.dist;
		this->source = other.source;
	}
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
	if (this != &other)
	{
		this->info = std::move(other.info);
		this->stat = std::move(other.stat);
		this->gmap = std::move(other.gmap);
		this->data = std::move(other.data);
	}
	return *this;
}

bool Marker::operator <  (const Marker & other) const { return (this->info <  other.info); }
bool Marker::operator >  (const Marker & other) const { return (this->info >  other.info); }
bool Marker::operator == (const Marker & other) const { return (this->info == other.info); }
bool Marker::operator != (const Marker & other) const { return (this->info != other.info); }


