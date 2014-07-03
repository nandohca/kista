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

#include "fun/gen_data.hpp"

extern "C" 
BEGIN_TASK_WITH_INIT(t1_fun, 

// --------------------------	
// init and state section
// --------------------------
    // Declaration and initialization of internal variables
	my_type out_var;
	
	char my_char;
	unsigned int my_uint;
	
	// retrieve channels used by the task for communication
	fifo_buffer<void *> *out1;
	
	if(fifo_buffer_voidp.find("fifo1")==fifo_buffer_voidp.end()) {
		cout << "T1: Error retrieving fifo1 reference" << endl;
		exit(-1);
	}
	
	out1 = fifo_buffer_voidp["fifo1"];
)

//--------------
// Task code
//--------------		
		cout << "T1: exec. at time " << sc_time_stamp() << endl;
		
		// functionality
		my_char = gen_rand_char();
		my_uint = gen_rand_uint();
		
		CONSUME_T;
		
		out_var.a=my_uint;
		out_var.b=my_char;
		
		cout << "T1: send: (" << out_var.a << "," << out_var.b << ") at time " << sc_time_stamp() << endl;
		out1->write((void *)&out_var);
		
END_TASK
