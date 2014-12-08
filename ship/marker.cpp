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

unsigned long MarkerData::size() const
{
	return this->size_;
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
: n_alleles_(0)
, contains_snp_(false)
, contains_indel_(false)
, contains_other_(false)
, pos(0)
, id0(".")
, id1(".")
{}

MarkerInfo::MarkerInfo(const MarkerInfo & other)
: alleles(other.alleles)
, n_alleles_(other.n_alleles_)
, contains_snp_(other.contains_snp_)
, contains_indel_(other.contains_indel_)
, contains_other_(other.contains_other_)
, chr(other.chr)
, pos(other.pos)
, id0(other.id0)
, id1(other.id1)
{}

MarkerInfo::MarkerInfo(MarkerInfo && other)
: alleles(std::move(other.alleles))
, n_alleles_(other.n_alleles_)
, contains_snp_(other.contains_snp_)
, contains_indel_(other.contains_indel_)
, contains_other_(other.contains_other_)
, chr(other.chr)
, pos(other.pos)
, id0(std::move(other.id0))
, id1(std::move(other.id1))
{}

MarkerInfo::~MarkerInfo()
{
	this->alleles.clear();
	this->id0.clear();
	this->id1.clear();
}

MarkerInfo & MarkerInfo::operator = (const MarkerInfo & other)
{
	if (this != &other)
	{
		this->alleles = other.alleles;
		this->n_alleles_ = other.n_alleles_;
		this->contains_snp_   = other.contains_snp_;
		this->contains_indel_ = other.contains_indel_;
		this->contains_other_ = other.contains_other_;
		this->chr = other.chr;
		this->pos = other.pos;
		this->id0 = other.id0;
		this->id1 = other.id1;
	}
	return *this;
}

MarkerInfo & MarkerInfo::operator = (MarkerInfo && other)
{
	if (this != &other)
	{
		this->alleles = std::move(other.alleles);
		this->n_alleles_ = other.n_alleles_;
		this->contains_snp_   = other.contains_snp_;
		this->contains_indel_ = other.contains_indel_;
		this->contains_other_ = other.contains_other_;
		this->chr = other.chr;
		this->pos = other.pos;
		this->id0 = std::move(other.id0);
		this->id1 = std::move(other.id1);
	}
	return *this;
}

bool MarkerInfo::operator <  (const MarkerInfo & other) const { return (this->pos <  other.pos); }
bool MarkerInfo::operator >  (const MarkerInfo & other) const { return (this->pos >  other.pos); }
bool MarkerInfo::operator == (const MarkerInfo & other) const { return (this->pos == other.pos); }
bool MarkerInfo::operator != (const MarkerInfo & other) const { return (this->pos != other.pos); }

void MarkerInfo::allele(const Allele & allele)
{
	this->alleles.insert(std::make_pair(Haplotype(static_cast<int>(this->n_alleles_)), allele));
	this->n_alleles_ += 1;
	
	switch (allele.type())
	{
		case Allele::Type::SNP:   this->contains_snp_   = true; break;
		case Allele::Type::INDEL: this->contains_indel_ = true; break;
		case Allele::Type::OTHER: this->contains_other_ = true; break;
	}
}

void MarkerInfo::allele(Allele && allele)
{
	this->alleles.insert(std::make_pair(Haplotype(static_cast<int>(this->n_alleles_)), std::move(allele)));
	this->n_alleles_ += 1;
	
	switch (allele.type())
	{
		case Allele::Type::SNP:   this->contains_snp_   = true; break;
		case Allele::Type::INDEL: this->contains_indel_ = true; break;
		case Allele::Type::OTHER: this->contains_other_ = true; break;
	}
}

const Allele & MarkerInfo::allele(const Haplotype h) const
{
	try
	{
		return this->alleles.at(h);
	}
	catch (std::out_of_range & oor)
	{
		throw std::logic_error("Haplotype '" + std::string(1, static_cast<char>(h)) + "' does not refer to an allele");
	}
	return this->alleles.at(h); // satisfy compiler
}

unsigned int MarkerInfo::n_alleles() const
{
	return this->n_alleles_;
}

bool MarkerInfo::contains_snp() const
{
	return this->contains_snp_;
}

bool MarkerInfo::contains_indel() const
{
	return this->contains_indel_;
}

bool MarkerInfo::contains_other() const
{
	return this->contains_other_;
}

void MarkerInfo::print(std::ostream & stream, const char last) const
{
	stream << this->chr.str() << ' ' << this->pos << ' ' << this->id0 << ' ' << this->id1 << ' ' << this->n_alleles_ << ' ';
	
	if (this->n_alleles_ == 0)
	{
		stream << "." << last;
		return;
	}
	
	char sep = NULL;
	for (std::map<Haplotype, Allele>::const_iterator it = this->alleles.begin(), end = this->alleles.end(); it != end; ++it)
	{
		stream << sep << it->second.base();
		sep = ',';
	}
	
	stream << last;
}

void MarkerInfo::print(FILE * fp, const char last) const
{
	fprintf(fp, "%s %lu %s %s %du ", this->chr.str().c_str(), this->pos, this->id0.c_str(), this->id1.c_str(), this->n_alleles_);
	
	if (this->n_alleles_ == 0)
	{
		fprintf(fp, ".");
		return;
	}
	
	char sep = NULL;
	for (std::map<Haplotype, Allele>::const_iterator it = this->alleles.begin(), end = this->alleles.end(); it != end; ++it)
	{
		fprintf(fp, "%c%s", sep, it->second.base().c_str());
		sep = ',';
	}
	
	fprintf(fp, "%c", last);
}

std::string MarkerInfo::str() const
{
	std::ostringstream oss;
	this->print(oss);
	return oss.str();
}

const std::string MarkerInfo::header = "chromosome position marker_id rs_id n_alleles alleles";


//
// Marker statistics
//

MarkerStat::MarkerStat()
: good(false)
{}

MarkerStat::MarkerStat(const MarkerStat & other)
: good(other.good)
, haplostat(other.haplostat)
, genostat(other.genostat)
{}

MarkerStat::MarkerStat(MarkerStat && other)
: good(other.good)
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
		this->good = other.good;
		this->haplostat = other.haplostat;
		this->genostat  = other.genostat;
	}
	return *this;
}

MarkerStat & MarkerStat::operator = (MarkerStat && other)
{
	this->good = other.good;
	this->haplostat = std::move(other.haplostat);
	this->genostat  = std::move(other.genostat);
	return *this;
}

void MarkerStat::eval(const MarkerInfo & info, const MarkerData & data)
{
	if (this->good)
		throw std::logic_error("Marker statistics already calculated");
	
	const unsigned long size = data.size();
	
	// fill expected haplotype stats
	for (int h = 0; h < info.n_alleles(); ++h)
	{
		this->haplostat[ Haplotype(h) ] = Census();
	}
	
	// add missing haplotype
	this->haplostat[ Haplotype() ] = Census();
	
	// fill expected genotypes (missing already included)
	for (std::map<Haplotype, Census>::const_iterator it0 = this->haplostat.cbegin(), end = this->haplostat.cend(); it0 != end; ++it0)
	{
		Haplotype h0 = it0->first;
		
		for (std::map<Haplotype, Census>::const_iterator it1 = it0; it1 != end; ++it1)
		{
			Haplotype h1 = it1->first;
			
			this->genostat[ Genotype(h0, h1, false) ] = Census();
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
		g = Genotype(std::min(g.h0, g.h1), std::max(g.h0, g.h1), false); // re-order genotype
		++this->genostat.at(g);
	}
	
	// scale counts to get frequencies
	for (std::map<Haplotype, Census>::iterator it = this->haplostat.begin(), end = this->haplostat.end(); it != end; ++it)
	{
		it->second.scale(size * 2); // two haplotypes per data element
	}
	for (std::map<Genotype, Census>::iterator it = this->genostat.begin(), end = this->genostat.end(); it != end; ++it)
	{
		it->second.scale(size);
	}
}

const Census & MarkerStat::operator [] (const Haplotype h) const
{
	if (!this->good)
		throw std::logic_error("Marker statistics not calculated");
	
	try
	{
		return this->haplostat.at(h);
	}
	catch (std::out_of_range & oor)
	{
		throw std::string("Haplotype '" + std::string(1, static_cast<char>(h)) + "' out of range");
	}
	return this->haplostat.at(h); // satisfy compiler
}

const Census & MarkerStat::operator [] (const Genotype g) const
{
	if (!this->good)
		throw std::logic_error("Marker statistics not calculated");
	
	try
	{
		return this->genostat.at(g);
	}
	catch (std::out_of_range & oor)
	{
		throw std::string("Genotype (" + std::string(1, static_cast<char>(g.h0)) + ", " + std::string(1, static_cast<char>(g.h1)) + ") out of range");
	}
	return this->genostat.at(g); // satisfy compiler
}

void MarkerStat::print(std::ostream & stream, const char last) const
{
	if (!this->good)
		throw std::logic_error("Marker statistics not calculated");
	
	char sep;
	
	// allele_count
	sep = NULL;
	for (std::map<Haplotype, Census>::const_iterator it = this->haplostat.cbegin(), end = this->haplostat.cend(); it != end; ++it)
	{
		stream << sep << static_cast<char>(it->first) << ':' << static_cast<unsigned long>(it->second);
		sep = ',';
	}
	stream << " ";
	
	// allele_freq
	sep = NULL;
	for (std::map<Haplotype, Census>::const_iterator it = this->haplostat.cbegin(), end = this->haplostat.cend(); it != end; ++it)
	{
		stream << sep << static_cast<char>(it->first) << ':' << std::setprecision(8) << static_cast<double>(it->second);
		sep = ',';
	}
	stream << " ";
	
	// genotype_count
	sep = NULL;
	for (std::map<Genotype, Census>::const_iterator it = this->genostat.cbegin(), end = this->genostat.cend(); it != end; ++it)
	{
		stream << sep << static_cast<char>(it->first.h0) << ((it->first.phased) ? '|': '/') << static_cast<char>(it->first.h1) << ':' << static_cast<unsigned long>(it->second);
		sep = ',';
	}
	stream << " ";
	
	// genotype_freq
	sep = NULL;
	for (std::map<Genotype, Census>::const_iterator it = this->genostat.cbegin(), end = this->genostat.cend(); it != end; ++it)
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


