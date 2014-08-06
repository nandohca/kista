/*****************************************************************************

  defaults.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January

 *****************************************************************************/

#ifndef DEFAULTS_HPP
#define DEFAULTS_HPP

#include <systemc.h>


namespace kista {

// *****************************************************
// Scheduler defaults
// *****************************************************
#define DEFAULT_TICK_TIMER_DELAY_NS 10000000

//default scheduling time in NS
#define DEFAULT_SCHED_TIME_NS 0
//#define DEFAULT_SCHED_TIME_NS 1000000

//default simulation time limit in NS
//#define DEFAULT_SIM_TIME_NS 1
#define DEFAULT_SIM_TIME_NS 1000000000000

// by defining the following variable, arbitrary deadlines, that is, deadlines greater than periods (Davis&Burns 09) are admited 
// (However, when their are settled, a warning is reported)
#define _ALLOW_ARBITRARY_DEADLINES

//#define _PERIODIC_TASK_RESYNCHRONIZE_WHEN_MISSING_DEADLINE
// Defines de behavior of a periodic task when missing a deadline
// if _PERIODIC_TASK_RESYNCHRONIZE_WHEN_MISSING_DEADLINE disabled, then the tasks gets released immediately after the release
// event where the miss deadline is detected and can go on executing,
// if _PERIODIC_TASK_RESYNCHRONIZE_WHEN_MISSING_DEADLINE is enabled, then the tasks blocks untile the next release event, so will
// released in 2T, or N*T
//

// Task creation defaults
#define DEFAULT_INITIAL_PRIORITY 255

// *****************************************************
// Dynamic checks configuration
// *****************************************************
//		Ass well as the checkings done at elaboration time by kista
//      There are a number of dynamic checks (performed during the 
//      simulation. These can be disabled for gaining simulation performance
//
	//
	// To check that scheduling times do not exceed tick timer times 
	// during the simulation (this is interesting when modelling variable
	// scheduling times, with context changes, etc). For fixed scheduling
	// times, it is already covered by a static check
	//
#define _DYNAMIC_CHECK_SCHED_TIMES

#define _CHECK_YIELDS_IN_NON_PREEMPTIVE_SCHEDULERS
//#define _IGNORE_YIELDS_IN_NON_PREEMPTIVE_SCHEDULER

//#define _ENABLE_CHECK_ALLOCATED_CHANNELS_IN_CALCULATE_MAX_P2P_DELAY	

// *****************************************************
// Tracing configuration
// *****************************************************
#define _TRACE_TASK_STATE_SIGNALS
#define _TRACE_SCHEDULER_SIGNALS
//#define _TRACE_UNFOLDED_TASK_STATE_SIGNALS
//
  #define _TRACE_ACTIVE_SIGNALS				
  #define _TRACE_W_PERIOD_SIGNALS
  #define _TRACE_W_COMM_SYN_SIGNALS		
//
//#define _TRACE_UNFOLDED_SCHEDULER_SIGNALS


// *****************************************************
// defaults for system sketch
// *****************************************************
#define DEGREES_OUTPUTS_SEPARATION 5
#define DEGREES_INPUTS_SEPARATION 5

// *****************************************************
// defaults for TiKZ trace
// *****************************************************
#define DEFAULT_TIKZ_TRACE_EVENTS_LIMIT 	15
#define DEFAULT_TIKZ_TRACE_START_TIME_NS	0
#define DEFAULT_TIKZ_TRACE_BASE_LINES_SEPARATION 1
#define DEFAULT_TIKZ_TIME_STAMPS_MAX_SEPARATION 1
#define DEFAULT_TIKZ_TRACE_SCALE 1
#define DEFAULT_MAX_X_LOCATION 20

// *****************************************************
// Measurements configuration
// *****************************************************

// constant stating the number of repeated lags in order to decide a stationary measure
// (currently used for stationary throughput measure)
#define N_REPEATED_LAGS_FOR_STATIONARY 2
#define LAG_MATCHES_LIMIT 1000000
//#define REPORT_NUMBER_OF_MATCHES_IN_STATIONARITY_ASSESSMENT
// The current implementation is a simple one which, actually, only compares two lags,
// (that is, the minimum required)

// *****************************************************
// Code Instrumentation configuration
// *****************************************************
// defines if the code is automatically instrumented and annotated,
// by relying on some external plugin. In such a case, KisTA
// use those annotations, instead WCETs
// (By default, this variable is undefined, so WCET will be used.
//  Then the user can just enable the annotation by enabling 
//  the variable, see brt_ex7 example)
//#define _USE_AUTOMATIC_CODE_ANNOTATION

// select the plugin (scope plugin by default)
#define _USE_SCOPE_OPCOST_ANNOTATION

// *****************************************************
// Configuration for consideration of platform effects
// *****************************************************
#define _CONSIDER_COMMUNICATION_DELAYS

// *****************************************************
// Error, Warning Configuration
// *****************************************************
//#define _ENABLE_WARNINGS_WHEN_MEASURED_UTILIZATION_GOT_DURING_SIMULATION
//#define _ENABLE_WARNINGS_WHEN_MEASURED_RESPONSE_GOT_DURING_SIMULATION
//#define _REPORT_TASK_ALLOCATED_TO_SCHEDULER_BUT_NOT_IN_STATIC_SCHEDULE_AS_WARNING
//#define _REPORT_LINK_ATTRIBUTES_SETTING_FROM_BASE_LINK_AS_WARNING
//#define _REPORT_LINK_ATTRIBUTES_SETTING_FROM_BASE_LINK_AS_ERROR
//#define _REPORT_LINK_ATTRIBUTES_RETRIEVAL_FROM_BASE_LINK_AS_WARNING
//#define _REPORT_LINK_ATTRIBUTES_RETRIEVAL_FROM_BASE_LINK_AS_ERROR

// *****************************************************
// DEBUG printout configuration
// *****************************************************
//#define FULLY_VERBOSE
#define CUSTOM_VERBOSITY
//#define MINIMUM_VERBOSITY

// options of printout configurations

#ifdef FULLY_VERBOSE

#define _PRINT_VALIDATORS_PASS
#define WARN_SET_FUNCTIONAL_FAILURE_WHEN_FUNCTIONAL_VALIDATION_NOT_ENABLED
#define WARN_SET_FUNCTIONAL_FAILURE_THRESHOLD_WHEN_FUNCTIONAL_VALIDATION_NOT_ENABLED	

#define _PRINT_SETTING_OF_GLOBAL_SIMULATION_TIME_LIMIT
#define _PRINT_SCHEDULER_REACHES_GLOBAL_SIMULATION_TIME_LIMIT

#define _PRINT_SCHEDULERS_CONFIGURATION
#define _PRINT_SCHEDULERS_BEGIN
#define _PRINT_TASKSET_ASSIGNED_TO_SCHEDULERS
#define _PRINT_SCHEDULERS_ACTIVITY
#define _PRINT_DISPATCHERS_ACTIVITY
#define _PRINT_EDF_DISPATCHING_INFO
#define _PRINT_SCHED_PRIO_CHECKINGS
#define _PRINT_TASK_BY_PRIO_STRUCTURE

#define _PRINT_BEGIN_OF_CONSUMPTION_TIMES 
#define _PRINT_END_OF_CONSUMPTION_TIMES 
#define _ADDITIONAL_DEBUG_MESSAGES
#define _PRINT_TICK_TIMER_EVENTS

#define _PRINT_TASK_STATE_TRANSITION
#define _PRINT_TASK_RELEASE_EVENTS
#define _PRINT_TASK_COMPLETION

#define _WARN_TASKS_NOT_ASSIGNED_TO_SCHED
#define _WARN_SCHED_UPDATES_GLOBAL_SIMULATION_TIME

#define _PRINT_ACCUMULATION_OF_CONSUMPTION_TIMES

#define _REPORT_CONFIGURATION_COMPLETION_TDMA_BUS
#define _WARN_POSSIBLE_ACCURACY_INCONSISTENCIES_FOR_TDMA_BUS
#define _WARN_CYCLE_TIME_NOT_CONFIGURED_FOR_TDMA_BUS
#define _WARN_CYCLE_TIME_BIGGER_FOR_TDMA_BUS
#define _REPORT_TDMA_BUS_CONFIGURATION

#define _REPORT_COMMUNICATION_DELAYS

#define _REPORT_AUTOMATIC_CHANNEL_MAPPINGS
#define _PRINT_BINDIND_INFO

#define _REPORT_FIFO_BUFFER_AUTOMAP_FAILURE
#define _REPORT_FIFO_BUFFER_TOKEN_SIZE_DETERMINATION
#define _REPORT_FIFO_BUFFER_NETIF_DETECT_WRITER_PE

#define _WARN_THROUGHPUT_MONITOR_ISSUES
#define _REPORT_NUMBER_OF_MATCHES_IN_STATIONARITY_ASSESSMENT

#define _WARN_SMALL_NETIF_TX_BUFFER
#define _REPORT_NETIF_SEND_REQUESTS
#define _REPORT_NO_STATIC_COMM_SCHEDULING_FOR_NETIF
#define _REPORT_NETIF_DISPATCH

#define _REPORT_SYSTEM_COMMUNICATION_WRITE_ACCESSES
#define _REPORT_SYSTEM_COMMUNICATION_READ_ACCESSES
#define _REPORT_SYSTEM_COMMUNICATION_WRITE_COMPLETION
#define _REPORT_SYSTEM_COMMUNICATION_READ_COMPLETION

#define _WARN_LINKS_FOUND_WITHOUT_ASSOCIATED_INFORMATION	

#define _REPORT_WRITE_IO_ACCESS
#define _REPORT_WRITE_IO_COMPLETION
#define _REPORT_READ_IO_ACCESS
#define _REPORT_READ_IO_COMPLETION
#define _REPORT_INIT_TOKENS_INJECTION

// environment defaults
#define _REPORT_ENV_MESSAGES	

// static analysis reports
#define _REPORT_DEFAULT_STATIC_ANALYSIS
#define _REPORT_COMMON_STATIC_ANALYSIS_REPLACEMENT
#define _REPORT_STATIC_ANALYSIS_ADDITION
#define _REPORT_STATIC_ANALYSIS_EXECUTION

// scheduling time calculation functions reports
#define _REPORT_COMMON_SCHED_TIME_REPLACEMENT	
#define _REPORT_SCHED_TIME_FUNCION_ADDITION


#endif

  // configure of VERBOSE mode by defining and undefining configuration activity
#ifdef CUSTOM_VERBOSITY

#define _PRINT_VALIDATORS_PASS
//#define WARN_SET_FUNCTIONAL_FAILURE_WHEN_FUNCTIONAL_VALIDATION_NOT_ENABLED
//#define WARN_SET_FUNCTIONAL_FAILURE_THRESHOLD_WHEN_FUNCTIONAL_VALIDATION_NOT_ENABLED	

#define _PRINT_SETTING_OF_GLOBAL_SIMULATION_TIME_LIMIT
#define _PRINT_SCHEDULER_REACHES_GLOBAL_SIMULATION_TIME_LIMIT

#define _PRINT_SCHEDULERS_CONFIGURATION
#define _PRINT_SCHEDULERS_BEGIN
#define _PRINT_TASKSET_ASSIGNED_TO_SCHEDULERS
//#define _PRINT_SCHEDULERS_ACTIVITY
//#define _PRINT_DISPATCHERS_ACTIVITY
//#define _PRINT_SCHED_PRIO_CHECKINGS
//#define _PRINT_EDF_DISPATCHING_INFO
//#define _PRINT_TASK_BY_PRIO_STRUCTURE

//#define  _PRINT_BEGIN_OF_CONSUMPTION_TIMES 
//#define  _PRINT_END_OF_CONSUMPTION_TIMES 
//#define _ADDITIONAL_DEBUG_MESSAGES
//#define _PRINT_TICK_TIMER_EVENTS

//#define _PRINT_TASK_STATE_TRANSITION
//#define _PRINT_TASK_RELEASE_EVENTS
#define _PRINT_TASK_COMPLETION

#define _WARN_TASKS_NOT_ASSIGNED_TO_SCHED
#define _WARN_SCHED_UPDATES_GLOBAL_SIMULATION_TIME

//#define _PRINT_ACCUMULATION_OF_CONSUMPTION_TIMES

#define _REPORT_CONFIGURATION_COMPLETION_TDMA_BUS
#define _WARN_POSSIBLE_ACCURACY_INCONSISTENCIES_FOR_TDMA_BUS
#define _WARN_CYCLE_TIME_NOT_CONFIGURED_FOR_TDMA_BUS
#define _WARN_CYCLE_TIME_BIGGER_FOR_TDMA_BUS
#define _REPORT_TDMA_BUS_CONFIGURATION

#define _REPORT_COMMUNICATION_DELAYS

#define _REPORT_AUTOMATIC_CHANNEL_MAPPINGS
#define _PRINT_BINDIND_INFO

#define _REPORT_FIFO_BUFFER_AUTOMAP_FAILURE
#define _REPORT_FIFO_BUFFER_TOKEN_SIZE_DETERMINATION
#define _REPORT_FIFO_BUFFER_NETIF_DETECT_WRITER_PE

#define _WARN_THROUGHPUT_MONITOR_ISSUES
#define _REPORT_NUMBER_OF_MATCHES_IN_STATIONARITY_ASSESSMENT

#define _WARN_SMALL_NETIF_TX_BUFFER
#define _REPORT_NETIF_SEND_REQUESTS
#define _REPORT_NO_STATIC_COMM_SCHEDULING_FOR_NETIF
#define _REPORT_NETIF_DISPATCH

#define _REPORT_SYSTEM_COMMUNICATION_WRITE_ACCESSES
#define _REPORT_SYSTEM_COMMUNICATION_READ_ACCESSES
#define _REPORT_SYSTEM_COMMUNICATION_WRITE_COMPLETION
#define _REPORT_SYSTEM_COMMUNICATION_READ_COMPLETION

#define _WARN_LINKS_FOUND_WITHOUT_ASSOCIATED_INFORMATION	

#define _REPORT_IO_WRITE_ACCESS
#define _REPORT_IO_WRITE_COMPLETION
#define _REPORT_IO_READ_ACCESS
#define _REPORT_IO_READ_COMPLETION
#define _REPORT_INIT_TOKENS_INJECTION

// environment defaults
#define _REPORT_ENV_MESSAGES	

// static analysis reports
#define _REPORT_DEFAULT_STATIC_ANALYSIS
#define _REPORT_COMMON_STATIC_ANALYSIS_OVERRIDE
#define _REPORT_STATIC_ANALYSIS_ADDITION
#define _REPORT_STATIC_ANALYSIS_OVERRIDE	
#define _REPORT_STATIC_ANALYSIS_EXECUTION

#endif

#ifdef MINIMUM_VERBOSITY

#define _PRINT_VALIDATORS_PASS

#endif

} // namespace kista

#endif
