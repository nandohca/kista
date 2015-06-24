/*****************************************************************************

  load_analysis.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: University of Cantabria
  Deparment:   TEISA Dpt
  Date: 2015 June

 *****************************************************************************/

#ifndef ANALYSIS_HPP
#define ANALYSIS_HPP

#include <string>
#include <map>

#include "systemc.h"

namespace kista {
	
class load_unit {
public:	
	std::string		name;
	sc_time			wc_load;
	sc_time			period;
	sc_time			rel_deadline;
	sc_time			wc_response;
	unsigned int	priority;
};

// orders the load units just by priority (does not presume rate monotonic or similar)
bool operator<(const load_unit &lhs, const load_unit &rhs);


// load_unit_set:
//     ordered set of load units. There can be load units with the same priority
typedef std::multimap<unsigned int, load_unit> load_unit_set_t;

// load_unit_set_with_unique_priorities_t:
//     ordered set of load units. Eeach load unit has UNIQUE priorities
typedef std::map<unsigned int, load_unit> load_unit_set_with_unique_priorities_t;

// Observation: maybe a priority_queue could be used also for the implementation of the assess_precise_bound function

//
// Precise assessment if a set of periodic load units is schedulable
// given their deadlines
// The function fill the worst-case responde times of each load unit
// accounting for the interferece caused by the other units
// and assess if the response time exceeds the deadline
//
//
// The function returns false if any response time exceeds its the deadline 
// true otherwise
//
//  By default, the function assess all the response times even if any
//  intermediate response times exceed a deadline.
//
//  If the input parameter immediate_exit==true, then the function returns
//  immediately if any response time exceeds its deadline
// (all response times might not be calculated in case of non-schedulability)
//
//  A false flag (or not parameter) ensures the calculation of all the parameters
//  although the analysis can take more time (it is the default behaviour)
//
bool assess_precise_bound(load_unit_set_with_unique_priorities_t &loaders, bool immediate_exit=false);


// This is the analysis for the generic case. It considers the interference
// of loaders with the same priority. It leads to a more pesimistic bouds.
bool assess_precise_bound(load_unit_set_t &loaders, bool immediate_exit=false);

} // namespace kista

#endif
