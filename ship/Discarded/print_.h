//
//  print.h
//  ship
//
//  Created by Patrick Albers on 19.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef ship_print_h
#define ship_print_h

#include <iostream>
#include <fstream>
#include <string>


//******************************************************************************
// Print output to cout and other file stream simultaneously
//******************************************************************************
struct Print : std::ostream
{
	std::ostream * os;
	std::ofstream ofs;
	bool good;
	
	// provide standard stream and file stream
	void file(std::ostream & stdostream, const std::string filename)
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
	
	// stream type
	template <typename T>
	std::ostream & operator << (T & val)
	{
		std::cout << val;
		
		if (this->good)
		{
			*(this->os) << val;
		}
		
		return std::cout;
	}
	
	std::ostream & operator << (std::streambuf * sb)
	{
		std::cout << sb;
		
		if (this->good)
		{
			*(this->os) << sb;
		}
		
		return std::cout;
	}
	
	// stream function
	std::ostream & operator << (std::ostream & (* pf)(std::ostream&))
	{
		std::cout << pf;
		
		if (this->good)
		{
			*(this->os) << pf;
		}
		
		return std::cout;
	}
	
	std::ostream & operator << (std::ios& (* pf)(std::ios&))
	{
		std::cout << pf;
		
		if (this->good)
		{
			*(this->os) << pf;
		}
		
		return std::cout;
	}
	
	std::ostream & operator << (std::ios_base& (* pf)(std::ios_base&))
	{
		std::cout << pf;
		
		if (this->good)
		{
			*(this->os) << pf;
		}
		
		return std::cout;
	}
	
	// construct
	Print()
	: good(false)
	{}
};


// declare in standard
namespace std
{
	extern Print coutlog;
	extern Print couterr;
}


#endif
