//
//  input.h
//  ship
//
//  Created by Patrick Albers on 21.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship__input__
#define __ship__input__

#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>
#include <set>
#include <deque>
#include <algorithm>
#include <stdexcept>

#include "parse.h"
#include "timer.h"
#include "census.h"
#include "sample.h"
#include "marker.h"
#include "genmap.h"
#include "source.h"


//******************************************************************************
// Input handling
//******************************************************************************

//
// Filter input before putting in source
//
struct FilterInput
{
	struct FilterMarkerInfo
	{
		bool remove_if_contains_indel;
		bool remove_if_contains_other;
		bool remove_if_multiallelic;
		
		FilterMarkerInfo();
	};
	
	struct FilterMarkerData
	{
		bool remove_if_unphased;
		bool remove_if_unknown;
		
		FilterMarkerData();
	};
	
	struct FilterMarkerStat
	{
		bool any;
		bool remove_hap_below_;
		bool remove_hap_above_;
		bool remove_gen_below_;
		bool remove_gen_above_;
		
		Census remove_hap_below; // equal or lower
		Census remove_hap_above; // equal or greater
		Census remove_gen_below; // equal or lower
		Census remove_gen_above; // equal or greater
		
		FilterMarkerStat();
	};
	
	FilterMarkerInfo markerinfo;
	FilterMarkerData markerdata;
	FilterMarkerStat markerstat;
	
	// apply filters
	bool apply(const MarkerInfo &, const unsigned long) const;
	bool apply(const MarkerData &, const unsigned long) const;
	bool apply(const MarkerStat &, const unsigned long) const;
};


//
// Scan input file format
//
class ScanInput : public ReadStream
{
private:
	
	// scan for each defined format
	bool scan_format_vcf();
	bool scan_format_hap();
	bool scan_format_gen();
	
public:
	
	InputFormat format; // detected input format
	unsigned long header_rows; // number of header lines
	unsigned long marker_cols; // number of marker columns
	unsigned long sample_cols; // number of sample columns (not sample size)
	
	// construct
	ScanInput(const std::string &, const InputFormat);
};


//
// Load input files into source
//
class LoadInput
{
private:
	
	// sample cache
	struct SampleCache
	{
		std::vector<SampleInfo> cache; // parsed samples from each line
	};
	
	// legend cache
	struct LegendCache
	{
		std::deque<MarkerInfo> cache; // parsed markers from each line
		std::deque<char> valid; // flag that markers are valid
	};
	
	// genmap cache
	struct GenmapCache
	{
		Genmap cache; // genetic map container
	};
	
	
	ReadInput input; // input file stream
	FilterInput filter; // input filter
	
	SampleCache _sample; // sample container
	LegendCache _legend; // legend container
	GenmapCache _genmap; // genmap container
	
	bool sample_; // flag that sample file was provided
	bool legend_; // flag that legend file was provided
	bool genmap_; // flag that genmap file was provided
	
	unsigned long size; // scanned sample size
	
	// handle missing sample
	std::vector<SampleInfo> get_sample_vcf() const;
	std::vector<SampleInfo> get_sample_hap() const;
	std::vector<SampleInfo> get_sample_gen() const;
	std::vector<SampleInfo> make_sample() const; // generate sample IDs
	
public:
	
	// provide additional files
	void sample(const std::string &);
	void legend(const std::string &);
	void genmap(const std::string &);
	
	// load source
	void load(Source &);
	
	// construct/destruct
	LoadInput(const ScanInput &, const FilterInput &);
	//~LoadInput();
};




#endif /* defined(__ship__input__) */
