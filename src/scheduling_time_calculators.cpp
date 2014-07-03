/*****************************************************************************

  scheduling_time_calculators.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 May
   
  Notes: KisTA provided functions for quick incorporation of time scheduling calculators

 *****************************************************************************/
 
#ifndef SCHEDULING_TIME_CALCULATORS_CPP
#define SCHEDULING_TIME_CALCULATORS_CPP

#include "scheduling_time_calculators.hpp"

namespace kista {
	
// ----------------------------------------------------
// To set KisTA provided scheduling time calculators
// ---------------------------------------------------

void set_default_scheduling_time_calculators() {
	// set the common generic scheduling time calculator
	set_scheduling_time_calculator(sched_time_proportional_to_ready_tasks);
	
	// set a scheduling time calculator for the static scheduling policy
	scheduler_t current_scheduler_t;	
		
	// Add default policy dependent static schedulability analysis
	
	// Liu & Layland bound checj
	current_scheduler_t.scheduler_policy = STATIC_SCHEDULING;
//	current_scheduler_t.static_priorities_policy = ;
//	current_scheduler_t.dynamic_priorities_policy = ; // actually not used
//	current_scheduler_t.scheduler_triggering_policy = ;

	set_scheduling_time_calculator(sched_time_constant,current_scheduler_t);

}

// ----------------------------------------------------
// KisTA provided scheduling time calculators
// ---------------------------------------------------


//
// model the scheduling time as a constant
//

sc_time sched_time_constant(scheduler *sched) {
	return global_scheduling_time;
}

//
// model the scheduling time as a value proportional to the assigned tasks
// (assuming that task assignement is static, it will lead to a constant
// value, that is time-invariant value during simulation)
//

sc_time sched_time_proportional_to_assigned_tasks(scheduler *sched)  {
	sc_time scheduling_time;
	taskset_by_name_t *assigned_tasks;
	assigned_tasks = sched->gets_tasks_assigned();
	scheduling_time = global_scheduling_time * assigned_tasks->size();
	return scheduling_time;
}

//
// model the scheduling time as a value proportional to the assigned tasks
// (assuming that task assignement is static, it will lead to a constant
// value, that is time-invariant value during simulation)
//

sc_time sched_time_proportional_to_ready_tasks(scheduler *sched)  {
	sc_time scheduling_time;
	unsigned int n_active_tasks;
	taskset_by_name_t *assigned_tasks;
	assigned_tasks = sched->gets_tasks_assigned();
	n_active_tasks = 0;
	for(auto it=assigned_tasks->begin();it!=assigned_tasks->end();it++) {
		if(
		  (it->second->active_signal.read()==true) || 
		  (it->second->state_signal.read()=='R') ||
		  (it->second->state_signal.read()=='E')
		  )
		 n_active_tasks++;
	}
	scheduling_time = global_scheduling_time * n_active_tasks;
//cout << " prop. to ready: scheduling time: " << scheduling_time << endl;		
	return scheduling_time;
}

} // namespace KisTA

#endif
