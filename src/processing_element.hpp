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
	
	// binding to hardware communication resources
	void connect(phy_comm_res_t *bound_comm_res_par);
	phy_comm_res_t* get_connected_comm_res();

    //
	// performance characterizations
	//
	// set CPI
	void set_CPI(unsigned int cpi_var);	                 // set Cycles per instruction (default =1)
	inline unsigned int &get_CPI() { return cpi; }		 // get cycles per instruction
	
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
	
		// basic power&energy consumption input variables
	    // set static instantaneous power consumption in Watt
	void set_static_power_consumption(double static_power_cons_watt_par);
	    // set energystatic instantaneous power consumption in Watt
	void set_energy_consumption_per_instruction(double jules_per_instruction_par);	
		// corresponding getters
	inline double &get_static_power_consumption();
	inline double &get_energy_consumption_per_instruction();

	// to control power measurement
	void enable_energy_and_power_measurement();
	void set_power_averaging_time(sc_time avg_time);
              // time period for which average power is calculated
	          //    default value = 1s
	          //    (if settled to 0 average is done on total simulated time)

	// getters for the estimations done
	  // total energy consumed by this PE (computation) along the whole simulation time (in Jules)
	const double &get_total_energy_J();
	  // static and dynamic components of total energy consumed by this PE (computation) along the whole simulation time (in Jules)
	const double &get_total_static_energy_J(); 
	const double &get_total_dynamic_energy_J();
	
	   // Average power as a result of total energy/simulated time
	const double &get_total_average_power_W();
		// Get current average power (energy/averaging time, 1s or the value settled by set_power_averaging_time)
	const double &get_average_power_W();
		// Maximum value of current average power (similar to find the maximum load at average intervals and multiply by the peak power
	const double &get_peak_av_power_W();

		// Maximum value of power, statically determined by CPI, PE frequency and static+instruction energy
	const double &get_peak_dyn_power_W();

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
	
	// energy and power characterization
	double static_power_cons_watt;
	double jules_per_instruction;
		
	// network interface variables
	network_interface *netif_p;
	
	// Processor related energy and power counters
	bool do_energy_and_power_measurement;
	
	double total_energy_J;
	double total_average_power_watts;
	
	  // the processor is notified every bunch of instructions that
	  // it is continuously executed
	double av_power_watts;               // current averaged power
	double max_av_power_watts;              // maximum value of averaged power
								   // minimum value of averaged power (after getting the first value of average power)
	
	sc_time power_averaging_time;  // time period for average power
	                               // default value = 1s
	                               // (if settled to 0 average is done on total simulated time)
	
	double peak_dyn_power_watts; // peak dynamic power consumption:
	                             //    statically calculated (at elab time, as energy per instruction / frequency) (shall consider CPI)
	sc_time peak_dyn_power_time; // minimum time resolution for peaky dynamic power:
	                             //    inverse of frequency (shall consider CPI)
	
	void init_energy_power_vars();
	// to be called before simulation start, at end of elaboration
	void calculate_static_outputs();
                             
	void before_end_of_elaboration();
	
	void end_of_elaboration();
	
	void add_energy(); // method for enabling tasks add energy to
	// periodic process accounting energy and power
	void power_accounter_proc();

};


} // namespace kista

#endif


