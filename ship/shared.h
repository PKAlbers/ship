//
//  shared.h
//  ship
//
//  Created by Patrick Albers on 11.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship__shared__
#define __ship__shared__

#include <stdio.h>
#include <stdint.h>
#include <vector>

#include "types.hpp"
#include "source.h"


#define DEBUG_SHARED




struct SharedSegment
{
	
	
	
	// subsample sharing haplotype
	std::vector<Sample*> subsample;
	unsigned long n_sample; // number of samples
	
	// markers in shared sequence
	Marker* from, to;
	unsigned long n_marker; // number of markers
	
	// node at breakpoint
	SharedSegment * node;
	uint8_t n_nodes;
};



//******************************************************************************
// Shared haplotype container
//******************************************************************************
class SharedHaplotype
{
private:
	
	std::vector<size_t> sample_id; // samples sharing haplotype
	size_t size_; // number of sample IDs
	
public:
	
	const size_t    marker_id; // marker where haplotype is located
	const Haplotype haplotype; // shared haplotype
	
	// detect subsample sharing haplotype
	void detect(const Source &);
	
	// return subsample
	size_t subsample(const size_t) const;
	
	// return number of subsamples
	size_t size() const;
	
	// construct
	SharedHaplotype(const size_t, const Haplotype);
};


//******************************************************************************
// Shared haplotype list
//******************************************************************************
class Shared
{
private:
	
	std::vector<SharedHaplotype> list;
	size_t size_;
	size_t marker_count_;
	
public:
	
	// return shared haplotype
	const SharedHaplotype & operator [] (const size_t) const;
	SharedHaplotype & at(const size_t);
	
	// return number of shared haplotypes
	size_t size() const;
	
	// return number of markers
	size_t marker_count() const;
	
	// identify shared haplotypes
	void identify(const Source &, const Census &);
	
	// construct
	Shared();
};


/*


class Subsample
{
	const Selected selected;
	std::vector<size_t> subsample;
};



class Shared
{
	const size_t sample_id;
	const bool   is_shared;
	
	// construct
	Shared(const size_t, const bool);
};


class SharedList
{
	std::vector<Shared> list;
	size_t size;
	
	// construct
	SharedList(const Source &, const SelectedList &);
};



struct SharedHaplotype
{
	const Haplotype allele;
	const std::vector<Marker>::const_iterator marker;
	const std::vector< std::vector<Sample>::const_iterator > sample;
};


class SharedList
{
private:
	
	std::vector<SharedHaplotype> list;
	const std::vector<Marker>::const_iterator beg;
	const std::vector<Marker>::const_iterator end;
	size_t n;
	
public:
	
	operator std::vector<Marker>::const_iterator () const;
	
	bool next();
	bool prev();
	
	// construct
	SharedList(const std::vector<Marker> &);
};


*/






//******************************************************************************
// Shared haplotype containers
//******************************************************************************

/*
 struct SharedTree
 {
	// subsample sharing haplotype
	std::vector<Sample*> subsample;
	unsigned long n_sample; // number of samples
	
	// markers in shared sequence
	Marker* from, to;
	unsigned long n_marker; // number of markers
	
	// node at breakpoint
	SharedTree * node;
	uint8_t n_nodes;
	
	// assign
	SharedTree & operator = (const SharedTree &);
	SharedTree & operator = (SharedTree &&);
	
	// construct
	SharedTree();
	SharedTree(const SharedTree &);
	SharedTree(SharedTree &&);
	
	// destruct
	~SharedTree();
 };
 */


#endif /* defined(__ship__shared__) */
