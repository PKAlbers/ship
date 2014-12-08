//
//  inputs.h
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship__input__
#define __ship__input__

#include <stdio.h>
#include <string>
#include <vector>
#include <array>
#include <utility>

#include "types.h"
#include "sample.h"
#include "marker.h"
#include "source.h"
#include "census.h"
#include "genmap.h"
#include "stream.h"

// String Toolkit Library, URL: http://www.partow.net/programming/strtk/index.html
#define strtk_no_tr1_or_boost
#include "strtk.hpp"



// constants
const int sample_headers = 1; // # header lines in sample file
const int legend_headers = 1; // # header lines in legend file
const int genmap_headers = 1; // # header lines in genmap file

const int hap_set_format = 2; // # genotype columns corresponding to one individual
const int gen_set_format = 3; // # haplotype columns corresponding to one individual
const int vcf_col_format = 9; // # marker columns in VCF file
const std::vector<std::string> vcf_col_header = { "#CHROM", "POS", "ID", "REF", "ALT", "QUAL", "FILTER", "INFO", "FORMAT" }; // expected column headers in VCF file



//******************************************************************************
// Parse file lines
//******************************************************************************

// parse sample line
inline bool parse_sample_line_3col(char *, SampleInfo &);
inline bool parse_sample_line_2col(char *, SampleInfo &);
inline bool parse_sample_line_1col(char *, SampleInfo &);

// parse legend line
inline bool parse_legend_line_6col(char *, MarkerInfo &);
inline bool parse_legend_line_5col(char *, MarkerInfo &);
inline bool parse_legend_line_4col(char *, MarkerInfo &);
inline bool parse_legend_line_3col(char *, MarkerInfo &);

// parse genmap line
inline bool parse_genmap_line_4col(char *, GenmapInfo &);
inline bool parse_genmap_line_3col(char *, GenmapInfo &);

// parse VCF file line
inline bool parse_vcf_full(char *, MarkerInfo &, MarkerData &);

// parse HAP file line
inline bool parse_hap_full_6col(char *, MarkerInfo &, MarkerData &);
inline bool parse_hap_full_5col(char *, MarkerInfo &, MarkerData &);
inline bool parse_hap_full_4col(char *, MarkerInfo &, MarkerData &);
inline bool parse_hap_full_3col(char *, MarkerInfo &, MarkerData &);
inline bool parse_hap_data(char *, MarkerData &);

// parse GEN file line
inline bool parse_gen_full_6col(char *, MarkerInfo &, MarkerData &);
inline bool parse_gen_full_5col(char *, MarkerInfo &, MarkerData &);
inline bool parse_gen_full_4col(char *, MarkerInfo &, MarkerData &);
inline bool parse_gen_full_3col(char *, MarkerInfo &, MarkerData &);
inline bool parse_gen_data(char *, MarkerData &);



//******************************************************************************
// Parse files
//******************************************************************************

//
// Base class
//
template <class Container>
struct ParseFile : public ReadStream
{
	// load sample from current line
	virtual bool parse(Container &) = 0;
	
protected:
	
	// construct
	ParseFile(const std::string & filename, const unsigned int skip_header_lines)
	: ReadStream(filename)
	{
		for (unsigned int i = 0; i < skip_header_lines; ++i)
		{
			if (! this->readline())
				throw std::runtime_error("Cannot read header in '" + this->name + "'");
		}
	}
	
	// destruct
	~ParseFile()
	{
		this->close();
	}
};


//
// Parse sample file
//
struct ParseSample : public ParseFile<SampleInfo>
{
	// dynamic function choice, dependent on ncols
	bool (*_parse)(char *, SampleInfo &);
	
	bool parse(SampleInfo & info)
	{
		if (this->_parse(this->line, info))
		{
			return true;
		}
		
		std::clog << "Cannot parse sample file: line " << this->count << std::endl;
		
		return false;
	}
	
	ParseSample(const std::string & filename)
	: ParseFile<SampleInfo>(filename, sample_headers)
	{
		unsigned long n = this->count_fields();
		
		if (n >= 3) this->_parse = &parse_sample_line_3col; return;
		if (n == 2) this->_parse = &parse_sample_line_2col; return;
		if (n == 1) this->_parse = &parse_sample_line_1col; return;
		
		throw std::invalid_argument(this->error("Cannot read from sample files with " + std::to_string(n) + " columns", true));
	}
};


//
// Parse legend file
//
struct ParseLegend : public ParseFile<MarkerInfo>
{
	// dynamic function choice, dependent on ncols
	bool (*_parse)(char *, MarkerInfo &);
	
	bool parse(MarkerInfo & info)
	{
		if (this->_parse(this->line, info))
		{
			return true;
		}
		
		std::clog << "Cannot parse legend file: line " << this->count << std::endl;
		
		return false;
	}
	
	ParseLegend(const std::string & filename)
	: ParseFile<MarkerInfo>(filename, legend_headers)
	{
		unsigned long n = this->count_fields();
		
		if (n >= 6) this->_parse = &parse_legend_line_6col; return;
		if (n == 5) this->_parse = &parse_legend_line_5col; return;
		if (n == 4) this->_parse = &parse_legend_line_4col; return;
		if (n == 3) this->_parse = &parse_legend_line_3col; return;
		
		throw std::invalid_argument(this->error("Cannot read from legend files with " + std::to_string(n) + " columns", true));
	}
};


//
// Parse genmap file
//
struct ParseGenmap : public ParseFile<GenmapInfo>
{
	// dynamic function choice, dependent on ncols
	bool (*_parse)(char *, GenmapInfo &);
	
	bool parse(GenmapInfo & info)
	{
		if (this->_parse(this->line, info))
		{
			return true;
		}
		
		std::clog << "Cannot parse genetic map file: line " << this->count << std::endl;
		
		return false;
	}
	
	ParseGenmap(const std::string & filename)
	: ParseFile<GenmapInfo>(filename, genmap_headers)
	{
		unsigned long n = this->count_fields();
		
		if (n >= 4) this->_parse = &parse_genmap_line_4col; return;
		if (n == 3) this->_parse = &parse_genmap_line_3col; return;
		
		throw std::invalid_argument(this->error("Cannot read from genetic map files with " + std::to_string(n) + " columns", true));
	}
};









//
// Load legend file
//
struct ParseLegend : public ParseFile<MarkerInfo>
{
	// dynamic function choice, dependent on ncols
	bool (*_parse)(char *, SampleInfo &);
	
	bool parse(MarkerInfo &);
	ParseLegend(const std::string &);
};


//
// Load genmap file
//
struct ParseGenmap : public ParseFile<GenmapInfo>
{
	// dynamic function choice, dependent on ncols
	bool (*_parse)(char *, SampleInfo &);
	
	bool parse(GenmapInfo &);
	ParseGenmap(const std::string &);
};



//**************************************************************************
// Read data file
//**************************************************************************

bool format_is_vcf(char *);
bool format_is_hap(char *, unsigned long &, unsigned long &);
bool format_is_gen(char *, unsigned long &, unsigned long &);

//
// Base class
//
struct InputData
{
	InputStream stream; // read from file
	
	unsigned long header_rows; // number of header lines
	unsigned long marker_cols; // count marker columns
	unsigned long sample_cols; // count sample columns
	
	bool require_sample; // flag that sample file is required
	bool require_legend; // flag that legend file is required
	bool require_genmap; // flag that genetic map file is required
	
	std::vector<std::string> header; // collect header lines
	
	unsigned long i; // count lines read, excluding header
	
	// load data from current line
	virtual bool full(MarkerInfo &, MarkerData &) = 0; // full: info + data
	virtual bool data(MarkerData &) = 0; // data only
	
protected:
	
	// construct
	InputData(const std::string & filename);
};


//
// Load from VCF file
//
struct InputData_VCF : public InputData
{
	bool full(MarkerInfo &, MarkerData &);
	bool data(MarkerData &);
	InputData_VCF(const std::string &);
};


//
// Load from HAP file
//
struct InputData_HAP : public InputData
{
	bool full(MarkerInfo &, MarkerData &);
	bool data(MarkerData &);
	InputData_HAP(const std::string &);
};


//
// Load from GEN file
//
struct InputData_GEN : public InputData
{
	bool full(MarkerInfo &, MarkerData &);
	bool data(MarkerData &);
	InputData_GEN(const std::string &);
};




/* 
 *  INLINE 
 */


//
// parse sample line
//

inline bool parse_sample_line_3col(char * l, SampleInfo & info)
{
	std::string key, grp, pop;
	
	if (strtk::parse_columns(l, " \t", strtk::column_list(0, 1, 2), key, grp, pop)) // ignore additional columns
	{
		info.key = std::move(key); key.clear();
		info.grp = std::move(grp); grp.clear();
		info.pop = std::move(pop); pop.clear();
		return true;
	}
	
	return false;
}

inline bool parse_sample_line_2col(char * l, SampleInfo & info)
{
	std::string key, pop;
	
	if (strtk::parse_columns(l, " \t", strtk::column_list(0, 1), key, pop))
	{
		info.key = std::move(key); key.clear();
		info.pop = std::move(pop); pop.clear();
		return true;
	}
	
	return false;
}

inline bool parse_sample_line_1col(char * l, SampleInfo & info)
{
	std::string key;
	
	if (strtk::parse_columns(l, " \t", strtk::column_list(0), key))
	{
		info.key = std::move(key); key.clear();
		return true;
	}
	
	return false;
}



//
// parse legend line
//

inline bool parse_legend_line_6col(char * l, MarkerInfo & info)
{
	int chr;
	unsigned long pos;
	std::string id0, id1, a0, a1;
	
	// chr snpID rsID pos allele0 allele1
	if (strtk::parse_columns(l, " \t", strtk::column_list(0, 1, 2, 3, 4, 5), chr, id0, id1, pos, a0, a1))
	{
		info.chr = chr;
		info.pos = pos;
		info.id0 = std::move(id0); id0.clear();
		info.id1 = std::move(id1); id1.clear();
		info.allele(Allele(std::move(a0))); a0.clear();
		info.allele(Allele(std::move(a1))); a1.clear();
	}
	
	return false;
}

inline bool parse_legend_line_5col(char * l, MarkerInfo & info)
{
	unsigned long pos;
	std::string id0, id1, a0, a1;
	
	// snpID rsID pos allele0 allele1
	if (strtk::parse_columns(l, " \t", strtk::column_list(0, 1, 2, 3, 4), id0, id1, pos, a0, a1))
	{
		info.pos = pos;
		info.id0 = std::move(id0); id0.clear();
		info.id1 = std::move(id1); id1.clear();
		info.allele(Allele(std::move(a0))); a0.clear();
		info.allele(Allele(std::move(a1))); a1.clear();
		return true;
	}
	
	return false;
}

inline bool parse_legend_line_4col(char * l, MarkerInfo & info)
{
	unsigned long pos;
	std::string id1, a0, a1;
	
	// rsID pos allele0 allele1
	if (strtk::parse_columns(l, " \t", strtk::column_list(0, 1, 2, 3), id1, pos, a0, a1))
	{
		info.pos = pos;
		info.id1 = std::move(id1); id1.clear();
		info.allele(Allele(std::move(a0))); a0.clear();
		info.allele(Allele(std::move(a1))); a1.clear();
		return true;
	}
	
	return false;
}

inline bool parse_legend_line_3col(char * l, MarkerInfo & info)
{
	unsigned long pos;
	std::string a0, a1;
	
	// pos allele0 allele1
	if (strtk::parse_columns(l, " \t", strtk::column_list(0, 1, 2), pos, a0, a1))
	{
		info.pos = pos;
		info.allele(Allele(std::move(a0))); a0.clear();
		info.allele(Allele(std::move(a1))); a1.clear();
		return true;
	}
	
	return false;
}



//
// parse genetic map line
//

inline bool parse_genmap_line_4col(char * l, GenmapInfo & gmap)
{
	int chr;
	unsigned long pos;
	double rate, dist;
	
	if (strtk::parse_columns(l, " \t", strtk::column_list(0, 1, 2, 3), chr, pos, rate, dist))
	{
		gmap.chr  = chr;
		gmap.pos  = pos;
		gmap.rate = rate;
		gmap.dist = dist;
		return true;
	}
	
	return false;
}

inline bool parse_genmap_line_3col(char * l, GenmapInfo & gmap)
{
	unsigned long pos;
	double rate, dist;
	
	if (strtk::parse_columns(l, " \t", strtk::column_list(0, 1, 2), pos, rate, dist))
	{
		gmap.pos  = pos;
		gmap.rate = rate;
		gmap.dist = dist;
		return true;
	}
	
	return false;
}



//
// parse VCF line
//

inline bool _parse_vcf_alleles(const std::string & ref, const std::string & alts, MarkerInfo & info)
{
	// ref allele
	if (!info.allele(Allele(std::move(ref))))
		return false;
	
	std::vector<std::string> alt;
	
	// alt alleles
	if (strtk::parse(alts, ",", alt)) // parse
	{
		for (unsigned long i = 0, size = alt.size(); i < size; ++i)
		{
			if (!info.allele(Allele(std::move(alt[i]))))
				return false;
		}
	}
	
	return true;
}

inline bool _parse_vcf_genotypes(const std::vector<std::string> & raw, MarkerData & data)
{
	unsigned long size = raw.size();
	
	// check size
	if (size != data.size())
		return false;
	
	// walkabout genotypes
	for (unsigned long i = 0; i < size; ++i)
	{
		char h0, h1, ph;
		
		h0 = raw[i][0]; // 1st haplotype
		ph = raw[i][1]; // phasing
		h1 = raw[i][2]; // 2nd haplotype
		
		// append genotype
		if (! data.append(h0, h1, ((ph == '|') ? true: false))) // determine phase
			return false;
	}
	
	return data.is_complete();
}

inline bool parse_vcf_full(char * l, MarkerInfo & info, MarkerData & data)
{
	int chr;
	unsigned long pos;
	std::string id1, allele_ref, allele_alt, quality_field, filter_field, info_field, format_field;
	std::vector<std::string> raw;
	
	// reserve vector size
	raw.reserve(data.size());
	
	// parse line
	if (strtk::parse(l, " \t", // CHROM POS ID REF ALT QUAL FILTER INFO FORMAT
					 chr, pos, id1, allele_ref, allele_alt, quality_field, filter_field, info_field, format_field, raw))
		return false;
	
	// parse marker information
	if (filter_field != "PASS") return false; // must have passed filtering
	if (format_field[0] != 'G' || format_field[1] != 'T') return false; // format must have leading "GT"
	
	info.chr = chr;
	info.pos = pos;
	info.id1 = std::move(id1); id1.clear();
	
	// parse alleles
	if (! _parse_vcf_alleles(allele_ref, allele_alt, info))
		return false;
	
	// parse raw genotypes
	if (! _parse_vcf_genotypes(raw, data))
		return false;
	
	return true;
}


//
// parse HAP line
//

inline bool _parse_hap_genotypes(const std::vector<int> & raw, MarkerData & data)
{
	unsigned long size = raw.size();
	
	// check size
	if (size != (data.size() * 2))
		return false;
	
	// walkabout genotypes
	for (unsigned long i = 0; i < size; )
	{
		Haplotype h0(raw[i++]);
		Haplotype h1(raw[i++]);
		
		// append genotype
		if (! data.append(h0, h1, false)) // ignore phasing
			return false;
	}
	
	return data.is_complete();
}

inline bool parse_hap_full_6col(char * l, MarkerInfo & info, MarkerData & data)
{
	int  chr;
	unsigned long pos;
	std::string id0, id1, a0, a1;
	std::vector<int> raw;
	
	// reserve vector size
	raw.reserve(data.size() * 2);
	
	if (strtk::parse(l, " ", chr, id0, id1, pos, a0, a1, raw)) // chr snpID rsID pos allele0 allele1
	{
		info.chr = chr;
		info.pos = pos;
		info.id0 = std::move(id0); id0.clear();
		info.id1 = std::move(id1); id1.clear();
		info.allele(Allele(std::move(a0))); a0.clear();
		info.allele(Allele(std::move(a1))); a1.clear();
		return _parse_hap_genotypes(raw, data);
	}
	
	return false;
}

inline bool parse_hap_full_5col(char * l, MarkerInfo & info, MarkerData & data)
{
	unsigned long pos;
	std::string id0, id1, a0, a1;
	std::vector<int> raw;
	
	// reserve vector size
	raw.reserve(data.size() * 2);
	
	if (strtk::parse(l, " ", id0, id1, pos, a0, a1, raw)) // snpID rsID pos allele0 allele1
	{
		info.pos = pos;
		info.id0 = std::move(id0); id0.clear();
		info.id1 = std::move(id1); id1.clear();
		info.allele(Allele(std::move(a0))); a0.clear();
		info.allele(Allele(std::move(a1))); a1.clear();
		return _parse_hap_genotypes(raw, data);
	}
	
	return false;
}

inline bool parse_hap_full_4col(char * l, MarkerInfo & info, MarkerData & data)
{
	unsigned long pos;
	std::string id1, a0, a1;
	std::vector<int> raw;
	
	// reserve vector size
	raw.reserve(data.size() * 2);
	
	if (strtk::parse(l, " ", id1, pos, a0, a1, raw)) // rsID pos allele0 allele1
	{
		info.pos = pos;
		info.id1 = std::move(id1); id1.clear();
		info.allele(Allele(std::move(a0))); a0.clear();
		info.allele(Allele(std::move(a1))); a1.clear();
		return _parse_hap_genotypes(raw, data);
	}
	
	return false;
}

inline bool parse_hap_full_3col(char * l, MarkerInfo & info, MarkerData & data)
{
	unsigned long pos;
	std::string a0, a1;
	std::vector<int> raw;
	
	// reserve vector size
	raw.reserve(data.size() * 2);
	
	if (strtk::parse(l, " ", pos, a0, a1, raw)) // pos allele0 allele1
	{
		info.pos = pos;
		info.allele(Allele(std::move(a0))); a0.clear();
		info.allele(Allele(std::move(a1))); a1.clear();
		return _parse_hap_genotypes(raw, data);
	}
	
	return false;
}


inline bool parse_hap_data(char * l, MarkerData & data)
{
	std::vector<int> raw;
	
	// reserve vector size
	raw.reserve(data.size() * 2);
	
	if (strtk::parse(l, " ", raw))
	{
		return _parse_hap_genotypes(raw, data);
	}
	
	return false;
}


//
// parse GEN line
//

inline bool _parse_gen_genotypes(const std::vector<double> & raw, MarkerData & data)
{
	unsigned long size = raw.size();
	
	// check size
	if (size != (data.size() * 3))
		return false;
	
	// walkabout genotypes
	for (unsigned long i = 0; i < size; )
	{
		Haplotype h0, h1;
		double g[3];
		
		g[0] = raw[i++];
		g[1] = raw[i++];
		g[2] = raw[i++];
		
		// interpret genotype
		if (g[0] > g[1] && g[0] > g[2]) { h0 = 0; h1 = 0; } else // homozygous 0
		if (g[1] > g[0] && g[1] > g[2]) { h0 = 0; h1 = 1; } else // heterozygous
		if (g[2] > g[0] && g[2] > g[1]) { h0 = 1; h1 = 1; }      // homozygous 1
		
		// append genotype
		if (! data.append(h0, h1, false)) // unphased
			return false;
	}
	
	return data.is_complete();
}

inline bool parse_gen_full_6col(char * l, MarkerInfo & info, MarkerData & data)
{
	int  chr;
	unsigned long pos;
	std::string id0, id1, a0, a1;
	std::vector<double> raw;
	
	// reserve vector size
	raw.reserve(data.size() * 3);
	
	if (strtk::parse(l, " ", chr, id0, id1, pos, a0, a1, raw)) // chr snpID rsID pos allele0 allele1
	{
		info.chr = chr;
		info.pos = pos;
		info.id0 = std::move(id0); id0.clear();
		info.id1 = std::move(id1); id1.clear();
		info.allele(Allele(std::move(a0))); a0.clear();
		info.allele(Allele(std::move(a1))); a1.clear();
		return _parse_gen_genotypes(raw, data);
	}
	
	return false;
}

inline bool parse_gen_full_5col(char * l, MarkerInfo & info, MarkerData & data)
{
	unsigned long pos;
	std::string id0, id1, a0, a1;
	std::vector<double> raw;
	
	// reserve vector size
	raw.reserve(data.size() * 3);
	
	if (strtk::parse(l, " ", id0, id1, pos, a0, a1, raw)) // snpID rsID pos allele0 allele1
	{
		info.pos = pos;
		info.id0 = std::move(id0); id0.clear();
		info.id1 = std::move(id1); id1.clear();
		info.allele(Allele(std::move(a0))); a0.clear();
		info.allele(Allele(std::move(a1))); a1.clear();
		return _parse_gen_genotypes(raw, data);
	}
	
	return false;
}

inline bool parse_gen_full_4col(char * l, MarkerInfo & info, MarkerData & data)
{
	unsigned long pos;
	std::string id1, a0, a1;
	std::vector<double> raw;
	
	// reserve vector size
	raw.reserve(data.size() * 3);
	
	if (strtk::parse(l, " ", id1, pos, a0, a1, raw)) // rsID pos allele0 allele1
	{
		info.pos = pos;
		info.id1 = std::move(id1); id1.clear();
		info.allele(Allele(std::move(a0))); a0.clear();
		info.allele(Allele(std::move(a1))); a1.clear();
		return _parse_gen_genotypes(raw, data);
	}
	
	return false;
}

inline bool parse_gen_full_3col(char * l, MarkerInfo & info, MarkerData & data)
{
	unsigned long pos;
	std::string a0, a1;
	std::vector<double> raw;
	
	// reserve vector size
	raw.reserve(data.size() * 3);
	
	if (strtk::parse(l, " ", pos, a0, a1, raw)) // pos allele0 allele1
	{
		info.pos = pos;
		info.allele(Allele(std::move(a0))); a0.clear();
		info.allele(Allele(std::move(a1))); a1.clear();
		return _parse_gen_genotypes(raw, data);
	}
	
	return false;
}

inline bool parse_gen_data(char * l, MarkerData & data)
{
	std::vector<double> raw;
	
	// reserve vector size
	raw.reserve(data.size() * 3);
	
	if (strtk::parse(l, " ", raw))
	{
		return _parse_gen_genotypes(raw, data);
	}
	
	return false;
}



#endif /* defined(__ship__input__) */
