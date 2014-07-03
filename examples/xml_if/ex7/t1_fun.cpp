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
  
 
extern "C"
void t1_fun() {
	
	//fifo_buffer<unsigned int> *out1;
	// fifo_buffer<short> *out1;
	fifo_buffer<unsigned short> *out1;
	
	//unsigned int out_var;
	//short out_var;
	unsigned short out_var;
	
	//if(fifo_buffer_uint.find("fifo1")==fifo_buffer_uint.end()) {
	//if(fifo_buffer_short.find("fifo1")==fifo_buffer_short.end()) {
	if(fifo_buffer_ushort.find("fifo1")==fifo_buffer_ushort.end()) {
		cout << "T1: Error retrieving fifo1 reference" << endl;
		exit(-1);
	}
	
	//out1 = fifo_buffer_uint["fifo1"];
	//out1 = fifo_buffer_short["fifo1"];
	out1 = fifo_buffer_ushort["fifo1"];

	while(true) {
		
		cout << "T1: exec. at time " << sc_time_stamp() << endl;
		
		CONSUME_T;
		
		out_var = 1;
		
		cout << "T1: send 1 " << out_var << "at time " << sc_time_stamp() << endl;
		out1->write(out_var);
		
		yield();
	}
}
