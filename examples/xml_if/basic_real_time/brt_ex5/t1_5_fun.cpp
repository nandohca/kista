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
  

void t1_5_fun() {
		cout << "T1_5: exec. at time " << sc_time_stamp() << endl;
}

extern "C"		
TASK(t1_5,t1_5_fun(););
