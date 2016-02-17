/*****************************************************************************

  global_elements.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 May

 *****************************************************************************/

#ifndef GLOBAL_ELEMENTS_HPP
#define GLOBAL_ELEMENTS_HPP

#include "types.hpp"
#include "sched_types.hpp"
#include "utils.hpp"

namespace kista {

extern sc_time global_sim_time;
extern sketch_report_t sketch_report;

// vebosity control
extern bool global_verbosity;

 // by default, the modeller can build a model where all the system
 // tasks belong to single application
 // KisTA supports models where the system tasks can be clustered into
 // several applications (then the use_implicit_application flag becomes
 // false). It requires to explicitly declare applications first and
 // to declare the tasks passing the application they are associated to
 // (both styles cannot be merged, either one or another is used)
extern app_visibility_t app_visibility;
// applications_by_name holds the list of explicit applications declared
extern applications_by_name_t	applications_by_name;

extern taskset_t tasks;
// This is a global hash table for fast access to task info from
// the process handler, and by name:
// These global structures are filled by the different schedulers
extern tasks_info_by_phandle_t	task_info_by_phandle;
extern tasks_info_by_name_t		task_info_by_name;
extern tasks_info_by_name_t		env_tasks_by_name;
extern std::vector<task_info_t*>		task_vector; // to store integer ids internally assigned by KisTA

extern system_level_connection_set_t sys_conn;
// This "enforces" or assumes that even if there are different types of system-level channels, 
// the names associated for their logic link identifier is unique
extern system_level_connection_set_by_name_t sys_conn_by_name;

extern system_level_connection_type_set_t sys_conn_types;

extern sched_set_t scheds;
extern sched_set_by_name_t scheds_by_name;

extern pe_set_t pes;
extern mem_set_t mems;
// Global hash table for fast access to processing element info by name
// it is updated by each new PE created
extern PE_by_name_t PEref_by_name;
extern MEM_by_name_t MEMref_by_name;

extern phy_commres_set_t phy_commres_set;
extern phy_comm_res_by_name_t phy_commres_by_name;

extern tdma_bus_set_t tdma_buses;

// global structures to let hooking of communication with kista-xml executable

// for the moment, only a finite set of channel types supported by the XML interface
extern fifo_buffer_int_set_t     fifo_buffer_int;
extern fifo_buffer_uint_set_t    fifo_buffer_uint;
extern fifo_buffer_short_set_t   fifo_buffer_short;
extern fifo_buffer_ushort_set_t  fifo_buffer_ushort;
extern fifo_buffer_float_set_t	 fifo_buffer_float;
extern fifo_buffer_double_set_t  fifo_buffer_double;
extern fifo_buffer_char_set_t    fifo_buffer_char;
extern fifo_buffer_voidp_set_t   fifo_buffer_voidp;
extern fifo_buffer_message_set_t   fifo_buffer_msg;

// By default, KisTA creates a functionality monitor
extern validator_t *functionality_monitor;

// Global parameter list for enabling define global parameters in the user code, and let then explore then from the XML interface
extern global_parameters_t global_parameters;

// access to supported scheduling policies name tables
extern scheduler_policy_name_table_t scheduler_policy_name;
extern static_priorities_policy_name_table_t static_priorities_policy_name;
extern dynamic_priorities_policy_name_table_t dynamic_priorities_policy_name;
extern scheduler_triggering_policy_name_table_t  scheduler_triggering_policy_name;

//------------------------------------------------------------------
// global flags and structures for the control of static analysis
//------------------------------------------------------------------

// global flag to set or not static scheduler analysis (e.g. schedulability analysis)
extern bool static_sched_analysis_enabled_flag;

// function pointer containing a common static schedulability analysis
extern SAFPTR common_static_analysis;
	
// table of static analysis functions
extern static_analysis_table_t	static_analysis_table;

//------------------------------------------------------------
// global flags for the control of simulation-based analysis
//------------------------------------------------------------

// global flag to set or not worst case communication
extern bool worst_case_communication_enabled;

// global flag to consider or not scheduling
extern bool scheduling_times_enabled_flag;

// function pointer containing a scheduling time calculation functionality, applicable for all scheduler types
extern SCHTIME_FPTR common_scheduling_time_calculator;
	
// table of scheduling time calculation functionalities
extern scheduling_time_calculator_table_t	scheduling_time_calculator_table;

// scheduling time taken for all the scheduler instances which do not configre
// a scheduling time calculator, or a specific scheduling time
extern sc_time global_scheduling_time;

//----------------------------------------------
// global flags for the control of exploration
//----------------------------------------------
extern bool error_stop_exploration_flag;

//--------------------------------------------------------------------
// Table with the handlers of all the TIKZ activity traces declared
//-------------------------------------------------------------------
extern tikz_activity_trace_handler_table_t tikz_activity_trace_handler_table;

} // namespace kista

#endif



