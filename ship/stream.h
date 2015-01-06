//
//  stream.h
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship__stream__
#define __ship__stream__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <zlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>


//******************************************************************************
// Stream file by lines
//******************************************************************************
class StreamLine
{
private:
	
	union Stream
	{
		FILE * fp; // stream for uncompressed, i.e. text file
		gzFile gz; // stream for compressed, i.e. binary file
	};
	
	Stream      stream; // file/gzip stream
	bool        opened; // flag that stream was opened
	size_t      n_line; // line count
	const int   length; // buffer read length
	std::vector<char> buffer; // read line buffer
	char * bufptr;
	char * bufend;
	std::vector<char*> cache;
	std::vector<char*>::const_iterator use, end;
	bool eof;
	
	std::string file;   // source file
	bool        cmpr;   // flag that file is gzip compressed
	
	// detect gzip compression
	void gzip();
	
	// read line into buffer
	bool read();
	
public:
	
	// return line pointer
	operator char * () const;
	
	// return line as string
	std::string str() const;
	
	// return line count
	size_t count() const;
	
	// return file name
	std::string source() const;
	
	// open stream
	void open(const std::string &);
	
	// close stream
	void close();
	
	// reset stream
	void reset();
	
	// forward to next line
	bool next();
	
	// construct
	StreamLine();
	StreamLine(const std::string &);
	
	// destruct
	~StreamLine();
	
	// do not copy
	StreamLine(const StreamLine &) = delete;
	StreamLine & operator = (const StreamLine &) = delete;
};





//******************************************************************************
// Split line into tokens
//******************************************************************************
class StreamSplit
{
private:
	
	std::vector<char> del; // delimiter chars
	char *ptr, * beg, * end;
	size_t n;
	
	static const char * def; // default delimiters
	
public:
	
	// return token pointer
	operator char * () const;
	
	// convert to type
	bool convert(int &);
	bool convert(size_t &);
	bool convert(double &);
	
	// return token as string
	std::string str() const;
	
	// return token count
	size_t count() const;
	
	// return token length
	size_t size() const;
	
	// forward to next token, optionally break at seperator
	bool next();
	
	// construct
	StreamSplit(char *, const char * = StreamSplit::def);
	StreamSplit(std::string &, const char * = StreamSplit::def);
	
	// destruct
	//~StreamSplit();
	
	// do not copy
	StreamSplit(const StreamSplit &) = delete;
	StreamSplit & operator = (const StreamSplit &) = delete;
};



//******************************************************************************
// Write to file
//******************************************************************************
class StreamOut
{
private:
	
	FILE * fp; // file pointer
	bool good; // flag that file was opened
	
public:
	
	std::string name; // file name
	
	// cast file pointer
	operator FILE * () const;
	
	// write string as line
	void line(const std::string &, const char = '\n') const;
	
	// open stream
	void open(const std::string &);
	
	// close stream
	void close();
	
	// construct/destruct
	StreamOut();
	StreamOut(const std::string &);
	~StreamOut();
	
	// do not copy
	StreamOut(const StreamOut &) = delete;
	StreamOut & operator = (const StreamOut &) = delete;
};



//******************************************************************************
// Redirect log & err streams
//******************************************************************************
class StreamLogErr
{
private:
	
	std::ofstream flog;
	std::ofstream ferr;
	
	std::streambuf * clog;
	std::streambuf * cerr;
	
public:
	
	// construct/destruct
	StreamLogErr(const std::string &);
	~StreamLogErr();
};


#endif /* defined(__ship__stream__) */
