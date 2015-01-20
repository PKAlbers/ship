//
//  types.hpp
//  ship
//
//  Created by Patrick Albers on 12.12.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef ship_types_hpp
#define ship_types_hpp

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
	
	static const char unknown = 0; // unknown chromosome
	
	// check if unknown
	bool is_unknown() const
	{
		return (this->value == Chromosome::unknown);
	}
	
	// cast int
	operator int () const
	{
		return static_cast<int>(this->value);
	}
	
	// compare/sort
	bool operator <  (const Chromosome & other) const { return (this->value <  other.value); }
	bool operator >  (const Chromosome & other) const { return (this->value >  other.value); }
	bool operator == (const Chromosome & other) const { return (this->value == other.value); }
	bool operator != (const Chromosome & other) const { return (this->value != other.value); }
	
	// match chromosome with unknown being ambiguous
	bool match(const Chromosome & other) const
	{
		if (this->value == Chromosome::unknown ||
			other.value == Chromosome::unknown)
		{
			return true;
		}
		
		return (this->value == other.value);
	}
	
	// convert to string
	std::string str() const
	{
		if (this->value == Chromosome::unknown)
		{
			return std::string(".");
		}
		
		return std::to_string(this->value);
	}
	
	// assign
	Chromosome & operator = (const Chromosome & other)
	{
		this->value = other.value;
		return *this;
	}
	Chromosome & operator = (const int i)
	{
		this->value = (i < 1 || i > CHAR_MAX)? Chromosome::unknown: i; // reserve -1 for unknown
		return *this;
	}
	
	// construct
	Chromosome()
	: value(Chromosome::unknown) // unknown
	{}
	Chromosome(const int i)
	{
		this->value = (i < 1 || i > CHAR_MAX)? Chromosome::unknown: i; // reserve -1 for unknown
	}
	
	friend class std::hash<Chromosome>;
};


//
// Haplotype element
//

#define HAPLOTYPE_MAX 14 // = 15 (00001111) - 1 ('15' reserved for unknown)

class Haplotype
{
private:
	
	unsigned char value;
	
public:
	
	static const unsigned char unknown = HAPLOTYPE_MAX + 1; // unknown haplotype
	
	// check if value is unknown
	bool is_unknown() const
	{
		return (this->value == Haplotype::unknown);
	}
	
	// cast int
	operator int () const
	{
		return static_cast<int>(this->value);
	}
	
	// compare/sort
	bool operator <  (const Haplotype & other) const { return (this->value <  other.value); }
	bool operator >  (const Haplotype & other) const { return (this->value >  other.value); }
	bool operator == (const Haplotype & other) const { return (this->value == other.value); }
	bool operator != (const Haplotype & other) const { return (this->value != other.value); }
	
	// convert to string
	std::string str() const
	{
		if (this->value == Haplotype::unknown)
		{
			return std::string(".");
		}
		
		return std::to_string(this->value);
	}
	
	// assign
	Haplotype & operator = (const Haplotype & other)
	{
		this->value = other.value;
		return *this;
	}
	Haplotype & operator = (const int i)
	{
		this->value = (i < 0 || i > HAPLOTYPE_MAX) ? Haplotype::unknown: i; // reserve 15 (00001111) for unknown
		return *this;
	}
	
	// construct
	Haplotype()
	: value(Haplotype::unknown) // unknown
	{}
	Haplotype(const int i)
	{
		this->value = (i < 0 || i > HAPLOTYPE_MAX) ? Haplotype::unknown: i; // reserve 15 (00001111) for unknown
	}
	
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
	bool operator <  (const Genotype & other) const { return (this->h0 == other.h0) ? (this->h1 <  other.h1): (this->h0 <  other.h0); }
	bool operator >  (const Genotype & other) const { return (this->h0 == other.h0) ? (this->h1 >  other.h1): (this->h0 >  other.h0); }
	bool operator == (const Genotype & other) const { return (this->h0 == other.h0  && this->h1 == other.h1); }
	bool operator != (const Genotype & other) const { return (this->h0 != other.h0  || this->h1 != other.h1); }
	
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
	Datatype()
	: value(Haplotype::unknown << 4 | Haplotype::unknown)
	{}
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
	template <> struct hash<Chromosome>
	{
		size_t operator () (const Chromosome & _c) const
		{
			return hash<int>()(_c.value);
		}
	};
	
	template <> struct hash<Haplotype>
	{
		size_t operator () (const Haplotype & _h) const
		{
			return hash<int>()(_h.value);
		}
	};
	
	template <> struct hash<Genotype>
	{
		size_t operator () (const Genotype & _g) const
		{
			return hash<int>()(_g.h0.value << 4 | _g.h1.value);
		}
	};
	
	template <> struct hash<Datatype>
	{
		size_t operator () (const Datatype & _d) const
		{
			return hash<int>()(_d.value);
		}
	};
}



#endif
