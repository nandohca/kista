/*****************************************************************************

  global_energy_power_metrics.hpp
  
     Functions to obtain energy and power consumption metrics
   
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: UC
  Deparment:   TEISA / GIM
  Date: 2016 March

 *****************************************************************************/


#ifndef GLOBAL_ENERGY_POWER_METRICS_HPP
#define GLOBAL_ENERGY_POWER_METRICS_HPP

#include "power_measurement_probe.hpp"

namespace kista {


// global flag for energy and power measurement
void enable_energy_and_power_measurement();
void disable_energy_and_power_measurement();

// getter for the global flag for energy and power measurement
// to be called during or at the end of the simulation
// tells if energy and power measurement was enabled
bool was_energy_and_power_measurement_enabled();

// --------------------------------
// global energy analysis
// --------------------------------

 // energy consumed by all the elements of the platform (currently only PEs) 
 //  (callable during and at the end of the simulation)
double get_total_consumed_energy_J();

 // static energy consumed by all the elements of the platform (currently only PEs)
 //  (callable during and at the end of the simulation) 
double get_total_static_consumed_energy_J();

 // dynamic energy consumed by all the elements of the platform (currently only PEs)
 //  (callable during and at the end of the simulation) 
double get_total_dynamic_consumed_energy_J();

// --------------------------------
// global power analysis
// --------------------------------

// set the average time for calculating the global peak dyn. power and the peak av power
// (not necessarily synchronized with PE average calculators)
void set_global_power_averaging_time(sc_time avg_time);
	          //    default value = 1s
	          //    (if settled to 0 average is done on total simulated time)


// power metrics getters
  // get the maximum dynamic power, where dynamic power is computed every global power averaging time
 //  (callable during and at the end of the simulation)  
double get_peak_dyn_power_W();

  // get the maximum power (static+dynamic), where dynamic power is computed every global power averaging time
 //  (callable during and at the end of the simulation)  
double get_peak_avg_power_W();

  // get average power as the division of total consumed energy and last simulated time
 //  (callable during and at the end of the simulation)  
double get_total_average_power_W();

} // end namespace kista

#endif
