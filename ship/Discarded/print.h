//
//  print.h
//  ship
//
//  Created by Patrick Albers on 24.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship___print__
#define __ship___print__


#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>


//******************************************************************************
// Print output to cout and other file stream simultaneously
//******************************************************************************
struct Print //: std::ostream
{
	std::ostream * os;
	std::ofstream ofs;
	bool good;
	
	// provide standard stream and file stream
	void file(std::ostream & stdostream, const std::string filename);
	
	/*
	// stream type
	template <typename T>
	std::ostream & operator << (T & val);
	
	// stream functions
	std::ostream & operator << (std::streambuf * sb);
	std::ostream & operator << (std::ostream & (* pf)(std::ostream&));
	std::ostream & operator << (std::ios& (* pf)(std::ios&));
	std::ostream & operator << (std::ios_base& (* pf)(std::ios_base&));
	*/
	 
	// construct
	Print();
};


// declare in standard
namespace std
{
	extern Print coutlog;
	extern Print couterr;
}


#endif /* defined(__ship___print__) */
