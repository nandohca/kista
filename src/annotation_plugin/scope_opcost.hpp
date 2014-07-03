/*****************************************************************************

  scope_opcost.hpp
  
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

#ifndef _SCOPE_OPCOST_HPP
#define _SCOPE_OPCOST_HPP

// types and functions referred by the SCoPE opcost annotation

typedef struct icache_line_s {
	char num_set;		///< The set where line should be allocated
	char hit;			///< 1=hit, 0=miss
} uc_icache_line_t;

void ic_insert_line(uc_icache_line_t *line);


 // to avoid redefinition warnings...
#undef BEGIN_TASK
#undef END_TASK_AFTER_T
#undef TASK

// redefinition of macros

#define BEGIN_TASK(task_name,...) 		\
     void task_name()					\
     {									\
		sc_time  annotated_time;		\
        processing_element *PE;      \
        PE = task_info_by_phandle[sc_get_current_process_handle()]->get_scheduler()->get_PE();\
		while(true) {					\
			uc_segment_time = 0;		\
			uc_segment_instr = 0;		\
			__VA_ARGS__;

#define BEGIN_TASK_WITH_INIT(task_name,init_sec,...) 		\
     void task_name()					\
     {									\
		sc_time  annotated_time;		\
        processing_element *PE;      \
        PE = task_info_by_phandle[sc_get_current_process_handle()]->get_scheduler()->get_PE();\
        init_sec;                       \
		while(true) {					\
			uc_segment_time = 0;		\
			uc_segment_instr = 0;		\
			__VA_ARGS__;
			
#define CONSUME_T						\
			annotated_time = sc_time((double)(PE->get_CPI()*PE->get_clock_period_ns()*uc_segment_instr),SC_NS);\
			consume(annotated_time);	\
			uc_segment_time = 0;		\
			uc_segment_instr = 0;
//			cout << "Call to CONSUME ANNOTATED TIME!!" << endl;	

#define END_TASK_AFTER_T				\
			annotated_time = sc_time((double)(PE->get_CPI()*PE->get_clock_period_ns()*uc_segment_instr),SC_NS);\
			consume(annotated_time);	\
			yield();					\
		}								\
     }

#define TASK(task_name, ...)	\
     void task_name()					\
     {									\
		sc_time  annotated_time;		\
        processing_element *PE;      \
        PE = task_info_by_phandle[sc_get_current_process_handle()]->get_scheduler()->get_PE();\
		while(true) {					\
			uc_segment_time = 0;		\
			uc_segment_instr = 0;		\
			__VA_ARGS__;				\
			annotated_time = sc_time((double)(PE->get_CPI()*PE->get_clock_period_ns()*uc_segment_instr),SC_NS);	\
			consume(annotated_time);	\
			yield();					\
		}								\
     }

#endif


