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

//
// ordered map: there can be load units with the same priority
//              but they are ordered by priority
//
typedef std::map<unsigned int, load_unit> load_unit_set_t;

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
//
bool assess_precise_bound(load_unit_set_t loaders, bool immediate_exit=false);

} // namespace kista

#endif
