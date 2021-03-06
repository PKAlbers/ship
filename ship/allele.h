//
//  allele.h
//  ship
//
//  Created by Patrick Albers on 04.12.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#ifndef __ship__allele__
#define __ship__allele__

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>


//******************************************************************************
// Allele container
//******************************************************************************

//
// Allele type
//
enum class AlleleType
{
	SNP,
	INDEL,
	OTHER
};


//
// Allele element
//
class Allele
{
private:
	
	std::string base_; // base sequence
	AlleleType  type_; // type of allele
	
	// determine type
	static AlleleType determine(const std::string &);
	
public:
	
	// return base sequence
	const std::string & base() const;
	
	// return type of allele
	AlleleType type() const;
	bool type(const AlleleType) const;
	
	// compare
	bool operator == (const Allele &) const;
	bool operator != (const Allele &) const;
	
	// assign
	Allele & operator = (const Allele &);
	Allele & operator = (Allele &&);
	
	// construct
	Allele(const std::string &); // default, copy
	Allele(std::string &&); // default, move
	Allele(const Allele &); // copy
	Allele(Allele &&); // move
	
	// destruct
	~Allele();
};


//
// Allele list
//
class AlleleList
{
private:
	
	std::vector<Allele> list_; // allele list
	int                 size_; // number of alleles
	bool contains_snp_;   // marker contains SNP alleles
	bool contains_indel_; // marker contains insertion/deletion alleles
	bool contains_other_; // marker contains unknown alleles
	
public:
	
	// return number of alleles
	int size() const;
	
	// return types contained
	bool contains_snp() const;
	bool contains_indel() const;
	bool contains_other() const;
	
	// append allele
	void append(const Allele &);
	void append(Allele &&);
	
	// return allele
	const Allele & operator [] (const int) const;
	
	// check if corresponding allele exists
	bool exists(const int) const;
	
	// print to stream
	void print(std::ostream &, const char = '\0') const;
	void print(FILE *, const char = '\0') const;
	
	// convert to string
	std::string str() const;
	
	// assign
	AlleleList & operator = (const AlleleList &);
	AlleleList & operator = (AlleleList &&);
	
	// construct
	AlleleList();
	AlleleList(const AlleleList &); // copy
	AlleleList(AlleleList &&); // move
	
	// destruct
	~AlleleList();
};



#endif /* defined(__ship__allele__) */
