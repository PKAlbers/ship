//
//  census.cpp
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "census.h"


#define DEBUG_CENSUS


//******************************************************************************
// Census container
//******************************************************************************

//
// Number & frequency container
//

Census::Census()
: n(0)
, f(0)
, scaled_(false)
{}

Census::Census(const size_t size)
: n(size)
, f(0)
, scaled_(false)
{}

Census::Census(const Census & other)
: n(other.n)
, f(other.f)
, scaled_(other.scaled_)
{}

Census::Census(Census && other)
: n(other.n)
, f(other.f)
, scaled_(other.scaled_)
{}

Census & Census::operator = (const Census & other)
{
	if (this != &other)
	{
		this->n = other.n;
		this->f = other.f;
		this->scaled_ = other.scaled_;
	}
	return *this;
}

Census & Census::operator = (Census && other)
{
	if (this != &other)
	{
		this->n = other.n;
		this->f = other.f;
		this->scaled_ = other.scaled_;
	}
	return *this;
}

bool Census::operator <  (const Census & other) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_ || !other.scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n < other.n);
}

bool Census::operator >  (const Census & other) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_ || !other.scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n > other.n);
}

bool Census::operator <= (const Census & other) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_ || !other.scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n <= other.n);
}

bool Census::operator >= (const Census & other) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_ || !other.scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n >= other.n);
}

bool Census::operator == (const Census & other) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_ || !other.scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n == other.n);
}

bool Census::operator != (const Census & other) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_ || !other.scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n != other.n);
}

bool Census::operator < (const size_t _n) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n < _n);
}

bool Census::operator > (const size_t _n) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n > _n);
}

bool Census::operator <= (const size_t _n) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n <= _n);
}

bool Census::operator >= (const size_t _n) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n >= _n);
}

bool Census::operator == (const size_t _n) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n == _n);
}

bool Census::operator != (const size_t _n) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n != _n);
}

bool Census::operator < (const double _f) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->f < _f);
}

bool Census::operator > (const double _f) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->f > _f);
}

bool Census::operator <= (const double _f) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->f <= _f);
}

bool Census::operator >= (const double _f) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->f >= _f);
}

bool Census::operator == (const double _f) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->f == _f);
}

bool Census::operator != (const double _f) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled_)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->f != _f);
}

Census::operator size_t () const
{
	return this->n;
};

Census::operator double () const
{
	return this->f;
};

Census & Census::operator = (const size_t _n)
{
	this->n = _n;
	return *this;
}

void Census::scale(const size_t _size)
{
	if (this->n > _size)
	{
		throw std::logic_error("Cannot scale frequency with size larger than count");
	}
	
	this->f = static_cast<double>(this->n) / static_cast<double>(_size);
	this->scaled_ = true;
}

bool Census::scaled() const
{
	return this->scaled_;
}

Census & Census::operator ++ ()
{
	++this->n;
	this->scaled_ = false;
	return *this;
}

Census & Census::operator -- ()
{
	if (this->n == 0)
	{
		throw std::runtime_error("Cannot decrement zero count");
	}
	
	--this->n;
	this->scaled_ = false;
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
			
			this->n = static_cast<unsigned long>(value);
			return;
		}
		
		// frequency
		if (value < 0)
		{
			throw std::invalid_argument("Threshold value cannot be smaller than 0");
		}
		
		this->f  = value;
		return;
	}
	
	throw std::invalid_argument("Cannot interpret threshold value: " + str);
}

void Cutoff::size(const size_t _size)
{
	if (this->n > 0 && this->f == 0)
	{
		this->f = static_cast<double>(this->n) / static_cast<double>(_size);
		this->scaled_ = true;
		return;
	}
	
	if (this->f > 0 && this->n == 0)
	{
		this->n = static_cast<size_t>(this->f * static_cast<double>(_size));
		this->scaled_ = true;
		return;
	}
	
	throw std::logic_error("Cannot scale threshold");
}



