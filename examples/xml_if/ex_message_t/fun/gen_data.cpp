/*****************************************************************************

  gen_data.cpp
  
  generation data functions implementation
  
  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/

#include "gen_data.hpp"
#include <stdlib.h>
 
extern "C"
char gen_rand_char() {
	char my_char;
	int num;
	printf("Executing gen_rand_char\n");
	
	my_char = 'a';
	num=rand()%26;
		
	my_char = (char)((unsigned int)my_char+num);
	
	return my_char;
}

extern "C" 
unsigned int gen_rand_uint() {
	unsigned int my_uint;
	int num;
	
	printf("Executing gen_rand_uint\n");
	
	my_uint = 0;
	
	num=rand()%100;
	
	my_uint = my_uint + (unsigned int)num;
	
	return my_uint;
}

