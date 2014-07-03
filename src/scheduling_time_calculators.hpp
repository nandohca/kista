/*****************************************************************************

  scheduling_time_calculators.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 May
   
  Notes: KisTA provided functions for quick incorporation of time scheduling calculators

 *****************************************************************************/
 
#ifndef SCHEDULING_TIME_CALCULATORS_HPP
#define SCHEDULING_TIME_CALCULATORS_HPP

#include "scheduler_penalties.hpp"

namespace kista {
	
// ----------------------------------------------------
// To set KisTA provided scheduling time calculators
// ---------------------------------------------------

void set_default_scheduling_time_calculators();

// ----------------------------------------------------
// KisTA provided scheduling time calculators
// ---------------------------------------------------

// model the scheduling time as a constant
sc_time sched_time_constant(scheduler *sched);

// model the scheduling proportional to the number of assigned tasks
sc_time sched_time_proportional_to_assigned_tasks(scheduler *sched);

// model the scheduling proportional to the number of active tasks (among assigned tasks)
sc_time sched_time_proportional_to_ready_tasks(scheduler *sched);

} // namespace KisTA

#endif
