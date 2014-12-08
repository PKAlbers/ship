//
//  sample.cpp
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "sample.h"


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
, data(nullptr)
, size_(0)
, good(false)
{}

SampleData::SampleData(const SampleData & other)
: collect(other.collect)
, current(collect.rbegin())
, data(nullptr)
, size_(other.size_)
, good(other.good)
{
	if (this->good)
	{
		try
		{
			this->data = new Datatype[ this->size_ ];
		}
		catch (std::bad_alloc &)
		{
			throw std::runtime_error("Cannot allocate memory while overwriting sample data");
		}
		std::copy(other.data, other.data + other.size_, this->data);
	}
}

SampleData::SampleData(SampleData && other)
: collect(std::move(other.collect))
, current(collect.rbegin())
, data(nullptr)
, size_(other.size_)
, good(other.good)
{
	this->data = other.data;
	other.data = nullptr;
}

SampleData::~SampleData()
{
	this->collect.clear();
	
	if (this->data != nullptr)
		delete [] this->data;
}

SampleData & SampleData::operator = (const SampleData & other)
{
	if (this != &other)
	{
		this->collect = other.collect;
		this->current = this->collect.rbegin();
		
		if (this->good)
		{
			delete [] this->data;
		}
		
		if (other.good)
		{
			try
			{
				this->data = new Datatype[ other.size_ ];
			}
			catch (std::bad_alloc &)
			{
				throw std::runtime_error("Cannot allocate memory while moving sample data");
			}
			std::copy(other.data, other.data + other.size_, this->data);
		}
		
		this->size_ = other.size_;
		this->good  = other.good;
	}
	return *this;
}

SampleData & SampleData::operator = (SampleData && other)
{
	if (this != &other)
	{
		this->collect.swap(other.collect);
		this->current = this->collect.rbegin();
		
		if (this->good)
		{
			delete [] this->data;
		}
		
		this->data = other.data;
		other.data = nullptr;
		
		this->size_ = other.size_;
		this->good  = other.good;
	}
	return *this;
}

void SampleData::append(Datatype && d)
{
	unsigned long i = this->current->i;
	
	if (i == SampleData::Block::n)
	{
		this->collect.push_back(Block());
		this->current = this->collect.rbegin();
		i = 0;
	}
	
	this->current->block[i] = d;
	this->current->i += 1;
}

void SampleData::append(Genotype && g)
{
	unsigned long i = this->current->i;

	if (i == SampleData::Block::n)
	{
		this->collect.push_back(Block());
		this->current = this->collect.rbegin();
		i = 0;
	}
	
	this->current->block[i] = Datatype(g);
	this->current->i += 1;
}

void SampleData::finish()
{
	if (good)
		throw std::logic_error("Sample data already finished");
	
	// get complete size
	this->size_ = ((this->collect.size() - 1) * SampleData::Block::n) + this->current->i;
	
	// allocate memory
	try
	{
		this->data = new Datatype[ this->size_ ];
	}
	catch (std::bad_alloc &)
	{
		throw std::runtime_error("Cannot allocate memory for sample size of " + std::to_string(this->size_));
	}
	
	// copy data
	unsigned long i = 0;
	for (std::vector<Block>::const_iterator it = this->collect.cbegin(), end = this->collect.cend(); it != end; ++it)
	{
		std::copy(it->block, it->block + it->i, this->data + i);
		i += it->i;
	}
	
	// clear data blocks
	std::vector<Block> x;
	this->collect.swap(x);
	this->current = this->collect.rend();
	
	this->good = true;
}

Datatype SampleData::operator [] (const unsigned long i) const
{
	if (i < this->size_)
	{
		return this->data[i];
	}
	throw std::out_of_range("Sample data out of range");
	return Datatype();
}

const Datatype & SampleData::at(const unsigned long i) const
{
	static const Datatype unknown;
	
	if (i < this->size_)
	{
		return this->data[i];
	}
	throw std::out_of_range("Sample data out of range");
	return unknown;
}

unsigned long SampleData::size() const
{
	return this->size_;
}


//
// Sample information
//

SampleInfo::SampleInfo()
: key(SampleInfo::unknown_key)
, grp(SampleInfo::unknown_grp)
, pop(SampleInfo::unknown_pop)
{}

SampleInfo::SampleInfo(const SampleInfo & other)
: key(other.key)
, grp(other.grp)
, pop(other.pop)
{}

SampleInfo::SampleInfo(SampleInfo && other)
: key(std::move(other.key))
, grp(std::move(other.grp))
, pop(std::move(other.pop))
{}

SampleInfo::~SampleInfo()
{
	this->key.clear();
	this->grp.clear();
	this->pop.clear();
}

SampleInfo & SampleInfo::operator = (const SampleInfo & other)
{
	if (this != &other)
	{
		this->key = other.key;
		this->grp = other.grp;
		this->pop = other.pop;
	}
	return *this;
}

SampleInfo & SampleInfo::operator = (SampleInfo && other)
{
	if (this != &other)
	{
		this->key = std::move(other.key);
		this->grp = std::move(other.grp);
		this->pop = std::move(other.pop);
	}
	return *this;
}

bool SampleInfo::operator <  (const SampleInfo & other) const { return (this->grp == other.grp) ?  ( (this->pop == other.pop) ? (this->key <  other.key): (this->pop <  other.pop) ): (this->grp <  other.grp); }
bool SampleInfo::operator >  (const SampleInfo & other) const { return (this->grp == other.grp) ?  ( (this->pop == other.pop) ? (this->key >  other.key): (this->pop >  other.pop) ): (this->grp >  other.grp); }
bool SampleInfo::operator == (const SampleInfo & other) const {	return (this->key == other.key); } // compare sample ID only
bool SampleInfo::operator != (const SampleInfo & other) const { return (this->key != other.key); } // compare sample ID only

void SampleInfo::print(std::ostream & stream, const char last) const
{
	stream << this->key << ' ' << this->grp <<  ' ' << this->pop << last;
}

void SampleInfo::print(FILE * fp, const char last) const
{
	fprintf(fp, "%s %s %s%c", this->key.c_str(), this->grp.c_str(), this->pop.c_str(), last);
}

std::string SampleInfo::str() const
{
	std::ostringstream oss;
	this->print(oss);
	return oss.str();
}

const std::string SampleInfo::unknown_key = ".";
const std::string SampleInfo::unknown_grp = ".";
const std::string SampleInfo::unknown_pop = ".";
const std::string SampleInfo::header = "sample_id super_pop sub_pop";


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


