//
//  types.cpp
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "types.h"


//******************************************************************************
// Genetic data types
//******************************************************************************

//
// Chromosome container
//

Chromosome::Chromosome()
: chromosome(Chromosome::defined)
{}

Chromosome::Chromosome(const int i)
{
	if (i < 1 || i > UINT8_MAX) // undefined
	{
		throw std::range_error("Unable to define chromosome\n"
							   "Chromosome must range between 1 and " + std::to_string(UINT8_MAX));
	}
	
	this->chromosome = static_cast<uint8_t>(i);
	
	if (! Chromosome::defined_)
	{
		Chromosome::defined_ = true;
		Chromosome::defined  = this->chromosome;
		Chromosome::string   = std::to_string(this->chromosome);
	}
	
	if (this->chromosome != Chromosome::defined)
	{
		throw std::domain_error("Different chromosome detected\n"
								"Chromosome provided: " + this->str() + "\n"
								"Chromosome expected: " + Chromosome::string);
	}
}

Chromosome::Chromosome(const Chromosome & other)
: chromosome(other.chromosome)
{}

Chromosome::Chromosome(Chromosome && other)
: chromosome(other.chromosome)
{}

//Chromosome::~Chromosome()
//{}

Chromosome & Chromosome::operator = (const Chromosome & other)
{
	this->chromosome = other.chromosome;
	return *this;
}

Chromosome & Chromosome::operator = (Chromosome && other)
{
	
	this->chromosome = other.chromosome;
	return *this;
}

Chromosome::operator int () const
{
	return static_cast<int>(Chromosome::defined);
}

void Chromosome::operator = (const int i)
{
	if (i < 1 || i > UINT8_MAX) // undefined
	{
		throw std::range_error("Unable to define chromosome\n"
							   "Chromosome must range between 1 and " + std::to_string(UINT8_MAX));
	}
	
	this->chromosome = static_cast<uint8_t>(i);
	
	if (! Chromosome::defined_)
	{
		Chromosome::defined_ = true;
		Chromosome::defined  = this->chromosome;
		Chromosome::string   = std::to_string(this->chromosome);
	}
	
	if (this->chromosome != Chromosome::defined)
	{
		throw std::domain_error("Different chromosome detected\n"
								"Chromosome provided: " + this->str() + "\n"
								"Chromosome expected: " + Chromosome::string);
	}
}

std::string Chromosome::str() const
{
	return Chromosome::string;
}

uint8_t Chromosome::defined  = 0;
bool    Chromosome::defined_ = false; // initially false until defined
std::string Chromosome::string = "."; // initially empty until defined


//
// Haplotype element
//

Haplotype::Haplotype()
: h(-1)
{}

Haplotype::Haplotype(const Haplotype & other)
: h(other.h)
{}

Haplotype::Haplotype(Haplotype && other)
: h(other.h)
{}

Haplotype::Haplotype(const int i)
: h(static_cast<int8_t>(i))
{
	if (this->h < 0 || this->h > 6)
		this->h = -1;
}

Haplotype::Haplotype(const char c)
: h(static_cast<int8_t>(c - '0'))
{
	if (this->h < 0 || this->h > 6)
		this->h = -1;
}

//Haplotype::~Haplotype()
//{}

Haplotype & Haplotype::operator = (const Haplotype & other)
{
	this->h = other.h;
	return *this;
}

Haplotype & Haplotype::operator = (Haplotype && other)
{
	this->h = other.h;
	return *this;
}

Haplotype & Haplotype::operator = (const int i)
{
	this->h = (i < 0 || i > 6) ? -1: static_cast<int8_t>(i);
	return *this;
}

Haplotype & Haplotype::operator = (const char c)
{
	int i = c - '0';
	this->h = (i < 0 || i > 6) ? -1: static_cast<int8_t>(i);
	return *this;
}

bool Haplotype::operator <  (const Haplotype & other) const
{
	if (this->is_unknown())
	{
		return false;
	}
	return (this->h <  other.h);
}

bool Haplotype::operator >  (const Haplotype & other) const
{
	if (this->is_unknown())
	{
		return (other.is_unknown()) ? false: true;
	}
	return (this->h >  other.h);
}

bool Haplotype::operator == (const Haplotype & other) const
{
	return (this->h == other.h);
}

bool Haplotype::operator != (const Haplotype & other) const
{
	return (this->h != other.h);
}

Haplotype::operator int () const
{
	return (this->is_unknown()) ? -1: static_cast<int>(this->h);
}

Haplotype::operator char () const
{
	return (this->is_unknown()) ? '.': static_cast<char>(this->h + '0');
}

bool Haplotype::is_unknown() const
{
	return (this->h < 0 || this->h > 6);
}

std::string Haplotype::str() const
{
	return std::string(1, (this->is_unknown()) ? '.': static_cast<char>(this->h + '0'));
}


//
// Genotype element
//

Genotype::Genotype()
: h0()
, h1()
, phased(false)
{}

Genotype::Genotype(const Genotype & other)
: h0(other.h0)
, h1(other.h1)
, phased(other.phased)
{}

Genotype::Genotype(Genotype && other)
: h0(other.h0)
, h1(other.h1)
, phased(other.phased)
{}

Genotype::Genotype(const Haplotype & _h0, const Haplotype & _h1, const bool _phased)
: h0(_h0)
, h1(_h1)
, phased(_phased)
{}

Genotype::Genotype(Haplotype && _h0, Haplotype && _h1, const bool _phased)
: h0(_h0)
, h1(_h1)
, phased(_phased)
{}

//Genotype::~Genotype()
//{}

Genotype & Genotype::operator = (const Genotype & other)
{
	this->h0 = other.h0;
	this->h1 = other.h1;
	this->phased = other.phased;
	return *this;
}

Genotype & Genotype::operator = (Genotype && other)
{
	this->h0 = other.h0;
	this->h1 = other.h1;
	this->phased = other.phased;
	return *this;
}

bool Genotype::operator <  (const Genotype & other) const
{
	return (this->h0 == other.h0) ? (this->h1 < other.h1): (this->h0 < other.h0);
}

bool Genotype::operator >  (const Genotype & other) const
{
	return (this->h0 == other.h0) ? (this->h1 > other.h1): (this->h0 > other.h0);
}

bool Genotype::operator == (const Genotype & other) const
{
	return (this->h0 == other.h0 && this->h1 == other.h1 && this->phased == other.phased);
}

bool Genotype::operator != (const Genotype & other) const
{
	return (this->h0 != other.h0 || this->h1 != other.h1 || this->phased != other.phased);
}

std::string Genotype::str() const
{
	return std::string(this->h0.str() + ((this->phased) ? '|': '/') +  this->h1.str());
}


//
// Datatype element (Genotype compressed into one byte)
//

Datatype::Datatype()
: d(0)
{}

Datatype::Datatype(const Datatype & other)
: d(other.d)
{}

Datatype::Datatype(Datatype && other)
: d(other.d)
{}

Datatype::Datatype(const Genotype & g)
: d(mask(g))
{}

Datatype::Datatype(Genotype && g)
: d(mask(g))
{}

Datatype::Datatype(const Haplotype & h0, const Haplotype & h1, const bool phased)
: d(mask(Genotype(h0, h1, phased)))
{}

Datatype::Datatype(Haplotype && h0, Haplotype && h1, const bool phased)
: d(mask(Genotype(h0, h1, phased)))
{}

//Datatype::~Datatype()
//{}

Datatype & Datatype::operator = (const Datatype & other)
{
	this->d = other.d;
	return *this;
}

Datatype & Datatype::operator = (Datatype && other)
{
	this->d = other.d;
	return *this;
}

Datatype & Datatype::operator = (const Genotype & g)
{
	this->d = mask(g);
	return *this;
}

Datatype & Datatype::operator = (Genotype && g)
{
	this->d = mask(g);
	return *this;
}

bool Datatype::operator == (const Datatype & other) const
{
	return (this->d == other.d);
}

Datatype::operator Genotype () const
{
	return unmask(this->d);
}

Datatype::operator int () const
{
	return static_cast<int>(this->d);
}



