// 
// basic_scope.h
// 
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, April
//
//
// Basic declarations to use scope op-cost annotations
//

#ifndef _BASIC_SCOPE_H
#define _BASIC_SCOPE_H

// Variables for the SCoPE opcost annotation
extern unsigned long long uc_segment_time;
extern unsigned long long uc_segment_instr;
typedef struct icache_line_s {
	char num_set;		///< The set where line should be allocated
	char hit;			///< 1=hit, 0=miss
} uc_icache_line_t;
void ic_insert_line(uc_icache_line_t *line);



#endif
