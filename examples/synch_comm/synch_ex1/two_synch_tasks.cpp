/*****************************************************************************

  two_synch_tasks_flag.cpp
  
		Example of modelling of memoryless event synchronization at a System-Level and
		at a SW modelling level with scheduling (because there is only one processing resource)
 
  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February
   
  Notes:
     The example shows how the memoryless (or "pure") event synchronization
     is actually a  mechanism difficult to use and not quite suitable at a SW level.
     
     A "memoryless event" mechanism, that is, an event without memory is
     straightforwardly modelled in SystemC as a sc_event.
     At SW level its literal translation could be a POSIX signal with
     or without registering and which can be ignored if the process is
     not waiting for it.
     In KisTA, it is modelled through SystemC events, but using the
     wait_synch() calls.
     
     As this example reflect (EXPERIMENT_1), the SystemC model with the
     pure event notificacion can work always and it is simply used.
     It is because infinite commputation&communication resources can be assumed,  
     so task1 and task2 execute in truly concurrency.
     Therefore, despite SystemC sc_events are not registered, task2 catches
     the event launched by task1 because it is always waiting for it.
     
     In any case, using raw events it is still "dangerous" and requires
     to either consider event lost or all the casuistic to avoid it.
     In the EXPERIMENT_2 it is easily seen if the user only places a wait
     in delta or in time before the first wait statement,
     then the system-level model do not get the alternance
     because task1 notification is lost.
           
     However, KisTA models that scheduling times are never null
     Therefore, event 1 notification from task1 will be lost, since
     task2 did not start executed first and so it did not executed the
     statement which makes the task2 passing to a state waiting and ready
     for catching event notification. This is what it happens both
     in EXPERIMENT_3 (using a preemptive cyclic executive policy)
     and EXPERIMENT_4 (using a static scheduling with user assigned
     priorities, where task 1 is more prioritary than task2).

     Even if the scheduling time modelleded in KisTA is 0,
     KisTA introduces one delta delay which models the event lost
     which would always happen in the real system.
     This can be confirmed by configuring the scheduling time in KisTA
     to SC_ZERO_TIME for experiments 3 and 4.
      
     In this example there is a way to make the alternance work,
     it is by setting static priorities and forcing task2 to have more
     priority than task1 (EXPERIMENT_5). Then, it is ensured that task2
     will be always ready because it executes first at the beggining the
     and at any time task 1 and task 2 are ready.
     By doing so, enabling EXPERIMENT_5, compiling and executing the
     KisTA model, the user can check it works and trace the simulation.
     
     This example shows that, however, that:
     *  a SW synchronization through pure events is tricky and difficult to implement.
     *  a system-level (in SystemC) synchronization through pure events is simpler
     due to ilimited resources in computation and communication, although it still
     requires to care about los of events.
     *  pure events do not work the same in system-level and in SW level, since in the
     latter the limitation of resources (at least computation onces) involves a sequentialization
      which introduces more scenarios of even lost.
     * The latter means that, in SW synthesis, it is not a good idea to
     perform the SW implementation of a system-level communication&synchronization
     mechanism relying on pure events (e.g. SystemC channels),
     by a direct translation of system-level events into software events. 
     
     Indeed, this leverages the approach adopted by some SW generation
     methodologies, such as SWGen, which substitute the system-level channel
     implementation (tipically SystemC channels or HetSC channels relying
     on sc_events) by a SW implementation based on the RTOS synchronization
     primitives which can be considered semantically equivalent at system-level
     and independent from SW scheduler configuration,
     instead of providing a SW counterpart for the SystemC sc_event.  
      
//
//_SYSTEM_LEVEL_BYPASS is a KisTA configuration variable
// If defined, KisTA understands that the user wants to do a 
// system-level model, only relying on "pure" SystemC
//
// Then KisTA makes the following translations:
//
//  KisTA:							SystemC
//  -----------     					--------
//  consume(sc_time)					no delay or wait(sc_time) (depend on _CONSUME_EQ_WAIT_IN_SYSTEM_LEVEL)
//	wait_synch(sc_event_and_list)		wait(sc_event_and_list)
//	wait_synch(sc_event_or_list)		wait(sc_event_or_list)
//	wait_synch(sc_event_and_list)		wait(sc_event_and_list)
//
//

 *****************************************************************************/

// Uncomment only once experiment variable at once!!
//#define EXPERIMENT_1
//#define EXPERIMENT_2
#define EXPERIMENT_3
//#define EXPERIMENT_4
//#define EXPERIMENT_5

#ifdef EXPERIMENT_1
#define _SYSTEM_LEVEL_BYPASS
#endif

#ifdef EXPERIMENT_2
#define _SYSTEM_LEVEL_BYPASS
#define _INTRODUCE_DELTA_DELAY_IN_TASK_2_OF_SYSTEM_LEVEL_MODEL
#endif

#ifdef EXPERIMENT_3
// cycle executive is used. In this case, KisTA selects the task1 first
// as it could happen in the reality, and it is seems it does not work 
#undef _WITH_USER_PRIORITIES_A
#undef _WITH_USER_PRIORITIES_B
#endif

#ifdef EXPERIMENT_4
#define _WITH_USER_PRIORITIES_A
#define _SET_STATIC_PRIORITY_WITH_USER_PRIORITIES
#endif

#ifdef EXPERIMENT_5
#define _WITH_USER_PRIORITIES_B
#define _SET_STATIC_PRIORITY_WITH_USER_PRIORITIES
#endif

#include "kista.hpp"

// two (non-periodical) tasks with different consumptions
     
     memoryless_event event1;
     memoryless_event event2;
     
     void task1()
     {
		while(true) {
		  cout << "task 1 " << " begins or resumes at t=" << sc_time_stamp() << endl;
		  consume(sc_time(100,SC_MS));
          cout << "task 1 " << " notifies and wait at t=" << sc_time_stamp() << endl;		  
		  event1.notify(SC_ZERO_TIME);		
		  wait(event2);		  
		}
     }

     void task2()
     {
		while(true) {
#ifdef _INTRODUCE_DELTA_DELAY_IN_TASK_2_OF_SYSTEM_LEVEL_MODEL
			wait(sc_time(101,SC_MS));
#endif			
			cout << "Task 2 waits for event1 at time " << sc_time_stamp() << endl;
			wait(event1);
			// KisTA model
		    cout << "task 2 " << " resumed at t=" << sc_time_stamp() << endl;
			consume(sc_time(200,SC_MS));
			event2.notify(SC_ZERO_TIME);			
		}
     }
     

#ifdef _SYSTEM_LEVEL_BYPASS
SC_MODULE(top) {
public:
	
	//SC_HAS_PROCESS(top);
	
	SC_CTOR(top) {
		sc_spawn_options spawn_options;

		sc_spawn(
				sc_bind( task1 ),
				"task1",
				&spawn_options
				);
								
		sc_spawn(
				sc_bind( task2 ),
				"task2",
				&spawn_options
				);				
				
	}
	
};
#endif

int sc_main (int, char *[]) {

#ifdef _SYSTEM_LEVEL_BYPASS

	top top_m("top_m");

	sc_start(1,SC_SEC); // otherwise, the simulation never stop due to starvation
	sc_stop();
	
#else
   // Definition of a task set (set to be assigned to a processor)
   // with the kista::taskset_by_name_t and task_info_t classes
   
   taskset_by_name_t task_set1;
   
   task_info_t task_info_t1("task1",task1);
   task_info_t task_info_t2("task2",task2);

#ifdef _WITH_USER_PRIORITIES_A
   task_info_t1.set_priority(1);
   task_info_t2.set_priority(2);
#endif

#ifdef _WITH_USER_PRIORITIES_B
   task_info_t1.set_priority(2);
   task_info_t2.set_priority(1);
#endif
   
   task_set1["task1"] = &task_info_t1;
   task_set1["task2"] = &task_info_t2;

//   gs->tasks_assigned["task1"] = task_info_t("task1",task1, gs);
//   gs->tasks_assigned["task2"] = task_info_t("task2",task2, gs);

   // Declaration of an scheduler and assignation of the task set
   // with the kista::scheduler class
   scheduler scheduler1(&task_set1, "scheduler1"); 
   
   // Equivalently, the scheduler can be declared...
//   scheduler *scheduler1;
	// ... and assigned the task set afterwards
//   scheduler1 = new scheduler(&task_set1, "scheduler1"); // by default, non-preemptive, one processor
   
   // configuring the scheduler
	// enabling time slicing
   
   scheduler1.set_preemptive();
#ifdef _SET_STATIC_PRIORITY_WITH_USER_PRIORITIES
   scheduler1.set_static_priorities_policy(USER_PRIORITIES);
#endif

   // Tracing of the tasks and scheduler ocupation signals
   // with the kista::trace method of the kista::scheduler class
   scheduler1.trace_utilizations();
  
   sc_start(1,SC_SEC); // otherwise, the simulation never stop due to starvation 
   
   sc_stop(); // to ensure call to call backs (end_of_simulation),
              //  this is requires for a right accounting of execution times, and starvation assesment

   taskset_by_name_t::iterator ts_it;

   cout << "-------------------------" << endl;   
   cout << "Simulation reports" << endl;
   cout << "-------------------------" << endl;
   
   for(ts_it = task_set1.begin(); ts_it != task_set1.end(); ts_it++) {
	      cout << ts_it->first << " task utilization : (after simulation): " << (float)(scheduler1.get_task_utilization(ts_it->first)*100.0) << "%" << endl;
   }
   
   cout << "Task Set utilization :  (after simulation) " << (float)(scheduler1.get_tasks_utilization()*100.0) << "%" << endl;
         
   cout << scheduler1.name() << " (sim) number of schedulings: " << (unsigned int)scheduler1.get_number_of_schedulings() << endl;
   cout << scheduler1.name() << " (sim) scheduler utilization: " << (float)(scheduler1.get_scheduler_utilization()*100.0) << "%" << endl;   
   cout << scheduler1.name() << " (sim) platform utilization : " << (float)(scheduler1.get_platform_utilization()*100.0) << "%" << endl;
   cout << "last simulation time: " << sc_time_stamp() << endl;

   if(scheduler1.assess_starvation()!=true) {
	   cout << scheduler1.name() << " scheduler: No starvation detected. All tasks had chance to execute." << endl;
   }
#endif
   
   return 0;
     
}
