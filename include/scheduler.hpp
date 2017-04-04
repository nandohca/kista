/*****************************************************************************

  scheduler.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January

 *****************************************************************************/

#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <systemc.h>

#include "sched_types.hpp"
#include "defaults.hpp"

#include "task_info_t.hpp"
#include "taskset_by_name_t.hpp"

#include <map>

#include "static_sched_analysis.hpp"

// Relying on the STL library

namespace kista {

class processing_element;

// Declare a vector table for fast look up of tasks information for priority based scheduling
typedef std::vector<task_info_t*> tasks_by_priority_t;

class scheduler : public sc_module {
	friend void consume(sc_time comp_time);
	friend void yield();	
	friend void wait_period();
	friend class task_info_t;
	friend class sketch_report_t;
	friend class processing_element; // has to access private element "tasks_assigned"
public:
	SC_HAS_PROCESS(scheduler); 
	
	// constructors

	// raw constructor (allocation of taks set is separately done with the allocate method)
	scheduler(	sc_module_name sched_name = sc_module_name(sc_gen_unique_name("sched")),
				// properties
				sc_time ttimer_delay = sc_time(DEFAULT_TICK_TIMER_DELAY_NS,SC_NS),
				bool time_slicing_flag = false,
				scheduler_triggering_policy_t scheduler_triggering_policy_par = NON_PREEMPTIVE,
				scheduler_policy_t scheduler_policy_par = CYCLE_EXECUTIVE
				);			
					
	// compact constructor (tak set can be provided in the same call)
	scheduler(
				// assigned task list structure
				taskset_by_name_t *tasks_assigned_p,
				sc_module_name sched_name = sc_module_name(sc_gen_unique_name("sched")),
				// properties
				sc_time ttimer_delay = sc_time(DEFAULT_TICK_TIMER_DELAY_NS,SC_NS),
				bool time_slicing_flag = false,
				scheduler_triggering_policy_t scheduler_triggering_policy_par = NON_PREEMPTIVE,
				scheduler_policy_t scheduler_policy_par = CYCLE_EXECUTIVE
				);
	
	void scheduler_proc();
	
	sc_process_handle &get_process_handle(); // to get global scheduler process handle
	
	// ************************************************************************************
	// SCHEDULER CONFIGURATION
	// ************************************************************************************
	// --------------------------------------------
	// TASK SCHEDULING
	// --------------------------------------------

	// setters 	(should only be used at elaboration time, before the simulation starts) 	
	void set_triggering_policy(scheduler_triggering_policy_t policy = NON_PREEMPTIVE);  // PREEMPTIVE , COOPERATIVE, NON_PREEMPTIVE (default)

    // alternative style for setting triggering policy	
	void set_preemptive();       // A Task can be preempted at any time by a higher priority task
										// - scheduler triggered by yield, changes on task availability (wait) state, tick-timer (if enabled), and by task completion
	void set_cooperative();      // A Task can be preempted only at specific points designated by the task  
										// - scheduler triggered by yield (and by task completion)
	
	void set_non_preemptive();   // once a scheduler grants a processor to a task, the tasks executes till completion (an infinite task never returns processor resource)
	                                
	void enable_time_slicing(); // enable time slice events, which can trigger the scheduler in preemptive and cooperative policies
	
	void set_tick_timer_delay(sc_time ttimer_delay);
	
	sc_event &get_tick_timer_event();
		
	void set_policy(scheduler_policy_t sp = CYCLE_EXECUTIVE); // static ones: STATIC_SCHEDULING, CYCLE_EXECUTIVE (default),
	                                                                 // dynamic ones: STATIC_PRIORITIES, DYNAMIC_PRIORITIES, RANDOM_SCHEDULING, PSEUDO_RANDOM_SCHEDULING,

	// ------------------------------------------------------	
	// methods for configuring STATIC SCHEDULER policies.
	// ------------------------------------------------------
	void set_static_schedule(static_schedule_by_task_names_t schedule);  // To set the schedule through task names
	void set_static_schedule_by_task_ptrs(static_schedule_by_task_info_pointers_t schedule); // To set the schedule through task information pointers
		// Notes:
		//	- In case several calls to one of these configuration functions, the last one prevails
		//	- In case calls to both configuration functions are done, KisTA checks them, and in case they are not consistent, it reports an error
	
	// ------------------------------------------------------	
	// methods for configuring DYNAMIC SCHEDULER policies.
	// ------------------------------------------------------
	//    set_dispatch_policy (STATIC_PRIORITIES)
	void set_static_priorities_policy(static_priorities_policy_t sdp = USER_PRIORITIES); // USER_PRIORITIES (default), RATE_MONOTONIC, DEADLINE_MONOTONIC, DENSITY_MONOTONIC	
	void set_rate_monotonic();            // == set_static_dispatch_policy(RATE_MONOTONIC)
	void set_deadline_monotonic();        // == set_static_dispatch_policy(DEADLINE_MONOTONIC)
	void set_density_monotonic();         // == set_static_dispatch_policy(DENSITY_MONOTONIC)	
								// These methods 
								// * Reasign internal priorities to follow an order relying on the Period (set_rate_monotonic) or Deadline (set_deadline_monotonic)
								// - The reordering is APPLIED TO:
								//      - ALL TASKS ASSIGNED TO THE SCHEDULER
								//           - First all Tasks with a Period/Deadline are reordered as a function of their Period/Deadline
								//             The lesser the Period/Deadline, the highest the internal priority
								//           -  (**) Then, the other tasks not having a Period/Deadline, are scheduled under USER priority
								//              (the lesser the user priority, the higher (lower number) the internal priority, although
								//               always lower priority with a task with Period/Deadline)
								//					(**) To Be Implemented

	// methods for configuring dynamic policies
	void set_dynamic_priorities_policy(dynamic_priorities_policy_t dpp = EARLIEST_DEADLINE_FIRST); // EARLIES_DEADLINE_FIRST (default), USER
	void set_earliest_deadline_first();      // == set_dynamic_priority_policy(EARLIEST_DEADLINE_FIRST)

	void set_Round_Robin(); // set the scheduler as preemptive, with CYCLE_EXECUTIVE scheduler policy, preemptive and enables time slicing

	// TO GET SCHEDULER CONFIGURATION
	// --------------------------------------------
	scheduler_policy_t& get_policy(); // return STATIC, CYCLE_EXECUTIVE (default), RANDOM, PSEUDO_RANDOM, STATIC_PRIORITIES, DYNAMIC_PRIORITIES
	
	static_priorities_policy_t& get_static_priorities_policy();

	dynamic_priorities_policy_t& get_dynamic_priorities_policy();
		
		// get/test configured scheduling triggering policy
	scheduler_triggering_policy_t& get_triggering_policy();		
	bool is_preemptive();
	bool is_cooperative();
	bool is_non_preemptive();

    bool    &time_slicing_enabled();
    sc_time  &get_tick_timer_delay();
    
	// --------------------------------------------
	// COMMUNICATION SCHEDULING
	// --------------------------------------------
	
	// Static communication scheduling
	void set_static_comm_scheduling(llink_set_t static_comm_schedule_par);
	void set_static_comm_scheduling(static_comm_schedule_by_link_names_t static_comm_schedule_by_name_par);
	
	//   ... or by means of a vector reflecting the logic destination addresses (tasks)
	//       from which the llink is extracted.
	//       This methods works under the assumption that there is only one communication
	//       synchronization edge for 
	void set_static_comm_scheduling(name_pair_vector_t static_comm_schedule_by_task_names_par);

	// to be called at simulation time
	bool static_comm_scheduling_set();
	
	// TRACING AND REPORT METHODS and data
	// --------------------------------------------	
	sc_signal<sched_state_t>  state_signal;		 // For the scheduler, the state signal will server to reflect waitng for the end of context switches (WAITING)
	//   for unfolded tracing
	sc_signal<bool>            consumption_signal;   // Scheduler computation (or busy) signal
	sc_signal<bool>		        waiting_signal;

	// Trace the utilization signal of the scheduler and the utilization signals of the tasks assigned to the scheduler
	// in VCD format
	void trace_utilizations(std::string &scheduler_trace_file_name_par);
    void trace_utilizations();  // uses default name (relying on scheduler name)
                                
                                 
    // REPORTS of utilization and occupations
    //     They can be called at any time, before, during the simulation and the end of the simulation
    //     When called during the simulation they report the value up to that moment
    //     if called at the beginning of the simulation they will return 0.0
    double get_tasks_utilization();                   // occupation of all the tasks wrt time (the "useful")
    double get_platform_utilization();                // tasks occupation plus scheduler occupation (excludes waiting: synchronization and communication times)
                                       // currently, equivalent to PE utilization as this class model a local scheduler, i.e. 1 scheduler -> 1 PE
	double get_scheduler_utilization();               // resource time consumed by the scheduler
	double get_task_utilization(const char *name);   // resource time consumed by a task assiged to the scheduler
	
	unsigned long long get_number_of_schedulings();  		// nuber of schedulings
	unsigned long long get_number_of_context_switches();  // nuber of CONTEXT SWITCHES (schedulings which involve change of task)
	
		// other possible repors: numer of context switches (not the same as schedulings)
		
		// to be called only at the end of simulation
	bool	assess_starvation(const char *task_name);	// reports if the assigned tast has ever executed
	bool	assess_starvation();						// returns true if at least one task has never exected
														// (sumps into consola the starved tasks if any)
														
	bool	miss_deadlines();							// returns true if at least one task assigned to the scheduler lost one deadline (can be called at any time)
	void	report_miss_deadlines();					// provides a report of the tasks that have lost deadlines (can be called at the end of simulation)
						//NOTE: missed deadlines assesment functions are only applied over the task if this declares a deadline
						//       Periodical tasks declare by default a deadline (which is equal to its period by default)

	void	report_response_times();  // provides a report of the response times of the tasks assigned to the scheduler
	
	void	set_sufficient_global_simulation_time(unsigned int n = 1); // Ensures "sufficiently long" simulation time.
																		 // For it, it changes the global simulation time limit to make it longer
 																		 // A multiplicative factor, n,  can be provided as argument.																		 
																		 // If global simulation time limit is longer it leaves as it is
																		 // This call takes effect only in the following cases:
																		 //   * policy == STATIC_SCHEDULING :
																		 //			-> then it settles the limit as n * the WCET of the static schedule cycle
																		 //   * dynamic policy and periodic task set: 
																		 //			-> then it settles the limit as n * hyperperiod
																		 // Otherwise, the global simulation time limit is not touched (but a warning is provided)
																		 
	// Allocation of taskset to the scheduler
	// --------------------------------------------------------------------------------------	
	void  allocate(	taskset_by_name_t *tasks_assigned_p); // static allocation
	taskset_by_name_t *get_allocated_taskset() { return tasks_assigned; }

	// Map to processing elements (this local scheduler supports mapping to a single PE)
	// --------------------------------------------------------------------------------------
	
	void  map_to(processing_element *PE_var); // static mapping, since scheduler class represents a local scheduler, it can be mapped to a single PE
	processing_element *get_PE() { return PE; }

    //
    // Interface for modelling the non-idealities of  a specific scheduler instance
   	// -------------------------------------------------------------------------------
	
	// attach a function for the calculation of the scheduling time specifically for a given
	// scheduler insance (this can be done also with the function set_scheduling_time_calculator
	// The prototype of such function has to fulfil the 
	void  set_scheduling_time_calculator(SCHTIME_FPTR fun);

   	// to set a fixed scheduling time specific for a scheduler instance
   	// (Note: this is only applied when the neither a scheduling time calculator has been settled )
	void  set_scheduling_time(sc_time sched_time);
	
	// methods to facilitate external development of static analysis
	taskset_by_name_t *gets_tasks_assigned();  // to be called at the end of elaboration

	// returns the currently consumed (dynamic) energy by the scheduler task in Jules;
	const double &get_consumed_energy_J();

private:

    // getter for the total amount of tasks associated to the scheduler
    unsigned int &number_of_tasks();
    
    task_info_t* dispatch();
    // state variables for the dispatcher class
    //    note: This substitute an initial implementation based on static variables, 
    //          which prevented the use of several instances of scheduler class
    //          due to the side effects, because the static variable is shared by all class instances.
    unsigned int        it_static_sched;  // used by static scheduling
    taskset_by_name_t::iterator  it_dyn_sched;     // used by some dynamic scheduling policies
	
    
    // internal data for scheduler attributes
	sc_time    			last_tick, next_tick;
	sc_time    			tick_timer_delay;
	bool       						t_slicing_flag;
	scheduler_triggering_policy_t	scheduler_triggering_policy;
	scheduler_policy_t  			scheduler_policy;
	static_priorities_policy_t  	static_priorities_policy;
	dynamic_priorities_policy_t  	dynamic_priorities_policy;
	scheduler_t						sched_t; // updated at the end of elaboration, given the user configuration

	// scheduler process handler
	sc_process_handle 	sched_phandler;  
	
	// structure refereing to all tasks process handlers and their associated information
	taskset_by_name_t       *tasks_assigned;
	unsigned int   total_n_tasks; // autocalculated in constructor after static assignation

	tasks_by_priority_t tasks_by_priority; // unlike tasks assigned, this structure is internal and created by KisTA at the end of elaboration

	sc_time sched_comp_time; // computation time required or consumed by the scheduler task up to the current simuation time
	unsigned long long n_schedulings;		// number of schedulings up to the current simuation time
	unsigned long long n_context_changes;	// number of context changes up to the current simuation time
	
	sc_time last_simulation_time;

	sc_trace_file	*scheduler_trace_file;
	std::string		scheduler_trace_file_name;

	float platform_utilization; // utilization of the scheduler is given in terms
								 // of the ocupation of the scheduler
	
	void end_of_simulation(); // This call back is used to fill final_simulation_time variable
							  // (In the Acellera SC simulator, it is possible to use sc_time_stamp at the end of simulation
							  // but IEEE-1666-2011 standard says nothing about that
                              // It is also used to close trace files when they are reported
	
	void before_end_of_elaboration(); // used to automatically create a PE and bind it to the scheduler if the user does not it at building time
	void end_of_elaboration(); // used for the construction of task vector, for fast task selection at scheduling time
								// task spawning has been also moved here from the constructor, instead to construction time, in order to allow to
								// allow the separated allocation of tasksets to schedulers (not necessarily at construction time)
	
	void check_scheduling_time(sc_time t);                        
	void check_priorities();
		
	// Events which trigger can trigger the scheduler (depend on the configuration of the scheduler and time slicing enabling/disabling)
	sc_event yield_event;                      // - a task explicitly yields CPU (a cooperative scheduler use this for preempt the task)
	sc_event completion_event;                 // - a task explicitly yields and completes
	sc_event innactive_task_ends_wait_event;   // - an executing task passes to some waiting state (active or not)	
	sc_event running_task_to_wait_state_event; // - an inactive task passes from some waiting state to ready
	sc_event tick_timer_event;                 // - the tick timer produced an event
	
	// process generating tick timer event                    
	void gen_tick_timer_event();
	
	// internal variables for static task scheduling
	static_schedule_by_task_names_t           static_schedule_by_task_names;
	static_schedule_by_task_info_pointers_t  static_schedule_by_task_info_pointers;

	// internal variables for static communication scheduling
	llink_set_t								static_comm_schedule; // by logic links
	static_comm_schedule_by_link_names_t 	static_comm_schedule_by_name;
	name_pair_vector_t 						static_comm_schedule_by_task_names;

    // functions related to static scheduling
	void check_and_prepare_static_schedule();
	sc_time get_WCET(static_schedule_by_task_info_pointers_t static_schedule); // accumulative WCET of static schedule taskset
	sc_time get_WCET();															// accumulative WCET of tasks assigned taskset
	
	// control of simulation time limit from schedulers
	unsigned int sufficient_global_sim_time_factor; // 0 means it has not been settled, 1 or more it has been settled and the multiplicative factor

	// Variables for non-idealities
	SCHTIME_FPTR  scheduling_time_calculator;
	sc_time 	    scheduling_time;
	bool 	   		scheduling_time_set_flag;
		
	// processing elements
	// for the moment mapping only to a single one (local scheduler)
	processing_element *PE;
		
	// hook for adding static analysis
	void static_analysis();
	
	// energy accounter
	double consumed_energy_J;
	
};

} // namespace KisTA

#endif
