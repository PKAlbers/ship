//
//  loader.cpp
//  ship
//
//  Created by Patrick Albers on 18.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "loader.h"


//**************************************************************************
// Cargo containers, for temporary holding and moving types
//**************************************************************************

//
// Sample cargo
//
void CargoSample::insert(const Sample & sample)
{
	if (this->index.count(sample.info.key) != 0)
	{
		throw std::invalid_argument("Duplicate sample ID detected");
	}
	
	this->index.insert(sample.info.key);
	this->cargo.push(sample);
	this->size += 1;
}

void CargoSample::insert(Sample && sample) noexcept
{
	if (this->index.count(sample.info.key) != 0)
	{
		throw std::invalid_argument("Duplicate sample ID detected");
	}
	
	this->index.insert(sample.info.key);
	this->cargo.push(std::move(sample)); // move
	this->size += 1;
}

void CargoSample::finish()
{
	this->index.clear();
}

void CargoSample::clear()
{
	if (! this->cargo.empty())
	{
		throw std::logic_error(std::to_string(this->cargo.size()) + " samples were left behind");
	}
}

void CargoSample::copy(Sample & sample)
{
	if (this->cargo.empty())
	{
		throw std::logic_error("No samples left");
	}
	
	sample = this->cargo.front(); // copy first element
	this->cargo.pop(); // remove first element
}

void CargoSample::move(Sample & sample)
{
	if (this->cargo.empty())
	{
		throw std::logic_error("No samples left");
	}
	
	std::swap(sample, this->cargo.front()); // swap first element
	this->cargo.pop(); // remove first element
}


//
// Legend cargo
//
void CargoLegend::insert(unsigned long i, const Marker & marker)
{
	this->index[marker.info.pos].push_back(i);
	this->cargo[i] = marker;
	this->size += 1;
}

void CargoLegend::insert(unsigned long i, Marker && marker) noexcept
{
	this->index[marker.info.pos].push_back(i);
	this->cargo[i] = std::move(marker); // move
	this->size += 1;
}

void CargoLegend::finish()
{
	if (this->size == 0)
		return;
	
	// remove duplicate markers att all duplicated positions
	for (std::unordered_map<Position, std::vector<Index> >::const_iterator it = this->index.cbegin(), end = this->index.cend(); it != end; ++it)
	{
		if (it->second.size() > 1)
		{
			// remove all in cargo
			for (const Index & i : it->second)
			{
				this->cargo.erase(i);
			}
			
			// log removal
			std::clog << "Removed " << it->second.size() <<  " duplicate markers at position '" << it->first << "'" << std::endl;
		}
	}
	
	this->index.clear();
}

void CargoLegend::clear()
{
	if (this->cargo.size() == 0)
		return;
	
	for (std::unordered_map<Index, Marker>::const_iterator it = this->cargo.cbegin(), end = this->cargo.cend(); it != end; ++it)
	{
		// log leftovers
		std::clog << "Unmatched marker: " << std::flush;
		it->second.info.print(std::clog);
		std::clog << std::endl;
	}
	
	throw std::runtime_error("Unable to match " + std::to_string(this->cargo.size()) +  " markers to data");
}

bool CargoLegend::copy(const unsigned long i, Marker & marker)
{
	if (this->cargo.count(i) == 0)
		return false;
	
	try
	{
		marker = this->cargo.at(i);
		this->cargo.erase(i);
	}
	catch (std::out_of_range &)
	{
		return false;
	}
	
	return true;
}

bool CargoLegend::move(const unsigned long i , Marker & marker)
{
	if (this->cargo.count(i) == 0)
		return false;
	
	try
	{
		marker = std::move(this->cargo.at(i));
		this->cargo.erase(i);
	}
	catch (std::out_of_range &)
	{
		return false;
	}
	
	return true;
}



//**************************************************************************
// Load data file
//**************************************************************************
Loader::Loader(const std::string & filename, Loader::Format _format)
: format_(scan(filename, _format))
, sample_(false)
, legend_(false)
, genmap_(false)
{
	switch (this->format_)
	{
		case Format::VCF: this->input_format = std::unique_ptr<InputData>(new InputData_VCF(filename)); break;
		case Format::HAP: this->input_format = std::unique_ptr<InputData>(new InputData_HAP(filename)); break;
		case Format::GEN: this->input_format = std::unique_ptr<InputData>(new InputData_GEN(filename)); break;
		case Format::UNKNOWN:
		default:
			throw std::invalid_argument("Unknown file format\n"
							            "File: " + filename);
			break;
	}
}



Loader::Format Loader::scan(const std::string & filename, const Loader::Format _format)
{
	if (_format == Format::VCF) return Format::VCF; // VCF was user specified
	if (_format == Format::HAP) return Format::HAP; // HAP was user specified
	if (_format == Format::GEN) return Format::GEN; // GEN was user specified
	
	InputStream   file(filename);
	clock_t       time = clock();
	
	while (((clock() - time) / CLOCKS_PER_SEC) < 5) // read lines up to 5 sec, if no format was determined
	{
		// read next line
		if (!file.readline())
		{
			if (file.count == 0) // if first line cannot be read
				throw std::runtime_error(file.error("Cannot read file", true));
			
			return Format::UNKNOWN;
		}
		
		// check VCF (only first line)
		if (file.count == 1 && format_is_vcf(file.line))
			return Format::VCF;
		
		unsigned long x, y; // output not needed
		
		// check HAP
		bool is_hap = format_is_hap(file.line, x, y);
		
		// check GEN
		bool is_gen = format_is_gen(file.line, x, y);
		
		// determine HAP/GEN format
		if (is_hap && !is_gen) return Format::HAP;
		if (is_gen && !is_hap) return Format::GEN;
		
		// if unable to distinguish between HAP/GEN, continue up to time limit
	}
	
	return Format::UNKNOWN;
}

void Loader::match_sample(unsigned long size)
{
	if (this->sample_)
	{
		if (this->cargo_sample.size() == 0)
			throw std::invalid_argument(this->input_sample->stream.error("Cannot detect sample information in sample file", true));
	}
	
	// get samples from VCF header
	if (this->format_ == Format::VCF)
	{
		std::vector<std::string> key;
		
		// check header is present (just in case)
		if (this->input_format->header.size() == 0)
			throw std::exception();
		
		// parse header
		if (! strtk::parse(this->input_format->header.back(), del_white, key))
			throw std::runtime_error(this->input_format->stream.error("Cannot parse sample IDs in VCF header", true));
		
		// remove marker fields
		key.erase(key.begin(), key.begin() + this->input_format->marker_cols);
		
		// check parsed sample size (just in case)
		if (key.size() != size)
			throw std::runtime_error(this->input_format->stream.error("Unable to determine correct sample size", true));
		
		// check duplicates
		std::set<std::string> unique;
		for (const std::string & k : key)
		{
			if (unique.count(k) != 0)
			{
				throw std::invalid_argument(this->input_format->stream.error("Duplicate sample ID '" + k + "' detected in VCF header", true));
			}
			unique.insert(k);
		}
		unique.clear();
		
		//
		
		//
		// sample IDs in VCF file determine order of sample information
		//
		CargoSample vcf_cargo;
		std::unordered_map<std::string, Sample> index;
		
		// extract samples from cargo
		for (unsigned long i = 0; i < size; ++i)
		{
			Sample S;
			this->cargo_sample.move(S);
			old_sample.insert(std::move(S)); // move
		}
		
		
		for (const std::string & k : key)
		{
			Sample vcf_sample;
			vcf_sample
			
			if (old_sample.count(k) != 0)
			{
				
			}
			
			vcf_cargo.insert(std::move(vcf_sample));
			
			
			if (this->cargo_sample.index.count(k) == 0)
			{
				Sample S;
				S.info.key = key; // sample ID from VCF file
				sample.push_back(std::move(S)); //move
			}
			else
			{
				// sample file may contain additional information
				sample.push_back(std::move(unique.at(key))); // move
				
				// remove sample ID from read sample file
				unique.erase(key);
			}
		}
		
		// check unused sample IDs
		if (unique.size() != 0)
		{
			for (std::map<std::string, Sample>::const_iterator it = unique.cbegin(), end = unique.cend(); it != end; ++it)
			{
				// log that sample was ignored
				std::clog << "Ignoring sample information: ";
				it->second.info.print(std::clog);
				std::clog << " (due to: not defined in VCF file)" << std::endl;
			}
		}
		
		// check sample number in VCF file
		if (this->sample_ && sample.size() != size)
		{
			throw std::length_error(this->input_sample->stream.error("Different sample size in sample file detected\n"
																	 "Samples in VCF file:    " + std::to_string(size) + "\n"
																	 "Samples in sample file: " + std::to_string(sample.size()), true));
		}
	}
	
	// check sample number in HAP file
	else if (this->format_ == Format::HAP)
	{
		// no sample information here
		
		if (this->sample_ && sample.size() != size)
		{
			throw std::length_error(this->input_sample->stream.error("Different sample size in sample file detected\n"
																	 "Samples in HAP file:    " + std::to_string(size) + "\n"
																	 "Samples in sample file: " + std::to_string(sample.size()), true));
		}
	}
	
	// check sample number in GEN file
	else if (this->format_ == Format::GEN)
	{
		// no sample information here
		
		if (this->sample_ && sample.size() != size)
		{
			throw std::length_error(this->input_sample->stream.error("Different sample size in sample file detected\n"
																	 "Samples in GEN file:    " + std::to_string(size) + "\n"
																	 "Samples in sample file: " + std::to_string(sample.size()), true));
		}
	}
	
	// generate sample IDs if no sample file was provided, and format is not VCF
	if (! this->sample_ && sample.size() == 0)
	{
		unsigned long l, len = std::to_string(size).size();
		
		for (unsigned long i = 0; i < size; ++i)
		{
			l = std::to_string(i).size();
			
			Sample S;
			S.info.key = "Sample_" + std::string(len - l, '0') + std::to_string(i);
			sample.push_back(std::move(S)); // move
		}
	}
}

char Loader::format() const
{
	switch (this->format_)
	{
		case Format::VCF: return 'v'; break;
		case Format::HAP: return 'h'; break;
		case Format::GEN: return 'g'; break;
		case Format::UNKNOWN: return '.'; break;
	}
}

bool Loader::format(const Format _format) const
{
	return (this->format_ == _format);
}

void Loader::sample(const std::string & filename)
{
	std::coutlog << "Loading sample file:" << std::endl;
	
	// open sample file
	this->input_sample = std::unique_ptr<InputSample>(new InputSample(filename));
	this->sample_ = true;
	
	Runtime runtime;
	ProgressMsg progress("samples");
	
	// load sample information from file
	while (this->input_sample->stream.readline())
	{
		Sample S;
		
		this->input_sample->load(S.info);
		
		try
		{
			this->cargo_sample.insert(std::move(S)); // move
		}
		catch (std::invalid_argument & ia)
		{
			throw std::invalid_argument(this->input_sample->stream.error(ia.what(), true, true, true));
		}
		
		progress.update();
	}
	
	progress.finish();
	std::clog << "Samples read: " << this->input_sample->stream.count << std::endl;
	std::clog << runtime.str() << std::endl;
}

void Loader::legend(const std::string & filename)
{
	std::coutlog << "Loading legend file:" << std::endl;
	
	// open sample file
	this->input_legend = std::unique_ptr<InputLegend>(new InputLegend(filename));
	this->legend_ = true;
	
	Runtime runtime;
	ProgressMsg progress("markers");
	
	// load marker information from file
	while (this->input_legend->stream.readline())
	{
		Marker M;
		
		this->input_legend->load(M.info)
		
		this->cargo_marker.insert(this->input_legend->i, std::move(M)); // move
		
		progress.update();
	}
	
	this->cargo_marker.finish();
	
	progress.finish();
	std::clog << "Markers read: " << this->input_legend->stream.count << std::endl;
	std::clog << runtime.str() << std::endl;
}

void Loader::genmap(const std::string & filename)
{
	std::coutlog << "Loading genetic map file:" << std::endl;
	
	// open sample file
	this->input_genmap = std::unique_ptr<InputGenmap>(new InputGenmap(filename));
	this->genmap_ = true;
	
	Runtime runtime;
	ProgressMsg progress("mapped markers");
	
	// load marker information from file
	while (this->input_genmap->stream.readline())
	{
		GenmapInfo info;
		
		this->input_genmap->load(info)
		
		try
		{
			this->cargo_genmap.insert(std::move(info)); // move
		}
		catch (std::invalid_argument & ia)
		{
			throw std::invalid_argument(this->input_genmap->stream.error(ia.what(), true, true, true));
		}
		
		progress.update();
	}
	
	this->cargo_marker.finish();
	
	progress.finish();
	std::clog << "Mapped markers read: " << this->input_legend->stream.count << std::endl;
	std::clog << runtime.str() << std::endl;
}

void Loader::run(Source & source)
{
	unsigned long size = 0;
	
	// get sample size
	switch (this->format_)
	{
		case Format::VCF: size = this->input_format->sample_cols; break;
		case Format::HAP: size = this->input_format->sample_cols / hap_set_format; break;
		case Format::GEN: size = this->input_format->sample_cols / gen_set_format; break;
		case Format::UNKNOWN:
		default:
			throw std::logic_error("Unable to determine file format");
			break;
	}
	
	// check required sample file
	if (this->input_format->require_sample && !this->sample_)
		throw std::runtime_error("Sample file required");
	
	// check required legend file
	if (this->input_format->require_legend && !this->legend_)
		throw std::runtime_error("Legend file required");
	
	// check required genmap file
	if (this->input_format->require_genmap && !this->genmap_)
		throw std::runtime_error("Genetic map file required");
	
	// put samples into source
	if (this->sample_)
	{
		
		
		// check sample size
		if (this->cargo_sample.size() != size)
		{
			
		}
	}
	else
	{
		
	}
	
	
	
	//
	// load data format into source
	//
	std::coutlog << "Loading data: " << std::endl;
	
	Runtime runtime;
	ProgressMsg progress("lines");
	
	// load sample information from file
	while (this->input_format->stream.readline())
	{
		Marker marker;
		
		MarkerData data(size);
		
		// parse line
		if (! this->input_format->data(data))
			throw std::runtime_error(this->input_format->stream.error("Unable to interpret data", true, true));
		
		source.append(std::move(marker), data);
		
		progress.update();
	}
	
	progress.finish();
	std::clog << "Lines read: " << this->input_format->stream.count << std::endl;
	std::clog << runtime.str() << std::endl;
	
	std::coutlog << "Sorting data ... " << std::flush;
	source.finish();
	std::coutlog << "OK" << std::endl;
}


