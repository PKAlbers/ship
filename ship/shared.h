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
#include <set>
#include <thread>
#include <mutex>

#include "types.hpp"
#include "source.h"


#define DEBUG_SHARED


//******************************************************************************
// Shared haplotype containers
//******************************************************************************

struct SharedType;
struct SharedTree;
struct SharedNode;
struct SharedRoot;

//
// Shared haplotype
//
struct SharedType
{
	const Haplotype     haplotype; // shared haplotype
	const size_t        marker_id; // marker where haplotype is located
	std::vector<size_t> sample_id; // subsample sharing haplotype
	
	// construct
	SharedType(const Haplotype, const size_t);
};


//
// Shared haplotype tree structure
//
struct SharedTree
{
	const bool              side; // left (false) or right (true) sided scan
	size_t                  stop; // marker ID at breakpoint
	std::vector<SharedNode> node; // node of off-going branches
	
	// scan structure to create node, recursively
	void scan(const Source &, const SharedType &); // return breakpoint
	
	// count nodes and sub-nodes
	size_t count() const;
	
	// construct
	SharedTree(const bool);
};


//
// Shared haplotype node in tree
//
struct SharedNode
{
	SharedType type; // shared haplotype
	SharedTree tree; // tree structure
	
	// construct
	SharedNode(const Haplotype, const size_t, const bool);
};


//
// Shared haplotype root of tree
//
struct SharedRoot
{
	SharedType type; // shared haplotype
	SharedTree ltree, rtree; // left/right tree structure
	
	// get subsample sharing root haplotype
	void subsample(const Source &);
	
	// scan left & right tree
	void scan(const Source &);
	
	// construct
	SharedRoot(const Haplotype, const size_t);
};


//
// All shared haplotypes
//
class Shared
{
private:
	
	std::vector<SharedRoot> root; // root of shared haplotype structures
	size_t size_; // number of shared haplotypes
	size_t marker_count_; // number of markers
	
	// scan with mutliple threads
	void scan_parallel(const std::vector<size_t> &, const Source &, ProgressBar &);
	std::mutex ex_scan;
	
public:
	
	// return shared haplotype
	const SharedRoot & operator [] (const size_t) const;
	SharedRoot & at(const size_t);
	
	// return number of shared haplotypes
	size_t size() const;
	
	// return number of markers
	size_t marker_count() const;
	
	// scan all shared haplotype structures
	void scan(const Source &, const int);
	
	// construct
	Shared(const Source &, const Census &);
};




#endif /* defined(__ship__shared__) */
