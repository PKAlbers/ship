//
//  marker.h
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship__marker__
#define __ship__marker__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <utility>

#include "types.h"
#include "census.h"
#include "allele.h"


//******************************************************************************
// Marker containers
//******************************************************************************

//
// Marker data
//
class MarkerData
{
private:
	
	Genotype * data_; // genotype array
	size_t     size_; // full size
	size_t     i;     // increment for appending
	bool contains_unknown_; // flag that data contains unknown haplotypes
	
public:
	
	// return genotype element
	Genotype operator [] (const size_t) const; // return copy
	const Genotype &  at (const size_t) const; // return reference
	
	// check if array is completely filled
	bool is_complete() const;
	
	// check if any haplotype is unknown
	bool contains_unknown() const;
	
	// return size
	size_t size() const;
	
	// return current
	size_t count() const;
	
	// append element
	bool append(const Genotype &);
	
	// remove all data
	void remove();
	
	// assign
	MarkerData & operator = (const MarkerData &);
	MarkerData & operator = (MarkerData &&);
	
	// construct
	MarkerData(const size_t); // default
	MarkerData(const MarkerData &); // copy
	MarkerData(MarkerData &&); // move

	// destruct
	~MarkerData();
};


//
// Marker information
//
struct MarkerInfo
{
	Chromosome  chr; // chromosome
	size_t      pos; // position
	std::string key; // identifier
	AlleleList  allele; // alleles
	
	// compare/sort
	bool operator <  (const MarkerInfo & other) const;
	bool operator >  (const MarkerInfo & other) const;
	bool operator == (const MarkerInfo & other) const;
	bool operator != (const MarkerInfo & other) const;
	
	// print to stream
	void print(std::ostream &, const char = '\0') const;
	void print(FILE *, const char = '\0') const;
	
	// convert to string
	std::string str() const;
	
	// constant header for printing
	static const std::string header;
	
	// assign
	MarkerInfo & operator = (const MarkerInfo &);
	MarkerInfo & operator = (MarkerInfo &&);
	
	// construct
	MarkerInfo(); // default
	MarkerInfo(const MarkerInfo &); // copy
	MarkerInfo(MarkerInfo &&); // move
	
	// destruct
	~MarkerInfo();
};


//
// Marker statistics
//

template <class>
class MarkerSelect;

class MarkerStat
{
private:
	
	bool evaluated; // flag that stats were calculated
	std::unordered_map<Haplotype, Census> census_h; // expected haplotypes (alleles)
	std::unordered_map<Genotype,  Census> census_g; // expected genotypes
	std::vector<Haplotype> index_h; // order of haplotypes
	std::vector<Genotype>  index_g; // order of genotypes
	
public:
	
	// evaluate allele list and genotype data
	void evaluate(const MarkerInfo &, const MarkerData &);
	
	// return census statistics
	const Census & operator [] (const Haplotype &) const;
	const Census & operator [] (const Genotype &)  const;
	
	// print to stream
	void print(std::ostream &, const char = '\0') const;
	void print(FILE *, const char = '\0') const;
	
	// convert to string
	std::string str() const;
	
	// constant header for printing
	static const std::string header;
	
	// assign
	MarkerStat & operator = (const MarkerStat &);
	MarkerStat & operator = (MarkerStat &&);
	
	// construct
	MarkerStat(); // default
	MarkerStat(const MarkerStat &); // copy
	MarkerStat(MarkerStat &&); // move
	
	// destruct
	~MarkerStat();
	
	friend class MarkerSelect<Haplotype>;
	friend class MarkerSelect<Genotype>;
};

template <class TYPE>
class MarkerSelect
{
private:
	
	const MarkerStat * stat;
	
public:
	
	std::vector< typename std::unordered_map<TYPE, Census>::const_iterator > operator <  (const Census &) const;
	std::vector< typename std::unordered_map<TYPE, Census>::const_iterator > operator >  (const Census &) const;
	std::vector< typename std::unordered_map<TYPE, Census>::const_iterator > operator <= (const Census &) const;
	std::vector< typename std::unordered_map<TYPE, Census>::const_iterator > operator >= (const Census &) const;
	
	MarkerSelect(const MarkerStat &);
};

template <class TYPE>
MarkerSelect<TYPE>::MarkerSelect(const MarkerStat & _stat)
: stat(&_stat)
{
#ifdef DEBUG_MARKER
	if (!stat->evaluated)
	{
		throw std::logic_error("Marker statistics not calculated");
	}
#endif
}

template<> std::vector< std::unordered_map<Haplotype, Census>::const_iterator > MarkerSelect<Haplotype>::operator < (const Census & census) const
{
#ifdef DEBUG_MARKER
	if (!stat->evaluated)
	{
		throw std::logic_error("Marker statistics not calculated");
	}
#endif
	
	std::vector< std::unordered_map<Haplotype, Census>::const_iterator > selected;
	
	for (std::unordered_map<Haplotype, Census>::const_iterator it = stat->census_h.cbegin(), end = stat->census_h.cend(); it != end; ++it)
	{
		if (it->second < census)
		{
			selected.push_back(it);
		}
	}
	
	return selected;
}

template<> std::vector< std::unordered_map<Haplotype, Census>::const_iterator > MarkerSelect<Haplotype>::operator > (const Census & census) const
{
#ifdef DEBUG_MARKER
	if (!stat->evaluated)
	{
		throw std::logic_error("Marker statistics not calculated");
	}
#endif
	
	std::vector< std::unordered_map<Haplotype, Census>::const_iterator > selected;
	
	for (std::unordered_map<Haplotype, Census>::const_iterator it = stat->census_h.cbegin(), end = stat->census_h.cend(); it != end; ++it)
	{
		if (it->second > census)
		{
			selected.push_back(it);
		}
	}
	
	return selected;
}

template<> std::vector< std::unordered_map<Haplotype, Census>::const_iterator > MarkerSelect<Haplotype>::operator <= (const Census & census) const
{
#ifdef DEBUG_MARKER
	if (!stat->evaluated)
	{
		throw std::logic_error("Marker statistics not calculated");
	}
#endif
	
	std::vector< std::unordered_map<Haplotype, Census>::const_iterator > selected;
	
	for (std::unordered_map<Haplotype, Census>::const_iterator it = stat->census_h.cbegin(), end = stat->census_h.cend(); it != end; ++it)
	{
		if (it->second <= census)
		{
			selected.push_back(it);
		}
	}
	
	return selected;
}

template<> std::vector< std::unordered_map<Haplotype, Census>::const_iterator > MarkerSelect<Haplotype>::operator >= (const Census & census) const
{
#ifdef DEBUG_MARKER
	if (!stat->evaluated)
	{
		throw std::logic_error("Marker statistics not calculated");
	}
#endif
	
	std::vector< std::unordered_map<Haplotype, Census>::const_iterator > selected;
	
	for (std::unordered_map<Haplotype, Census>::const_iterator it = stat->census_h.cbegin(), end = stat->census_h.cend(); it != end; ++it)
	{
		if (it->second >= census)
		{
			selected.push_back(it);
		}
	}
	
	return selected;
}

template<> std::vector< std::unordered_map<Genotype, Census>::const_iterator > MarkerSelect<Genotype>::operator < (const Census & census) const
{
#ifdef DEBUG_MARKER
	if (!stat->evaluated)
	{
		throw std::logic_error("Marker statistics not calculated");
	}
#endif
	
	std::vector< std::unordered_map<Genotype, Census>::const_iterator > selected;
	
	for (std::unordered_map<Genotype, Census>::const_iterator it = stat->census_g.cbegin(), end = stat->census_g.cend(); it != end; ++it)
	{
		if (it->second < census)
		{
			selected.push_back(it);
		}
	}
	
	return selected;
}

template<> std::vector< std::unordered_map<Genotype, Census>::const_iterator > MarkerSelect<Genotype>::operator > (const Census & census) const
{
#ifdef DEBUG_MARKER
	if (!stat->evaluated)
	{
		throw std::logic_error("Marker statistics not calculated");
	}
#endif
	
	std::vector< std::unordered_map<Genotype, Census>::const_iterator > selected;
	
	for (std::unordered_map<Genotype, Census>::const_iterator it = stat->census_g.cbegin(), end = stat->census_g.cend(); it != end; ++it)
	{
		if (it->second > census)
		{
			selected.push_back(it);
		}
	}
	
	return selected;
}

template<> std::vector< std::unordered_map<Genotype, Census>::const_iterator > MarkerSelect<Genotype>::operator <= (const Census & census) const
{
#ifdef DEBUG_MARKER
	if (!stat->evaluated)
	{
		throw std::logic_error("Marker statistics not calculated");
	}
#endif
	
	std::vector< std::unordered_map<Genotype, Census>::const_iterator > selected;
	
	for (std::unordered_map<Genotype, Census>::const_iterator it = stat->census_g.cbegin(), end = stat->census_g.cend(); it != end; ++it)
	{
		if (it->second <= census)
		{
			selected.push_back(it);
		}
	}
	
	return selected;
}

template<> std::vector< std::unordered_map<Genotype, Census>::const_iterator > MarkerSelect<Genotype>::operator >= (const Census & census) const
{
#ifdef DEBUG_MARKER
	if (!stat->evaluated)
	{
		throw std::logic_error("Marker statistics not calculated");
	}
#endif
	
	std::vector< std::unordered_map<Genotype, Census>::const_iterator > selected;
	
	for (std::unordered_map<Genotype, Census>::const_iterator it = stat->census_g.cbegin(), end = stat->census_g.cend(); it != end; ++it)
	{
		if (it->second >= census)
		{
			selected.push_back(it);
		}
	}
	
	return selected;
}



//
// Marker in genetic map
//
struct MarkerGmap
{
	double rate;   // recombination rate (cM/Mb)
	double dist;   // genetic distance (cM)
	char   source; // from (m) genetic map, (i) interpolated, or (e) extrapolated
	
	// source types
	static const char mapped;
	static const char interpolated;
	static const char extrapolated;
	static const char unknown;
	
	// print element
	void print(std::ostream &, const char = NULL) const;
	void print(FILE *, const char = NULL) const;
	
	// convert to string
	std::string str() const;
	
	// constant header for printing
	static const std::string header;
	
	// assign
	MarkerGmap & operator = (const MarkerGmap &);
	MarkerGmap & operator = (MarkerGmap &&);
	
	// construct
	MarkerGmap(); // default
	MarkerGmap(const MarkerGmap &); // copy
	MarkerGmap(MarkerGmap &&); // move
	
	// destruct
	//~MarkerGmap();
};


//
// Genetic marker
//
struct Marker
{
	MarkerInfo info;
	MarkerStat stat;
	MarkerGmap gmap;
	MarkerData data;
	
	// compare/sort
	bool operator <  (const Marker & other) const;
	bool operator >  (const Marker & other) const;
	bool operator == (const Marker & other) const;
	bool operator != (const Marker & other) const;
	
	// assign
	Marker & operator = (const Marker &);
	Marker & operator = (Marker &&);
	
	// construct
	Marker(const size_t); // default
	Marker(const Marker &); // copy
	Marker(Marker &&); // move
	
	// destruct
	//~Marker();
};



#endif /* defined(__ship__marker__) */
