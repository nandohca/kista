/*****************************************************************************

  throughput_monitor.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

  Notes:

 *****************************************************************************/
 
#ifndef _TROUGHPUT_MONITOR_CPP
#define _TROUGHPUT_MONITOR_CPP

#include "throughput_monitor.hpp"

#include "utils.hpp"

namespace kista {

throughput_monitor::throughput_monitor() {
	data_size = 0;	
	cur_time = SC_ZERO_TIME;
	prev_time = SC_ZERO_TIME;
	cur_lag = SC_ZERO_TIME;
	prev_lag = SC_ZERO_TIME;	
	n_devents = 0;
	stationary = false;
	lag_matches = 0;
	activated = false;
}

throughput_monitor::throughput_monitor(unsigned int data_size_par) {
	data_size = data_size_par;
	cur_time = SC_ZERO_TIME;
	prev_time = SC_ZERO_TIME;
	cur_lag = SC_ZERO_TIME;
	prev_lag = SC_ZERO_TIME;	
	n_devents = 0;
	stationary = false;
	lag_matches = 0;
	activated = false;
}

void throughput_monitor::set_data_size(unsigned int data_size_par) {
	data_size = data_size_par;
}

void throughput_monitor::annotate_data_event() {
	n_devents++;
	
	// NOTE: 
	//   It is considered that the first data transfer event 
	//   can be already used to calculate a througput assessment
	//   as long as it involves a time advance.
	//   time 0 is taken as the previous time.
	//   However, the first event does not serves to have two lags,
	//   so at least a second event is required
	//
	if(n_devents == 1 ) { // there is one lag, which enables 
	                             // at least one throughput measurement
		cur_time = sc_time_stamp();
		cur_lag = cur_time - prev_time;	
	} else if(n_devents>=2) { // there is at least two or more lags
	                         // which enables a minimum assessment of
	                         // throughput
		cur_time = sc_time_stamp();
		prev_lag = cur_lag;
		cur_lag = cur_time - prev_time;

		if(cur_lag==prev_lag) {
			if(lag_matches<LAG_MATCHES_LIMIT) {
				lag_matches++;
				// stationary = true; // not longer required
			} 
			if(lag_matches>=N_REPEATED_LAGS_FOR_STATIONARY) {
				stationary = true;
			}
		} else { // resent the state machine, being demanding in
		         // the search of stationarity
			lag_matches = 0;
			stationary = false;
		}

		prev_time = cur_time;
	}
}

double throughput_monitor::get_throughput(const char *unit) {
	std::string rpt_msg;

#ifdef _WARN_THROUGHPUT_MONITOR_ISSUES	
	if(data_size==0) {
		SC_REPORT_WARNING("KisTA", "Throughput monitor with data size = 0. Check that the throughput monitor has a data size assigned, either via its constructor or the set_data_size method.\n");
	}
#endif

	if(n_devents>0) {
		if(cur_time==SC_ZERO_TIME) {
			return -1.0;
#ifdef _WARN_THROUGHPUT_MONITOR_ISSUES	
			SC_REPORT_WARNING("KisTA", "There was not time advance from simulation start. Throughput is infinite. A negative value is reported!");
#endif			
		} else {
			
			if( (strlen(unit)==0) 
				 ||
				!strncmp(unit,"b",1)
			  )
			{
				return ( n_devents / cur_time.to_seconds());
			} else {
				return ( (conv_data_rate_factor(unit[0]) * n_devents) / cur_time.to_seconds() );
			}
		}
	} else {
		return 0.0;
	}
}


double throughput_monitor::get_stationary_throughput(const char *unit) {
	std::string rpt_msg;
	if(data_size==0) {
#ifdef _WARN_THROUGHPUT_MONITOR_ISSUES			
		SC_REPORT_WARNING("KisTA", "Throughput monitor with data size = 0. Check that the throughput monitor has a data size assigned, either via its constructor or the set_data_size method.\n");
#endif		
	}	
	if(n_devents>1) {
		if(cur_time==SC_ZERO_TIME) {
			return -1.0;
#ifdef _WARN_THROUGHPUT_MONITOR_ISSUES				
			SC_REPORT_WARNING("KisTA", "There was not time advance from simulation start. Throughput is infinite. A negative value is reported!");
#endif			
		} else {
			if(!stationary) {
#ifdef _WARN_THROUGHPUT_MONITOR_ISSUES					
				rpt_msg = "Data transfer did not reached a stationary state, msg_size*n_transfer_events/last_lag will be reported\n";
				SC_REPORT_WARNING("KisTA", rpt_msg.c_str());
#endif				
			}
#ifdef _REPORT_NUMBER_OF_MATCHES_IN_STATIONARITY_ASSESSMENT
			else {
				rpt_msg = "Throughput measurement confirmed the stationarity with ";
				rpt_msg += std::to_string(lag_matches);
				rpt_msg += " lag matches in a raw";
				if(lag_matches==LAG_MATCHES_LIMIT) {
					rpt_msg += "(reaching lag matches account limit)";
				}
				rpt_msg += ".";
				SC_REPORT_INFO("KisTA", rpt_msg.c_str());
			}
#endif				
			
			if( (strlen(unit)==0) 
				 ||
				!strncmp(unit,"b",1)
			  )
			{
				return ( data_size / cur_lag.to_seconds() );
			} else {
				return ( ( conv_data_rate_factor(unit[0]) * data_size) / cur_lag.to_seconds() );
			}
		}
	}	
	else {
#ifdef _WARN_THROUGHPUT_MONITOR_ISSUES			
		SC_REPORT_WARNING("KisTA", "Unsuficient number of transfer events for reporting stationary throughput. 0 will be reported!");
#endif		
		//SC_REPORT_ERROR("KisTA", "Unsufficient number of transfer events for reporting stationary throughput. 0 will be reported!");
		return 0.0;
	}	
}


double throughput_monitor::get_normalized_throughput(const char *unit) {
	std::string rpt_msg;
#ifdef _WARN_THROUGHPUT_MONITOR_ISSUES	
	if(data_size==0) {
		SC_REPORT_WARNING("KisTA", "Throughput monitor with data size = 0. Check that the throughput monitor has a data size assigned, either via its constructor or the set_data_size method.\n");
	}
#endif
	if(n_devents>0) {
		if(cur_time==SC_ZERO_TIME) {
			return -1.0;
#ifdef _WARN_THROUGHPUT_MONITOR_ISSUES			
			SC_REPORT_WARNING("KisTA", "There was not time advance from simulation start. Throughput is infinite. A negative value is reported!");
#endif			
		} else {
			if( (strlen(unit)==0) 
				 ||
				!strncmp(unit,"H",1)
				 ||
				!strncmp(unit,"h",1)
			  )
			{
				return ( n_devents / cur_time.to_seconds());
			} else {
				return ( (conv_isu_factor(unit[0]) * n_devents) / cur_time.to_seconds() );
			}
		}
	} else {
		return 0.0;
	}
}


double throughput_monitor::get_normalized_stationary_throughput(const char *unit) {
	std::string rpt_msg;
#ifdef _WARN_THROUGHPUT_MONITOR_ISSUES	
	if(data_size==0) {
		SC_REPORT_WARNING("KisTA", "Throughput monitor with data size = 0. Check that the throughput monitor has a data size assigned, either via its constructor or the set_data_size method.\n");
	}	
#endif
	if(n_devents>1) {
		if(cur_time==SC_ZERO_TIME) {
			return -1.0;
#ifdef _WARN_THROUGHPUT_MONITOR_ISSUES			
			SC_REPORT_WARNING("KisTA", "There was not time advance from simulation start. Throughput is infinite. A negative value is reported!");
#endif			
		} else {
			if(!stationary) {
#ifdef _WARN_THROUGHPUT_MONITOR_ISSUES				
				rpt_msg = "Data transfer did not reached a stationary state, n_transfer_events/last_lag will be reported\n";
				SC_REPORT_WARNING("KisTA", rpt_msg.c_str());
#endif				
			}
#ifdef _REPORT_NUMBER_OF_MATCHES_IN_STATIONARITY_ASSESSMENT
			else {
				rpt_msg = "Throughput measurement confirmed the stationarity with ";
				rpt_msg += std::to_string(lag_matches);
				rpt_msg += " lag matches in a raw";
				if(lag_matches==LAG_MATCHES_LIMIT) {
					rpt_msg += "(reaching lag matches account limit)";
				}
				rpt_msg += ".";
				SC_REPORT_INFO("KisTA", rpt_msg.c_str());
			}
#endif	
			if( (strlen(unit)==0) 
				 ||
				!strncmp(unit,"H",1)
				 ||
				!strncmp(unit,"h",1)
			  )
			{
				return ( 1 / cur_lag.to_seconds() );
			} else {
				return ( conv_isu_factor(unit[0]) / cur_lag.to_seconds() );
			}
		}
	} else {
#ifdef _WARN_THROUGHPUT_MONITOR_ISSUES		
		SC_REPORT_WARNING("KisTA", "Unsuficient number of transfer events for reporting stationary throughput. 0 will be reported!");
#endif		
		//SC_REPORT_ERROR("KisTA", "Unsufficient number of transfer events for reporting stationary throughput. 0 will be reported!");
		return 0.0;
	}
}


// reports the last_lag on the last consecutive events, as long as they have been regular
// such stationarity can be considered, and so it can be considered a period

sc_time throughput_monitor::get_period() {
	std::string rpt_msg;
	if(n_devents>1) {
		if(cur_time==SC_ZERO_TIME) {
			return SC_ZERO_TIME;
#ifdef _WARN_THROUGHPUT_MONITOR_ISSUES	
			rpt_msg = "There was not time advance from simulation start. O time is reported for get_period() call of throughput monitor.";
			SC_REPORT_WARNING("KisTA",rpt_msg.c_str());
#endif			
		} else {
			if(!stationary) {
#ifdef _WARN_THROUGHPUT_MONITOR_ISSUES				
				rpt_msg = "Data transfer did not reached a stationary state, last lag will be reported for throughput monitor.";
				SC_REPORT_WARNING("KisTA", rpt_msg.c_str());
#endif				
			}
#ifdef _REPORT_NUMBER_OF_MATCHES_IN_STATIONARITY_ASSESSMENT
			else {
				rpt_msg = "Throughput measurement confirmed the stationarity with ";
				rpt_msg += std::to_string(lag_matches);
				rpt_msg += " lag matches in a raw";
				if(lag_matches==LAG_MATCHES_LIMIT) {
					rpt_msg += "(reaching lag matches account limit)";
				}
				rpt_msg += ".";
				SC_REPORT_INFO("KisTA", rpt_msg.c_str());
			}
#endif	
			return cur_lag;
		}
	} else {
#ifdef _WARN_THROUGHPUT_MONITOR_ISSUES		
		rpt_msg = "Unsuficient number of transfer events for reporting stationary throughput. 0 will be reported for throughput monitor.";
		SC_REPORT_WARNING("KisTA", rpt_msg.c_str());
#endif		
		//SC_REPORT_ERROR("KisTA", "Unsufficient number of transfer events for reporting stationary throughput. 0 will be reported!");
		return SC_ZERO_TIME;
	}
}

double throughput_monitor::get_period(const char *unit) {
	double value_in_units;
	value_in_units = ( this->get_period().to_seconds() ) * conv_isu_factor(unit[0]);
	return value_in_units;
}

} // end kista namespace

#endif
