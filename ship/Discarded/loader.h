//
//  loader.h
//  ship
//
//  Created by Patrick Albers on 18.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship__loader__
#define __ship__loader__

#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>

#include "print.h"
#include "timer.h"
#include "sample.h"
#include "marker.h"
#include "genmap.h"
#include "inputs.h"
#include "source.h"


//**************************************************************************
// Cargo containers, for temporary holding and moving types
//**************************************************************************

//
// Sample cargo
//
struct CargoSample
{
	std::queue<SampleInfo> cargo; // sample vector
	std::set<std::string> index; // index of sample IDs
	unsigned long size; // number of samples in cargo
	
	// insert new sample
	void insert(const SampleInfo &);
	void insert(SampleInfo &&) noexcept;
	
	// finish loading of samples
	void finish();
	
	// clear cargo after returns
	void clear();
	
	// return sample information
	SampleInfo && operator [] (unsigned long);
};

//
// Legend cargo
//
struct CargoLegend
{
	typedef unsigned long Index;
	typedef unsigned long Position;
	
	std::unordered_map<Index, Marker> cargo; // indexed markers
	std::unordered_map<Position, std::vector<Index> > index; // count markers per position
	
	unsigned long size; // number of markers in cargo
	
	// insert new marker
	void insert(unsigned long, const Marker &);
	void insert(unsigned long, Marker &&) noexcept;
	
	// finish loading of markers
	void finish();
	
	// check leftover markers and clear cargo after returns
	void clear();
	
	// return marker by index
	bool copy(const unsigned long, Marker &); // copy
	bool move(const unsigned long, Marker &); // move
};



//**************************************************************************
// Load data files
//**************************************************************************
class Loader
{
public:
	
	// accepted data file formats
	enum Format
	{
		VCF,
		HAP,
		GEN,
		UNKNOWN
	};
	
private:
	
	std::unique_ptr<InputData>   input_format; // data file of yet unknown format
	std::unique_ptr<InputSample> input_sample; // sample file
	std::unique_ptr<InputLegend> input_legend; // legend file
	std::unique_ptr<InputGenmap> input_genmap; // genmap file
	
	Format format_; // determined format after scanning
	
	bool sample_; // flag that sample file was provided
	bool legend_; // flag that legend file was provided
	bool genmap_; // flag that genmap file was provided
	
	CargoSample sample; // sample file data container
	CargoLegend legend; // legend file data container
	CargoGenmap genmap; // genmap file data container
	
	// scan file format
	static Format scan(const std::string &, const Format);
	
	// match samples with data file
	void match_sample(unsigned long);
	
public:
	
	// get data file format
	char format() const;
	bool format(const Format) const;
	
	// provide additional files
	void run_sample(const std::string &);
	void run_legend(const std::string &);
	void run_genmap(const std::string &);
	
	// run loading
	void run(Source &);
	
	// construct/destruct
	Loader(const std::string &, const Format = UNKNOWN);
	~Loader();
	
	// do not copy
	Loader(const Loader &) = delete;
	Loader & operator = (const Loader &) = delete;
};



#endif /* defined(__ship__loader__) */
