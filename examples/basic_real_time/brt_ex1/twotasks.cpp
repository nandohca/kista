/*****************************************************************************

  twotasks.cpp
  
  Two task example used as a simple example which serves to check 
  how to model two infinite tasks in kista, and their mapping to
  a scheduler mapped to a single processor.
  The example shows the effect of non-preemptive vs RR scheduling.
 

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January

  - With this example WITHOUT ANY TYPE OF WAITS you can see:
	* How to describe a simple two tasks example of infinite (unbounded tasks)
	  which always consume CPU (never make a "yield")
	  *  (indeed, a limited simulation time has to becused in that case)
	* Make them cooperate making explicit yields
	* However, if time slicing	(preemption) is enabled, and given the FIFO
	  default dispatch policy the two tasks can execute
	* See how to dump reports and tracing
	* what happens when changing time slicing (swap set_time_slicing)
		(time slicing lets equilibrate utilization of each task, but introduces more overhead in scheduling because of the number of schedulings)
	* play with overheads (scheduling time, etc)
	* uses the "asses_starvation" call

 *****************************************************************************/
     
#include "kista.hpp"

#define _EXPERIMENT_1
//#define _EXPERIMENT_2
//#define _EXPERIMENT_3


// experiment 1: check that, by default kista policy is non-preemptive, and
// that only completion of task means that the scheduler will be notified and
// a further scheduling will be done.
// In a fist mode of experiment 1 the user comment _FINITE_TASKS, so to model
// infinite tasks and check what happen. The user will see that without yields
// (commenting _USE_YIELDS one tasks takes the processor and the other is
// starved. If the yields are used (define _USE_YIELDS), it does not solves
// the problem yet, because the default policy of the scheduler is non-preemptive,
// which ignores yields. To solve it, the Experiment2 defines as well the
// cooperative scheduling policy (which triggers the scheduler when the task
// completes or when it makes a yield)
//
// If the user comments _FINITE_TASKS, then it will observe a clear starvation
// because we have to infinite tasks, so the former taking the processors involves
// the starvation of the other
// When defining _FINITE_TASKS, the tasks specified are finite and the completion of
// the first executing, provided that there is at least one consume statement in the task
// (no mater if it is a consume of SC_ZERO_TIME), gives a chance to the other task to execute.
// For it, it is not necessary to change scheduler policy (non-preemptive by default).
// For the default non-preemptive policy, completion is the only efective event, and
// yields will not involve scheduling.
#ifdef _EXPERIMENT_1
#define _FINITE_TASKS
//#define _USE_YIELDS
// Notice that if you comment _SET_COOPERATIVE variable, the scheduler will be non-preemptive
// by default, and one of the tasks will get starved. 
// However, YIELDS SHOULD NOT APPEAR in a Non-Preemptive configuration because they lock the execution
// and prevent further schedulings because of termination.
// That situation is detected by KisTA and rerported .
// KisTA admits through a configuration option to go on the execution, and ignore yield effect
// if the configuration variable _IGNORE_YIELDS_IN_NON_PREEMPTIVE_SCHEDULER is defined.
#endif

// experiment 2: use explicit (user task) yields to make tasks cooperate among them
#ifdef _EXPERIMENT_2
#define _USE_YIELDS
#define _SET_COOPERATIVE
// Notice that if you comment _SET_COOPERATIVE variable, the scheduler will be non-preemptive
// by default, and one of the tasks will get starved
#endif

// experiment 3: use time slicing and a preemptive/cooperative scheduler to make them cooperate without explicit grant
#ifdef _EXPERIMENT_3
#define _SET_ROUND_ROBIN
#define _COMPACT_STYLE
#endif

// two (non-periodical) tasks with different consumptions
     
     void task1()
     {
#ifndef _FINITE_TASKS
		while(true) {
#endif			
		  cout << "task 1 " << " t_begin=" << sc_time_stamp() << endl;
		  consume(sc_time(100,SC_MS));
#ifdef _USE_YIELDS
          yield();
#endif
#ifndef _FINITE_TASKS
		}
#endif		
     }

     void task2()
     {
#ifndef _FINITE_TASKS
		while(true) {
#endif			
		  cout << "task 2 " << " t_begin=" << sc_time_stamp() << endl;
		  consume(sc_time(200,SC_MS));
#ifdef _USE_YIELDS
          yield();
#endif
#ifndef _FINITE_TASKS
		}
#endif		
     }
     

int sc_main (int, char *[]) {
  
   // Definition of a task set (set to be assigned to a processor)
   // with the kista::taskset_by_name_t and task_info_t classes
   
   taskset_by_name_t task_set1;
   
   task_info_t task_info_t1("task1",task1);
   task_info_t task_info_t2("task2",task2);
   
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
#ifdef _SET_ROUND_ROBIN
#ifdef _COMPACT_STYLE
   scheduler1.set_Round_Robin();
#else   
   scheduler1.set_preemptive();
   scheduler1.enable_time_slicing();
#endif   
#endif

#ifdef _SET_COOPERATIVE
   scheduler1.set_preemptive();
#endif

   // Tracing of the tasks and scheduler ocupation signals
   // with the kista::trace method of the kista::scheduler class
   scheduler1.trace_utilizations();

   set_global_simulation_time_limit(sc_time(1,SC_SEC));

#ifdef _SET_ROUND_ROBIN   
   sc_start(); // here simulation ends through the simulation limit enabled by default by KisTA library
               // That limit can be changed through the set_global_simulation_time_limit function
               // The end of the simulation might not exactly end at the time fixed by the global_time_limit,
               // since this limit is checked by the schedulers (so it is done at the time the first scheduling 
               // performed at a time over the global time limit
#else   
   // sc_start()		// in this case, an sc_start would involve a neverending simulation, since the scheduler never gets
                        // the controls and no detection to check if it is over the simulation time limit is done.
   sc_start(1,SC_SEC); // This way, simulation is "abruptly" terminated
						// ("abruptly" in the sense that it can happenm in the middle of the execution of a task
						// not necessary in the execution of 
                         
#endif  
   
   sc_stop(); // to ensure call to call backs (end_of_simulation),
              //  this is required for a right accounting of execution times, and starvation assesment

   taskset_by_name_t::iterator ts_it;

   cout << "-------------------------" << endl;   
   cout << "Simulation reports" << endl;
   cout << "-------------------------" << endl;
   
   for(ts_it = task_set1.begin(); ts_it != task_set1.end(); ts_it++) {
	      cout << ts_it->first << " task utilization : (after simulation): " << (float)(scheduler1.get_task_utilization(ts_it->first)*100.0) << "%" << endl;
   }
   
   cout << "Task Set utilization :  (after simulation) " << (float)(scheduler1.get_tasks_utilization()*100.0) << "%" << endl;
         
   cout << scheduler1.name() << " (sim) number of schedulings: " << (unsigned int)scheduler1.get_number_of_schedulings() << endl;
   cout << scheduler1.name() << " (sim) number of context switches: " << (unsigned int)scheduler1.get_number_of_context_switches() << endl;
   cout << scheduler1.name() << " (sim) scheduler utilization: " << (float)(scheduler1.get_scheduler_utilization()*100.0) << "%" << endl;   
   cout << scheduler1.name() << " (sim) platform utilization : " << (float)(scheduler1.get_platform_utilization()*100.0) << "%" << endl;
   cout << "last simulation time: " << sc_time_stamp() << endl;

   if(scheduler1.assess_starvation()!=true) {
	   cout << scheduler1.name() << " scheduler: No starvation detected. All tasks had chance to execute." << endl;
   }
   
   return 0;
     
}
