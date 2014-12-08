//
//  main.cpp
//  ship
//
//  Created by Patrick Albers on 07.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <stdexcept>

#include "types.h"
#include "timer.h"
#include "input.h"
#include "sample.h"
#include "marker.h"
#include "genmap.h"
#include "source.h"
//#include "shared.h"


//
// SHIP
//
const std::string signature = "[S]hared [H]aplotype [I]nference using [P]opulation data";
const std::string copyright = "(C) Patrick K. Albers - University of Oxford";


//
// Usage, explaining command line arguments
//
void usage()
{
	std::cout
	<< std::endl <<
	
	"Usage:\n\n"
	
	<< std::endl;
}


//
// Exception error message
//
void error(const std::exception & x, const std::string msg = "")
{
	std::cerr << std::endl;
	
	if (msg.size() != 0)
	{
		std::cerr << msg << std::endl;
	}
	
	try
	{
		throw;
	}
	catch (const std::logic_error & x)		{ std::cerr << "# Logic error" << std::endl; }
	catch (const std::domain_error & x)		{ std::cerr << "# Domain error" << std::endl; }
	catch (const std::invalid_argument & x)	{ std::cerr << "# Invalid argument" << std::endl; }
	catch (const std::length_error & x)		{ std::cerr << "# Length error" << std::endl; }
	catch (const std::out_of_range & x)		{ std::cerr << "# Out of range error" << std::endl; }
	catch (const std::runtime_error & x)	{ std::cerr << "# Runtime error" << std::endl; }
	catch (const std::range_error & x)		{ std::cerr << "# Range error" << std::endl; }
	catch (const std::overflow_error & x)	{ std::cerr << "# overflow error" << std::endl; }
	catch (const std::underflow_error & x)	{ std::cerr << "# Underflow error" << std::endl; }
	catch (const std::bad_alloc & x)		{ std::cerr << "# Memory error" << std::endl; }
	catch (const std::exception & x)		{ std::cerr << "# Unexpected error" << std::endl; }
	
	std::cerr << x.what() << std::endl;
	std::cerr << "# Execution terminated" << std::endl;
}

struct Redirect
{
	std::streambuf * clog;
	std::streambuf * cerr;
	
	std::ofstream flog;
	std::ofstream ferr;
	
	Redirect(const std::string & prefix)
	: flog(prefix + ".log.txt")
	, ferr(prefix + ".err.txt")
	{
		this->clog = std::clog.rdbuf();
		this->cerr = std::cerr.rdbuf();
		
		std::clog.rdbuf(this->flog.rdbuf());
		std::cerr.rdbuf(this->ferr.rdbuf());
	}
	
	~Redirect()
	{
		std::clog.rdbuf(this->clog);
		std::cerr.rdbuf(this->cerr);
		
		this->flog.close();
		this->ferr.close();
	}
};


//
// Main
//
int main(int argc, const char * argv[])
{
	std::cout << signature << std::endl << copyright << std::endl << std::endl;
	
	Runtime runtime;
	
	//
	// parse command line arguments
	//
	std::string arg_input;
	std::string arg_output;
	std::string arg_cutoff;
	std::string arg_legend;
	std::string arg_sample;
	std::string arg_genmap;
	std::string arg_format;
	std::string arg_chromo;
	bool arg_input_  = false;
	bool arg_output_ = false;
	bool arg_cutoff_ = false;
	bool arg_legend_ = false;
	bool arg_sample_ = false;
	bool arg_genmap_ = false;
	bool arg_format_ = false;
	bool arg_chromo_ = false;
	
	while (argc > 1 && argv[1][0] == '-')
	{
		switch (argv[1][1])
		{
			case 'i': // input file
				arg_input  = argv[2];
				arg_input_ = true;
				++argv; --argc; break;
			case 'o': // output file prefix
				arg_output  = argv[2];
				arg_output_ = true;
				++argv; --argc; break;
			case 't': // rare variant threshold
				arg_cutoff  = argv[2];
				arg_cutoff_ = true;
				++argv; --argc; break;
			case 'l': // legend file
				arg_legend  = argv[2];
				arg_legend_ = true;
				++argv; --argc; break;
			case 's': // sample file
				arg_sample  = argv[2];
				arg_sample_ = true;
				++argv; --argc; break;
			case 'm': // genetic map
				arg_genmap  = argv[2];
				arg_genmap_ = true;
				++argv; --argc; break;
				
				// other options
			case '-':
				if (std::string(argv[1] + 2) == "enforce-format") // format
				{
					arg_format  = argv[2];
					arg_format_ = true;
					++argv; --argc; break;
				}
				if (std::string(argv[1] + 2) == "chromosome") // chromosome
				{
					arg_chromo  = argv[2];
					arg_chromo_ = true;
					++argv; --argc; break;
				}
				else
				{
					std::cerr << "Unknown command line option: " << argv[1] << std::endl;
					usage();
					return 1;
				}
				break;
			default:
				std::cerr << "Unknown command line argument: " << argv[1] << std::endl;
				usage();
				break;
				return 1;
		}
		
		++argv; --argc;
	}
	
	
	//
	// check required arguments
	//
	if (!arg_input_)
	{
		std::cerr << "No input file specified" << std::endl;
		usage();
		return 1;
	}
	if (!arg_output_)
	{
		std::cerr << "No output file prefix specified" << std::endl;
		usage();
		return 1;
	}
	if (!arg_cutoff_)
	{
		std::cerr << "No rare variant threshold specified" << std::endl;
		usage();
		return 1;
	}

	
	//
	// redirect log & err output to files
	//
	Redirect redirect(arg_output);
	
	//
	// Determine threshold
	//
	Cutoff cutoff;
	try
	{
		cutoff.parse(arg_cutoff);
	}
	catch (const std::exception & x)
	{
		error(x, "Error while reading command line");
		return 1;
	}
	
	
	//
	// Assume chromosome
	//
	if (arg_chromo_)
	{
		Chromosome(std::stoi(arg_chromo));
	}
	
	//
	// Define filters
	//
	FilterInput filter;
	// ...
	
	
	//
	// print command line arguments
	//
	std::cout << "Input file:             "  << arg_input  << std::endl;
	std::cout << "Output file prefix:     "  << arg_output << std::endl;
	std::cout << "Rare variant threshold: "  << arg_cutoff << std::endl;
	
	if (arg_sample_) std::cout << "Sample file:            "  << arg_sample << std::endl;
	if (arg_legend_) std::cout << "Legend file:            "  << arg_legend << std::endl;
	if (arg_genmap_) std::cout << "Genetic map file:       "  << arg_genmap << std::endl;
	if (arg_format_) std::cout << "Format of input file:   "  << arg_format << std::endl;
	if (arg_chromo_) std::cout << "Assumed chromosome:     "  << arg_chromo << std::endl;
	
	std::cout << std::endl;
	
	
	//
	// Load source data
	//
	Source source;
	try
	{
		InputFormat format = InputFormat::UNKNOWN;
		
		// enforce input file format
		if (arg_format_)
		{
			std::cout << "Enforced input file format: " << std::flush;
			
			switch (arg_format[0])
			{
				case 'v': format = InputFormat::VCF; std::cout << "VCF" << std::endl; break;
				case 'h': format = InputFormat::HAP; std::cout << "Haplotype format" << std::endl; break;
				case 'g': format = InputFormat::GEN; std::cout << "Genotype format" << std::endl; break;
				default:
					throw std::invalid_argument("Unknown input file format: " + arg_format);
					break;
			}
		}
		
		// scan input file format
		ScanInput scan(arg_input, format);

		if (! arg_format_)
		{
			std::cout << "Detected input file format: " << std::flush;
			
			switch (scan.format)
			{
				case InputFormat::VCF: std::cout << "VCF" << std::endl;
				case InputFormat::HAP: std::cout << "Haplotype format" << std::endl;
				case InputFormat::GEN: std::cout << "Genotype format" << std::endl;
				case InputFormat::UNKNOWN:
				default:
					std::cout << "unknown" << std::endl;
			}
		}
		
		// load input file into source
		LoadInput input(scan, filter);
		
		if (arg_sample_) input.sample(arg_sample); // provide sample file
		if (arg_legend_) input.legend(arg_legend); // provide legend file
		if (arg_genmap_) input.genmap(arg_genmap); // provide genmap file
		
		input.load(source);
	}
	catch (std::exception & x)
	{
		error(x, "Error while loading input files");
		return 1;
	}
	
	// scale cutoff with sample size
	try
	{
		cutoff.scale(source.sample_size());
	}
	catch (const std::exception & x)
	{
		error(x, "Error while applying threshold");
		return 1;
	}
	
	// print source dimensions
	std::cout << "# samples: " << source.sample_size() << std::endl;
	std::cout << "# markers: " << source.marker_size() << std::endl;
	std::cout << std::endl;
	
//	std::cout << "checkpoint: ";
//	std::string x;
//	std::cin >> x;
	
	std::cout << std::endl << "Done!" << std::endl << runtime.str() << std::endl;
	
	return 0;
}


