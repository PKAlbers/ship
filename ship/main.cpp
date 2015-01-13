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
#include <vector>
#include <unordered_map>
#include <exception>
#include <stdexcept>

#include "command.h"
#include "timer.h"
#include "input.h"
#include "census.h"
#include "source.h"
#include "shared.h"

#include "stream.h"

//
// SHIP
//
const std::string signature = "[S]hared [H]aplotype [I]nference using [P]opulation data";
const std::string copyright = "(C) Patrick K. Albers - University of Oxford";


//
// Exception error message
//
int error(const std::string & msg, const std::exception & except)
{
	std::cout << std::endl << msg << std::endl << except.what() << std::endl << "Execution halted" << std::endl;
	std::cerr << std::endl << msg << std::endl << except.what() << std::endl << "Execution halted" << std::endl;
	return EXIT_FAILURE;
}


//
// Main
//
int main(int argc, const char * argv[])
{
	std::cout << std::endl;
	std::cout << signature << std::endl << copyright << std::endl << std::endl;
	
	Runtime runtime;
	
	
	//
	// parse command line arguments
	//
	CommandLine cmd(argc, argv);
	
	cmd.register_par(0);
	cmd.register_arg("i", 1, true); // input file
	cmd.register_arg("t", 1, true); // rare variant threshold
	cmd.register_arg("o", 1, false); // output file prefix
	cmd.register_arg("s", 1, false); // sample file
	cmd.register_arg("m", 1, false); // genetic map
	cmd.register_opt("threads", 1, false); // threads
	
	if(! cmd.parse())
	{
		return EXIT_FAILURE;
	}
	
	//
	// determine number of threads
	//
	int threads = (cmd.is_opt("threads")) ? std::stoi(cmd.opt("threads")): 1;
	
	if (threads < 1)
	{
		std::cout << "Cannot execute with " << threads << " threads" << std::endl;
		return EXIT_FAILURE;
	}
	
	//
	// create output files
	//
	std::string prefix = (cmd.is_arg("o")) ? cmd.arg("o"): cmd.prog() + "." + timestamp(true);
	
	StreamLogErr __logerr__(prefix); // redirect log & err streams
	
	StreamOut marker_file; // output markers
	StreamOut sample_file; // output samples
	
	try
	{
		marker_file.open(prefix + ".marker");
		sample_file.open(prefix + ".sample");
	}
	catch (const std::exception & x)
	{
		return error("Error while creating output files", x);
	}
	
	std::clog << signature << std::endl << copyright << std::endl << std::endl;
	
	
	//
	// Determine rare variant threshold
	//
	Cutoff cutoff;
	try
	{
		cutoff.parse(cmd.arg("t"));
	}
	catch (const std::exception & x)
	{
		return error("Error while interpreting command line", x);
	}
	
	
	//
	// print command line arguments
	//
	std::cout << std::setw(25) << std::left << "Input file: "  << cmd.arg("i") << std::endl;
	
	if (cmd.is_arg("s"))
		std::cout << std::setw(25) << std::left << "Sample file: "  << (std::string)cmd.arg("s") << std::endl;
	
	if (cmd.is_arg("g"))
		std::cout << std::setw(25) << std::left << "Genetic map: "  << (std::string)cmd.arg("g") << std::endl;
	
	if (cmd.is_opt("threads"))
		std::cout << std::setw(25) << std::left << "# threads:" << threads << std::endl;
	
	std::cout << std::setw(25) << std::left << "Rare variant threshold: "  << (std::string)cmd.arg("t") << std::endl;
	
	std::cout << std::setw(25) << std::left << "Output files:" << std::endl;
	std::cout << std::setw(5) << std::left << " " << sample_file.name << std::endl;
	std::cout << std::setw(5) << std::left << " " << marker_file.name << std::endl;
	
	std::cout << std::endl;
	
	
	//
	// Load source data
	//
	Source source;
	
	try
	{
		Input_VCF input(cmd.arg("i"));
		
		input.filter.markerinfo.remove_if_contains_other();
		input.filter.markerdata.remove_if_contains_unknown();
		
		if (cmd.is_arg("s")) input.sample(cmd.arg("s"));
		if (cmd.is_arg("g")) input.genmap(cmd.arg("g"));
		
		input.run(source, threads);
		
		source.finish(threads);
	}
	catch (std::exception & x)
	{
		return error("Error while reading input files", x);
	}
	std::cout << std::endl;
	
	// scale cutoff with sample size
	try
	{
		cutoff.scale(source.sample_size());
	}
	catch (const std::exception & x)
	{
		return error("Error while applying threshold", x);
	}
	
	// print source dimensions
	std::cout << "Analysed samples: " << source.sample_size() << std::endl;
	std::cout << "Analysed markers: " << source.marker_size() << std::endl;
	std::cout << std::endl;
	
	
	//
	// Write marker & sample information
	//
	
	// sample
	std::cout << "Writing sample information: " << std::flush;
	
	sample_file.line(SampleInfo::header);
	
	for (size_t i = 0; i < source.sample_size(); ++i)
	{
		source.sample(i).info.print(sample_file, '\n');
	}
	sample_file.close();
	std::cout << "OK" << std::endl;
	
	// marker
	std::cout << "Writing marker information: " << std::flush;
	
	marker_file.line(MarkerInfo::header, ' ');
	marker_file.line(MarkerStat::header, ' ');
	marker_file.line(MarkerGmap::header, '\n');
	
	for (size_t i = 0; i < source.marker_size(); ++i)
	{
		source.marker(i).info.print(marker_file, ' ');
		source.marker(i).stat.print(marker_file, ' ');
		source.marker(i).gmap.print(marker_file, '\n');
	}
	marker_file.close();
	std::cout << "OK" << std::endl;
	
	std::cout << std::endl;
	
//	
//	StreamOut xfile;
//	xfile.open(prefix + ".txt");
//	
//
//	{
//		for (size_t j = 0; j < source.marker_size(); ++j)
//		{
//			source.marker(j).info.print(xfile);
//			
//			for (size_t i = 0; i < source.sample_size(); ++i)
//			{
//				Genotype g = source.sample(i).data[j];
//			
//				fprintf(xfile, " %d|%d", (int)g.h0, (int)g.h1);
//			}
//			
//			xfile.endl();
//		}
//	}
//	
	
	
	//
	// Identify rare variants
	//
	std::cout << "Identifying rare alleles" << std::endl;
	RareHaplotype rare(source, cutoff);
	std::cout << "Identified rare alleles: " << rare.n_allele << " (in " << rare.n_marker << " markers)" << std::endl;
	
	std::cout << std::endl << "Done!" << std::endl << runtime.str() << std::endl;
	
	return EXIT_SUCCESS;
}


