// 
// sc_f1.cpp
//
//
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, April
//

#ifndef _SC_F1_CPP
#define _SC_F1_CPP

#include "systemc.h"

#include "sc_f1.h"

#include "functions.h"

#include "basic_scope.h"
#include "wcet_est.h"

#ifdef _RANDOM_VECTORS
 // structure where data wit the estimation instructions are dumpled 
extern unsigned long long results[N_RANDOM_TEST_VECTORS];
#endif

void sc_f1::sc_f1_proc()
{
	unsigned int i;
	unsigned short int in1_buff[SIZE_1];
	unsigned short int in2_buff[SIZE_1];
	unsigned short int mode_var;
	unsigned short int out_buff[SIZE_1];

#ifdef _RANDOM_VECTORS
	unsigned int j = 0;
#endif

	do {

	    for(i=0;i<SIZE_1;i++) {
			in1.read(in1_buff[i]);
			in2.read(in2_buff[i]);	
		}
		mode.read(mode_var);
		
//		cout << "uc_segment_time: " << uc_segment_time << endl;
//		cout << "uc_segment_instr:" << uc_segment_instr << endl;		
		
		uc_segment_time = 0;
		uc_segment_instr = 0;
		
		f1(out_buff,in1_buff,in2_buff,mode_var);

#ifdef _RANDOM_VECTORS
		results[j]=uc_segment_instr;
		j++;
#endif
//		cout << "uc_segment_time: " << uc_segment_time << endl;
//		cout << "uc_segment_instr:" << uc_segment_instr << endl;
				

	    for(i=0;i<SIZE_1;i++) {
			out.write(out_buff[i]);
		}

	} while(1);

}

#endif


