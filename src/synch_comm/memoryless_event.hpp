/*****************************************************************************

  memoryless_event.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

 *****************************************************************************/

#ifndef _MEMORYLESS_EVENT_HPP
#define _MEMORYLESS_EVENT_HPP

#include "systemc.h"

#include "defaults.hpp"

namespace kista {

	// note: using typedefs is not sufficient, since it creates ambiguities
class memoryless_event : public sc_event {
public:	
		void wait();
};

class memoryless_event_or_list: public sc_event_or_list  {
public:	
		void wait();
};

class memoryless_event_and_list: public sc_event_and_list  {
public:	
		void wait();
};

// wait for event synchronization for user tasks
void wait(memoryless_event				&e);
void wait(memoryless_event_or_list		&eol);
void wait( memoryless_event_and_list	&eal);
	
} // namespace KisTA

#endif
