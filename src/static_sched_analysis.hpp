/*****************************************************************************

  static_sched_analysis.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January
   
  Notes: Collection of functions for static schedulability analylis
         incorporated to KisTA

 *****************************************************************************/
 
#ifndef STATIC_SCHED_ANALYSIS_HPP
#define STATIC_SCHED_ANALYSIS_HPP

#include "taskset_by_name_t.hpp"
#include "global_elements.hpp"

namespace kista {

// as the user method, but it does not involve any message
void attach_default_static_analysis(SAFPTR fun, scheduler_t  	sched_t);


void enable_sched_static_analysis();  
void disable_sched_static_analysis(); // disabled by default

bool is_static_analysis_enabled(); // return true if static schedulability analysis is enabled

// User method to override the KisTA provided common scheduling analysis
//    - KisTA analysis provided: full utilization (any police)
// --------------------------------------------------------------------------------
void attach_static_analysis(SAFPTR fun);

// User method to override an analysis already provided or to provide a new one
// for a given scheduling policy
//    - KisTA analysis provided: full utilization (any police), L&L (RMA)
// --------------------------------------------------------------------------------
void attach_static_analysis(SAFPTR fun, scheduler_t  	sched_t);

	// Some hooks for commodity
//	void attach_static_analysis(SAFPTR fun, scheduler_policy_t  			scheduler_policy);
//	void attach_static_analysis(SAFPTR fun, scheduler_policy_t  			scheduler_policy, static_priorities_policy_t  	static_priorities_policy);
//	void attach_static_analysis(SAFPTR fun, scheduler_policy_t  			scheduler_policy, dynamic_priorities_policy_t  	dynamic_priorities_policy);
	
// STATIC ANALYSIS PROVIDED:

// checks if a given task set utilization is below a 100% bound
void full_utilization_bound(const taskset_by_name_t *assigned_tasks);

// precise utilization bound (based on the calculation of response times
// (REF: )
// Necessary and sufficient check for schedulability
// capable to state if a task set is schedulable for utilizations
// over Liu&Layland bound, but below 100%
void precise_schedulability_analysis(const taskset_by_name_t *assigned_tasks);

// checks if a given task set utilization is below the Liu&Layland bound
// That is a sufficient condition for schedulability
void Liu_and_Layland_bound(const taskset_by_name_t *assigned_tasks);


// utility function where the built-in provided analysis are added
void attach_default_available_static_analysis();

} // namespace KisTA

#endif
