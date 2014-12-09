//
//  sample.cpp
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "sample.h"


#define DEBUG_SAMPLE


//******************************************************************************
// Sample containers
//******************************************************************************

//
// Sample data
//


SampleData::Block::Block()
: block(new Datatype[ SampleData::Block::n ])
, i(0)
{}

SampleData::Block::Block(const Block & other)
: block(new Datatype[ SampleData::Block::n ])
, i(other.i)
{
	std::copy(other.block, other.block + other.i, this->block);
}

SampleData::Block::Block(Block && other)
: block(nullptr)
, i(other.i)
{
	this->block = other.block;
	other.block = nullptr;
}

SampleData::Block::~Block()
{
	if (this->block != nullptr)
		delete [] this->block;
}

SampleData::Block & SampleData::Block::operator = (const Block & other)
{
	if (this != &other)
	{
		std::copy(other.block, other.block + SampleData::Block::n, this->block);
		this->i = other.i;
	}
	return *this;
}

SampleData::Block & SampleData::Block::operator = (Block && other)
{
	if (this != &other)
	{
		delete [] this->block;
		this->block = other.block;
		other.block = nullptr;
		this->i = other.i;
	}
	return *this;
}

const size_t SampleData::Block::n = 32768; // 32Kb block

SampleData::SampleData()
: collect(1)
, current(collect.rbegin())
, data_(nullptr)
, size_(0)
, finished(false)
, cleared(false)
{}

SampleData::SampleData(const SampleData & other)
: collect(other.collect)
, current(collect.rbegin())
, data_(nullptr)
, size_(other.size_)
, finished(other.finished)
, cleared(other.cleared)
{
	if (this->finished)
	{
		this->data_ = new Datatype[ this->size_ ];
		std::copy(other.data_, other.data_ + other.size_, this->data_);
	}
}

SampleData::SampleData(SampleData && other)
: collect(std::move(other.collect))
, current(collect.rbegin())
, data_(nullptr)
, size_(other.size_)
, finished(other.finished)
, cleared(other.cleared)
{
	this->data_ = other.data_;
	other.data_ = nullptr;
}

SampleData::~SampleData()
{
	this->collect.clear();
	
	if (this->data_ != nullptr)
		delete [] this->data_;
}

SampleData & SampleData::operator = (const SampleData & other)
{
	if (this != &other)
	{
		this->collect = other.collect;
		this->current = this->collect.rbegin();
		
		if (this->finished && !this->cleared)
		{
			delete [] this->data_;
		}
		
		if (other.finished && !other.cleared)
		{
			this->data_ = new Datatype[ other.size_ ];
			std::copy(other.data_, other.data_ + other.size_, this->data_);
		}
		
		this->size_ = other.size_;
		this->finished = other.finished;
		this->cleared = other.cleared;
	}
	return *this;
}

SampleData & SampleData::operator = (SampleData && other)
{
	if (this != &other)
	{
		this->collect.swap(other.collect);
		this->current = this->collect.rbegin();
		
		if (this->finished && !this->cleared)
		{
			delete [] this->data_;
		}
		
		this->data_ = other.data_;
		other.data_ = nullptr;
		
		this->size_ = other.size_;
		this->finished = other.finished;
		this->cleared = other.cleared;
	}
	return *this;
}

void SampleData::append(const Genotype & g)
{
#ifdef DEBUG_SAMPLE
	if (cleared)
	{
		throw std::runtime_error("Sample data was cleared");
	}
	if (finished)
	{
		throw std::runtime_error("Sample data already finished");
	}
#endif
	
	size_t i = this->current->i;

	if (i == SampleData::Block::n)
	{
		this->collect.push_back(Block());
		this->current = this->collect.rbegin();
		i = 0;
	}
	
	this->current->block[i] = g;
	this->current->i += 1;
}

void SampleData::finish()
{
#ifdef DEBUG_SAMPLE
	if (cleared)
	{
		throw std::runtime_error("Sample data was cleared");
	}
	if (finished)
	{
		throw std::runtime_error("Sample data already finished");
	}
#endif
	
	// get complete size
	this->size_ = ((this->collect.size() - 1) * SampleData::Block::n) + this->current->i;
	
	// allocate memory
	try
	{
		this->data_ = new Datatype[ this->size_ ];
	}
	catch (std::bad_alloc &)
	{
		throw std::runtime_error("Cannot allocate memory for " + std::to_string(this->size_) + " markers");
	}
	
	// copy data
	size_t i = 0;
	for (std::vector<Block>::const_iterator it = this->collect.cbegin(), end = this->collect.cend(); it != end; ++it)
	{
		std::copy(it->block, it->block + it->i, this->data_ + i);
		i += it->i;
	}
	
	// clear data blocks
	std::vector<Block> x;
	this->collect.swap(x);
	this->current = this->collect.rend();
	
	this->finished = true;
}

void SampleData::clear()
{
	if (this->data_ != nullptr)
		delete [] this->data_;
	
	std::vector<Block> x;
	this->collect.swap(x);
	this->current = this->collect.rend();
	
	this->size_ = 0;
	this->finished = false;
	this->cleared = true;
}

Datatype SampleData::operator [] (const size_t i) const
{
#ifdef DEBUG_SAMPLE
	if (cleared)
	{
		throw std::runtime_error("Sample data was cleared");
	}
	if (!finished)
	{
		throw std::runtime_error("Sample data not finished");
	}
	if (i >= this->size_)
	{
		throw std::out_of_range("Sample data out of range");
	}
#endif
	
	return this->data_[i];
}

const Datatype & SampleData::at(const size_t i) const
{
#ifdef DEBUG_SAMPLE
	if (cleared)
	{
		throw std::runtime_error("Sample data was cleared");
	}
	if (!finished)
	{
		throw std::runtime_error("Sample data not finished");
	}
	if (i >= this->size_)
	{
		throw std::out_of_range("Sample data out of range");
	}
#endif
	
	return this->data_[i];
}

size_t SampleData::size() const
{
#ifdef DEBUG_SAMPLE
	if (cleared)
	{
		throw std::runtime_error("Sample data was cleared");
	}
	if (!finished)
	{
		throw std::runtime_error("Sample data not finished");
	}
#endif
	
	return this->size_;
}

void SampleData::print(std::ostream & stream, const char last) const
{
#ifdef DEBUG_SAMPLE
	if (cleared)
	{
		throw std::runtime_error("Sample data was cleared");
	}
	if (!finished)
	{
		throw std::runtime_error("Sample data not finished");
	}
#endif
	
	char sep = NULL;
	
	for (size_t i = 0; i < this->size_; ++i)
	{
		const Genotype g = (Genotype)this->data_[i];
		stream << sep << (int)g.h0 << ' ' << (int)g.h1;
		sep = ' ';
	}
	
	if (last != '\0')
		stream << last;
}

void SampleData::print(FILE * fp, const char last) const
{
#ifdef DEBUG_SAMPLE
	if (cleared)
	{
		throw std::runtime_error("Sample data was cleared");
	}
	if (!finished)
	{
		throw std::runtime_error("Sample data not finished");
	}
#endif
	
	char sep = NULL;
	
	for (size_t i = 0; i < this->size_; ++i)
	{
		const Genotype g = (Genotype)this->data_[i];
		fprintf(fp, "%c%d %d", sep, (int)g.h0, (int)g.h1);
		sep = ' ';
	}
	
	if (last != '\0')
		fprintf(fp, "%c", last);
}

std::string SampleData::str() const
{
	std::ostringstream oss;
	this->print(oss);
	return oss.str();
}


//
// Sample information
//

SampleInfo::SampleInfo()
: key(SampleInfo::unknown_key)
, pop(SampleInfo::unknown_pop)
{}

SampleInfo::SampleInfo(const SampleInfo & other)
: key(other.key)
, pop(other.pop)
{}

SampleInfo::SampleInfo(SampleInfo && other)
: key(std::move(other.key))
, pop(std::move(other.pop))
{}

SampleInfo::~SampleInfo()
{
	this->key.clear();
	this->pop.clear();
}

SampleInfo & SampleInfo::operator = (const SampleInfo & other)
{
	if (this != &other)
	{
		this->key = other.key;
		this->pop = other.pop;
	}
	return *this;
}

SampleInfo & SampleInfo::operator = (SampleInfo && other)
{
	if (this != &other)
	{
		this->key = std::move(other.key);
		this->pop = std::move(other.pop);
	}
	return *this;
}

bool SampleInfo::operator <  (const SampleInfo & other) const { return (this->pop == other.pop) ? (this->key <  other.key): (this->pop <  other.pop); }
bool SampleInfo::operator >  (const SampleInfo & other) const { return (this->pop == other.pop) ? (this->key >  other.key): (this->pop >  other.pop); }
bool SampleInfo::operator == (const SampleInfo & other) const {	return (this->key == other.key); } // compare sample ID only
bool SampleInfo::operator != (const SampleInfo & other) const { return (this->key != other.key); } // compare sample ID only

void SampleInfo::print(std::ostream & stream, const char last) const
{
	stream << this->key <<  ' ' << this->pop << last;
}

void SampleInfo::print(FILE * fp, const char last) const
{
	fprintf(fp, "%s %s%c", this->key.c_str(), this->pop.c_str(), last);
}

std::string SampleInfo::str() const
{
	std::ostringstream oss;
	this->print(oss);
	return oss.str();
}

const std::string SampleInfo::unknown_key = ".";
const std::string SampleInfo::unknown_pop = ".";
const std::string SampleInfo::header = "sample_id sample_pop";


//
// Sample container
//

Sample::Sample()
{}

Sample::Sample(const Sample & other)
: info(other.info)
, data(other.data)
{}

Sample::Sample(Sample && other)
: info(std::move(other.info))
, data(std::move(other.data))
{}

//Sample::~Sample() 
//{}

Sample & Sample::operator = (const Sample & other)
{
	if (this != &other)
	{
		this->info = other.info;
		this->data = other.data;
	}
	return *this;
}

Sample & Sample::operator = (Sample && other)
{
	if (this != &other)
	{
		this->info = std::move(other.info);
		this->data = std::move(other.data);
	}
	return *this;
}

bool Sample::operator <  (const Sample & other) const { return (this->info <  other.info); }
bool Sample::operator >  (const Sample & other) const { return (this->info >  other.info); }
bool Sample::operator == (const Sample & other) const {	return (this->info == other.info); }
bool Sample::operator != (const Sample & other) const { return (this->info != other.info); }


