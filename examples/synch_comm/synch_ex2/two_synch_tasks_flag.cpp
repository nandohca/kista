/*****************************************************************************

  two_synch_tasks_flag.cpp
  
		Example of modelling of registered event (flag) synchronization at a System-Level and
		at a SW modelling level with scheduling (because there is only one processing resource)
 
  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February
   
  Notes:
     The example ilustrates the use of the registered event synchronization
     mechanism.
     In SW it is usually undertood as a flag or an event flag (e.g. eCoS RTOS).
     The registered event synchronization mechanism can be understood as
     a memoryless event plus a register to store the arrival of a data.
      
     This example has the same "synchronization structure" as the two_synch_tasks.cpp
     example. However, it can be seen how the registered event solves for the
     5 cases well the alternance.
     
     This mechanism has been safer since the registered event avoid the loss
     off events.
      
     However it does not mean yet that event loss is guaranteed under any circunstance.
     In this example, the strict alternance avoid loss of events.
     However, if a producer set a registered event twice before the release,
     only one event will be actually accounted.
     Therefore, it would be an error if an user intends to transfer one
     data associated to one registered event, and notifices twice before
     the receiver is triggered.
     It would only work if the receiver nows that an event corresponds
     to the sending of certain amount of data, and those data have been
     dumped on a storage media without overlap.
    
     Because of that, those conditions have to be avoided in the code.
     A good policy is to use more powerful communication&synchronization constructs
     with this basic primitive where the implementation avoids that situation.
      
     KisTA provides them, as can be seen in other examples.

     This is illustrated with the blocking_fifo_buf kista construct in a
     later example   
     
 *****************************************************************************/

// Uncomment only once experiment variable at once!!
//#define EXPERIMENT_1
//#define EXPERIMENT_2
#define EXPERIMENT_3
//#define EXPERIMENT_4
//#define EXPERIMENT_5

// To try the usage of separated unset of the registered event
//     KisTA event flag is, by default, immediately unsettled once 
//     the waiting task is released,
//     although it can be configured to not to be unset then, but
//     only when the user code calls unset()
//    (e.g. eCos flags are setted and unset with different calls)

//#define _USE_FLAG_WITH_SEPARATE_UNSET  

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
#define _SET_STATIC_PRIORITIES_POLICY
#endif

#ifdef EXPERIMENT_5
#define _WITH_USER_PRIORITIES_B
#define _SET_STATIC_PRIORITIES_POLICY
#endif

#include "kista.hpp"

// two (non-periodical) tasks with different consumptions
#ifdef _USE_FLAG_WITH_SEPARATE_UNSET          
     registered_event flag1(false);
     registered_event flag2(false);
#else     
     registered_event flag1;
     registered_event flag2;
#endif
    
     void task1()
     {
		while(true) {
		  cout << "task 1 " << " begins or resumes at t=" << sc_time_stamp() << endl;
		  consume(sc_time(100,SC_MS));
          cout << "task 1 " << " notifies and wait at t=" << sc_time_stamp() << endl;		  

		  flag1.set();
#ifdef _USE_FLAG_WITH_SEPARATE_UNSET          		  
		  flag2.unset();
#endif		  
		  wait(flag2);
		}
     }

     void task2()
     {
		while(true) {
#ifdef _INTRODUCE_DELTA_DELAY_IN_TASK_2_OF_SYSTEM_LEVEL_MODEL
			wait(sc_time(101,SC_MS));
#endif			
			cout << "Task 2 waits for event1 at time " << sc_time_stamp() << endl;
			wait(flag1);
#ifdef _USE_FLAG_WITH_SEPARATE_UNSET			
			flag1.unset();
#endif			
			// KisTA model
		    cout << "task 2 " << " resumed at t=" << sc_time_stamp() << endl;
			consume(sc_time(200,SC_MS));
			flag2.set();
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
#ifdef _SET_STATIC_PRIORITIES_POLICY
   scheduler1.set_static_priorities_policy(USER_PRIORITIES);
#endif

   // Tracing of the tasks and scheduler ocupation signals
   // with the kista::trace method of the kista::scheduler class
   scheduler1.trace_utilizations();

#ifdef _SET_ROUND_ROBIN   
   sc_start(); // here simulation ends through the simulation limit enabled by default by KisTA library
               // That limit can be changed through the set_global_simulation_time_limit function
#else   
   sc_start(1,SC_SEC); // otherwise, the simulation never stop due to starvation
#endif  
   
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
