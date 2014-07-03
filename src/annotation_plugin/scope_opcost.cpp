/*****************************************************************************

  scope_opcost.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

  Note: 
    The following code is used to enable the use of scope-g++ and opcost tools,
    belonging to the SCoPE toolkit, within KisTA
     
    Functional code has to:
     
    * include annotation.hpp header
     
	* be compiled with : scope-g++ --scope-method=op-cost --scope-noicache

 *****************************************************************************/

#ifndef _SCOPE_OPCOST_CPP
#define _SCOPE_OPCOST_CPP

#include "scope_opcost.hpp"

// Variables for the SCoPE opcost annotation
unsigned long long uc_segment_time;

unsigned long long uc_segment_instr;

/*
 * typedef struct icache_line_s {
	char num_set;		///< The set where line should be allocated
	char hit;			///< 1=hit, 0=miss
} uc_icache_line_t;
*/

void ic_insert_line(uc_icache_line_t *line) {}

#endif


