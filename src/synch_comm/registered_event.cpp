/*****************************************************************************

  registered_event.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

 *****************************************************************************/

#ifndef _REGISTERED_EVENT_CPP
#define _REGISTERED_EVENT_CPP

#include "registered_event.hpp"

#include "task_info_t.hpp"
#include "global_elements.hpp"

namespace kista {
	
	registered_event::registered_event(bool wait_release_unset_flag_par) {
		flag = false;
		wait_release_unset_flag = wait_release_unset_flag_par;
	}
	
	void registered_event::set() {
		flag = true;
		notify(SC_ZERO_TIME);	
	}

	void registered_event::unset() {
		flag = false;
	}

	void registered_event::wait() {
		if(!flag) sc_core::wait(static_cast<sc_event&>(*this)); // calls to the functions on wait(sc_event)
		if(wait_release_unset_flag) unset();		
	}
	
	void wait(registered_event &re) {
		
		sc_process_handle 	current_task;
		task_info_t		*current_task_info;
				
		current_task=sc_get_current_process_handle();
		current_task_info = task_info_by_phandle[current_task];
		
		current_task_info->finished_flag = false; // to avoid it to be considered completion of the task
		
		current_task_info->waiting_comm_sync_signal.write(true);

		// wait( re ); // This would not work! because it would be a recursive call!
		re.wait(); // Calls to the wait method of the memoryless_event object (wich is the inherited one from the sc_event)
		
		current_task_info->waiting_comm_sync_signal.write(false);
		
		current_task_info->finished_flag = true;

		// guard to let the user task go on executing after relelase only if the scheduler has granted it a processor
		if(current_task_info->active_signal.read()==false) wait(current_task_info->active_signal.posedge_event());
				
	}

} // namespace KisTA

#endif
