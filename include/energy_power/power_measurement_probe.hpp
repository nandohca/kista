/*****************************************************************************

  power_measurement_probe.hpp
  
     Functions to obtain energy and power consumption metrics
   
   This file belongs to the KisTA library
 
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: UC
  Deparment:   TEISA / GIM
  Date: 2016 March

 *****************************************************************************/

#ifndef KISTA_POWER_MEASUREMENT_PROBE_HPP
#define KISTA_POWER_MEASUREMENT_PROBE_HPP

#include "systemc.h"

namespace kista {
	
// --------------------------------------------------------------
// class for power measurement
// --------------------------------------------------------------

// prototype for the function which allows to retrieve the consumed energy
typedef double  (*CONS_ENERGY_FUN_PTR)();

class power_measurement_probe:public sc_module {
public:
	SC_HAS_PROCESS(power_measurement_probe);

	// constructor
	//  		name  of the probe (shall be unique per probe)
	//        	pointer to the function which allows to retrieve the consumed energy until a specific time
	//          averaging time (1 sec by default)  
	//
	
	power_measurement_probe(sc_module_name name,
							CONS_ENERGY_FUN_PTR consumed_energy_J_f,
	                        sc_time av_period_par = sc_time(1,SC_SEC)
	                        );
	                        
	// constructor with out name parameter (a unique name is assigned by default)
	//        	pointer to the function which allows to retrieve the consumed energy until a specific time
	//          averaging time (1 sec by default)  
	//
	power_measurement_probe(CONS_ENERGY_FUN_PTR consumed_energy_J_f,
	                        sc_time av_period_par = sc_time(1,SC_SEC)
	                        );

	void set_averaging_time(sc_time av_time);

	   // Average power as a result of total energy/simulated time
	   //   (can be called during simulation and at the end of the simulation)
	const double &get_total_average_power_W();
	
		// Get the last calculated average power (energy/averaging time, 1s or the value settled by set_power_averaging_time)
	const double &get_average_power_W();
	
		// Maximum value of the average power (similar to find the maximum load at average intervals and multiply by the peak power
	const double &get_peak_av_power_W();
		
	void power_accounter_proc();
	
private:
	CONS_ENERGY_FUN_PTR get_consumed_energy_J_f;
	sc_time averaging_time;
	// accounting variables
	double curr_av_power_W;
	double peak_av_power_W;
	double total_average_power_watts;
};

}
// end namespace kista

#endif
