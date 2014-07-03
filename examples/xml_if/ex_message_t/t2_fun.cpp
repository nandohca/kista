/*****************************************************************************

  t2_fun.cpp
  
  Task function for t2
  
  Reads from the channel which transfers generic data with message_t

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
    my_type  *in_var;	
	message_t in_msg;
	
	void *voidp;
	
	FIFO_BUFFER_MSG_REF(in1,"fifo1");
	
)

//--------------
// Task code
//--------------
		
		in1->read(in_msg);
		
		//voidp = in_msg;
		//in_var = (my_type *)voidp;
		
		in_var = (my_type *)(void *)in_msg;
		
		//in_var = (my_type *)in_msg;

		cout << sc_time_stamp() << ": " << endl;
		cout << "T2: \t\t\t rec: ";
		cout << "a= (";
		for(unsigned int i=0;i<8;i++) {
			cout << in_var->a[i];
			if(i<7) cout << ",";
		}
		cout << ") ; b= ";
		cout << in_var->b;
		cout << endl; 
						
		CONSUME_T;
		
END_TASK
