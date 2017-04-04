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
#include "prova_header.hpp"  

extern "C"

BEGIN_TASK_WITH_TOKEN_INOUT(t1,_T1_IN,_T1_OUT,)
  cout<<"fanculo"<<std::endl;
END_TASK_WITH_TOKEN_INOUT(t1)
/*
BEGIN_TASK_WITH_INIT(t1,
	fifo_buffer<unsigned short> *in1;
	fifo_buffer<unsigned short> *out1;
	if(fifo_buffer_ushort.find("fifo"+std::to_string(_T1_IN))==fifo_buffer_ushort.end()) {
		cout << "T2: Error retrieving fifo1 reference" << endl;
		exit(-1);
	}	
#ifdef _T1_OUT
	if(fifo_buffer_ushort.find("fifo"+std::to_string(_T1_OUT))==fifo_buffer_ushort.end()) {
		cout << "T1: Error retrieving fifo1 reference" << endl;
		exit(-1);
	}
	out1 = fifo_buffer_ushort["fifo"+std::to_string(_T1_OUT)];
#endif
	in1 = fifo_buffer_ushort["fifo"+std::to_string(_T1_IN)];
	
	unsigned short in_var;
	unsigned short out_var;	
	
)

	in1->read(in_var);
	cout << "T2: recv: " << in_var << " at time " << sc_time_stamp() << endl;
#ifdef _T1_OUT
	out_var=2;
	cout << "T2: send: " << out_var << " at time " << sc_time_stamp() << endl;
	out1->write(out_var);
#endif
	
END_TASK_AFTER_WCET
*/