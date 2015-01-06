//
//  command.cpp
//  ship
//
//  Created by Patrick Albers on 14.12.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "command.h"


#define DEBUG_COMMAND


//******************************************************************************
// Handle command line input
//******************************************************************************

//
// Expected command parameters
//

CommandPars::CommandPars()
: quantity(0)
{}

void CommandPars::operator () (const int size)
{
	this->quantity = size;
}


//
// Expected command arguments/options
//

CommandBase::CommandBase()
{}

CommandArgs::CommandArgs()
: CommandBase()
{}

CommandOpts::CommandOpts()
: CommandBase()
{}

void CommandBase::operator () (const std::string & key, const int quantity, const bool required)
{
	CommandBase::Expect _expect = { quantity, required };
	this->expect.insert(std::make_pair(key, std::move(_expect)));
}


//
// Command unit
//

CommandUnit::CommandUnit()
: count(0)
{}

CommandUnit::operator std::string () const
{
#ifdef DEBUG_COMMAND
	if (this->count != 1)
	{
		throw std::length_error("Command line value not singular");
	}
#endif
	
	return this->value[0];
}

std::string CommandUnit::operator [] (const int i) const
{
#ifdef DEBUG_COMMAND
	if (i >= this->count)
	{
		throw std::out_of_range("Command line value out of range");
	}
#endif
	
	return this->value[i];
}

std::ostream & operator << (std::ostream & stream, const CommandUnit & fetch)
{
	stream << (std::string)fetch;
	return stream;
}


//
// Parse command line
//

CommandLine::CommandLine(int argc, const char * argv[])
: _path(argv[0])
, _line(argv + 1, argv + argc)
, parsed(false)
{}

bool CommandLine::check_par() const
{
	// check quantity
	if (this->register_par.quantity == 0 &&
		this->_par.count != 0) // empty
	{
		std::cerr << "Unknown commmand line parameters:";
		for (int i = 0; i < this->_par.count; ++i)
		{
			std::cerr << " " << this->_par.value[i];
		}
		std::cerr << std::endl;
		return false;
	}
	if (this->register_par.quantity > 0 &&
		this->_par.count != this->register_par.quantity) // exact
	{
		std::cerr << "Unexpected number of command line parameters" << std::endl;
		std::cerr << "Expected # parameters: " << this->register_par.quantity << std::endl;
		std::cerr << "Detected # parameters: " << this->_par.count << std::endl;
		return false;
	}
	
	return true;
}

bool CommandLine::check_arg() const
{
	for (std::unordered_map<std::string, CommandUnit>::const_iterator it = this->_arg.cbegin(), end = this->_arg.cend(); it != end; ++it)
	{
		// check unexpected arguments
		if (this->register_arg.expect.count(it->first) == 0)
		{
			std::cerr << "Unexpected command line argument: -" << it->first << std::endl;
			return false;
		}
		
		int q = this->register_arg.expect.at(it->first).quantity;
		
		// check quantity
		if (q == 0 && it->second.count != 0) // empty
		{
			std::cerr << "Unknown commmand line arguments:";
			for (int i = 0; i < it->second.count; ++i)
			{
				std::cerr << " " << it->second.value[i];
			}
			std::cerr << std::endl;
			return false;
		}
		if (q > 0 && it->second.count != q) // exact
		{
			std::cerr << "Unexpected number of command line values for argument -" << it->first << std::endl;
			std::cerr << "Exactly " << q << " argument values must be provided" << std::endl;
			return false;
		}
	}
	
	// check required arguments
	for (std::unordered_map<std::string, CommandArgs::Expect>::const_iterator it = this->register_arg.expect.cbegin(), end = this->register_arg.expect.cend(); it != end; ++it)
	{
		if (it->second.required && this->_arg.count(it->first) == 0)
		{
			std::cerr << "Command line argument required: -" << it->first << std::endl;
			return false;
		}
	}
	
	return true;
}

bool CommandLine::check_opt() const
{
	for (std::unordered_map<std::string, CommandUnit>::const_iterator it = this->_opt.cbegin(), end = this->_opt.cend(); it != end; ++it)
	{
		// check unexpected arguments
		if (this->register_opt.expect.count(it->first) == 0)
		{
			std::cerr << "Unexpected command line option: --" << it->first << std::endl;
			return false;
		}
		
		int q = this->register_opt.expect.at(it->first).quantity;
		
		// check quantity
		if (q == 0 && it->second.count != 0) // empty
		{
			std::cerr << "Unknown commmand line options:";
			for (int i = 0; i < it->second.count; ++i)
			{
				std::cerr << " " << it->second.value[i];
			}
			std::cerr << std::endl;
			return false;
		}
		if (q > 0 && it->second.count != q) // exact
		{
			std::cerr << "Unexpected number of command line values for option --" << it->first << std::endl;
			std::cerr << "Exactly " << q << " option values must be provided" << std::endl;
			return false;
		}
	}
	
	// check required arguments
	for (std::unordered_map<std::string, CommandArgs::Expect>::const_iterator it = this->register_opt.expect.cbegin(), end = this->register_opt.expect.cend(); it != end; ++it)
	{
		if (it->second.required && this->_opt.count(it->first) == 0)
		{
			std::cerr << "Command line option required: --" << it->first << std::endl;
			return false;
		}
	}
	
	return true;
}

bool CommandLine::parse()
{
	if (this->_line.size() == 0)
	{
		std::cerr << "No command line parameters specified" << std::endl;
		return false;
	}
	
	std::vector<std::string>::const_reverse_iterator it, end;
	std::vector<std::string> collect;
	
	// reverse walkabout
	for (it = this->_line.crbegin(), end = this->_line.crend(); it != end; ++it)
	{
		// check help keywords
		for (const std::string & key : CommandLine::help_keywords)
		{
			if (std::string("-") + key == std::string(*it))
			{
				this->help();
				return false;
			}
		}
		
		if ((*it)[0] == '-')
		{
			const size_t n = it->size();
			
			// check if number
			if (n > 1 && (*it)[1] >= '0' && (*it)[1] <= '9')
			{
				collect.push_back(*it);
				continue;
			}
			
			if (n < 2)
			{
				std::cerr << "No argument specified: -[?]" << std::endl;
				return false;
			}
			
			if (collect.size() > 0)
			{
				std::reverse(collect.begin(), collect.end());
			}
			
			CommandUnit unit;
			unit.value.swap(collect);
			unit.count = static_cast<int>(unit.value.size());
			
			if ((*it)[1] == '-')
			{
				if (n < 3)
				{
					std::cerr << "No option specified: --[?]" << std::endl;
					return false;
				}
				
				// options
				const std::string key = it->substr(2);
				this->_opt.insert(std::make_pair(key, unit));
				continue;
			}
			
			// arguments
			const std::string key = it->substr(1);
			this->_arg.insert(std::make_pair(key, unit));
			continue;
		}
		
		collect.push_back(*it);
	}
	
	// parameters
	if (collect.size() > 0)
	{
		std::reverse(collect.begin(), collect.end());
		this->_par.value.swap(collect);
		this->_par.count = static_cast<int>(this->_par.value.size());
	}
	
	// check parameters
	if (! this->check_par())
		return false;
	
	// check arguments
	if (! this->check_arg())
		return false;
	
	// check options
	if (! this->check_opt())
		return false;
	
	this->parsed = true;
	return true;
}

size_t CommandLine::n_par() const
{
#ifdef DEBUG_COMMAND
	if (! this->parsed)
	{
		throw std::runtime_error("Command line not parsed");
	}
#endif
	
	return this->_par.count;
}

bool CommandLine::is_arg(const std::string & str) const
{
#ifdef DEBUG_COMMAND
	if (! this->parsed)
	{
		throw std::runtime_error("Command line not parsed");
	}
#endif
	
	return (this->_arg.count(str) != 0);
}

bool CommandLine::is_opt(const std::string & str) const
{
#ifdef DEBUG_COMMAND
	if (! this->parsed)
	{
		throw std::runtime_error("Command line not parsed");
	}
#endif
	
	return (this->_opt.count(str) != 0);
}

std::string CommandLine::path() const
{
	return this->_path;
}

std::string CommandLine::prog() const
{
	size_t i = this->_path.find_last_of("/\\");
	
	if (i != std::string::npos)
	{
		return this->_path.substr(i + 1);
	}
	
	return this->_path;
}

std::string CommandLine::par(const int i) const
{
#ifdef DEBUG_COMMAND
	if (! this->parsed)
	{
		throw std::runtime_error("Command line not parsed");
	}
#endif
	
	return this->_par[i];
}

CommandUnit CommandLine::arg(const std::string & str) const
{
#ifdef DEBUG_COMMAND
	if (! this->parsed)
	{
		throw std::runtime_error("Command line not parsed");
	}
	if (this->_arg.count(str) == 0)
	{
		throw std::invalid_argument("Command line argument not defined: -" + str);
	}
#endif
	
	return this->_arg.at(str);
}

CommandUnit CommandLine::opt(const std::string & str) const
{
#ifdef DEBUG_COMMAND
	if (! this->parsed)
	{
		throw std::runtime_error("Command line not parsed");
	}
	if (this->_opt.count(str) == 0)
	{
		throw std::invalid_argument("Command line option not defined: --" + str);
	}
#endif
	
	return this->_opt.at(str);
}

void CommandLine::help()
{
	std::cout << "Usage:   " << this->prog();
	
	std::cout << " <ARGUMENT> ";
	
	std::cout << " -[OPTION ... ] ";
	
	std::cout << " --[OTHER ... ] ";
	
	std::cout << std::endl << std::endl;
	
	// parameters
	std::cout << std::string(100, '-') << std::endl << "Arguments: " << std::endl << std::endl;
	
	// arguments
	std::cout << std::string(100, '-') << std::endl << "Options: " << std::endl << std::endl;
	
	// options
	std::cout << std::string(100, '-') << std::endl << "Other options: " << std::endl << std::endl;
	
	std::cout << std::endl;
}

const std::vector<std::string> CommandLine::help_keywords = { "help", "usage" };


