//
//  parse.h
//  ship
//
//  Created by Patrick Albers on 21.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef ship_parse_h
#define ship_parse_h


#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <set>
#include <utility>
#include <functional>

#include "stream.h"
#include "sample.h"
#include "marker.h"
#include "genmap.h"







inline bool parse_vcf_line(const char * line, MarkerInfo & info, MarkerData & data, std::string & comment)
{
	ParseLine parse(line);
	
	std::set<std::string> unique; // check allele strings
	
	while (parse.next())
	{
		switch (parse.count)
		{
			case 0:
			{
				comment = "Line does not contain information";
				return false;
				break;
			}
			case 1: // CHROM
			{
				info.chr = (int)parse;
				break;
			}
			case 2: // POS
			{
				info.pos = (unsigned long)parse;
				break;
			}
			case 3: // ID
			{
				info.key = std::string(parse.token);
				break;
			}
			case 4: // REF
			{
				std::string allele = parse.token;
				unique.insert(allele);
				info.allele.append(std::move(allele));
				break;
			}
			case 5: // ALT
			{
				ParseLine alt(parse.token, ','); // parse multiple alleles
				while (alt.next())
				{
					std::string allele = alt.token;
					if (unique.count(allele) != 0)
					{
						comment = "Duplicate allele detected: '" + allele + "'";
						return false;
					}
					unique.insert(allele);
					info.allele.append(std::move(allele));
				}
				break;
			}
			case 6: // QUAL
			{
				// ignore
				break;
			}
			case 7: // FILTER
			{
				if (strcmp(parse.token, "PASS") != 0) // check filtering passed
				{
					comment = "Marker did not pass filtering";
					return false;
				}
				break;
			}
			case 8: // INFO
			{
				// ignore
				break;
			}
			case 9: // FORMAT
			{
				if (parse.token[0] != 'G' || parse.token[1] != 'T') // check genotype data
				{
					comment = "Format does not define genotype";
					return false;
				}
				break;
			}
			default:
			{
				// continue with data
				while (parse.next())
				{
					if (parse.width < 3)
					{
						comment = "Invalid genotype in column " + std::to_string(parse.count);
						return false;
					}
					
					Genotype g;
					g.h0 = parse.token[0];
					g.h1 = parse.token[2];
					
					data.append(g);
				}
				
				break;
			}
		}
	}
	
	if (parse.count < 10)
	{
		comment = "VCF file does not contain sample data";
		return false;
	}
	
	return true;
}













inline size_t TEST_parse_haplotypes(char * line, Genotype * type, const size_t size)
{
	size_t i = 0, n = 0;
	bool b = true;
	
	while (line[i] != '\0')
	{
		if (line[i] >= '0' && line[i] <= '9')
		{
			if (b)
			{
				type[n].h0 = line[i];
				//std::cout << line[i] << std::flush;
				b = false;
			}
			else
			{
				type[n].h1 = line[i];
				//std::cout << line[i] << std::flush;
				b = true;
				++n;
			}
		}
		
		++i;
	}
	//std::cout << size << " " << n << std::endl;
	return n;
}









/*
 *  String Toolkit Library
 *  URL: http://www.partow.net/programming/strtk/index.html
 */
#define strtk_no_tr1_or_boost
#include "strtk.hpp"



// constants
static const int sample_headers = 1; // # header lines in sample file
static const int legend_headers = 1; // # header lines in legend file
static const int genmap_headers = 1; // # header lines in genmap file

static const int hap_set_format = 2; // # genotype columns corresponding to one individual
static const int gen_set_format = 3; // # haplotype columns corresponding to one individual
static const int vcf_col_number = 9; // # required marker columns in VCF file

// expected column headers in VCF file
static const std::vector<std::string> vcf_col_header = {
	"#CHROM", "POS", "ID", "REF", "ALT", "QUAL", "FILTER", "INFO", "FORMAT"
};
static const std::string vcf_top_header = "##fileformat=VCF"; // first characters in VCF file



//******************************************************************************
// Parse file lines
//******************************************************************************

/*
// parse sample line
inline void parse_sample_line_3col(char *, SampleInfo &);
inline void parse_sample_line_2col(char *, SampleInfo &);
inline void parse_sample_line_1col(char *, SampleInfo &);

// parse legend line
inline void parse_legend_line_6col(char *, MarkerInfo &);
inline void parse_legend_line_5col(char *, MarkerInfo &);
inline void parse_legend_line_4col(char *, MarkerInfo &);
inline void parse_legend_line_3col(char *, MarkerInfo &);

// parse genmap line
inline void parse_genmap_line_4col(char *, GenmapInfo &);
inline void parse_genmap_line_3col(char *, GenmapInfo &);

// parse VCF file line
inline void parse_vcf_full(char *, MarkerInfo &, MarkerData &);

// parse HAP file line
inline void parse_hap_full_6col(char *, MarkerInfo &, MarkerData &);
inline void parse_hap_full_5col(char *, MarkerInfo &, MarkerData &);
inline void parse_hap_full_4col(char *, MarkerInfo &, MarkerData &);
inline void parse_hap_full_3col(char *, MarkerInfo &, MarkerData &);
inline void parse_hap_data(char *, MarkerData &);

// parse GEN file line
inline void parse_gen_full_6col(char *, MarkerInfo &, MarkerData &);
inline void parse_gen_full_5col(char *, MarkerInfo &, MarkerData &);
inline void parse_gen_full_4col(char *, MarkerInfo &, MarkerData &);
inline void parse_gen_full_3col(char *, MarkerInfo &, MarkerData &);
inline void parse_gen_data(char *, MarkerData &);
*/


//******************************************************************************
// Parse file lines
//******************************************************************************

//
// parse sample line
//

inline void parse_sample_line_3col(char * l, SampleInfo & info)
{
	std::string key, grp, pop;
	
	if (! strtk::parse_columns(l, " \t", strtk::column_list(0, 1, 2), key, grp, pop)) // ignore additional columns
	{
		throw std::string("Cannot parse line");
	}
	
	info.key = std::move(key);
	info.grp = std::move(grp);
	info.pop = std::move(pop);
}

inline void parse_sample_line_2col(char * l, SampleInfo & info)
{
	std::string key, pop;
	
	if (! strtk::parse_columns(l, " \t", strtk::column_list(0, 1), key, pop))
	{
		throw std::string("Cannot parse line");
	}
	
	info.key = std::move(key);
	info.pop = std::move(pop);
}

inline void parse_sample_line_1col(char * l, SampleInfo & info)
{
	std::string key;
	
	if (! strtk::parse_columns(l, " \t", strtk::column_list(0), key))
	{
		throw std::string("Cannot parse line");
	}
	
	info.key = std::move(key);
}



//
// parse legend line
//

inline void parse_legend_line_6col(char * l, MarkerInfo & info)
{
	int chr;
	unsigned long pos;
	std::string id0, id1, a0, a1;
	
	// chr snpID rsID pos allele0 allele1
	if (! strtk::parse_columns(l, " \t", strtk::column_list(0, 1, 2, 3, 4, 5), chr, id0, id1, pos, a0, a1))
	{
		throw std::string("Cannot parse line");
	}
	
	info.chr = chr;
	info.pos = pos;
	info.id0 = std::move(id0);
	info.id1 = std::move(id1);
	info.allele(Allele(std::move(a0)));
	info.allele(Allele(std::move(a1)));
}

inline void parse_legend_line_5col(char * l, MarkerInfo & info)
{
	unsigned long pos;
	std::string id0, id1, a0, a1;
	
	// snpID rsID pos allele0 allele1
	if (! strtk::parse_columns(l, " \t", strtk::column_list(0, 1, 2, 3, 4), id0, id1, pos, a0, a1))
	{
		throw std::string("Cannot parse line");
	}
	
	info.pos = pos;
	info.id0 = std::move(id0);
	info.id1 = std::move(id1);
	info.allele(Allele(std::move(a0)));
	info.allele(Allele(std::move(a1)));
}

inline void parse_legend_line_4col(char * l, MarkerInfo & info)
{
	unsigned long pos;
	std::string id1, a0, a1;
	
	// rsID pos allele0 allele1
	if (! strtk::parse_columns(l, " \t", strtk::column_list(0, 1, 2, 3), id1, pos, a0, a1))
	{
		throw std::string("Cannot parse line");
	}
	
	info.pos = pos;
	info.id1 = std::move(id1);
	info.allele(Allele(std::move(a0)));
	info.allele(Allele(std::move(a1)));
}

inline void parse_legend_line_3col(char * l, MarkerInfo & info)
{
	unsigned long pos;
	std::string a0, a1;
	
	// pos allele0 allele1
	if (! strtk::parse_columns(l, " \t", strtk::column_list(0, 1, 2), pos, a0, a1))
	{
		throw std::string("Cannot parse line");
	}
	
	info.pos = pos;
	info.allele(Allele(std::move(a0)));
	info.allele(Allele(std::move(a1)));
}



//
// parse genetic map line
//

inline void parse_genmap_line_4col(char * l, GenmapInfo & gmap)
{
	int chr;
	unsigned long pos;
	double rate, dist;
	
	if (! strtk::parse_columns(l, " \t", strtk::column_list(0, 1, 2, 3), chr, pos, rate, dist))
	{
		throw std::string("Cannot parse line");
	}
	
	gmap.chr  = chr;
	gmap.pos  = pos;
	gmap.rate = rate;
	gmap.dist = dist;
}

inline void parse_genmap_line_3col(char * l, GenmapInfo & gmap)
{
	unsigned long pos;
	double rate, dist;
	
	if (! strtk::parse_columns(l, " \t", strtk::column_list(0, 1, 2), pos, rate, dist))
	{
		throw std::string("Cannot parse line");
	}
	
	gmap.pos  = pos;
	gmap.rate = rate;
	gmap.dist = dist;
}



//
// parse VCF line
//

inline void _parse_vcf_alleles(const std::string & ref, const std::string & alt, MarkerInfo & info)
{
	// check multiple alt alleles
	if (alt.find(',') == std::string::npos) // if not multiple alt alleles
	{
		// check duplicate
		if (ref != alt)
		{
			throw std::string("Both alleles are identical");
		}
		
		info.allele(Allele(std::move(ref))); // move
		info.allele(Allele(std::move(alt))); // move
		return;
	}
	
	std::set<std::string> unique;
	unique.insert(ref);
	
	// insert ref allele
	info.allele(Allele(std::move(ref))); // move
	
	// parse alt alleles
	std::vector<std::string> alts;
	
	if (! strtk::parse(alt, ",", alts))
	{
		throw std::string("Cannot parse alleles");
	}
	
	for (std::string & a : alts)
	{
		// check duplicates
		if (unique.count(a) != 0)
		{
			throw std::string("Duplicate allele detected");
		}
		unique.insert(a);
		
		// insert alt allele
		info.allele(Allele(std::move(a))); // move
	}
}

inline void _parse_vcf_genotypes(const std::vector<std::string> & raw, MarkerData & data)
{
	unsigned long size = raw.size();
	
	// check size
	if (size != data.max())
	{
		throw std::string("Line contains unexpected genotype count");
	}
	
	// walkabout genotypes
	for (unsigned long i = 0; i < size; ++i)
	{
		Genotype G;
		
		G.h0     =  raw[i][0]; // 1st haplotype
		G.phased = (raw[i][1] == '|') ? true: false; // phasing
		G.h1     =  raw[i][2]; // 2nd haplotype
		
		// append genotype
		data.append(std::move(G)); // determine phase
	}
}

inline void parse_vcf_full(char * l, MarkerInfo & info, MarkerData & data)
{
	int chr;
	unsigned long pos;
	std::string id1, allele_ref, allele_alt, quality_field, filter_field, info_field, format_field;
	std::vector<std::string> raw;
	
	// reserve vector size
	raw.reserve(data.max());
	
	// parse line
	if (!strtk::parse(l, " \t", // CHROM POS ID REF ALT QUAL FILTER INFO FORMAT
					  chr, pos, id1, allele_ref, allele_alt, quality_field, filter_field, info_field, format_field, raw))
	{
		throw std::string("Cannot parse line");
	}
	
	// must have passed filtering
	if (filter_field != "PASS")
	{
		throw std::string("Marker did not pass filtering");
	}
	
	// format must have leading "GT"
	if (format_field[0] != 'G' || format_field[1] != 'T')
	{
		throw std::string("Marker not formated to contain genotype data");
	}
	
	info.chr = chr;
	info.pos = pos;
	info.id1 = std::move(id1); // move
	
	// parse alleles
	_parse_vcf_alleles(allele_ref, allele_alt, info);
	
	// parse raw genotypes
	_parse_vcf_genotypes(raw, data);
}


//
// parse HAP line
//

inline void _parse_hap_genotypes(const std::vector<int> & raw, MarkerData & data)
{
	unsigned long size = raw.size();
	
	// check size
	if (size != (data.max() * hap_set_format))
	{
		throw std::string("Line contains unexpected haplotype count");
	}
	
	// walkabout genotypes
	for (unsigned long i = 0; i < size; )
	{
		Genotype G;
		
		G.h0 = raw[i++];
		G.h1 = raw[i++];
		G.phased = true;
		
		// append genotype
		data.append(std::move(G));
	}
}

inline void parse_hap_full_6col(char * l, MarkerInfo & info, MarkerData & data)
{
	int  chr;
	unsigned long pos;
	std::string id0, id1, a0, a1;
	std::vector<int> raw;
	
	// reserve vector size
	raw.reserve(data.max() * hap_set_format);
	
	if (! strtk::parse(l, " ", chr, id0, id1, pos, a0, a1, raw)) // chr snpID rsID pos allele0 allele1
	{
		throw std::string("Cannot parse line");
	}
	
	info.chr = chr;
	info.pos = pos;
	info.id0 = std::move(id0); // move
	info.id1 = std::move(id1); // move
	info.allele(Allele(std::move(a0))); // move
	info.allele(Allele(std::move(a1))); // move
	
	_parse_hap_genotypes(raw, data);
}

inline void parse_hap_full_5col(char * l, MarkerInfo & info, MarkerData & data)
{
	unsigned long pos;
	std::string id0, id1, a0, a1;
	std::vector<int> raw;
	
	unsigned long size = data.max() * hap_set_format;
	
	// reserve vector size
	raw.reserve(size);
	
	if (! strtk::parse(l, " ", id0, id1, pos, a0, a1, raw)) // snpID rsID pos allele0 allele1
	{
		throw std::string("Cannot parse line");
	}
	
	info.pos = pos;
	info.id0 = std::move(id0); // move
	info.id1 = std::move(id1); // move
	info.allele(Allele(std::move(a0))); // move
	info.allele(Allele(std::move(a1))); // move
	
	_parse_hap_genotypes(raw, data);
}

inline void parse_hap_full_4col(char * l, MarkerInfo & info, MarkerData & data)
{
	unsigned long pos;
	std::string id1, a0, a1;
	std::vector<int> raw;
	
	// reserve vector size
	raw.reserve(data.max() * hap_set_format);
	
	if (! strtk::parse(l, " ", id1, pos, a0, a1, raw)) // rsID pos allele0 allele1
	{
		throw std::string("Cannot parse line");
	}
	
	info.pos = pos;
	info.id1 = std::move(id1); // move
	info.allele(Allele(std::move(a0))); // move
	info.allele(Allele(std::move(a1))); // move
	
	_parse_hap_genotypes(raw, data);
}

inline void parse_hap_full_3col(char * l, MarkerInfo & info, MarkerData & data)
{
	unsigned long pos;
	std::string a0, a1;
	std::vector<int> raw;
	
	// reserve vector size
	raw.reserve(data.max() * hap_set_format);
	
	if (! strtk::parse(l, " ", pos, a0, a1, raw)) // pos allele0 allele1
	{
		throw std::string("Cannot parse line");
	}
	
	info.pos = pos;
	info.allele(Allele(std::move(a0))); a0.clear(); // move
	info.allele(Allele(std::move(a1))); a1.clear(); // move
	
	_parse_hap_genotypes(raw, data);
}


inline void parse_hap_data(char * l, MarkerData & data)
{
	
	size_t size = TEST_parse_haplotypes(l, data.data, data.max());
	
	if (size != data.max())
		throw std::string("???");
	
	data.i = size;
	
	return;
	
	
	std::vector<int> raw;
	
	// reserve vector size
	raw.reserve(data.max() * hap_set_format);
	
	if (! strtk::parse(l, " ", raw))
	{
		throw std::string("Cannot parse line");
	}
	
	_parse_hap_genotypes(raw, data);
}


//
// parse GEN line
//

inline void _parse_gen_genotypes(const std::vector<double> & raw, MarkerData & data)
{
	unsigned long size = raw.size();
	
	// check size
	if (size != (data.max() * gen_set_format))
	{
		throw std::string("Line contains unexpected genotype count");
	}
	
	// walkabout genotypes
	for (unsigned long i = 0; i < size; )
	{
		Genotype G;
		double g[3];
		
		g[0] = raw[i++];
		g[1] = raw[i++];
		g[2] = raw[i++];
		
		// interpret genotype
		if (g[0] > g[1] && g[0] > g[2]) { G.h0 = 0; G.h1 = 0; } else // homozygous 0
		if (g[1] > g[0] && g[1] > g[2]) { G.h0 = 0; G.h1 = 1; } else // heterozygous
		if (g[2] > g[0] && g[2] > g[1]) { G.h0 = 1; G.h1 = 1; }      // homozygous 1
		
		G.phased = false;
		
		// append genotype
		data.append(std::move(G)); // unphased
	}
}

inline void parse_gen_full_6col(char * l, MarkerInfo & info, MarkerData & data)
{
	int  chr;
	unsigned long pos;
	std::string id0, id1, a0, a1;
	std::vector<double> raw;
	
	// reserve vector size
	raw.reserve(data.max() * gen_set_format);
	
	if (! strtk::parse(l, " ", chr, id0, id1, pos, a0, a1, raw)) // chr snpID rsID pos allele0 allele1
	{
		throw std::string("Cannot parse line");
	}
	
	info.chr = chr;
	info.pos = pos;
	info.id0 = std::move(id0); // move
	info.id1 = std::move(id1); // move
	info.allele(Allele(std::move(a0))); // move
	info.allele(Allele(std::move(a1))); // move
	
	_parse_gen_genotypes(raw, data);
}

inline void parse_gen_full_5col(char * l, MarkerInfo & info, MarkerData & data)
{
	unsigned long pos;
	std::string id0, id1, a0, a1;
	std::vector<double> raw;
	
	// reserve vector size
	raw.reserve(data.max() * gen_set_format);
	
	if (! strtk::parse(l, " ", id0, id1, pos, a0, a1, raw)) // snpID rsID pos allele0 allele1
	{
		throw std::string("Cannot parse line");
	}
	
	info.pos = pos;
	info.id0 = std::move(id0); // move
	info.id1 = std::move(id1); // move
	info.allele(Allele(std::move(a0))); // move
	info.allele(Allele(std::move(a1))); // move
	
	_parse_gen_genotypes(raw, data);
}

inline void parse_gen_full_4col(char * l, MarkerInfo & info, MarkerData & data)
{
	unsigned long pos;
	std::string id1, a0, a1;
	std::vector<double> raw;
	
	// reserve vector size
	raw.reserve(data.max() * gen_set_format);
	
	if (! strtk::parse(l, " ", id1, pos, a0, a1, raw)) // rsID pos allele0 allele1
	{
		throw std::string("Cannot parse line");
	}
	
	info.pos = pos;
	info.id1 = std::move(id1); id1.clear(); // move
	info.allele(Allele(std::move(a0))); a0.clear(); // move
	info.allele(Allele(std::move(a1))); a1.clear(); // move
	
	_parse_gen_genotypes(raw, data);
}

inline void parse_gen_full_3col(char * l, MarkerInfo & info, MarkerData & data)
{
	unsigned long pos;
	std::string a0, a1;
	std::vector<double> raw;
	
	// reserve vector size
	raw.reserve(data.max() * gen_set_format);
	
	if (! strtk::parse(l, " ", pos, a0, a1, raw)) // pos allele0 allele1
	{
		throw std::string("Cannot parse line");
	}
	
	info.pos = pos;
	info.allele(Allele(std::move(a0))); a0.clear(); // move
	info.allele(Allele(std::move(a1))); a1.clear(); // move
	return _parse_gen_genotypes(raw, data);
}

inline void parse_gen_data(char * l, MarkerData & data)
{
	std::vector<double> raw;
	
	// reserve vector size
	raw.reserve(data.max() * gen_set_format);
	
	if (! strtk::parse(l, " ", raw))
	{
		throw std::string("Cannot parse line");
	}
	
	_parse_gen_genotypes(raw, data);
}


/*
 *  defined below
 */

// log parsing error
inline void log_parse_error(const std::string & error, const unsigned long line)
{
	std::clog << error << ": line " << line << std::endl;
}



//******************************************************************************
// Input format defintions
//******************************************************************************

enum class InputFormat
{
	UNKNOWN,
	VCF,
	HAP,
	GEN
};



//******************************************************************************
// Parse files
//******************************************************************************

//
// Read sample file
//
struct ReadSample : public ReadStream
{
	// dynamic function choice, dependent on ncols
	std::function<void(char *, SampleInfo &)> _parse;
	
	// parse line
	bool parse(SampleInfo & info) const
	{
		try
		{
			this->_parse(this->line, info);
		}
		catch (const std::string & error)
		{
			// log parsing error
			log_parse_error(error, this->count);
			
			return false;
		}
		
		return true;
	}
	
	ReadSample(const std::string & filename)
	: ReadStream(filename)
	{
		unsigned long n = this->count_fields();
		
		// skip header
		for (unsigned int i = 0; i < sample_headers; ++i)
		{
			if (! this->read())
				throw std::runtime_error("Cannot read header in '" + this->name + "'");
		}
		
		// choose parser
		if (n >= 3) { this->_parse = parse_sample_line_3col; return; }
		if (n == 2) { this->_parse = parse_sample_line_2col; return; }
		if (n == 1) { this->_parse = parse_sample_line_1col; return; }
		
		throw std::invalid_argument(this->error("Cannot read from sample files with " + std::to_string(n) + " columns", true));
	}
};

//
// Read legend file
//
struct ReadLegend : public ReadStream
{
	// dynamic function choice, dependent on ncols
	std::function<void(char *, MarkerInfo &)> _parse;
	
	// parse line
	bool parse(MarkerInfo & info) const
	{
		try
		{
			this->_parse(this->line, info);
		}
		catch (const std::string & error)
		{
			// log parsing error
			log_parse_error(error, this->count);
			
			return false;
		}
		
		return true;
	}
	
	ReadLegend(const std::string & filename)
	: ReadStream(filename)
	{
		unsigned long n = this->count_fields();
		
		// skip header
		for (unsigned int i = 0; i < legend_headers; ++i)
		{
			if (! this->read())
				throw std::runtime_error("Cannot read header in '" + this->name + "'");
		}
		
		// choose parser
		if (n >= 6) { this->_parse = parse_legend_line_6col; return; }
		if (n == 5) { this->_parse = parse_legend_line_5col; return; }
		if (n == 4) { this->_parse = parse_legend_line_4col; return; }
		if (n == 3) { this->_parse = parse_legend_line_3col; return; }
		
		throw std::invalid_argument(this->error("Cannot read from legend files with " + std::to_string(n) + " columns", true));
	}
};


//
// Parse genmap file
//
struct ReadGenmap : public ReadStream
{
	// dynamic function choice, dependent on ncols
	void (*_parse)(char *, GenmapInfo &);
	
	// parse line
	bool parse(GenmapInfo & info) const
	{
		try
		{
			this->_parse(this->line, info);
		}
		catch (const std::string & error)
		{
			// log parsing error
			log_parse_error(error, this->count);
			
			return false;
		}
		
		return true;
	}
	
	ReadGenmap(const std::string & filename)
	: ReadStream(filename)
	{
		unsigned long n = this->count_fields();
		
		// skip header
		for (unsigned int i = 0; i < genmap_headers; ++i)
		{
			if (! this->read())
				throw std::runtime_error("Cannot read header in '" + this->name + "'");
		}
		
		// choose parser
		if (n >= 4) { this->_parse = &parse_genmap_line_4col; return; }
		if (n == 3) { this->_parse = &parse_genmap_line_3col; return; }
		
		throw std::invalid_argument(this->error("Cannot read from genetic map files with " + std::to_string(n) + " columns", true));
	}
};



//******************************************************************************
// Read input file in different formats
//******************************************************************************

struct ReadInput : public ReadStream
{
	const InputFormat format; // input file format
	
	std::vector<std::string> header; // collect header lines
	
	bool require_sample; // flag that external sample file is required
	bool require_legend; // flag that external legend file is required
	bool require_genmap; // flag that external genetic map is required
	
	// dynamic function choice, dependent on ncols
	std::function<void(char *, MarkerInfo &, MarkerData &)> full;
	std::function<void(char *, MarkerData &)> data;
	
	// parse as full line
	bool parse(MarkerInfo & info, MarkerData & data) const
	{
		try
		{
			this->full(this->line, info, data);
		}
		catch (const std::string & error)
		{
			// log parsing error
			log_parse_error(error, this->count);
			
			return false;
		}
		
		return true;
	}
	
	// parse as data line
	bool parse(MarkerData & data) const
	{
		try
		{
			this->data(this->line, data);
		}
		catch (const std::string & error)
		{
			// log parsing error
			log_parse_error(error, this->count);
			
			return false;
		}
		
		return true;
	}
	
	// construct
	ReadInput(const std::string & filename,
			   const InputFormat _format,
			   const unsigned long ncols,
			   const unsigned long nhead)
	: ReadStream(filename)
	, format(_format)
	, require_sample(false)
	, require_legend(false)
	, require_genmap(false)
	{
		// skip and store header lines
		for (unsigned long i = 0; i < nhead; ++i)
		{
			if (! this->read())
			{
				throw std::runtime_error(this->error("Cannor read header in input file", true, true));
			}
			
			this->header.push_back(this->line);
		}
		
		switch (format)
		{
			case InputFormat::VCF:
				
				this->full = parse_vcf_full;
				return;
				
			case InputFormat::HAP:
				
				switch (ncols)
				{
					case 6:
						this->full = parse_hap_full_6col;
						return;
					case 5:
						this->full = parse_hap_full_5col;
						return;
					case 4:
						this->full = parse_hap_full_4col;
						return;
					case 3:
						this->full = parse_hap_full_3col;
						return;
					case 0:
						this->data = parse_hap_data;
						this->require_legend = true;
						return;
				}
				throw std::invalid_argument(this->error("Cannot read from HAP files with " + std::to_string(ncols) + " legend columns", true));
				
			case InputFormat::GEN:
				
				switch (ncols)
				{
					case 6:
						this->full = parse_gen_full_6col;
						return;
					case 5:
						this->full = parse_gen_full_5col;
						return;
					case 4:
						this->full = parse_gen_full_4col;
						return;
					case 3:
						this->full = parse_gen_full_3col;
						return;
					case 0:
						this->data = parse_gen_data;
						this->require_legend = true;
						return;
				}
				throw std::invalid_argument(this->error("Cannot read from GEN files with " + std::to_string(ncols) + " legend columns", true));
			
			case InputFormat::UNKNOWN:
			default:
				throw std::invalid_argument(this->error("Input file is of unknown format", true));
		}
	}
	
	// destruct
	~ReadInput()
	{
		this->close();
	}
};




/*
 *  INLINE
 */




#endif
