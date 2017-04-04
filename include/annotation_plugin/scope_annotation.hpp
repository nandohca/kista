/*****************************************************************************

  scope_annotation.hpp
  
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

#ifndef _SCOPE_ANNOTATION_HPP
#define _SCOPE_ANNOTATION_HPP

// Variables for the SCoPE opcost annotation
extern unsigned long long uc_segment_time;

extern unsigned long long uc_segment_instr;

void ic_insert_line(uc_icache_line_t *line);

#endif


