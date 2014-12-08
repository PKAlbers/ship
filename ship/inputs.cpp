//
//  inputs.cpp
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "inputs.h"


//**************************************************************************
// Read external file
//**************************************************************************

//
// Base class
//
template <class Container>
InputFile<Container>::InputFile(const std::string & filename, const unsigned int _nhead)
: stream(filename)
, nhead(_nhead)
, c(0)
, i(0)
{
	this->c = this->stream.count_fields();
	
	for (unsigned int j = 0; j < this->nhead; ++j)
	{
		if (! this->stream.readline())
			throw std::runtime_error("Cannot read header in '" + this->stream.name + "'");
		
		this->header.push_back(this->stream.line);
	}
}


//
// Load sample file
//
InputSample::InputSample(const std::string & filename)
: InputFile<SampleInfo>(filename, sample_headers)
{}

bool InputSample::load(SampleInfo & info)
{
	bool flag = false;
	
	// parse sample line
	switch (this->c)
	{
		case 3:  flag = parse_sample_line_3col(this->stream.line, info); break;
		case 2:  flag = parse_sample_line_2col(this->stream.line, info); break;
		case 1:  flag = parse_sample_line_1col(this->stream.line, info); break;
		default:
			throw std::invalid_argument(this->stream.error("Cannot read from sample files with " + std::to_string(this->c) + " columns", true));
			break;
	}
	
	if (!flag)
	{
		std::clog << "Cannot parse sample file: line " << this->stream.count << std::endl;
	}
	
	this->i += 1; // index
	return flag;
}


//
// Load legend file
//
InputLegend::InputLegend(const std::string & filename)
: InputFile<MarkerInfo>(filename, legend_headers)
{}

bool InputLegend::load(MarkerInfo & info)
{
	bool flag = false;
	
	// parse legend line
	switch (this->c)
	{
		case 6:  flag = parse_legend_line_6col(this->stream.line, info); break;
		case 5:  flag = parse_legend_line_5col(this->stream.line, info); break;
		case 4:  flag = parse_legend_line_4col(this->stream.line, info); break;
		case 3:  flag = parse_legend_line_3col(this->stream.line, info); break;
		default:
			throw std::invalid_argument(this->stream.error("Cannot read from legend files with " + std::to_string(this->c) + " columns", true));
			break;
	}
	
	if (!flag)
	{
		std::clog << "Cannot parse legend file: line " << this->stream.count << std::endl;
	}
	
	this->i += 1; // index
	return flag;
}


//
// Load genmap file
//
InputGenmap::InputGenmap(const std::string & filename)
: InputFile<GenmapInfo>(filename, genmap_headers)
{}

bool InputGenmap::load(GenmapInfo & info)
{
	bool flag = false;
	
	// parse genetic map line
	switch (this->c)
	{
		case 4:  flag = parse_genmap_line_4col(this->stream.line, info); break;
		case 3:  flag = parse_genmap_line_3col(this->stream.line, info); break;
		default:
			throw std::invalid_argument(this->stream.error("Cannot read from genetic map files with " + std::to_string(this->c) + " columns", true));
			break;
	}
	
	if (!flag)
	{
		std::clog << "Cannot parse legend file: line " << this->stream.count << std::endl;
	}
	
	this->i += 1; // index
	return flag;
}



//**************************************************************************
// Read data file
//**************************************************************************

inline bool format_is_vcf(char * line)
{
	return (std::string(line, 16) == "##fileformat=VCF") ? true: false;
}

inline bool format_is_hap(char * line, unsigned long & marker_cols, unsigned long & sample_cols)
{
	std::vector<std::string> tokens;
	
	// parse line
	if (!strtk::parse(line, " ", tokens))
		return false;
	
	// init marker/cample column counts
	marker_cols = tokens.size();
	sample_cols = 0;
	
	// reverse walkabout tokens
	for (std::vector<std::string>::const_reverse_iterator it = tokens.crbegin(), end = tokens.crend(); it != end; ++it)
	{
		if (*it == "0" || *it == "1") // either 0 or 1
		{
			--marker_cols;
			++sample_cols;
			continue;
		}
		break;
	}
	
	// check sample column count
	if (sample_cols % hap_set_format != 0)
		return false;
	
	return true;
}

inline bool format_is_gen(char * line, unsigned long & marker_cols, unsigned long & sample_cols)
{
	std::vector<std::string> tokens;
	std::vector<double> D;
	
	// parse line
	if (!strtk::parse(line, " ", tokens))
		return false;
	
	// init marker/cample column counts
	marker_cols = tokens.size();
	sample_cols = 0;
	
	// reverse walkabout tokens
	for (std::vector<std::string>::const_reverse_iterator it = tokens.crbegin(), end = tokens.crend(); it != end; ++it)
	{
		double d = std::stod(*it);
		
		if (d > 0.0 && d < 1.0) // float between 0 and 1
		{
			--marker_cols;
			++sample_cols;
			
			D.push_back(d); // collect values
			
			continue;
		}
		break;
	}
	
	// check sample column count
	if (sample_cols % gen_set_format != 0)
		return false;
	
	// check genotype per individual
	for (unsigned long i = 0, k; i < sample_cols; )
	{
		double sum = 0;
		for (k = 0; k < gen_set_format; ++k, ++i)
			sum += D[i];
		
		if (sum < 0.95 || sum > 1.05) // tripple must sum to ~1
		{
			return false;
		}
	}
	
	return true;
}


//
// Base class
//
InputData::InputData(const std::string & filename)
: stream(filename)
, header_rows(0)
, marker_cols(0)
, sample_cols(0)
, require_sample(true)
, require_legend(true)
, require_genmap(true)
, i(0)
{}


//
// Load from VCF file
//
InputData_VCF::InputData_VCF(const std::string & filename)
: InputData(filename)
{
	std::vector<std::string> tokens;
	bool flag = false;
	
	// read first line
	if (!this->stream.readline())
		throw std::runtime_error(this->stream.error("Cannot read from VCF file", true));
	
	// check format
	if (! format_is_vcf(this->stream.line))
	{
		throw std::invalid_argument(this->stream.error("Cannot interpret VCF content", true));
	}
	
	// collect header
	while (this->stream.readline())
	{
		if (this->stream.line[0] != '#')
		{
			flag = true;
			break;
		}
		
		this->header_rows += 1;
		this->header.push_back(this->stream.line);
	}
	
	this->stream.rollback(); // reset file
	
	if (!flag)
		throw std::invalid_argument(this->stream.error("VCF file does not contain any data", true));
	
	if (this->header_rows == 0)
		throw std::invalid_argument(this->stream.error("VCF file does not contain header information", true));
	
	// skip header
	for (int i = 0; i < this->header_rows; ++i)
	{
		this->stream.readline();
	}
	
	// count fields in last header line (current, after skipping)
	if (!strtk::parse(this->stream.line, " \t", tokens))
		throw std::runtime_error(this->stream.error("Unable to parse header information", true));
	
	// check sample size
	if (tokens.size() <= vcf_col_format)
		throw std::invalid_argument(this->stream.error("VCF file does not contain sample information", true));
	
	// required header fields for VCF
	for (int i = 0; i < vcf_col_format; ++i)
	{
		if (tokens[i] != vcf_col_header[i])
		{
			throw std::invalid_argument(this->stream.error("Invalid VCF file format\n"
														   "'" + vcf_col_header[i] + "' is missing in header", true));
		}
	}

	this->marker_cols = vcf_col_format;
	this->sample_cols = tokens.size() - vcf_col_format;
	
	// require external files
	this->require_sample = false;
	this->require_legend = false; // always
	this->require_genmap = false;
}

bool InputData_VCF::full(MarkerInfo & info, MarkerData & data)
{
	bool flag = parse_vcf_full(this->stream.line, info, data);
		
	if (!flag)
	{
		std::clog << "Cannot parse VCF file: line " << this->stream.count << std::endl;
	}
	
	this->i += 1;
	return flag;
}

bool InputData_VCF::data(MarkerData &)
{
	return false;
}


//
// Load from HAP file
//
InputData_HAP::InputData_HAP(const std::string & filename)
: InputData(filename)
{
	// read first line
	if (!this->stream.readline())
		throw std::runtime_error(this->stream.error("Cannot read from haplotypes file", true));
	
	// check format
	if (! format_is_hap(this->stream.line, this->marker_cols, this->sample_cols))
	{
		throw std::invalid_argument(this->stream.error("Cannot interpret haplotype information", true));
	}
	
	this->stream.rollback(); //reset file
	
	// require external files
	this->require_sample = false;
	this->require_legend = (this->marker_cols == 0) ? true: false;
	this->require_genmap = false;
}

bool InputData_HAP::full(MarkerInfo & info, MarkerData & data)
{
	bool flag = false;
	
	// parse marker + data
	switch (this->marker_cols)
	{
		case 6:  flag = parse_hap_full_6col(this->stream.line, info, data); break;
		case 5:  flag = parse_hap_full_5col(this->stream.line, info, data); break;
		case 4:  flag = parse_hap_full_4col(this->stream.line, info, data); break;
		case 3:  flag = parse_hap_full_3col(this->stream.line, info, data); break;
		default:
			throw std::invalid_argument("Cannot read from haplotype data files with " + std::to_string(this->marker_cols) + " legend columns");
			break;
	}
	
	if (!flag)
	{
		std::clog << "Cannot parse HAP file: line " << this->stream.count << std::endl;
	}
	
	this->i += 1;
	return flag;
}

bool InputData_HAP::data(MarkerData & data)
{
	bool flag = parse_hap_data(this->stream.line, data);
	
	if (!flag)
	{
		std::clog << "Cannot parse HAP file: line " << this->stream.count << std::endl;
	}
	
	this->i += 1;
	return flag;
}


//
// Load from GEN file
//
InputData_GEN::InputData_GEN(const std::string & filename)
: InputData(filename)
{
	// read first line
	if (!this->stream.readline())
		throw std::runtime_error(this->stream.error("Cannot read from genotypes file", true));
	
	// check format
	if (! format_is_gen(this->stream.line, this->marker_cols, this->sample_cols))
	{
		throw std::invalid_argument(this->stream.error("Cannot interpret genotype information", true));
	}
	
	this->stream.rollback(); //reset file
	
	// require external files
	this->require_sample = false;
	this->require_legend = (this->marker_cols == 0) ? true: false;
	this->require_genmap = false;
}

bool InputData_GEN::full(MarkerInfo & info, MarkerData & data)
{
	bool flag = false;
	
	// parse marker + data
	switch (this->marker_cols)
	{
		case 6:  flag = parse_gen_full_6col(this->stream.line, info, data); break;
		case 5:  flag = parse_gen_full_5col(this->stream.line, info, data); break;
		case 4:  flag = parse_gen_full_4col(this->stream.line, info, data); break;
		case 3:  flag = parse_gen_full_3col(this->stream.line, info, data); break;
		default:
			throw std::invalid_argument("Cannot read from genotype data files with " + std::to_string(this->marker_cols) + " legend columns");
			break;
	}
	
	if (!flag)
	{
		std::clog << "Cannot parse GEN file: line " << this->stream.count << std::endl;
	}
	
	this->i += 1;
	return flag;
}

bool InputData_GEN::data(MarkerData & data)
{
	bool flag = parse_gen_data(this->stream.line, data);
	
	if (!flag)
	{
		std::clog << "Cannot parse GEN file: line " << this->stream.count << std::endl;
	}
	
	this->i += 1;
	return flag;
}






