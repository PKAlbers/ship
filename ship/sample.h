//
//  sample.h
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship__sample__
#define __ship__sample__

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <queue>
#include <utility>
#include <algorithm>
#include <functional>
#include <sstream>
#include <new>

#include "types.h"


//******************************************************************************
// Sample containers
//******************************************************************************

//
// Sample data
//
class SampleData
{
private:
	
	// collect types in data blocks
	struct Block
	{
		Datatype * block; // data block
		size_t i; // increment counter
		
		static const size_t n; // default size
		
		// assign
		Block & operator = (const Block &);
		Block & operator = (Block &&);
		
		// construct
		Block();
		Block(const Block &);
		Block(Block &&);
		
		// destruct
		~Block();
	};
	
	std::vector<Block> collect;
	std::vector<Block>::reverse_iterator current;
	
	Datatype * data_;
	size_t     size_;
	bool finished;
	bool cleared;
	
public:
	
	// append type
	void append(const Genotype &);
	
	// compile types from data blocks
	void finish();
	
	// clear all data
	void clear();
	
	// return size
	size_t size() const;
	
	// return type
	Datatype operator [] (const size_t) const;
	const Datatype & at(const size_t) const;
	
	// print to stream
	void print(std::ostream &, const char = '\0') const;
	void print(FILE *, const char = '\0') const;
	
	// convert to string
	std::string str() const;
	
	// assign
	SampleData & operator = (const SampleData &);
	SampleData & operator = (SampleData &&);
	
	// construct
	SampleData();
	SampleData(const SampleData &);
	SampleData(SampleData &&);
	
	// destruct
	~SampleData();
};


//
// Sample information
//
struct SampleInfo
{
	std::string key; // unique sample identifier
	std::string pop; // population
	
	static const std::string unknown_key; // unknown individual ID
	static const std::string unknown_pop; // unknown population
	
	// compare/sort
	bool operator <  (const SampleInfo & other) const;
	bool operator >  (const SampleInfo & other) const;
	bool operator == (const SampleInfo & other) const;
	bool operator != (const SampleInfo & other) const;
	
	// print to stream
	void print(std::ostream &, const char = '\0') const;
	void print(FILE *, const char = '\0') const;
	
	// convert to string
	std::string str() const;
	
	// constant header for printing
	static const std::string header;
	
	// assign
	SampleInfo & operator = (const SampleInfo &);
	SampleInfo & operator = (SampleInfo &&);
	
	// construct
	SampleInfo(); // default
	SampleInfo(const SampleInfo &); // copy
	SampleInfo(SampleInfo &&); // move
	
	// destruct
	~SampleInfo();
};


//
// Sample container
//
struct Sample
{
	SampleInfo info; // sample information
	SampleData data; // genotype sequence for this sample
	
	// compare/sort, by info
	bool operator <  (const Sample & other) const;
	bool operator >  (const Sample & other) const;
	bool operator == (const Sample & other) const;
	bool operator != (const Sample & other) const;
	
	// assign
	Sample & operator = (const Sample &);
	Sample & operator = (Sample &&);
	
	// construct
	Sample(); // default
	Sample(const Sample &); // copy
	Sample(Sample &&); // move
	
	// destruct
	//~Sample();
};



#endif /* defined(__ship__sample__) */
