/*****************************************************************************

  several_tasks_static_sched.cpp
        These example serves to see what happens when static scheduled
        tasks include some synchronization mechanism.
        In this case, the synchronization mechanism is redundant because
        tasks are maped to the same local scheduler and the static scheduler
        already states the order. Moreover, synchronizations can "contradict"
        such an order (this happens in this example when undefining
        _COHERENT_AND_REDUNDANT_SYNCH) leading the simulation to deadlock.
        
        The example is also used to see the effect of events with some timing
        from a "pure SystemC" environment (for it enable either 
        ENV_EVENT_TO_TASK1 or ENV_EVENT_TO_TASK2).
        
  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 May

  Note:
 	

        
 *****************************************************************************/
     
#include "kista.hpp"

#define _ALTERNATIVE_3
#define _COHERENT_AND_REDUNDANT_SYNCH

// enable one of these variables to show the effect
// of the environment to the KisTA part
//#define ENV_EVENT_TO_TASK1
//#define ENV_EVENT_TO_TASK2

//
//      Notice also the results of the reports, and of the starvation assesment
//      at the end of simulation
//

registered_event flag1;

registered_event flag_env;

SC_MODULE(prod) {
  void prod_proc() {
#ifdef ENV_EVENT_TO_TASK1	  
	wait(sc_time(250,SC_MS));
#endif
#ifdef ENV_EVENT_TO_TASK2
	wait(sc_time(50,SC_MS));
#endif	
	while(true) { // release after each hyperperiod
		flag_env.set();
		wait(sc_time(300,SC_MS));
	}
  }

  SC_CTOR(prod) {
	SC_THREAD(prod_proc);
  }
};


sc_time WCET_task1(100,SC_MS);
sc_time WCET_task2(200,SC_MS);

// two (non-periodical) tasks with different consumptions
  
#ifdef _COHERENT_AND_REDUNDANT_SYNCH   

BEGIN_TASK(task1)
#ifdef ENV_EVENT_TO_TASK1
wait(flag_env);
#endif
	wait(flag1);
	cout << "task 1 " << " t=" << sc_time_stamp() << endl;
END_TASK_AFTER_WCET

BEGIN_TASK(task2)
#ifdef ENV_EVENT_TO_TASK2
wait(flag_env);	
#endif
	cout << "task 2 " << " t=" << sc_time_stamp() << endl;
	flag1.set();
END_TASK_AFTER_WCET

#else

BEGIN_TASK(task1)
	cout << "task 1 " << " t=" << sc_time_stamp() << endl;
	flag1.set();	
END_TASK_AFTER_WCET

BEGIN_TASK(task2)
	wait(flag1);
	cout << "task 2 " << " t=" << sc_time_stamp() << endl;
END_TASK_AFTER_WCET

#endif

int sc_main (int, char *[]) {

   // DESCRIPTION OF THE KISTA MODEL (POST-PROCESSING)
     
   // Definition of a task set (set to be assigned to a processor)
   // with the kista::taskset_by_name_t and task_info_t classes
   
   taskset_by_name_t task_set1;
   
   task_info_t task1_info("task1",task1);
   task_info_t task2_info("task2",task2);
   
   // set WCETS
   task1_info.set_WCET(WCET_task1);
   task2_info.set_WCET(WCET_task2);

   // create task set
   task_set1["task1"] = &task1_info;
   task_set1["task2"] = &task2_info;

   // Declaration of an scheduler and assignation of the task set
   // with the kista::scheduler class
   scheduler scheduler1(&task_set1, "scheduler1"); 
   
   // Equivalently, the scheduler can be declared...
//   scheduler *scheduler1;
	// ... and assigned the task set afterwards
//   scheduler1 = new scheduler(&task_set1, "scheduler1"); // by default, non-preemptive, one processor
   
   // configuring the scheduler
   scheduler1.set_policy(STATIC_SCHEDULING);

#ifdef _ALTERNATIVE_1
   scheduler1.set_static_schedule({"task2","task1"});
#endif

#ifdef _ALTERNATIVE_2
   static_schedule_by_task_names_t   my_static_schedule = {"task2","task1"};   
   scheduler1.set_static_schedule(my_static_schedule);
#endif

#ifdef _ALTERNATIVE_3
   static_schedule_by_task_info_pointers_t   my_static_schedule = {&task2_info,&task1_info};   
   scheduler1.set_static_schedule_by_task_ptrs(my_static_schedule);
#endif

   
   // Tracing of the tasks and scheduler ocupation signals
   // with the kista::trace method of the kista::scheduler class
   scheduler1.trace_utilizations();
   
//   scheduler1.set_sufficient_global_simulation_time(0);  // provoques KisTA
//   scheduler1.set_sufficient_global_simulation_time(); // equivalent to ...set_sufficient_global_simulation_time(1)
   scheduler1.set_sufficient_global_simulation_time(2);
   
   // And the environment!
   prod prod1("prod1");
   
   // SIMULATION START
   sc_start();
   
   // REPORTS (POST-PROCESSING)
   
   // print ocupation of scheduler...
   cout << scheduler1.name() << " scheduler utilization : " << (float)(scheduler1.get_scheduler_utilization()*100.0) << "%" << endl;
   
   // ... and of assigned tasks ...
   taskset_by_name_t::iterator ts_it;
   for(ts_it = task_set1.begin(); ts_it != task_set1.end(); ts_it++) {
	      cout << ts_it->first << " task utilization : " << (float)(scheduler1.get_task_utilization(ts_it->first)*100.0) << "%" << endl;
   }
   cout << "Total tasks utilization : " << (float)(scheduler1.get_tasks_utilization()*100.0) << "%" << endl;
   
   cout << scheduler1.name() << " number of schedulings: " << (unsigned int)scheduler1.get_number_of_schedulings() << endl;
   cout << scheduler1.name() << " platform utilization : " << (float)(scheduler1.get_platform_utilization()*100.0) << "%" << endl;
   
   scheduler1.assess_starvation();
   
   cout << "last simulation time:" << sc_time_stamp() << endl;
   return 0;
   
}
