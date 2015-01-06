//
//  census.h
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship__census__
#define __ship__census__

#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <stdexcept>


#define DEBUG_CENSUS


//******************************************************************************
// Census container
//******************************************************************************

//
// Number & frequency container
//
class Census
{
protected:
	
	size_t n; // number
	double f; // frequency
	bool scaled; // flag that number was scaled to yield frequency
	
public:
	
	// in/decrement
	Census & operator ++ ();
	Census & operator -- ();
	
	// scale number/frequency given size
	virtual void scale(const size_t);
	
	// assign count
	Census & operator = (const size_t);
	
	// cast
	explicit operator size_t () const;
	explicit operator double () const;
	
	// compare with other census container
	bool operator <  (const Census & other) const;
	bool operator >  (const Census & other) const;
	bool operator <= (const Census & other) const;
	bool operator >= (const Census & other) const;
	bool operator == (const Census & other) const;
	bool operator != (const Census & other) const;
	
	// compare with count
	bool operator <  (const size_t) const;
	bool operator >  (const size_t) const;
	bool operator <= (const size_t) const;
	bool operator >= (const size_t) const;
	bool operator == (const size_t) const;
	bool operator != (const size_t) const;
	
	// compare with frequency
	bool operator <  (const double) const;
	bool operator >  (const double) const;
	bool operator <= (const double) const;
	bool operator >= (const double) const;
	bool operator == (const double) const;
	bool operator != (const double) const;
	
	// assign
	Census & operator = (const Census &);
	Census & operator = (Census &&);
	
	// construct
	Census();
	Census(const size_t);
	Census(const size_t, const size_t);
	Census(const Census &);
	Census(Census &&);
};



//
// Threshold container
//
class Cutoff : public Census
{
private:
	
	bool n_; // flag that count was provided
	bool f_; // flag that freq. was provided
	
public:
	
	// parse command line value
	void parse(const std::string &);
	
	// scale using sample size
	void scale(const size_t);
	
	Cutoff();
};



//
// Number & frequency list
//
template <class Type>
class CensusList
{
private:
	
	bool indexed;
	std::unordered_map<Type, int> index; // type points to index in list
	std::vector<Type> type_; // type list
	std::vector<Census> census_; // census list
	int n;
	
public:
	
	// return census from type
	const Census & operator [] (const Type & _type) const
	{
#ifdef DEBUG_CENSUS
		if (this->index.count(_type) == 0)
		{
			throw std::invalid_argument("Census type does not exist");
		}
#endif
		
		return this->census_[ this->index.at(_type) ];
	}
	
	// return census from index
	const Census & census(const int _i) const
	{
#ifdef DEBUG_CENSUS
		if (_i >= this->n)
		{
			throw std::out_of_range("Census index out of range");
		}
#endif
		return this->census_[ _i ];
	}
	
	// return type from index
	const Type & type(const int _i) const
	{
#ifdef DEBUG_CENSUS
		if (_i >= this->n)
		{
			throw std::out_of_range("Census index out of range");
		}
#endif
		return this->type_[ _i ];
	}
	
	// return size
	int size() const
	{
		return this->n;
	}
	
	// append type, with census
	bool append(const Type & _type)
	{
#ifdef DEBUG_CENSUS
		if (this->index_.count(_type) != 0)
			return false;
#endif
		this->index_[ _type ] = this->n;
		this->types_.push_back(_type);
		this->census_.push_back(Census());
		++this->n;
		return true;
	}
	
	bool append(const Type & _type, const Census & _census)
	{
		
#ifdef DEBUG_CENSUS
		if (this->index.count(_type) != 0)
			return false;
#endif
		this->index[ _type ] = this->n;
		this->type_.push_back(_type);
		this->census_.push_back(_census);
		++this->n;
		return true;
	}
	
	bool append(const Type & _type, Census && _census)
	{
		
#ifdef DEBUG_CENSUS
		if (this->index.count(_type) != 0)
			return false;
#endif
		this->index[ _type ] = this->n;
		this->type_.push_back(_type);
		this->census_.push_back(std::move(_census));
		++this->n;
		return true;
	}
	
	// assign
	CensusList<Type> & operator = (const CensusList<Type> & other)
	{
		if (this != &other)
		{
			this->indexed = other.indexed;
			this->index = other.index;
			this->type_ = other.type_;
			this->census_ = other.census_;
			this->n = other.n;
		}
		return *this;
	}
	
	CensusList<Type> & operator = (CensusList<Type> && other)
	{
		if (this != &other)
		{
			this->indexed = other.indexed;
			this->index = std::move(other.index);
			this->type_ = std::move(other.type_);
			this->census_ = std::move(other.census_);
			this->n = other.n;
		}
		return *this;
	}
	
	// construct
	CensusList()
	: n(0)
	{}
	
	CensusList(const CensusList<Type> & other)
	: indexed(other.indexed)
	, index(other.index)
	, type_(other.type_)
	, census_(other.census_)
	, n(other.n)
	{}
	
	CensusList(CensusList<Type> && other)
	: indexed(other.indexed)
	, index(std::move(other.index))
	, type_(std::move(other.type_))
	, census_(std::move(other.census_))
	, n(other.n)
	{}
};



#endif /* defined(__ship__census__) */
