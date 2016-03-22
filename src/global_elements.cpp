/*****************************************************************************

  global_elements.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 May

 *****************************************************************************/

#ifndef GLOBAL_ELEMENTS_CPP
#define GLOBAL_ELEMENTS_CPP

#include "defaults.hpp"

#include "types.hpp"
#include "utils.hpp"

#include "draw.hpp"

namespace kista {

sc_time global_sim_time(DEFAULT_SIM_TIME_NS,SC_NS);

sketch_report_t sketch_report;
// vebosity control
bool global_verbosity;

 // by default, the modeller can build a model where all the system
 // tasks belong to single application
 // KisTA supports models where the system tasks can be clustered into
 // several applications (then the use_implicit_application flag becomes
 // false). It requires to explicitly declare applications first and
 // to declare the tasks passing the application they are associated to
 // (both styles cannot be merged, either one or another is used)
app_visibility_t app_visibility = UNSET_APP_VISIBILITY;
// applications_by_name holds the list of explicit applications declared
applications_by_name_t	applications_by_name;

taskset_t tasks;
// This is a global hash table for fast access to task info from
// the process handler, and by name:
// These global structures are filled by the different schedulers
tasks_info_by_phandle_t	task_info_by_phandle;
tasks_info_by_name_t		task_info_by_name;
tasks_info_by_name_t		env_tasks_by_name;
std::vector<task_info_t*>	task_vector; // to store integer ids internally assigned by KisTA

system_level_connection_set_t sys_conn;
// This "enforces" or assumes that even if there are different types of system-level channels, 
// the names associated for their logic link identifier is unique
system_level_connection_set_by_name_t sys_conn_by_name;

system_level_connection_type_set_t sys_conn_types;

sched_set_t scheds;
sched_set_by_name_t scheds_by_name;

pe_set_t pes;
mem_set_t mems;
// Global hash table for fast access to processing element info by name
// it is updated by each new PE created
PE_by_name_t PEref_by_name;
MEM_by_name_t MEMref_by_name;

phy_commres_set_t phy_commres_set;
phy_comm_res_by_name_t phy_commres_by_name;

tdma_bus_set_t tdma_buses;

// global structures to let hooking of communication with kista-xml executable

// for the moment, only a finite set of channel types supported by the XML interface
fifo_buffer_int_set_t     fifo_buffer_int;
fifo_buffer_uint_set_t    fifo_buffer_uint;
fifo_buffer_short_set_t   fifo_buffer_short;
fifo_buffer_ushort_set_t  fifo_buffer_ushort;
fifo_buffer_float_set_t	 fifo_buffer_float;
fifo_buffer_double_set_t  fifo_buffer_double;
fifo_buffer_char_set_t    fifo_buffer_char;
fifo_buffer_voidp_set_t   fifo_buffer_voidp;
fifo_buffer_message_set_t   fifo_buffer_msg;

// By default, KisTA creates a functionality monitor
validator_t *functionality_monitor = NULL;

// Global parameter list for enabling define global parameters in the user code, and let then explore then from the XML interface
global_parameters_t global_parameters;

// Global Initialization of the supported scheduling policies
scheduler_policy_name_table_t scheduler_policy_name = {"static", "random", "pseudo-random", "cycle executive", "static priorities", "dynamic priorities"};

static_priorities_policy_name_table_t static_priorities_policy_name = {"user", "deadline monotonic", "rate monotonic", "density monotonic"};

dynamic_priorities_policy_name_table_t dynamic_priorities_policy_name = {"EDF"};

scheduler_triggering_policy_name_table_t  scheduler_triggering_policy_name = {"PREEMPTIVE","COOPERATIVE","NON-PREEMPTIVE"};

//------------------------------------------------------------------
// global flags and structures for the control of static analysis
//------------------------------------------------------------------

// global flag to set or not static scheduler analysis (e.g. schedulability analysis)
bool static_sched_analysis_enabled_flag = false; // disabled by default

// function pointer containing a common static schedulability analysis
SAFPTR common_static_analysis;
	
// table of static analysis functions
static_analysis_table_t	static_analysis_table;

//-------------------------------------------
// global flags for the control of analysis
//-------------------------------------------

// global flag for energy and power measurement
bool energy_and_power_measurement_enabled = true;

// global flag to set or not worst case communication
bool worst_case_communication_enabled = true; // Enabled by default

// global flag to consider or not scheduling times
bool scheduling_times_enabled_flag = false; // disabled by default

// function pointer containing a scheduling time calculation functionality, applicable for all scheduler types
SCHTIME_FPTR common_scheduling_time_calculator;
	
// table of scheduling time calculation functionalities
scheduling_time_calculator_table_t	scheduling_time_calculator_table;

// scheduling time taken for all the scheduler instances which do not configre
// a scheduling time calculator, or a specific scheduling time
sc_time global_scheduling_time = sc_time(DEFAULT_SCHED_TIME_NS,SC_NS);

// global power averaging time
sc_time global_power_averaging_time = sc_time(DEFAULT_POWER_AVERAGING_TIME_S,SC_SEC);

//----------------------------------------------
// global flags for the control of exploration
//----------------------------------------------
bool error_stop_exploration_flag = false; //on default error stop the exploration

//--------------------------------------------------------------------
// Table with the handlers of all the TIKZ activity traces declared
//-------------------------------------------------------------------
tikz_activity_trace_handler_table_t tikz_activity_trace_handler_table;

} // namespace kista

#endif



