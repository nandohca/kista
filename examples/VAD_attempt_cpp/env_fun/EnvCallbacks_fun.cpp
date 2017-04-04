/*****************************************************************************

  GlobalInit_fun.cpp

  For Global initialization before start of simulation

  Author: F. Herrera
  Institution: KTH
  Deparment: ICT/ES
  Date: 2014 February

 *****************************************************************************/

#include "kista.hpp"

#include <iostream>

#define FAILURES_THRESHOLD 1
extern unsigned int frames_proc;
extern unsigned int frames_checked;

extern "C"
void src_param_fun() {
	// enables to pass as a parameter the test name
	declare_global_parameter("test_name");	
	show(global_parameters);
}

extern "C"
void env_init_fun() {
	set_functional_failure_threshold(FAILURES_THRESHOLD);
	cout << "Global Environment init function set the Functional Failures Threshold to " << FAILURES_THRESHOLD << endl;
}

extern "C"
void env_end_fun() {

	param_value_t *param_value_p;
	std::string test_name;
	
	param_value_p = get_global_parameter("test_name");
	if(param_value_p==NULL) {
		SC_REPORT_ERROR("Env. user code: VAD source","Error retrieving test name");
	}	
	test_name = (*param_value_p)[0];

	cout << "------------------------" << endl;	
	cout << "VAD Environment Report " << endl;	
	cout << "Test: " << test_name << endl;

	cout << "Frames processed: " << frames_proc << endl;
	cout << "Frames checked: " << frames_checked << endl;
	cout << "Validation failures: ";
	if(functional_validation_enabled()) cout << get_functional_failures() << endl;
	else  cout << "Functional validation was not set!" << endl;
	cout << "------------------------" << endl;	
}
