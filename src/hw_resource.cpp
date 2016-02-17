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
#ifndef HW_RESOURCE_CPP
#define HW_RESOURCE_CPP

#include <systemc.h>

#include "defaults.hpp"

#include "hw_resource.hpp"

namespace kista {
	
hw_resource::hw_resource(sc_module_name name, resource_t type) : sc_module(name) {
	address = this;
	resource_type = type;
}

resource_t hw_resource::get_type() {
	return resource_type;
}
	
phy_address hw_resource::get_address() {
	return this;
	//return (phy_address)this; // actually not required conversion, 
	                            // (see types.hpp)
}

bool hw_resource::operator==(hw_resource& rha) {
	if ( (this->address == rha.address) && (this->resource_type == rha.resource_type) ) return true;
	else return false;
}


} // namespace kista

#endif
