//
//  shared.cpp
//  ship
//
//  Created by Patrick Albers on 11.11.2014.
//  Copyright (c) 2014 Patrick K. Albers. All rights reserved.
//

#include "shared.h"



RareHaplotype::RareHaplotype(const Source & source, const Census & cutoff)
: n_marker(0)
, n_allele(0)
{
	ProgressBar progress(source.marker_size());
	
	for (size_t i = 0; i < source.marker_size(); ++i)
	{
		const Marker * m = &source.marker(i);
		bool flag = false;
		
		for (int k = 0; k < m->stat.haplotype.size(); ++k)
		{
			if (m->stat.haplotype.census(k) >  size_t(1) &&
				m->stat.haplotype.census(k) <= cutoff)
			{
				this->list.push_back(std::make_pair(m, m->stat.haplotype.type(k)));
				++this->n_allele;
				flag = true;
			}
		}
		
		if (flag)
			++this->n_marker;
	}
	
	progress.finish();
}



