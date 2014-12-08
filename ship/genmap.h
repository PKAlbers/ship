//
//  genmap.h
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship__genmap__
#define __ship__genmap__

#include <stdio.h>
#include <map>

#include "types.h"
#include "marker.h"


//******************************************************************************
// Genetic map container
//******************************************************************************

//
// Genetic map site information
//
struct GenmapInfo
{
	Chromosome    chr;    // chromosome
	unsigned long pos;    // position
	double        rate;   // recombination rate (cM/Mb)
	double        dist;   // genetic distance (cM)
	
	// compare/sort
	bool operator <  (const GenmapInfo &) const;
	bool operator >  (const GenmapInfo &) const;
	bool operator == (const GenmapInfo &) const;
	bool operator != (const GenmapInfo &) const;
	
	// assign
	GenmapInfo & operator = (const GenmapInfo &);
	GenmapInfo & operator = (GenmapInfo &&);
	
	// construct
	GenmapInfo();
	GenmapInfo(const GenmapInfo &);
	GenmapInfo(GenmapInfo &&);
	
//	// destruct
//	~GenmapInfo();
};


//
// Genetic map container
//
class Genmap
{
private:
	
	std::map<unsigned long, GenmapInfo> genmap; // genetic map, by position
	double median_rate_; // median recombination rate (cM/Mb)
	double median_dist_; // median distance (cM) between sites
	bool good; // flag that genmap is complete
	
	unsigned long size_; // number of mapped positions in cargo
	
	// calculate median of rates
	void median_rate();
	
	// calculate median genetic distance
	void median_dist();
	
public:
	
	// insert new mapped position
	void insert(const GenmapInfo &);
	void insert(GenmapInfo &&);
	
	// finish loading of mapped positions
	void finish();
	
	// approximate for marker
	MarkerGmap approx(const MarkerInfo &) const;
	
	// return size
	unsigned long size() const;
	
	// assign
	Genmap & operator = (const Genmap &);
	Genmap & operator = (Genmap &&);
	
	// construct
	Genmap();
	Genmap(const Genmap &);
	Genmap(Genmap &&);
	
	// destruct
	~Genmap();
};



#endif /* defined(__ship__genmap__) */
