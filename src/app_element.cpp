/*****************************************************************************

  hw_resource.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: At home
  Deparment:   
  Date: 2015 August
   
  Class to support an abstract hw resource implementation
  
 *****************************************************************************/
#ifndef APP_ELEMENT_CPP
#define APP_ELEMENT_CPP

#include <systemc.h>

#include "defaults.hpp"

#include "app_element.hpp"

namespace kista {
	
app_element::app_element(sc_module_name name, app_element_e type) : sc_module(name) {
	address = this;
	type = type;
}

app_element_e app_element::get_app_element_type() {
	return type;
}

std::string app_element::name() {
	return sc_module::name();  // name() sc_module method
}
	
logic_address app_element::get_address() {
	return (logic_address)this;
	//return (phy_address)this; // actually not required conversion, 
	                            // (see types.hpp)
}

bool app_element::operator==(app_element& rha) {
	if ( (this->address == rha.address) && (this->type == rha.type) ) return true;
	else return false;
}


void app_element::set_scheduler(scheduler *scheduler_p)
{
	sched_p = scheduler_p;
}

scheduler* app_element::get_scheduler() {
	return sched_p;
}


} // namespace kista

#endif
