/*****************************************************************************

  t1_fun.cpp
  
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
  
#include "types.hpp"   
 
extern "C"
void t1_fun() {
	
	fifo_buffer<void *> *out1;
	
	my_type out_var;
	
	if(fifo_buffer_voidp.find("fifo1")==fifo_buffer_voidp.end()) {
		cout << "T1: Error retrieving fifo1 reference" << endl;
		exit(-1);
	}
	
	out1 = fifo_buffer_voidp["fifo1"];

	while(true) {
		
		cout << "T1: exec. at time " << sc_time_stamp() << endl;
		
		CONSUME_T;
		
		out_var.a=3.0;
		out_var.b='c';
		
		cout << "T1: send: (" << out_var.a << "," << out_var.b << ") at time " << sc_time_stamp() << endl;
		out1->write((void *)&out_var);
		
		yield();
	}
}
