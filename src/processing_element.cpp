/*****************************************************************************

  processing_element.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January

 *****************************************************************************/

#ifndef PROCESSING_ELEMENT_CPP
#define PROCESSING_ELEMENT_CPP

#include <systemc.h>

#include "defaults.hpp"

#include "global_elements.hpp"

#include "processing_element.hpp"

#include "draw.hpp"

namespace kista {

// constructor
processing_element::processing_element(sc_module_name name) : hw_resource(name, PROCESSING_ELEMENT) {
//	std::string busy_sig_name= this->name();
//	busy_sig_name += "busy_sig";
	
//	PE_busy_sig = new sc_signal<bool>(busy_sig_name);
	PE_busy_sig = new sc_signal<bool>;
	
	PE_busy_sig->write(false); // init value for the ocupation signal of the processign element
	
	clock_period_ns = 20; // 50MHz by default
	cpi =1;               // 1 cycle per instruction by default
	
	netif_p = NULL;
	
	// default values for time characterization for intra commmunication
	IntraComDelay_bit = SC_ZERO_TIME;
	MaxIntraCommDelay_bit = SC_ZERO_TIME;
	
	// updates the global PE elements table accessed by name
	std::string PE_name;
	PE_name = this->name();
	PEref_by_name[PE_name]=this;
	
	bound_comm_res_p = NULL;
	
	init_energy_power_vars();
}

void processing_element::set_clock_period_ns(unsigned int cloc_period_ns_var) {
	std::string rpt_msg;
	if ( sc_get_status()&(SC_START_OF_SIMULATION|SC_RUNNING|SC_STOPPED|SC_PAUSED|SC_END_OF_SIMULATION) ) {
		rpt_msg = "set_clock_period_ns, called for PE ";
		rpt_msg += name();
		rpt_msg += ", has to be called before simulation start.";
		SC_REPORT_ERROR("KisTA","");
	}
	clock_period_ns = cloc_period_ns_var;
}

void processing_element::set_CPI(unsigned int cpi_var) {	// set Cycles per instruction (default =1)
	std::string rpt_msg;
	if ( sc_get_status()&(SC_START_OF_SIMULATION|SC_RUNNING|SC_STOPPED|SC_PAUSED|SC_END_OF_SIMULATION) ) {
		rpt_msg = "set_CPI, called for PE ";
		rpt_msg += name();
		rpt_msg += ", has to be called before simulation start.";
		SC_REPORT_ERROR("KisTA","");
	}
	cpi = cpi_var;
}

    // implementation of set static instantaneous power consumption in Watt
void processing_element::set_static_power_consumption(double static_power_cons_watt_par)
{
	check_call_before_sim_start("set_static_power_consumption");
	static_power_cons_watt = static_power_cons_watt_par;
}

   // implementation nof set energystatic instantaneous power consumption in Watt
void processing_element::set_energy_consumption_per_instruction(double jules_per_instruction_par)
{
	check_call_before_sim_start("set_energy_consumption_per_instruction");
	jules_per_instruction = jules_per_instruction_par;
}

double &processing_element::get_static_power_consumption() {
	return static_power_cons_watt;
}
	
double &processing_element::get_energy_consumption_per_instruction() {
	return jules_per_instruction;
}


void processing_element::set_has_netif() {
	std::string rpt_msg;
	std::string netif_name;
	
	if ( sc_get_status()&(SC_START_OF_SIMULATION|SC_RUNNING|SC_STOPPED|SC_PAUSED|SC_END_OF_SIMULATION) ) {
		rpt_msg = "set_has_netif, called for PE ";
		rpt_msg += name();
		rpt_msg += ", has to be called before simulation start.";
		SC_REPORT_ERROR("KisTA","");
	}

	netif_name = this->name();

	netif_name += "_netif";
	
	netif_p = new network_interface(sc_module_name(netif_name.c_str()), this);

}


bool processing_element::has_netif() {
	if(netif_p==NULL) return false;
	else return true;
}

network_interface* processing_element::get_netif() {
	std::string rpt_msg;
	if (netif_p==NULL) {
		rpt_msg = "Trying to access network interface of PE ";
		rpt_msg += name();
		rpt_msg += " throw a NULL pointer. Ensure that has_netif for such PE before accessing it.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	}
	return netif_p;
}
	
void processing_element::connect(phy_comm_res_t *bound_comm_res_par) {	//
	std::string msg;
	if((bound_comm_res_p != NULL) &&
		(bound_comm_res_par != bound_comm_res_p)
	  ) {
		msg = "Trying to connect processing element ";
		msg += this->name();
		msg += " to the communication resource ";
		msg += bound_comm_res_par->name();
		msg += ". The processing element was already connected to the communication resource ";
		msg += bound_comm_res_p->name();
		msg += ". KisTA currently supports only the connection of a processing element to a single communication resource.";
		SC_REPORT_ERROR("Kista",msg.c_str());
	}
	else
	{
		bound_comm_res_p = bound_comm_res_par;
		bound_comm_res_p->plug(this); // complete a double link between the communication resource and the processing element
	}
}

phy_comm_res_t* processing_element::get_connected_comm_res() {
	return bound_comm_res_p;
}

sc_time processing_element::getIntraComP2Pdelay(unsigned int message_size ) { 		// current delay, message size in bits
	return message_size*IntraComDelay_bit;
}

sc_time processing_element::getIntraComMaxP2Pdelay(unsigned int message_size) { 		// max delay, message size in bits
	return message_size*MaxIntraCommDelay_bit;
}

		// by default, these times are set to 0s, but the user can configure the delay for each bit
void processing_element::setIntraComP2Pdelay_per_bit(sc_time time_par) { 			// current delay
	IntraComDelay_bit = time_par;
}

void processing_element::setIntraComMaxP2Pdelay_per_bit(sc_time time_par)  { 		// max delay
	MaxIntraCommDelay_bit = time_par;
}

void processing_element::before_end_of_elaboration()  {
	// allows drawing it
	sketch_report.draw(this);
}

void processing_element::init_energy_power_vars() {
	do_energy_and_power_measurement = false;
	total_energy_J = 0.0;
	average_power_watts = 0.0;
	
	av_power_watts = 0.0;
	max_power_watts = 0.0;
	
	power_averaging_time = sc_time(1,SC_SEC);			
}

void processing_element::enable_energy_and_power_measurement() {
	check_call_before_sim_start("enable_energy_and_power_measurement");
	do_energy_and_power_measurement = true;
}

void processing_element::set_power_averaging_time(sc_time avg_time) {
	check_call_before_sim_start("set_power_averaging_time");
	power_averaging_time = avg_time;
}

// to be called at end of elaboration
void processing_element::calculate_static_outputs() {
	peak_power_time = sc_time(cpi*clock_period_ns,SC_NS); // cycles per instruction * clock period (assumes homogeneous instructions)
	peak_power_watts = (jules_per_instruction*1E9)/(cpi*clock_period_ns); // Watts
}

void processing_element::end_of_elaboration() {
	calculate_static_outputs();
}



} // namespace kista

#endif
