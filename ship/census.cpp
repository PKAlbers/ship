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
, scaled(false)
{}

Census::Census(const size_t _n)
: n(_n)
, f(0)
, scaled(false)
{}

Census::Census(const size_t _n, const size_t _scale)
: n(_n)
, f(0)
, scaled(false)
{
	this->scale(_scale);
}

Census::Census(const Census & other)
: n(other.n)
, f(other.f)
, scaled(other.scaled)
{}

Census::Census(Census && other)
: n(other.n)
, f(other.f)
, scaled(other.scaled)
{}

Census & Census::operator = (const Census & other)
{
	if (this != &other)
	{
		this->n = other.n;
		this->f = other.f;
		this->scaled = other.scaled;
	}
	return *this;
}

Census & Census::operator = (Census && other)
{
	if (this != &other)
	{
		this->n = other.n;
		this->f = other.f;
		this->scaled = other.scaled;
	}
	return *this;
}

Census::operator size_t () const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return this->n;
};

Census::operator double () const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return this->f;
};

Census & Census::operator = (const size_t _n)
{
	this->n = _n;
	this->f = 0;
	this->scaled = false;
	return *this;
}

void Census::scale(const size_t _size)
{
#ifdef DEBUG_CENSUS
	if (this->scaled)
	{
		throw std::logic_error("Census already scaled");
	}
#endif
	
	if (this->n > _size)
	{
		throw std::logic_error("Cannot scale frequency with count larger than size");
	}
	
	this->f = static_cast<double>(this->n) / static_cast<double>(_size);
	this->scaled = true;
}

Census & Census::operator ++ ()
{
	++this->n;
	this->scaled = false;
	return *this;
}

Census & Census::operator -- ()
{
	if (this->n == 0)
	{
		throw std::runtime_error("Cannot decrease zero count");
	}
	
	--this->n;
	this->scaled = false;
	return *this;
}

bool Census::operator <  (const Census & other) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled || !other.scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n < other.n);
}

bool Census::operator >  (const Census & other) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled || !other.scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n > other.n);
}

bool Census::operator <= (const Census & other) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled || !other.scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n <= other.n);
}

bool Census::operator >= (const Census & other) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled || !other.scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n >= other.n);
}

bool Census::operator == (const Census & other) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled || !other.scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n == other.n);
}

bool Census::operator != (const Census & other) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled || !other.scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n != other.n);
}

bool Census::operator < (const size_t _n) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n < _n);
}

bool Census::operator > (const size_t _n) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n > _n);
}

bool Census::operator <= (const size_t _n) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n <= _n);
}

bool Census::operator >= (const size_t _n) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n >= _n);
}

bool Census::operator == (const size_t _n) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n == _n);
}

bool Census::operator != (const size_t _n) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->n != _n);
}

bool Census::operator < (const double _f) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->f < _f);
}

bool Census::operator > (const double _f) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->f > _f);
}

bool Census::operator <= (const double _f) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->f <= _f);
}

bool Census::operator >= (const double _f) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->f >= _f);
}

bool Census::operator == (const double _f) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->f == _f);
}

bool Census::operator != (const double _f) const
{
#ifdef DEBUG_CENSUS
	if (!this->scaled)
	{
		throw std::logic_error("Census was not scaled");
	}
#endif
	return (this->f != _f);
}



//
// Threshold container
//

Cutoff::Cutoff()
: Census()
, n_(false)
, f_(false)
{}

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
			
			this->n  = static_cast<size_t>(value);
			this->n_ = true;
			return;
		}
		
		// frequency
		if (value < 0)
		{
			throw std::invalid_argument("Threshold value cannot be smaller than 0");
		}
		
		this->f  = value;
		this->f_ = true;
		return;
	}
	
	throw std::invalid_argument("Cannot interpret threshold value: " + str);
}

void Cutoff::scale(const size_t _size)
{
	if (this->n_)
	{
		if (this->n > _size)
		{
			throw std::logic_error("Cannot scale frequency with size larger than count");
		}
		
		this->f = static_cast<double>(this->n) / static_cast<double>(_size);
		this->scaled = true;
		return;
	}
	
	if (this->f_)
	{
		this->n = static_cast<size_t>(round(this->f * static_cast<double>(_size)));
		this->scaled = true;
		return;
	}
	
	throw std::logic_error("Cannot scale threshold");
}



//
// Number & frequency list
//
/* template inlined in header */

