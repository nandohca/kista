/*****************************************************************************

  user_task_constructs.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

 *****************************************************************************/

#ifndef USER_TASK_CONSTRUCTS_HPP
#define USER_TASK_CONSTRUCTS_HPP

#include "systemc.h"

#include "defaults.hpp"

#include "types.hpp"

#include "task_info_t.hpp"

namespace kista {

void set_global_simulation_time_limit(sc_time t);
sc_time &get_global_simulation_time_limit();

// consumption functions for user tasks
void consume(sc_time comp_time);
void consume_WCET(); // Function which takes the WCET (taking it from task info structure)

// yield for for user tasks
void yield();

// wait period functions for user tasks
void wait_period(); // Function which takes the Period from the task information structure)

class task_info_t; // forward declaration

// Macro for consuming time
// By default, it consumes WCET
// If an annotation plugin is used, the CONSUME_T macro is redefined to consume the annotated time
#define CONSUME_T						\
			consume_WCET();
			
// convenience macros

// define wrapper function for periodic task in a single macro
// with implicit consumption of WCET
#define PERIODIC_TASK(wrapper_fun_name, task_fun)	\
     void wrapper_fun_name()						\
     {									\
		while(true) {					\
		  task_fun();					\
		  consume_WCET();				\
		  wait_period();				\
		}								\
     }

// define wrapper function for periodic task in a single macro
// where the time consumed by functionality can be 
// either the WCET or the time estimated by means of annotation
#define GENERIC_PERIODIC_TASK(wrapper_fun_name, task_fun)	\
     void wrapper_fun_name()					\
     {									\
		while(true) {					\
		  task_fun();					\
		  consume_T;					\
		  wait_period();				\
		}								\
     }

#define BEGIN_PERIODIC_TASK(wrapper_fun_name)	\
     void wrapper_fun_name()					\
     {									\
		while(true) {

// Macro for end of periodic task with implicit WCET consumption
#define END_PERIODIC_TASK	\
		  consume_WCET();				\
		  wait_period();				\
		}								\
     }
     
// Macro for end of periodic task with implicit WCET consumption
#define GENERIC_END_PERIODIC_TASK	\
		  CONSUME_T;					\
		  wait_period();				\
		}								\
     }

//
// To directly input aperiodic tasks, whose
// functionality is compose of a sequence of function calls,
// separated by ";", e.g.:
//        TASK( name, f1();f2() )
// By default, it consumes WCET, but it can be overriden
// by an annotation mechanism
//
#define TASK(task_name, ...)	\
     void task_name()					\
     {									\
		while(true) {					\
		  __VA_ARGS__;					\
		  consume_WCET();				\
		  yield();						\
		}								\
     }
     
// BEGIN_TASK (task_name)
// BEGIN_TASK (task_name, inner code)
#define BEGIN_TASK(task_name,...) 		\
    void task_name()					\
    {									\
		while(true) {                  \
			__VA_ARGS__

// BEGIN_TASK (task_name, inner code)
#define BEGIN_TASK_WITH_INIT(task_name,init_sec,...) 		\
    void task_name()					\
    {									\
		init_sec;						\
		while(true) {                  \
			__VA_ARGS__
			
// Macro for ending the task functionality and yield
#define END_TASK						\
		  yield();						\
		}								\
     }

// Macro for ending the task functionality and yield after consuming the WCET associated to the task
#define END_TASK_AFTER_WCET				\
		  consume_WCET();				\
		  yield();						\
		}								\
     }

#define END_TASK_AFTER_WCET_AND(last_commands)	\
		  consume_WCET();				\
		  last_commands					\
		  yield();						\
		}								\
     }     

// Macro for ending the task functionality and yield after consuming a time  of sc_time time
// which is directly annotated to the KisTA code (serves for integrating any annotation
// technique
#define END_TASK_AFTER(time)			\
		  consume(time);				\
		  yield();						\
		}								\
     }

// By default, END_TASK_AFTER_T is an alias of END_TASK_AFTER_T
#define END_TASK_AFTER_T	END_TASK_AFTER_WCET

// These following macros are redefined by the annotation plugins
// and this currently supported by the annotation mechanism, if this is used
//
//		BEGIN_TASK(task_name, task_fun, ...)
// 		END_TASK_AFTER_T
// 		TASK(task_name, task_fun,...)
//

// MACROS FOF GETTING CHANNEL REFERENCES
// Macros to simplify the declaration and obtention of a reference
// to an existing channel.
// When used with the XML front-end, the 
	
#define FIFO_BUFFER_SHORT_REF(CH_PTR,CH_NAME)\
	fifo_buffer<short> *CH_PTR;\
	if(fifo_buffer_short.find(CH_NAME)==fifo_buffer_short.end()) {\
		cout << " Error retrieving reference " << #CH_PTR << " for fifo buffer channel " << CH_NAME << "." << endl;\
		exit(-1);\
	}\
	CH_PTR = fifo_buffer_short[CH_NAME];

#define FIFO_BUFFER_USHORT_REF(CH_PTR,CH_NAME)\
	fifo_buffer<unsigned short> *CH_PTR;\
	if(fifo_buffer_ushort.find(CH_NAME)==fifo_buffer_ushort.end()) {\
		cout << " Error retrieving reference " << #CH_PTR << " for fifo buffer channel " << CH_NAME << "." << endl;\
		exit(-1);\
	}\
	CH_PTR = fifo_buffer_ushort[CH_NAME];

#define FIFO_BUFFER_INT_REF(CH_PTR,CH_NAME)\
	fifo_buffer<int> *CH_PTR;\
	if(fifo_buffer_int.find(CH_NAME)==fifo_buffer_int.end()) {\
		cout << " Error retrieving reference " << #CH_PTR << " for fifo buffer channel " << CH_NAME << "." << endl;\
		exit(-1);\
	}\
	CH_PTR = fifo_buffer_int[CH_NAME];

#define FIFO_BUFFER_UINT_REF(CH_PTR,CH_NAME)\
	fifo_buffer<unsigned int> *CH_PTR;\
	if(fifo_buffer_uint.find(CH_NAME)==fifo_buffer_uint.end()) {\
		cout << " Error retrieving reference " << #CH_PTR << " for fifo buffer channel " << CH_NAME << "." << endl;\
		exit(-1);\
	}\
	CH_PTR = fifo_buffer_uint[CH_NAME];

#define FIFO_BUFFER_FLOAT_REF(CH_PTR,CH_NAME)\
	fifo_buffer<float> *CH_PTR;\
	if(fifo_buffer_float.find(CH_NAME)==fifo_buffer_float.end()) {\
		cout << " Error retrieving reference " << #CH_PTR << " for fifo buffer channel " << CH_NAME << "." << endl;\
		exit(-1);\
	}\
	CH_PTR = fifo_buffer_float[CH_NAME];

#define FIFO_BUFFER_DOUBLE_REF(CH_PTR,CH_NAME)\
	fifo_buffer<double> *CH_PTR;\
	if(fifo_buffer_double.find(CH_NAME)==fifo_buffer_double.end()) {\
		cout << " Error retrieving reference " << #CH_PTR << " for fifo buffer channel " << CH_NAME << "." << endl;\
		exit(-1);\
	}\
	CH_PTR = fifo_buffer_double[CH_NAME];

#define FIFO_BUFFER_CHAR_REF(CH_PTR,CH_NAME)\
	fifo_buffer<char> *CH_PTR;\
	if(fifo_buffer_char.find(CH_NAME)==fifo_buffer_char.end()) {\
		cout << " Error retrieving reference " << #CH_PTR << " for fifo buffer channel " << CH_NAME << "." << endl;\
		exit(-1);\
	}\
	CH_PTR = fifo_buffer_char[CH_NAME];
	  
#define FIFO_BUFFER_VOIDP_REF(CH_PTR,CH_NAME)\
	fifo_buffer<void*> *CH_PTR;\
	if(fifo_buffer_voidp.find(CH_NAME)==fifo_buffer_voidp.end()) {\
		cout << " Error retrieving reference " << #CH_PTR << " for fifo buffer channel " << CH_NAME << "." << endl;\
		exit(-1);\
	}\
	CH_PTR = fifo_buffer_voidp[CH_NAME];

#define FIFO_BUFFER_MSG_REF(CH_PTR,CH_NAME)\
	fifo_buffer<message_t> *CH_PTR;\
	if(fifo_buffer_msg.find(CH_NAME)==fifo_buffer_msg.end()) {\
		cout << " Error retrieving reference " << #CH_PTR << " for fifo buffer channel " << CH_NAME << "." << endl;\
		exit(-1);\
	}\
	CH_PTR = fifo_buffer_msg[CH_NAME];

//
// Get references for IO (communication between environment and system tasks)
//
//  They do basically the same, plus checking that the channel the reference is retrieved is an I/O channel
//
#define CHECK_CHREF_IO(CH_PTR,CH_NAME)\
	if(!CH_PTR->is_IO()) {\
		cout << " Error, Reference to I/O channel retrieved from '" << CH_NAME << "' channel, which is not an I/O communication/synchronization channel." << endl;\
	}

#define IO_FIFO_BUFFER_SHORT_REF(CH_PTR,CH_NAME)\
	FIFO_BUFFER_SHORT_REF(CH_PTR,CH_NAME);\
	CHECK_CHREF_IO(CH_PTR,CH_NAME)

#define IO_FIFO_BUFFER_USHORT_REF(CH_PTR,CH_NAME)\
	FIFO_BUFFER_USHORT_REF(CH_PTR,CH_NAME);\
	CHECK_CHREF_IO(CH_PTR,CH_NAME)
	
#define IO_FIFO_BUFFER_INT_REF(CH_PTR,CH_NAME)\
	FIFO_BUFFER_INT_REF(CH_PTR,CH_NAME);\
	CHECK_CHREF_IO(CH_PTR,CH_NAME)
		
#define IO_FIFO_BUFFER_UINT_REF(CH_PTR,CH_NAME)\
	FIFO_BUFFER_UINT_REF(CH_PTR,CH_NAME);\
	CHECK_CHREF_IO(CH_PTR,CH_NAME)

#define IO_FIFO_BUFFER_FLOAT_REF(CH_PTR,CH_NAME)\
	FIFO_BUFFER_FLOAT_REF(CH_PTR,CH_NAME);\
	CHECK_CHREF_IO(CH_PTR,CH_NAME)

#define IO_FIFO_BUFFER_DOUBLE_REF(CH_PTR,CH_NAME)\
	FIFO_BUFFER_DOUBLE_REF(CH_PTR,CH_NAME);\
	CHECK_CHREF_IO(CH_PTR,CH_NAME)
				
#define IO_FIFO_BUFFER_CHAR_REF(CH_PTR,CH_NAME)\
	FIFO_BUFFER_CHAR_REF(CH_PTR,CH_NAME);\
	CHECK_CHREF_IO(CH_PTR,CH_NAME)
	
#define IO_FIFO_BUFFER_VOIDP_REF(CH_PTR,CH_NAME)\
	FIFO_BUFFER_VOIDP_REF(CH_PTR,CH_NAME);\
	CHECK_CHREF_IO(CH_PTR,CH_NAME)

#define IO_FIFO_BUFFER_MSG_REF(CH_PTR,CH_NAME)\
	FIFO_BUFFER_MSG_REF(CH_PTR,CH_NAME);\
	CHECK_CHREF_IO(CH_PTR,CH_NAME)

} // namespace KisTA

#endif
