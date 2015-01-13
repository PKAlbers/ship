//
//  source.h
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship__source__
#define __ship__source__

#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <thread>

#include "marker.h"
#include "sample.h"
#include "timer.h"


#define DEBUG_SOURCE


//******************************************************************************
// Marker & sample (data matrix) container
//******************************************************************************
class Source
{
private:
	
	Chromosome chromosome; // chromosome of source
	std::vector<Sample> sample_; // list of samples & data
	std::vector<Marker> marker_; // list of markers
	size_t sample_size_; // number of samples
	size_t marker_size_; // number of markers
	bool finished; // flag that appending was finished
	
	// sort data matrix
	void sort(const int);
	
public:
	
	void finish(const int); // finish by sorting markers
	
	// return marker/sample size
	size_t sample_size() const;
	size_t marker_size() const;
	
	// return marker/sample reference
	const Sample & sample(const size_t) const;
	const Marker & marker(const size_t) const;
	
	// return marker/sample vector reference
	const std::vector<Sample> & sample() const;
	const std::vector<Marker> & marker() const;
	
	// append marker/sample
	void append(Sample &&); // move
	void append(Marker &&); // move
	
	// assign
	Source & operator = (const Source &);
	Source & operator = (Source &&);
	
	// construct
	Source();
	Source(const Source &);
	Source(Source &&);
	
	// destruct
	~Source();
};



#endif /* defined(__ship__source__) */
