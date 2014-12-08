//
//  allele.cpp
//  ship
//
//  Created by Patrick Albers on 04.12.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "allele.h"



//******************************************************************************
// Allele container
//******************************************************************************


//
// Allele, base sequence
//

Allele::Allele(const std::string & _base)
: base_(_base)
, type_(Allele::determine(base_))
{}

Allele::Allele(std::string && _base)
: base_(std::move(_base))
, type_(Allele::determine(base_))
{}

Allele::Allele(const Allele & other)
: base_(other.base_)
, type_(other.type_)
{}

Allele::Allele(Allele && other)
: base_(std::move(other.base_))
, type_(other.type_)
{}

Allele::~Allele()
{
	this->base_.clear();
}

Allele & Allele::operator = (const Allele & other)
{
	if (this != &other)
	{
		this->base_ = other.base_;
		this->type_ = other.type_;
	}
	return *this;
}

Allele & Allele::operator = (Allele && other)
{
	if (this != &other)
	{
		this->base_ = std::move(other.base_);
		this->type_ = other.type_;
	}
	return *this;
}

bool Allele::operator == (const Allele & other) const
{
	return (this->base_ == other.base_);
}

bool Allele::operator != (const Allele & other) const
{
	return (this->base_ != other.base_);
}

const std::string & Allele::base() const
{
	return this->base_;
}

// return type of allele
AlleleType Allele::type() const
{
	return this->type_;
}

bool Allele::type(const AlleleType _type) const
{
	return (this->type_ == _type);
}

AlleleType Allele::determine(const std::string & base)
{
	static const char B[] = "ACGTN";
	
	const unsigned long size = base.size();
	
	// SNP
	if (size == 1)
	{
		if (base[0] == '*') // special case for deletion
		{
			return AlleleType::INDEL;
		}
		return (strchr(B, base[0]) != NULL) ? AlleleType::SNP: AlleleType::OTHER;
	}
	
	// INDEL
	bool flag = true;
	for (unsigned long i = 0; i < size; ++i)
	{
		if (strchr(B, base[i]) == NULL)
		{
			flag = false;
			break;
		}
	}
	if (flag)
	{
		return AlleleType::INDEL;
	}
	
	return AlleleType::OTHER;
}


//
// Allele list
//

AlleleList::AlleleList()
: size_(0)
, contains_snp_(false)
, contains_indel_(false)
, contains_other_(false)
{}

AlleleList::AlleleList(const AlleleList & other)
: list_(other.list_)
, size_(other.size_)
, contains_snp_(other.contains_snp_)
, contains_indel_(other.contains_indel_)
, contains_other_(other.contains_other_)
{}

AlleleList::AlleleList(AlleleList && other)
: list_(std::move(other.list_))
, size_(other.size_)
, contains_snp_(other.contains_snp_)
, contains_indel_(other.contains_indel_)
, contains_other_(other.contains_other_)
{}

AlleleList::~AlleleList()
{
	this->list_.clear();
}

AlleleList & AlleleList::operator = (const AlleleList & other)
{
	if (this != &other)
	{
		this->list_ = other.list_;
		this->size_ = other.size_;
		this->contains_snp_   = other.contains_snp_;
		this->contains_indel_ = other.contains_indel_;
		this->contains_other_ = other.contains_other_;
	}
	return *this;
}

AlleleList & AlleleList::operator = (AlleleList && other)
{
	if (this != &other)
	{
		this->list_ = std::move(other.list_);
		this->size_ = other.size_;
		this->contains_snp_   = other.contains_snp_;
		this->contains_indel_ = other.contains_indel_;
		this->contains_other_ = other.contains_other_;
	}
	return *this;
}

void AlleleList::append(const Allele & allele)
{
	if (! this->contains_snp_)
		this->contains_snp_ = allele.type(AlleleType::SNP);
	
	if (! this->contains_indel_)
		this->contains_indel_ = allele.type(AlleleType::INDEL);
	
	if (! this->contains_other_)
		this->contains_other_ = allele.type(AlleleType::OTHER);
	
	this->list_.push_back(allele);
}

void AlleleList::append(Allele && allele)
{
	if (! this->contains_snp_)
		this->contains_snp_ = allele.type(AlleleType::SNP);
	
	if (! this->contains_indel_)
		this->contains_indel_ = allele.type(AlleleType::INDEL);
	
	if (! this->contains_other_)
		this->contains_other_ = allele.type(AlleleType::OTHER);
	
	this->list_.push_back(std::move(allele));
}

const Allele & AlleleList::operator [] (const int i) const
{
    if (i < 0 || i >= this->size_)
    {
        throw std::out_of_range("No allele defined for '" + std::to_string(i) + "'");
    }
    
    return this->list_[i];
}

const Allele & AlleleList::operator [] (const Haplotype & haplotype) const
{
    const int i = (int)haplotype;
    
    if (i < 0 || i >= this->size_)
    {
        throw std::out_of_range("No allele defined for haplotype '" + std::to_string(i) + "'");
    }
    
    return this->list_[i];
}

bool AlleleList::contains_snp() const
{
    return this->contains_snp_;
}

bool AlleleList::contains_indel() const
{
    return this->contains_indel_;
}

bool AlleleList::contains_other() const
{
    return this->contains_other_;
}

int AlleleList::size() const
{
    return this->size_;
}

void AlleleList::print(std::ostream & stream, const char last) const
{
    if (this->size_ == 0)
    {
        stream << '.';
    }
    else
    {
        char sep = NULL;
        for (int i = 0; i < this->size_; ++i)
        {
            stream << sep << i << ':' << this->list_[i].base();
            sep = ',';
        }
    }
    if (last != '\0')
        stream << last;
}

void AlleleList::print(FILE * fp, const char last) const
{
    if (this->size_ == 0)
    {
        fprintf(fp, ".");
    }
    else
    {
        char sep = NULL;
        for (int i = 0; i < this->size_; ++i)
        {
            fprintf(fp, "%c%d:%s", sep, i, this->list_[i].base().c_str());
            sep = ',';
        }
    }
    if (last != '\0')
        fprintf(fp, "%c", last);
}


std::string AlleleList::str() const
{
    std::ostringstream oss;
    this->print(oss);
    return oss.str();
}


