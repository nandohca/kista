/*****************************************************************************

  twotasks2.cpp
  
  Two task example used as a simple example which serves to show 
  the modelling of periodic task sets in kista, mapped to a scheduler
  in turn mapped to a single processor, and how to obtain different reporting results

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January

  - In this example with a periodical taskset (thus WITH WAITS ON PERIODs) you can explore
	* how to specify periodic tasks and how to do it with compact formats
	
	* that waiting times appear in the tasks, so the utilization of the 
	  platform can be less than 100% (assuming single processor)
	  
	* The impact of scheduling times. They will not have
	  impact on deadlines fulfilment and even in "useful" utilization (that of the
	  tasks) provided if are sufficiently small compared to waiting times
	  
	* Play with WCETS and check how eliminating preemption can provoke
	  loosing deadlines
	 
	* Adding/removing time slicing and check the overhead of scheduling times
	  

 *****************************************************************************/

// define just one of the following three styles for describing the periodic task 2
//#define VERBOSE_VERSION_OF_T2
//#define COMPACT_ALTERNATIVE_1_OF_T2
#define COMPACT_ALTERNATIVE_2_OF_T2

#define _USE_HYPERPERIOD_AS_GLOBAL_SIM_TIME
     
#include "kista.hpp"

sc_time PERIOD_T1(300,SC_MS);
sc_time PERIOD_T2(500,SC_MS);

//sc_time WCET_T1(100,SC_MS);
sc_time WCET_T1(50,SC_MS);
sc_time WCET_T2(200,SC_MS);


// Max utilization 1/3 + 2/5 = 0,733

// two periodical tasks with different consumptions
     
     void task1()
     {
		while(true) {
		  cout << "task 1 " << " t_begin=" << sc_time_stamp() << endl;
		  //consume(WCET_T1);
		  consume_WCET(); // is the same once WCET is settled in the properties
		  wait_period();
		}
     }   

#ifdef VERBOSE_VERSION_OF_T2
	 void task2()
     {
		while(true) {
		  cout << "task 2 " << " t_begin=" << sc_time_stamp() << endl;
		  //consume(WCET_T1);
		  consume_WCET(); // is the same once WCET is settled in the properties
		  wait_period();
		}
     }   
#endif

// Compact alternatives
// function based
#ifdef COMPACT_ALTERNATIVE_1_OF_T2
	 // task 2 specified simply as a void(void) C-function
     void task2_functionality()
     {
		  cout << "task 2 " << " t_begin=" << sc_time_stamp() << endl; // function body
     }
     
     PERIODIC_TASK(task2,task2_functionality);
#endif

// macro based
#ifdef COMPACT_ALTERNATIVE_2_OF_T2
	// being macro delimits the beginning of the body of the function
	 BEGIN_PERIODIC_TASK(task2)
	 	  cout << "task 2 " << " t_begin=" << sc_time_stamp() << endl; // function body
	 END_PERIODIC_TASK
	// end macro delimits the end of the body of the function	 
#endif


int sc_main (int, char *[]) {
  
   // Definition of a task set (set to be assigned to a processor)
   // with the kista::taskset_by_name_t and task_info_t classes
   
   taskset_by_name_t task_set1;
   
   task_info_t task_info_t1("task1",task1);
   task_info_t task_info_t2("task2",task2);
   
   task_info_t1.set_WCET(WCET_T1);
   task_info_t2.set_WCET(WCET_T2);

   task_info_t1.set_Period(PERIOD_T1);
   task_info_t2.set_Period(PERIOD_T2);
   
   task_set1["task1"] = &task_info_t1;
   task_set1["task2"] = &task_info_t2;

//   gs->tasks_assigned["task1"] = task_info_t("task1",task1, gs);
//   gs->tasks_assigned["task2"] = task_info_t("task2",task2, gs);

   // Declaration of an scheduler and assignation of the task set
   // with the kista::scheduler class
   scheduler scheduler1(&task_set1, "scheduler1"); 
   
   // Equivalently, the scheduler can be declared...
//   scheduler *scheduler1;
	// ... and assigned the task set afterwards, at construnction time
//   scheduler1 = new scheduler(&task_set1, "scheduler1"); // by default, non-preemptive, one processor
   
   // configuring the scheduler

   scheduler1.set_preemptive();

	// enabling time slicing
//   scheduler1.enable_time_slicing();

   // Tracing of the tasks and scheduler ocupation signals
   // with the kista::trace method of the kista::scheduler class
   scheduler1.trace_utilizations();

#ifdef _USE_HYPERPERIOD_AS_GLOBAL_SIM_TIME

   // sc_start( get_hyperperiod(task_set1) );

   // It is equivalant to do
   set_global_simulation_time_limit( get_hyperperiod(task_set1) ); // Set exactly one hyperperiod of task set 1 as global simulation time limit
   sc_start(global_sim_time);

#else
   // This way, an user specific simulation time is settled
   // sc_start(1,SC_SEC);
   
   // It is equivalent to write (default value stablished in KisTA defaults file)
   sc_start(global_sim_time);
#endif
   
   sc_stop();

   taskset_by_name_t::iterator ts_it;

   cout << "-------------------------" << endl;   
   cout << "POST-SIM. REPORTS" << endl;
   cout << "-------------------------" << endl;
   
   cout << "*********************************************" << endl;
   cout << " Utilization and Starvation Report " << endl;
   cout << "*********************************************" << endl;
   
   cout << "Hyperperiod of task set task_set1: " << get_hyperperiod(task_set1) << endl;
   
   for(ts_it = task_set1.begin(); ts_it != task_set1.end(); ts_it++) {
	      cout << ts_it->first << " task utilization : (static) " << (float)(ts_it->second->utilization()*100.0) << "%  (simulation): " << (float)(scheduler1.get_task_utilization(ts_it->first)*100.0) << "%" << endl;
   }
   
   cout << "Task Set utilization :  (static) " << (float)(utilization(task_set1)*100.0) << "%  (simulation) " << (float)(scheduler1.get_tasks_utilization()*100.0) << "%" << endl;
   
   for(ts_it = task_set1.begin(); ts_it != task_set1.end(); ts_it++) {
	      cout << ts_it->first << " task density : (static) " << (float)(ts_it->second->density()*100.0) << "%" << endl;
   }

   cout << "Task Set density :  (static) " << (float)(density(task_set1)*100.0) << "% " << endl;
         
   cout << scheduler1.name() << " (sim) number of schedulings: " << (unsigned int)scheduler1.get_number_of_schedulings() << endl;
   cout << scheduler1.name() << " (sim) scheduler utilization : " << (float)(scheduler1.get_scheduler_utilization()*100.0) << "%" << endl;   
   cout << scheduler1.name() << " (sim) platform utilization : " << (float)(scheduler1.get_platform_utilization()*100.0) << "%" << endl;
   cout << "last simulation time: " << sc_time_stamp() << endl;

   if(scheduler1.assess_starvation()!=true) {
	   cout << scheduler1.name() << " scheduler: No starvation detected. All tasks had chance to execute." << endl;
   }
   
   scheduler1.report_miss_deadlines();
      
   return 0;
   
}
