/*****************************************************************************

  process_data.cpp
  
  process data functions implementation
  
  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/

#include "process_data.hpp"

extern "C" 
char process_my_type(char char_par, unsigned int a) {
	char my_char;
	
	printf("Executing process_my_type\n");
	
	// foolish computation to produce some load
	if(a>5) {
		my_char = a;
	} else {
		my_char = 'b';		
	}
	
	return my_char;
}

extern "C" 
char process_my_char(char char_par) {
	char my_char;
	
	printf("Executing process_my_char\n");
	
	// foolish computation to produce some load
	
	my_char = char_par-20;
	
	return my_char;
}
