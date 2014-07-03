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
void t2_fun() {
	
	//fifo_buffer<unsigned int> *in1;
	//fifo_buffer<short> *in1;
	fifo_buffer<unsigned short> *in1;
	fifo_buffer<char> *out1;
	
	//if(fifo_buffer_uint.find("fifo1")==fifo_buffer_uint.end()) {
	//if(fifo_buffer_short.find("fifo1")==fifo_buffer_short.end()) {
	if(fifo_buffer_ushort.find("fifo1")==fifo_buffer_ushort.end()) {
		cout << "T2: Error retrieving fifo1 reference" << endl;
		exit(-1);
	}	
	
	if(fifo_buffer_char.find("fifo2")==fifo_buffer_char.end()) {
		cout << "T2: Error retrieving fifo2 reference" << endl;
		exit(-1);
	}
		
	//in1 = fifo_buffer_uint["fifo1"];
	//in1 = fifo_buffer_short["fifo1"];
	in1 = fifo_buffer_ushort["fifo1"];
	out1 = fifo_buffer_char["fifo2"];
	
	//unsigned int in_var;
	//short in_var;
	unsigned short in_var;
	
	char  out_var;	
	
	while(true) {
		
		in1->read(in_var);
		
		cout << "T2: recv: " << in_var << " at time " << sc_time_stamp() << endl;
		
		CONSUME_T;
		
		out_var='c';
		
		cout << "T2: send: " << out_var << " at time " << sc_time_stamp() << endl;
		
		out1->write(out_var);
		
		yield();
	}
}
