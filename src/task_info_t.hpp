/*****************************************************************************

  task_info.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January

 *****************************************************************************/

#ifndef TASK_INFO_T_HPP
#define TASK_INFO_T_HPP

// for the moment, a map, but as commented later, it likely would be better a hash table
#include <map>

#include "types.hpp"

#include "sched_types.hpp"

#include "user_task_constructs.hpp"

namespace kista {

// forward declaration of synchronization and communication events to define them as friend
class memoryless_event;
class memoryless_event_or_list;
class memoryless_event_and_list;
class registered_event;

// forward declaration required to break circular dependency in declarations
class scheduler;

class task_info_t : public sc_module  {
	friend class application;
	friend class scheduler;
	friend void consume(sc_time comp_time);
	friend void consume_WCET();
	friend void yield();
	friend void wait_period();
	friend void wait(memoryless_event			&e);
	friend void wait(memoryless_event_or_list	&eol);
	friend void wait(memoryless_event_and_list	&eal);
	//friend class registered_event;
	friend void wait(registered_event &re);
	friend class semaphore;
	template<class T> friend class fifo_buffer;
public:

	// constructor
	//   to declare a task which is assigned to the default application
	task_info_t(sc_module_name name, VOIDFPTR f, task_role_t role_par=SYSTEM_TASK); // by default, is a system task
	// when this constructor is called, the task is implicitly associated to a default application ("deffapp")
	// this default application is implicitly created the very first time a task is created with this constructor
	
	// constructor stating the application it belongs to (an application must exist and be created before
	// This constructor assumes that there is at least one application declared
	// Then, it does not allows to use the default application (for clarity in 
	// the description methodology)	
	task_info_t(sc_module_name name, VOIDFPTR f, std::string app_name, task_role_t role_par=SYSTEM_TASK); // by default, is a system task
	
	void 				set_task_process_handler(sc_process_handle phandler); // to store the process handler
	sc_process_handle	&get_task_process_handler(); 	  // to get the process handler

	application_t*		get_application(); // returns the associated application

	void                set_scheduler(scheduler *scheduler_p); 
	scheduler		    *get_scheduler();	

	// returns the funtion poitner where task functionality is
	VOIDFPTR& 	get_functionality(); 

	// Task role methods
	task_role_t &get_role();
	bool is_system_task();
	bool is_env_task();

	 // Calls to retrieve task properties  (these calls should be done only at elaboration time)
	void 				set_WCET(sc_time t);
	void 				set_WCEI(unsigned long long wcei_var); // set the worst-case amount of instructions executed
	void 				set_Period(sc_time t); // Set interelease time and implicitly set the task as periodic
                                                       // calling it once declares the task as periodic. (so set_periodic call can be saved)
													   // It also settles a deadline, equal to the period (if deadline has not been settled yet)
	void 				set_Deadline(sc_time t); 
						// settles a deadline. In the case of periodic task
						// deadline setttled through this function overrides
						// the period deadline (regardless of the order of the call)
						// If the deadline is higher than the period it raises an error.
						
	void 				set_priority(unsigned int pr); // this sets a static USER priority

     // Calls to retrieve task properties (these calls can be at any time)
	sc_time				&get_WCET();
	unsigned long long &get_WCEI();
	sc_time				&get_period();
	sc_time				&get_deadline();	
	bool				is_periodic();	
	bool				deadline_settled();

	unsigned int		&get_priority();
	
	// Figures which can be calculated statically, from task characterization
	double				utilization();  // utilization calculates as WCET/Period
	double				density();  	// utilization calculates as WCET/Period
		
	// TASKS SIGNALS
	sc_signal<task_state_t>	state_signal; // (DRIVEN BY THE TASK STATE HANDLER SystemC process)
		//   for unfolded tracing
											  // (DRIVEN BY THE TASK SystemC process)
	sc_signal<bool>				consumption_signal;     	// enabled by the tasks, signaling that WANTS to computes something on a granted CPU
	sc_signal<bool>				waiting_period_signal;  	// enabled when the tasks waits for period: driven by communication
	sc_signal<bool>				waiting_comm_sync_signal;   // enabled when the tasks waits for communication (regardless it has CPU or not): driven by communication
																
												// (DRIVEN BY THE Scheduler SystemC process)
			// signals driven by scheduler
	sc_signal<bool> 			active_signal;	     // enabled when the scheduler grants CPU / disabled when not: driven by scheduler
	
	// Calls for retrieving report information of the task
	// (normally called at the end of simulation, but it can make sense to call them during simulation too)
	bool						miss_deadline(); // returns true if some deadline was missed
	
	// for reporting response times of the task (can be called at the end of the simulation)
	sc_time						max_response_time();		// Maximum response of the task (assumes the task a single activity)
	sc_time						worst_case_response_time(); // both methods provide the same figure: time elapese from task release
															// to the time end of consumption (in kista, detected when reaching wait_period)
															// Therefore, up to know, reponse times are supported only for periodic tasks
	sc_time						location_of_worst_case_response_time(); // Provides the release time when the WC response time is detected
	
	sc_time						avg_response_time();	 // TBC
	sc_time						std_dev_response_time(); // TBC

	unsigned int				kista_id; // assign an unsigned id to the task
	
private:
	// auxiliary method for initializaiton after stablishing task-app link in the constructors
	void init(sc_module_name name, VOIDFPTR f, task_role_t role_par=SYSTEM_TASK); // by default, is a system task
	
	application_t* app_p;
	VOIDFPTR task_functionality;
	task_role_t role;
	
    sc_time WCET;
    unsigned long long WCEI;
    sc_time Period, Deadline;    
				// period and deadline settled detected as non-zero values

	bool WCET_set, WCEI_set;

	sc_event task_release_event;

	bool miss_deadline_flag;

    //sc_process_handle scheduler_handler;
    scheduler *sched_p;		// scheduler the task has been assigned to

	// INTERNAL DATA
	sc_process_handle this_task_handler;
	unsigned int priority;

    // Internal data for contention simulaition	
	sc_time left_consume_time;  // letft time that the task need to consume in its next resumption
	
	// Internal data for reporting utilizations
	sc_time occupation_time;
	sc_time occupation_interval_start_time; // time stamp of the previous edge in the occupation signal
	task_state_t prev_inc_state;

	// For the accounting of response times
	sc_time max_response_time_var;
	sc_time location_of_max_response_time_var;
	sc_time avg_response_time_var, dev_typ_response_time_var;
		
	// internal data for triggering of periodic tasks
	bool is_periodic_flag;
	sc_time current_absolute_deadline;
	
	// internal processes
	// FSM for updating the task state
	void update_state_proc();

	// for accounting consumption time
	void account_occupation_time_proc();
	
	// for accounting response times
	void account_response_times_proc();
	sc_event end_response_event; // event for notification to account response times process
	                             // This events allows a correct accounting when deadlines are missed
	                             // by periodic tasks and KisTA is configure to let the periodic
	                             // task go on running, instead of resynchronizing to the next deadline.

	// gen release event
	void gen_release_event();

	// methods for implementation of consume()
	void       set_left_consume_time(sc_time time);
	sc_time     &get_left_consume_time();
		
    // event and data for handling state transition of task	
	bool finished_flag;	
	
	// to perform actions at the end of simulation (e.g., add execution time if ended active)
	void end_of_simulation();
	
	void before_end_of_elaboration();
	
	void end_of_elaboration();
	
	// auxiliar function, call by dynamic priorities scheduling policies, e.g. EDF
	sc_time get_time_to_next_deadline();
	
	// method to check that the call is done before simulation start (in case not, it raises an error)
	void check_before_simulation(std::string call_name);
};


// Implementation of comparison functions for task vector structure (that is vector of task info pointers)
// ... for ordering by user priority
bool less_than_by_user_priority(task_info_t *task_info_p1, task_info_t *task_info_p2);

// ... for ordering by period
bool less_than_by_period(task_info_t *task_info_p1, task_info_t *task_info_p2);

// ... for ordering by deadline
bool less_than_by_deadline(task_info_t *task_info_p1, task_info_t *task_info_p2);

// ... for ordering by density
bool less_than_by_density(task_info_t *task_info_p1, task_info_t *task_info_p2);

} // namespace KisTA

#endif
