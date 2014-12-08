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
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <zlib.h>


//******************************************************************************
// Reading files by line
//******************************************************************************

//
// Stream file lines
//
class StreamLine
{
private:
	
	FILE * file_text_stream; // stream for uncompressed, i.e. text file
	gzFile file_gzip_stream; // stream for compressed, i.e. binary file
	char * buff;             // buffer for chunks
	bool good_;              // flag that stream was opened
	bool compressed_;        // flag that reading from compressed file
	
	// determine file type, text or compressed/binary
	void compressed(const std::string &);
	
	// read next chunk from file
	bool chunk();
	
public:
	
	std::string name;    // file name
	char * line;         // line buffer
	unsigned long count; // current line count
	
	// open stream
	void open(const std::string &);
	
	// check if opened
	bool is_open() const;
	
	// close stream
	void close();
	
	// read next line from stream
	bool read();
	
	// return to start of file after reading
	void reset();
	
	// count lines in file
	unsigned long count_lines();
	
	// count fields in first line
	unsigned long count_fields();
	
	// error message
	std::string error(const std::string, const bool = false, const bool = false, const bool = false) const;
	
	// construct / destruct
	StreamLine();
	StreamLine(const std::string & filename);
	~StreamLine();
	
	// do not copy
	StreamLine(const StreamLine &) = delete;
	StreamLine & operator = (const StreamLine &) = delete;
};


//
// Parse line into tokens
//
class ParseLine
{
private:
	
	const char * l; // line char array
	size_t i; // position on array
	size_t n; // max size of token
	
	// custom seperator
	const char sep;  // seperate when char is encountered
	const bool sep_; // flag that seperator was defined
	
public:
	
	char * token; // extracted chars
	size_t width; // number of chars
	size_t count; // count of tokens
	
	// parse next token
	bool next();
	
	// parse next token
	bool next(const char);
	
	// check token contains only numeric chars (interger/float)
	bool is_numeric() const;
	
	// convert to type
	template<typename T>
	explicit operator T () const;
	
	// construct/destruct
	ParseLine(const char *);
	ParseLine(const char *, const char);
	~ParseLine();
	
	// do not copy
	ParseLine(const ParseLine &) = delete;
	ParseLine & operator = (const ParseLine &) = delete;
};



#endif /* defined(__ship__stream__) */
