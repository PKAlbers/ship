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

#include "marker.h"
#include "sample.h"


//******************************************************************************
// Marker & sample (data matrix) container
//******************************************************************************
class Source
{
private:
	
	unsigned long marker_size_; // number of markers
	unsigned long sample_size_; // number of samples
	std::vector<Marker> marker_; // list of markers
	std::vector<Sample> sample_; // list of samples & data
	
	// sort data matrix
	void sort();
	
	bool good; // flag that markers were sorted, no new markers/samples can be added
	
public:
	
	void finish(); // finish by sorting markers
	
	// return marker/sample size
	unsigned long marker_size() const;
	unsigned long sample_size() const;
	
	// return marker/sample data
	const Marker & marker(const unsigned long) const;
	const Sample & sample(const unsigned long) const;
	
	// append marker/sample
	void append(Marker &&); // move
	void append(Sample &&); // move
	
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
