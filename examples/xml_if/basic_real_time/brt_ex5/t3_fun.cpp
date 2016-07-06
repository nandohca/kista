/*****************************************************************************

  t3_fun.cpp
  
  Task function for t3
  
  Periodic task functionality attached to a XML description
  
  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/
    
#include "kista.hpp"   


void t3_fun() {
		cout << "T3: exec. at time " << sc_time_stamp() << endl;
}

extern "C" 
PERIODIC_TASK(T3_periodic_task_wrapper,t3_fun);

/*  
extern "C"
BEGIN_PERIODIC_TASK(t3_fun)
	cout << "T3: exec at time " << sc_time_stamp() << endl;
GENERIC_END_PERIODIC_TASK
*/
