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
	bool n_, f_; // flag that number/frequency was provided
	
public:
	
	// in/decrement
	Census & operator ++ ();
	Census & operator -- ();
	
	// scale number/frequency given size
	void scale(const unsigned long);
	
	// check if scaled
	bool scaled() const;
	
	// assign values
	void operator () (const unsigned long _n); // number
	void operator () (const double _f);        // frequency
	
	// cast
	explicit operator unsigned long () const;
	explicit operator double        () const;
	
	// compare
	bool operator <  (const Census & other) const;
	bool operator >  (const Census & other) const;
	bool operator <= (const Census & other) const;
	bool operator >= (const Census & other) const;
	bool operator == (const Census & other) const;
	bool operator != (const Census & other) const;
	
	// assign
	Census & operator = (const Census &);
	Census & operator = (Census &&);
	
	// construct
	Census();
	Census(const Census &);
	Census(Census &&);
	
	// destruct
	//~Census();
};


//
// Commandline threshold container
//
struct Cutoff : public Census
{
	// parse command line value
	void parse(const std::string & str);
};



#endif /* defined(__ship__census__) */
