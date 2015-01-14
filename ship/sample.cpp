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

#define SAMPLE_DATA_BLOCK_SIZE 32768 // 32Kb block

//
// Sample data
//

SampleData::Block::Block()
: block(SAMPLE_DATA_BLOCK_SIZE)
, i(0)
{}

SampleData::SampleData()
: collect(1) // first block
, current(collect.rbegin())
, n(0)
{}

SampleData::SampleData(const SampleData & other)
: collect(other.collect)
, current(collect.rbegin())
, data(other.data)
, n(other.n)
{}

SampleData::SampleData(SampleData && other)
: collect(std::move(other.collect))
, current(collect.rbegin())
, data(std::move(other.data))
, n(other.n)
{}

SampleData & SampleData::operator = (const SampleData & other)
{
	if (this != &other)
	{
		this->collect = other.collect;
		this->current = this->collect.rbegin();
		this->data = other.data;
		this->n = other.n;
	}
	return *this;
}

SampleData & SampleData::operator = (SampleData && other)
{
	if (this != &other)
	{
		this->collect.swap(other.collect);
		this->current = this->collect.rbegin();
		this->data.swap(other.data);
		this->n = other.n;
	}
	return *this;
}

void SampleData::append(const Genotype & g)
{
#ifdef DEBUG_SAMPLE
	if (this->collect.size() == 0)
	{
		throw std::runtime_error("Sample data empty");
	}
#endif
	
	size_t i = this->current->i;
	
	if (i == SAMPLE_DATA_BLOCK_SIZE)
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
	if (this->collect.size() == 0)
	{
		throw std::runtime_error("Sample data already finished");
	}
#endif
	
	// get complete size
	this->n = ((this->collect.size() - 1) * SAMPLE_DATA_BLOCK_SIZE) + this->current->i;
	
	// allocate data
	this->data.reserve(this->n);
	
	// copy data
	for (std::vector<Block>::const_iterator it = this->collect.cbegin(), end = this->collect.cend(); it != end; ++it)
	{
		this->data.insert(this->data.end(), it->block.begin(), it->block.end());
	}
	
	// clear data blocks
	std::vector<Block> _collect;
	this->collect.swap(_collect);
	
	this->current = this->collect.rend();
}

void SampleData::remove()
{
	std::vector<Block> _collect;
	this->collect.swap(_collect);
	
	this->current = this->collect.rend();
	
	std::vector<Datatype> _data;
	this->data.swap(_data);
	
	this->n = 0;
}

const Datatype & SampleData::operator [] (const size_t i) const
{
#ifdef DEBUG_SAMPLE
	if (this->collect.size() != 0)
	{
		throw std::runtime_error("Sample data not completed");
	}
	if (i >= this->n)
	{
		throw std::out_of_range("Sample data out of range");
	}
#endif
	
	return this->data[i];
}

size_t SampleData::size() const
{
	return this->n;
}

void SampleData::print(std::ostream & stream, const char last) const
{
#ifdef DEBUG_SAMPLE
	if (this->collect.size() != 0)
	{
		throw std::runtime_error("Sample data not completed");
	}
#endif
	
	Genotype g = (Genotype)this->data[0];
	stream << (int)g.h0 << ' ' << (int)g.h1;
	
	for (size_t i = 1; i < this->n; ++i)
	{
		g = (Genotype)this->data[i];
		stream << ' ' << (int)g.h0 << ' ' << (int)g.h1;
	}
	
	if (last != '\0')
		stream << last;
}

void SampleData::print(FILE * fp, const char last) const
{
#ifdef DEBUG_SAMPLE
	if (this->collect.size() != 0)
	{
		throw std::runtime_error("Sample data not completed");
	}
#endif
	
	Genotype g = (Genotype)this->data[0];
	fprintf(fp, "%d %d", (int)g.h0, (int)g.h1);
	
	for (size_t i = 1; i < this->n; ++i)
	{
		g = (Genotype)this->data[i];
		fprintf(fp, " %d %d", (int)g.h0, (int)g.h1);
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
: key(".")
, pop(".")
, grp(".")
{}

SampleInfo::SampleInfo(const SampleInfo & other)
: key(other.key)
, pop(other.pop)
, grp(other.grp)
{}

SampleInfo::SampleInfo(SampleInfo && other)
: key(std::move(other.key))
, pop(std::move(other.pop))
, grp(std::move(other.grp))
{}

SampleInfo::~SampleInfo()
{
	this->key.clear();
	this->pop.clear();
	this->grp.clear();
}

SampleInfo & SampleInfo::operator = (const SampleInfo & other)
{
	if (this != &other)
	{
		this->key = other.key;
		this->pop = other.pop;
		this->grp = other.grp;
	}
	return *this;
}

SampleInfo & SampleInfo::operator = (SampleInfo && other)
{
	if (this != &other)
	{
		this->key = std::move(other.key);
		this->pop = std::move(other.pop);
		this->grp = std::move(other.grp);
	}
	return *this;
}

bool SampleInfo::operator <  (const SampleInfo & other) const { return (this->grp == other.grp) ? ((this->pop == other.pop) ? (this->key <  other.key): (this->pop <  other.pop)): (this->grp <  other.grp); }
bool SampleInfo::operator >  (const SampleInfo & other) const { return (this->grp == other.grp) ? ((this->pop == other.pop) ? (this->key >  other.key): (this->pop >  other.pop)): (this->grp >  other.grp); }
bool SampleInfo::operator == (const SampleInfo & other) const {	return (this->key == other.key); } // compare sample ID only
bool SampleInfo::operator != (const SampleInfo & other) const { return (this->key != other.key); } // compare sample ID only

void SampleInfo::print(std::ostream & stream, const char last) const
{
	stream << this->key <<  ' ' << this->pop <<  ' ' << this->grp << last;
}

void SampleInfo::print(FILE * fp, const char last) const
{
	if (last != '\0')
		fprintf(fp, "%s %s %s%c", this->key.c_str(), this->pop.c_str(), this->grp.c_str(), last);
	else
		fprintf(fp, "%s %s %s", this->key.c_str(), this->pop.c_str(), this->grp.c_str());
}

std::string SampleInfo::str() const
{
	std::ostringstream oss;
	this->print(oss);
	return oss.str();
}

const std::string SampleInfo::header = "sample_id sample_pop sample_group";


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
	std::cout << "sample" << std::endl;
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


