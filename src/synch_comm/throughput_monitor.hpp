/*****************************************************************************

  throughput_monitor.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

  Notes:

 *****************************************************************************/
 
#ifndef _TROUGHPUT_MONITOR_HPP
#define _TROUGHPUT_MONITOR_HPP

#include <systemc.h>

#include "defaults.hpp"
#include "types.hpp"

namespace kista {

// Throughput monitor
class throughput_monitor {
public:	
	throughput_monitor();
	throughput_monitor(unsigned int data_size_par); // in bits
//	throughput_monitor(unsigned int data_size_par, unsigned int max); // max: limit for events buffer to measure throughput
	
	void set_data_size(unsigned int data_size_par); // in bits
		
	void annotate_data_event();
	
	// total throughput (events or data transferred along the simulation time
	double get_throughput(const char *unit = "b");	// bps by default
	// for systems reaching a stationary phase, where data ends up arriving strictly periodically
	double get_stationary_throughput(const char *unit = "b"); // bps by default

	// normalized throughput: provides event frequency, regardless the message size of the edge)
	double get_normalized_throughput(const char *unit = "H");	// bps by default
	// for systems reaching a stationary phase, where data ends up arriving strictly periodically
	double get_normalized_stationary_throughput(const char *unit = "H"); // bps by default	
	
	// reports the last_lag on the last consecutive events, as long as they have been regular
	// such stationarity can be considered, and so it can be considered a period
	sc_time get_period();
	double get_period(const char *unit);

	// UNITS FOR:
	//            Throughput:   'b' (bps) (DEFAULT), 'B' (bytes/s), 'K' (Kbps) , 'M' (Mbps), 'G' (Gbps)
	// Normalized Throughput:   'H' (Hertz) (DEFAULT), ´K´ or 'k' (KiloHerzt), 'Ḿ´ (MegaHerzt), 'G´ (GigaHerzt), 'T´ (TeraHerzt),
	//		(ISU)										 ,  'm' (miliHerzt), 'u' (microHerzt), 'n' (nanoHerzt), 'p' (picoHertz)
	
private:
	bool activated;
    // variables to monitor throughput
    sc_time cur_time;
    sc_time prev_time;
    sc_time cur_lag, prev_lag;
    unsigned int data_size;
    unsigned long long n_devents;
    bool stationary;
    unsigned int lag_matches; // number of consequutive lags matches
};

} // end kista namespace

#endif
