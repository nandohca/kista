/*****************************************************************************

  memory_resource.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2015 July

 *****************************************************************************/

#ifndef MEMORY_RESOURCE_HPP
#define MEMORY_RESOURCE_HPP

#include <systemc.h>

#include "defaults.hpp"

namespace kista {

class memory_resource : public sc_module {

public:
	// constructor
	memory_resource(sc_module_name name = sc_gen_unique_name("mem"));
	   // by default fixed access time equal to 0
		
	// methods to connect the memory resource to a communication resource
	void connect(phy_comm_res_t *bound_comm_res_par);
	// method to obtain a connected communication resource 
	// if there is more than one connected, the result is not specified
	// for the moment, and and error will be raised
	// (however this is required for dual port ram directly linking a 
	// a PE for instance)
	phy_comm_res_t* get_connected_comm_res();

	// time characterizations for communication within the processing element
	sc_time get_access_time(unsigned int message_size = 1 ); 		// current delay, message size in bits
	
	// This is to fix the delay (latency) , modelled as independent from message size
	void    set_access_time(sc_time access_time = SC_ZERO_TIME ); 	// current delay, message size in bits
	sc_time	get_access_time();
	
	void set_bandwidth(double bps);
	double get_bandwidth();
		
private:
	unsigned int latency;
	unsigned int bandwidth;
	
	phy_comm_res_t *bound_comm_res_p; // bound communication resource
		
};


} // namespace kista

#endif
