// 
// sc_f3.cpp
//
//
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, April
//

#ifndef _SC_F3_CPP
#define _SC_F3_CPP

#include "systemc.h"

#include "sc_f3.h"
#include "functions.h"

#include "basic_scope.h"
#include "wcet_est.h"

#ifdef _RANDOM_VECTORS
 // structure where data wit the estimation instructions are dumpled 
extern unsigned long long results[N_RANDOM_TEST_VECTORS];
#endif

void sc_f3::sc_f3_proc()
{
	unsigned int i;
	unsigned short int in_buff[SIZE_1];
	unsigned short int out_buff[SIZE_1];

#ifdef _RANDOM_VECTORS
	unsigned int j = 0;
#endif

	do {

	    for(i=0;i<SIZE_1;i++) {
			in.read(in_buff[i]);	
		}
		
		uc_segment_time = 0;
		uc_segment_instr = 0;
		
		f3(out_buff,in_buff);

#ifdef _RANDOM_VECTORS
		results[j]=uc_segment_instr;
		j++;
#endif

	    for(i=0;i<SIZE_1;i++) {
			out.write(out_buff[i]);
		}
		
	} while(1);

}

#endif

