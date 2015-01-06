//
//  command.h
//  ship
//
//  Created by Patrick Albers on 14.12.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship__command__
#define __ship__command__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>


//******************************************************************************
// Handle command line input
//******************************************************************************

//
// Expected command parameters
//
class CommandPars
{
private:
	
	// construct
	CommandPars();
	
	int quantity; // 0 = none expected, >0 = n expected, -1 = ambiguous
	
public:
	
	// specify quantity
	void operator () (const int);
	
	friend class CommandLine;
};


//
// Expected command arguments/options
//
class CommandBase
{
protected:
	
	struct Expect
	{
		int  quantity; // 0 = none expected, >0 = n expected, -1 = ambiguous
		bool required;
	};
	
	std::unordered_map<std::string, Expect> expect;
	
	// construct
	CommandBase();
	
public:
	
	// add argument/option
	void operator () (const std::string &, const int, const bool);
	
	friend class CommandLine;
};

class CommandArgs : public CommandBase
{
private:
	CommandArgs();
public:
	friend class CommandLine;
};

class CommandOpts : public CommandBase
{
private:
	CommandOpts();
public:
	friend class CommandLine;
};


//
// Command unit
//
class CommandUnit
{
private:
	
	// construct
	CommandUnit();
	
public:
	
	std::vector<std::string> value;
	int count;
	
	// cast first value
	operator std::string () const;
	
	// return specific value
	std::string operator [] (const int) const;
	
	// stream
	friend std::ostream & operator << (std::ostream & stream, const CommandUnit & fetch);
	
	friend class CommandLine;
};


//
// Parse command line
//
class CommandLine
{
private:
	
	const std::string _path; // execution path
	const std::vector<std::string> _line; // parsed command line
	bool parsed; // flag that command line was parsed
	
	CommandUnit _par; // input parameters without specifiers
	std::unordered_map<std::string, CommandUnit> _arg; // arguments: -x
	std::unordered_map<std::string, CommandUnit> _opt; // options:  --x
	
	// check command line values
	bool check_par() const;
	bool check_arg() const;
	bool check_opt() const;
	
	static const std::vector<std::string> help_keywords;
	
public:
	
	CommandPars register_par; // register parameters
	CommandArgs register_arg; // register arguments
	CommandOpts register_opt; // register options
	
	// parse commands
	bool parse();
	
	// check commands
	size_t n_par() const;
	bool is_arg(const std::string &) const;
	bool is_opt(const std::string &) const;
	
	// return commands
	std::string path() const;
	std::string prog() const;
	std::string par(const int) const;
	CommandUnit arg(const std::string &) const;
	CommandUnit opt(const std::string &) const;
	
	// print help
	void help();
	
	// construct/destruct
	CommandLine(int argc, const char * argv[]);
};



#endif /* defined(__ship__command__) */
