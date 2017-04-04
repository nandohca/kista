/*****************************************************************************

  three_ptasks_and_dynamic_prio.cpp
  
    Three task example used as a simple example which serves to ilustrate 
    dynamic scheduling in KisTA,  specifically EDF, with mapping to an
    scheduler mapped to a single processing element.

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January

Notes:

  - Example based on slides from G. Lipari, Scuola Superiore Sant'Anna, Pisa, Italy
    available in http://retis.sssup.it/~lipari/courses/str06/10.edf.pdf

	* EXPERIMENT O (as it is the code now): See how the rate monotonic scheduling (RMS)
	  miss deadline since the utilization (23/24=95.83%) is bigger than the bound for RMS.
	  The deadline is missed by task 3.
	  
	* EXPERIMENT A (enabling the corresponging define variable):
		See how EDF managed
	
	* Play with the scheduling delays (in EDF should be bigger than in static
	  dispatching, so it is a factor to take into account)

 *****************************************************************************/

#define _EXPERIMENT_A
     
#include "kista.hpp"

sc_time PERIOD_T1(400,SC_MS);
sc_time PERIOD_T2(600,SC_MS);
sc_time PERIOD_T3(800,SC_MS);

sc_time WCET_T1(100,SC_MS);
sc_time WCET_T2(200,SC_MS);
sc_time WCET_T3(300,SC_MS);

// task 1 specified simply as a void(void) C-function
void task1_functionality()
{
  cout << "task 1 " << " t_begin=" << sc_time_stamp() << endl; // function body
}
PERIODIC_TASK(task1,task1_functionality);

void task2_functionality()
{
  cout << "task 2 " << " t_begin=" << sc_time_stamp() << endl; // function body
}

PERIODIC_TASK(task2,task2_functionality);

void task3_functionality()
{
  cout << "task 3 " << " t_begin=" << sc_time_stamp() << endl; // function body
}

PERIODIC_TASK(task3,task3_functionality);


int sc_main (int, char *[]) {
  
   // Definition of a task set (set to be assigned to a processor)
   // with the kista::taskset_by_name_t and task_info_t classes
   
   taskset_by_name_t task_set1;
   
   task_info_t task_info_t1("task1",task1);
   task_info_t task_info_t2("task2",task2);
   task_info_t task_info_t3("task3",task2);
   
   task_info_t1.set_WCET(WCET_T1);
   task_info_t2.set_WCET(WCET_T2);
   task_info_t3.set_WCET(WCET_T3);
   
   task_info_t1.set_Period(PERIOD_T1);
   task_info_t2.set_Period(PERIOD_T2);
   task_info_t3.set_Period(PERIOD_T3);
  
   task_set1["task1"] = &task_info_t1;
   task_set1["task2"] = &task_info_t2;
   task_set1["task3"] = &task_info_t3;

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

#ifdef _EXPERIMENT_A
   printf("Set EDF policy\n");
// ANY of the following estatements is equivalent thanks to the default value and short style methods
//   scheduler1.set_dispatch_policy(DYNAMIC_PRIORITIES);
//   scheduler1.set_dynamic_dispatch_policy(EARLY_DEADLINE_FIRST);
   scheduler1.set_earliest_deadline_first();
#else
   printf("Set RM policy\n");
   scheduler1.set_rate_monotonic();
#endif

   // Tracing of the tasks and scheduler ocupation signals
   // with the kista::trace method of the kista::scheduler class
   scheduler1.trace_utilizations();


   set_global_simulation_time_limit(sc_time(900,SC_MS));
   
   //get_hyperperiod(task_set1) ); 

   sc_start( get_hyperperiod(task_set1) );
   
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
