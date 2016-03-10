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

#include "scheduler.hpp"

#include "processing_element.hpp"

#include "draw.hpp"

namespace kista {

// constructor
processing_element::processing_element(sc_module_name name) : hw_resource(name, PROCESSING_ELEMENT) {
	
	mapped_scheduler = NULL;
	
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
	total_energy_J = 0.0;
	total_average_power_watts = 0.0;
	
	av_power_watts = 0.0;
	max_av_power_watts = 0.0;
	
	power_averaging_time = sc_time(1,SC_SEC);			
}


void processing_element::set_power_averaging_time(sc_time avg_time) {
	check_call_before_sim_start("set_power_averaging_time");
	power_averaging_time = avg_time;
}

//
// to be called at end of elaboration
// calculate the time per instruction (assumes an average same time per instruction)
//                                     for an average cpi
// - time per instruction
// - power for the instruction time (energy of the instruction / time of the instruction
//   which  is modelled as constant in KisTA
void processing_element::calculate_instruction_values() {
	instruction_time = sc_time(cpi*clock_period_ns,SC_NS); // cycles per instruction * clock period (assumes homogeneous instructions)
	peak_dyn_power_watts = (jules_per_instruction*1E9)/(cpi*clock_period_ns); // Watts
}

void processing_element::end_of_elaboration() {
	calculate_instruction_values();
}


void processing_element::map_scheduler(scheduler *sched) {
	check_call_before_sim_start("map_scheduler");
	mapped_scheduler = sched;
}

scheduler *processing_element::get_scheduler() {
	string err_str;
	check_call_before_sim_start("check_call_after_sim_start");
/*	if(mapped_scheduler==NULL) {
		err_str = "In PE \"";
		err_str += name();
		err_str = "\": calling get_scheduler() return NULL. ";
		SC_REPORT_WARNING("KisTA",err_str.c_str());
	}
	*/
	return mapped_scheduler;
}

double processing_element::get_utilization() {
	check_call_after_sim_start("get_utilization");
	// assuming the 1 scheduler -> 1 PE mapping
	// takes the calculation done from scheduler side 
	// through the addition of tasks and scheduler occupations
	// and the divission by the current time (or the last simulated time if
	// the simulation is ended)
	if(mapped_scheduler==NULL) {
		// If neither an scheduler nor any task assigned to this
		// PE, then it is assumed a 0 utiliation
		return 0.0;
	} else {
		mapped_scheduler->get_platform_utilization();
	}
}

const double& processing_element::get_consumed_dynamic_energy_J() {
	check_call_after_sim_start("get_utilization");
	
	// updates total_energy_J, and internal variable for the class
	
	// assuming the 1 scheduler -> 1 PE mapping
	// takes the calculation done from scheduler side 
	// through the addition of tasks and scheduler occupations
	// and the divission by the current time (or the last simulated time if
	// the simulation is ended)
	if(mapped_scheduler==NULL) {
		// If neither an scheduler nor any task assigned to this
		// PE, then it is assumed a 0 consumption
		return total_energy_J; // total_energy_J shall have the initialized value =0.0 here
	} else {
		// energy currently consumed by the scheduler
		total_energy_J = mapped_scheduler->get_consumed_energy_J();
		// adds energy currently consumed by each of the assigned tasks

		for( taskset_by_name_t::iterator it = mapped_scheduler->tasks_assigned->begin(); it != mapped_scheduler->tasks_assigned->end(); ++it ) {
			total_energy_J += it->second->get_consumed_energy_J();
		}
	}
	
	// return the value of that internal variable
	return total_energy_J; 
}


/* 
 * Remind this type of struture for power report, either current or lat time average
 * 
		if(sc_get_status()&SC_STOPPED) {	
			// IEEE Std 1666-2011 does not state waht sc_time_stamps returns
			// after SystemC simulation as expired. Therefore, we detect
			// if simulation has finished, so them we use the last time stamp
			// at the end of the simulation.
			// This way, this report function can be safely used at the
			// end of the SystemC simulation
			//cout << "Last simulation time: " << sc_time_stamp() << endl;
			return all_tasks_utilization_time / last_simulation_time;
		} else {	
			if(sc_is_running())  {				
				if(sc_time_stamp()>SC_ZERO_TIME) {					
					// this way, this method can be call at any time during the simulation
					return all_tasks_utilization_time / sc_time_stamp();
				} else {					
					SC_REPORT_WARNING("KisTA", "No time advance yet. 0.0 will be reported for all tasks utilization.");
				}
			} else {				
				SC_REPORT_WARNING("KisTA", "All tasks utilization retrieved before simulation start");
			}	
			return 0.0;			
		}	
		
		*/

void processing_element::power_accounter_proc() {
	
	while(true) {
			wait(power_averaging_time);
	}
}
} // namespace kista

#endif
