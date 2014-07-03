/*****************************************************************************

  gen_data.cpp
  
  generation data functions implementation
  
  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/

#include "gen_data.hpp"
 
extern "C"
char gen_rand_char() {
	char my_char;
	
	// foolish computation to produce some load
	unsigned int i;
	unsigned int b,c;

	printf("Executing gen_rand_char\n");
	
	my_char = 'a';
	b=2;
	c=1;
	for (i=0;i<10000;i++) {
		b = b*i;
		b = b%10;	
	}
	
	c = (char)((unsigned int)my_char+b);
	
	return c;
}

extern "C" 
unsigned int gen_rand_uint() {
	unsigned int my_uint;
	
	// foolish computation to produce some load
	unsigned int i;
	unsigned int b;
	
	printf("Executing gen_rand_uint\n");
	
	my_uint = 0;
	b=7;
	for (i=0;i<100000;i++) {
		b = b+i;
		b = b%10;	
	}
	
	my_uint = my_uint + b;
	
	return my_uint;
}

