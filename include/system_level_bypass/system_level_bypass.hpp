/*****************************************************************************

  system_level_bypass.hpp
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

   Note:
   Here KisTA provides a "bypass meaning" for some KisTA user tasks,
   synchronization and communication constructs for system-level (SystemC)

 *****************************************************************************/

//
//  KisTA:							SystemC
//  -----------     					--------
//  consume(sc_time)					no delay or wait(sc_time) (depend on _CONSUME_EQ_WAIT_IN_SYSTEM_LEVEL)
//
//  memoryless_event					sc_event
//  memoryless_event_or_list			sc_event_or_list
//  memoryless_event_and_list			sc_event_and_list
//  and by extension...
//	wait(memoryless_event)				wait(sc_event)
//	wait(memoryless_event_or_list)		wait(sc_event_or_list)
//	wait(memoryless_event_and_list)		wait(sc_event_and_list)
//  
//  semaphore							sc_semaphore
//  fifo_buffer (message queue)			sc_fifo				
//
// AND elements for which a NEW SYSTEM-LEVEL (SystemC) implementation is provided
//  registered_event/flag				registered_event 
//  wait(registered_event)				wait(registered_event)
//

#ifndef _SYSTEM_LEVEL_BYPASS_HPP
#define _SYSTEM_LEVEL_BYPASS_HPP

#include "systemc.h"

void consume(sc_time t);

// memoryless event bypass
typedef sc_event memoryless_event;
typedef sc_event_or_list memoryless_event_or_list;
typedef sc_event_and_list memoryless_event_and_list;

// registered event bypass
#include "system_level_bypass/sl_registered_event.hpp"

// semaphore event bypass
typedef sc_semaphore semaphore;

// it seems that typedef on templates is not allowed.
template<class T>
class fifo_buffer: public sc_fifo<T> {
	public:
	fifo_buffer( int _size) : sc_fifo<T>(_size) {}

	fifo_buffer( const char* _name, int _size) : sc_fifo<T>(_name, _size) {}	
};

// This is a fast work-around
//#define fifo_buffer sc_fifo

#endif
