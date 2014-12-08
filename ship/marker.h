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
public:
//private:
	
	Genotype * data_; // genotype array
	size_t     size_; // full size
	size_t     i;     // increment for appending
	bool contains_unknown_; // flag that data contains unknown haplotypes
	
//public:
	
	// return genotype element
	Genotype operator [] (const size_t) const; // return copy
	const Genotype &  at (const size_t) const; // return reference
	
	// check if array is completely filled
	bool is_complete() const;
	
	// check if any haplotype is unknown
	bool contains_unknown() const;
	
	// return size
	size_t size() const;
	
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
	void print(std::ostream &, const char = NULL) const;
	void print(FILE *, const char = NULL) const;
	
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
	
	bool good; // flag that stats were calculated
	
	
	struct HaplotypeCount
	{
		std::vector<const Haplotype> haplotype;
		std::vector<unsigned long> count;
		
		bool insert(const Haplotype &);
		
		unsigned long operator () (const Haplotype &);
		
		HaplotypeCount & operator ++ ();
	};
	
	struct GenotypeCount
	{
		Genotype genotype;
		unsigned long count;
	};
	
	
public:
	
	std::map<Haplotype, Census> haplostat; // expected haplotypes (alleles)
	std::map<Genotype,  Census> genostat;  // expected genotypes
	
	// evaluate allele list and genotype data
	void eval(const MarkerInfo &, const MarkerData &);
	
	// return census statistics
	const Census & operator [] (const Haplotype) const;
	const Census & operator [] (const Genotype)  const;
	
	// print to stream
	void print(std::ostream &, const char = NULL) const;
	void print(FILE *, const char = NULL) const;
	
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
