//
//  stream.cpp
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "stream.h"


#define DEBUG_STREAM


//******************************************************************************
// Stream file by lines
//******************************************************************************

#define READ_BUFFER_SIZE 33554431 // 32 Mb - 1 byte

StreamLine::StreamLine()
: opened(false)
, n_line(0)
, length(std::min(INT_MAX, READ_BUFFER_SIZE))
, buffer(std::min(INT_MAX, READ_BUFFER_SIZE) + 1)
, bufptr(&buffer[0])
, bufend(NULL)
, cache(1)
, use(cache.cbegin())
, end(cache.cend())
, eof(false)
{}

StreamLine::StreamLine(const std::string & filename)
: StreamLine()
{
	this->open(filename);
}

StreamLine::~StreamLine()
{
	this->close();
}

void StreamLine::gzip()
{
	int c0, c1;
	
	FILE * file = fopen(this->file.c_str(), "rb");
	if (file == NULL)
		throw std::runtime_error("Cannot open file: " + this->file);
	
	c0 = fgetc(file);
	c1 = fgetc(file);
	
	if (feof(file) || ferror(file))
		throw std::runtime_error("Cannot read file: " + this->file);
	
	// magic number
	this->cmpr = (c0 == 0x1f && c1 == 0x8b);
	
	fclose(file);
}

bool StreamLine::read()
{
	if (this->eof)
	{
		return false;
	}
	
	int n = 0;
	const size_t size = strlen(this->bufptr);
	
	// copy overhang to front of buffer
	if (size > 0 && this->bufptr != &this->buffer[0])
	{
		memmove(&this->buffer[0], this->bufptr, size);
	}
	
	// increase buffer size to fit overhang and next read
	if (size + this->length > this->buffer.size() - 1)
	{
		this->buffer.resize(size + this->length + 1);
	}
	
	this->bufptr = &this->buffer[size];
	*this->bufptr = '\0';
	
	if (this->cmpr)
	{
		// read from gzip
		n = gzread(this->stream.gz, this->bufptr, this->length);
	}
	else
	{
		// read from file
		n = (int)fread(this->bufptr, sizeof(this->buffer[0]), this->length, this->stream.fp);
	}
	
	// handle end of file
	if (n < 1)
	{
		if (strlen(&this->buffer[0]) > 0)
		{
			this->cache.push_back(&this->buffer[0]);
		}
		
		this->eof = true;
		
		return false;
	}
	
	*(this->bufptr + n) = '\0'; // terminate read
	
	this->bufptr = &this->buffer[0];
	this->bufend = strchr(this->bufptr, '\n'); // detect newline
	
	// walkabout buffer
	while (this->bufend != NULL)
	{
		this->cache.push_back(this->bufptr);
		
		*this->bufend = '\0'; // replace newline
		
		this->bufptr = this->bufend + 1;
		this->bufend = strchr(this->bufptr, '\n'); // detect newline
	}
	
	return true;
}

StreamLine::operator char * () const
{
#ifdef DEBUG_STREAM
	if (! this->opened)
	{
		throw std::runtime_error("Read stream not open");
	}
#endif
	
	return *this->use;
}

std::string StreamLine::str() const
{
#ifdef DEBUG_STREAM
	if (! this->opened)
	{
		throw std::runtime_error("Read stream not open");
	}
#endif
	
	return std::string(*this->use);
}

size_t StreamLine::count() const
{
#ifdef DEBUG_STREAM
	if (! this->opened)
	{
		throw std::runtime_error("Read stream not open");
	}
#endif
	
	return this->n_line;
}

std::string StreamLine::source() const
{
#ifdef DEBUG_STREAM
	if (! this->opened)
	{
		throw std::runtime_error("Read stream not open");
	}
#endif
	
	return this->file;
}

void StreamLine::open(const std::string & filename)
{
#ifdef DEBUG_STREAM
	if (this->opened)
	{
		throw std::runtime_error("Read stream already open");
	}
#endif
	
	this->file = filename;
	
	// determine file type (text or compressed/binary)
	this->gzip();
	
	// open file
	if (this->cmpr)
	{
		if ((this->stream.gz = gzopen(this->file.c_str(), "rb")) == NULL)
			throw std::runtime_error("Cannot open compressed file: " + this->file);
	}
	else
	{
		if ((this->stream.fp = fopen(this->file.c_str(), "r")) == NULL)
			throw std::runtime_error("Cannot open file: " + this->file);
	}
	
	this->opened = true;
}

void StreamLine::close()
{
	if (this->opened)
	{
		if (this->cmpr)
			gzclose(this->stream.gz);
		else
			fclose(this->stream.fp);
		
		this->opened = false;
	}
	
	this->buffer.clear();
	this->cache.clear();
	this->use = this->cache.cend();
	this->end = this->cache.cend();
}

void StreamLine::reset()
{
#ifdef DEBUG_STREAM
	if (! this->opened)
	{
		throw std::runtime_error("Read stream not open");
	}
#endif
	
	if (this->cmpr)
	{
		if (gzrewind(this->stream.gz) != 0)
			throw std::runtime_error("Exception while handling compressed file: " + this->file);
	}
	else
	{
		if (fseek(this->stream.fp, 0, SEEK_SET) != 0)
			throw std::runtime_error("Exception while handling file: " + this->file);
	}
	
	this->n_line = 0;
	this->cache = std::vector<char*>(1);
	this->use = this->cache.cbegin();
	this->end = this->cache.cend();
}

bool StreamLine::next()
{
#ifdef DEBUG_STREAM
	if (! this->opened)
	{
		throw std::runtime_error("Read stream not open");
	}
#endif
	
	++this->use;
	
	if (this->use == this->end)
	{
		this->cache.clear();
		
		while (this->read())
		{
			if (this->cache.size() > 0)
			{
				this->use = this->cache.cbegin();
				this->end = this->cache.cend();
				
				break;
			}
		}
		
		if (this->cache.size() > 0)
		{
			this->use = this->cache.cbegin();
			this->end = this->cache.cend();
		}
		else
		{
			return false;
		}
	}
	
	++this->n_line;
	
	return true;
}



//******************************************************************************
// Split line into tokens
//******************************************************************************
StreamSplit::StreamSplit(char * _use, const char * _del)
: del(strlen(_del) + 1)
, ptr(_use)
, beg(_use)
, end(NULL)
, n(0)
{
	strcpy(&this->del[0], _del);
}

StreamSplit::StreamSplit(std::string & _use, const char * _del)
: del(strlen(_del) + 1)
, ptr(&_use[0])
, beg(&_use[0])
, end(NULL)
, n(0)
{
	strcpy(&this->del[0], _del);
}

bool StreamSplit::next()
{
	// skip leading delimiters
	if (this->beg != NULL)
	{
		while (*this->beg != '\0' && strchr(&this->del[0], *this->beg) != NULL)
		{
			++this->beg;
		}
		
		if (*this->beg == '\0')
		{
			return false;
		}
	}
	
	if (this->beg == NULL)
	{
		return false;
	}
	
	this->ptr = this->beg; // point to begin
	
	this->end = strpbrk(this->beg, &this->del[0]); // detect delimiter
	
	if (this->end != NULL)
	{
		*this->end = '\0'; // terminate token
		
		this->beg = this->end + 1; // set begin
	}
	else
	{
		this->beg = NULL; // indicate exit
	}
	
	++this->n; // count token
	
	return true;
}

StreamSplit::operator char * () const
{
	return this->ptr;
}

bool StreamSplit::convert(int & i)
{
	char * p = this->ptr;
	
	if (*p == '\0')
		return false;
	
	// accept leading minus
	if (*p == '-')
	{
		++p;
		
		if (*p == '\0')
			return false;
	}
	
	while (*p != '\0')
	{
		if (*p < '0' || *p > '9')
			return false;
		
		++p;
	}
	
	i = atoi(this->ptr);
	
	return true;
}

bool StreamSplit::convert(size_t & i)
{
	char * p = this->ptr;
	
	if (*p == '\0')
		return false;
	
	while (*p != '\0')
	{
		if (*p < '0' || *p > '9')
			return false;
		
		++p;
	}
	
	i = (size_t)atol(this->ptr);
	
	return true;
}

bool StreamSplit::convert(double & f)
{
	char * p = this->ptr;
	
	if (*p == '\0')
		return false;
	
	// accept leading minus
	if (*p == '-')
	{
		++p;
		
		if (*p == '\0')
			return false;
	}
	
	while (*p != '\0')
	{
		if ((*p < '0' || *p > '9') && *p != '.')
			return false;
		
		++p;
	}
	
	f = atof(this->ptr);
	
	return true;
}

std::string StreamSplit::str() const
{
	return std::string(this->ptr);
}

size_t StreamSplit::count() const
{
	return this->n;
}

size_t StreamSplit::size() const
{
	return strlen(this->ptr);
}

const char * StreamSplit::def = " \t"; // whitespace



//******************************************************************************
// Write to file
//******************************************************************************

StreamOut::StreamOut()
: fp(nullptr)
, good(false)
{}

StreamOut::StreamOut(const std::string & filename)
: good(true)
, name(filename)
{
	if ((this->fp = fopen(filename.c_str(), "w")) == NULL)
	{
		throw std::runtime_error("Cannot create output file: " + this->name);
	}
}

StreamOut::~StreamOut()
{
	this->close();
}

void StreamOut::open(const std::string & filename)
{
#ifdef DEBUG_STREAM
	if (this->good)
	{
		throw std::runtime_error("Write stream already open");
	}
#endif
	
	if ((this->fp = fopen(filename.c_str(), "w")) == NULL)
	{
		throw std::runtime_error("Cannot create output file: " + this->name);
	}
	
	this->name = filename;
	this->good = true;
}

void StreamOut::close()
{
	if (this->good)
		fclose(this->fp);
}

StreamOut::operator FILE * () const
{
#ifdef DEBUG_STREAM
	if (! this->good)
	{
		throw std::runtime_error("Write stream not open");
	}
#endif
	
	return this->fp;
}

void StreamOut::line(const std::string & str, const char last) const
{
#ifdef DEBUG_STREAM
	if (! this->good)
	{
		throw std::runtime_error("Write stream not open");
	}
#endif
	
	fprintf(this->fp, "%s%c", str.c_str(), last);
}



//******************************************************************************
// Redirect log & err streams
//******************************************************************************

StreamLogErr::StreamLogErr(const std::string & prefix)
: flog(prefix + ".log")
, ferr(prefix + ".err")
{
	this->clog = std::clog.rdbuf();
	this->cerr = std::cerr.rdbuf();
	
	std::clog.rdbuf(this->flog.rdbuf());
	std::cerr.rdbuf(this->ferr.rdbuf());
}

StreamLogErr::~StreamLogErr()
{
	std::clog.rdbuf(this->clog);
	std::cerr.rdbuf(this->cerr);
	
	this->flog.close();
	this->ferr.close();
}


