/*****************************************************************************

  t3_fun.cpp
  
  Task function for t3
  
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
void t3_fun() {

	char in_var;
	
	fifo_buffer<char> *in1;
	
	if(fifo_buffer_char.find("fifo2")==fifo_buffer_char.end()) {
		cout << "T3: Error retrieving fifo2 reference" << endl;
		exit(-1);
	}
	
	in1 = fifo_buffer_char["fifo2"];

	while(true) {
		
		in1->read(in_var);
				
		cout << "T3: recv: " << in_var << " at time " << sc_time_stamp() << endl;
		
		CONSUME_T;
		
		yield();
	}
}
