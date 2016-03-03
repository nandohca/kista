/*****************************************************************************

  kista.hpp
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January

 *****************************************************************************/

#ifndef KISTA_HPP
#define KISTA_HPP

#include <systemc.h>


// Predefined KisTA variable: _SYSTEM_LEVEL_BYPASS
// If defined, KisTA understands that the user wants to do a 
// system-level model, only relying on "pure" SystemC
//
// Then KisTA limits itsself to provide the following translations:
//
//  KisTA:							SystemC
//  -----------     					--------
//  consume(sc_time)					wait(sc_time)
//	wait_synch(sc_event_and_list)		wait(sc_event_and_list)
//	wait_synch(sc_event_or_list)		wait(sc_event_or_list)
//	wait_synch(sc_event_and_list)		wait(sc_event_and_list)
//

#ifdef _SYSTEM_LEVEL_BYPASS

// Configuration of the System-Level bypass
#define _CONSUME_EQ_WAIT_IN_SYSTEM_LEVEL

#include "system_level_bypass/system_level_bypass.hpp"

#else

#include "defaults.hpp"

#include "global_elements.hpp"
#include "kista_version.hpp"
//#include "processing_element.hpp"
#include "app_element.hpp"
#include "application_t.hpp"
#include "task_info_t.hpp"
#include "link_t.hpp"
#include "link_info_t.hpp"
#include "logic_link.hpp"
#include "auxiliar.hpp"
#include "taskset_by_name_t.hpp"
#include "scheduler.hpp"
#include "scheduler_penalties.hpp"
#include "scheduling_time_calculators.hpp"
#include "hw_resource.hpp"
#include "processing_element.hpp"
#include "memory_resource.hpp"
// user tasks constructs
#include "user_task_constructs.hpp"
#include "mapped_links_t.hpp"

//communication resource ( generic network)
#include "comm_res/phy_link_t.hpp"
#include "comm_res/phy_link_info_t.hpp"
#include "comm_res/comm_res.hpp"
#include "comm_res/logic_comm_res.hpp"
#include "comm_res/phy_comm_res.hpp"

// Specific platform communication resources
// bus
#include "comm_res/buses/tdma_bus.hpp"

// network

//   synchronization&communication constructs
#include "synch_comm/memoryless_event.hpp"
#include "synch_comm/registered_event.hpp"
#include "synch_comm/semaphore.hpp"
//#include "synch_comm/fifo_buffer.hpp"

// facilities
#include "draw.hpp"
#include "tikz_activity_trace.hpp"

// analysis utilities
#include "load_analysis/load_analysis.hpp"

// -------------------------------------------------------------------------------
// SYNCHRONIZATION
// -------------------------------------------------------------------------------
//
// Many times the same constructs, with subtle semantic differences or just
// more or less refinements are used with very different names in the different
// fields (system-level, SW and HW).
//
// KisTA provides constructs (alias) for synchronization and communication
// which can have a correspondence ins ystem-level models (e.g. SystemC),
// and in SW,
// as an abstract link between the system-level and SW world.
// 
// In the most basic cases, these constructs basically provide and alias for
// SystemC system-level constructs, to make them more meaningful and let
// that users closer the SW world can associate to constructs present in RTOS,
// and implements the link to the kista kernel by notifying events related
// to the construct.
// The more clear example is the registered_event.
// 

//
// System-Level						KisTA 	 			Actual SW example
// SystemC				HetSC
// -----------------------------------------------------------------------------
// sc_event							memoryless_event		no much, e.g. POSIX signal ignored depending on the state of the processm, (eCos) event flags when more than one notifies...
// sc_event+variable				registered_event 		(eCos) event flag, condition variable (in many RTOS APIs) on a boolean .,
// sc_semaphore						registered_event_queue	semaphores (present in many RTOS APIs
// sc_fifo				uc_fifo		blocking_fifo_buf		message queue,  etc
// sc_transport			uc_rv		rendez_vous				ADA rendez-vous
//
// Other variants which can be considered are memoryless event queues and registered event queues
//
// memoryless event queues are not as interesting, but
// one can realise that a semaphore is actually a kind of registered event queue, since a post is basically add one more event,
// then, the size of the semaphore is the size of the event queue, that is, the maximum size of the variable accounting the events 
// In KisTA, both "registered_event_queue" or "semaphore" names are supported.
//
//
// At the end, alls these constructs are covered by the blocking fifo buffer
//   which adds buffering capability
//   and  (it is enough to overlook the transported data w)
//
//

// plugins     
#include "annotation_plugin/annotation_plugin.hpp"

// templates (here while not separated compilation)
#include "synch_comm/fifo_buffer.hpp"
#include "synch_comm/shared_var.hpp"

using namespace kista;

#endif

#endif
