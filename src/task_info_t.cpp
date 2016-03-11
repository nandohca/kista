/*****************************************************************************

  task_info_t.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January

 *****************************************************************************/

#ifndef TASK_INFO_T_CPP
#define TASK_INFO_T_CPP

#include <string>

#include <math.h>
  // ceil() used by get_time_to_next_deadline

using namespace std;

#include "application_t.hpp"
#include "task_info_t.hpp"
#include "scheduler.hpp"
#include "processing_element.hpp"
#include "draw.hpp"
#include "global_elements.hpp"

namespace kista {
	

// ---------------------------------------------
// Implementation of TASK_INFO_T
// ---------------------------------------------
	
	
/*
	task_info_t::task_info_t(const char* name, VOIDFPTR f) : sc_module(name) {
		task_info_t(sc_module_name(name), f);
	}	

	task_info_t::task_info_t(string name, VOIDFPTR f) : sc_module(name) {
		task_info_t(sc_module_name(name.c_str()), f);
	}	
*/

	//   to declare a task which is assigned to the default application

	task_info_t::task_info_t(sc_module_name name, VOIDFPTR f, task_role_t role_par)
													: app_element(name,TASK_ELEMENT)
//													: sc_module(name)
	{
		std::string rpt_msg;
		if(role_par==SYSTEM_TASK) {	// stating the application visibility only refer to system tasks		
			switch(app_visibility) { // app_modelling_type in global_elements
				case UNSET_APP_VISIBILITY:
					app_visibility = SET_IMPLICIT_APPS;
					break;
				case SET_IMPLICIT_APPS:
				    // go on normaly
					break;					
				case SET_EXPLICIT_APPS:
				default:
					// previous task declarations used multiple apps style so an error condition is raised
					rpt_msg = "While creating task ";
					rpt_msg += this->name();
					rpt_msg += ". At least one task has been declared and associated to an explicitly declared application, while ";
					rpt_msg += this->name();
					rpt_msg += " task is associated to the implicit application (no association an explicit application).\n";
					rpt_msg += "You should either construct a model where all the tasks are associated to the implicit application or a model where every task is associated to a previously declared application\n";
					SC_REPORT_ERROR("KisTA", rpt_msg.c_str());									
			}
		}	
		// the pointer to the associated application is null, 
		// which is coherent with the global parameter  app_modelling_type=IMPLICIT_APP,
		// meaning that all the system tasks declared belong to the same application
		this->app_p = NULL;
		
		// initialize the remaining task info
		init(name,f,role_par);
	}


	// constructor stating the application it belongs to (an application must exist and be created before
	
	// This constructor assumes that there is at least one application declared
	// Then, it does not allows to use the default application (for clarity in 
	// the description methodology)


	task_info_t::task_info_t(sc_module_name name, VOIDFPTR f, std::string app_name, task_role_t role_par)
													: app_element(name)
													//: sc_module(name)
	{
		std::string rpt_msg;
		if(role_par==SYSTEM_TASK) {	// stating the application visibility only refer to system tasks		
			switch(app_visibility) { // app_modelling_type in global_elements
				case UNSET_APP_VISIBILITY:
					app_visibility = SET_EXPLICIT_APPS;
					break;
				case SET_IMPLICIT_APPS:
					// previous task declarations used multiple apps style so an error condition is raised
					rpt_msg = "While creating task ";
					rpt_msg += this->name();
					rpt_msg += ". At least a task was previously declared and associated to the implicit application (i.e. no associated to any app), while ";
					rpt_msg += this->name();
					rpt_msg += " task is explicitly associated to the application ";
					rpt_msg += app_name;
					rpt_msg += ".\n";
					rpt_msg += "You should either construct a model where all the tasks are associated to the implicit application or a model where every task is associated to a previously declared application\n";
					SC_REPORT_ERROR("KisTA", rpt_msg.c_str());									
					break;				
				case SET_EXPLICIT_APPS:
				default:
					break;
					// go on normaly
			}
		}
		
		// generate the default application in case it does not exist
		if(applications_by_name.size()==0) {
			rpt_msg = "While creating task ";
			rpt_msg += this->name();
			rpt_msg += ". The aplication list is empty. You should declare an application before declaring a task associated to it.";
			SC_REPORT_ERROR("KisTA", rpt_msg.c_str());
		} else {
			applications_by_name_t::iterator it = applications_by_name.find(app_name);

			if (it!=applications_by_name.end()) {
				// There is only one application declared and it is the default application
				// (we assume that a model with the default application is being constructed
				this->app_p = it->second;
				  // This updates the task_info_t structure, and links it to the corresponding application
			} else {
				rpt_msg = "While creating task ";
				rpt_msg += this->name();
				rpt_msg += ". Application";
				rpt_msg += app_name;
				rpt_msg += " not found in the application list. Please, ensure that the application is declared before the task declaration.";
				SC_REPORT_ERROR("KisTA", rpt_msg.c_str());				
			}			
			
			// Following, the application object is updated, adding to it this newly created task (by name)
			// and making effective a double link
			// here, this->app_p should be pointing to the application_t object of the app_name application		
			this->app_p->tasks_by_name[this->name()]=this;
											
			// initialize the remaining task info
			init(name,f,role_par);
		}
	}


	void task_info_t::init(sc_module_name name, VOIDFPTR f, task_role_t role_par) {	
		// refuster t
														
		task_functionality = f;
				// task_info_t elements are created as System tasks by default
		role = role_par;

		if(role == SYSTEM_TASK) {
			priority = DEFAULT_INITIAL_PRIORITY;
			consumption_signal.write(false);
			waiting_period_signal.write(false);
			waiting_comm_sync_signal.write(false);		
			state_signal.write(READY);
			occupation_time = SC_ZERO_TIME;
			occupation_interval_start_time = SC_ZERO_TIME;
			occupation_time = SC_ZERO_TIME;
			consumed_energy_J = 0.0;
				
			SC_HAS_PROCESS(task_info_t);
			
			SC_THREAD(update_state_proc); // Likely can be later converted to SC_METHOD for speed optimization
			
			SC_METHOD(account_occupation_time_proc);
			sensitive << state_signal;

			SC_THREAD(account_response_times_proc);
			
			// task release event thread
			SC_THREAD(gen_release_event);
						
			Deadline = SC_ZERO_TIME;
			Period = SC_ZERO_TIME;
			is_periodic_flag = false; // default value
			
			miss_deadline_flag = false;
			
			// initialization of state data
			active_signal.write(false);		
			finished_flag = false;
			
			WCET_set=false;
			WCEI_set=false;


		// update the global task info table by name
//cout << "Adding task " << this->name() << " to the by name dereference" << endl;
			task_info_by_name[this->name()] = this;
			this->kista_id = task_vector.size(); // assign KisTA internal id to the task
			task_vector.push_back(this);
		}
	}

	std::string task_info_t::name() {
		return app_element::name();
	}

	application_t* task_info_t::get_application() {
		return app_p;
	}
	
	VOIDFPTR &task_info_t::get_functionality() {
		return task_functionality;
	}

	void 				task_info_t::set_task_process_handler(sc_process_handle phandler) { this_task_handler = phandler; } 
	sc_process_handle	&task_info_t::get_task_process_handler()	 { return this_task_handler; } 

/*
	void				task_info_t::set_scheduler(scheduler *scheduler_p)  { sched_p = scheduler_p; } 
	scheduler          *task_info_t::get_scheduler() { return sched_p; }
*/

	task_role_t& task_info_t::get_role() {
		return this->role;
	}

	bool task_info_t::is_system_task() {
		return (role==SYSTEM_TASK);
	}

	bool task_info_t::is_env_task() {
		return (role==ENV_TASK);
	}

	void task_info_t::set_WCET(sc_time t) {
		string msg;
		check_before_simulation("set_WCET");
	    WCET = t;
		WCET_set = true;		    
	}

	void task_info_t::set_WCEI(unsigned long long wcei_var) {
		string msg;
		check_before_simulation("set_WCEI");
	    WCEI = wcei_var;
	    WCEI_set = true;
	}
	
	void task_info_t::set_Period(sc_time t) {
		string msg;
		
		check_before_simulation("set_Period");
		
		if(t==SC_ZERO_TIME) {
			msg = "Setting Period of task ";
			msg += name();		
			msg = " to 0.";
			SC_REPORT_ERROR("KisTA", msg.c_str());
		}		
		
 	    Period = t; // Period > 0s serves already as flaf of period settled periodic task
		is_periodic_flag = true;

		if(Deadline == SC_ZERO_TIME) { // if there is no deadline
			Deadline = Period; // It stablishes and INTRINSIC deadline (Davis&Burns '09)
		} 
		// otherwise, deadline previously stablished is hold
	}
	
	void task_info_t::set_Deadline(sc_time t) {
		string msg;

		check_before_simulation("set_Deadline");
		
		if(t==SC_ZERO_TIME) {
			msg = "Setting user deadline of task ";
			msg += name();
			msg = "to 0.";
			if(is_periodic()) {
				msg += "The task is periodic, so the task period (";
				msg += Period.to_string();
				msg += ") is adopted as deadline";
			} else {
				msg += "The task is not periodic, so task so deadline is unsettled.";
			}
			SC_REPORT_WARNING("KisTA", msg.c_str());
		}	

		if((t>Period) && is_periodic()) {
#ifdef _ALLOW_ARBITRARY_DEADLINES				
			msg = "Stablishing a Deadline=";
#else
			msg = "Trying to stablish a Deadline=";
#endif				
			msg += t.to_string();
			msg += " greater than the Period=";
			msg += get_period().to_string();
			msg += " for periodic task ";
			msg += name();
#ifdef _ALLOW_ARBITRARY_DEADLINES
			SC_REPORT_WARNING("KisTA", msg.c_str() ); // In this option arbitrary deadline is allowed but a warning is yet launched
#else
			SC_REPORT_ERROR("KisTA", msg.c_str() ); // In this option arbitrary deadline are not allowed
#endif
		}
		Deadline = t;
	}
	
	void task_info_t::set_priority(unsigned int pr) {
		string msg;
		msg = "Priority of task ";
		msg += name();
		check_before_simulation("set_priority");
		if(pr==0) {
			msg = " user priority cannot be 0 (value reserved).";
			SC_REPORT_ERROR("KisTA", msg.c_str());
		}		
		priority = pr;
	}
	

	sc_time				&task_info_t::get_WCET() { return WCET; }
	
	unsigned long long	&task_info_t::get_WCEI() { return WCEI; }
	
	sc_time				&task_info_t::get_period() { return Period; }
	
	sc_time				&task_info_t::get_deadline() { return Deadline; }
	
	bool				task_info_t::is_periodic() {
		  return is_periodic_flag;
		 // return (Period>SC_ZERO_TIME);
	}
		
	bool				task_info_t::deadline_settled() { return (Deadline>SC_ZERO_TIME); }
	
	unsigned int		&task_info_t::get_priority() { return priority; }
	
	double task_info_t::utilization() {
		 string msg;
		 if(is_periodic()) {
			return WCET/Period;
		 } else {
 			msg = "No period has been settled for task ";
			msg += name();	 
			msg += ". Therefore, utilization cannot be statically calculated.";
            SC_REPORT_ERROR("KisTA", msg.c_str());
            return 0.0; // To avoid warnings
		 }
	}
	
	double task_info_t::density() {
		 string msg;
		 if(deadline_settled()) {
//cout << name() << " WCET = " << WCET << " Deadline = " << Deadline << endl;
			return WCET/Deadline;
		 } else {
 			msg = "No deadline has been settled for task ";
			msg += name();	 
			msg += ". Therefore, density cannot be statically calculated.";
            SC_REPORT_ERROR("KisTA", msg.c_str());
            return 0.0; // To avoid warnings
		 }
    }
	
	// Calls for retrieving report information of the task
	// (normally called at the end of simulation, but it can make sense to call them during simulation too)
	 // returns true if some deadline was missed	
	bool	task_info_t::miss_deadline() {
		return miss_deadline_flag;
	}
	
	// for implementation
	void        task_info_t::set_left_consume_time(sc_time time) { left_consume_time = time; }
	sc_time      &task_info_t::get_left_consume_time() { return left_consume_time; }	


	// ---------------------------------------------------
	// TASK INFO PROCESSES IMPLEMENTATION
	// - they are not activated for environment tasks
	// ----------------------------------------------------


	// for accounting consumption times
	//  SC_METHOD implementation
	void task_info_t::account_occupation_time_proc() {
		processing_element *PE = NULL;
		
		if(this->is_system_task()) {
			PE = this->get_scheduler()->get_PE();		
		}
		
		if(state_signal.read()=='E') { // executing, the task wants to execute (computation signal)
		                               // and as well, one CPU is granted
			occupation_interval_start_time = sc_time_stamp();
#ifdef _PRINT_ACCUMULATION_OF_CONSUMPTION_TIMES
			  cout << "Ocupation Account Task for " << name();
			  cout << " passes to E " ;
			  cout << " at " << sc_time_stamp() << endl;			  
#endif	
			prev_inc_state = EXECUTING;
		} else { // stops execution
		    if(prev_inc_state==EXECUTING) {
			  //
			  // Updates the occupation time of the task 
			  // at the end of a PE occupation interval
			  //
			  occupation_time += (sc_time_stamp() - occupation_interval_start_time);
#ifdef _PRINT_ACCUMULATION_OF_CONSUMPTION_TIMES
			  cout << "Ocupation Account Task for " << name();
			  cout << " accumulates " << (sc_time_stamp() - occupation_interval_start_time);
			  cout << " for a total of " << occupation_time;
			  cout << " at " << sc_time_stamp() << endl;			  
#endif
			  //
			  // Updates the consumed energy
			  // at the end of a PE occupation interval
			  //
			  consumed_energy_J = occupation_time.to_seconds() * PE->get_peak_dyn_power_W();
			                     // remind that get_peak_dyn_power_W is the power consumption
			                     // calculated for the instruction time (modelled as constant in KisTA)
#ifdef _PRINT_TASK_ENERGY_ACCOUNTING
			  cout << "\t Task \"" << name();
			  cout << "\" consuming " << consumed_energy_J ;
			  cout << " J at " << sc_time_stamp() << endl;			  
#endif		                     
			}
		    prev_inc_state = state_signal.read();
		}
	}
	
	// for accounting response times
	//  SC_METHOD implementation
 	void task_info_t::account_response_times_proc() {
		
		sc_time current_response_time;
		sc_time last_release_time;
		
		max_response_time_var = SC_ZERO_TIME;

//	Take into account when introducing init phase delay!!		
//		wait(init_phase_delay);
		
		last_release_time = sc_time_stamp();

		// Assuming task starts in 0 phase
		while(true) {

//			do {
//				wait( state_signal.default_event() );
//			} while ( !( (state_signal.read()==WAIT_PERIOD) || (state_signal.read()==INACTIVE_WAIT_PERIOD) ) );
// This has the issue that it does not work porperly when we configure KisTA to let
// periodic tasks to go on running when a deadline is missed.
//
// Then, the periodic task makes no wait and the task does not stop. Therefore, the
// wait period signal does not goes up, so nor the state of the task foes to WAIT_PERIOD or INACTIVE_WAIT_PERIO
// However, it can be considered that a reponse has been completed.
//
// Therefore, for accounting purporses, an additional and specific event (end_response_event) is used,
// notified by the task it self when the wait_period function is called
//
			wait(end_response_event);
			
//cout << name() << " t =" << sc_time_stamp() << " account response" << endl;
			current_response_time = sc_time_stamp() - last_release_time;
			
			if(current_response_time > max_response_time_var) {
				max_response_time_var = current_response_time;
				location_of_max_response_time_var = last_release_time;
			}
			
			wait( task_release_event );			

			last_release_time = sc_time_stamp();
			
		}
	}
	
	
	sc_time		task_info_t::max_response_time() {
		string msg;
		
		if(!is_periodic()) {
			msg = "Reading worst case response time is supported for periodic tasks.";
			msg += name();
			msg += " is not periodic. Check its declaration.";
			SC_REPORT_ERROR("KisTA", msg.c_str());			
		}
		
		if(sc_get_status()&SC_STOPPED) {	
			return max_response_time_var;
		} else {	
						
			if(sc_is_running()) {
#ifdef _ENABLE_WARNINGS_WHEN_MEASURED_RESPONSE_GOT_DURING_SIMULATION				
				msg = "Reading worst case response time of task ";
				msg += name();
				msg += " during the simulation (before it finishes).";
				SC_REPORT_WARNING("KisTA", msg.c_str());
#endif			
				return max_response_time_var;
			} else {	
				msg = "Reading worst case response time of task ";
				msg += name();
				msg += " before simulation start. 0 time will be reported";
				SC_REPORT_WARNING("KisTA", msg.c_str());
				return SC_ZERO_TIME;
			}	
		}
		
	}
	
	sc_time		task_info_t::worst_case_response_time() {
		return max_response_time();
	}
	
	sc_time		task_info_t::location_of_worst_case_response_time() {
		string msg;
		
		if(!is_periodic()) {
			msg = "Reading location of worst case response time is supported for periodic tasks.";
			msg += name();
			msg += " is not periodic. Check its declaration.";			
			SC_REPORT_ERROR("KisTA", msg.c_str());			
		}
		
		if(sc_get_status()&SC_STOPPED) {	
			return location_of_max_response_time_var;
		} else {					
			if(sc_is_running()) {
#ifdef _ENABLE_WARNINGS_WHEN_MEASURED_RESPONSE_GOT_DURING_SIMULATION				
				msg = "Reading location of worst case response time of task ";
				msg += name();
				msg += " during the simulation (before it finishes).";
				SC_REPORT_WARNING("KisTA", msg.c_str());
#endif			
				return location_of_max_response_time_var;
			} else {	
				msg = "Reading location of worst case response time of task ";
				msg += name();
				msg += " before simulation start. 0 time will be reported";
				SC_REPORT_ERROR("KisTA", msg.c_str());
				return SC_ZERO_TIME;
			}	
		}		
	}
	
	sc_time		task_info_t::avg_response_time() {
		SC_REPORT_ERROR("KisTA", "Avg. response times not implemented yet");
		return sc_time(0,SC_SEC);
	}
	
	sc_time		task_info_t::std_dev_response_time() {
		SC_REPORT_ERROR("KisTA", "std_dev_response_time not implemented yet");
		return sc_time(0,SC_SEC);		
	}

	void task_info_t::update_state_proc() {
	
		string msg; // for error reporting
		
	    // at the beginning we are in the ready state (done at initialization) and
	    // only the active signal can trigger the process
		wait(active_signal.posedge_event());

		while(true) {
/*
// Debug printouts, to remove in release
cout << sc_time_stamp() << endl;			
if(active_signal.read()) cout << "active_Signal = TRUE" << endl;
else cout << "active_signal = FALSE" << endl;

if(consumption_signal.read()) cout << "consumption_signal = TRUE" << endl;
else cout << "consumption_signal = FALSE" << endl;

if(finished_flag) cout << "finished_flag = TRUE" << endl;
else cout << "finished_flag = FALSE" << endl;
*/

#ifdef _PRINT_TASK_STATE_TRANSITION
			cout << "(" << sc_time_stamp() << " ) Task " << name() << ": From state: " << state_signal;			
#endif	
			switch(state_signal) {
				// ----------------------------------------------------------
				// Active states
				// ----------------------------------------------------------
				case EXECUTING:
				    // sensitivity to suspend and yield
					//if(finished_flag) {					
					if ( waiting_period_signal.read() && active_signal.read()) {	
						// transition to active waiting for period
						get_scheduler()->running_task_to_wait_state_event.notify(SC_ZERO_TIME);
						state_signal.write(WAIT_PERIOD);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "WAIT_PERIOD." << endl;			
#endif						
						wait(waiting_period_signal.negedge_event() | active_signal.negedge_event());
					}
					else if ( waiting_period_signal.read() && active_signal.read()==false) { 					
						// direct transition to inactive waiting for period (active signal does down at the same time as waitint_period_signal raises)
						get_scheduler()->running_task_to_wait_state_event.notify(SC_ZERO_TIME);
						state_signal.write(INACTIVE_WAIT_PERIOD);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "INACTIVE_WAIT_PERIOD." << endl;		
#endif
						wait(active_signal.posedge_event() |  waiting_period_signal.negedge_event());
					}
					else if ( waiting_comm_sync_signal.read() && active_signal.read()) { 					
						// transition to active waiting for communication or synchronization
						get_scheduler()->running_task_to_wait_state_event.notify(SC_ZERO_TIME);
						state_signal.write(WAIT_COMM_SYNC);		
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "WAIT_COMM_SYNC." << endl;		
#endif
						wait(waiting_comm_sync_signal.negedge_event() | active_signal.negedge_event());				
					}
					else if ( waiting_comm_sync_signal.read() && active_signal.read()==false) { 					
						// transition to active waiting for communication or synchronization (active signal does down at the same time as waitint_period_signal raises)
						get_scheduler()->running_task_to_wait_state_event.notify(SC_ZERO_TIME);
						state_signal.write(INACTIVE_WAIT_COMM_SYNC);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "INACTIVE_WAIT_COMM_SYNC." << endl;		
#endif						
						wait(waiting_comm_sync_signal.negedge_event() | active_signal.posedge_event());
					}
					else if(finished_flag && (consumption_signal.read()==false) ) {
						// here, waiting period signal and waiting_comm_sync_signal are false...
											// THE TASK YIELD CPU and task is ended
											// (it assumes that scheduler will inactivate the signal too)
						get_scheduler()->completion_event.notify(SC_ZERO_TIME);
						state_signal.write(FINISHED);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "FINISHED." << endl;	
#endif
#ifdef _PRINT_TASK_COMPLETION
						cout << " Task " << name() << " completes at " << sc_time_stamp() << endl;
#endif							
						wait(); // The task no longer needs to be triggered
						SC_REPORT_ERROR("KisTA", "Tasks resumed after having reached FINISHED state. Please, contact the authors and pose the example.");
					}										
					else if (
					    // here, waiting period signal and waiting_comm_sync_signal are false...
					    // ... the tasks has not completed, but has stop consuming and ...
					    // ... case a)
					    // ...     the task ends consuming (e.g. does a yield) but is is not the end of the task
					    ( (active_signal.read()==true) && (consumption_signal.read()==false)  )
					    // or
					    ||
					    // ... case b)
					    // ...the task is active and wants go on executing, but the scheduler
					    // preempts the task, that is, removed it the processor resource
					    (active_signal.read()==false)
						) {						
						// goes to ready when the task either
						//     yields the processor, consumption ends because reaches a 							
						state_signal.write(READY);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "READY." << endl;		
#endif					
						wait(active_signal.posedge_event());
					} // for the rest of options active_signal.read()==true, as well as waiting period signal and waiting_comm_sync_signal are false..			
					 else {
						SC_REPORT_ERROR("KisTA","Unexpected transition from -Execution- task state. Please, contact the authors.");
					}
				
					break;		

				case WAIT_PERIOD: // Active Waiting state
					
					if(waiting_period_signal.read()==false) {
						state_signal.write(EXECUTING);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "EXECUTING." << endl;		
#endif						
						wait(active_signal.negedge_event() | consumption_signal.negedge_event()
						    | waiting_period_signal.posedge_event() | waiting_comm_sync_signal.posedge_event());
					} else if(active_signal.read()==false) {
						state_signal.write(INACTIVE_WAIT_PERIOD);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "INACTIVE_WAIT_PERIOD." << endl;		
#endif						
						wait(active_signal.posedge_event() |  waiting_period_signal.negedge_event());
					} else {
						msg = "Unexpected transition from EXECUTION state for task ";
						msg += name();
						msg += " at time ";
						msg += sc_time_stamp().to_string();
						SC_REPORT_ERROR("KisTA", msg.c_str());
					}
						
					break;
					
				case WAIT_COMM_SYNC:
				
					if(waiting_comm_sync_signal.read()==false) {
						state_signal.write(EXECUTING);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "EXECUTING." << endl;		
#endif						
						wait(active_signal.negedge_event() | consumption_signal.negedge_event()
						    | waiting_period_signal.posedge_event() | waiting_comm_sync_signal.posedge_event());
					} else if(active_signal.read()==false) {
						state_signal.write(INACTIVE_WAIT_COMM_SYNC);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "INACTIVE_WAIT_COMM_SYNC." << endl;		
#endif						
					    wait(waiting_comm_sync_signal.negedge_event() | active_signal.posedge_event());

					} else {
						SC_REPORT_ERROR("KisTA","Unknown triggering event in COMMUNICATING state for task.");
					}
										
					break;			
									
				// ----------------------------------------------------------
				// Inactive states
				// ----------------------------------------------------------
				case READY: // intial state
				    // reached only after the sched>resume event! (sensitivity list in the method)
					if(active_signal.read()) {
						state_signal.write(EXECUTING);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "EXECUTING." << endl;		
#endif						
						wait(active_signal.negedge_event() | consumption_signal.negedge_event()
						    | waiting_period_signal.posedge_event() | waiting_comm_sync_signal.posedge_event());
					}
					break;		
							
				case INACTIVE_WAIT_PERIOD:
										
					if( (waiting_period_signal.read()==false) && active_signal.read() ) { // doble concurrent transition
						                                                                    // waiting ends and task is made active at the same time
						state_signal.write(EXECUTING);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "EXECUTING." << endl;		
#endif						
						wait(active_signal.negedge_event() | consumption_signal.negedge_event()
						    | waiting_period_signal.posedge_event() | waiting_comm_sync_signal.posedge_event());
					} else if((waiting_period_signal.read()==false) && (active_signal.read()==false) ) {  // waiting ends, but task remains inactive
						state_signal.write(READY);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "READY." << endl;		
#endif						
						get_scheduler()->innactive_task_ends_wait_event.notify(SC_ZERO_TIME);
						wait(active_signal.posedge_event());
					} else if(active_signal.read() && waiting_period_signal.read()) { // task is activated, but still waiting for period
						state_signal.write(WAIT_PERIOD);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "WAIT_PERIOD." << endl;		
#endif						
						wait(waiting_period_signal.negedge_event() | active_signal.negedge_event());						
					} else {
						SC_REPORT_ERROR("KisTA","Unknown transition from INACTIVE WAITING state for task.");
					}
					
					break;				
					
				case INACTIVE_WAIT_COMM_SYNC:
				
					if( (waiting_comm_sync_signal.read()==false) && active_signal.read() ) { // doble concurrent transition
					                                                                      // comm./sunch. ends and task is made active at the same time
						state_signal.write(EXECUTING);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "EXECUTING." << endl;		
#endif						
						wait(active_signal.negedge_event() | consumption_signal.negedge_event()
						    | waiting_period_signal.posedge_event() | waiting_comm_sync_signal.posedge_event());
					} else if(waiting_comm_sync_signal.read()==false) {  // comm./synch. ends, but task remains inactive 
						state_signal.write(READY);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "READY." << endl;		
#endif						
						get_scheduler()->innactive_task_ends_wait_event.notify(SC_ZERO_TIME);
						wait(active_signal.posedge_event());
					} else if(active_signal.read() && waiting_comm_sync_signal.read()) { // task is activated, but still waiing for communication or synchronization
						state_signal.write(WAIT_COMM_SYNC);
#ifdef _PRINT_TASK_STATE_TRANSITION
						cout << " to: " << "WAIT_COMM_SYNC." << endl;		
#endif						
						wait(waiting_comm_sync_signal.negedge_event() | active_signal.negedge_event());
					} else {
						SC_REPORT_ERROR("KisTA","Unknown transition from INACTIVE COMMUNICATION/SYNCHRONIZATION state for task.");
					}
					
					break;									
					
				case FINISHED:
					// sink state: actually, this code should never be reached,
					// it can be reached if resume_event is notified. The report detects such an anomalous situation
					msg = "Task ";
					msg += name();
					msg += " trying to execute after having reached FINISHED state, at time ";
					msg += sc_time_stamp().to_string();
					SC_REPORT_ERROR("KisTA", msg.c_str());
					break;							
					
				default:
					SC_REPORT_ERROR("KisTA", "Task switching to Unknown state.");
			}
		}
	}

	void task_info_t::end_of_simulation() {
		processing_element *PE = NULL;
		
		if(this->is_system_task()) {
			PE = this->get_scheduler()->get_PE();		
		}		
		// If the task ends on Executing state, the last piece of execution
		// has not been accounted (since the accounting takes place from the
		// transition of Exection state to other state)
		// This piece of code adds that contribution
		if(
		    // sc_stop arrives in the middle of the execution of a task (the scheduler is not active)
		    ( (state_signal.read()==EXECUTING) && (prev_inc_state==EXECUTING) && (get_scheduler()->state_signal.read()==INNACTIVE) )
			                            // (prev_inc_state==EXECUTING) is required to ensure that the task was already executing before
			                            // the instant of the end of simulation otherwise it might happen, (and happened in one experiment!)
			                            // that the simulation finishes just when the state of a task is moved and then accounted 
			                            // as an extra time from the previous execution start, which is actually an error
			                            ||
			// the user calls sc_start(), without specifying a strict asynchronous end limit of the simulation,
			// which is detected through the global time limit by a scheduler if the simulation time is over that limit,
			// Then, the sc_stop is detected by any scheduler.
			// In such a case, the last task passes from READY to EXECUTING
			// but with the current structure of KisTA it is not accounted, by account_occupation_time_proc, because
			// the last delta executed, when scheduler detects simulation time limit and performs the sc_stop, 
			// the task FSM produces the transition from EXECUTING to READY. However, that event will be detected and accounted
			// by ocupation time account task in the next delta, which is not goint to be executed.
			// Because of that, such an accumulation is done by detecting that the scheduler is active, and that the transition
			// from Execution was about to be done now.
			( (state_signal.read()==READY) && (prev_inc_state==EXECUTING) && (get_scheduler()->state_signal.read()==SCHEDULING) )
		) { 
			   // Last update of occupation time
			  occupation_time += (sc_time_stamp() - occupation_interval_start_time);
#ifdef _PRINT_ACCUMULATION_OF_CONSUMPTION_TIMES
			  cout << "Task " << sc_get_current_process_handle().name();
			  cout << " accumulates " << (sc_time_stamp() - occupation_interval_start_time);
			  cout << " for a total of " << occupation_time;
			  cout << " at " << sc_time_stamp() << " (last update at end of simulation) " << endl;
			  
			  cout << "Estado previo: " << prev_inc_state << endl;
#endif

			  //
			  // Updates the consumed energy
			  // at the end of a PE occupation interval
			  //
			  consumed_energy_J = occupation_time.to_seconds() * PE->get_peak_dyn_power_W();
			                     // remind that get_peak_dyn_power_W is the power consumption
			                     // calculated for the instruction time (modelled as constant in KisTA)
#ifdef _PRINT_TASK_ENERGY_ACCOUNTING
			  cout << "\t Task \"" << name();
			  cout << "\" consuming " << consumed_energy_J ;
			  cout << " J at " << sc_time_stamp() << " (last update at end of simulation) " << endl;			  
#endif	
		}
	}
	
	void task_info_t::before_end_of_elaboration() {
		// If sketch report enabled, this task is dumped to the tikz sketch file
		if(sketch_report.is_enabled()) {
			sketch_report.draw(this);
		}
	}
	
	void task_info_t::end_of_elaboration() {
		processing_element *PE;
		
		if(this->is_system_task()) {
			PE = this->get_scheduler()->get_PE();
	//cout << "Elaboration of sched: " << name() << endl;
			if(!WCET_set && WCEI_set) { // WCET has not been settled (so set_WCEI does not overides set_WCET) and WCEI has been settled
				// calculate WCET from WCEI, given the settled mapping, which states the processor, and thus the CPI and frequency
				WCET = sc_time((double)(WCEI*PE->get_CPI()*PE->get_clock_period_ns()), SC_NS);
	//cout << "Calculaton of WCET for sched: " << name() << endl;			
	//cout << "CPI: " << PE->get_CPI() << endl;
	//cout << "clock period ns: " << PE->get_clock_period_ns() << endl;
	//cout << "WCEI: " << WCEI << endl;
	//cout << "WCET: " << WCET << endl;
			} 
			
			if(is_periodic_flag) {
				if(Deadline==SC_ZERO_TIME ) { // In case the task is periodic and no deadline distinct from 0 has been settled
					current_absolute_deadline = Period; // first deadline is period if the task is periodic 
				} else {
					current_absolute_deadline = Deadline;
				}
			}
		}
	}

	sc_time task_info_t::get_time_to_next_deadline() {
		return ( ceil( sc_time_stamp() / Period ) * Period - sc_time_stamp() );
	}

	void task_info_t::gen_release_event() {
#ifdef 	_PRINT_TASK_RELEASE_EVENTS
		long unsigned int i =0;
#endif

        if (is_periodic_flag)	 {	// executed at simulation time, when is periodic flag is stable
          
          // task phase can be adde here
          
		  while(true) {
			wait(Period);  
			task_release_event.notify(SC_ZERO_TIME);
#ifdef 	_PRINT_TASK_RELEASE_EVENTS			
cout << " Release event " << i << " (task " << name() << ") : at time "	<< sc_time_stamp() << endl;
			i++;
#endif
		  }
		  
		}
	}
	
	void task_info_t::check_before_simulation(std::string call_name) {
		std::string msg;
		if ( sc_get_status()&(SC_START_OF_SIMULATION|SC_RUNNING|SC_STOPPED|SC_PAUSED|SC_END_OF_SIMULATION) ) {
			msg = "Call to '";
			msg += call_name;
			msg += "' for task ";
			msg += name();
			msg += " has to be done before the simulation start.";
			SC_REPORT_ERROR("KisTA",msg.c_str());
		}
	}	
	
	const double &task_info_t::get_consumed_energy_J() {
		check_call_after_sim_start("get_consumed_energy_J");
		return consumed_energy_J;
	}
	
	// Implementation of comparison functions for task vector structure (that is vector of task info pointers)
	// ... for ordering by user priority
    bool less_than_by_user_priority(task_info_t *task_info_p1, task_info_t *task_info_p2)
	{       		
		return (task_info_p1->get_priority() < task_info_p2->get_priority());
	}
	
	// ... for ordering by period
    bool less_than_by_period(task_info_t *task_info_p1, task_info_t *task_info_p2)
	{       		
		return (task_info_p1->get_period() < task_info_p2->get_period());
	}	

	// ... for ordering by deadline
    bool less_than_by_deadline(task_info_t *task_info_p1, task_info_t *task_info_p2)
	{       		
		return (task_info_p1->get_deadline() < task_info_p2->get_deadline());
	}	

	// ... for ordering by density
    bool less_than_by_density(task_info_t *task_info_p1, task_info_t *task_info_p2)
	{       		
		return (task_info_p1->density() > task_info_p2->density()); // Notice the change of the sign!, since the bigger the density the more prioritary
	}
	
	
	
} // namespace KisTA

#endif
