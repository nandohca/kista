/*****************************************************************************

  density_monotonic_ex.cpp
  
	This example shows the usage of density monotonic example
	as an example of other less usual scheduling policies that can
	be integrated and analyzed in in KisTA, contrasted to two well
	known static scheduling policies, RMS and DMS.

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January

Notes:
 - Experiment_A uses rate monotonic. Given the assigned values
   Task1 gets the more prioritary, then Task2 and then task3,
   which does not gets the system work without
   deadline misses, since task3 misses deadlines. 
   
 - Experiment_B uses deadline monotonic. Given the assigned values,
   task2 becomes the more prioritary since it is the one with the shorter
   deadline, then task3 and then task1 (intrinsic deadline of 300ms).
   This experiment illustrates the optimality of DM for this types of 
   sets, as it can be seen that DM manages to schedule the same taskset
   without missed deadlines.
 
  *  which does not gets the system work without
   deadline misses yet, because task 3 has higher density (although not the lesser deadline).

 - Experiment_C uses density monotonic as an example of different and not as
   usual scheduling algorithms which can be easly integrated in KisTA.
   It is seen that with this fixed priority policy results are much
   worse for this example, reflected in that both task1 and task2 loose deadlines.
   In effect, given the assigned values, in this case task3 becomes the
   most prioritary given its density, then task2 and task1.
   Then it can be observed how the response times of task1 and task2 
   increase over their deadlines.
	 
 *****************************************************************************/

#define _EXPERIMENT_A
//#define _EXPERIMENT_B
//#define _EXPERIMENT_C
     
#include "kista.hpp"

sc_time WCET_T1(10,SC_MS);
sc_time WCET_T2(70,SC_MS);
sc_time WCET_T3(160,SC_MS);

// task 1 intrinsic deadline = period
sc_time DEADLINE_T2(200,SC_MS);
sc_time DEADLINE_T3(240,SC_MS);

sc_time PERIOD_T1(300,SC_MS);
sc_time PERIOD_T2(400,SC_MS);
sc_time PERIOD_T3(500,SC_MS);


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

BEGIN_PERIODIC_TASK(task3)
	  cout << "task 3 " << " t_begin=" << sc_time_stamp() << endl; // function body
END_PERIODIC_TASK

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
   
// task 1 intrinsic deadline = period
   task_info_t2.set_Deadline(DEADLINE_T2);
   task_info_t3.set_Deadline(DEADLINE_T3);
  
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
   scheduler1.set_rate_monotonic();
#endif

#ifdef _EXPERIMENT_B   
   scheduler1.set_deadline_monotonic();
#endif

#ifdef _EXPERIMENT_C
   scheduler1.set_density_monotonic();
#endif

   // Tracing of the tasks and scheduler ocupation signals
   // with the kista::trace method of the kista::scheduler class
   scheduler1.trace_utilizations();

   set_global_simulation_time_limit( get_hyperperiod(task_set1) );
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
   cout << scheduler1.name() << " (sim) number of context switches: " << (unsigned int)scheduler1.get_number_of_context_switches() << endl;
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
