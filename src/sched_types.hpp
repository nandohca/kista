/*****************************************************************************

  sched_types.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 May

 *****************************************************************************/

#ifndef SCHED_TYPES_HPP
#define SCHED_TYPES_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "types.hpp"

using namespace std;

namespace kista {

// disptaching policies

enum scheduler_policy_t {
                         STATIC_SCHEDULING = 0,		// dispatch is established by a list which fixes the schedule and repeates for ever
                           // dynamic scheduler policies
                         RANDOM_SCHEDULING = 1,			// selects one random task among available ones
                         PSEUDO_RANDOM_SCHEDULING = 2,  // selects one rando task among available ones following a random and reproducible sequence
                         CYCLE_EXECUTIVE = 3,         // dispatch is performed in a cyclical manner
                                                      // (CPU granted to the next task ready in the list, where the list order is "random"
                                                      // (in kista implementation, defined by the assignation order)
                         STATIC_PRIORITIES = 4,       // dispatch relies on static priorities
															// STATIC PRIORITIES use, by default USER priorities for the ordering
															// set_rate_monotonic, and set_deadline_monotonic methods change and overide user priorities
                         DYNAMIC_PRIORITIES = 5,       // dispatch relies on dynamic priorities by updating a priority list
                         USER_SCHEDULER_POLICY = 6
                         };

typedef std::vector<std::string> scheduler_policy_name_table_t;
// scheduler_policy_name table declared and initialized in global elements

enum static_priorities_policy_t {
						USER_PRIORITIES = 0,
						DEADLINE_MONOTONIC = 1,
						RATE_MONOTONIC = 2,
						DENSITY_MONOTONIC = 3,
						USER_AUTOMATIC_STATIC_PRIORITIES = 4
};

typedef std::vector<std::string> static_priorities_policy_name_table_t;
// static_priorities_policy_name table declared and initialized in global elements

enum dynamic_priorities_policy_t {
	                     EARLIEST_DEADLINE_FIRST = 0,  
	                     USER_DYNAMIC_PRIORITY_POLICY =1
};

typedef std::vector<std::string> dynamic_priorities_policy_name_table_t;
// dynamic_priorities_policy_name table declared and initialized in global elements

enum scheduler_triggering_policy_t {
						PREEMPTIVE = 0,
						COOPERATIVE= 1,
						NON_PREEMPTIVE = 2
};

typedef std::vector<std::string> scheduler_triggering_policy_name_table_t;
// scheduler_triggering_policy_name table declared and initialized in global elements

class scheduler_t {
public:
	scheduler_policy_t 			scheduler_policy;
	static_priorities_policy_t	static_priorities_policy;
	dynamic_priorities_policy_t dynamic_priorities_policy;
	scheduler_triggering_policy_t scheduler_triggering_policy;
	
	std::string to_string();
};

bool operator==(const scheduler_t &lhs, const scheduler_t &rhs);

bool operator<(const scheduler_t &lhs, const scheduler_t &rhs);

// declaration of static analysis function interface
typedef void (*SAFPTR)(const taskset_by_name_t *assigned_tasks);

// table of static analysis
//typedef std::unordered_map<scheduler_t,VOIDFPTR> static_analysis_table_t;
typedef std::map<scheduler_t,SAFPTR> static_analysis_table_t;

// Prototype for user function to model/calculate scheduling times
// A scheduler instance pointer is passed as argument.
// This way, the user can access to the assigned task set
// and its properties, and to the status of the scheduler.
//
typedef sc_time (*SCHTIME_FPTR)(scheduler *sch_ptr);

// table of scheduling time calculation functions
typedef std::map<scheduler_t,SCHTIME_FPTR> scheduling_time_calculator_table_t;

// functions to add new user scheduling policies
unsigned int add_scheduler_policy(std::string sched_policy_name);
unsigned int add_static_scheduler_policy(std::string static_prio_policy_name);
unsigned int add_dynamic_scheduler_policy(std::string dynamic_prio_policy_name);


typedef unsigned char task_state_t;
typedef unsigned char sched_state_t;


// TASK STATES:
// Active states
#define EXECUTING                'E'
#define WAIT_PERIOD              'W'
#define WAIT_COMM_SYNC           'C'
// Inactive states
#define READY                    'R'
#define INACTIVE_WAIT_PERIOD     'I'
#define INACTIVE_WAIT_COMM_SYNC  'X'

#define FINISHED		         'F'

// Meaning for user tasks:
//   Active states:
//    E: Executing over any processing resource
//    W: Waiting for a synchronization event (for communication or for periodical event), while holding (thus wasting) some processing resource
//    R: Ready to execute, but no processing resource granted
//    I: Waiting for a synchronization event (for communication or for periodical event), without processing resource granted


// SCHEDULER STATES:
#define SCHEDULING               'S'
#define INNACTIVE		         'I'
//#define WAIT_COMM_SYNC         'C'
// 'C' state for scheduler can be used when supporting synchronization among schedulers

// Meaning for scheduler tasks:
//    S: Executing, so scheduling
//    I: Ready to execute, so no scheduling because an user task is executing (or in active waiting), thus consuming the processing resource scheduled

// typedefs for activity data
class tikz_activity_trace;
typedef tikz_activity_trace* tikz_activity_trace_handler;

typedef std::map<sc_time,task_state_t>  task_states_table_t;
typedef std::map<sc_time,sched_state_t> sched_states_table_t;
typedef std::unordered_map<task_info_t*, task_states_table_t*>	task_activity_t;
typedef std::unordered_map<scheduler*, sched_states_table_t*>	sched_activity_t;
// table type for estoring the tikz activity trace configured
typedef vector<tikz_activity_trace_handler> tikz_activity_trace_handler_table_t;

} // end namespace kista

#endif

