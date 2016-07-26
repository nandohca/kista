/*****************************************************************************

  t2_fun.cpp
  
  Task function for t2
  
  Periodic task functionality attached to a XML description
 

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/
    
#include "kista.hpp"   


void t2_fun() {
		cout << "T2: exec. at time " << sc_time_stamp() << endl;
}

extern "C" 
PERIODIC_TASK(T2_periodic_task_wrapper,t2_fun);

/*  
extern "C"
BEGIN_PERIODIC_TASK(t2_fun)
	cout << "T2: exec at time " << sc_time_stamp() << endl;
END_PERIODIC_TASK
*/
