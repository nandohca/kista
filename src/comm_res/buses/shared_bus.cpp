/*****************************************************************************

  shared_bus.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: University of Cantabria
  Deparment:   Electronic Systems
  Date: 2015 April
  Last Date revision: 
  
   Notes: Implementation of the shared bus model supporting
   * worst case analysis, with previous check that the assumsions to provide
     a WCCT are fulfilled. Otherwise, an error is raised
   * model for actual, precise commuication time (CT), based on the current state and activity
    in the bus and on the arbitration policy
   * Support of priorities (e.g. to model architectures, such as AXI
     which can give support to mixed-criticality applications
   * statistical CT, (as a faster model, which can be reused in other tools,
     like VIPPE)

 *****************************************************************************/

#ifndef SHARED_BUS_CPP
#define SHARED_BUS_CPP

#include <map>
//#include <string>

#include <systemc.h>

#include "defaults.hpp"

#include "global_elements.hpp"

#include "shared_bus.hpp"

#include "processing_element.hpp"
#include "draw.hpp"

#include "utils.hpp"

namespace kista {
	
#define MAX_SHARED_BUS_CONFIGURATION_ERROR 0.001
// 0.1%

void shared_bus::set_default_params() {
	// initialization values according to the default values
	bus_width_bits = 32; // in bits
	bus_bandwidth_bps = 100000;
	bus_frequency_Hz = 3200000;
}

shared_bus::shared_bus(sc_module_name name) {
	set_default_params();
}

// Bus attribute setters/getters
void shared_bus::set_width(unsigned int width_bits){
	check_call_before_sim_start("set_width");
	bus_width_bits = width_bits;
}

unsigned int& shared_bus::get_width() { // return bus width in bits
	return bus_width_bits;
}

void shared_bus::set_frequency(double frequency_Hz){
	check_call_before_sim_start("set_frequency");
	bus_frequency_Hz = frequency_Hz;
}

double& shared_bus::get_frequency() { // return bus frequency in Hz
	return bus_frequency_Hz;
}

void shared_bus::set_bandwidth(double bandwith_bps) {
	check_call_before_sim_start("set_bandwidth");
	bus_bandwidth_bps = bandwith_bps;
}

double& shared_bus::get_bandwidth() { // return bus bandwidth in bps
	return bus_bandwidth_bps;
}

void shared_bus::before_end_of_elaboration() {
	double calc_bandwidth_bps;
	double error; // percentual error, deviation over the lower bandwidth value,
	              // either settled or calculated, to make consider the more restrictive case
	std::string msg;
	calc_bandwidth_bps = bus_width_bits*bus_frequency_Hz;
	// calculates the biggest error possible (as an absolute value, always positive)
	if(bus_width_bits==0.0) {
		msg="Setting shared bus \"";
		msg += this->name();
		msg += "\". Bus width cannot be 0 bits.";
		SC_REPORT_ERROR("KisTA",msg.c_str());
	}

	if(bus_bandwidth_bps==0.0) {
		msg="Setting shared bus \"";
		msg += this->name();
		msg += "\". Bus bandwidth cannot be 0 bps.";
		SC_REPORT_ERROR("KisTA",msg.c_str());
	}

	if(bus_frequency_Hz==0.0) {
		msg="Setting shared bus \"";
		msg += this->name();
		msg += "\". Bus frequency cannot be 0.0 Hz.";
		SC_REPORT_ERROR("KisTA",msg.c_str());
	}
	
	if(calc_bandwidth_bps<bus_bandwidth_bps) {
		error = (bus_bandwidth_bps-calc_bandwidth_bps)/calc_bandwidth_bps;
	} else {
		error = (calc_bandwidth_bps-bus_bandwidth_bps)/bus_bandwidth_bps;
	}
	if(error>MAX_SHARED_BUS_CONFIGURATION_ERROR) {
		msg="Setting shared bus \"";
		msg += this->name();
		msg += "\". Incoherent setting of attributes. The product of the current bus bandwidth (";
		msg += bus_bandwidth_bps;
		msg += " bits), per the bus frequency (";
		msg += bus_frequency_Hz;
		msg += " Hz) yields a deviation from the configured bus bandwith (";
		msg += error;
		msg += " bps) bigger than the maximum allowed deviation configured (";
		msg += MAX_SHARED_BUS_CONFIGURATION_ERROR;
		msg += " )";
		SC_REPORT_ERROR("KisTA",msg.c_str());
	}
}


void shared_bus::report_bus_configuration() {
	cout << "Shared Bus " << name() << " configuration completed: " << endl;
	cout << "\t\t bus width (bits) = " << bus_width_bits << endl;
	cout << "\t\t bus bandwidth (bps) = " << bus_bandwidth_bps << endl;
	cout << "\t\t bus frequency (frequency) = " << bus_frequency_Hz << endl;
	
}

// -------------------------------------------------------------
//   Overloading Implementation of virtual assessment methods
// -------------------------------------------------------------

void shared_bus::set_MaxP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size)
{
	SC_REPORT_ERROR("KisTA","method of shared bus not implemented");
}

void shared_bus::set_MaxP2Pdelay(phy_link_t &link, unsigned int msg_size)
{
	SC_REPORT_ERROR("KisTA","method of shared bus not implemented");
}

sc_time& shared_bus::get_MaxP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size) {
	SC_REPORT_ERROR("KisTA","method of shared bus not implemented");	
}

sc_time& shared_bus::get_MaxP2Pdelay(phy_link_t &link, unsigned int msg_size) {
	SC_REPORT_ERROR("KisTA","method of shared bus not implemented");
}
		
void shared_bus::set_CurrentP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size) {
	SC_REPORT_ERROR("KisTA","method of shared bus not implemented");
}
	
void shared_bus::set_CurrentP2Pdelay(phy_link_t &link, unsigned int msg_size)
{
	SC_REPORT_ERROR("KisTA","method of shared bus not implemented");
}

sc_time& shared_bus::get_CurrentP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size)
{
	SC_REPORT_ERROR("KisTA","method of shared bus not implemented");
}

sc_time& shared_bus::get_CurrentP2Pdelay(phy_link_t &link, unsigned int msg_size) {
	SC_REPORT_ERROR("KisTA","method of shared bus not implemented");
}
	

} // namespace kista

#endif
