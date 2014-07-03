// 
// functions.cpp
//
//
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, April
//

#ifndef F1_CPP
#define F1_CPP

#include "functions.h"

#include "annotation.hpp"

void f1(unsigned short *out, unsigned short *in1, unsigned short *in2, unsigned short mode) {
	unsigned short i = 0;

	if(mode==0) {
		for(i=0; i< SIZE_1; i++) {
			out[i] = in1[i] + in2[i];
			if(out[i]>SAT_LIMIT) {
				out[i] = SAT_LIMIT;
			}
		}
	} else {
		for(i=0; i< SIZE_1; i++) {
			out[i] = in1[i] * in2[i];
			if(out[i]>SAT_LIMIT) {
				out[i] = SAT_LIMIT;
			}			
		}
	}
}

#endif

