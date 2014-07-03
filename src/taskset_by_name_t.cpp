/*****************************************************************************

  taskset_by_name_t.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

 *****************************************************************************/

#ifndef taskset_by_name_t_CPP
#define taskset_by_name_t_CPP

#include "auxiliar.hpp"

namespace kista {

// to check is a task set is a "periodic task set", that is, all the tasks are periodic (Davis&Burns '09)
bool is_periodic(const taskset_by_name_t &taskset) {	
	if(sc_is_running()) {
		SC_REPORT_ERROR("KisTA", "is_period(taskset) function must be invoked before the beginning of the simulation");
	}
			
	for(auto it = taskset.begin(); it != taskset.end(); ++it ) {
		if(!(it->second->is_periodic())) return false;
	}
	return true; // reaching this point means that all the tasks are periodic and therefore the for loop can finish
}

// Returns the hyper period (minimum common multiple of all periods , Davis&Burns '09 )
//    This function, first checks that the task set is periodic (so it does not requires a call to the is_periodic function for the task set
sc_time get_hyperperiod(const taskset_by_name_t &taskset) {
	std::vector<unsigned long long int> taskset_periods;
	taskset_by_name_t::iterator it;
	std::vector<unsigned long long int>::iterator vector_it;
	sc_time period;
	std::string msg;
	
	// First checks if the taskset is periodic
	if(!is_periodic(taskset)) {
		msg = "Task set";
		// msg += taskset.name();
		msg += " is not periodic. Therefore, it is not possible to calculate an hyperperiod for it.";
		msg += " Check that get_hyperperiod function is called after having settled all periods of the task set.";
		SC_REPORT_ERROR("KisTA", msg.c_str() );
	}

	for(auto it = taskset.begin(); it != taskset.end(); it++ ) {
		vector_it = taskset_periods.begin();
		//taskset_periods[i] = it->second->get_period();
		period = it->second->get_period();
//cout << "hiperperiodo = " << period.to_seconds()*1000000000.0 << endl;		
//cout << "hiperperiodo = " << (unsigned long long int)period.to_seconds()*1000000000.0 << endl;
		// (unsigned long long int)period.to_seconds()*1000000000.0 // this does not work properly, use static_cast better
//cout << "hiperperiodo = " << static_cast<unsigned long long int>(period.to_seconds()*1000000000.0) << endl;		
		//taskset_periods[i] = static_cast<unsigned long long int>(period.to_seconds()*1000000000.0); // pass to nanoseconds (in double value) and then explicit conversion to unsigned int
		taskset_periods.insert(vector_it, static_cast<unsigned long long int>(period.to_seconds()*1000000000.0) ); // pass to nanoseconds (in double value) and then explicit conversion to unsigned int
		vector_it++;
	}
	
	return sc_time( (double)lcm<unsigned long long int>(taskset_periods) , SC_NS );
	//return lcm<sc_time>(taskset_periods);
}

// Gets "static" utilization of the taskset (as the addition of the utilizations of the static set)
double utilization(const taskset_by_name_t &taskset) {
	// Gets "static" utilization of the taskset (as the addition of the utilization of the static set)
	double taskset_utilization = 0.0;
	
	for(auto it = taskset.begin(); it != taskset.end(); ++it ) {
		taskset_utilization += it->second->utilization();
	}
	return taskset_utilization;
}

// Gets "static" utilization of the taskset (as the addition of the densities of the static set)
double density(const taskset_by_name_t &taskset) {
	// Gets "static" density of the taskset (as the addition of the density of the static set)
	double taskset_density = 0.0;
	
	for(auto it = taskset.begin(); it != taskset.end(); ++it ) {
		taskset_density += it->second->density();
	}
	return taskset_density;	
}
	
} // end namespace kista

#endif
