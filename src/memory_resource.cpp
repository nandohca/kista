/*****************************************************************************

  memory_resource.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2015 July

 *****************************************************************************/

#ifndef MEMORY_RESOURCE_CPP
#define MEMORY_RESOURCE_CPP

#include <systemc.h>

#include "defaults.hpp"

#include "global_elements.hpp"

#include "memory_resource.hpp"

#include "draw.hpp"

namespace kista {

// constructor
memory_resource::memory_resource(sc_module_name name) : sc_module(name)
{
	std::string PE_name;
	mem_name = this->name();
	memref_by_name[mem_name]=this;
};

void memory_resource::connect(phy_comm_res_t *bound_comm_res_par)
{

}

	// if there is more than one connected, the result is not specified
	// for the moment, and and error will be raised
	// (however this is required for dual port ram directly linking a 
	// a PE for instance)
phy_comm_res_t* memory_resource::get_connected_comm_res()
{

}


// time characterizations for communication within the processing element
sc_time memory_resource::get_access_time(unsigned int message_size = 1 ) 		// current delay, message size in bits
{

}
	
// This is to fix the delay (latency) , modelled as independent from message size
void    memory_resource::set_access_time(sc_time access_time = SC_ZERO_TIME ) // current delay, message size in bits
{
	
}
 	
sc_time	memory_resource::get_access_time()
{
	
}
	
void memory_resource::set_bandwidth(double bps)
{

}

double memory_resource::get_bandwidth()
{

}

} // namespace kista

#endif
