/*****************************************************************************

  user_task_constructs.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

 *****************************************************************************/

#ifndef USER_TASK_CONSTRUCTS_CPP
#define USER_TASK_CONSTRUCTS_CPP

#include "defaults.hpp"

#include "global_elements.hpp"

#include "task_info_t.hpp"
#include "scheduler.hpp"

namespace kista {

	void set_global_simulation_time_limit(sc_time t) {
#ifdef _PRINT_SETTING_OF_GLOBAL_SIMULATION_TIME_LIMIT		
		cout << "KisTA INFO: Global simulation time limit settled to " << t << endl;
#endif
		if(sc_get_status() & (SC_RUNNING | SC_PAUSED | SC_STOPPED) ) {
			SC_REPORT_ERROR("KisTA", "Global simulation time has to be settled before the simulation start." );
		} else {
			global_sim_time = t;
		}	
	}

	sc_time &get_global_simulation_time_limit() {
		return global_sim_time;
	}
	
// Implementation of CONSUMPTION functions for user tasks
// ***********************************************************

	void consume(sc_time comp_time) {
		
		scheduler *sched_info_p;
		task_info_t *task_info_p;
		
		sc_time start_consumption_time, consumed_time;
		
		sc_process_handle current_task;
		current_task=sc_get_current_process_handle();		
	
        task_info_p  = task_info_by_phandle[current_task];
		sched_info_p = task_info_by_phandle[current_task]->get_scheduler();
		
		task_info_p->set_left_consume_time(comp_time);
				
#ifdef _PRINT_BEGIN_OF_CONSUMPTION_TIMES
cout << "Task " << current_task.name() << ": Starts consumption time of " << comp_time << " at " << sc_time_stamp() << endl;	
#endif

		task_info_p->finished_flag = false;

		if(sched_info_p->is_preemptive()) { // in case there is preemption, there has to yield to the scheduler

			do {	
				
					task_info_p->consumption_signal.write(true);
					
					// guard for ensuring that the consumption is done whenever the task is resumed (activated) by the scheduler)
					if(task_info_p->active_signal.read() == false) wait(task_info_p->active_signal.posedge_event());
					start_consumption_time = sc_time_stamp();
					wait( task_info_p->get_left_consume_time(), task_info_p->active_signal.negedge_event() );
					//wait( task_info_p->get_left_consume_time(), task_info_p->active_signal.negedge_event(), sched_info_p->get_tick_timer_event() );

#ifdef _PRINT_END_OF_CONSUMPTION_TIMES
                    cout << "Task " << current_task.name() << " stops consumption";
                    if(task_info_p->active_signal.read()==false) {
						cout << ", preempted by scheduler, ";
					}
					cout << "at time " << sc_time_stamp() << endl;					
#endif
				
					task_info_p->consumption_signal.write(false);

					consumed_time = sc_time_stamp() - start_consumption_time;
					
					if(task_info_p->get_left_consume_time() > consumed_time) {
						task_info_p->set_left_consume_time(task_info_p->get_left_consume_time() - consumed_time);
					} else {
						task_info_p->set_left_consume_time(SC_ZERO_TIME);
						break;
					}
	
			} while(true);				

#ifdef _PRINT_END_OF_CONSUMPTION_TIMES
cout << "Task " << current_task.name() << " ends consumption at time " << sc_time_stamp() << endl;
#endif
		
		} else { // in case there is no preemption, there is no need to consider an "interruption of the consuming wait" ...
		
		    task_info_p->consumption_signal.write(true);
			
			wait(comp_time); // ... the full time is consumed, while the rest of the processes in the processor are not running...

		    task_info_p->consumption_signal.write(false);
		    				
#ifdef _PRINT_END_OF_CONSUMPTION_TIMES
cout << "Task " << current_task.name() << ": Finished consumption time of " << comp_time << " at " << sc_time_stamp() << endl;	
#endif
		}

		task_info_p->finished_flag = true; // this ensures that a task will pass to finished state if a consume statement is the last statement

	}

	void consume_WCET() {
		sc_process_handle current_task;
		current_task=sc_get_current_process_handle();
		consume( task_info_by_phandle[current_task]->get_WCET() );
	}

//
// Implementation of YIELD function for user tasks
// 

	void yield() {
#ifdef _CHECK_YIELDS_IN_NON_PREEMPTIVE_SCHEDULERS
		std::string msg;
#endif
		task_info_t *task_info_p;
		sc_process_handle current_task;
		scheduler *sched; // to get the scheduler which controls the execution of this task

		current_task=sc_get_current_process_handle();
		task_info_p = task_info_by_phandle[current_task];
		sched = task_info_p->get_scheduler();

		if(sched->get_policy()==STATIC_SCHEDULING) { // static scheduler
			// basically do the same as for the dynamic case, but for non-preemptive case
			sched->yield_event.notify(SC_ZERO_TIME);
			task_info_p->finished_flag = false;
			wait(task_info_p->active_signal.posedge_event());
			task_info_p->finished_flag = true;		
		} else { // dynamic scheduler
			
#ifdef _CHECK_YIELDS_IN_NON_PREEMPTIVE_SCHEDULERS
			if(sched->is_non_preemptive()) {
					msg = "Task ";
					msg += task_info_p->name();
					msg += " performing a yield() call at time ";
					msg += sc_time_stamp().to_string();
					msg +=  ", while executed in the scheduler ";
					msg += task_info_p->get_scheduler()->name();
					msg += ", which is configured as NON-PREEMPTIVE.\n";
#ifdef _IGNORE_YIELDS_IN_NON_PREEMPTIVE_SCHEDULER
					msg += " You have configured KisTA to go on the simulation in this case,";			
					msg += "but take into account that an implementation will get locked at the first yield() call.\n";			
					SC_REPORT_WARNING("KisTA", msg.c_str());				
#else				
					msg += " An actual implementation will get locked at this point,";			
					msg += " You can make KisTA to ignore yield() calls as if they did not exist in the case of NON-PREEMPTIVE case.";
					msg += " For it, configure KisTA by defining _IGNORE_YIELDS_IN_NON_PREEMPTIVE_SCHEDULER.";
					msg += " In this case, KisTA will report yield accesses in non-preemptive schedulers as warnings.";
					SC_REPORT_ERROR("KisTA", msg.c_str());
#endif
			}
#endif		

			// In the non-preemptive case, this point is reached only if _IGNORE_YIELDS_IN_NON_PREEMPTIVE_SCHEDULER
			// is enabled. That is, if the user states to not to report an error and continue.
			// In such a case, the scheduler ignores the yield event, so it will not realise it
			// and still leaves execution control to the task doing yield. 
			// Although the yield even is ignored by the scheduler in such a case, 
			// the generation of the event is also disabled rom here.
			//  - One reason for doing it is to avoid generation of events which will be ignored.
			//  - Other reason is to avoid that a yield done at the end of a finite task,
			//    "mask" the termination in not-preemptive case.
			//    (termination masking would consits in the process getting
			//     stalled in the yield wait, which can be unblocked only if
			//     the scheduler triggers it again. But it cannot, because in
			//     non-preemptive mode, yield-events are masked too, so the 
			//     call to yield do not trigger the scheduler, the only one which
			//     can trigger the other process).
					
			if(sched->is_cooperative()
					||
			   sched->is_preemptive()
			) {
				sched->yield_event.notify(SC_ZERO_TIME);
				task_info_p->finished_flag = false;
				wait(task_info_p->active_signal.posedge_event());
				task_info_p->finished_flag = true;
			}
		}
	}	

//
// Implementation of COMMUNICATION AND SYNCHRONIZATION WAIT functions for user tasks
// ************************************************************************************
	
	void wait_period() {
		
		std::string msg;
		sc_process_handle 	current_task;
		task_info_t		*current_task_info;
				
		current_task=sc_get_current_process_handle();
		current_task_info = task_info_by_phandle[current_task];
		
		current_task_info->finished_flag = false;
		
		if(!current_task_info->is_periodic()) {
			msg = "Calling wait_period method in task ";
			msg += current_task.name();
			msg += ", which is not periodic, is forbidden.";
			SC_REPORT_ERROR("KisTA", msg.c_str());
		}	
			
		sc_time waiting_time;
		sc_time period = current_task_info->get_period();
			
//		cout << current_task.name() << " periodic functionality executed, task will enter now in wait state at time = " << sc_time_stamp() << endl;
		
		current_task_info->end_response_event.notify(SC_ZERO_TIME); // for accounting of response times
		
		// check if after consumption time, the period is violated
		if ( sc_time_stamp() > current_task_info->current_absolute_deadline) {
			 // resumption after the triggerint time expected 
			 // t_current > (t_previous + T)
			current_task_info->miss_deadline_flag = true;
			msg = sc_get_current_process_handle().name();
			msg +=  " tasks MISSED DEADLINE. \n";
			msg += "period = "; msg += period.to_string();
			msg += ", due trigger = "; msg += current_task_info->current_absolute_deadline.to_string();
			msg += ", actual trigger time = "; msg += sc_time_stamp().to_string();	
			SC_REPORT_WARNING("KisTA", msg.c_str());	
			
			// despite missing the deadline, the tasks tries to go on executing
			// resynchronizing in for the next time t_! = fulfilling (last_trigger_time + N*T) > sc_time_stamp()
			// This way, if the consumption times are stable and bigger than T, then the tasks stabilizes to
			// a period N*T, while if the consumptions get down again, it adapts to period T.
//			N=1;
//			while ( sc_time_stamp() > current_task_info->last_trigger_time + N*period) N++;
			
//			waiting_time = current_task_info->last_trigger_time + N*period - sc_time_stamp();
#if _PERIODIC_TASK_RESYNCHRONIZE_WHEN_MISSING_DEADLINE
			do {
				current_task_info->current_absolute_deadline += period;
				wait( current_task_info->task_release_event );	
			} while (sc_time_stamp() > current_task_info->current_absolute_deadline);
#endif
// otherwise, goes on executing immediately

		} else {
			// resumption immediately after (at the same physical time) the triggering time expected 
			// or fullfiling t_current < (t_previous +T)
			current_task_info->waiting_period_signal.write(true);
			// perform the wait in waiting state
			// wait( current_task_info->current_absolute_deadline -  sc_time_stamp() );
			wait( current_task_info->task_release_event );
		}
		
		// updates information about current absolute deadline
		current_task_info->current_absolute_deadline += period;
		
		// updates task state to become waiting state
		current_task_info->waiting_period_signal.write(false);
		
		current_task_info->finished_flag = true; // this ensures that a task will pass to finished state if a wait statement is the last statement
		
		// guard to let the user task go on executing after relelase only if the scheduler has granted it a processor
		if(current_task_info->active_signal.read()==false) wait(current_task_info->active_signal.posedge_event());

	}

} // namespace KisTA

#endif
