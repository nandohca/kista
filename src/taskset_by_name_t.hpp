/*****************************************************************************

  taskset_by_name_t.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

 *****************************************************************************/

#ifndef TASKSET_BY_NAME_HPP
#define TASKSET_BY_NAME_HPP

#include "types.hpp"
#include "task_info_t.hpp"
#include <string.h>

namespace kista {

// KisTA Functions which can be performed on the task set

bool is_periodic(const taskset_by_name_t &taskset);    		// to check is a task set is a "periodic task set", that is, all the tasks are periodic (Davis&Burns '09)

sc_time get_hyperperiod(const taskset_by_name_t &taskset);   // Returns the hyper period (least common multiple of all periods , Davis&Burns '09 )
                                                //    This function, first checks that the task set is periodic (so it does not requires a call to the is_periodic function for the task set

double utilization(const taskset_by_name_t &taskset);  // Gets "static" utilization of the taskset (as the addition of the utilizations of the static set)

double density(const taskset_by_name_t &taskset);    // Gets "static" utilization of the taskset (as the addition of the densities of the static set)

} // end namespace kista

#endif
