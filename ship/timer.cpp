//
//  timer.cpp
//  ship
//
//  Created by Patrick Albers on 17.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "timer.h"


//******************************************************************************
// Current time
//******************************************************************************
std::string timestamp(const bool no_spec_char)
{
	time_t now = time(NULL);
	tm *   ltm = localtime(&now);
	char c[256];
	
	if (!no_spec_char)
		sprintf(c, "%d-%02d-%02d_%02d:%02d:%02d",  ltm->tm_year + 1900, ltm->tm_mon, ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
	else
		sprintf(c, "%d-%02d-%02d_%02dh%02dm%02ds", ltm->tm_year + 1900, ltm->tm_mon, ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
	
	return std::string(c);
}


//******************************************************************************
// Elapsed time clocks
//******************************************************************************
Runtime::Runtime()
: start_comp(clock())
, start_real(time(NULL))
{}

unsigned long long Runtime::raw() const
{
	return static_cast<unsigned long long>(clock() - this->start_comp);
}

size_t Runtime::comp() const
{
	return static_cast<size_t>(floor((clock() - this->start_comp) / CLOCKS_PER_SEC));
}

size_t Runtime::real() const
{
	return static_cast<size_t>(floor(difftime(time(NULL), this->start_real)));
}

std::string Runtime::str_comp() const
{
	size_t _comp = this->comp();
	unsigned int h  = static_cast<unsigned int>((_comp / 60 / 60));
	unsigned int m  = static_cast<unsigned int>((_comp / 60 ) % 60);
	unsigned int s  = static_cast<unsigned int>((_comp % 60));
	unsigned int ms = static_cast<unsigned int>((_comp % 60) * 1000);
	
	std::ostringstream oss;
	if (h > 0)
		oss << h << "h ";
	if (h > 0 || m > 0)
		oss << m << "m ";
	if (s != 0)
		oss << s << "s";
	else
		oss << ms << "ms";
	return oss.str();
}

std::string Runtime::str_real() const
{
	size_t _real = this->real();
	unsigned int h  = static_cast<unsigned int>((_real / 60 / 60));
	unsigned int m  = static_cast<unsigned int>((_real / 60 ) % 60);
	unsigned int s  = static_cast<unsigned int>((_real % 60));
	unsigned int ms = static_cast<unsigned int>((_real % 60) * 1000);
	
	std::ostringstream oss;
	if (h > 0)
		oss << h << "h ";
	if (h > 0 || m > 0)
		oss << m << "m ";
	if (s != 0)
		oss << s << "s";
	else
		oss << ms << "ms";
	return oss.str();
}

std::string Runtime::str() const
{
	return "Elapsed time: " + this->str_real() + " (processing time: " + this->str_comp() + ")";
}



//******************************************************************************
// Progress status
//******************************************************************************

//
// Progress bar - required to know target count
//
ProgressBar::ProgressBar(const size_t n, const bool _log)
: log(_log)
, target(n)
, interval_dynamic(static_cast<double>(n) / static_cast<double>(ProgressBar::rate))
, interval_static(static_cast<double>(n) / static_cast<double>(ProgressBar::size))
, i(0)
, t_dynamic(0)
, t_static(0)
{
	std::cout << " ... \r" << std::flush;
	
	if (this->log)
	{
		// print static bar
		std::clog
		<< " 0% "  << std::string(floor(static_cast<double>(ProgressBar::size) / 2) - 5, '-')
		<< " 50% " << std::string( ceil(static_cast<double>(ProgressBar::size) / 2) - 7, '-')
		<< " 100%" << std::endl << ' ' << std::flush;
	}
}

void ProgressBar::update(const size_t _i)
{
	this->i += (this->i == this->target) ? 0: _i; // do not count more than target
	
	while (this->i + 0.5 >= this->t_dynamic)
	{
		this->t_dynamic += this->interval_dynamic; // jump to next interval
		
		double frq = static_cast<double>(this->i) / static_cast<double>(this->target); // calculate current frequency
		if (frq > 1) frq = 1;
		const unsigned int bar_fill = frq * ProgressBar::size;      // calculate current bar fill length
		const unsigned int bar_rest = ProgressBar::size - bar_fill; // calculate current bar rest length
		
		std::cout
		<< std::setw(5) << std::right << std::setprecision(1) << std::fixed << (frq * (double)100)
		<< "% ["
		<< std::string(bar_fill, ProgressBar::fill)
		<< std::string(bar_rest, ProgressBar::rest)
		<< "]\r"
		<< std::flush;
	}
	
	if (this->log)
	{
		while (this->i + 0.01 >= this->t_static)
		{
			this->t_static += this->interval_static; // jump to next interval
			
			std::clog << '.' << std::flush;
		}
	}
}

void ProgressBar::finish() const
{
	std::cout
	<< std::setw(5) << std::right << std::setprecision(1) << std::fixed << (double)100
	<< "% ["
	<< std::string(ProgressBar::size, ProgressBar::fill)
	<< "] OK"
	<< std::endl;
	
	if (this->log)
	{
		std::clog << std::endl;
	}
}

const unsigned int ProgressBar::size = 80;
const unsigned int ProgressBar::rate = 1000;
const char ProgressBar::fill = '=';
const char ProgressBar::rest = ' ';


//
// Progress message - target count is unknown
//
ProgressMsg::ProgressMsg(const std::string _unit, const size_t _rate, std::ostream * osp)
: time(clock())
, unit(_unit)
, rate(_rate)
, line(new char[ProgressMsg::size])
, i(0)
, l(0)
, t(0)
, stream_(osp)
{
	*(this->stream_) << " ... \r" << std::flush;
}

ProgressMsg::~ProgressMsg()
{
	delete [] this->line;
}

void ProgressMsg::update(const size_t _i)
{
	this->i += _i;
	
	if (this->i % this->rate != 0)
		return;
	
	const double elapsed = static_cast<double>(clock() - this->time) / static_cast<double>(CLOCKS_PER_SEC);
	
	// output
	if ((elapsed - this->t) > ProgressMsg::interval)
	{
		this->t = elapsed; // update time checkpoint
		
		const double frq = static_cast<double>(this->i) / elapsed;
		
		int len = snprintf(this->line, ProgressMsg::size, " %lu %s (%.1f / sec)", this->i, this->unit.c_str(), frq);
		
		if (len > ProgressMsg::size)
		{
			if (this->l < 5)
				this->l = 5;
			
			*(this->stream_) << " ... " << std::string(this->l - 5, ' ') << '\r' << std::flush;
		}
		else
		{
			if (this->l < len) // remember max line length
				this->l = len;
			
			*(this->stream_) << this->line << std::string(this->l - len, ' ') << '\r' << std::flush;
		}
	}
}

void ProgressMsg::finish(const size_t _i) const
{
	const size_t final = (_i == 0) ? this->i: _i;
	
	const double total = static_cast<double>(clock() - this->time) / static_cast<double>(CLOCKS_PER_SEC);
	int len;
	
	if (total < 1.0)
		len = snprintf(this->line, ProgressMsg::size, " %lu %s in %.3f sec", final, this->unit.c_str(), total);
	else
		len = snprintf(this->line, ProgressMsg::size, " %lu %s in %.1f sec", final, this->unit.c_str(), total);
	
	if (len > ProgressMsg::size)
	{
		*(this->stream_) << " ... " << std::string(this->l - 5, ' ') << std::endl;
	}
	else
	{
		*(this->stream_) << this->line << ((this->l < len) ? "": std::string(this->l - len, ' ')) << std::endl;
	}
}

const double ProgressMsg::interval = 0.5; // print interval in seconds
const int    ProgressMsg::size = 4096;    // max size, not exact size


