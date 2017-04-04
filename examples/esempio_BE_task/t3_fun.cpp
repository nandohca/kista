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

/*
void t3_fun() {
		cout << "T3: exec. at time " << sc_time_stamp() << endl;
}

extern "C" 
PERIODIC_TASK(T3_periodic_task_wrapper,t3_fun);

*/  

extern "C"
BEGIN_TASK_WITH_INIT(t3,
		     
	fifo_buffer<unsigned short> *out1;
	unsigned short out_var;
	if(fifo_buffer_ushort.find("fifo"+std::to_string(_D1_OUT))==fifo_buffer_ushort.end()) {
		cout << "T1: Error retrieving fifo1 reference" << endl;
		exit(-1);
	}
	out1 = fifo_buffer_ushort["fifo"+std::to_string(_D1_OUT)];
		     
		     
		     
		     
)

	out_var = 1;
	cout << "T3: send 1 " << out_var << "at time " << sc_time_stamp() << endl;
	out1->write(out_var);
	cout << "T3: exec at time " << sc_time_stamp() << endl;

	CONSUME_T;					
	wait_period();				
	}								
}

