/*****************************************************************************

  utils.hpp
  
     Miscelaneous utilities for the whole library
     
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   ICT / Electronic Systems
  Date: 2014 February

 *****************************************************************************/


#ifndef UTILS_HPP
#define UTILS_HPP

#include "systemc.h"

#include "types.hpp"

namespace kista {

// 
// Control of analysis
//

// worst-case vs actual performance estimation
void enable_worst_case_communication(); // not required, set by default
void disable_worst_case_communication();

// --------------------------------------------------
// class for reporting functional validation results
//---------------------------------------------------

class validator_t: public sc_module {
public:
	validator_t(sc_module_name name);
	void set_failure();
	void set_failure_threshold(unsigned int _threshold);
	unsigned int get_n_failures();
	bool failed();
	bool validated();
private:
	void end_of_simulation();
	std::string validation_t;
	unsigned int n_failures;
	unsigned int n_failures_threshold;
	bool OK;
};

// user-friendly function for 
// Enabling functional validation
void enable_functional_validation();

// true if functional validation was enabled
bool functional_validation_enabled();

// setting a threshold for the number of functional failures admited during execution to report an error
// If this is not call, the default is 1
void set_functional_failure_threshold(unsigned int threshold);

// setting a functional failure during execution
void set_functional_failure();

// to get the number of functional failures
unsigned int get_functional_failures();


// ----------------------------------------------------
// checks
// ----------------------------------------------------
// to ensure that a call is done before simulation start
void check_call_before_sim_start(std::string call_name);

// to ensure that a call is done at elaboration time
void check_call_at_elaboration(std::string call_name);

// to ensure that a call is done after simulation start
void check_call_after_sim_start(std::string call_name);

// ----------------------------------------------------
// units
// ----------------------------------------------------
// conversion factor for desired output data rates
double conv_data_rate_factor(char unit);
// conversion factor for desired system units
double conv_isu_factor(char unit);

// ----------------------------------------------------
// definition of parameters hook to KisTA-XML
// ----------------------------------------------------
// defines a global parameter named "param_value" whose value is empty
void declare_global_parameter(const char *param_name);

// defines a global parameter named "param_value" whose default value is param_value (a vector of std::strings)
void declare_global_parameter(const char *param_name, param_value_t *param_value);

// returns a pointer to the value of a global parameter named "param_value"
param_value_t* get_global_parameter(const char *param_name);

void show(global_parameters_t &global_param);

//--------------------------------------------------------
// control the fatality of errors during the exploration
//-----------------------------------------------------

void set_error_stop_exploration();
void set_error_does_not_stop_exploration(); // set by default
	// note: this calls can be called at any time

bool &error_stop_exploration();

} // end namespace kista

#endif


