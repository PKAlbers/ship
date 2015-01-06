//
//  token.hpp
//  ship
//
//  Created by Patrick Albers on 21.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef ship_token_hpp
#define ship_token_hpp


#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <utility>
#include <functional>

#include "stream.h"
#include "sample.h"
#include "marker.h"
#include "genmap.h"


//
// token line from sample file
//
inline bool parse_sample_line(char * line, SampleInfo & info)
{
	StreamSplit token(line);
	
	while (token.next())
	{
		switch (token.count())
		{
			case 1: // key
			{
				info.key = token;
				break;
			}
			case 2: // pop
			{
				info.pop = token;
				break;
			}
			case 3: // grp
			{
				info.grp = token;
				break;
			}
		}
	}
	
	return (token.count() != 0);
}


//
// token line from genetic map (with 3 columns)
//
inline bool parse_genmap_3col(char * line, GenmapInfo & info)
{
	StreamSplit token(line);
	
	while (token.next())
	{
		switch (token.count())
		{
			case 1: // position
			{
				if (! token.convert(info.pos))
				{
					return false;
				}
				break;
			}
			case 2: // recombination rate
			{
				if (! token.convert(info.rate))
				{
					return false;
				}
				break;
			}
			case 3: // genetic distance
			{
				if (! token.convert(info.dist))
				{
					return false;
				}
				break;
			}
		}
	}
	
	return (token.count() != 0);
}


//
// token line from genetic map (with 4 columns)
//
inline bool parse_genmap_4col(char * line, GenmapInfo & info)
{
	StreamSplit token(line);
	
	while (token.next())
	{
		switch (token.count())
		{
			case 1: // chromosome
			{
				int _chr;
				if (! token.convert(_chr))
				{
					if (token.size() > 3 && std::string(token, 3) == "chr")
					{
						_chr = std::stoi(std::string(token + 3));
					}
					else
					{
						return false;
					}
				}
				info.chr = _chr;
				break;
			}
			case 2: // position
			{
				if (! token.convert(info.pos))
				{
					return false;
				}
				break;
			}
			case 3: // recombination rate
			{
				if (! token.convert(info.rate))
				{
					return false;
				}
				break;
			}
			case 4: // genetic distance
			{
				if (! token.convert(info.dist))
				{
					return false;
				}
				break;
			}
		}
	}
	
	return (token.count() != 0);
}


//
// token line from VCF file
//
inline bool parse_vcf_line(char * line, MarkerInfo & info, MarkerData & data, std::string & comment)
{
	static std::unordered_set<std::string> unique; // check allele strings
	unique.clear();
	
	bool flag = false; // flag that genotypes were read
	int  conv;
	
	StreamSplit token(line);
	
	while (token.next())
	{
		switch (token.count())
		{
			case 1: // CHROM
			{
				if (! token.convert(conv))
				{
					comment = "Unable to determine chromosome";
					return false;
				}
				info.chr = conv;
				break;
			}
			case 2: // POS
			{
				if (! token.convert(info.pos))
				{
					comment = "Unable to determine position";
					return false;
				}
				break;
			}
			case 3: // ID
			{
				info.key = token;
				break;
			}
			case 4: // REF
			{
				std::string allele(token);
				unique.insert(allele);
				info.allele.append(std::move(allele));
				break;
			}
			case 5: // ALT
			{
				if (token.size() == 1)
				{
					const std::string allele(token);
					if (unique.count(allele) != 0)
					{
						comment = "Duplicate allele detected: '" + allele + "'";
						return false;
					}
					unique.insert(allele);
					info.allele.append(std::move(allele));
				}
				else
				{
					StreamSplit sub(token, ",");
					
					while (sub.next()) // parse multiple alleles
					{
						const std::string allele(sub);
						if (unique.count(allele) != 0)
						{
							comment = "Duplicate allele detected: '" + allele + "'";
							return false;
						}
						unique.insert(allele);
						info.allele.append(std::move(allele));
					}
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
				if (token.str() != "PASS") // check filtering passed
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
				if (token[0] != 'G' || token[1] != 'T') // check genotype data
				{
					comment = "Format does not define genotype";
					return false;
				}
				break;
			}
			default:
			{
				Genotype g;
				flag = true;
				
				// continue with data
				do
				{
					if (token.size() < 3)
					{
						comment = "Invalid genotype in column " + std::to_string(token.count());
						return false;
					}
					
					// single-char haplotypes
					if ((token[1] == '|' || token[1] == '/') &&
						(token[3] == ':' || token[3] == '\0'))
					{
						g.h0 = static_cast<int>(token[0] - '0');
						g.h1 = static_cast<int>(token[2] - '0');
					}
					// sub-parse haplotypes
					else
					{
						StreamSplit sub(token, "|/:");
						
						if (! sub.next() || ! sub.convert(conv))
						{
							comment = "Invalid genotype in column " + std::to_string(token.count());
							return false;
						}
						g.h0 = conv;
						
						if (! sub.next() || ! sub.convert(conv))
						{
							comment = "Invalid genotype in column " + std::to_string(token.count());
							return false;
						}
						g.h1 = conv;
					}
					
					if (! data.append(g))
					{
						comment = "More genotypes than expected: exceeds " + std::to_string(data.size());
						return false;
					}
				}
				while (token.next());
				
				if (! data.is_complete())
				{
					comment = "Less genotypes than expected: " + std::to_string(data.count()) + " found, " + std::to_string(data.size()) + " expected";
					return false;
				}
				
				break;
			}
		}
	}
	
	if (token.count() == 0)
	{
		comment = "Line is blank";
		return false;
	}
	
	if (! flag)
	{
		comment = "Line does not contain sample data";
		return false;
	}
	
	return true;
}



#endif
