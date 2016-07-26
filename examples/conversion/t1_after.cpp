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
  

extern "C"
BEGIN_TASK_WITH_INIT(t1,
	fifo_buffer<unsigned short> *in1;
	fifo_buffer<unsigned short> *out1;
	if(fifo_buffer_ushort.find("fifo1")==fifo_buffer_ushort.end()) {
		cout << "T2: Error retrieving fifo1 reference" << endl;
		exit(-1);
	}	
	if(fifo_buffer_ushort.find("fifo2")==fifo_buffer_ushort.end()) {
		cout << "T2: Error retrieving fifo2 reference" << endl;
		exit(-1);
	}
	in1 = fifo_buffer_ushort["fifo1"];
	out1 = fifo_buffer_ushort["fifo2"];
	unsigned short in_var;
	unsigned short out_var;	
	
)

	in1->read(in_var);
	cout << "T2: recv: " << in_var << " at time " << sc_time_stamp() << endl;
	out_var=2;
	cout << "T2: send: " << out_var << " at time " << sc_time_stamp() << endl;
	out1->write(out_var);
	
	
END_TASK_AFTER_WCET