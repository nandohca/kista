/*****************************************************************************

  scheduler_penalties.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 May
   
  Notes: Functions to control the modelling of penalties for schedulers

 *****************************************************************************/
 
#ifndef SCHEDULER_PENALTIES_HPP
#define SCHEDULER_PENALTIES_HPP

#include "taskset_by_name_t.hpp"
#include "global_elements.hpp"
#include "scheduler.hpp"

namespace kista {
	
void enable_scheduling_times();  
void disable_scheduling_times(); // disabled by default

bool are_scheduling_times_enabled(); // return true if scheduling times are considered in the analysis

// User function to set the function for calculating scheduling times
//   - Applicable to all scheduler instances whatever its scheduling policy
// --------------------------------------------------------------------------------
void set_scheduling_time_calculator(SCHTIME_FPTR fun);

// User function to set the function for calculating scheduling times
//   - Applicable to all scheduler instances configured for a given scheduling policy
// --------------------------------------------------------------------------------
void set_scheduling_time_calculator(SCHTIME_FPTR fun, scheduler_t sched_t);

// User function to set the function for calculating scheduling times ONLY
// for a SPECIFIC SCHEDULER INSTANCE
// --------------------------------------------------------------------------------
void set_scheduling_time_calculator(SCHTIME_FPTR fun, scheduler *sched);

// set constant scheduling time for a specific scheduler instance
// (takes effect only if not scheduling time calculator assigned)
void set_scheduling_time(sc_time time, scheduler *sched);

// set constant scheduling time for all scheduler instances
// (takes effect in a scheduler instance only if not scheduling time calculator assigned and o specific scheduling time assigned)
void set_scheduling_time(sc_time time);

// Get a scheduler specific scheduling time calculation function (applicable to all scheduler instances configured with scheduling policy sched_t
//                                                                unless an instance specific calculator assigned, see documentation for more details)
SCHTIME_FPTR get_scheduling_time_calculator(scheduler_t sched_t);

// Get the common scheduling time calculation function (applicable to all schedulers instances, unless a scheduling policy specific, or
//                                                      an instance specific calculator assigned, see documentation for more details))
SCHTIME_FPTR get_scheduling_time_calculator();

} // namespace KisTA

#endif
