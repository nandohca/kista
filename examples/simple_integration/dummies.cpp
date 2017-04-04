#include "kista.hpp"   
#include "prova_header.hpp"

#ifdef _D1_IN
extern "C"
BEGIN_TASK_WITH_INIT(d1in,
		     
	fifo_buffer<unsigned short> *out1;
	unsigned short out_var;
	if(fifo_buffer_ushort.find("fifo"+std::to_string(_D1_IN))==fifo_buffer_ushort.end()) {
		cout << "D1: Error retrieving fifo1 reference" << endl;
		exit(-1);
	}
	out1 = fifo_buffer_ushort["fifo"+std::to_string(_D1_IN)]; 
)
	CONSUME_T;					
	out_var = 1;
	cout << "D1: send 1 " << out_var << "at time " << sc_time_stamp() << endl;
	out1->write(out_var);
	cout << "D1: exec at time " << sc_time_stamp() << endl;

	wait_period();				
	}								
}
#endif

#ifdef _D2_IN
extern "C"
BEGIN_TASK_WITH_INIT(d2in,
		     
	fifo_buffer<unsigned short> *out1;
	unsigned short out_var;
	if(fifo_buffer_ushort.find("fifo"+std::to_string(_D2_IN))==fifo_buffer_ushort.end()) {
		cout << "D2: Error retrieving fifo1 reference" << endl;
		exit(-1);
	}
	out1 = fifo_buffer_ushort["fifo"+std::to_string(_D2_IN)]; 
)
	CONSUME_T;					
	out_var = 1;
	cout << "D2: send 1 " << out_var << "at time " << sc_time_stamp() << endl;
	out1->write(out_var);
	cout << "D2: exec at time " << sc_time_stamp() << endl;

	wait_period();				
	}								
}
#endif

/*
 * #ifdef _D3_IN
extern "C"
BEGIN_TASK_WITH_INIT(d3in,
		     
	fifo_buffer<unsigned short> *out1;
	unsigned short out_var;
	if(fifo_buffer_ushort.find("fifo"+std::to_string(_D3_IN))==fifo_buffer_ushort.end()) {
		cout << "D3: Error retrieving fifo1 reference" << endl;
		exit(-1);
	}
	out1 = fifo_buffer_ushort["fifo"+std::to_string(_D3_IN)]; 
)
	CONSUME_T;					
	out_var = 1;
	cout << "D3: send 1 " << out_var << "at time " << sc_time_stamp() << endl;
	out1->write(out_var);
	cout << "D3: exec at time " << sc_time_stamp() << endl;

	wait_period();				
	}								
}
#endif

*/
#ifdef _D1_OUT
extern "C"
BEGIN_TASK_WITH_TOKEN_IN(d1out,_D1_OUT,)
END_TASK_WITH_TOKEN_IN
#endif

#ifdef _D2_OUT
extern "C"
BEGIN_TASK_WITH_TOKEN_IN(d2out,_D2_OUT,)
END_TASK_WITH_TOKEN_IN
#endif
/*
#ifdef _D3_OUT
extern "C"
BEGIN_TASK_WITH_TOKEN_IN(d3out,_D3_OUT,)
END_TASK_WITH_TOKEN_IN
#endif
*/


void t1funz() {
		cout << "be1: exec. at time " << sc_time_stamp() << endl;
}

extern "C"		
TASK(be1,t1funz(););


void t2funz() {
		cout << "be2: exec. at time " << sc_time_stamp() << endl;
}

extern "C"		
TASK(be2,t2funz(););


