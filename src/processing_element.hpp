/*****************************************************************************

  processing_element.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January

 *****************************************************************************/

#ifndef PROCESSING_ELEMENT_HPP
#define PROCESSING_ELEMENT_HPP

#include <systemc.h>

#include "defaults.hpp"

#include "comm_res/phy_comm_res.hpp"
#include "hw_resource.hpp"
#include "network_interface.hpp"

namespace kista {

class processing_element :  public hw_resource {
	// advanced declarations:
//	class phy_comm_res_t; 

//  typedef comm_res<phy_address>	phy_comm_res_t;

public:
	// constructor
	processing_element(sc_module_name name = sc_gen_unique_name("PE"));
	
	// set clock period (ns)
	void set_clock_period_ns(unsigned int clock_period_ns_var);            // set PE frequency default 20ns (50MHz)
	inline unsigned int &get_clock_period_ns() {	return clock_period_ns; }    // get PE frequency
	
	// set CPI
	void set_CPI(unsigned int cpi_var);	                 // set Cycles per instruction (default =1)
	inline unsigned int &get_CPI() { return cpi; }		 // get cycles per instruction
	
	void connect(phy_comm_res_t *bound_comm_res_par);
	phy_comm_res_t* get_connected_comm_res();

	// time characterizations for communication within the processing element
	sc_time getIntraComP2Pdelay(unsigned int message_size = 1 ); 		// current delay, message size in bits
	sc_time getIntraComMaxP2Pdelay(unsigned int message_size = 1); 		// max delay, message size in bits

		// by default, these times are set to 0s, but the user can configure the delay for each bit
	void setIntraComP2Pdelay_per_bit(sc_time time_par); 			// current delay
	void setIntraComMaxP2Pdelay_per_bit(sc_time time_par); 		// max delay

	// network interface
	// ------------------------------------------------------------------------------------------
	//    (so far it has implications on the modeling of the writing delay
	//     for system-level channels, e.g., see semantics of write access of fifo_buffer,
	//     in case there is net_if, it is assumed that the PE has a net_if with a TX buffer
	//      of a size equivalent to the output token, and so, all the tokens at the output
	//      )
	void set_has_netif();
	bool has_netif();
	network_interface* get_netif();
	// ------------------------------------------------------------------------------------------
	
private:
    // getter for the total amount of tasks
	sc_signal<bool>  *PE_busy_sig;
	sc_process_handle scheduler; 
	sc_process_handle current_task;
	unsigned int clock_period_ns;
	unsigned int cpi;
		
	phy_comm_res_t *bound_comm_res_p; // bound communication resource
	
	// time characterization
	sc_time IntraComDelay_bit;
	sc_time MaxIntraCommDelay_bit;
	
	void before_end_of_elaboration();
	
	// network interface variables
	network_interface *netif_p;
	
};


} // namespace kista

#endif
