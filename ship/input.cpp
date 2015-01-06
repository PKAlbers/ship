//
//  line.cpp
//  ship
//
//  Created by Patrick Albers on 13.12.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "input.h"
#include "parse.hpp"


//******************************************************************************
// Input handling
//******************************************************************************

const std::vector<std::string> vcf_required_columns = {
	"#CHROM",
	"POS",
	"ID",
	"REF",
	"ALT",
	"QUAL",
	"FILTER",
	"INFO",
	"FORMAT"
}; // expected column headers in VCF file


//
// Filter line before sourcing
//

FilterInput::FilterMarkerInfo::FilterMarkerInfo()
: any(false)
, remove_if_contains_snp_(false)
, remove_if_contains_indel_(false)
, remove_if_contains_other_(false)
, remove_if_biallelic_(false)
, remove_if_multiallelic_(false)
, remove_pos_(false)
{}

void FilterInput::FilterMarkerInfo::remove_if_contains_snp()
{
	std::clog << "Filter applied: exclude markers containing SNPs" << std::endl;
	this->any = true;
	this->remove_if_contains_snp_ = true;
}

void FilterInput::FilterMarkerInfo::remove_if_contains_indel()
{
	std::clog << "Filter applied: exclude markers containing INDELs" << std::endl;
	this->any = true;
	this->remove_if_contains_indel_ = true;
}

void FilterInput::FilterMarkerInfo::remove_if_contains_other()
{
	std::clog << "Filter applied: exclude markers containing missing/undefined allele definitions" << std::endl;
	this->any = true;
	this->remove_if_contains_other_ = true;
}

void FilterInput::FilterMarkerInfo::remove_if_biallelic()
{
	std::clog << "Filter applied: exclude bi-alleleic markers" << std::endl;
	this->any = true;
	this->remove_if_biallelic_ = true;
}

void FilterInput::FilterMarkerInfo::remove_if_multiallelic()
{
	std::clog << "Filter applied: exclude multi-alleleic markers" << std::endl;
	this->any = true;
	this->remove_if_multiallelic_ = true;
}

void FilterInput::FilterMarkerInfo::remove_pos(const std::string & filename)
{
	std::ifstream line(filename);
	size_t pos;
	
	while (line >> pos)
	{
		this->remove_pos_set.insert(pos);
	}
	
	std::clog << "Filter applied: exclude markers at " << this->remove_pos_set.size() << " positions" << std::endl;
	this->any = true;
	this->remove_pos_ = true;
}


FilterInput::FilterMarkerData::FilterMarkerData()
: any(false)
, remove_if_contains_unknown_(false)
{}

void FilterInput::FilterMarkerData::remove_if_contains_unknown()
{
	std::clog << "Filter applied: exclude markers containing missing/undefined samples" << std::endl;
	this->any = true;
	this->remove_if_contains_unknown_ = true;
}


FilterInput::FilterMarkerStat::FilterMarkerStat()
: any(false)
, remove_hap_below_(false)
, remove_hap_above_(false)
, remove_gen_below_(false)
, remove_gen_above_(false)
{}

void FilterInput::FilterMarkerStat::remove_hap_below(const std::string & str, const size_t size)
{
	this->remove_hap_below_cutoff.parse(str);
	this->remove_hap_below_cutoff.scale(size);
	
	std::clog << "Filter applied: exclude markers below or equal to allele count/frequency of " << str << std::endl;
	this->any = true;
	this->remove_hap_below_ = true;
}

void FilterInput::FilterMarkerStat::remove_hap_above(const std::string & str, const size_t size)
{
	this->remove_hap_above_cutoff.parse(str);
	this->remove_hap_above_cutoff.scale(size);
	
	std::clog << "Filter applied: exclude markers above or equal to allele count/frequency of " << str << std::endl;
	this->any = true;
	this->remove_hap_above_ = true;
}

void FilterInput::FilterMarkerStat::remove_gen_below(const std::string & str, const size_t size)
{
	this->remove_gen_below_cutoff.parse(str);
	this->remove_gen_below_cutoff.scale(size);
	
	std::clog << "Filter applied: exclude markers below or equal to genotype count/frequency of " << str << std::endl;
	this->any = true;
	this->remove_gen_below_ = true;
}

void FilterInput::FilterMarkerStat::remove_gen_above(const std::string & str, const size_t size)
{
	this->remove_gen_above_cutoff.parse(str);
	this->remove_gen_above_cutoff.scale(size);
	
	std::clog << "Filter applied: exclude markers above or equal to genotype count/frequency of " << str << std::endl;
	this->any = true;
	this->remove_gen_above_ = true;
}


FilterInput::FilterMarkerGmap::FilterMarkerGmap()
: any(false)
, remove_if_source_interpolated_(false)
, remove_if_source_extrapolated_(false)
, remove_if_source_unknown_(false)
{}

void FilterInput::FilterMarkerGmap::remove_if_source_interpolated()
{
	std::clog << "Filter applied: exclude markers if interpolated from genetic map" << std::endl;
	this->any = true;
	this->remove_if_source_interpolated_ = true;
}

void FilterInput::FilterMarkerGmap::remove_if_source_extrapolated()
{
	std::clog << "Filter applied: exclude markers if extrapolated from genetic map" << std::endl;
	this->any = true;
	this->remove_if_source_extrapolated_ = true;
}

void FilterInput::FilterMarkerGmap::remove_if_source_unknown()
{
	std::clog << "Filter applied: exclude markers if not computed from genetic map" << std::endl;
	this->any = true;
	this->remove_if_source_unknown_ = true;
}


FilterInput::FilterSampleInfo::FilterSampleInfo()
: any(false)
, remove_key_(false)
{}

void FilterInput::FilterSampleInfo::remove_key(const std::string & filename)
{
	std::ifstream line(filename);
	std::string key;
	
	while (line >> key)
	{
		this->remove_key_set.insert(key);
	}
	
	std::clog << "Filter applied: exclude " << this->remove_key_set.size() << " sample IDs" << std::endl;
	this->any = true;
	this->remove_key_ = true;
}


bool FilterInput::apply(const MarkerInfo & info)
{
	if (! this->markerinfo.any)
		return true;
	
	if (this->markerinfo.remove_if_contains_snp_ &&
		info.allele.contains_snp())
	{
		this->comment = "Marker excluded: contains SNPs";
		return false;
	}
	
	if (this->markerinfo.remove_if_contains_indel_ &&
		info.allele.contains_indel())
	{
		this->comment = "Marker excluded: contains INDELs";
		return false;
	}
	
	if (this->markerinfo.remove_if_contains_other_ &&
		info.allele.contains_other())
	{
		this->comment = "Marker excluded: contains invalid allele definitions";
		return false;
	}
	
	if (this->markerinfo.remove_if_biallelic_ &&
		info.allele.size() == 2)
	{
		this->comment = "Marker excluded: is bi-allelic";
		return false;
	}
	
	if (this->markerinfo.remove_if_multiallelic_ &&
		info.allele.size() > 2)
	{
		this->comment = "Marker excluded: is multi-allelic";
		return false;
	}
	
	if (this->markerinfo.remove_pos_ &&
		this->markerinfo.remove_pos_set.count(info.pos) != 0)
	{
		this->comment = "Marker excluded: at specified position '" + std::to_string(info.pos) + "'";
		return false;
	}
	
	return true;
}

bool FilterInput::apply(const MarkerData & data)
{
	if (! this->markerdata.any)
		return true;
	
	if (this->markerdata.remove_if_contains_unknown_ &&
		data.contains_unknown())
	{
		this->comment = "Marker excluded: contains invalid sample alleles";
		return false;
	}
	
	return true;
}

bool FilterInput::apply(const MarkerStat & stat)
{
	if (! this->markerstat.any)
		return true;
	
	int i;
	const int h_size = stat.haplotype.size();
	const int g_size = stat.genotype.size();
	
	if (this->markerstat.remove_hap_below_)
	{
		for (i = 0; i < h_size; ++i)
		{
			if (stat.haplotype.census(i) <= this->markerstat.remove_hap_below_cutoff)
			{
				this->comment = "Marker excluded: allele count/frequency below threshold";
				return false;
			}
		}
	}
	
	if (this->markerstat.remove_hap_above_)
	{
		for (i = 0; i < h_size; ++i)
		{
			if (stat.haplotype.census(i) >= this->markerstat.remove_hap_above_cutoff)
			{
				this->comment = "Marker excluded: allele count/frequency above threshold";
				return false;
			}
		}
	}
	
	if (this->markerstat.remove_gen_below_)
	{
		for (i = 0; i < g_size; ++i)
		{
			if (stat.genotype.census(i) <= this->markerstat.remove_gen_below_cutoff)
			{
				this->comment = "Marker excluded: genotype count/frequency below threshold";
				return false;
			}
		}
	}
	
	if (this->markerstat.remove_gen_above_)
	{
		for (i = 0; i < g_size; ++i)
		{
			if (stat.genotype.census(i) >= this->markerstat.remove_gen_above_cutoff)
			{
				this->comment = "Marker excluded: genotype count/frequency above threshold";
				return false;
			}
		}
	}
	
	return true;
}

bool FilterInput::apply(const MarkerGmap & gmap)
{
	if (! this->markergmap.any)
		return true;
	
	if (this->markergmap.remove_if_source_interpolated_ &&
		gmap.source == MarkerGmap::interpolated)
	{
		this->comment = "Marker excluded: was interpolated from genetic map";
		return false;
	}
	
	if (this->markergmap.remove_if_source_extrapolated_ &&
		gmap.source == MarkerGmap::extrapolated)
	{
		this->comment = "Marker excluded: was extrapolated from genetic map";
		return false;
	}
	
	if (this->markergmap.remove_if_source_unknown_&&
		gmap.source == MarkerGmap::unknown)
	{
		this->comment = "Marker excluded: unable to approxmiate from genetic map";
		return false;
	}
	
	return true;
}

bool FilterInput::apply(const SampleInfo & info)
{
	if (! this->sampleinfo.any)
		return true;
	
	if (this->sampleinfo.remove_key_ &&
		this->sampleinfo.remove_key_set.count(info.key) != 0)
	{
		this->comment = "Sample excluded: identifier '" + info.key + "' excluded";
		return false;
	}
	
	return true;
}


//
// Load line and other files
//

Input_VCF::Input_VCF(const std::string & filename)
: line(filename)
, size(0)
, sample_(false)
, genmap_(false)
{
	bool flag = false;
	std::string last;
	const size_t vcf_n = vcf_required_columns.size();
	
	// read first line
	if (!this->line.next())
	{
		throw std::invalid_argument(this->error("Cannot read from VCF file"));
	}
	
	// check format
	if (std::string(this->line, 16) != "##fileformat=VCF")
	{
		throw std::invalid_argument(this->error("Input file not in Variant Call Format"));
	}
	
	// read last header line
	while (this->line.next())
	{
		if (this->line[0] != '#') // until line is no header
		{
			flag = true;
			break;
		}
		
		last = this->line;
	}
	
	if (!flag)
	{
		throw std::invalid_argument(this->error("Input file does not contain data"));
	}
	
	
	StreamSplit token(last);
	
	while (token.next())
	{
		if (token.count() <= vcf_n)
		{
			// check required columns
			if (token.str() != vcf_required_columns[ token.count() - 1 ])
			{
				throw std::invalid_argument(this->error("Invalid line file format\n"
														"Column '" + vcf_required_columns[ token.count() - 1 ] + "' "
														"is missing"));
			}
			
			continue;
		}
		
		// store sample information
		SampleInfo info;
		info.key = token;
		this->_sample.push_back(info);
		++this->size;
	}
}

void Input_VCF::log(const std::string & comment)
{
	std::clog << comment << " (line " << this->line.count() << ")" << std::endl;
}

std::string Input_VCF::error(const std::string & comment)
{
	std::ostringstream err;
	err << comment << "\n";
	err << "Line: " << this->line.count() << "\n";
	err << "File: " << this->line.source();
	return err.str();
}

void Input_VCF::sample(const std::string & filename)
{
	size_t n = 0;
	
	StreamLine line(filename);
	
	// skip header
	if (! line.next())
	{
		throw std::invalid_argument("Cannot read from sample file");
	}
	
	std::cout << "Loading sample file" << std::endl;
	std::clog << "Loading sample file: " << line.source() << std::endl;
	ProgressMsg progress("lines");
	Runtime timer;
	
	// walkabout
	while (line.next())
	{
		SampleInfo info;
		
		progress.update();
		
		if(parse_sample_line(line, info))
		{
			if (info.key != _sample[n].key)
			{
				throw std::invalid_argument("Sample IDs do not match\n"
											"In line file:  '" + _sample[n].key + "'\n"
											"In sample file: '" + info.key + "' (line " + std::to_string(line.count()) +  ")");
			}
			
			// overwrite
			_sample[n] = std::move(info);
			++n;
			continue;
		}
		
		throw std::invalid_argument("Invalid sample file format\n"
									"Cannot parse sample information on line " + std::to_string(line.count()));
	}
	
	if (n != this->size)
	{
		throw std::length_error("Sample file contains unexpected number of samples\n"
								"Expected from line file: " + std::to_string(this->size) + "\n"
								"Detected in sample file:  " + std::to_string(n));
	}
	
	progress.finish(this->line.count());
	std::clog << "Done! " << timer.str() << std::endl << std::endl;
	
	this->sample_ = true;
}

void Input_VCF::genmap(const std::string & filename)
{
	size_t cols;
	
	StreamLine line(filename);
	StreamSplit token(line);
	
	// count columns
	while (line.next()) {}
	cols = line.count();
	
	// skip header
	if (! line.next())
	{
		throw std::invalid_argument("Cannot read from genetic map");
	}
	
	std::cout << "Loading genetic map" << std::endl;
	std::clog << "Loading genetic map: " << line.source() << std::endl;
	ProgressMsg progress("lines");
	Runtime timer;
	
	// walkabout
	while (line.next())
	{
		GenmapInfo info;
		
		progress.update();
		
		switch (cols)
		{
			case 3:
			{
				if (parse_genmap_3col(line, info))
				{
					try
					{
						this->_genmap.insert(info);
					}
					catch (const std::exception & x)
					{
						throw std::invalid_argument(std::string("Error while building genetic map\n") + x.what());
					}
				}
				else
				{
					throw std::invalid_argument("Invalid genetic map format\n"
												"Cannot parse information on line " + std::to_string(line.count()));
				}
				break;
			}
			case 4:
			{
				if (parse_genmap_4col(line, info))
				{
					try
					{
						this->_genmap.insert(info);
					}
					catch (const std::exception & x)
					{
						throw std::invalid_argument(std::string("Error while building genetic map\n") + x.what());
					}
				}
				else
				{
					throw std::invalid_argument("Invalid genetic map format\n"
												"Cannot parse information on line " + std::to_string(line.count()));
				}
				break;
			}
			default:
			{
				throw std::invalid_argument("Cannot handle genetic map files with " + std::to_string(cols) + " columns");
				break;
			}
		}
	}
	
	progress.finish(this->line.count());
	std::clog << "Done! " << timer.str() << std::endl << std::endl;
	
	this->genmap_ = true;
}

void Input_VCF::run(Source & source)
{
	// source samples
	std::vector<size_t> skip; // skip sample index
	size_t n_skip; // count skipped samples
	bool skip_ = false; // flag that samples were skipped
	
	for (size_t i = 0; i < this->size; ++i)
	{
		// filter sample
		if (this->filter.apply(this->_sample[i]))
		{
			Sample sample;
			sample.info = std::move(this->_sample[i]);
			source.append(std::move(sample));
			continue;
		}
		
		std::clog << this->filter.comment << std::endl;
		
		skip.push_back(i);
		++n_skip;
		skip_ = true;
	}
	
	std::cout << "Loading input data" << std::endl;
	std::clog << "Loading input data: " << this->line.source() << std::endl;
	ProgressMsg progress("lines");
	Runtime timer;
	
	std::string comment;
	
	// source markers
	std::unordered_set<size_t> unique_pos;
	
	do
	{
		Marker marker(this->size);
		
		progress.update();
		
		// parse marker
		if (parse_vcf_line(this->line, marker.info, marker.data, comment))
		{
			// check unique position
			if (unique_pos.count(marker.info.pos) != 0)
			{
				this->log("Duplicate maker position");
				continue;
			}
			unique_pos.insert(marker.info.pos);
			
			// remove skipped samples
			if (skip_)
			{
				for (size_t i = 0; i < n_skip; ++i)
				{
					if (! marker.data.erase(skip[i] - i)) // account for reduced size after previous skips
					{
						throw std::logic_error("Unexpected error while removing samples");
					}
				}
			}
			
			// evaluate marker stats
			if (! marker.stat.evaluate(marker.info, marker.data))
			{
				std::cout << marker.info.str() << std::endl;
				this->log("Invalid allele definition");
				continue;
			}
			
			// genetic map
			if (this->genmap_)
			{
				marker.gmap = this->_genmap.approx(marker.info);
			}
			
			// filter marker
			if (this->filter.apply(marker.info) &&
				this->filter.apply(marker.data) &&
				this->filter.apply(marker.stat) &&
				this->filter.apply(marker.gmap) )
			{
				source.append(std::move(marker));
				continue;
			}
			
			comment = this->filter.comment;
		}
		
		this->log(comment);
	}
	while (this->line.next());
	
	progress.finish(this->line.count());
	std::clog << "Done! " << timer.str() << std::endl << std::endl;
}


