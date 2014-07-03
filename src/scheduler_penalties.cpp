/*****************************************************************************

  scheduler_penalties.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 May
   
  Notes: Functions to control the modelling of penalties for schedulers

 *****************************************************************************/
 
#ifndef SCHEDULER_PENALTIES_CPP
#define SCHEDULER_PENALTIES_CPP

#include "scheduler_penalties.hpp"

namespace kista {
	
void enable_scheduling_times() {
	scheduling_times_enabled_flag = true;
}

void disable_scheduling_times() {
	scheduling_times_enabled_flag = false;	
}

bool are_scheduling_times_enabled() {
	return scheduling_times_enabled_flag;
}

// User method to override the KisTA provided default scheduling times
//   - Applicable to all scheduler instances whatever its scheduling policy
// --------------------------------------------------------------------------------
void set_scheduling_time_calculator(SCHTIME_FPTR fun) {
#ifdef _REPORT_COMMON_SCHED_TIME_REPLACEMENT
	std::string msg;
	msg = "Common Scheduling Time calculation function replaced.";
	SC_REPORT_INFO("KisTA",msg.c_str());
#endif
	common_scheduling_time_calculator = fun;
}

// User method to override the KisTA procived default scheduling analysis
//   - Applicable to all scheduler instances configured for a given scheduling policy
// --------------------------------------------------------------------------------
void set_scheduling_time_calculator(SCHTIME_FPTR fun, scheduler_t sched_t) {
	std::string msg;
	scheduling_time_calculator_table_t::iterator it;
	
	// check if there is already an associated static analysis and overrides it.
	it = scheduling_time_calculator_table.find(sched_t);
	
#ifdef _REPORT_STATIC_ANALYSIS_ADDITION
	if(it != scheduling_time_calculator_table.end()) {		
		// There is already an analyis, which will be replaced
		msg = "There was already a scheduling time calculation function associated to scheduling policy of type ";
		msg += sched_t.to_string();
		msg += ". It is replaced by the user provided scheduling time calculation function.";
	} else {
		msg = "Adding scheduling time calculation function for scheduling policy of type ";
		msg += sched_t.to_string();
	}
	SC_REPORT_INFO("KisTA",msg.c_str());
#endif
	// insert a new analysis
	scheduling_time_calculator_table[sched_t]=fun;

}

// Get a scheduler specific scheduling time calculation function (applicable to all scheduler instances configured with scheduling policy sched_t
//                                                                unless an instance specific calculator assigned, see documentation for more details)

SCHTIME_FPTR get_scheduling_time_calculator(scheduler_t sched_t) {
	scheduling_time_calculator_table_t::iterator it;
	it=scheduling_time_calculator_table.find(sched_t);
	if(it==scheduling_time_calculator_table.end()) return NULL;
	else return it->second;
}

// Get the common scheduling time calculation function (applicable to all schedulers instances, unless a scheduling policy specific, or
//                                                      an instance specific calculator assigned, see documentation for more details))
SCHTIME_FPTR get_scheduling_time_calculator() {
	return common_scheduling_time_calculator;	
}

// Functions to set the function for calculating scheduling times ONLY
// for a SPECIFIC SCHEDULER INSTANCE
// --------------------------------------------------------------------------------
void set_scheduling_time_calculator(SCHTIME_FPTR fun, scheduler *sched) {
	check_call_at_elaboration("set_scheduling_time_calculator");
	sched->set_scheduling_time_calculator(fun);
}

// set constant scheduling time for a specific scheduler instance
void set_scheduling_time(sc_time time, scheduler *sched) {
	check_call_at_elaboration("set_scheduling_time(sc_time time, scheduler *sched)");
	sched->set_scheduling_time(time);
}

// set constant scheduling time for all scheduler instances
void set_scheduling_time(sc_time time) {
	check_call_at_elaboration("set_scheduling_time(sc_time time)");
	global_scheduling_time = sc_time(time);
}

} // namespace KisTA

#endif
