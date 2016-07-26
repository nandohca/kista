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

extern "C"
BEGIN_TASK_WITH_INIT(t2,
	fifo_buffer<unsigned short> *in1;
	
	if(fifo_buffer_ushort.find("fifo2")==fifo_buffer_ushort.end()) {
		cout << "T2: Error retrieving fifo2 reference" << endl;
		exit(-1);
	}
	in1 = fifo_buffer_ushort["fifo2"];
	unsigned short in_var;
	
)

	in1->read(in_var);
	cout << "T2: recv: " << in_var << " at time " << sc_time_stamp() << endl;
		
	
END_TASK_AFTER_WCET