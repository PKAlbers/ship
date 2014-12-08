//
//  census.cpp
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "census.h"


//******************************************************************************
// Census container
//******************************************************************************

//
// Number & frequency container
//

Census::Census()
: n(0)
, f(0)
, n_(true)
, f_(false)
{}

Census::Census(const Census & other)
: n(other.n)
, f(other.f)
, n_(other.n_)
, f_(other.f_)
{}

Census::Census(Census && other)
: n(other.n)
, f(other.f)
, n_(other.n_)
, f_(other.f_)
{}

//Census::~Census()
//{}

Census & Census::operator = (const Census & other)
{
	if (this != &other)
	{
		this->n = other.n;
		this->f = other.f;
		this->n_ = other.n_;
		this->f_ = other.f_;
	}
	return *this;
}

Census & Census::operator = (Census && other)
{
	if (this != &other)
	{
		this->n = other.n;
		this->f = other.f;
		this->n_ = other.n_;
		this->f_ = other.f_;
	}
	return *this;
}

bool Census::operator <  (const Census & other) const { return ((this->n_ && other.n_ && this->n <  other.n) || (this->f_ && other.f_ && this->f <  other.f)); }
bool Census::operator >  (const Census & other) const { return ((this->n_ && other.n_ && this->n >  other.n) || (this->f_ && other.f_ && this->f >  other.f)); }
bool Census::operator <= (const Census & other) const { return ((this->n_ && other.n_ && this->n <= other.n) || (this->f_ && other.f_ && this->f <= other.f)); }
bool Census::operator >= (const Census & other) const { return ((this->n_ && other.n_ && this->n >= other.n) || (this->f_ && other.f_ && this->f >= other.f)); }
bool Census::operator == (const Census & other) const { return ((this->n_ && other.n_ && this->n == other.n) || (this->f_ && other.f_ && this->f == other.f)); }
bool Census::operator != (const Census & other) const { return ((this->n_ && other.n_ && this->n != other.n) || (this->f_ && other.f_ && this->f != other.f)); }

Census::operator unsigned long () const
{
	if (this->n_)
		return this->n;
	throw std::runtime_error("Number not provided in census");
	return 0;
};

Census::operator double () const
{
	if (this->f_)
		return this->f;
	throw std::runtime_error("Frequency not provided in census");
	return 0;
};

void Census::operator () (const unsigned long _n)
{
	this->n = _n;
	this->n_ = true;
	this->f_ = false;
}

void Census::operator () (const double _f)
{
	if (_f > 1)
		throw std::logic_error("Cannot use frequency larger than 1");
	
	this->f = _f;
	this->f_ = true;
	this->n_ = false;
}

void Census::scale(const unsigned long size)
{
	if (! this->n_ && this->f_)
	{
		this->n  = this->f * static_cast<double>(size);
		this->n_ = true;
		return;
	}
	
	if (! this->f_ && this->n_)
	{
		if (this->n > size)
			throw std::logic_error("Cannot scale frequency with size larger than count");
		
		this->f  = static_cast<double>(this->n) / static_cast<double>(size);
		this->f_ = true;
		return;
	}
	
	throw std::logic_error("Cannot scale census due to unknown count and frequency");
}

bool Census::scaled() const
{
	return (this->n_ && this->f_);
}

Census & Census::operator ++ ()
{
	this->n += 1;
	this->n_ = true;
	
	this->f  = 0;
	this->f_ = false;
	
	return *this;
}

Census & Census::operator -- ()
{
	if (this->n == 0)
		throw std::runtime_error("Cannot decrement zero count");
	
	this->n -= 1;
	this->n_ = true;
	
	this->f  = 0;
	this->f_ = false;
	return *this;
}


//
// Commandline threshold container
//

void Cutoff::parse(const std::string & str)
{
	double value;
	
	std::istringstream iss(str);
	if (iss >> value)
	{
		// number
		if (value > 1)
		{
			if (value != round(value))
			{
				throw std::invalid_argument("Threshold frequency cannot be larger than 1");
			}
			else
			{
				this->n  = static_cast<unsigned long>(value);
				this->n_ = true;
				this->f_ = false;
				return;
			}
		}
		
		// frequency
		if (value < 0)
		{
			throw std::invalid_argument("Threshold value cannot be smaller than 0");
		}
		else
		{
			this->f  = value;
			this->f_ = true;
			this->n_ = false;
			return;
		}
		
	}
	
	throw std::invalid_argument("Cannot interpret threshold value: " + str);
}



