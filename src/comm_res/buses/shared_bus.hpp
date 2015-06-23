/*****************************************************************************

  shared_bus.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: University of Cantabria
  Deparment:   Electronic Systems
  Date: 2015 April
  Last Date revision: 
  
   Notes: Shared bus model supporting
   * worst case analysis, with previous check that the assumsions to provide
     a WCCT are fulfilled. Otherwise, an error is raised
   * model for actual, precise commuication time (CT), based on the current state and activity
    in the bus and on the arbitration policy
   * Support of priorities (e.g. to model architectures, such as AXI
     which can give support to mixed-criticality applications
   * statistical CT, (as a faster model, which can be reused in other tools,
     like VIPPE)

 *****************************************************************************/

#ifndef SHARED_BUS_HPP
#define SHARED_BUS_HPP

#include <systemc.h>

#include "defaults.hpp"
#include "types.hpp"

#include "comm_res/phy_link.hpp"
#include "comm_res/phy_comm_res.hpp"

#include <list>

namespace kista {


class shared_bus : public	phy_comm_res_t {
	
public:
	// constructor
	// Generation of the shared_bus object for a separated configuration (useful for XML front-end)
	// This constructor makes a default setting of parameters(bus_width=32, frequency=100000Hz, and
	// bandwidth=3200000)		
	shared_bus(sc_module_name name = sc_gen_unique_name("shared_bus"));
			   
	// Bus attribute setters/getters
	void set_width(unsigned int width_bits);
	unsigned int &get_width(); // return bus width in bits

	void set_frequency(double frequency_Hz);
	double &get_frequency(); // return bus frequency in Hz

	void set_bandwidth(double frequency_Hz);
	double &get_bandwidth(); // return bus bandwidth in bps
	
	// Overload of setters independent on the specific value
	using phy_comm_res_t::set_MaxP2Pdelay; // this is required to enable the use of the non-overloaded methods with the same function name
	
	void set_MaxP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size=1);
	void set_MaxP2Pdelay(phy_link_t &link, unsigned int msg_size=1);

	// redefinitions of inherited methods	
	sc_time &get_MaxP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size=1);
	sc_time &get_MaxP2Pdelay(phy_link_t &link, unsigned int msg_size=1);
	
	// Overload of setters independent on the specific value
	using phy_comm_res_t::set_CurrentP2Pdelay; // this is required to enable the use of the non-overloaded methods with the same function name
	
	void set_CurrentP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size=1);
	void set_CurrentP2Pdelay(phy_link_t &link, unsigned int msg_size=1);

	// redefinitions of inherited methods	
	sc_time &get_CurrentP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size=1);
	sc_time &get_CurrentP2Pdelay(phy_link_t &link, unsigned int msg_size=1);
	

	void report_bus_configuration();
	
	// CONTROL OF THE ACCURACY OF THE MODEL
	// 	Currently supported levels:
	//			0: In principle, only one bound level provided only
	//
	//void 			  set_accuracy_level(accuracy_level_t level);
	//accuracy_level_t& get_accuracy_level();


private:

	unsigned int bus_width_bits; // in bits
	
	double bus_bandwidth_bps;
	
	double bus_frequency_Hz;

	void set_default_params();
			
	void before_end_of_elaboration();	
	//void end_of_elaboration();

};

} // namespace kista

#endif
