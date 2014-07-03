/*****************************************************************************

  two_ptasks_and_static_prio.cpp
  
		This is the brt_ex3 example, extended to show the following features:

		* add scheduling times to the model and extension of scheduling time models which scheduling time calculators
		* TikZ tracing and some of the options enabled by KisTA to control such tracing

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January

Notes:

	Following, the description of brt_ex3 example is reproduced for convenience

	Two task example used to show some static scheduling modelling
	features of kista, with mapping to an scheduler handling
	a single processing element.


  - This example with a periodical taskset (thus WITH WAITS ON PERIODs)
	is similar to ex2 case, but here use it to play with static dispatching policies
	* EXPERIMENT_0 (Disabling all defines): Cycle executive dispatching

	* Experiment A: With user assigned priorities, we can see the effect of
	  using the default dispatching policy (cycle-executive) and after
	  establishing the usage of STATIC_PRIORITIES.
	  
	* Experiment B: Then, we can see the effect of using a rate monotonic
	  scheduling. It is "manually" stated by the KisTA code (assigning the
	  biggest priority to the shortest period). the set_rate_monotic method
	  can be used too. It can be see also how response times for task1 are
	  lesser and response times of task2 bigger.
	  
	* Experiment C: Deadlines are defined for the two tasks. Their densities
	  augment and are bigger than (static) utilizations.
	  Still Rate Monotonic analysis is used, which makes lossing deadlines
	  to task2, which has longer period but much tighter deadline than task1,
	  so higher density.
		
	* Experiment D: Now, deadline monotonic scheduling is stated, and it
	  can be seen how now task 2 (and no task) miss any deadline, since
	  deadline monotonic takes into account which tasks has lesser densities.	  
	 
 *****************************************************************************/

//#define _EXPERIMENT_A
#define _EXPERIMENT_B
//#define _EXPERIMENT_C
//#define _EXPERIMENT_D

#ifdef _EXPERIMENT_A
#define _SET_USER_PRIORITIES  
#define _SET_STATIC_PRIORITIES
#endif

#ifdef _EXPERIMENT_B
// we can let the code set user priorities to see that rate monotonic configuration statement overrides them
#define _SET_USER_PRIORITIES  
#define _SET_RATE_MONOTONIC
#endif

#ifdef _EXPERIMENT_C
#define _SET_USER_DEADLINES
#define _SET_RATE_MONOTONIC
#endif

#ifdef _EXPERIMENT_D
#define _SET_USER_DEADLINES
#define _SET_DEADLINE_MONOTONIC
#endif

     
#include "kista.hpp"

sc_time PERIOD_T1(300,SC_MS);
sc_time PERIOD_T2(500,SC_MS);

sc_time DEADLINE_T1(275,SC_MS);
sc_time DEADLINE_T2(245,SC_MS);

sc_time WCET_T1(50,SC_MS);
sc_time WCET_T2(200,SC_MS);

// Max. utilization in an hyperperiod t1 utilization (1/6) + t2 (utilization) 2/5 = 0,56

// task 1 specified simply as a void(void) C-function
void task1_functionality()
{
  cout << "task 1 " << " t_begin=" << sc_time_stamp() << endl; // function body
}
     
PERIODIC_TASK(task1,task1_functionality);


BEGIN_PERIODIC_TASK(task2)
	  cout << "task 2 " << " t_begin=" << sc_time_stamp() << endl; // function body
END_PERIODIC_TASK


int sc_main (int, char *[]) {
  
	sc_time sim_time;
	
   // Definition of a task set (set to be assigned to a processor)
   // with the kista::taskset_by_name_t and task_info_t classes
   
   taskset_by_name_t task_set1;
   
   task_info_t task_info_t1("task1",task1);
   task_info_t task_info_t2("task2",task2);
   
   task_info_t1.set_WCET(WCET_T1);
   task_info_t2.set_WCET(WCET_T2);

   task_info_t1.set_Period(PERIOD_T1);
   task_info_t2.set_Period(PERIOD_T2);

   //
   // NOW WE ESTABLISH PRIORITIES (In KisTA: the lowest the value, the highest the priority, 0 reserved)
   // (The user assignation of this example is the opposite to Rate Monotonic scheduling)
   // You later can see how the set_rate_monotonic method makes an automatic assignation,
   // which overrides the user assignation once it is invoked afterwards, within the task set
   //
#ifdef _SET_USER_PRIORITIES  
   task_info_t1.set_priority(5);
   task_info_t2.set_priority(3);
#endif

#ifdef _SET_USER_DEADLINES
   task_info_t1.set_Deadline(DEADLINE_T1);
   task_info_t2.set_Deadline(DEADLINE_T2);
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
	// ... and assigned the task set afterwards, at construnction time
//   scheduler1 = new scheduler(&task_set1, "scheduler1"); // by default, non-preemptive, one processor
   
   // configuring the scheduler

   scheduler1.set_preemptive();

	// enabling time slicing (we keep it disabled)
//   scheduler1.enable_time_slicing();

#ifdef _SET_STATIC_PRIORITIES
   scheduler1.set_policy(STATIC_PRIORITIES);
#endif

#ifdef _SET_RATE_MONOTONIC   
   scheduler1.set_rate_monotonic(); // it already includes the set_dispatch_policy(STATIC_PRIORITIES)
#endif

#ifdef _SET_DEADLINE_MONOTONIC   
   scheduler1.set_deadline_monotonic(); // it already includes the set_dispatch_policy(STATIC_PRIORITIES)
#endif

   // Tracing of the tasks and scheduler ocupation signals
   // with the kista::trace method of the kista::scheduler class
   scheduler1.trace_utilizations();
   
   tikz_activity_trace_handler gen_tikz_report_handler;
   
   
	//   create_tikz_activity_trace();
   //gen_tikz_report_handler = create_tikz_activity_trace();
   gen_tikz_report_handler = create_tikz_activity_trace("2tasks_tikz_trace");
   
   //set_scale(gen_tikz_report_handler,1);
   set_landscape(gen_tikz_report_handler);
//   no_text_in_traces(gen_tikz_report_handler);
   do_not_show_unactive_boxes(gen_tikz_report_handler);
   set_time_stamps_max_separation(gen_tikz_report_handler,3);
   
   cluster(gen_tikz_report_handler);
   
//   compact(gen_tikz_report_handler);

   
   sketch_report.set_file_name("ex3_sketch"); // This will set the file name of the sketch report to "ex3_sketch.tex"
												// otherwise, the default name "system_sketch.tex" will be applied
   sketch_report.enable();
//   sketch_report.set_file_name("ex3_sketch"); // Here, it would be overriden, and the the default name used										

   sim_time = get_hyperperiod(task_set1); 
   
   cout << "Hyperperiod of task set : " << sim_time << endl; 
 
   cout << "Global simulation time limit : " << get_global_simulation_time_limit() << endl;
 
   // to enable scheduling times
   enable_scheduling_times();
 
   set_scheduling_time(sc_time(1,SC_NS));
   set_default_scheduling_time_calculators();
 
   sc_start(sim_time);
   
   cout << "End of simulation at " << sc_time_stamp() << endl;
   
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
   scheduler1.report_response_times();
         
   return 0;
   
}
