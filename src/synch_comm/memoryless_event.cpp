
/*****************************************************************************

  memoryless_event.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

 *****************************************************************************/

#ifndef _MEMORYLESS_EVENT_CPP
#define _MEMORYLESS_EVENT_CPP

#include "memoryless_event.hpp"

#include "task_info_t.hpp"
#include "global_elements.hpp"

namespace kista {

	void memoryless_event::wait() {
		sc_core::wait(static_cast<sc_event&>(*this)); // calls to the functions on wait(sc_event)
	}
	
	void memoryless_event_or_list::wait() {
		sc_core::wait(static_cast<sc_event_or_list&>(*this)); // calls to the functions on wait(sc_event_or_list)
	}
	
	void memoryless_event_and_list::wait() {
		sc_core::wait(static_cast<sc_event_and_list&>(*this)); // calls to the functions on wait(sc_event_and_list)
	}

		// Notes about implementation: 
		// when the user calls wait synch, by default, it is not understood as an
		// end mark for calculation of response times. That is why there is no
		// calls to end_response_event
	void wait(memoryless_event &e) {
		
		sc_process_handle 	current_task;
		task_info_t		*current_task_info;
				
		current_task=sc_get_current_process_handle();
		current_task_info = task_info_by_phandle[current_task];
		
		current_task_info->finished_flag = false; // to avoid it to be considered completion of the task
		
		current_task_info->waiting_comm_sync_signal.write(true);

		// wait( e ); // This would not work! because it would be a recursive call!
		e.wait(); // Calls to the wait method of the memoryless_event object (wich is the interited one from the sc_event)
		
		current_task_info->waiting_comm_sync_signal.write(false);
		
		current_task_info->finished_flag = true;

		// guard to let the user task go on executing after relelase only if the scheduler has grated it a processor
		if(current_task_info->active_signal.read()==false) wait(current_task_info->active_signal.posedge_event());
		
	}

	void wait(memoryless_event_or_list &eol) {

		sc_process_handle 	current_task;
		task_info_t		*current_task_info;
				
		current_task=sc_get_current_process_handle();
		current_task_info = task_info_by_phandle[current_task];
		
		current_task_info->finished_flag = false; // to avoid it to be considered completion of the task
		
		current_task_info->waiting_comm_sync_signal.write(true);
		
		//wait(eol);
		eol.wait();
		
		current_task_info->waiting_comm_sync_signal.write(false);
		
		current_task_info->finished_flag = true;

		// guard to let the user task go on executing after relelase only if the scheduler has grated it a processor
		if(current_task_info->active_signal.read()==false) wait(current_task_info->active_signal.posedge_event());
		
	}

	void wait(memoryless_event_and_list &eal) {
		
		sc_process_handle 	current_task;
		task_info_t		*current_task_info;
				
		current_task=sc_get_current_process_handle();
		current_task_info = task_info_by_phandle[current_task];
		
		current_task_info->finished_flag = false; // to avoid it to be considered completion of the task
		
		current_task_info->waiting_comm_sync_signal.write(true);
				
//		wait(eal);
		eal.wait();
		
		current_task_info->waiting_comm_sync_signal.write(false);
		
		current_task_info->finished_flag = true;

		// guard to let the user task go on executing after relelase only if the scheduler has grated it a processor
		if(current_task_info->active_signal.read()==false) wait(current_task_info->active_signal.posedge_event());
				
	}

} // namespace KisTA

#endif
