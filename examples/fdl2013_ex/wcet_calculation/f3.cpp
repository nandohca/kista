// 
// functions.cpp
//
//
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, April
//

#ifndef F3_CPP
#define F3_CPP

#include "functions.h"

void f3(unsigned short *out, unsigned short *in) {
/*
	for(i=0; i< SIZE_1; i++) {
		if(in1[i]>in2[i])  {	
			out[i] = in1[i] - in2[i];
		} else {
			out[i] = in2[i] - in1[i];
		}
	}
*/
	// buble short
	unsigned short int i,j,aux;
	aux = 0;
	for (i=0 ; i<SIZE_1 ; i++) {
		out[i] = in[i];
	}
	for (i=1 ; i<SIZE_1 ; i++) {
		for (j=0 ; j<SIZE_1-i ; j++) {
			if (out[j] > out[j+1]) {
				aux = out[j];
				out[j] = out[j+1];
				out[j+1] = aux;
			}
		}
	}
}



#endif

