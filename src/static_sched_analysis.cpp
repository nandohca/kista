/*****************************************************************************

  static_sched_analysis.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January
   
  Notes: Implementation of the collection of functions for static schedulability analylis
         incorporated to KisTA

 *****************************************************************************/

#ifndef STATIC_SCHED_ANALYSIS_CPP
#define STATIC_SCHED_ANALYSIS_CPP

#include "static_sched_analysis.hpp"

namespace kista {

void enable_sched_static_analysis() {
	attach_default_available_static_analysis();
	static_sched_analysis_enabled_flag = true;
}

void disable_sched_static_analysis() {
	static_sched_analysis_enabled_flag = false;
}

bool is_static_analysis_enabled() {
	return static_sched_analysis_enabled_flag;
}

// utility function where the built-in provided analysis are added
void attach_default_available_static_analysis() {
	std::string msg;
	scheduler_t current_scheduler_t;	
	
	// Add default common static schedulability analysis
	common_static_analysis = full_utilization_bound;
	
	// Add default policy dependnet static schedulability analysis
	
	// Liu & Layland bound check
	current_scheduler_t.scheduler_policy = STATIC_PRIORITIES;
	current_scheduler_t.static_priorities_policy = RATE_MONOTONIC;
	current_scheduler_t.dynamic_priorities_policy = USER_DYNAMIC_PRIORITY_POLICY; // actually not used
	current_scheduler_t.scheduler_triggering_policy = PREEMPTIVE;
	
	static_analysis_table[current_scheduler_t]=Liu_and_Layland_bound;

#ifdef _REPORT_DEFAULT_STATIC_ANALYSIS
	msg = "Common static schedulability analysis (full utilization) enabled.\n";
	msg += to_string(static_analysis_table.size());
	msg += " policy specific analyses enabled.";
	SC_REPORT_INFO("KisTA",msg.c_str());
#endif
}


// User method to replace the KisTA provided common scheduling analysis
//    - KisTA analysis provided: full utilization (any police)
// --------------------------------------------------------------------------------
void attach_static_analysis(SAFPTR fun) {
#ifdef _REPORT_COMMON_STATIC_ANALYSIS_REPLACEMENT
	std::string msg;
	msg = "Common static analysis replaced.";
	SC_REPORT_INFO("KisTA",msg.c_str());
#endif
	common_static_analysis = fun;
}

// User method to replace an analysis already provided or to provide a new one
// for a given scheduling policy
//    - KisTA analysis provided: full utilization (any police), L&L (RMA)
// --------------------------------------------------------------------------------
void attach_static_analysis(SAFPTR fun, scheduler_t  	sched_t) {
	std::string msg;
	static_analysis_table_t::iterator it;
	
	// check if there is already an associated static analysis and replaces it.
	it = static_analysis_table.find(sched_t);
#ifdef _REPORT_STATIC_ANALYSIS_ADDITION
	if(it != static_analysis_table.end()) {		
		// There is already an analyis, which will be replaced
		msg = "There was already an static analysis associated to scheduling policy of type ";
		msg += sched_t.to_string();
		msg += ". It is replaced by the user provided analysis.";
	} else {
		msg = "Adding Static analysis for scheduling policy of type ";
		msg += sched_t.to_string();
	}
	SC_REPORT_INFO("KisTA",msg.c_str());
#endif
	// insert a new analysis
	static_analysis_table[sched_t]=fun;
}

// Implementation of KisTA provided static analysis methods

void full_utilization_bound(const taskset_by_name_t *assigned_tasks) {
	std::string msg;
	double assigned_tasks_utilization;
	msg = "Passing full utilization bound...";

	if (is_periodic(*assigned_tasks)) {
		assigned_tasks_utilization = utilization(*assigned_tasks);
		if(assigned_tasks_utilization>=1.0) {
			msg += "FAILED.\n  Utilization = " ;
			msg += std::to_string(assigned_tasks_utilization);
			SC_REPORT_ERROR("KisTA",msg.c_str());
		} else {
			msg += "PASSED.\n  Utilization = " ;
			msg += std::to_string(assigned_tasks_utilization);		
			SC_REPORT_INFO("KisTA",msg.c_str());
		}
	} else { // if the task set is not periodic, the utilization cannot be 
	         // calculated statically at least for one task
	         // (since at least one task is not static or there is not period)
	         msg += "SKIPPED.\n The taskset is not periodic.";
	         SC_REPORT_WARNING("KisTA",msg.c_str());
	}
}


void Liu_and_Layland_bound(const taskset_by_name_t *assigned_tasks) {
	std::string msg;
	double assigned_tasks_utilization;
	unsigned int N_int;
	double N;
	
	double bound;
	
	msg = "Checking Liu&Layland utilization bound ( U <= N (2^(1/N) -1) )...";
	
	N_int = assigned_tasks->size();
	N = (double)N_int;
	
	// calculate 2^(1/N)
	bound =  ( pow (2.0 , 1.0/N) - 1.0) * N;
	
	assigned_tasks_utilization = utilization(*assigned_tasks);

	if(assigned_tasks_utilization>=bound) {
		msg += "FAILED.\n";	
	} else {
		msg += "PASSED.\n";		
	}
	
	msg += "Utilization = ";
	msg += std::to_string(assigned_tasks_utilization);
	msg += ", N=";
	msg += std::to_string(N_int);
	msg += ", L&L bound=";
	msg += std::to_string(bound);	
		
	if(assigned_tasks_utilization>=bound) {
		SC_REPORT_ERROR("KisTA",msg.c_str());
	} else {	
		SC_REPORT_INFO("KisTA",msg.c_str());
	}
				
}

void precise_bound(const taskset_by_name_t *assigned_tasks) {
		std::string msg;
	/*
	 * double assigned_tasks_utilization;
	unsigned int N_int;
	double N;
	
	double bound;
	*/
	
	msg = "Precise schedulability_analysis ...";

	msg += "This schedulability analysis is not implemented.\n";
	SC_REPORT_ERROR("KisTA",msg.c_str());

/*	
	N_int = assigned_tasks->size();
	N = (double)N_int;
	
	// calculate 2^(1/N)
	bound =  ( pow (2.0 , 1.0/N) - 1.0) * N;
	
	assigned_tasks_utilization = utilization(*assigned_tasks);

	if(assigned_tasks_utilization>=bound) {
		msg += "FAILED.\n";	
	} else {
		msg += "PASSED.\n";		
	}
	
	msg += "Utilization = ";
	msg += std::to_string(assigned_tasks_utilization);
	msg += ", N=";
	msg += std::to_string(N_int);
	msg += ", L&L bound=";
	msg += std::to_string(bound);	
		
	if(assigned_tasks_utilization>=bound) {
		SC_REPORT_ERROR("KisTA",msg.c_str());
	} else {	
		SC_REPORT_INFO("KisTA",msg.c_str());
	} 
	*/
}


} // namespace KisTA

#endif

