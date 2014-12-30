/*****************************************************************************

  t2_fun.cpp
  
  Task function for t1
  
  Task function contains the task functionality and also communication and
  synchronization calls (therefore, it is not "pure" functionality)
 
  Task function is a very generic mechanism for supporting any task
  "processing and communication structure".
  
  From the XML interface, specific templates of "processing and communication structure"
  can be used to automate the generation of this structure, so to hide this
  from the user.

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/
    
#include "kista.hpp"   
  
extern "C"
BEGIN_TASK_WITH_INIT(t2_fun,
// --------------------------	
// init and state section
// --------------------------
    // Declaration and initialization of internal variables

)

//--------------
// Task code
//--------------

		cout << "T2: exec at time " << sc_time_stamp() << endl;
		
		CONSUME_T;
		
END_TASK
