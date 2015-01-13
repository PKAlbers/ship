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
	bool flag = false; // flag that genotypes were read
	
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
				info.pos = (size_t)parse;
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
					
					if (! data.append(g))
					{
						comment = "More genotypes than expected (exceeds " + std::to_string(data.size()) + ")";
						return false;
					}
				}
				
				if (! data.is_complete())
				{
					comment = "Less genotypes than expected (" + std::to_string(data.count()) + " found, " + std::to_string(data.size()) + " expected)";
					return false;
				}
				
				flag = true;
				
				break;
			}
		}
	}
	
	if (! flag)
	{
		comment = "VCF file does not contain sample data";
		return false;
	}
	
	return true;
}








#endif
