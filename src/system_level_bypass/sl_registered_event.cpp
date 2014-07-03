/*****************************************************************************

  sl_registered_event.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

 *****************************************************************************/

#ifndef _SYSTEM_LEVEL_REGISTERED_EVENT_CPP
#define _SYSTEM_LEVEL_REGISTERED_EVENT_CPP

#include "sl_registered_event.hpp"
	  
registered_event::registered_event(bool wait_release_unset_flag_par) {
	flag = false;
	wait_release_unset_flag = wait_release_unset_flag_par;
}
	
void registered_event::set() {
	flag = true;
	notify(SC_ZERO_TIME);
}

void registered_event::unset() {
	flag = false;
}

void registered_event::wait() {
	if(!flag) sc_core::wait(static_cast<sc_event&>(*this)); // calls to the functions on wait(sc_event)
	if(wait_release_unset_flag) unset();
}
	
void wait(registered_event &re) {
	re.wait(); // Calls to the wait method of the memoryless_event object (wich is the interited one from the sc_event)				
}

#endif
