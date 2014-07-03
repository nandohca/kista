/*****************************************************************************

  registered_event.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

 *****************************************************************************/

#ifndef _SEMAPHORE_CPP
#define _SEMAPHORE_CPP

#include "semaphore.hpp"

#include "task_info_t.hpp"
#include "global_elements.hpp"

namespace kista {
	
//	using namespace sc_core;
	
	semaphore::semaphore( int _size) : sc_semaphore(_size) {}
	
	semaphore::semaphore( const char* _name, int _size) : sc_semaphore(_size) {}

	int semaphore::wait() {

		sc_process_handle 	current_task;
		task_info_t		*current_task_info;
		int ret_value;
					
		current_task=sc_get_current_process_handle();
		current_task_info = task_info_by_phandle[current_task];
			
		current_task_info->finished_flag = false; // to avoid it to be considered completion of the task
		
		current_task_info->waiting_comm_sync_signal.write(true);

		ret_value=(*this).sc_semaphore::wait(); // calls to the sc_semaphore wait method

		// be aware that the following would not work
		//static_cast<sc_semaphore&>(*this).wait();
		//static_cast<sc_core::sc_semaphore*>(this)->wait();

		current_task_info->waiting_comm_sync_signal.write(false);
			
		current_task_info->finished_flag = true;

		// guard to let the user task go on executing after release only if the scheduler has granted it a processor
		if(current_task_info->active_signal.read()==false) sc_core::wait(current_task_info->active_signal.posedge_event());

// ALTERNATIVE TO SAVE TIME
		
		// Notice that in a real software implementation it is not valid
		// since once entered in the second branch (size semaphore>0)
		// the code could be preempted under a preemptive scheduling,
		// and other task empty the semaphore.
		// Then, the task would block without accounting that time.
		
		// Since the SsytemC implementation has not "consumes", nor this
		// code can be preempted it is valid to save time
/*
		if(static_cast<sc_semaphore&>(*this).get_value()==0) { // in this case, the accesing tasks is going to go to waiting state due to synchronization

			sc_process_handle 	current_task;
			task_info_t		*current_task_info;
					
			current_task=sc_get_current_process_handle();
			current_task_info = task_info_by_phandle[current_task];
			
			current_task_info->finished_flag = false; // to avoid it to be considered completion of the task
			
			current_task_info->waiting_comm_sync_signal.write(true);
			
			static_cast<sc_semaphore&>(*this).wait(); // calls to the sc_semaphore wait method

			current_task_info->waiting_comm_sync_signal.write(false);
			
			current_task_info->finished_flag = true;

			// guard to let the user task go on executing after release only if the scheduler has granted it a processor
			if(current_task_info->active_signal.read()==false) wait(current_task_info->active_signal.posedge_event());
			
		}
		  // else, there is no need to wait, not even to notify the KisTA kernel or change the 
		  // finished flag
        else { 		  
		  // we do the call, but in principle it will it should not be necessary, here we now it will not block
		  static_cast<sc_semaphore&>(*this).wait(); // calls to the sc_semaphore wait method
		}
*/
		return ret_value;
	}

} // namespace KisTA

#endif
