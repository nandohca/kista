/*****************************************************************************

  scheduler.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January
 
  Last update: 2014 May, hooks for static analysis

 *****************************************************************************/
#ifndef SCHEDULER_CPP
#define SCHEDULER_CPP

#include <string>
#include <algorithm>   // std::sort

#include "global_elements.hpp"

#include "scheduler.hpp"
#include "scheduler_penalties.hpp"

#include "processing_element.hpp"
#include "draw.hpp"
#include "utils.hpp"

#include "math.h"

using namespace std;
namespace kista {
	
	// raw constructor
	scheduler::scheduler(
					// scheduler name
					sc_module_name sched_name,					
					// properties
					sc_time ttimer_delay,
					bool time_slicing_flag,						
		        	scheduler_triggering_policy_t scheduler_triggering_policy_par,
					scheduler_policy_t scheduler_policy_par
											) : sc_module(sched_name) {
#ifdef _PRINT_SCHEDULERS_ACTIVITY
		cout << "Scheduler " << name() << ": construction" << endl;
#endif

		// global scheduler process
		SC_THREAD(scheduler_proc);
		sched_phandler =  sc_get_current_process_handle(); // get handler of scheduler process
				
		tick_timer_delay = ttimer_delay;
		last_tick = SC_ZERO_TIME;
			
		scheduler_triggering_policy = scheduler_triggering_policy_par;
			
		t_slicing_flag = time_slicing_flag;
			
		scheduler_policy = scheduler_policy_par;
		static_priorities_policy = USER_PRIORITIES;
        dynamic_priorities_policy = EARLIEST_DEADLINE_FIRST;
			
		// If wanted, we can control from here not to initialize the processes
			
		// initialization of variables for control of simulation from scheduler
		sufficient_global_sim_time_factor = 0;
			
		//spawn_options.dont_initialize(); // Otherwise tasks will start to run at the beginning
										// they will leave the scheduler starved

		consumption_signal.write(false); // by default, the scheduler starts computing
		state_signal.write(INNACTIVE); // by default, the scheduler starts computing, that is, scheduling
										// note: currently, only if there are assigned tasks
										//       the local scheduler will start scheduling

		// init sched. occupation
		sched_comp_time = SC_ZERO_TIME;
		n_schedulings = 0;
		n_context_changes = 0;
			
		// creation of computing resources
		PE = NULL; // At construction time there is no processing element assigned
			
		// tick timer thread
		SC_THREAD(gen_tick_timer_event);
			
		// Scheduling time
		scheduling_time = SC_ZERO_TIME;
		scheduling_time_set_flag = false;
		scheduling_time_calculator = NULL;
		
		//scheduler trace file
		scheduler_trace_file = NULL; // initialization of the scheduler trace file
		                             // this ensures also that the unwinding actions of the scheduler class do not try to close a
		                             // trace file never oppenened.
	    
		// updates the global sched elements table accessed by name
		std::string sched_name_std_str;
		sched_name_std_str = sched_name;
		scheds_by_name[sched_name_std_str]=this;

		// default common static analysis
		common_static_analysis = full_utilization_bound;

	}
	
	
	// compact constructor
	scheduler::scheduler(
					// assigned task list structure
					taskset_by_name_t *tasks_assigned_p,
					// scheduler name
					sc_module_name sched_name,					
					// properties
					sc_time ttimer_delay,
					bool time_slicing_flag,						
		        	scheduler_triggering_policy_t scheduler_triggering_policy_par,
					scheduler_policy_t scheduler_policy_par
											) : sc_module(sched_name) {
#ifdef _PRINT_SCHEDULERS_ACTIVITY
		cout << "Scheduler " << name() << ": construction" << endl;
#endif

		// global scheduler process
		SC_THREAD(scheduler_proc);
		sched_phandler =  sc_get_current_process_handle(); // get handler of scheduler process
			
		tasks_assigned = tasks_assigned_p;
				
		tick_timer_delay = ttimer_delay;
		last_tick = SC_ZERO_TIME;
			
		scheduler_triggering_policy = scheduler_triggering_policy_par;
			
		t_slicing_flag = time_slicing_flag;
			
		scheduler_policy = scheduler_policy_par;
		static_priorities_policy = USER_PRIORITIES;
        dynamic_priorities_policy = EARLIEST_DEADLINE_FIRST;
			
		// If wanted, we can control from here not to initialize the processes
			
		// initialization of variables for control of simulation from scheduler
		sufficient_global_sim_time_factor = 0;
			
		//spawn_options.dont_initialize(); // Otherwise tasks will start to run at the beginning
										// they will leave the scheduler starved

		consumption_signal.write(false); // by default, the scheduler starts computing
		state_signal.write(INNACTIVE); // by default, the scheduler starts computing, that is, scheduling
										// note: currently, only if there are assigned tasks
										//       the local scheduler will start scheduling

		// init sched. occupation
		sched_comp_time = SC_ZERO_TIME;
		n_schedulings = 0;
		n_context_changes = 0;
			
		// creation of computing resources
		PE = NULL; // At construction time there is no processing element assigned
			
		// tick timer thread
		SC_THREAD(gen_tick_timer_event);
			
		// Scheduling time
		scheduling_time = SC_ZERO_TIME;
		scheduling_time_set_flag = false;
		scheduling_time_calculator = NULL;
		
		//scheduler trace file
		
		scheduler_trace_file = NULL; // initialization of the scheduler trace file
		                             // this ensures also that the unwinding actions of the scheduler class do not try to close a
		                             // trace file never oppenened.

		// updates the global sched elements table accessed by name
		std::string sched_name_std_str;
		sched_name_std_str = sched_name;
		scheds_by_name[sched_name_std_str]=this;
		
		// default common static analysis
		common_static_analysis = full_utilization_bound;		
	}

	 // -----------------------------------------------------------
	 // DISPATCHER functionality
	 // -----------------------------------------------------------
	  
	 task_info_t* scheduler::dispatch() {
		  
		// cycle executive: FIFO policy, circular buffer (fair rotation of tasks)  
		task_info_t *return_task_info;
		
		// variables for dynamic EDF scheduling dispatching policy
		sc_time curr_shortest_time_to_next_deadline;
		task_info_t   *curr_task_info; // this variable facilitates code readibility (it should be easily handled by optimization switches)
						
		  // For speed following code assumes that there is at least one assigned task
		  // which is checked at elaboration time, instead of at each dispatching
		  // So, as long as the checkings are correct, return_task_info should never return NULL
		  // and always a valid task info pointer

		switch(scheduler_policy) {
			
		// Static cases
					
			case STATIC_SCHEDULING:
				return_task_info = static_schedule_by_task_info_pointers[it_static_sched];				
				it_static_sched++;
				if(it_static_sched==static_schedule_by_task_info_pointers.size()) it_static_sched = 0;				
				break;
								
			case CYCLE_EXECUTIVE:
			
			    // Notice that this algorithm alternates decisions whenever there is a scheduling
			    // (It does not considers if there has been some consumption from the task)
			    
			    // returns the next tasks in the list, as they where assigned in READY state
			    // In case there is no task READY, it returns the next in the list
				
				// current
				for(unsigned int i=0; i<tasks_assigned->size(); i++) { // maximum 1 round!		
					return_task_info = it_dyn_sched->second;
					if( (return_task_info->state_signal.read() == READY) 
					     ) {	
						// udpate next iterator (circular semantics)
						it_dyn_sched++;
						if(it_dyn_sched==tasks_assigned->end()) {
							it_dyn_sched=tasks_assigned->begin();
						} 
						break;
					} else {
						it_dyn_sched++;
						if(it_dyn_sched==tasks_assigned->end()) {
							it_dyn_sched=tasks_assigned->begin();
						} 
					}
				}
			
				break;
				
		// Dynamic cases
				
			case STATIC_PRIORITIES:
			    // returns the first tasks (in order of priority and which is in READY state
			    // In case there is no task READY, it returns the most prioritary

				// Iterator for the tasks list ordered by priorities
				// static tasks_by_priority_t::iterator tbp_it;
				unsigned int tbp_it;

				// if no task is found ready, the most prioritary is returned
				return_task_info = tasks_by_priority[0];
									
				for(tbp_it=0; tbp_it != tasks_by_priority.size(); tbp_it++) {
					
					/* if( (tasks_by_priority[tbp_it]->state_signal==READY) ||
					    (tasks_by_priority[tbp_it]->state_signal==EXECUTING)
							// in principle, any task is not waiting for communicaiton of period should be ready.
							// This is also true for a task executing and preempted by an scheudling event,
							// since onces the scheduler starts, it passess the task to READY
							//  However, if the scheduling time is 0, the scheduler task will still see
							// the state signal as 
					   )
					   */
					if( tasks_by_priority[tbp_it]->state_signal==READY)	
					{ 
						return_task_info = tasks_by_priority[tbp_it];
						break;
					}
				}

				break;

			case RANDOM_SCHEDULING:
				SC_REPORT_ERROR("KisTA","RANDOM scheduling not implemented yet.");
				break;

			case PSEUDO_RANDOM_SCHEDULING:
				SC_REPORT_ERROR("KisTA","PSEUDO-RANDOM scheduling not implemented yet.");
				break;
				
			case DYNAMIC_PRIORITIES:
						
				switch(dynamic_priorities_policy) {
					case EARLIEST_DEADLINE_FIRST:
						// returns the active task with an earliest deadline
						// among ready tasks.
						// If there is no ready tasks, the one with the earliest
						// deadline is returned.
						bool first_ready_task_found;
						
						it_dyn_sched = tasks_assigned->begin();

						do {
							curr_task_info = it_dyn_sched->second;
							
							if(curr_task_info->state_signal.read() == READY) { // found ready task
#ifdef _PRINT_EDF_DISPATCHING_INFO
									cout << "Scheduler " << name() << " dispatcher: Ready task " <<  curr_task_info->name() << " time_to_deadline: " << curr_task_info->get_time_to_next_deadline() << endl;
#endif							
								if(first_ready_task_found) {
									if( curr_shortest_time_to_next_deadline > curr_task_info->get_time_to_next_deadline() ) {
											return_task_info = curr_task_info;
									}						
								} else { // executed the first time a ready task is found
									curr_shortest_time_to_next_deadline = curr_task_info->get_time_to_next_deadline();
									first_ready_task_found = true;
									return_task_info = curr_task_info;
								}
							}
							it_dyn_sched++;
						} while(it_dyn_sched!=tasks_assigned->end()); // maximum 1 round!
				
						if(first_ready_task_found) { // A ready task was found at least, so the decision has been already made
							break;
						} else  { // No ready task found, so we search again the task with the shortest time to deadline
						           // among all the tasks
						           // (we do no integrate this search among the previous one because we supose that most of
						           // the times a task will be found, so we prefer to make that search fast
						           
							it_dyn_sched = tasks_assigned->begin();
							curr_task_info = it_dyn_sched->second;
							curr_shortest_time_to_next_deadline = curr_task_info->get_time_to_next_deadline();
							return_task_info = curr_task_info;
							it_dyn_sched++;
							
							while(it_dyn_sched!=tasks_assigned->end()) { // maximum 1 round!
								if( curr_shortest_time_to_next_deadline > curr_task_info->get_time_to_next_deadline() ) {
									return_task_info = curr_task_info;
								}						
								it_dyn_sched++;
							}
						}

						break;
									
					default:
						string msg;
						msg = "Unknown or non-supported dynamic dispatch policy settled in scheduler ";
						msg = msg + name(); 
						msg = msg + ". Please, review it.";
						SC_REPORT_ERROR("kista",msg.c_str());
				}			
				
				break; 
				
			default:
				string msg;
				msg = msg + "Unknown or non-supported dispatch policy settled in scheduler ";
				msg = msg + name(); 
				msg = msg + ". Please, review it.";
				SC_REPORT_ERROR("KisTA",msg.c_str());
		}
					
#ifdef _PRINT_DISPATCHERS_ACTIVITY
		cout << "Scheduler " << name() << " (Dispatcher): Yielding processor to task " << return_task_info->name() << " at time " << sc_time_stamp() << endl;
#endif
					
	    // return current disptached
		return return_task_info;
	  }
 
	 // -----------------------------------------------------------
	 // Global scheduler process
	 // -----------------------------------------------------------

	 void scheduler::scheduler_proc() {
		 
		task_info_t 		*current_task_info;
		task_info_t 		*past_task_info=NULL;
		sc_process_handle 	current_task_scp_handle;
		
		sc_event_or_list	scheduler_event_trigger_list;

		string msg;

if(tasks_assigned->size()!=0) { // works only if the scheduler has assigned tasks
				
#ifdef _PRINT_SCHEDULERS_CONFIGURATION
		cout << "-----------------------------------------" << endl;	
		cout << "\"" << name() << "\" scheduler settings " << endl;
		cout << "Number of statically assigned tasks: " << number_of_tasks() << endl;
		
	    // static policies	
		if(get_policy()==STATIC_SCHEDULING) {
			cout << "- Static Scheduling: " << endl;			
			cout << "- Schedule: ";
			if((static_schedule_by_task_names.size() == 0 ) && ( static_schedule_by_task_info_pointers.size() == 0 )) {
				msg += " : No static schedule was provided for scheduler ";
				msg += name();
				msg += ".";
				SC_REPORT_ERROR("KisTA",msg.c_str());
			} else {
				if( static_schedule_by_task_names.size() != 0 ) {
					for(unsigned int i=0;i<static_schedule_by_task_names.size();i++) {
						cout << static_schedule_by_task_names[i];
						if (i<(static_schedule_by_task_names.size()-1)) cout << ", ";
						else cout << endl;
					}
				} else {
					for(unsigned int i=0;i<static_schedule_by_task_info_pointers.size();i++) {
						cout << static_schedule_by_task_info_pointers[i]->name();
						if (i<(static_schedule_by_task_info_pointers.size()-1)) cout << ", ";
						else cout << endl;
					}					
				}
			}
		} else if(get_policy()==CYCLE_EXECUTIVE) {
			cout << "- Static Scheduling: " << endl;
			cout << "- Schedule: Cyclic Executive " << endl;
		// dynamic policies
		} else {
			cout << "- Dynamic Scheduler" << endl;

			if(get_policy()==RANDOM_SCHEDULING) {
				cout << "  - Random Scheduling" << endl;
			} else if (get_policy()==PSEUDO_RANDOM_SCHEDULING) {
				cout << "  - Pseudo-Random Scheduler" << endl;
			} else if (get_policy()==STATIC_PRIORITIES) {
				cout << "  - Static Priorities" << endl;
				if(get_static_priorities_policy() == USER_PRIORITIES) {
					cout << "    - User Priorities" << endl;
				} else if(get_static_priorities_policy() == DEADLINE_MONOTONIC ) {
					cout << "    - Deadline Monotonic" << endl;					
				} else if(get_static_priorities_policy() == RATE_MONOTONIC) {
					cout << "    - Rate Monotonic" << endl;					
				} else if(get_static_priorities_policy() == DENSITY_MONOTONIC) {					
					cout << "    - Density Monotonic" << endl;					
				} else {
					cout << "    - Unknown static priorities assignation." << endl;						
				}				
			} else if (get_policy()==DYNAMIC_PRIORITIES) {
				if(get_dynamic_priorities_policy() == EARLIEST_DEADLINE_FIRST) {
					cout << "    - Earliest Deadline First (EDF)" << endl;
				} else if(get_dynamic_priorities_policy() == USER_DYNAMIC_PRIORITY_POLICY ) {
					cout << "    - User dynamic priority policy" << endl;						
				} else {
			    	cout << "    - Unknown dynamic priority policy" << endl;					
				}
			}
			
			if (is_preemptive()) {
				cout << "- Preemptive" << endl;
			} else if (is_cooperative()) {
				cout << "- Cooperative" << endl;
			} else { // non-preemptive: scheduler only reacts to explicit (user task) yields and to the task completion (has to produce a yield)
				cout << "- Non_Preemptive" << endl;
			}
			
			cout << "- Time slicing:  ";
			if(time_slicing_enabled()) cout << "enabled " << endl;
			else cout << "disabled" << endl;
			cout << "- Tick timer:  " << tick_timer_delay << endl;
		}
		
		cout << "-----------------------------------------" << endl;
#endif

		// Configures the scheduler_event_trigger_list
		if(get_policy()==STATIC_SCHEDULING) {
			scheduler_event_trigger_list = yield_event; // In kista, it makes no sense to consider the completion event
														// since KisTA performs a cyclic execution on the order stated by
														// the user static schedule.
														// Notice that an implementation based or related to the CYCLE_EXECUTIVE
														// dynamic scheduling of KisTA has not been done, because the static
														// scheduling can repeat task firings, and because it does not consider
														// availability. So, with the static_scheduling policy it might happen that
														// the control is granted to a task waiting for data, for instance
			
		} else {
			if (is_preemptive()) {
				scheduler_event_trigger_list = yield_event |  completion_event | tick_timer_event | running_task_to_wait_state_event | innactive_task_ends_wait_event; // required to enable preemption
			} else if (is_cooperative()) {
				scheduler_event_trigger_list = yield_event |  completion_event;
			} else { // non-preemptive: scheduler only reacts to explicit (user task) yields and to the task completion (has to produce a yield)
				scheduler_event_trigger_list = completion_event;
			}
		}
		
		// all the tasks are suspended at the beggining
		// (the dont_initialize() attribute was used in the sc_spawn options in the scheduler constructor)

#ifdef _PRINT_SCHEDULERS_BEGIN
cout << " ***********************************" << endl;
cout << " Begin of execution in scheduler: " << name() << endl;
cout << " ***********************************" << endl;
#endif

		//if (there_is_time_slicing()) {
		//	wait(tick_timer_delay);
		//} // otherwise,
		
		// start immediately (notice that in the time sliced scheduler, the first schedule is already doen at time 0.

		// Global scheduler
		while(true) {

			state_signal.write(SCHEDULING); // scheduler passes to executing state
			consumption_signal.write(true);
			
#ifdef _PRINT_SCHEDULERS_ACTIVITY
			cout << name() << " resume at time = " << sc_time_stamp() << " (global sim. time = " << global_sim_time << ")" << endl;
#endif			
			if (sc_time_stamp() >= global_sim_time) {
#ifdef _PRINT_SCHEDULER_REACHES_GLOBAL_SIMULATION_TIME_LIMIT
				cout << "KisTA-INFO: Scheduler " << name() << " reaches global simulation time limit = " << global_sim_time << endl;
#endif
				sc_stop(); 
			}

			if(are_scheduling_times_enabled()) {
				
				if(scheduling_time_calculator!=NULL) {
cout << "HAY SCHEDULING TIME CALCULATOR" << endl;
					// if there is a scheduling time calculator associated
					scheduling_time = scheduling_time_calculator(this);
				}
				// else {
				// constant scheduling time already settled at elaboration time
				//}
				
#ifdef _DYNAMIC_CHECK_SCHED_TIMES
				// For the time slicing case we assume that scheduling_time <= tick_timer_delay
				// Ensured through the static and dynamic checks (the latter necessary when modelling dynamic scheduling times			
				// Notice that if scheduling times approach tick timer delay times, no much time is left for user tasks
				if(time_slicing_enabled()) {
					check_scheduling_time(scheduling_time);
				}
#endif	
				
cout << "SCHEDULING TIME: " << scheduling_time << endl;				
				wait(scheduling_time);
				sched_comp_time += scheduling_time;
			}
			else {
				wait(SC_ZERO_TIME);	
				
				// The following wait cycle is required to give time to
				// an executing task update the state signal from executing to ready (when an executing process has been)
				// preempted
				wait(SC_ZERO_TIME);	
				
				// Optimization: A faster option can be done in principle to remove this second wait on delta.
				//
				//               It obligues to write the scheduling algorithms checking if the process was
				//               executing (as well as checking only READY tasks)
				//               An example is commented for the static priority scheduling policy
				//
				//            However, this option is not implemented yet because:
				//                - it enables easier writing of scheduling policies
				//                - seems in priciple safer, since it can consider any state transition of the 
				//                  tasks because of concurrent events (with impact on task state)
				//                  happening exactly when the scheduler is resumed
				//
			}
			
			current_task_info       = dispatch();
			
			if(past_task_info!=NULL) {
				if(current_task_info != past_task_info) n_context_changes++;
			}
			past_task_info = current_task_info;
			
			current_task_scp_handle = current_task_info->get_task_process_handler();

			n_schedulings++;

// somewhere here, evaluation of context switch and consideration of that additional delay

			state_signal.write(INNACTIVE); // scheduler passes to inactive (ready) state
			consumption_signal.write(false);
			
					// the if is included in order to avoid adding more deltas when
					// no actual time delay is associated to the scheduler
			
			// triggers the current task
			current_task_info->active_signal.write(true);

#ifdef _PRINT_SCHEDULERS_ACTIVITY				
				cout << "scheduler " << name() << ": suspending at " << sc_time_stamp() << endl;
#endif

			wait(scheduler_event_trigger_list); 

			current_task_info->active_signal.write(false);
		}
}
#ifdef _WARN_TASKS_NOT_ASSIGNED_TO_SCHED
else {
		msg = " Scheduler ";
		msg += name();
		msg += " has no task assigned, so it will remain inactive.";
		SC_REPORT_WARNING("KisTA",msg.c_str());
}
#endif
	}

	// -----------------------------------------------------------
	// Scheduler configurations
	// -----------------------------------------------------------
	 
	// setters
	void scheduler::set_triggering_policy(scheduler_triggering_policy_t policy) {
 		string msg;
		msg = "While setting triggering policy of scheduler ";
		msg += name();
		msg = ". This has to be done before the simulation starts.";
		if(sc_is_running()) {
			SC_REPORT_ERROR("KisTA", msg.c_str() );
		} else {
			scheduler_triggering_policy = policy;
		}
	}

    // alternative style for setting triggering policy	
	void scheduler::set_preemptive()  {
		set_triggering_policy(PREEMPTIVE);
	}
	
	void scheduler::set_cooperative() {
		set_triggering_policy(COOPERATIVE);
	}
	             
	void scheduler::set_non_preemptive() {
		set_triggering_policy(NON_PREEMPTIVE);
	}
	
	void scheduler::enable_time_slicing() {
		string msg;
		msg = "Time slicing in scheduler ";
		msg += name();
		msg = " has to be set before the simulation starts.";
		if(sc_is_running()) {
			SC_REPORT_ERROR("KisTA", msg.c_str() );
		} else {
			t_slicing_flag = true;
		}
	}
	
    void scheduler::set_tick_timer_delay(sc_time ttimer_delay) {
		string msg;
		msg = "Tick timer delay in scheduler ";
		msg += name();
		msg = " has to be set before the simulation starts.";
		if(sc_is_running()) {
			SC_REPORT_ERROR("KisTA", msg.c_str() );
		} else {
			tick_timer_delay =  ttimer_delay;
		}
	}
		
    void scheduler::set_policy(scheduler_policy_t sp) {
		string msg;
		msg = "Dispatch Policy in scheduler ";
		msg += name();
		msg = " has to be set and configured before the simulation starts.";
		if(sc_is_running()) {
			SC_REPORT_ERROR("KisTA", msg.c_str() );
		} else {
			scheduler_policy =  sp;
		}
	}
	
		// To set the schedule through task names
	void scheduler::set_static_schedule(static_schedule_by_task_names_t schedule) {
		static_schedule_by_task_names = schedule;
	}
		// To set the schedule through task information pointers
	void scheduler::set_static_schedule_by_task_ptrs(static_schedule_by_task_info_pointers_t schedule) {
		static_schedule_by_task_info_pointers = schedule;
	}

	void scheduler::set_static_priorities_policy(static_priorities_policy_t sdp) {
		set_policy(STATIC_PRIORITIES); // This includes the check to do it before simulation start
		static_priorities_policy = sdp;
	}
		
	void scheduler::set_rate_monotonic() {	
		set_static_priorities_policy(RATE_MONOTONIC);
	} 

	void scheduler::set_deadline_monotonic() {
		set_static_priorities_policy(DEADLINE_MONOTONIC);
	} 
	
	void scheduler::set_density_monotonic() {
		set_static_priorities_policy(DENSITY_MONOTONIC);
	}
		
	void scheduler::set_dynamic_priorities_policy(dynamic_priorities_policy_t dpp) {
		set_policy(DYNAMIC_PRIORITIES); // This includes the check to do it before simulation start
		dynamic_priorities_policy = dpp;
	}

	void scheduler::set_earliest_deadline_first() {	
		set_dynamic_priorities_policy(EARLIEST_DEADLINE_FIRST);
	} 
	
	
	void scheduler::set_Round_Robin() {
		set_preemptive();
		set_policy(CYCLE_EXECUTIVE); // This is actually not necessary since it is stated by default in KisTA
		enable_time_slicing();
	}	
			
		// return STATIC, CYCLE_EXECUTIVE (default), RANDOM, PSEUDO_RANDOM, STATIC_PRIORITIES, DYNAMIC_PRIORITIES
	scheduler_policy_t &scheduler::get_policy() {
		return scheduler_policy;
	} 
	
	static_priorities_policy_t &scheduler::get_static_priorities_policy() {
		return static_priorities_policy;
	}

	dynamic_priorities_policy_t &scheduler::get_dynamic_priorities_policy() {
		return dynamic_priorities_policy;
	}
						
		// get/test configured scheduling triggering policy

	scheduler_triggering_policy_t &scheduler::get_triggering_policy() {
		return scheduler_triggering_policy;
	}
		
	bool scheduler::is_preemptive() {
		return  (scheduler_triggering_policy==PREEMPTIVE);
	}

	bool scheduler::is_cooperative() {
		return  (scheduler_triggering_policy==COOPERATIVE);
	}

	bool scheduler::is_non_preemptive() {
		return  (scheduler_triggering_policy==NON_PREEMPTIVE);
	}
		
	bool &scheduler::time_slicing_enabled() {
		return  t_slicing_flag;
	}

	sc_time &scheduler::get_tick_timer_delay() {
		return  tick_timer_delay;
	}	
/*	
	bool &scheduler::is_preemptive() {
		return preemptive;
	}
*/

	sc_process_handle &scheduler::get_process_handle() {
		return sched_phandler;
	}

	// -----------------------------------------------------------
	// Trace files implementation
	// -----------------------------------------------------------

	// VCD tracing
	void scheduler::trace_utilizations() {
		scheduler_trace_file_name = name();
		scheduler_trace_file_name += "_trace";
		trace_utilizations(scheduler_trace_file_name);
	}
	
	void scheduler::trace_utilizations(std::string &scheduler_trace_file_name_par) {
	
		string msg;
		
		scheduler_trace_file_name = scheduler_trace_file_name_par;
		
		scheduler_trace_file = sc_create_vcd_trace_file(scheduler_trace_file_name.c_str());
		
		sc_write_comment(scheduler_trace_file, "File created by KisTA library.\n Author: F.Herrera\n Institution: KTH\n Date: 2013\n	All rights reserved by the authors, till further definition of the license.");
		
		msg = "Tracing options used include to trace";
#ifdef _TRACE_TASK_STATE_SIGNALS
		msg += " Task State signals, ";
#endif

#ifdef _TRACE_UNFOLDED_TASK_STATE_SIGNALS
		msg += "(including unfolded ones) and";
#endif

#ifdef _TRACE_SCHEDULER_SIGNALS
		msg += " Scheduler signals ";
#endif

#ifdef _TRACE_UNFOLDED_SCHEDULER_SIGNALS
		msg += "(including unfolded ones)";
#endif
		sc_write_comment(scheduler_trace_file,  msg.c_str() );

#ifdef _TRACE_SCHEDULER_SIGNALS								
		// tracing scheduler state signal 
		sc_trace(scheduler_trace_file, state_signal ,name());
#endif

#ifdef _TRACE_TASK_STATE_SIGNALS		
		// tracing tasks state signals
		for( taskset_by_name_t::iterator it = tasks_assigned->begin(); it != tasks_assigned->end(); ++it ) {
			sc_trace(scheduler_trace_file,  it->second->state_signal, it->second->name() );
		}
#endif
		
#ifdef _TRACE_UNFOLDED_SCHEDULER_SIGNALS								
		// tracing scheduler busy signal 
		msg = name();
		msg += "_exec";
		sc_trace(scheduler_trace_file, consumption_signal , msg );
#endif

#ifdef _TRACE_UNFOLDED_TASK_STATE_SIGNALS		

		// tracing tasks computation signals 
		for( taskset_by_name_t::iterator it = tasks_assigned->begin(); it != tasks_assigned->end(); ++it ) {
			msg = it->second->name();
			msg += "_cons";
			sc_trace(scheduler_trace_file,  it->second->consumption_signal, msg );
		}
#ifdef _TRACE_ACTIVE_SIGNALS				
		for( taskset_by_name_t::iterator it = tasks_assigned->begin(); it != tasks_assigned->end(); ++it ) {
			msg = it->second->name();
			msg += "_activ";
			sc_trace(scheduler_trace_file,  it->second->active_signal, msg );
		}
#endif		
#ifdef _TRACE_W_PERIOD_SIGNALS
		for( taskset_by_name_t::iterator it = tasks_assigned->begin(); it != tasks_assigned->end(); ++it ) {
			msg = it->second->name();
			msg += "_w_per";
			sc_trace(scheduler_trace_file,  it->second->waiting_period_signal, msg );
		}
#endif	
#ifdef _TRACE_W_COMM_SYN_SIGNALS			
		for( taskset_by_name_t::iterator it = tasks_assigned->begin(); it != tasks_assigned->end(); ++it ) {
			msg = it->second->name();
			msg += "_w_comm";
			sc_trace(scheduler_trace_file,  it->second->waiting_comm_sync_signal, msg );
		}	
#endif				
#endif
	}

	// -----------------------------------------------------------
	// Output metrics report
	// -----------------------------------------------------------

	// getters
	unsigned int &scheduler::number_of_tasks() {
		return total_n_tasks;
		// return task_assigned.size(); // this would support dynamic assignation of the task list
		                                // but not used for the moment 
	}

 	double	scheduler::get_task_utilization(const char *task_name) {
		string msg;
		if( tasks_assigned->find(task_name) == tasks_assigned->end()) {
			msg += task_name;
			msg += " is not a task assigned to scheduler ";
			msg += name(); msg += ".";
			SC_REPORT_ERROR("KisTA",msg.c_str());
			return 0.0; // for avoiding the compilation warning
		} else {
			if(sc_get_status()&SC_STOPPED) {	
				// IEEE Std 1666-2011 does not state waht sc_time_stamps returns
				// after SystemC simulation as expired. Therefore, we detect
				// if simulation has finished, so them we use the last time stamp
				// at the end of the simulation.
				// This way, this report function can be safely used at the
				// end of the SystemC simulation
				//cout << "Last simulation time: " << sc_time_stamp() << endl;
				//cout << tasks_assigned->find(task_name)->second->occupation_time << endl;									
				return tasks_assigned->find(task_name)->second->occupation_time/last_simulation_time;
			} else {	
				if(sc_is_running())  {				
					if(sc_time_stamp()>SC_ZERO_TIME) {					
#ifdef _ENABLE_WARNINGS_WHEN_MEASURED_UTILIZATION_GOT_DURING_SIMULATION
						msg += "Utilization retrieved, but no time advance yet. 0.0 will be reported for task ";
						msg += 	task_name;
						msg += " utilization.";
						SC_REPORT_WARNING("KisTA", msg.c_str() );
#endif						
						// this way, this method can be call at any time during the simulation
						return tasks_assigned->find(task_name)->second->occupation_time/sc_time_stamp();
					} else {
						msg += "Utilization retrieved, but no time advance yet. 0.0 will be reported for task ";
						msg += 	task_name;
						msg += " utilization.";
						SC_REPORT_WARNING("KisTA", msg.c_str() );
					}
				} else {
					msg += "Utilization retrieved before simulation start. 0.0 will be reported for task ";
					msg += 	task_name;
					msg += " utilization.";					
					SC_REPORT_WARNING("KisTA", msg.c_str() );
				}	
			}			
			
			return 0.0;
		}
    }

    double scheduler::get_tasks_utilization() {
		sc_time all_tasks_utilization_time;

		for( taskset_by_name_t::iterator it = tasks_assigned->begin(); it != tasks_assigned->end(); ++it ) {
			all_tasks_utilization_time+=it->second->occupation_time;
		}

		if(sc_get_status()&SC_STOPPED) {	
			// IEEE Std 1666-2011 does not state waht sc_time_stamps returns
			// after SystemC simulation as expired. Therefore, we detect
			// if simulation has finished, so them we use the last time stamp
			// at the end of the simulation.
			// This way, this report function can be safely used at the
			// end of the SystemC simulation
			//cout << "Last simulation time: " << sc_time_stamp() << endl;
			return all_tasks_utilization_time / last_simulation_time;
		} else {	
			if(sc_is_running())  {				
				if(sc_time_stamp()>SC_ZERO_TIME) {					
					// this way, this method can be call at any time during the simulation
					return all_tasks_utilization_time / sc_time_stamp();
				} else {					
					SC_REPORT_WARNING("KisTA", "No time advance yet. 0.0 will be reported for all tasks utilization.");
				}
			} else {				
				SC_REPORT_WARNING("KisTA", "All tasks utilization retrieved before simulation start");
			}	
			return 0.0;			
		}		

	}
	
    double scheduler::get_platform_utilization() {
		return get_tasks_utilization() + get_scheduler_utilization();
		// return 1.0-get_scheduler_occupation();
	}
	
	double scheduler::get_scheduler_utilization() {
		if(sc_get_status()&SC_STOPPED) {	
			// IEEE Std 1666-2011 does not state waht sc_time_stamps returns
			// after SystemC simulation as expired. Therefore, we detect
			// if simulation has finished, so them we use the last time stamp
			// at the end of the simulation.
			// This way, this report function can be safely used at the
			// end of the SystemC simulation
			//cout << "Last simulation time: " << sc_time_stamp() << endl;
			return sched_comp_time / last_simulation_time;
		} else {	
			if(sc_is_running())  {				
				if(sc_time_stamp()>SC_ZERO_TIME) {					
					// this way, this method can be call at any time during the simulation
					return sched_comp_time / sc_time_stamp();
				} else {					
					SC_REPORT_WARNING("KisTA", "No time advance yet. 0.0 will be reported as scheduler utilization.");
				}
			} else {				
				SC_REPORT_WARNING("KisTA", "Scheduler utilization retrieved before simulation start");
			}	
		}
		return 0.0;
	}
	
	// reports if the assigned tast has ever executed
	bool  scheduler::assess_starvation(const char *task_name) {
		string msg;
		if( tasks_assigned->find(task_name) == tasks_assigned->end()) {
			msg += task_name;
			msg += " is not a task assigned to scheduler ";
			msg += name(); msg += ".";
			SC_REPORT_ERROR("KisTA",msg.c_str());
			return 0.0; // for avoiding the compilation warning
		} else {
			if(sc_get_status()&SC_STOPPED) {	
				// IEEE Std 1666-2011 does not state waht sc_time_stamps returns
				// after SystemC simulation as expired. Therefore, we detect
				// if simulation has finished, so them we use the last time stamp
				// at the end of the simulation.
				// This way, this report function can be safely used at the
				// end of the SystemC simulation
				//cout << "Last simulation time: " << sc_time_stamp() << endl;
				//cout << tasks_assigned->find(task_name)->second->occupation_time << endl;									
				return (tasks_assigned->find(task_name)->second->occupation_time == SC_ZERO_TIME);
			} else {	
				SC_REPORT_ERROR("KisTA", "asses starvations methods can be called only at the end of the simulation." );
				return false; // to avoid compilation warnings
			}			
		}
		
	}
	
	// returns true if at least one task has never executed
	bool  scheduler::assess_starvation() {
		string msg;
		bool some_task_starved = false;
		
		for( taskset_by_name_t::iterator it = tasks_assigned->begin(); it != tasks_assigned->end(); ++it ) {
			if(it->second->occupation_time==SC_ZERO_TIME) {
				some_task_starved = true;
				msg =  it->first;
				msg += " starved,";
				SC_REPORT_WARNING("KisTA", msg.c_str());
			}
		}
		return some_task_starved;
	}
	

    unsigned long long scheduler::get_number_of_schedulings() {
		return n_schedulings;
	}

    // nuber of CONTEXT SWITCHES (schedulings which involve change of task)
	unsigned long long scheduler::get_number_of_context_switches() {
		return n_context_changes;
	};
    
    // returns true if at least one task assigned to the scheduler lost one deadline (can be called at any time)
	bool scheduler::miss_deadlines() {
		for( taskset_by_name_t::iterator it = tasks_assigned->begin(); it != tasks_assigned->end(); ++it ) {
			if(it->second->miss_deadline()) return true;
		}
		return false;
	}
	
	// returns true if at least one task assigned to the scheduler lost one deadline (can be called at the end of simulation)
	void scheduler::report_miss_deadlines() {
		cout << "******************************************************* " << endl;
		cout << " Miss Deadlines report" << endl;
		cout << " Scheduler " << name() << endl;
		cout << "******************************************************* " << endl;
		if (miss_deadlines()) {
			for( taskset_by_name_t::iterator it = tasks_assigned->begin(); it != tasks_assigned->end(); ++it ) {
				if(it->second->miss_deadline()) {
					cout << " Task " << it->second->name() << " has miss deadlines." << endl;
				}
			}
		} else {
			cout << "No assigned task has miss deadlines." << endl;
		}
	}
    
    // provides a report of the response times of the tasks assigned to the scheduler
	void scheduler::report_response_times() {
		cout << "******************************************************* " << endl;
		cout << " Response times report" << endl;
		cout << " Scheduler " << name() << endl;
		cout << "******************************************************* " << endl;
		for( taskset_by_name_t::iterator it = tasks_assigned->begin(); it != tasks_assigned->end(); ++it ) {
			cout << " R_max(" << it->second->name() << ") = " << it->second->worst_case_response_time() << " at " << it->second->location_of_worst_case_response_time() << endl;
		}
	}    
    
    // to check that the priorities are over 0
    // and that they are not multiple in case this restriction is wanted
    void scheduler::check_priorities() {   
		
		string msg;
		
 		taskset_by_name_t::iterator it;
 		unsigned int n_tasks_with_current_prior;
 		
 		typedef std::map<string, unsigned int>     tasks_with_invalid_priority_t;
 		typedef std::multimap<unsigned int,string>  priorities_mmap_t;
 		
 		tasks_with_invalid_priority_t 	tasks_with_invalid_priority;
 		priorities_mmap_t 				priorities_mmap; // store the priorities appearing, and their multiplicity (amount of times a priority appears in an assignation to the scehduler)
 		
 		std::map<string, unsigned int>::iterator invalid_pr_iter;
 		std::multimap<unsigned int,string>::iterator pmm_iter;
 		
 		invalid_pr_iter = tasks_with_invalid_priority.begin();
#ifdef _PRINT_SCHED_PRIO_CHECKINGS
SC_REPORT_INFO( "KisTA", "Checking Priorities assignements");
#endif 		
 		for(it = tasks_assigned->begin(); it != tasks_assigned->end(); ++it) {
#ifdef _PRINT_SCHED_PRIO_CHECKINGS			
cout << "Checking task " << it->first << " with user priority " << it->second->get_priority() <<endl;
#endif
			if(it->second->get_priority() == 0) { // add one more task to the list of invalid priorities
				tasks_with_invalid_priority[it->first]  = it->second->get_priority() ;
			}
			//priorities_mmap[it->second->get_priority()] = it->first;
			priorities_mmap.insert( pair<unsigned int, const char *>(it->second->get_priority() , it->first) );
		}
		
		if(tasks_with_invalid_priority.size() != 0) {		
//			SC_REPORT_ERROR("KisTA","Tasks assigned to the scheduler with priority 0 (which is reserved) ");			
			msg = to_string(tasks_with_invalid_priority.size());
			msg += " task(s) assigned with priority 0 (which is reserved) assigned to scheduler ";
			msg += name();
			msg+= "\n Tasks are: ";
			invalid_pr_iter = tasks_with_invalid_priority.begin();
			while(invalid_pr_iter != tasks_with_invalid_priority.end()) {			
				//msg+= tasks_with_invalid_priority[invalid_pr_iter->first];
				msg+= invalid_pr_iter->first;
				++invalid_pr_iter;
				if(invalid_pr_iter != tasks_with_invalid_priority.end()) msg+= ", ";
				else msg+= ".";
			}	
			msg+= "\n (check that you configured their priorities) \n";
			SC_REPORT_ERROR("KisTA",msg.c_str());
		}
		
		pmm_iter = priorities_mmap.begin();
		while( pmm_iter != priorities_mmap.end()) {
			n_tasks_with_current_prior = priorities_mmap.count(priorities_mmap.find(pmm_iter->first)->first);
			if( n_tasks_with_current_prior > 1 ) {
				SC_REPORT_WARNING("KisTA", "More than one tasks assigned to the scheduler with the same priority");
				cout << n_tasks_with_current_prior << " tasks assigned to scheduler " << name();
				cout << " with priority " << pmm_iter->first << endl;
				cout << "Tasks are: " ;
				for(unsigned int i = 0; i< n_tasks_with_current_prior; i++)  {
					cout << pmm_iter->second;
					if( i == (n_tasks_with_current_prior-1) ) cout << endl;
					else cout << ", ";
					++pmm_iter; 
				}
			 } else {
			    ++pmm_iter;
			 }
		}
		
	}

    void scheduler::check_scheduling_time(sc_time t) {
		string msg;
		if(time_slicing_enabled()) {
			if(t>tick_timer_delay) {
				msg = "Scheduler ";
				msg += this->name();
				msg += " : Scheduling time (";
				msg += t.to_string();
				msg += ") is bigger than tick timer delay (";
				msg += tick_timer_delay.to_string();
				msg += "), so there is no chance for tasks execution.";
				SC_REPORT_ERROR("KisTA", msg.c_str());	
			}
		}
	}
	
    void scheduler::check_and_prepare_static_schedule() {
		string msg;
		std::vector<string>::iterator it_names;
		std::vector<task_info_t*>::iterator it_handlers;
		taskset_by_name_t::iterator assigned_tasks_it;
			
		unsigned int i =0;
		
		msg = "Scheduler ";
		msg += this->name();
		
		// Check first if some static schedule has been provided
		if( (static_schedule_by_task_names.size() == 0 ) && ( static_schedule_by_task_info_pointers.size() == 0 ) ) {
			msg += " : No static schedule was provided.";
			SC_REPORT_ERROR("KisTA",msg.c_str());
		}

		// If the KisTA user provides scheduling in the two formats, it is checked that both of them are coherent
		if( (static_schedule_by_task_names.size() != 0 ) && ( static_schedule_by_task_info_pointers.size() != 0 ) ) {
			if(static_schedule_by_task_names.size() != static_schedule_by_task_info_pointers.size() ) {
				// different sizes
				msg += " : Non coherent schedules were provided both, by task names, and by pointers to task information handlers.\n";
				msg += "Sizes of schedulings are different. Size of schedule by names (";
				msg += static_schedule_by_task_names.size();
				msg += "), Size of schedule by task handlers(";
				msg += static_schedule_by_task_info_pointers.size();
				msg += ").";
				SC_REPORT_ERROR("KisTA",msg.c_str());
			} else {
				for( i = 0;i<static_schedule_by_task_info_pointers.size();i++ )
				{
					// check if the schedules specified by different means are different (despite same size on the schedule)
					if( static_schedule_by_task_names[i].compare(static_schedule_by_task_info_pointers[i]->name() ) != 0 ) {
						msg += " : static schedules by task names and by pointers to task information handlers were provided, but they are not coherent.\n";
						msg += "Detected discrepance at the " ;
						msg += to_string(i);
						msg += "-th task of the schedule:\n";
						msg += "static_schedule_by_task_names ->";
						msg += static_schedule_by_task_names[i];
						msg += "\nstatic_schedule_by_task_info_pointers ->";
						msg += static_schedule_by_task_info_pointers[i]->name();
						SC_REPORT_ERROR("KisTA",msg.c_str());
					}				
				}		
			}
		}		

		// At this point of code, it is guaranteed that the static schedule has been
		// captured either as a list of task names or as a list of task information pointers
		
		// KisTA will use internally the vector of pointers to task information.
		// Therefore, if only the task name list was provided, the list of task information pointers is created...
		if( (static_schedule_by_task_names.size() != 0 ) ) {
			for( i = 0;	i < static_schedule_by_task_names.size(); i++) {
				assigned_tasks_it = tasks_assigned->find(static_schedule_by_task_names[i].c_str());
				
				// ... in the same procedure, it is checked if
				// the schedule specified contains a task that is not in the task set assigned to the scheduler
				if(assigned_tasks_it == tasks_assigned->end()) {
					msg += " : Task ";
					msg += static_schedule_by_task_names[i] ;
					msg += " does not belong to the task set assigned to the scheduler.\n";
					SC_REPORT_ERROR("KisTA",msg.c_str());					
				}
				static_schedule_by_task_info_pointers.push_back(assigned_tasks_it->second);
			}		
		}
		
		// Up to here, a coherent schedule in static_schedule_by_task_names is guaranteed
		// and transferred to static_schedule_by_task_info_pointers
		

		// Now checks if the schedule specified lacks a task of the assigned set
		// (in other words, if any task of the task set assigned to the scheduler 
		//  would be never excuted by the static schedule)
		for( assigned_tasks_it = tasks_assigned->begin();
			 assigned_tasks_it != tasks_assigned->end();
			 assigned_tasks_it++) {
			if(find( static_schedule_by_task_info_pointers.begin(), static_schedule_by_task_info_pointers.end(), assigned_tasks_it->second) == static_schedule_by_task_info_pointers.end() ) {
				msg += " : Task ";
				msg += assigned_tasks_it->second->name();
				msg += " is present in the task set assigned to the scheduler, but not in the static schedule. Therefore this task will be never executed.";
#ifdef _REPORT_TASK_ALLOCATED_TO_SCHEDULER_BUT_NOT_IN_STATIC_SCHEDULE_AS_WARNING
				SC_REPORT_WARNING("KisTA",msg.to_str());
#else
				SC_REPORT_ERROR("KisTA",msg.c_str());
#endif
			}
		}
		
		// prepare iterator used by dispatcher for static scheduling
		it_static_sched = 0;

		
	}
	
	void scheduler::before_end_of_elaboration() {
		string msg;
		
		sc_process_handle created_process_handle;
 		
 		// updates the sched_t internal variable for compact access to the scheduler policy
		sched_t.scheduler_triggering_policy = this->scheduler_triggering_policy;
		sched_t.scheduler_policy = this->scheduler_policy;
		sched_t.static_priorities_policy = this->static_priorities_policy;
		sched_t.dynamic_priorities_policy = this->dynamic_priorities_policy;
 		
 		// Task spawning
		//    (using const_interator, since the task information requires to be added adding the 
		//     assigned process handler)
#ifdef _PRINT_SCHEDULERS_ACTIVITY			
			cout << "Scheduler " << name() << ": spawning tasks..." << endl;
#endif

		if(tasks_assigned==NULL) {
			msg = "Scheduler ";
			msg += name();
			msg += ": Unexpected error. Tasks assigned pointer is NULL.";
			SC_REPORT_ERROR("KisTA",msg.c_str() );			
		}
			
		for( taskset_by_name_t::iterator it = tasks_assigned->begin(); it != tasks_assigned->end(); ++it ) {
			
			sc_spawn_options spawn_options;
			
			// for avoiding initialization of the created tasks (avoid using a suspend_all method at the beggining)
			spawn_options.dont_initialize(); 
							
			// static sensitivity for the first trigger
			spawn_options.set_sensitivity(&it->second->active_signal.posedge_event());			
				
			created_process_handle = sc_spawn(
								sc_bind( it->second->get_functionality() ),
								it->second->name(),
								&spawn_options // all tasks created with the same options (basically, not initialization)
									);
										
			// stores the recently created process handler in the associated task information structure
			it->second->set_task_process_handler(created_process_handle);
			it->second->set_scheduler(this);
				
			// updates the global task info table by phandle
			task_info_by_phandle[created_process_handle] = it->second;
		}
			
		total_n_tasks = tasks_assigned->size();

		// -------------------
		// If not mapping of the the scheduler to at least one PE is done, then a "default PE" is generated
		// (each generation with an unique name) and the scheduler is allocated to it.
		// NOTE: note that this involves an extension (variation in general of the module hierarchy)
		//       depending on global conditions of the system construction. This has to be done in the before_end_of_elaboration callback
		//       and cannot be done in the end_of_elaboration callback, which takes place later on in SystemC semantics, and which does not
		//       admit any modification of the system module and binding structure.
		if(PE==NULL) {
			PE = new processing_element;
			msg = "Scheduler ";
			msg += name();
			msg += " was not assigned to a PE. The scheduler was automatically mapped to an automatically created processing element ";
			msg += PE->name();
			msg += ", created without network interface.";
			SC_REPORT_WARNING("KisTA",msg.c_str() );
		}
		
		// If sketch report enabled, this scheduler is dumped to the tikz sketch file
		if(sketch_report.is_enabled()) {
			sketch_report.draw(this);
		}

	}
	
    void scheduler::end_of_elaboration() {   
 		// Create the vector table for fast look up of tasks information for priority based scheduling
 		// It is done at the end of elaboration, and before simulation, once all constructors have been
 		// executed.
 		// This way, it is ensured that the shorted vector is created once all the tasks have been assigned to
 		// each scheduler
 		
 		//
 		// Using a vector enables better look up times O(1) than a map O(N).
 		// A multimap would allow a straightforward support of the modelling of systems with several tasks
 		// with the same priority, however, it would be slower.
 		// Moroever, it creates an ambiguity (there is no imposed ordering among tasks of the same priority)
 		// once the static priorties are selected
 		//
 		// KisTA uses a "unique internal priorities" system when static scheduling is selected.
 		// In order to implement such modelling with vector we need to use an unique internal index (internal_priority)
 		// per task, which in turn has a given associated priority
 		//
 		
 		taskset_by_name_t::iterator it;
 		
 		//unsigned int internal_priority;	// unique priorities handled by KisTA
 		tasks_by_priority_t::iterator internal_priority;	// unique priorities handled by KisTA
 		
 		task_info_t *task_info_tmp;
 		
 		scheduler_t sched_t;
 		
 		sc_time new_global_sim_time;
 		
 		string msg;
 		 		
 		if(tasks_assigned->size()==0) {
			msg += "No Tasks assigned to scheduler ";
			msg += name();
//			SC_REPORT_ERROR("KisTA",msg.c_str() );
#ifdef _WARN_TASKS_NOT_ASSIGNED_TO_SCHED
			SC_REPORT_WARNING("KisTA",msg.c_str() );
#endif
		}

/* // The following check is not necessary in principle, once the the task_by_priority is generated from the tasks:assigned structure
 		if(tasks_by_priority.size()==0) {
			msg += "No Tasks assigned to scheduler ";
			msg += name();
			msg += ". Detected in the tasks by priority structure";
			SC_REPORT_ERROR("KisTA", )
		}
*/
#ifdef _PRINT_TASKSET_ASSIGNED_TO_SCHEDULERS
		cout << "Scheduler " << name() << ": Taskset assigned:" << endl << "\t";
		if(tasks_assigned->size()==0) { 
			cout << "No task assigned" << endl;
		} else {
			it=tasks_assigned->begin();
			while(true) {
					cout << it->second->name();
					it++;
					if(it== tasks_assigned->end()) {
						cout << endl;
						break;
					} else {
						cout << ", ";
					}
			}
		}
#endif 		


if(tasks_assigned->size()!=0) { // actions which make sense only if the scheduler has assigned tasks
                                 // In case not, the scheduler will get lock and will not run
		
		// checks for scheduling policies, and preparation of some of them (static)	
		if(scheduler_policy == STATIC_SCHEDULING) {
			// check setting of static
			// - check 
			//    -- the coherence of static schedulin with the task set assigned 
			//    -- If there is a task in the set not present in the schedule
			//    -- if there is a task in the schedule that it is not present in the set
			//    -- leave ready the static schedule list (by task pointers)
			this->check_and_prepare_static_schedule();
		
		} else {

			// check priorities ony for the following scheduling policies:
			// static priorities and dynamic priorities (as priorities can be the initial values of them)
			if(scheduler_policy==STATIC_PRIORITIES || scheduler_policy==DYNAMIC_PRIORITIES) {
			  this->check_priorities();
			}
			
			//internal_priority = 0;
			internal_priority = tasks_by_priority.begin();	// unique priorities handled by KisTA, starting from "begin" (0) (lower more prioritaty)

			// create vector from map structure
			for( it = tasks_assigned->begin(); it != tasks_assigned->end(); it++ ) {
				// task_info_tmp = tasks_assigned[it];
				task_info_tmp = it->second;

				//tasks_by_priority[internal_priority] = task_info_tmp;
				internal_priority = tasks_by_priority.insert(internal_priority, task_info_tmp);
				// tasks_by_priority.insert(internal_priority, task_info_tmp); // WARNING, This statement would make it fail, since internal_priority iteratero would get lost
				internal_priority++; // augments iterator
			}

			// SORTING VECTOR...
			
			switch (static_priorities_policy) {
				case USER_PRIORITIES:
					std::sort(tasks_by_priority.begin(), tasks_by_priority.end(), less_than_by_user_priority); // it takes redefined operator"<" of task_info_t (impl. based on priorities)        
					// tried alternative which does not work:
					//std::sort(tasks_by_priority.begin(), tasks_by_priority.end()); // it takes redefined operator"<" of task_info_t (impl. based on priorities)
						  // here a code to short the vector (call to short) is yet MISSING to ensure a priority based order in the vector
						  // (a compare function based on user priorities has to be given to short())
						  //   but this solution cannot work becasue "<" operator can be defined only for class references or enumerated, nor for pointers,
						  //   which is the case, because we use a vector of pointers, task_info_t pointer, thus we have to provide a function pointer as the third argument with the comparation by priority
					break;
				case RATE_MONOTONIC:
					std::sort(tasks_by_priority.begin(), tasks_by_priority.end(), less_than_by_period); // it takes redefined operator"<" of task_info_t (impl. based on priorities)
					break;
				case DEADLINE_MONOTONIC:
					std::sort(tasks_by_priority.begin(), tasks_by_priority.end(), less_than_by_deadline); // it takes redefined operator"<" of task_info_t (impl. based on priorities)
					break;
				case DENSITY_MONOTONIC:
					std::sort(tasks_by_priority.begin(), tasks_by_priority.end(), less_than_by_density); // it takes redefined operator"<" of task_info_t (impl. based on densities)
					break;				
				default:
					msg = "Unknown or non-supported static dispatch policy settled in scheduler ";
					msg += name();
					SC_REPORT_ERROR("KisTA",msg.c_str() );
			}
			
			if(is_static_analysis_enabled()) {
				static_analysis();
			}
			
			// prepare iterator (state variable) used by dispatcher for dynamic scheduling
			it_dyn_sched = tasks_assigned->begin();
			
#ifdef _PRINT_TASK_BY_PRIO_STRUCTURE		
			cout << " -----------------------------------" << endl;
			cout << " -- Tasks by priority -- structure :" << endl;
			cout << "size: " << tasks_by_priority.size() << endl;
			for( unsigned int j=0; j< tasks_by_priority.size(); j++ ) {
				cout << "Internal Priority: " << j;
				cout << " task: " << tasks_by_priority[j]->name();
				cout << " user priority: " << tasks_by_priority[j]->get_priority() << endl;
			}
			cout << " -----------------------------------" << endl;	
#endif 
		}
		
		if(this->get_PE() == NULL) {
		  msg = "Unexpected situation. Scheduler ";
          msg += this->name();
          msg += " has no associated PE. KisTA always associates a default PE. Please, contact the developers.";
          // No task scheduler (not mandatory, at least from the XML interface)
          SC_REPORT_ERROR("KisTA", msg.c_str());
		} 

		if(this->get_PE()->has_netif()) {
			// preparation of (static) communication scheduling policies 
			// the scheduler is the element which receives the scheduling, but it transferis it 
			// the information to the network interface at the end of elaboration
			// since the communicaiton scheduling is delegated to the network interface
			// This transfer of information, and the preparation of the scheduling is done
			// at the end of elaboration in order to ensure that the LINK between the
			// scheduler and the PE wich containts the network interface performing the
			// communication scheduling is already SOLVED

#ifdef _PRINT_SCHEDULERS_CONFIGURATION		
		  msg = "Preparing (static) communication scheduling for Scheduler ";
          msg += this->name();	
          msg += ", mapped to PE ";	  
		  msg += this->get_PE()->name();	
          msg += ", with network interface.";
          // No task scheduler (not mandatory, at least from the XML interface)
          SC_REPORT_INFO("KisTA", msg.c_str());
#endif					
			// (1) transfer to the netif the static communication schedulings configured in any of the 3 supproted styles
			if(static_comm_schedule.size()>0) {
				this->get_PE()->get_netif()->set_static_comm_scheduling(static_comm_schedule);
			}

			if(static_comm_schedule_by_name.size()>0) {
				this->get_PE()->get_netif()->set_static_comm_scheduling(static_comm_schedule_by_name);
			}
			
			if(static_comm_schedule_by_task_names.size()>0) {
				this->get_PE()->get_netif()->set_static_comm_scheduling(static_comm_schedule_by_task_names);
			}
			
			// (2) once the schedulings have been trasnferred to the network interface, 
			//     the network interface is commanded to prepare the scheduling
			this->get_PE()->get_netif()->check_and_prepare_comm_static_schedule();
		
			// notice that (1) and (2) must follow that sequence. 
			// (2) could have be done in the end_of_elaboration method of netif class as long as the 
			// (1) were triggerend in the before_end_of_elaboration method of the scheduler (this) class
		}
#ifdef _PRINT_SCHEDULERS_CONFIGURATION		
		else {
		  msg = "PE ";
		  msg += this->get_PE()->name();	
		  msg += " executing Scheduler ";
          msg += this->name();
          msg += " has no associated network interface.";
          msg += " No preparation of (static) communication scheduling is done or required.";
          // No task scheduler (not mandatory, at least from the XML interface)
          SC_REPORT_INFO("KisTA", msg.c_str());
		}
#endif
	
	    // States either the associated scheduling time calculator, or a constant scheduling time
	    // The code ensures the following priority in the application of a scheduling time
	    // associated to an scheduler instance
	    
	    // (1) A scheduler instance specific scheduling time calculator
	    // (2) A policy specific scheduling time calculator for every scheduler instances under such policy
	    // (3) A scheduling time calculator applicable to every scheduler instances for which a policy specific scheduling time calculator is not available
	    // (4) A scheduler instance specific constant scheduling time
	    // (5) A global constant scheduling time stated by the user
	    // (6) A global constant scheduling time stated from the default scheduling time (defaults.hpp)
	    
	    if(scheduling_time_calculator==NULL) {
			// if no specific scheduling time calculator has been settled,
			// it is searched in the global table for a given scheduling policy
			
			scheduling_time_calculator = get_scheduling_time_calculator(sched_t);
			
			if(scheduling_time_calculator==NULL) {
				// if no specific scheduling time calculator has been settled for a given scheduling policy,
				// then a common scheduling time calculator is searched

				scheduling_time_calculator = get_scheduling_time_calculator();
				
				if(scheduling_time_calculator==NULL) {	
					if(scheduling_time_set_flag == false) { // cases (5) or (6)
						scheduling_time = global_scheduling_time;
					} // else case (4) applies
					
				} // else case (3) applies
				 
			} // else case (2) applies
			
		} // else case (1) applies	

		// check statically settled times
		// (note: this does not cover the values provided by scheduling time calculators, but that its done
		//         dynamically in the associated scheduler process)
		
		if(scheduling_time_calculator==NULL) {
			check_scheduling_time(scheduling_time); // Notice that this checks the scheduling time statically configured
												 	 // for a more exhaustive check of variable scheduling times (when modelled in kista)
													 // you should enable the corresponding dynamic check (_DYNAMIC_CHECK_SCHED_TIMES)
		}
 
		// revise global simulation time limits if specified by the user 
		new_global_sim_time = SC_ZERO_TIME;
		if(sufficient_global_sim_time_factor>0) { // User has made the call to set it at elaboration time
			msg ="Scheduler ";
			msg += name();
			
			if(get_policy()==STATIC_SCHEDULING)
			{
				new_global_sim_time = sufficient_global_sim_time_factor * get_WCET(static_schedule_by_task_info_pointers);
			}
			else if(get_policy()==CYCLE_EXECUTIVE)
			{				
				new_global_sim_time = sufficient_global_sim_time_factor * get_WCET();
			} else if( is_periodic(*tasks_assigned) ) {
				new_global_sim_time = sufficient_global_sim_time_factor * get_hyperperiod(*tasks_assigned);
			} else {
				msg = "set_sufficient_global_simulation_time() will not take effect because the scheduler is neither static, nor it has assigned a periodic task set";
				SC_REPORT_WARNING("KisTA", msg.c_str());
			}
			
			if(new_global_sim_time>get_global_simulation_time_limit()) {		
				set_global_simulation_time_limit(new_global_sim_time);
#ifdef _WARN_SCHED_UPDATES_GLOBAL_SIMULATION_TIME
				msg += " updates global simulation time to ";
				msg += new_global_sim_time.to_string();
				SC_REPORT_WARNING("KisTA", msg.c_str());								
#endif
			}
			// else : the simulation time stated by the user through the set_global_simulation_time_limit,
			//        (or through the library defaults is already sufficient)
		}
}
	}

    void scheduler::end_of_simulation() {	
		last_simulation_time = sc_time_stamp();  // This assumes that when end_of_simulation callback is invoked
		                                          // the last simulation time stamp can be read (while not guarantee afterwards)
		if(last_simulation_time == sc_time(0,SC_SEC) ) {
			SC_REPORT_WARNING("KisTA","End of simulation at 0s. Is it an untimed model?");
		}
		if (scheduler_trace_file!=NULL) {
			sc_close_vcd_trace_file(scheduler_trace_file);
		}		
	}
	
	
	void scheduler::gen_tick_timer_event() {
#ifdef 	_PRINT_TICK_TIMER_EVENTS
		long unsigned int i =0;
#endif

        if (t_slicing_flag)	 {	// if time slicing is disabled, no tick timer events are generated
		  while(true) {
			tick_timer_event.notify(SC_ZERO_TIME);			
			last_tick = sc_time_stamp();
			next_tick = last_tick + tick_timer_delay;
#ifdef 	_PRINT_TICK_TIMER_EVENTS			
cout << " Tick " << i << " (scheduler " << name() << ") : at time "	<< sc_time_stamp() << endl;
			i++;
#endif
			wait(tick_timer_delay);
		  }
		}
	}
	
	// returns the accumulative WCET of the static schedule task set assigned to the scheduler
	sc_time scheduler::get_WCET(static_schedule_by_task_info_pointers_t static_schedule) {
		sc_time tmp;
		unsigned int i;
		tmp = SC_ZERO_TIME;
		for(i = 0; i < static_schedule.size(); i++) {
			tmp += static_schedule[i]->get_WCET();
		}
		return tmp;
	}

	// returns the accumulative WCET of the task set assigned to the scheduler
	sc_time scheduler::get_WCET() {
		sc_time tmp;
		tmp = SC_ZERO_TIME;
		for( taskset_by_name_t::iterator it = tasks_assigned->begin(); it != tasks_assigned->end(); ++it ) {
			tmp += it->second->get_WCET();
		}
		return tmp;
	}
		
	void scheduler::set_sufficient_global_simulation_time(unsigned int n) {
		std::string msg;
		sc_time new_global_sim_time;
		
		msg = "Scheduler ";
		msg += name();
		msg += ": ";
		
		if(sc_get_status()&SC_ELABORATION) {
			
			if(n==0) {
				msg += "set_sufficient_global_simulation_time(0) invalid. The multiplication factor has to be 1 (default), or bigger.";
				SC_REPORT_ERROR("KisTA", msg.c_str());
			}
			
			if ( (get_policy()==STATIC_SCHEDULING) || (get_policy()==CYCLE_EXECUTIVE) ||
				 is_periodic(*tasks_assigned)
				)
			{
				sufficient_global_sim_time_factor = n;
			} else {
				msg += "set_sufficient_global_simulation_time() will not take effect because the scheduler is neither static, nor it has assigned a periodic task set";
				SC_REPORT_WARNING("KisTA", msg.c_str());
			}
			
		} else {
			msg += "set_sufficient_global_simulation_time(n) has to be called at elaboration time (before the sc_start).";
			SC_REPORT_ERROR("KisTA", msg.c_str());	
		}				
	}
	
	void scheduler::set_scheduling_time(sc_time sched_time) {
		check_call_at_elaboration("set_scheduling_time(sc_time time) (scheduler method)");
		scheduling_time = sched_time;
		scheduling_time_set_flag = true;
	}	
	
	void scheduler::set_scheduling_time_calculator(SCHTIME_FPTR fun) {
		check_call_at_elaboration("set_scheduling_time(sc_time time) (scheduler method)");
		scheduling_time_calculator = fun;
	}
	
	// Allocation of Application tasks (mapping of applications to the scheduler)
	void  scheduler::allocate(taskset_by_name_t *tasks_assigned_p) {
		std::string msg;
		sc_time new_global_sim_time;
		
		msg = "Scheduler ";
		msg += name();
		msg += ": ";
		
		if(sc_get_status()&SC_ELABORATION) {
			tasks_assigned = tasks_assigned_p;
		} else {
			msg += "allocate() has to be called at elaboration time (before the sc_start).";
			SC_REPORT_ERROR("KisTA", msg.c_str());	
		}
	}

	// Mapping to HW resources 
	void  scheduler::map_to(processing_element *PE_var) {
		std::string msg;
		sc_time new_global_sim_time;
		
		msg = "Scheduler ";
		msg += name();
		msg += ": ";
		
		if(sc_get_status()&SC_ELABORATION) {
			PE = PE_var;
		} else {
			msg += "map_to() has to be called at elaboration time (before the sc_start).";
			SC_REPORT_ERROR("KisTA", msg.c_str());	
		}
	}
	
	// Static communication scheduling
	void scheduler::set_static_comm_scheduling(llink_set_t static_comm_schedule_par) {
		check_call_before_sim_start("set_static_comm_scheduling by llink");
		static_comm_schedule = static_comm_schedule_par;
	}
	
	void scheduler::set_static_comm_scheduling(static_comm_schedule_by_link_names_t static_comm_schedule_by_name_par) {
		check_call_before_sim_start("set_static_comm_scheduling by link names");
		static_comm_schedule_by_name = static_comm_schedule_by_name_par;
	}
	
	//   ... or by means of a vector reflecting the logic destination addresses (tasks)
	//       from which the llink is extracted.
	//       This methods works under the assumption that there is only one communication
	//       synchronization edge for 
	void scheduler::set_static_comm_scheduling(name_pair_vector_t static_comm_schedule_by_task_names_par) {
		check_call_before_sim_start("set_static_comm_scheduling by task link names");
		static_comm_schedule_by_task_names = static_comm_schedule_by_task_names_par;
	}
	
	
void scheduler::static_analysis() {
	std::string msg;
	scheduler_t current_scheduler_t;
		
	static_analysis_table_t::iterator it;
		
	SAFPTR static_analysis_function;
		
	static_analysis_function = NULL;

	// Passes static analysis stablished for any policy
	common_static_analysis(gets_tasks_assigned());
	
	// Passes static analysis for the configured policy
	current_scheduler_t.scheduler_policy = scheduler_policy;
	current_scheduler_t.static_priorities_policy = static_priorities_policy;
	current_scheduler_t.dynamic_priorities_policy = dynamic_priorities_policy;
	current_scheduler_t.scheduler_triggering_policy = scheduler_triggering_policy;
		
	it = static_analysis_table.find(current_scheduler_t);

	if(it == static_analysis_table.end()) {
#ifdef _REPORT_STATIC_ANALYSIS_OVERRIDE	
		msg = "No static analysis available for the configured scheduling policy: ";
		msg += current_scheduler_t.to_string();
		SC_REPORT_INFO("KisTA", msg.c_str());
#endif
		return;
		
	} else {
		static_analysis_function = it->second;
	}
	
	if(static_analysis_function == NULL) {
		msg = "NULL static analysis function was retrieved for the configured scheduling policy: ";
		msg += current_scheduler_t.to_string();
		SC_REPORT_ERROR("KisTA", msg.c_str());
	} else {
#ifdef _REPORT_STATIC_ANALYSIS_EXECUTION
		msg = "Executing static analysis for the configured scheduling policy: ";
		msg += current_scheduler_t.to_string();
		SC_REPORT_INFO("KisTA", msg.c_str());
#endif
		static_analysis_function(gets_tasks_assigned());
	}

}

taskset_by_name_t* scheduler::gets_tasks_assigned() {
	return tasks_assigned;
} 

} // namespace KisTA

#endif



