/*****************************************************************************

  utils.cpp
  
     Miscelaneous utilities implementation for the whole library
   
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   ICT / Electronic Systems
  Date: 2014 February

 *****************************************************************************/


#ifndef UTILS_CPP
#define UTILS_CPP

#include "systemc.h"

#include "defaults.hpp"
#include "utils.hpp"
#include "global_elements.hpp"

namespace kista {

void enable_worst_case_communication() {
	check_call_before_sim_start("enable_worst_case_communication");
	worst_case_communication_enabled = true;
}

void disable_worst_case_communication() {
	check_call_before_sim_start("disable_worst_case_communication");
	worst_case_communication_enabled = false;
}

void check_call_before_sim_start(std::string call_name) {
	std::string rpt_msg;
	if ( sc_get_status()&(SC_START_OF_SIMULATION|SC_RUNNING|SC_STOPPED|SC_PAUSED|SC_END_OF_SIMULATION) ) {
		rpt_msg = call_name;
		//rpt_msg += name();
		rpt_msg += " has to be called before simulation start.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	}	
}

void check_call_at_elaboration(std::string call_name) {
	std::string rpt_msg;	
	if ( !(sc_get_status()&SC_ELABORATION) )  {
		rpt_msg = call_name;
		//rpt_msg += name();
		rpt_msg += " has to be called at elaboration time (before sc_start).";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	}
}

void check_call_after_sim_start(std::string call_name) {
	std::string rpt_msg;	
	if ( sc_get_status()&(SC_ELABORATION|SC_BEFORE_END_OF_ELABORATION|SC_END_OF_ELABORATION) )  {
		rpt_msg = call_name;
		//rpt_msg += name();
		rpt_msg += " has to be called after simulation start.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	}
}


// conversion factor for desired output data rates
double conv_data_rate_factor(char unit) {
	std::string rpt_msg;
	if(unit == 'b') {
		return 1.0;
	}
	else
	if(unit == 'B') {
		return 0.125; // 1.0/8.0
	}
	else
	if( (unit == 'K') || (unit == 'k')) {
		return 0.001;
	}
	else
	if(unit == 'M') {
		return 0.000001;
	}
	else
	if(unit == 'G') {
		return 0.000000001;
	}
	else
	if(unit == 'T') {
		return 0.000000000001;
	}
	else {
		rpt_msg = "Unknown unit. Supported units for throughput are 'b' (bps), 'B' (Bytes/s), 'K' or 'k' (Kbps), 'M' (Mbps), 'G' (Gbps) and 'T' (Tbps).";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	}
}

// conversion factor for desired output system unit
double conv_isu_factor(char unit) {
	std::string rpt_msg;	
	if( (unit == '-') || (unit == '\0')) {
		return 1.0;
	}
	else
	if( (unit == 'K') || (unit == 'k')) {
		return 0.001;
	}
	else
	if(unit == 'M') {
		return 0.000001;
	}
	else
	if(unit == 'G') {
		return 0.000000001;
	}
	else
	if(unit == 'T') {
		return 0.000000000001;
	}
	if(unit == 'm') {
		return 1000.0;
	}	
	else
	if(unit == 'u') {
		return 1000000.0;
	}	
	else
	if(unit == 'n') {
		return 1000000000.0;
	}
	else
	if(unit == 'p') {
		return 1000000000000.0;
	}	
	else
	if(unit == 'f') {
		return 1000000000000000.0;
	}		
	else {
		rpt_msg = "Unknown unit.\n";
		rpt_msg += "Supported units are '\0' or '-' for units, 'K' or 'k' (Kilo), 'M' (Mega), 'G' (Giga), 'T' (Tera), ";
		rpt_msg += "'m' (mili), 'u' (micro), 'n' (nano), 'p' (pico), and 'f' (femto).";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	}
}

validator_t::validator_t(sc_module_name name) {
	OK =true;
	n_failures = 0;
	n_failures_threshold = 1;
	validation_t = name;
}

void validator_t::set_failure() {
	n_failures++;
	if(n_failures>=n_failures_threshold) OK = false;
}

void validator_t::set_failure_threshold(unsigned int _threshold) {
	std::string msg;
	if(_threshold<1) {
		msg = "The failures threshold of the validator ";
		msg += name();
		msg += " has to be at least 1 or more.";
		SC_REPORT_ERROR("KisTA",msg.c_str());
	}
	n_failures_threshold = _threshold;
}
	
unsigned int validator_t::get_n_failures() {
	return n_failures;
}
	
bool validator_t::failed() {
	return (!OK);
}

bool validator_t::validated() {
	return OK;
}

void validator_t::end_of_simulation() {
	std::string msg;
	if(this->failed()) {
		msg = "Validation of ";
		msg += validation_t;
		msg += " FAILED.";
		SC_REPORT_ERROR("KisTA",msg.c_str());
	}
#ifdef _PRINT_VALIDATORS_PASS
	else {
		msg = "Validation of ";
		msg += validation_t;
		msg += " PASSED.";
		SC_REPORT_INFO("KisTA",msg.c_str());		
	}
#endif
#ifdef _PRINT_VALIDATORS_FAILURES
	cout << "Validator for " << name() << ", number of failures: " << n_failures << endl;
#endif
}

	
// work on the global element functional monitor
void enable_functional_validation() {
	std::string msg;
	check_call_before_sim_start("enable_functional_validation");
	if(functionality_monitor == NULL) {	
		functionality_monitor = new validator_t("FUNCTIONALITY");
	} else {
		msg = "The function  enable_functional_validation must be called only once (before simulation start).";
		SC_REPORT_ERROR("KisTA",msg.c_str());		
	}
}

bool functional_validation_enabled() {
	if(functionality_monitor != NULL) {
		return true;
	} else {
		return false;
	}
}

// setting a limit for the number of functional failures admited during execution to report an error
// If this is not call, the default is 1
void set_functional_failure_threshold(unsigned int _threshold) {
	std::string msg;
	if(functionality_monitor != NULL) {
		check_call_before_sim_start("set_functional_failure_limit");
		functionality_monitor->set_failure_threshold(_threshold);
	}
#ifdef WARN_SET_FUNCTIONAL_FAILURE_THRESHOLD_WHEN_FUNCTIONAL_VALIDATION_NOT_ENABLED	
	else {
		msg = "The function set_functional_failure_threshold was called, but the functional validation is not enabled.";
		SC_REPORT_WARNING("KisTA",msg.c_str());
	}
#endif
}

// setting a functional failure during execution
void set_functional_failure() {
	if(functionality_monitor != NULL) {
		check_call_after_sim_start("set_functional_failure");
		functionality_monitor->set_failure();
	}
#ifdef WARN_SET_FUNCTIONAL_FAILURE_WHEN_FUNCTIONAL_VALIDATION_NOT_ENABLED
	else {
		SC_REPORT_WARNING("KisTA","set_functional_failure called, but functional validation not enabled. The call will not make effect.");
	}
#endif
}

// to get the number of functional failures (at any time)
unsigned int get_functional_failures() {
	if(functionality_monitor != NULL) {
		return functionality_monitor->get_n_failures();
	} else {
		SC_REPORT_WARNING("KisTA","get_functional_failures called, but functional validation not enabled. 0 will be retorned");
		return 0;
	}
}

// defines a global parameter named "param_value" whose value is empty
void declare_global_parameter(const char *param_name) {
	global_parameters[param_name]=NULL;
}

// defines a global parameter named "param_value" whose default value is param_value (a vector of std::strings)
void declare_global_parameter(const char *param_name, param_value_t *param_value) {
	global_parameters[param_name]=param_value;
}

// returns a pointer to the value of a global parameter named "param_value"
param_value_t* get_global_parameter(const char *param_name) {
	return global_parameters[param_name];
}


void show(global_parameters_t &global_param) {
	global_parameters_t::iterator it;
	cout << "Parameter list:" << endl;
	for(it = global_param.begin();it!=global_param.end();it++) {
		cout << "\t" << it->first << "=";
		if(it->second==NULL) {
			cout << "NONE" << endl;
		} else {
			if (it->second->size()==1) {
				cout << (*it->second)[0] << endl;
			} else { // asume size >1
				cout << "[";
				for(unsigned int i=0;i<it->second->size();i++) {
					cout << (*it->second)[i];
					if(i<(it->second->size()-1)) cout << ",";
				}
				cout << "]" << endl;
			}
		}
	}
}

//--------------------------------------------------------
// control the fatality of errors during the exploration
//-----------------------------------------------------
extern bool error_stop_exploration_flag;

void set_error_stop_exploration() {
	error_stop_exploration_flag = true;
}

void set_error_does_not_stop_exploration() {
	error_stop_exploration_flag = false;
}

bool &error_stop_exploration() {
	return error_stop_exploration_flag;
}


} // end namespace kista

#endif


