/*****************************************************************************

  t2_fun.cpp
  
  Task function for t2
  
  Periodic task functionality attached to a XML description
 

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/
    
#include "kista.hpp"   

extern "C"
BEGIN_TASK_WITH_INIT(t2,

	fifo_buffer<unsigned short> *in1;
	fifo_buffer<unsigned short> *out1;
	if(fifo_buffer_ushort.find("fifo"+std::to_string(_T2_IN))==fifo_buffer_ushort.end()) {
		cout << "T2: Error retrieving fifo1 reference" << endl;
		exit(-1);
	}	
#ifdef _T2_OUT
	if(fifo_buffer_ushort.find("fifo"+std::to_string(_T2_OUT))==fifo_buffer_ushort.end()) {
		cout << "T1: Error retrieving fifo1 reference" << endl;
		exit(-1);
	}
	out1 = fifo_buffer_ushort["fifo"+std::to_string(_T2_OUT)];
#endif
	in1 = fifo_buffer_ushort["fifo"+std::to_string(_T2_IN)];
	
	unsigned short in_var;
	unsigned short out_var;	
		     
		     
	
)
in1->read(in_var);
	cout << "T2: recv: " << in_var << " at time " << sc_time_stamp() << endl;
#ifdef _T2_OUT
	out_var=2;
	cout << "T2: send: " << out_var << " at time " << sc_time_stamp() << endl;
	out1->write(out_var);
#endif	
	
END_TASK_AFTER_WCET