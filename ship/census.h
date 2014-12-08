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
#include <sstream>
#include <stdexcept>


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
	bool scaled_; // flag that number was scaled to yield frequency
	
public:
	
	// in/decrement
	Census & operator ++ ();
	Census & operator -- ();
	
	// scale number/frequency given size
	void scale(const size_t);
	
	// check if scaled
	bool scaled() const;
	
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
	Census(const Census &);
	Census(Census &&);
};


//
// Commandline threshold container
//
struct Cutoff : public Census
{
	// parse command line value
	void parse(const std::string & str);
	
	// scale using sample size
	void size(const size_t);
};



#endif /* defined(__ship__census__) */
