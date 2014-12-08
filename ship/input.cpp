//
//  input.cpp
//  ship
//
//  Created by Patrick Albers on 21.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "input.h"


//******************************************************************************
// Input handling
//******************************************************************************

//
// Filter input before putting in source
//

FilterInput::FilterMarkerInfo::FilterMarkerInfo()
: remove_if_contains_indel(false)
, remove_if_contains_other(true)
, remove_if_multiallelic(false)
{}

FilterInput::FilterMarkerData::FilterMarkerData()
: remove_if_unphased(false)
, remove_if_unknown(true)
{}

FilterInput::FilterMarkerStat::FilterMarkerStat()
: any(false)
, remove_hap_below_(false)
, remove_hap_above_(false)
, remove_gen_below_(false)
, remove_gen_above_(false)
{}

bool FilterInput::apply(const MarkerInfo & info, const unsigned long i) const
{
	if (this->markerinfo.remove_if_contains_indel && info.contains_indel())
	{
		std::clog << "Marker filtered (contains indels): line " << i << std::endl;
		return false;
	}
	if (this->markerinfo.remove_if_contains_other && info.contains_other())
	{
		std::clog << "Marker filtered (contains undefined alleles): line " << i << std::endl;
		return false;
	}
	if (this->markerinfo.remove_if_multiallelic && info.n_alleles() > 2)
	{
		std::clog << "Marker filtered (is multi-allelic): line " << i << std::endl;
		return false;
	}
	return true;
}

bool FilterInput::apply(const MarkerData & data, const unsigned long i) const
{
	if (this->markerdata.remove_if_unphased && data.contains_unphased())
	{
		std::clog << "Marker filtered (contains unphased data): line " << i << std::endl;
		return false;
	}
	if (this->markerdata.remove_if_unknown  && data.contains_unknown())
	{
		std::clog << "Marker filtered (contains missing genotypes): line " << i << std::endl;
		return false;
	}
	return true;
}

bool FilterInput::apply(const MarkerStat & stat, const unsigned long i) const
{
	if (this->markerstat.any)
	{
		if (this->markerstat.remove_hap_below_)
		{
			for (std::map<Haplotype, Census>::const_iterator it = stat.haplostat.cbegin(), end = stat.haplostat.cend(); it != end; ++it)
			{
				if (it->second <= this->markerstat.remove_hap_below)
				{
					std::clog << "Marker filtered (allele count/frequency equal or below specified value): line " << i << std::endl;
					return false;
				}
			}
		}
		if (this->markerstat.remove_hap_above_)
		{
			for (std::map<Haplotype, Census>::const_iterator it = stat.haplostat.cbegin(), end = stat.haplostat.cend(); it != end; ++it)
			{
				if (it->second >= this->markerstat.remove_hap_above)
				{
					std::clog << "Marker filtered (allele count/frequency equal or above specified value): line " << i << std::endl;
					return false;
				}
			}
		}
		if (this->markerstat.remove_gen_below_)
		{
			for (std::map<Genotype, Census>::const_iterator it = stat.genostat.cbegin(), end = stat.genostat.cend(); it != end; ++it)
			{
				if (it->second <= this->markerstat.remove_gen_below)
				{
					std::clog << "Marker filtered (genotype count/frequency equal or below specified value): line " << i << std::endl;
					return false;
				}
			}
		}
		if (this->markerstat.remove_gen_above_)
		{
			for (std::map<Genotype, Census>::const_iterator it = stat.genostat.cbegin(), end = stat.genostat.cend(); it != end; ++it)
			{
				if (it->second >= this->markerstat.remove_gen_above)
				{
					std::clog << "Marker filtered (genotype count/frequency equal or above specified value): line " << i << std::endl;
					return false;
				}
			}
		}
	}
	return true;
}



//
// Scan input file format
//
ScanInput::ScanInput(const std::string & filename, const InputFormat format)
: ReadStream(filename)
, format(format)
, header_rows(0)
, marker_cols(0)
, sample_cols(0)
{
	if (this->format != InputFormat::UNKNOWN)
	{
		if (! this->read())
		{
			throw std::runtime_error(this->error("Cannot read file", true));
		}
		
		// check user-specified format
		switch (this->format)
		{
			case InputFormat::VCF:
				
				if (! this->scan_format_vcf())
				{
					throw std::invalid_argument(this->error("Invalid VCF file format", true));
				}
				break;
				
			case InputFormat::HAP:
				
				if (! this->scan_format_hap())
				{
					throw std::invalid_argument(this->error("Invalid HAP file format", true));
				}
				break;
				
			case InputFormat::GEN:
				
				if (! this->scan_format_gen())
				{
					throw std::invalid_argument(this->error("Invalid GEN file format", true));
				}
				break;
				
			case InputFormat::UNKNOWN:
				throw std::invalid_argument(this->error("Unknown file format", true));
				break;
		}

		this->close();
		return;
	}
	
	clock_t time = clock();
	
	while (((clock() - time) / CLOCKS_PER_SEC) < 5) // read lines up to 5 sec, if no format was determined
	{
		// read next line
		if (! this->read())
		{
			if (this->count == 0) // if first line cannot be read
				throw std::runtime_error(this->error("Cannot read file", true));
			
			this->close();
			return;
		}
		
		// check VCF
		if (this->scan_format_vcf())
		{
			this->format = InputFormat::VCF;
			this->close();
			return;
		}
		
		// check HAP
		bool is_hap = this->scan_format_hap();
		
		// check GEN
		bool is_gen = this->scan_format_gen();
		
		// determine HAP/GEN format, because they can appear similar
		if (is_hap && !is_gen)
		{
			this->format = InputFormat::HAP;
			this->close();
			return;
		}
		else if (is_gen && !is_hap)
		{
			this->format = InputFormat::GEN;
			this->close();
			return;
		}
	}
	
	this->close();
}

bool ScanInput::scan_format_vcf()
{
	// check first line, which determines VCF format
	if (this->count != 1)
	{
		return false;
	}
	
	// check first VCF header line
	if (std::string(this->line, vcf_top_header.size()) != vcf_top_header)
	{
		return false;
	}
	
	// continue reading header
	std::string last; // previous line

	do
	{
		last = this->line;
		this->header_rows += 1; // count header lines
	}
	while (this->read() && this->line[0] == '#');
	
	// check last header line for required column names
	std::vector<std::string> col;
	
	// parse sample IDs in header
	if (!strtk::parse(last, " \t", col))
		throw std::runtime_error(this->error("Unable to parse header information", true));
	
	for (int i = 0; i < vcf_col_number; ++i)
	{
		if (col[i] != vcf_col_header[i])
		{
			throw std::invalid_argument(this->error("Invalid VCF file format\n"
													"Required column '" + vcf_col_header[i] + "' cannot be found in header", true));
		}
	}
	
	// marker/sample column counts
	this->marker_cols = vcf_col_number;
	this->sample_cols = col.size() - vcf_col_number;
	
	return true;
}

bool ScanInput::scan_format_hap()
{
	std::vector<std::string> tokens;

	// parse line
	if (! strtk::parse(this->line, " ", tokens))
	{
		std::cout << this->line << std::endl;
		throw std::runtime_error(this->error("Cannot parse input file, at line: ", true, true));
	}

	// init marker/sample column counts
	this->marker_cols = tokens.size();
	this->sample_cols = 0;

	// reverse walkabout tokens
	for (const std::string & token : tokens)
	{
		if (token == "0" || token == "1") // either 0 or 1
		{
			this->marker_cols -= 1;
			this->sample_cols += 1;
			continue;
		}
		break;
	}

	// check sample column count
	if (sample_cols % hap_set_format != 0)
		return false;
	
	return true;
}

bool ScanInput::scan_format_gen()
{
	std::vector<std::string> tokens;
	std::vector<double> D;
	
	// parse line
	if (!strtk::parse(this->line, " ", tokens))
	{
		throw std::runtime_error(this->error("Cannot parse input file, at line: ", true, true));
	}
	
	// init marker/cample column counts
	this->marker_cols = tokens.size();
	this->sample_cols = 0;
	
	// reverse walkabout tokens
	for (const std::string & token : tokens)
	{
		double d = std::stod(token);
		
		if (d > 0.0 && d < 1.0) // float between 0 and 1
		{
			this->marker_cols -= 1;
			this->sample_cols += 1;
			
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
// Load input files into source
//
LoadInput::LoadInput(const ScanInput & scan, const FilterInput & _filter)
: input(scan.name, scan.format, scan.marker_cols, scan.header_rows)
, filter(_filter)
, sample_(false)
, legend_(false)
, genmap_(false)
{
	switch (scan.format)
	{
		case InputFormat::VCF: this->size = scan.sample_cols; break;
		case InputFormat::HAP: this->size = scan.sample_cols / hap_set_format; break;
		case InputFormat::GEN: this->size = scan.sample_cols / gen_set_format; break;
		case InputFormat::UNKNOWN:
		default:
			throw std::invalid_argument(this->input.error("Input file is of unknown format", true));
	}
}

//LoadInput::~LoadInput()
//{}

std::vector<SampleInfo> LoadInput::get_sample_vcf() const
{
	std::vector<SampleInfo> info(this->size); // init with sample size
	
	std::vector<std::string> keys;
	std::set<std::string>    uniq;
	
	keys.reserve(this->size); // reserve expected sample size
	
	if (this->input.header.size() == 0) throw std::exception(); // should never happen
	
	// tokenise header
	std::istringstream iss(this->input.header.back());
	std::string key;
	
	while (iss >> key)
	{
		if (uniq.count(key) != 0)
		{
			throw std::invalid_argument(this->input.error("Duplicate sample ID '" + key + "' detected in VCF header", true));
		}
		
		keys.push_back(key);
		uniq.insert(key);
	}
	uniq.clear();

	// remove marker columns
	keys.erase(keys.begin(), keys.begin() + vcf_col_number);
	
	if (this->size != keys.size()) throw std::exception(); // should never happen
	
	// move IDs in formated output
	for (unsigned long i = 0; i < this->size; ++i)
	{
		info[i].key = std::move(keys[i]); // move
	}
	keys.clear();
	
	return info;
}

std::vector<SampleInfo> LoadInput::get_sample_hap() const
{
	return this->make_sample(); // generate
}

std::vector<SampleInfo> LoadInput::get_sample_gen() const
{
	return this->make_sample(); // generate
}

std::vector<SampleInfo> LoadInput::make_sample() const
{
	std::vector<SampleInfo> info(this->size); // init with sample size
	
	const unsigned long len0 = std::to_string(this->size).size();
	
	for (unsigned long i = 0; i < this->size; ++i)
	{
		const std::string str = std::to_string(i);
		const unsigned long len1 = str.size();
		
		info[i].key = "SampleID_" + std::string(len0 - len1, '0') + str;
	}
	
	return info;
}

void LoadInput::sample(const std::string & filename)
{
	std::cout << "Loading sample file:" << std::endl;
	std::clog << "Loading sample file:" << std::endl;
	
	// open sample file
	ReadSample stream(filename);
	this->sample_ = true;
	
	Runtime runtime;
	ProgressMsg progress("samples");
	
	// expect sample size
	this->_sample.cache.reserve(this->size);
	
	std::set<std::string> unique;
	unsigned long count = 0;
	
	// load sample information from file
	while (stream.read())
	{
		SampleInfo info;
		
		if (! stream.parse(info))
		{
			throw std::invalid_argument(stream.error("Cannot parse sample information", true, true, true));
		}
		
		// check duplicates
		if (unique.count(info.key))
		{
			throw std::invalid_argument(stream.error("Duplicate sample ID detected", true, true, true));
		}
		
		unique.insert(info.key);
		this->_sample.cache.push_back(std::move(info)); // move
		count += 1;
		
		progress.update();
	}
	
	progress.finish();
	std::clog << "Samples read: " << count << std::endl;
	std::clog << runtime.str() << std::endl;
	
	// check expected sample size
	if (this->size != count)
	{
		throw std::invalid_argument(stream.error("Unexpected sample size\n"
												 "Samples expected: " + std::to_string(this->size) + "\n"
												 "Samples detected: " + std::to_string(count), true));
	}
	
	// sample IDs from VCF header
	if (this->input.format == InputFormat::VCF)
	{
		std::vector<SampleInfo> info = this->get_sample_vcf();
		
		// check sample IDs in both files
		for (unsigned long i = 0; i < this->size; ++i)
		{
			if (info[i].key != this->_sample.cache[i].key)
			{
				throw std::invalid_argument("Sample IDs in VCF file do not correspond to IDs in sample file\n"
											"# " + std::to_string(i) +  " expected from VCF file:  " + info[i].key + "\n"
											"# " + std::to_string(i) +  " detected in sample file: " + this->_sample.cache[i].key);
			}
		}
	}
}

void LoadInput::legend(const std::string & filename)
{
	std::cout << "Loading legend file:" << std::endl;
	std::clog << "Loading legend file:" << std::endl;
	
	// open sample file
	ReadLegend stream(filename);
	this->legend_ = true;
	
	Runtime runtime;
	ProgressMsg progress("markers");
	
	std::set<unsigned long> unique;
	unsigned long count = 0;
	
	// load marker information from file
	while (stream.read())
	{
		MarkerInfo info;
		bool flag = false; // flag that marker is valid
		
		if (stream.parse(info))
		{
			// check duplicate
			if (unique.count(info.pos) == 0)
			{
				flag = this->filter.apply(info, this->input.count); // apply filter
				unique.insert(info.pos);
			}
			else
			{
				// log duplicate
				std::clog << "Duplicate marker at position " + std::to_string(info.pos) + " ignored: line " + std::to_string(stream.count) << std::endl;
			}
		}
		
		// include all marker lines, even if parsing failed
		this->_legend.cache.push_back(std::move(info)); // move
		this->_legend.valid.push_back(static_cast<char>(flag)); // flag validity
		count += 1;
		
		progress.update();
	}
	
	progress.finish();
	std::clog << "Markers read: " << count << std::endl;
	std::clog << runtime.str() << std::endl;
	
	if (count == 0)
	{
		throw std::length_error(stream.error("Legend file is empty", true));
	}
}

void LoadInput::genmap(const std::string & filename)
{
	std::cout << "Loading genetic map:" << std::endl;
	std::clog << "Loading genetic map:" << std::endl;
	
	// open genetic map
	ReadGenmap stream(filename);
	this->genmap_ = true;
	
	Runtime runtime;
	ProgressMsg progress("mapped markers");
	
	// load marker information from file
	while (stream.read())
	{
		GenmapInfo info;
		
		if (stream.parse(info))
		{
			try
			{
				this->_genmap.cache.insert(std::move(info)); // move
			}
			catch (std::invalid_argument & ia)
			{
				throw std::invalid_argument(stream.error(ia.what(), true, true, true));
			}
		}
		
		progress.update();
	}
	
	this->_genmap.cache.finish();
	
	progress.finish();
	std::clog << "Mapped markers read: " << this->_genmap.cache.size() << std::endl;
	std::clog << runtime.str() << std::endl;
	
	if (this->_genmap.cache.size() == 0)
	{
		throw std::length_error(stream.error("No mapped markers found", true));
	}
}

void LoadInput::load(Source & source)
{
	// check required sample file
	if (this->input.require_sample && !this->sample_)
		throw std::runtime_error("Sample file required");
	
	// check required legend file
	if (this->input.require_legend && !this->legend_)
		throw std::runtime_error("Legend file required");
	
	// check required genmap file
	if (this->input.require_genmap && !this->genmap_)
		throw std::runtime_error("Genetic map required");
	
	// handle missing sample file
	if (!this->sample_)
	{
		// get sample IDs from input file (or generate)
		switch (this->input.format)
		{
			case InputFormat::VCF: this->_sample.cache = this->get_sample_vcf(); break;
			case InputFormat::HAP: this->_sample.cache = this->get_sample_hap(); break;
			case InputFormat::GEN: this->_sample.cache = this->get_sample_gen(); break;
			case InputFormat::UNKNOWN:
			default:
				this->_sample.cache = this->make_sample(); break;
		}
	}
	
	// source sample
	for (SampleInfo & info : this->_sample.cache)
	{
		Sample S;
		S.info = std::move(info); // move
		
		source.append(std::move(S)); //move
	}
	
	// handle missing genetic map
	if (! this->genmap_)
	{
		this->_genmap.cache.finish();
	}
	
	std::cout << "Loading input data file:" << std::endl;
	std::clog << "Loading input data file:" << std::endl;
	
	Runtime runtime;
	ProgressMsg progress("data lines");
	
	unsigned long count = 0;
	
	// read input file
	if (this->legend_ && this->input.require_legend) // legend provided & required
	{
		unsigned long n = this->_legend.cache.size();
		
		while (this->input.read())
		{
			progress.update();
			count += 1;
			
			if (n == 0)
			{
				throw std::length_error("Input file contains more markers than provided in legend file");
			}

			// retrieve marker information from cached legend
			MarkerInfo info = std::move(this->_legend.cache.front()); // move
			char       flag = this->_legend.valid.front();

			// pop cached legend
			this->_legend.cache.pop_front();
			this->_legend.valid.pop_front();
			n -= 1;
			
			// ignore data if marker is not valid
			if (! static_cast<bool>(flag))
			{
				continue;
			}
			
			Marker marker(this->size);
			
			// parse marker
			if (this->input.parse(marker.data))
			{
				marker.info = std::move(info); // move
				marker.stat.eval(marker.info, marker.data);
				marker.gmap = this->_genmap.cache.approx(marker.info);

				// apply filters
				if (this->filter.apply(marker.stat, this->input.count) &&
					this->filter.apply(marker.data, this->input.count))
				{
					source.append(std::move(marker)); //move
				}
			}
		}
		
		if (n != 0)
		{
			throw std::length_error("Input file contains less markers than provided in legend file");
		}
	}
	else if (this->legend_ && !this->input.require_legend) // legend provided & NOT required
	{
		unsigned long n = this->_legend.cache.size();
		
		while (this->input.read())
		{
			progress.update();
			count += 1;
			
			if (n == 0)
			{
				throw std::length_error("Input file contains more markers than provided in legend file");
			}
			
			// retrieve marker information from cached legend
			MarkerInfo info = std::move(this->_legend.cache.front()); // move
			char       flag = this->_legend.valid.front();
			
			// pop cached legend
			this->_legend.cache.pop_front();
			this->_legend.valid.pop_front();
			n -= 1;
			
			// ignore data if marker is not valid
			if (! static_cast<bool>(flag))
			{
				continue;
			}
			
			Marker marker(this->size);
			
			// parse marker
			if (this->input.parse(marker.info, marker.data))
			{
				if (info.pos != marker.info.pos)
				{
					throw std::invalid_argument(this->input.error("Marker positions do not match in input and legend files\n"
																  "Expected position: " + std::to_string(marker.info.pos) + "\n"
																  "Detected position: " + std::to_string(info.pos) + " (in legend file)", true, true));
				}
				
				if (info.n_alleles() != marker.info.n_alleles())
				{
					throw std::invalid_argument(this->input.error("Number of alleles do not match in input and legend files\n"
																  "Expected alleles: " + std::to_string(marker.info.n_alleles()) + "\n"
																  "Detected alleles: " + std::to_string(info.n_alleles()) + " (in legend file)", true, true));
				}
				
				marker.info = std::move(info); // move
				marker.stat.eval(marker.info, marker.data);
				marker.gmap = this->_genmap.cache.approx(marker.info);
				
				// apply filters
				if (this->filter.apply(marker.stat, this->input.count) &&
					this->filter.apply(marker.data, this->input.count))
				{
					source.append(std::move(marker)); //move
				}
			}
		}
		
		if (n != 0)
		{
			throw std::length_error("Input file contains less markers than provided in legend file");
		}
	}
	else if (!this->legend_ && !this->input.require_legend) // legend NOT provided & NOT required
	{
		while (this->input.read())
		{
			progress.update();
			count += 1;
			
			Marker marker(this->size);
			
			// parse marker
			if (this->input.parse(marker.info, marker.data))
			{
				marker.stat.eval(marker.info, marker.data);
				marker.gmap = this->_genmap.cache.approx(marker.info);
				
				// apply filters
				if (this->filter.apply(marker.info, this->input.count) &&
					this->filter.apply(marker.stat, this->input.count) &&
					this->filter.apply(marker.data, this->input.count))
				{
					source.append(std::move(marker)); //move
				}
			}
		}
	}
	
	progress.finish();
	std::clog << "Input data read: " << count << std::endl;
	std::clog << runtime.str() << std::endl;
	
	// finish source
	source.finish();
}


