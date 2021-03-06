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
	std::clog << "Filter applied: exclude markers containing missing/undefined data" << std::endl;
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


bool FilterInput::apply(const MarkerInfo & info, std::string & comment)  const
{
	if (! this->markerinfo.any)
		return true;
	
	if (this->markerinfo.remove_if_contains_snp_ &&
		info.allele.contains_snp())
	{
		comment = "Marker excluded: contains SNPs";
		return false;
	}
	
	if (this->markerinfo.remove_if_contains_indel_ &&
		info.allele.contains_indel())
	{
		comment = "Marker excluded: contains INDELs";
		return false;
	}
	
	if (this->markerinfo.remove_if_contains_other_ &&
		info.allele.contains_other())
	{
		comment = "Marker excluded: contains invalid allele definitions";
		return false;
	}
	
	if (this->markerinfo.remove_if_biallelic_ &&
		info.allele.size() == 2)
	{
		comment = "Marker excluded: is bi-allelic";
		return false;
	}
	
	if (this->markerinfo.remove_if_multiallelic_ &&
		info.allele.size() > 2)
	{
		comment = "Marker excluded: is multi-allelic";
		return false;
	}
	
	if (this->markerinfo.remove_pos_ &&
		this->markerinfo.remove_pos_set.count(info.pos) != 0)
	{
		comment = "Marker excluded: at specified position '" + std::to_string(info.pos) + "'";
		return false;
	}
	
	return true;
}

bool FilterInput::apply(const MarkerData & data, std::string & comment) const
{
	if (! this->markerdata.any)
		return true;
	
	if (this->markerdata.remove_if_contains_unknown_ &&
		data.contains_unknown())
	{
		comment = "Marker excluded: contains invalid sample alleles";
		return false;
	}
	
	return true;
}

bool FilterInput::apply(const MarkerStat & stat, std::string & comment) const
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
				comment = "Marker excluded: allele count/frequency below threshold";
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
				comment = "Marker excluded: allele count/frequency above threshold";
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
				comment = "Marker excluded: genotype count/frequency below threshold";
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
				comment = "Marker excluded: genotype count/frequency above threshold";
				return false;
			}
		}
	}
	
	return true;
}

bool FilterInput::apply(const MarkerGmap & gmap, std::string & comment) const
{
	if (! this->markergmap.any)
		return true;
	
	if (this->markergmap.remove_if_source_interpolated_ &&
		gmap.source == MarkerGmap::interpolated)
	{
		comment = "Marker excluded: was interpolated from genetic map";
		return false;
	}
	
	if (this->markergmap.remove_if_source_extrapolated_ &&
		gmap.source == MarkerGmap::extrapolated)
	{
		comment = "Marker excluded: was extrapolated from genetic map";
		return false;
	}
	
	if (this->markergmap.remove_if_source_unknown_&&
		gmap.source == MarkerGmap::unknown)
	{
		comment = "Marker excluded: unable to approxmiate from genetic map";
		return false;
	}
	
	return true;
}

bool FilterInput::apply(const SampleInfo & info, std::string & comment) const
{
	if (! this->sampleinfo.any)
		return true;
	
	if (this->sampleinfo.remove_key_ &&
		this->sampleinfo.remove_key_set.count(info.key) != 0)
	{
		comment = "Sample excluded: identifier '" + info.key + "' excluded";
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
, good(true)
{
	size_t n_head = 0;
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
			
			// reset and skip header
			this->line.reset();
			for (size_t i = 0; i < n_head; ++i)
			{
				if (! this->line.next())
					throw std::runtime_error(this->error("Unable to handle input file"));
			}
			
			break;
		}
		
		last = this->line;
		++n_head;
	}
	
	if (!flag)
	{
		throw std::invalid_argument(this->error("Input file does not contain data"));
	}
	
	// parse sample ids
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

void Input_VCF::log(const std::string & comment, const size_t line_num)
{
	this->ex_log.lock();
	std::clog << comment << " (line " << line_num << ")" << std::endl;
	this->ex_log.unlock();
}

std::string Input_VCF::error(const std::string & comment) const
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
	
	StreamLine sample_line(filename);
	
	// skip header
	if (! sample_line.next())
	{
		throw std::invalid_argument("Cannot read from sample file");
	}
	
	std::cout << "Loading sample file" << std::endl;
	std::clog << "Loading sample file: " << sample_line.source() << std::endl;
	ProgressMsg progress("lines");
	Runtime timer;
	
	// walkabout
	while (sample_line.next())
	{
		SampleInfo info;
		
		progress.update();
		
		if(parse_sample_line(sample_line, info))
		{
			if (info.key != _sample[n].key)
			{
				throw std::invalid_argument("Sample IDs do not match\n"
											"In line file:  '" + _sample[n].key + "'\n"
											"In sample file: '" + info.key + "' (line " + std::to_string(sample_line.count()) +  ")");
			}
			
			// overwrite
			_sample[n] = std::move(info);
			++n;
			continue;
		}
		
		throw std::invalid_argument("Invalid sample file format\n"
									"Cannot parse sample information on line " + std::to_string(sample_line.count()));
	}
	
	if (n != this->size)
	{
		throw std::length_error("Sample file contains unexpected number of samples\n"
								"Expected from line file: " + std::to_string(this->size) + "\n"
								"Detected in sample file:  " + std::to_string(n));
	}
	
	progress.finish(sample_line.count());
	std::clog << "Done! " << timer.str() << std::endl << std::endl;
	
	this->sample_ = true;
}

void Input_VCF::genmap(const std::string & filename)
{
	size_t cols;
	
	StreamLine genmap_line(filename);
	
	// count columns & skip header
	if (! genmap_line.next())
	{
		throw std::invalid_argument("Cannot read from genetic map");
	}
	
	// count columns
	{
		StreamSplit token(genmap_line);
		while (token.next()) {}
		cols = token.count();
	}
	
	std::cout << "Loading genetic map" << std::endl;
	std::clog << "Loading genetic map: " << genmap_line.source() << std::endl;
	ProgressMsg progress("lines");
	Runtime timer;
	
	// walkabout
	while (genmap_line.next())
	{
		GenmapInfo info;
		
		progress.update();
		
		switch (cols)
		{
			case 3:
			{
				if (parse_genmap_3col(genmap_line, info))
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
												"Cannot parse information on line " + std::to_string(genmap_line.count()));
				}
				break;
			}
			case 4:
			{
				if (parse_genmap_4col(genmap_line, info))
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
												"Cannot parse information on line " + std::to_string(genmap_line.count()));
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
	
	// finish genetic map
	this->_genmap.finish();
	
	progress.finish(genmap_line.count());
	std::clog << "Done! " << timer.str() << std::endl << std::endl;
	
	this->genmap_ = true;
}

void Input_VCF::source_sample(Source & source)
{
	std::string comment;
	
	this->skipsample.size = 0;
	
	for (size_t i = 0; i < this->size; ++i)
	{
		// filter sample
		if (this->filter.apply(this->_sample[i], comment))
		{
			Sample sample;
			sample.info = std::move(this->_sample[i]);
			source.append(std::move(sample));
			continue;
		}
		
		std::clog << comment << std::endl;
		
		this->skipsample.index.push_back(i);
		++this->skipsample.size;
	}
	
	this->skipsample.flag = (this->skipsample.size != 0);
	
	this->_sample.clear();
}

void Input_VCF::source_marker(Source & source, ProgressMsg & progress)
{
	thread_local std::string comment;
	thread_local std::vector<char> current;
	thread_local size_t line_num;
	
	while(this->good)
	{
		this->ex_line.lock();
		
		// load next line
		if (this->line.next())
		{
			size_t n = strlen(this->line);
			current.resize(n + 1);
			strcpy(&current[0], this->line);
			current[n] = '\0';
			line_num = this->line.count();
			
			progress.update();
		}
		else
		{
			this->good = false;
			this->ex_line.unlock();
			break;
		}
		
		this->ex_line.unlock();
		
		Marker marker(this->size);
		
		// parse marker
		if (parse_vcf_line(&current[0], marker.info, marker.data, comment))
		{
			// remove skipped samples
			if (this->skipsample.flag)
			{
				for (thread_local size_t i = 0; i < this->skipsample.size; ++i)
				{
					if (! marker.data.erase(this->skipsample.index[i] - i)) // account for reduced size after previous skips
					{
						throw std::logic_error("Unexpected error while removing samples");
					}
				}
			}

			// approximate from genetic map
			if (this->genmap_)
			{
				marker.gmap = this->_genmap.approx(marker.info);
			}
			
			// evaluate marker stats
			if (marker.stat.evaluate(marker.info, marker.data))
			{
				// filter marker
				if (this->filter.apply(marker.info, comment) &&
					this->filter.apply(marker.data, comment) &&
					this->filter.apply(marker.stat, comment) &&
					this->filter.apply(marker.gmap, comment) )
				{
					this->ex_source.lock();
					source.append(std::move(marker));
					this->ex_source.unlock();
				}
				else
				{
					this->log(comment, line_num);
				}
			}
			else
			{
				this->log("Invalid allele definition: " + marker.info.str(), line_num);
			}
		}
		else
		{
			this->log(comment, line_num);
		}
	}
}

void Input_VCF::run(Source & source, const int threads)
{
	// source samples
	this->source_sample(source);
	
	std::cout << "Loading input data" << std::endl;
	std::clog << "Loading input data: " << this->line.source() << std::endl;
	ProgressMsg progress("lines");
	Runtime timer;
	
	// source markers
	if (threads > 1)
	{
		std::vector<std::thread> t;
		
		for (int i = 0; i < threads - 1; ++i)
		{
			t.push_back(std::thread(&Input_VCF::source_marker, this, std::ref(source),  std::ref(progress)));
		}
		
		this->source_marker(source, progress); // on this thread
		
		for (int i = 0; i < threads - 1; ++i)
		{
			t[i].join();
		}
	}
	else
	{
		this->source_marker(source, progress);
	}
	
	progress.finish(this->line.count());
	std::clog << "Done! " << timer.str() << std::endl << std::endl;
}


