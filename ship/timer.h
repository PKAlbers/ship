//
//  timer.h
//  ship
//
//  Created by Patrick Albers on 17.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship__timer__
#define __ship__timer__

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>


//******************************************************************************
// Current time
//******************************************************************************
std::string timestamp(const bool = false);


//******************************************************************************
// Elapsed time clocks
//******************************************************************************
class Runtime
{
private:
	const clock_t start_comp; // start measuring CPU time
	const time_t  start_real; // start measuring real time
	
public:
	
	// calculate elapsed clock units
	unsigned long long raw() const;
	
	// calculate elapsed CPU seconds
	size_t comp() const;
	
	// calculate elapsed real seconds
	size_t real() const;
	
	// convert elapsed CPU time to string
	std::string str_comp() const;
	
	// convert elapsed real time to string
	std::string str_real() const;
	
	// convert both to string
	std::string str() const;
	
	// construct
	Runtime();
	
	// do not copy
	Runtime(const Runtime &) = delete;
	Runtime & operator = (const Runtime &) = delete;
};


//******************************************************************************
// Progress status
//******************************************************************************

//
// Progress bar - required to know target count
//
class ProgressBar
{
private:
	
	const bool   log; // flag that output log static progress bar
	const size_t target;  // target number of updates
	const double interval_dynamic; // cout interval
	const double interval_static;  // clog interval
	
	size_t i; // count of updates
	double t_dynamic, t_static; // next threshold from calculated rate
	
	static const unsigned int size; // width of bar
	static const unsigned int rate; // refresh rate
	static const char fill; // bar fill symbol
	static const char rest; // bar rest symbol
	
public:
	
	// update progress
	void update(const size_t = 1);
	
	// finish progress
	void finish() const;
	
	// construct
	ProgressBar(const size_t, const bool = true);
	
	// do not copy
	ProgressBar(const ProgressBar &) = delete;
	ProgressBar & operator = (const ProgressBar &) = delete;
};


//
// Progress message - target count is unknown
//
class ProgressMsg
{
private:
	
	const std::chrono::steady_clock::time_point time; // start time
	const std::string unit; // unit of update
	const size_t      rate; // update rate, every i'th update
	char *            line; // line for message printing
	
	int    l; // longest length of line
	size_t i; // count updates
	double t; // last time checkpoint
	
	static const double interval; // print interval
	static const int    size; // max size of line message
	
	std::ostream * stream_;
	
public:
	
	// update progress
	void update(const size_t = 1);
	
	// finish progress
	void finish(const size_t = 0) const;
	
	// construct/destruct
	ProgressMsg(const std::string, const size_t = 11, std::ostream * = &std::cout);
	~ProgressMsg();
	
	// do not copy
	ProgressMsg(const ProgressMsg &) = delete;
	ProgressMsg & operator = (const ProgressMsg &) = delete;
};



#endif /* defined(__ship__timer__) */
