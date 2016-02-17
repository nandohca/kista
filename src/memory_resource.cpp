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
memory_resource::memory_resource(sc_module_name name) : hw_resource(name, MEMORY_RESOURCE)
{
	std::string mem_name;
	mem_name = this->name();
	MEMref_by_name[mem_name]=this;
	bound_comm_res_p = NULL;
};

void memory_resource::connect(phy_comm_res_t *bound_comm_res_par)
{
	if((bound_comm_res_p != NULL) &&
		(bound_comm_res_par != bound_comm_res_p)
	  ) {
		msg = "Trying to connect memory resource ";
		msg += this->name();
		msg += " to the communication resource ";
		msg += bound_comm_res_par->name();
		msg += ". The memory resource was already connected to the communication resource ";
		msg += bound_comm_res_p->name();
		msg += ". KisTA currently supports only the connection of a memory resource to a single communication resource.";
		SC_REPORT_ERROR("Kista",msg.c_str());
	} else
	{
		bound_comm_res_p = bound_comm_res_par;
		bound_comm_res_p->plug(this); // complete a double link between the communication resource and the processing element
	}
}

	// if there is more than one connected, the result is not specified
	// for the moment, and and error will be raised
	// (however this is required for dual port ram directly linking a 
	// a PE for instance)
phy_comm_res_t* memory_resource::get_connected_comm_res()
{
	return bound_comm_res_p;
}

	
// This is to fix the delay (latency) , modelled as independent from message size
void memory_resource::set_access_time(sc_time access_time) // current delay, message size in bits
{
	latency = access_time;
}
 	
sc_time	memory_resource::get_access_time()
{
	return latency;
}
	
void memory_resource::set_bandwidth(double bps)
{
	bandwidth_bps = bps;
}

double memory_resource::get_bandwidth_bps()
{
	return bandwidth_bps;
}

void memory_resource::before_end_of_elaboration()  {
	// allows drawing it
	sketch_report.draw(this);
}

} // namespace kista

#endif
