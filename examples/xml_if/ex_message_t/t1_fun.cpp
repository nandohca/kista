/*****************************************************************************

  t1_fun.cpp
  
  Task function for t1
  
  Task  which transfer a generic data and uses message_t to transfer it

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
	
	FIFO_BUFFER_MSG_REF(out1,"fifo1");

)

//--------------
// Task code
//--------------		
		cout << "T1: exec. at time " << sc_time_stamp() << endl;

my_type pepe;

cout << "SIZE  my_type: " << sizeof(my_type) << endl;
cout << "SIZE  pepe: " << sizeof(pepe) << endl;

		
		// functionality
		out_var.b = gen_rand_char();
		for(unsigned int i=0;i<8;i++) {
			out_var.a[i] = gen_rand_uint();
		}

		CONSUME_T;		
cout << "my_type SIZE: " << sizeof(my_type) << endl;		
cout << "out_var SIZE: " << sizeof(out_var) << endl;		
		cout << sc_time_stamp() << ": " << endl;
		cout << "T1: send: ";
		cout << "a= (";
		for(unsigned int i=0;i<8;i++) {
			cout << out_var.a[i];
			if(i<7) cout << ",";
		}
		cout << ") ; b= ";
		cout << out_var.b; 
		cout << endl;
cout << "SIZE OF out_var : " << sizeof(out_var) << endl;
		message_t out_msg(&out_var,sizeof(out_var));
		
		out1->write(out_msg);
		
END_TASK
