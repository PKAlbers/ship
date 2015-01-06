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
#include <map>
#include <sstream>
#include <iomanip>
#include <utility>
#include <stdexcept>

#include "types.hpp"
#include "census.h"
#include "allele.h"


#define DEBUG_MARKER


//******************************************************************************
// Marker containers
//******************************************************************************

//
// Marker data
//
class MarkerData
{
private:
	
	std::vector<Genotype> data; // genotype array
	size_t n; // full size
	size_t i; // increment for appending
	bool contains_unknown_; // flag that data contains unknown haplotypes
	
public:
	
	// return genotype
	const Genotype & operator [] (const size_t) const;
	
	// check if array is completely filled
	bool is_complete() const;
	
	// check if any haplotype is unknown
	bool contains_unknown() const;
	
	// return size/count
	size_t size() const;
	size_t count() const;
	
	// append genotype
	bool append(const Genotype &);
	
	// erase genotype
	bool erase(const size_t);
	
	// remove all data
	void remove();
	
	// print to stream
	void print(std::ostream &, const char = '\0') const;
	void print(FILE *, const char = '\0') const;
	
	// convert to string
	std::string str() const;
	
	// assign
	MarkerData & operator = (const MarkerData &);
	MarkerData & operator = (MarkerData &&);
	
	// construct
	MarkerData(const size_t); // default
	MarkerData(const MarkerData &); // copy
	MarkerData(MarkerData &&); // move
};


//
// Marker information
//
struct MarkerInfo
{
	Chromosome  chr; // chromosome
	size_t      pos; // position
	std::string key; // identifier
	AlleleList  allele; // allele list
	
	// compare/sort, by position
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

class MarkerStat
{
private:
	
	bool evaluated; // flag that stats were calculated
	
public:
	
	CensusList<Haplotype> haplotype; // expected haplotypes (alleles)
	CensusList<Genotype>  genotype;  // expected genotypes
	Census unknown_haplotype; // missing/undefined haplotypes (alleles)
	Census unknown_genotype;  // missing/undefined genotypes
	
	// evaluate allele list and genotype data
	bool evaluate(const MarkerInfo &, const MarkerData &);
	
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
};


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
};



#endif /* defined(__ship__marker__) */
