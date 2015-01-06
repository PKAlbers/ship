//
//  input.h
//  ship
//
//  Created by Patrick Albers on 13.12.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship__input__
#define __ship__input__

#include <stdio.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <deque>
#include <algorithm>
#include <stdexcept>

#include "timer.h"
#include "stream.h"
#include "census.h"
#include "sample.h"
#include "marker.h"
#include "genmap.h"
#include "source.h"


//******************************************************************************
// Input handling
//******************************************************************************

//
// Filter input before sourcing
//
class FilterInput
{
private:
	
	class FilterMarkerInfo
	{
	private:
		
		bool any;
		bool remove_if_contains_snp_;
		bool remove_if_contains_indel_;
		bool remove_if_contains_other_;
		bool remove_if_biallelic_;
		bool remove_if_multiallelic_;
		bool remove_pos_;
		
		std::unordered_set<size_t> remove_pos_set;
		
	public:
		
		// set filters
		void remove_if_contains_snp();
		void remove_if_contains_indel();
		void remove_if_contains_other();
		void remove_if_biallelic();
		void remove_if_multiallelic();
		void remove_pos(const std::string &);
		
		FilterMarkerInfo();
		friend class FilterInput;
	};
	
	class FilterMarkerData
	{
	private:
		
		bool any;
		bool remove_if_contains_unknown_;
		
	public:
		
		// set filters
		void remove_if_contains_unknown();
		
		FilterMarkerData();
		friend class FilterInput;
	};
	
	class FilterMarkerStat
	{
	private:
		
		bool any;
		bool remove_hap_below_;
		bool remove_hap_above_;
		bool remove_gen_below_;
		bool remove_gen_above_;
		
		Cutoff remove_hap_below_cutoff; // equal or lower
		Cutoff remove_hap_above_cutoff; // equal or greater
		Cutoff remove_gen_below_cutoff; // equal or lower
		Cutoff remove_gen_above_cutoff; // equal or greater
		
	public:
		
		// set filters
		void remove_hap_below(const std::string &, const size_t);
		void remove_hap_above(const std::string &, const size_t);
		void remove_gen_below(const std::string &, const size_t);
		void remove_gen_above(const std::string &, const size_t);
		
		FilterMarkerStat();
		friend class FilterInput;
	};
	
	class FilterMarkerGmap
	{
	private:
		
		bool any;
		bool remove_if_source_interpolated_;
		bool remove_if_source_extrapolated_;
		bool remove_if_source_unknown_;
		
	public:
		
		// set filters
		void remove_if_source_interpolated();
		void remove_if_source_extrapolated();
		void remove_if_source_unknown();
		
		FilterMarkerGmap();
		friend class FilterInput;
	};
	
	class FilterSampleInfo
	{
	private:
		
		bool any;
		bool remove_key_;
		
		std::unordered_set<std::string> remove_key_set;
		
	public:
		
		// set filters
		void remove_key(const std::string &);
		
		FilterSampleInfo();
		friend class FilterInput;
	};
	
public:
	
	FilterMarkerInfo markerinfo;
	FilterMarkerData markerdata;
	FilterMarkerStat markerstat;
	FilterMarkerGmap markergmap;
	FilterSampleInfo sampleinfo;
	
	std::string comment; // leave explanation when declinded
	
	// apply filters
	bool apply(const MarkerInfo &);
	bool apply(const MarkerData &);
	bool apply(const MarkerStat &);
	bool apply(const MarkerGmap &);
	bool apply(const SampleInfo &);
};


//
// Load input and other files
//
class Input_VCF
{
private:
	
	StreamLine line; // input file stream
	size_t size; // detected sample size
	
	bool sample_; // flag that sample file was provided (optional)
	bool genmap_; // flag that genetic map file was provided (optional)
	
	std::vector<SampleInfo> _sample; // sample information
	Genmap                  _genmap; // genetic map container
	
	void log(const std::string &); // log warning message
	std::string error(const std::string &); // error message when throwing
	
public:
	
	FilterInput filter;
	
	void sample(const std::string &);
	void genmap(const std::string &);
	
	void run(Source &);
	
	Input_VCF(const std::string &);
};



#endif /* defined(__ship__input__) */
