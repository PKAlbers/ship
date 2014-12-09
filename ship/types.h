//
//  types.h
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship__types__
#define __ship__types__

#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <string>
#include <vector>
#include <utility>
#include <functional>
#include <type_traits>


//******************************************************************************
// Genetic data types
//******************************************************************************

class Chromosome;
class Haplotype;
class Genotype;
class Datatype;


//
// Chromosome number
//
class Chromosome
{
private:
	
	char value;
	
public:
	
	// check if unknown
	bool is_unknown() const
	{
		return (this->value == -1);
	}
	
	// cast
	operator int () const
	{
		return static_cast<int>(this->value);
	}
	
	// compare
	bool operator == (const Chromosome & other)
	{
		return (this->value == other.value);
	}
	bool operator != (const Chromosome & other)
	{
		return (this->value != other.value);
	}
	
	// assign
	Chromosome & operator = (const Chromosome & other)
	{
		this->value = other.value;
		return *this;
	}
	Chromosome & operator = (const int i)
	{
		this->value = (i < 1 || i > CHAR_MAX)? -1: i; // reserve -1 for unknown
		return *this;
	}
	
	// construct
	Chromosome()
	: value(0) // unknown
	{}
	Chromosome(const int i)
	{
		this->value = (i < 1 || i > CHAR_MAX)? -1: i; // reserve -1 for unknown
	}
	
	friend class std::hash<Chromosome>;
};


//
// Haplotype element
//

#define HAPLOTYPE_MAX 14 // = 15 (00001111) - 1 (reserved for unknown)

class Haplotype
{
private:
	
	unsigned char value;
	
public:
	
	static const unsigned char unknown = HAPLOTYPE_MAX + 1; // unknown haplotype
	
	bool is_unknown() const
	{
		return (this->value == HAPLOTYPE_MAX + 1);
	}
	
	// cast
	operator int () const
	{
		return (this->value == HAPLOTYPE_MAX + 1) ? -1: static_cast<int>(this->value);
	}
	
	// compare/sort
	bool operator <  (const Haplotype & other) { return (this->value <  other.value); }
	bool operator >  (const Haplotype & other) { return (this->value >  other.value); }
	bool operator == (const Haplotype & other) { return (this->value == other.value); }
	bool operator != (const Haplotype & other) { return (this->value != other.value); }
	
	
	// assign
	Haplotype & operator = (const Haplotype & other)
	{
		this->value = other.value;
		return *this;
	}
	Haplotype & operator = (const int i)
	{
		this->value = (i < 0 || i > HAPLOTYPE_MAX) ? HAPLOTYPE_MAX + 1: i; // reserve 15 (00001111) for unknown
		return *this;
	}
	
	// construct
	Haplotype()
	: value(0) // unknown
	{}
	Haplotype(const int i)
	{
		this->value = (i < 0 || i > HAPLOTYPE_MAX) ? HAPLOTYPE_MAX + 1: i; // reserve 15 (00001111) for unknown
	}
	
	friend class Genotype;
	friend class Datatype;
	friend class std::hash<Haplotype>;
	friend class std::hash<Genotype>;
};


//
// Genotype element
//
class Genotype
{
public:
	
	Haplotype h0; // 1st haplotype
	Haplotype h1; // 2nd haplotype
	
	// compare/sort
	bool operator <  (const Genotype & other) { return (this->h0 == other.h0) ? (this->h1 <  other.h1): (this->h0 <  other.h0); }
	bool operator >  (const Genotype & other) { return (this->h0 == other.h0) ? (this->h1 >  other.h1): (this->h0 >  other.h0); }
	bool operator == (const Genotype & other) { return (this->h0 == other.h0  && this->h1 == other.h1); }
	bool operator != (const Genotype & other) { return (this->h0 != other.h0  || this->h1 != other.h1); }
	
	// construct
	Genotype()
	{}
	Genotype(const Haplotype & _h0, const Haplotype & _h1)
	: h0(_h0)
	, h1(_h1)
	{}
};


//
// Datatype element (Genotype compressed into one byte)
//
class Datatype
{
private:
	
	unsigned char value;
	
public:
	
	// cast genotype
	operator Genotype () const
	{
		Genotype g;
		g.h0.value = (this->value & 0b11110000) >> 4;
		g.h1.value = (this->value & 0b00001111);
		return g;
	}
	
	// return genotype
	Genotype operator () ()
	{
		return static_cast<Genotype>(*this);
	}
	
	// assign
	Datatype & operator = (const Datatype & other)
	{
		this->value = other.value;
		return *this;
	}
	Datatype & operator = (const Genotype & _g)
	{
		this->value = _g.h0.value << 4 | _g.h1.value;
		return *this;
	}
	
	// construct
	Datatype();
	Datatype(const Genotype & _g)
	: value(_g.h0.value << 4 | _g.h1.value)
	{}
	Datatype(const Haplotype & _h0, const Haplotype & _h1)
	: value(_h0.value << 4 | _h1.value)
	{}
	
	friend class std::hash<Datatype>;
};


//
// hash types
//
namespace std
{
	template <>
	struct hash<Chromosome>
	{
		size_t operator () (const Chromosome & _c) const
		{
			return hash<int>()(_c.value);
		}
	};
	
	template <>
	struct hash<Haplotype>
	{
		size_t operator () (const Haplotype & _h) const
		{
			return hash<int>()(_h.value);
		}
	};
	
	template <>
	struct hash<Genotype>
	{
		size_t operator () (const Genotype & _g) const
		{
			return hash<int>()(_g.h0.value << 4 | _g.h1.value);
		}
	};
	
	template <>
	struct hash<Datatype>
	{
		size_t operator () (const Datatype & _d) const
		{
			return hash<int>()(_d.value);
		}
	};
}



#endif /* defined(__ship__types__) */
