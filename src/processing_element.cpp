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
	
	// active accounter process
	if(energy_and_power_measurement_enabled) {
		// if the global flag enabling energy and power measurement is activated
		// (actually only controls power enable/disabling currently)
		SC_HAS_PROCESS(processing_element);
		SC_THREAD(power_accounter_proc);
	}
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
	// input (Defaults)
	//static_power_cons_watt = 0.0;
	//jules_per_instruction = 0.0;
	static_power_cons_watt = 1.0;
	jules_per_instruction = 0.00000005; // 50nJ
	power_averaging_time = sc_time(1,SC_SEC);			
	
	// outputs
	// autocalculated at elaboration time
	//double peak_dyn_power_watts; 
	//sc_time instruction_time;
		
	// calculated at sim time and readable at sim time and at end of sim
	static_energy_J = 0.0;
	dynamic_energy_J = 0.0;
	total_energy_J = 0.0;
	
	total_average_power_watts = 0.0;
	av_power_watts = 0.0;
	max_av_power_watts = 0.0;

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
	check_call_after_sim_start("get_consumed_dynamic_energy_J");
	
	// updates total_energy_J, and internal variable for the class
	
	// assuming the 1 scheduler -> 1 PE mapping
	// takes the calculation done from scheduler side 
	// through the addition of tasks and scheduler occupations
	// and the divission by the current time (or the last simulated time if
	// the simulation is ended)
	if(mapped_scheduler==NULL) {
		// If neither an scheduler nor any task assigned to this
		// PE, then it is assumed a 0 consumption
		return dynamic_energy_J; // dyamic_energy_J should be 0.0 here
	} else {
		// energy currently consumed by the scheduler
		dynamic_energy_J = mapped_scheduler->get_consumed_energy_J();
		// adds energy currently consumed by each of the assigned tasks
		for( taskset_by_name_t::iterator it = mapped_scheduler->tasks_assigned->begin(); it != mapped_scheduler->tasks_assigned->end(); ++it ) {
			dynamic_energy_J += it->second->get_consumed_energy_J();
		}
	}
	
	// return the value of that internal variable
	return dynamic_energy_J; 
}

const double& processing_element::get_consumed_static_energy_J() {
	check_call_after_sim_start("get_consumed_static_energy_J");
	if(sc_get_status()&SC_STOPPED) {	
		// IEEE Std 1666-2011 does not state what sc_time_stamps returns
		// after SystemC simulation as expired. Therefore, we detect
		// if simulation has finished, so them we use the last time stamp
		// at the end of the simulation.
		// This way, this report function can be safely used at the
		// end of the SystemC simulation
		//cout << "Last simulation time: " << sc_time_stamp() << endl;
		//static_energy_J = get_static_power_consumption() * last_simulation_time;
		static_energy_J = get_static_power_consumption() * mapped_scheduler->last_simulation_time.to_seconds();
	} else {	
		if(sc_is_running())  {				
			if(sc_time_stamp()>SC_ZERO_TIME) {					
				// this way, this method can be call at any time during the simulation
				static_energy_J = get_static_power_consumption() * sc_time_stamp().to_seconds();
			} else {
				static_energy_J = 0.0;			
				SC_REPORT_WARNING("KisTA", "No time advance yet. get_consumed_static_energy_J wil return 0.0 J.");
			}
		} else {
			static_energy_J = 0.0;		
			SC_REPORT_WARNING("KisTA", "get_consumed_static_energy_J  called before simulation start. 0.0 J returned.");
		}		
	}
	return static_energy_J;
}

const double& processing_element::get_consumed_energy_J() {
	total_energy_J = this->get_consumed_dynamic_energy_J() + this->get_consumed_static_energy_J();
	return total_energy_J;
}

void processing_element::power_accounter_proc() {
	double previous_total_consumed_energy_J;
	
	previous_total_consumed_energy_J = get_consumed_energy_J();
	
	while(true) {
		wait(power_averaging_time);
		// calculates power for the configured power averaging time
		av_power_watts = (this->get_consumed_energy_J() - previous_total_consumed_energy_J) / power_averaging_time.to_seconds();
		
		// updates the maximum power (for the power averaging time)
		if(av_power_watts>max_av_power_watts) {
			max_av_power_watts = av_power_watts;
		}
	}
}

		// Maximum value of power, statically determined by CPI, PE frequency and static+instruction energy
		// it is als the power per instruction
const double& processing_element::get_peak_dyn_power_W()  {
	check_call_after_sim_start("get_peak_dyn_power_W");
	return peak_dyn_power_watts;
	
}
	
		
	   // Average power as a result of total energy/simulated time
	   //   (can be called during simulation and at the end of the simulation)
const double& processing_element::get_total_average_power_W() {
	check_call_after_sim_start("get_total_average_power_W");	
	if(sc_get_status()&SC_STOPPED) {	
		// IEEE Std 1666-2011 does not state what sc_time_stamps returns
		// after SystemC simulation as expired. Therefore, we detect
		// if simulation has finished, so them we use the last time stamp
		// at the end of the simulation.
		// This way, this report function can be safely used at the
		// end of the SystemC simulation
		//cout << "Last simulation time: " << sc_time_stamp() << endl;
		total_average_power_watts = get_consumed_energy_J() / mapped_scheduler->last_simulation_time.to_seconds();
	} else {	
		if(sc_is_running())  {				
			if(sc_time_stamp()>SC_ZERO_TIME) {					
				// this way, this method can be call at any time during the simulation
				total_average_power_watts = get_consumed_energy_J() / sc_time_stamp().to_seconds();
			} else {
				total_average_power_watts = 0.0;			
				SC_REPORT_WARNING("KisTA", "No time advance yet. get_total_average_power_W wil return 0W.");
			}
		} else {
			total_average_power_watts = 0.0;		
			SC_REPORT_WARNING("KisTA", "total_average_power_watts  called before simulation start. 0.0W returned.");
		}		
	}
	return total_average_power_watts;
}
	
		// Get current average power (energy/averaging time, 1s or the value settled by set_power_averaging_time)
const double& processing_element::get_curr_average_power_W() {
	check_call_after_sim_start("get_curr_average_power_W");
	return av_power_watts;
}

		// Maximum value of current average power (similar to find the maximum load at average intervals and multiply by the peak power
const double& processing_element::get_peak_av_power_W(){
	check_call_after_sim_start("get_peak_av_power_W");
	return max_av_power_watts;
}

} // namespace kista

#endif
