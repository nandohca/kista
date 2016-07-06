/*****************************************************************************

  t1_fun.cpp
  
  Task function for t1
  
  Periodic task functionality attached to a XML description
 
  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/
    
#include "kista.hpp"   
  

void t1_fun() {
		cout << "T1: exec. at time " << sc_time_stamp() << endl;
}

extern "C"		
PERIODIC_TASK(T1_periodic_task_wrapper,t1_fun);
