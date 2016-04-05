/*****************************************************************************

  power_measurement_probe.hpp
  
     Implementation of power measurement class
   
   This file belongs to the KisTA library
 
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: UC
  Deparment:   TEISA / GIM
  Date: 2016 March

 *****************************************************************************/

#ifndef KISTA_POWER_MEASUREMENT_PROBE_CPP
#define KISTA_POWER_MEASUREMENT_PROBE_CPP

#include "systemc.h"

#include "utils.hpp"

#include "power_measurement_probe.hpp"

namespace kista {
	
//
// implementation of helper power measurement class
//

power_measurement_probe::power_measurement_probe(sc_module_name name,
												CONS_ENERGY_FUN_PTR consumed_energy_J_f,
												sc_time av_period_par)
{
	get_consumed_energy_J_f = consumed_energy_J_f;
	averaging_time = av_period_par;
	SC_THREAD(power_accounter_proc);
}

power_measurement_probe::power_measurement_probe(
												CONS_ENERGY_FUN_PTR consumed_energy_J_f,
												sc_time av_period_par
												)
								: sc_module(sc_gen_unique_name("power_measurement_probe"))
{
	get_consumed_energy_J_f = consumed_energy_J_f;
	averaging_time = av_period_par;
	SC_THREAD(power_accounter_proc);
}

void power_measurement_probe::set_averaging_time(sc_time av_time) {
	check_call_before_sim_start("set_averaging_time");
	averaging_time = av_time;
}

void power_measurement_probe::power_accounter_proc()
{
	double previous_total_consumed_energy_J;
	double consumed_energy_J_f;
cout << "power_measurement_probe \"" << name() << "\" (av. time=" << averaging_time << "): START at " << sc_time_stamp() << endl;	

	previous_total_consumed_energy_J = get_consumed_energy_J_f();
	peak_av_power_W = 0;
	
	while(true) {
		wait(averaging_time);
cout << "power_measurement_probe \"" << name() << "\": executing at " << sc_time_stamp() << endl;		
		// calculates power for the configured power averaging time
		consumed_energy_J_f = get_consumed_energy_J_f();
		curr_av_power_W = (consumed_energy_J_f - previous_total_consumed_energy_J) / averaging_time.to_seconds();
cout << "power_measurement_probe \"" << name() << "\": consumed_energy (J) = " <<  get_consumed_energy_J_f() << endl;				
cout << "power_measurement_probe \"" << name() << "\": previous_total_consumed_energy (J) = " << previous_total_consumed_energy_J  << endl;				
cout << "power_measurement_probe \"" << name() << "\": averaging time (s) = " << averaging_time.to_seconds() << endl;
cout << "power_measurement_probe \"" << name() << "\": curr_av_power (W) = " <<  curr_av_power_W << endl;				
		// updates the maximum power (for the power averaging time)
		if(curr_av_power_W>peak_av_power_W) {
			peak_av_power_W = curr_av_power_W;
		}
		previous_total_consumed_energy_J = consumed_energy_J_f;
cout << "power_measurement_probe \"" << name() << "\": peak_av_power_W (W) = " <<  peak_av_power_W << endl;						
	}
}


// Average power as a result of total energy/simulated time
//   (can be called during simulation and at the end of the simulation)
const double &power_measurement_probe::get_total_average_power_W() {
	check_call_after_sim_start("get_total_average_power_W");	
	if(sc_get_status()&SC_STOPPED) {	
		// IEEE Std 1666-2011 does not state what sc_time_stamps returns
		// after SystemC simulation as expired. 
		// This will work if it returns the last simulation time
		// cout << "Last simulation time: " << sc_time_stamp() << endl;
		total_average_power_watts = this->get_consumed_energy_J_f() / sc_time_stamp().to_seconds();
	} else {	
		if(sc_is_running())  {				
			if(sc_time_stamp()>SC_ZERO_TIME) {					
				// this way, this method can be call at any time during the simulation
				total_average_power_watts = this->get_consumed_energy_J_f() / sc_time_stamp().to_seconds();
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
	
// Get the last calculated average power (energy/averaging time, 1s or the value settled by set_power_averaging_time)
const double &power_measurement_probe::get_average_power_W() {
	return curr_av_power_W;
}

// Maximum value of the average power (similar to find the maximum load at average intervals and multiply by the peak power
const double &power_measurement_probe::get_peak_av_power_W() {
	return peak_av_power_W;
}

}
// end namespace kista

#endif

