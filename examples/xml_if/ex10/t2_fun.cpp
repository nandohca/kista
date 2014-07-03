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
 
#include "fun/process_data.hpp"
 
extern "C"
BEGIN_TASK_WITH_INIT(t2_fun,
// --------------------------	
// init and state section
// --------------------------
    // Declaration and initialization of internal variables
	my_type  *in_var;
	void     *voidp;
	char  out_var;	
	
	// retrieve channels used by the task for communication
	fifo_buffer<void *> *in1;
	fifo_buffer<char> *out1;
	
	if(fifo_buffer_voidp.find("fifo1")==fifo_buffer_voidp.end()) {
		cout << "T2: Error retrieving fifo1 reference" << endl;
		exit(-1);
	}

	if(fifo_buffer_char.find("fifo2")==fifo_buffer_char.end()) {
		cout << "T2: Error retrieving fifo2 reference" << endl;
		exit(-1);
	}	

	in1 = fifo_buffer_voidp["fifo1"];		
	out1 = fifo_buffer_char["fifo2"];
)

//--------------
// Task code
//--------------
		
		in1->read(voidp);
		
		in_var = (my_type *)voidp;
		
		cout << "T2: recv: (" << in_var->a << "," << in_var->b << ") at time " << sc_time_stamp() << endl;
		
		out_var = process_my_type(in_var->a,in_var->b);
		
		CONSUME_T;
				
		cout << "T2: send: " << out_var << " at time " << sc_time_stamp() << endl;
		
		out1->write(out_var);
		
END_TASK
