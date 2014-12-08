//
//  print.cpp
//  ship
//
//  Created by Patrick Albers on 24.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "print.h"


//******************************************************************************
// Print output to cout and other file stream simultaneously
//******************************************************************************

void Print::file(std::ostream & stdostream, const std::string filename)
{
	this->ofs.open(filename); // open stream to filename
	
	// redirect stream to file stream
	if (this->ofs.is_open())
	{
		stdostream.rdbuf(this->ofs.rdbuf());
		this->good = true;
	}
	
	// remember provided stream
	this->os = &stdostream;
}

/*
template <typename T>
std::ostream & Print::operator << (T & val)
{
	std::cout << val;
	
	if (this->good)
	{
		*(this->os) << val;
	}
	
	return std::cout;
}

std::ostream & Print::operator << (std::streambuf * sb)
{
	std::cout << sb;
	
	if (this->good)
	{
		*(this->os) << sb;
	}
	
	return std::cout;
}

std::ostream & Print::operator << (std::ostream & (* pf)(std::ostream&))
{
	std::cout << pf;
	
	if (this->good)
	{
		*(this->os) << pf;
	}
	
	return std::cout;
}

std::ostream & Print::operator << (std::ios& (* pf)(std::ios&))
{
	std::cout << pf;
	
	if (this->good)
	{
		*(this->os) << pf;
	}
	
	return std::cout;
}

std::ostream & Print::operator << (std::ios_base& (* pf)(std::ios_base&))
{
	std::cout << pf;
	
	if (this->good)
	{
		*(this->os) << pf;
	}
	
	return std::cout;
}
*/


template <typename T>
std::ostream & operator << (Print & p, T & val)
{
	std::cout << val;
	
	if (p.good)
	{
		*(p.os) << val;
	}
	
	return std::cout;
}

std::ostream & operator << (Print & p, std::streambuf * sb)
{
	std::cout << sb;
	
	if (p.good)
	{
		*(p.os) << sb;
	}
	
	return std::cout;
}

std::ostream & operator << (Print & p, std::ostream & (* pf)(std::ostream&))
{
	std::cout << pf;
	
	if (p.good)
	{
		*(p.os) << pf;
	}
	
	return std::cout;
}

std::ostream & operator << (Print & p, std::ios& (* pf)(std::ios&))
{
	std::cout << pf;
	
	if (p.good)
	{
		*(p.os) << pf;
	}
	
	return std::cout;
}

std::ostream & operator << (Print & p, std::ios_base& (* pf)(std::ios_base&))
{
	std::cout << pf;
	
	if (p.good)
	{
		*(p.os) << pf;
	}
	
	return std::cout;
}


// construct
Print::Print()
: good(false)
{}


// declare in standard
namespace std
{
	Print coutlog;
	Print couterr;
}


