// 
// functions.cpp
//
//
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, April
//

#ifndef F2_CPP
#define F2_CPP
//#include "stdio.h"
#include "functions.h"

#include "annotation.hpp"

void f2(unsigned short *out, unsigned short *in1) {
	unsigned int acum;
	unsigned int i;
	acum=0;
	for(i=0; i< SIZE_1; i++) {
		acum = acum + in1[i];
	}
//printf("acum: %d\n", acum );
	*out = acum / SIZE_1;
	if(*out>SAT_LIMIT) {
		*out = SAT_LIMIT;
	}
}

#endif

