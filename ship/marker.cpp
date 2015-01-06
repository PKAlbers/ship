//
//  marker.cpp
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "marker.h"


//******************************************************************************
// Marker containers
//******************************************************************************

//
// Marker data
//

MarkerData::MarkerData(const size_t _size)
: data(_size)
, n(_size)
, i(0)
, contains_unknown_(false)
{}

MarkerData::MarkerData(const MarkerData & other)
: data(other.data)
, n(other.n)
, i(other.i)
, contains_unknown_(other.contains_unknown_)
{}

MarkerData::MarkerData(MarkerData && other)
: data(std::move(other.data))
, n(other.n)
, i(other.i)
, contains_unknown_(other.contains_unknown_)
{}

MarkerData & MarkerData::operator = (const MarkerData & other)
{
#ifdef DEBUG_MARKER
	if (this->n != other.n)
	{
		throw std::runtime_error("Marker data was defined with different size");
	}
#endif
	
	if (this != &other)
	{
		this->data = other.data;
		this->n = other.n;
		this->i = other.i;
		this->contains_unknown_ = other.contains_unknown_;
	}
	return *this;
}

MarkerData & MarkerData::operator = (MarkerData && other)
{
#ifdef DEBUG_MARKER
	if (this->n != other.n)
	{
		throw std::runtime_error("Marker data was defined with different size");
	}
#endif
	
	if (this != &other)
	{
		this->data = std::move(other.data);
		this->n = other.n;
		this->i = other.i;
		this->contains_unknown_ = other.contains_unknown_;
	}
	return *this;
}

bool MarkerData::append(const Genotype & g)
{
	if (this->i < this->n)
	{
		if (! this->contains_unknown_)
		{
			if (g.h0.is_unknown() || g.h1.is_unknown())
			{
				this->contains_unknown_ = true;
			}
		}
		
		this->data[this->i++] = g; // append
		
		return true;
	}
	
	return false;
}

bool MarkerData::erase(const size_t _i)
{
	if (_i >= this->i)
	{
		return false;
	}
	
	this->data.erase(this->data.begin() + _i);
	
	--this->n;
	--this->i;
	
	return true;
}

void MarkerData::remove()
{
	std::vector<Genotype> x;
	this->data.swap(x);
	
	this->n = 0;
	this->i = 0;
	this->contains_unknown_ = false;
}

size_t MarkerData::size() const
{
	return this->n;
}

size_t MarkerData::count() const
{
	return this->i;
}

bool MarkerData::is_complete() const
{
	return (this->i == this->n);
}

bool MarkerData::contains_unknown() const
{
	return this->contains_unknown_;
}

const Genotype & MarkerData::operator [] (const size_t _i) const
{
#ifdef DEBUG_MARKER
	if (_i >= this->i)
	{
		throw std::out_of_range("Marker data out of range at index: " + std::to_string(_i));
	}
#endif
	
	return this->data[_i];
}

void MarkerData::print(std::ostream & stream, const char last) const
{
#ifdef DEBUG_MARKER
	if (! this->is_complete())
	{
		throw std::runtime_error("Marker data is not complete");
	}
#endif
	
	stream << (int)this->data[0].h0 << ' ' << (int)this->data[0].h1;
	
	for (size_t k = 1; k < this->n; ++k)
	{
		stream << ' ' << (int)this->data[k].h0 << ' ' << (int)this->data[k].h1;
	}
	
	if (last != '\0')
		stream << last;
}

void MarkerData::print(FILE * fp, const char last) const
{
#ifdef DEBUG_MARKER
	if (! this->is_complete())
	{
		throw std::runtime_error("Marker data is not complete");
	}
#endif
	
	fprintf(fp, "%d %d", (int)this->data[i].h0, (int)this->data[i].h1);
	
	for (size_t k = 1; k < this->n; ++k)
	{
		fprintf(fp, " %d %d", (int)this->data[k].h0, (int)this->data[k].h1);
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
	fprintf(fp, "%d %lu %s %d ", (int)this->chr, this->pos, this->key.c_str(), this->allele.size());
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
, haplotype(other.haplotype)
, genotype(other.genotype)
, unknown_haplotype(other.unknown_haplotype)
, unknown_genotype(other.unknown_genotype)
{}

MarkerStat::MarkerStat(MarkerStat && other)
: evaluated(other.evaluated)
, haplotype(std::move(other.haplotype))
, genotype(std::move(other.genotype))
, unknown_haplotype(std::move(other.unknown_haplotype))
, unknown_genotype(std::move(other.unknown_genotype))
{}

MarkerStat & MarkerStat::operator = (const MarkerStat & other)
{
	if (this != &other)
	{
		this->evaluated = other.evaluated;
		this->haplotype = other.haplotype;
		this->genotype  = other.genotype;
		this->unknown_haplotype = other.unknown_haplotype;
		this->unknown_genotype  = other.unknown_genotype;
	}
	return *this;
}

MarkerStat & MarkerStat::operator = (MarkerStat && other)
{
	if (this != &other)
	{
		this->evaluated = other.evaluated;
		this->haplotype = std::move(other.haplotype);
		this->genotype  = std::move(other.genotype);
		this->unknown_haplotype = std::move(other.unknown_haplotype);
		this->unknown_genotype  = std::move(other.unknown_genotype);
	}
	return *this;
}

bool MarkerStat::evaluate(const MarkerInfo & info, const MarkerData & data)
{
#ifdef DEBUG_MARKER
	if (this->evaluated)
	{
		throw std::logic_error("Marker statistics already calculated");
	}
	if (! data.is_complete())
	{
		throw std::logic_error("Marker data is not complete");
	}
#endif
	
	const size_t size = data.size();
	
	size_t count_h[ HAPLOTYPE_MAX + 1 ] = { 0 }; // haplotype counts
	size_t count_g[ HAPLOTYPE_MAX + 1 ][ HAPLOTYPE_MAX + 1 ] = { 0 }; // genotype counts
	
	Haplotype h0, h1;
	bool flag;
	
	// walkabout data
	for (size_t i = 0; i < size; ++i)
	{
		h0 = data[i].h0;
		h1 = data[i].h1;
		
		flag = true;
		
		// count first haplotype
		if (h0.is_unknown())
		{
			++unknown_haplotype; // missing/undefined allele
			flag = false;
		}
		else
		{
			if (info.allele.exists((int)h0)) // check if allele is defined
				++count_h[ (int)h0 ];
			else
				return false;
		}
		
		// count second haplotype
		if (h1.is_unknown())
		{
			++this->unknown_haplotype; // missing/undefined allele
			flag = false;
		}
		else
		{
			if (info.allele.exists((int)h1)) // check if allele is defined
				++count_h[ (int)h1 ];
			else
				return false;
		}
		
		// count genotype (sorted)
		if (flag)
			++count_g[ (int)std::min(h0, h1) ][ (int)std::min(h0, h1) ];
		else
			++this->unknown_genotype;
	}
	
	
	// insert haplotype count
	for (int h = 0; h < info.allele.size(); ++h)
	{
		this->haplotype.append(h, Census(count_h[h], size * 2));  // scale: two haplotypes per genotype
	}
	
	// insert genotype count
	for (int i0 = 0; i0 < info.allele.size(); ++i0)
	{
		h0 = i0;
		
		for (int i1 = i0; i1 < info.allele.size(); ++i1)
		{
			h1 = i1;
			
			this->genotype.append(Genotype(h0, h1), Census(count_g[h0][h1], size));
		}
	}
	
	this->unknown_haplotype.scale(size * 2);
	this->unknown_genotype.scale(size);
	
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
	
	return this->haplotype[h];
}

const Census & MarkerStat::operator [] (const Genotype & g) const
{
#ifdef DEBUG_MARKER
	if (!this->evaluated)
	{
		throw std::logic_error("Marker statistics not calculated");
	}
#endif
	
	return this->genotype[ Genotype(std::min(g.h0, g.h1), std::max(g.h0, g.h1)) ];
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
	Genotype g;
	
	int i;
	const int h_size = this->haplotype.size();
	const int g_size = this->genotype.size();
	
	// allele_count
	sep = NULL;
	for (i = 0; i < h_size; ++i)
	{
		stream << sep << i << ':' << (size_t)this->haplotype.census(i);
		sep = ',';
	}
	stream << " ";
	
	// allele_freq
	sep = NULL;
	for (i = 0; i < h_size; ++i)
	{
		stream << sep << i << ':' << std::setprecision(6) << (double)this->haplotype.census(i);
		sep = ',';
	}
	stream << " ";
	
	// miss_allele_count
	stream << (size_t)this->unknown_haplotype << " ";
	
	// miss_allele_freq
	stream << std::setprecision(6) << (double)this->unknown_haplotype << " ";
	
	// genotype_count
	sep = NULL;
	for (i = 0; i < g_size; ++i)
	{
		g = this->genotype.type(i);
		stream << sep << g.h0 << '/' << g.h1 << ':' << (size_t)this->genotype.census(i);
		sep = ',';
	}
	stream << " ";
	
	// genotype_freq
	sep = NULL;
	for (i = 0; i < g_size; ++i)
	{
		g = this->genotype.type(i);
		stream << sep << g.h0 << '/' << g.h1 << ':' << std::setprecision(6) << (double)this->genotype.census(i);
		sep = ',';
	}
	stream << " ";
	
	// miss_allele_count
	stream << (size_t)this->unknown_genotype << " ";
	
	// miss_allele_freq
	stream << std::setprecision(6) << (double)this->unknown_genotype;
	
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
	
	Genotype g;
	
	int i;
	const int h_size = this->haplotype.size();
	const int g_size = this->genotype.size();
	
	// allele_count
	fprintf(fp, "0:%lu", (size_t)this->haplotype.census(0));
	for (i = 1; i < h_size; ++i)
		fprintf(fp, ",%d:%lu", i, (size_t)this->haplotype.census(i));
	
	// allele_freq
	fprintf(fp, " 0:%0.6f", (double)this->haplotype.census(0));
	for (i = 1; i < h_size; ++i)
		fprintf(fp, ",%d:%0.6f", i, (double)this->haplotype.census(i));
	
	// miss_allele_count + miss_allele_freq
	fprintf(fp, " %lu %0.6f", (size_t)this->unknown_haplotype, (double)this->unknown_haplotype);
	
	// genotype_count
	g = this->genotype.type(0);
	fprintf(fp, " %d/%d:%lu", (int)g.h0, (int)g.h1, (size_t)this->genotype.census(0));
	for (i = 1; i < g_size; ++i)
	{
		g = this->genotype.type(i);
		fprintf(fp, ",%d/%d:%lu", (int)g.h0, (int)g.h1, (size_t)this->genotype.census(i));
	}
	
	// genotype_freq
	g = this->genotype.type(0);
	fprintf(fp, " %d/%d:%0.6f", (int)g.h0, (int)g.h1, (double)this->genotype.census(0));
	for (i = 1; i < g_size; ++i)
	{
		g = this->genotype.type(i);
		fprintf(fp, ",%d/%d:%0.6f", (int)g.h0, (int)g.h1, (double)this->genotype.census(i));
	}
	
	// miss_genotype_count + miss_genotype_freq
	fprintf(fp, " %lu %0.6f", (size_t)this->unknown_genotype, (double)this->unknown_genotype);
	
	if (last != '\0')
		fprintf(fp, "%c", last);
}

std::string MarkerStat::str() const
{
	std::ostringstream oss;
	this->print(oss);
	return oss.str();
}

const std::string MarkerStat::header = "allele_count allele_freq miss_allele_count miss_allele_freq genotype_count genotype_freq miss_genotype_count miss_genotype_freq";


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
	if (last == '\0')
		stream << std::setprecision(8) << this->rate << ' ' << std::setprecision(6) << this->dist << ' ' << this->source;
	else
		stream << std::setprecision(8) << this->rate << ' ' << std::setprecision(6) << this->dist << ' ' << this->source << last;
}

void MarkerGmap::print(FILE * fp, const char last) const
{
	if (last == '\0')
		fprintf(fp, "%0.6f %0.6f %c", this->rate, this->dist, this->source);
	else
		fprintf(fp, "%0.6f %0.6f %c%c", this->rate, this->dist, this->source, last);
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


