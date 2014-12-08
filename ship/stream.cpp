//
//  stream.cpp
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "stream.h"


#define BUFFER_SIZE_LINE  33554432 // 32 Mb


//******************************************************************************
// Reading files by line
//******************************************************************************

//
// Stream file lines
//

StreamLine::StreamLine()
: good_(false)
, count(0)
{}

StreamLine::StreamLine(const std::string & filename)
: good_(false)
, count(0)
{
	this->open(filename);
}

StreamLine::~StreamLine()
{
	this->close();
}

void StreamLine::compressed(const std::string & filename)
{
	int c0, c1;
	
	FILE * file = fopen(filename.c_str(), "rb");
	if (file == NULL)
		throw std::runtime_error("Cannot open file: " + filename);
	
	c0 = fgetc(file);
	c1 = fgetc(file);
	
	if (feof(file) || ferror(file))
		throw std::runtime_error("Cannot read file: " + filename);
	
	// magic number
	this->compressed_ = (c0 == 0x1f && c1 == 0x8b) ? true : false;
	
	fclose(file);
}

bool StreamLine::chunk()
{
	this->buff[0] = '\0'; // reset buffer
	
	if (this->compressed_)
	{
		if (gzgets(this->file_gzip_stream, this->buff, INT_MAX) == NULL)
		{
			if (gzeof(this->file_gzip_stream))
				return false;
			
			throw std::runtime_error("Failed to read line from compressed file: " + this->name);
		}
	}
	else
	{
		if (fgets(this->buff, INT_MAX, this->file_text_stream) == NULL)
		{
			if (feof(this->file_text_stream))
				return false;
			
			throw std::runtime_error("Failed to read line from file: " + this->name);
		}
	}
	
	return true;
}

void StreamLine::open(const std::string & filename)
{
	if (this->good_) return;
	
	this->name  = filename;
	this->good_ = true;
	
	// allocate line and buffer size
	try
	{
		this->line = new char[BUFFER_SIZE_LINE];
		this->buff = new char[INT_MAX];
	}
	catch (std::bad_alloc &)
	{
		throw std::runtime_error("Cannot allocate memory");
	}
	
	// determine file type (text or compressed/binary)
	this->compressed(filename);
	
	// open file
	if (this->compressed_)
	{
		if ((this->file_gzip_stream = gzopen(filename.c_str(), "rb")) == NULL)
			throw std::runtime_error("Cannot open compressed input file: " + this->name);
	}
	else
	{
		if ((this->file_text_stream = fopen(filename.c_str(), "r")) == NULL)
			throw std::runtime_error("Cannot open input file: " + this->name);
	}
}

bool StreamLine::is_open() const
{
	return this->good_;
}

void StreamLine::close()
{
	if (this->good_)
	{
		delete [] this->line;
		delete [] this->buff;
		
		if (this->compressed_)
			gzclose(this->file_gzip_stream);
		else
			fclose(this->file_text_stream);
		
		this->good_ = false;
	}
}

bool StreamLine::read()
{
	if (! this->good_) return false;
	
	if ((this->compressed_ && gzeof(this->file_gzip_stream)) ||
		(!this->compressed_ && feof(this->file_text_stream)))
		return false;
	
	this->line[0] = '\0'; // reset line
	
	do
	{
		if (! this->chunk()) // read chunk from file
			break;
		
		strcat(this->line, this->buff); // append chunk to line
	}
	while (this->buff[ (strlen(this->buff) - 1) ] != '\n');
	// read chunks until newline encountered
	
	if (this->line[0] == '\0')
		return false;
	
	// remove trailing newline char
	size_t len = strlen(this->line) - 1;
	if (this->line[len] == '\n')
		this->line[len] =  '\0';
	
	++this->count;
	
	return true;
}

void StreamLine::reset()
{
	if (! this->good_) return;
	
	if (this->compressed_)
	{
		if (gzrewind(this->file_gzip_stream) != 0)
			throw std::runtime_error("Exception while handling compressed file: " + this->name);
	}
	else
	{
		if (fseek(this->file_text_stream, 0, SEEK_SET) != 0)
			throw std::runtime_error("Exception while handling file: " + this->name);
	}
	
	this->count = 0;
}

unsigned long StreamLine::count_lines()
{
	if (! this->good_) return 0;
	
	unsigned long n = 0;
	bool flag;
	
	if ((this->compressed_ && gzeof(this->file_gzip_stream)) ||
		(!this->compressed_ && feof(this->file_text_stream)))
		this->reset();
	
	while (true)
	{
		flag = false;
		
		if ((this->compressed_ && gzeof(this->file_gzip_stream)) ||
			(!this->compressed_ && feof(this->file_text_stream)))
			break;
		
		do
		{
			if (! this->chunk()) // read chunk from file
				break;
			
			flag = true; // indicate that line was read
		}
		while (this->buff[ (strlen(this->buff) - 1) ] != '\n');
		// read chunks until newline encountered
		
		if (flag)
			++n;
	}
	
	this->reset();
	
	return n;
}

unsigned long StreamLine::count_fields()
{
	if (! this->good_) return 0;
	
	unsigned long n = 0;
	std::string tok;
	
	if (!this->read())
	{
		throw std::runtime_error("Cannot read next line in input file");
	}
		
	std::istringstream raw(this->line);
	
	while(raw >> tok)
		++n;
	
	this->reset();
	
	return n;
}

std::string StreamLine::error(const std::string msg, const bool _name, const bool _count, const bool _line) const
{
	std::ostringstream oss;
	oss << msg << std::endl;
	if (_name)  oss << std::endl << "File: " << this->name;
	if (_count) oss << std::endl << "Line: " << this->count;
	if (_line)  oss << std::endl << ">> " << this->line << " <<";
	return oss.str();
}


//
// Parse line into tokens
//


ParseLine::ParseLine(const char * _line)
: l(_line)
, i(0)
, n(4096) // inital size for token char array
, sep('\0')
, sep_(false)
, count(0)
{
	this->token = new char[this->n];
}

ParseLine::ParseLine(const char * _line, const char _sep)
: l(_line)
, i(0)
, n(4096) // inital size for token char array
, sep(_sep)
, sep_(true)
, count(0)
{
	this->token = new char[this->n];
}

ParseLine::~ParseLine()
{
	delete [] this->token;
}

bool ParseLine::next()
{
	char c = this->l[this->i];
	bool b = false;
	
	// reset token
	this->token[0] = '\0';
	this->width = 0;
	
	while (c != '\n' && c != '\0') // is not at end
	{
		if (c > ' ' && (!this->sep_ || (this->sep_ && c != this->sep))) // is printable, and not a seperator
		{
			if (!b)
			{
				b = true;
			}
			
			this->token[ this->width ] = c; // insert char
			++this->width; // count token chars
			
			// increase max size of char array
			if (this->width == this->n)
			{
				this->n += 4096; // increase size
				
				char * temp = new char[this->n]; // make new temp token
				strcpy(temp, this->token); // copy to temp token
				delete [] this->token; // delete token
				
				this->token = new char[this->n]; // make new token
				strcpy(this->token, temp); // copy to token
				delete [] temp; // delete temp token
			}
		}
		else if (b)
		{
			++this->i;
			break; // break loop after token was extracted
		}
		
		c = this->l[ (++this->i) ];
	}
	
	if (b)
	{
		this->token[ this->width ] = '\0'; // terminate token
		this->count += 1; // count token
		
		return true;
	}
	
	return false;
}

bool ParseLine::is_numeric() const
{
	if (this->width > 1)
	{
		bool neg = false;
		
		for (size_t k = 0; k < this->width; ++k)
		{
			const char c = this->token[k];
			
			if (c < '0' || c > '9')
			{
				if (c == '-' && k == 0)
				{
					neg = true;
					continue;
				}
				
				if (c == '.' && (!neg && (k == 0 || k == 1)))
					continue;
				
				if (c == '.' && (neg && (k == 1 || k == 2)))
					continue;
				
				return false;
			}
		}
		
		return true;
	}
	
	return false;
}

template<>
ParseLine::operator char () const
{
	if (this->width == 1)
	{
		return this->token[0];
	}
	
	throw std::domain_error("Parsed token is a string: " + std::string(this->token));
}

template<>
ParseLine::operator unsigned char () const
{
	if (this->width == 1)
	{
		return static_cast<unsigned char>( this->token[0] );
	}
	
	throw std::domain_error("Parsed token is a string: " + std::string(this->token));
}

template<>
ParseLine::operator bool () const
{
	if (this->width == 1)
	{
		return static_cast<bool>( this->token[0] );
	}
	
	throw std::domain_error("Parsed token is not boolean: " + std::string(this->token));
}

template<>
ParseLine::operator int () const
{
	if (this->is_numeric())
	{
		return atoi(this->token);
	}
	
	throw std::domain_error("Parsed token is not numeric: " + std::string(this->token));
}

template<>
ParseLine::operator unsigned int () const
{
	if (this->is_numeric())
	{
		return static_cast<unsigned int>( atoi(this->token) );
	}
	
	throw std::domain_error("Parsed token is not numeric: " + std::string(this->token));
}

template<>
ParseLine::operator long () const
{
	if (this->is_numeric())
	{
		return atol(this->token);
	}
	
	throw std::domain_error("Parsed token is not numeric: " + std::string(this->token));
}

template<>
ParseLine::operator unsigned long () const
{
	if (this->is_numeric())
	{
		return static_cast<unsigned long>( atol(this->token) );
	}
	
	throw std::domain_error("Parsed token is not numeric: " + std::string(this->token));
}

template<>
ParseLine::operator double () const
{
	if (this->is_numeric())
	{
		return atof(this->token);
	}
	
	throw std::domain_error("Parsed token is not numeric: " + std::string(this->token));
}

template<>
ParseLine::operator float () const
{
	if (this->is_numeric())
	{
		return static_cast<float>( atof(this->token) );
	}
	
	throw std::domain_error("Parsed token is not numeric: " + std::string(this->token));
}



