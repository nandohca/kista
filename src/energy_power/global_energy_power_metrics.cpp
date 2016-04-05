/*****************************************************************************

  global_energy_power_metrics.cpp
  
     Implementation of functions to obtain energy and power consumption metrics
   
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: UC
  Deparment:   TEISA / GIM
  Date: 2016 March

 *****************************************************************************/


#ifndef GLOBAL_ENERGY_POWER_METRICS_CPP
#define GLOBAL_ENERGY_POWER_METRICS_CPP

#include "global_energy_power_metrics.hpp"

#include "power_measurement_probe.hpp"
#include "global_elements.hpp"
#include "utils.hpp"
#include "processing_element.hpp"

namespace kista {

// global power measurement probes created for global power accounting
// this global power probes are created always (whenenver energy&power analysis is enabled)
power_measurement_probe *global_power_probe = NULL;
power_measurement_probe *global_dynamic_power_probe = NULL;
	
// global flag for energy and power measurement
void enable_energy_and_power_measurement() {
	check_call_before_sim_start("enable_energy_and_power_measurement");
	energy_and_power_measurement_enabled = true;
	// generate the global power measurement probes using
	//     the global consumed energy measurements
	//     the global either default or configured global power average time
	global_power_probe = new power_measurement_probe("global_power_probe",get_total_consumed_energy_J, global_power_averaging_time);
	global_dynamic_power_probe = new power_measurement_probe("global_dynamic_power_probe",get_total_dynamic_consumed_energy_J, global_power_averaging_time);
}

void disable_energy_and_power_measurement() {
	check_call_before_sim_start("enable_energy_and_power_measurement");	
	energy_and_power_measurement_enabled = false;
	if (global_power_probe!=NULL) delete global_power_probe;
	if (global_dynamic_power_probe!=NULL) delete global_dynamic_power_probe;
}

bool was_energy_and_power_measurement_enabled() {
	check_call_after_sim_start("was_energy_and_power_measurement_enabled");
	return energy_and_power_measurement_enabled;
}

// --------------------------------
// global energy analysis
// --------------------------------

 // energy consumed by all the elements of the platform (currently only PEs) 
double get_total_consumed_energy_J() {
	// In theory, this should be equivalent using "pes" (global vector)
	// or "PEref_by_name" (global map by name). Both should contain the same
	// PE list.
	double total_consumed_energy_var = 0.0;
	// account all the energy consumed by each PE
	for(unsigned int i=0; i<pes.size(); i++) {
		total_consumed_energy_var+=pes[i]->get_consumed_energy_J();
	}
	// TO DO
	// add all the energy consumed by communication resources
	
	// add all the energy consumed by memories
	
	return total_consumed_energy_var;
}

 // static energy consumed by all the elements of the platform (currently only PEs)
double get_total_static_consumed_energy_J(){
	// In theory, this should be equivalent using "pes" (global vector)
	// or "PEref_by_name" (global map by name). Both should contain the same
	// PE list.
	double total_consumed_static_energy_var = 0.0;
	// account all the static energy consumed by each PE
	for(unsigned int i=0; i<pes.size(); i++) {
		total_consumed_static_energy_var+=pes[i]->get_consumed_static_energy_J();
	}
	// TO DO
	// add all the static energy consumed by communication resources
	
	// add all the static energy consumed by memories
	return total_consumed_static_energy_var;	
}


 // dynamic energy consumed by all the elements of the platform (currently only PEs) 
double get_total_dynamic_consumed_energy_J(){
	// In theory, this should be equivalent using "pes" (global vector)
	// or "PEref_by_name" (global map by name). Both should contain the same
	// PE list.
	double total_consumed_dynamic_energy_var = 0.0;
	// account all the dynamic energy consumed by each PE
	for(unsigned int i=0; i<pes.size(); i++) {
		total_consumed_dynamic_energy_var+=pes[i]->get_consumed_dynamic_energy_J();
	}
	// TO DO
	// add all the dynamic energy consumed by communication resources
	
	// add all the dynamic energy consumed by memories
	
	return total_consumed_dynamic_energy_var;
}


// --------------------------------
// global power analysis
// --------------------------------

// set the average time for calculating the global peak dyn. power and the peak av power
// (not necessarily synchronized with PE average calculators)
void set_global_power_averaging_time(sc_time avg_time){
	global_power_averaging_time = avg_time;
	// if the global power probes have been already created, their average times
	// are updated
	if (global_power_probe!=NULL) {
		global_power_probe->set_averaging_time(avg_time);
	}
	if (global_dynamic_power_probe!=NULL) {
		global_dynamic_power_probe->set_averaging_time(avg_time);
	}	
#ifdef _PRINT_POWER_ACCOUNTING_MESSAGES
	string rpt_msg;
	rpt_msg = "Global power averaging time settled to ";
	rpt_msg += 	global_power_averaging_time.to_string();
	SC_REPORT_INFO("KisTA",rpt_msg.c_str());
#endif
}

	          //    default value = 1s
	          //    (if settled to 0 average is done on total simulated time)


// power metrics getters
  // get the maximum dynamic power, where dynamic power is computed every global power averaging time
double get_peak_dyn_power_W(){
	return global_power_probe->get_average_power_W();
}


  // get the maximum power (static+dynamic), where dynamic power is computed every global power averaging time
double get_peak_avg_power_W(){
	return global_power_probe->get_peak_av_power_W();
}


  // get average power as the division of total consumed energy and last simulated time
double get_total_average_power_W(){
	return global_power_probe->get_total_average_power_W();
}


} // end namespace kista

#endif
