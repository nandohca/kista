/*****************************************************************************

  several_tasks_static_sched.cpp
        These example serves to see static scheduling configuration and
        modelling features, including the modelling of non-idealities
        (static scheduling time), and user support such as automatic calculation
        of simulation times to see at least a number of cycles.
        
  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January

  Note:
 		Several tasks are mapped to a single scheduler configured as an
		static scheduler. The static schedule is passed to the scheduler.
		Aspects as a small, but non SC_ZERO_TIME scheduiling time are modelled
		in the example.
		Alternatives show different ways to model the static schedule,
		as well as how simple to model an scheduling is.
		It also shows the capability to detect the modellign of wrong schedules,
		according the assigned set.

Notes 2016/03:
  Extension of basic real time example 5 of KisTA library for showing
  report of energy and power
        
 *****************************************************************************/
     
#include "kista.hpp"

//#define _ALTERNATIVE_1
//#define _ALTERNATIVE_2
#define _ALTERNATIVE_3
//#define _ALTERNATIVE_4
//#define _ALTERNATIVE_5
//#define _ALTERNATIVE_6
//#define _ALTERNATIVE_7
// alternative 7 is that not static schedule is provided, and the corresponding KisTA error should detect it

//
//      Notice also the results of the reports, and of the starvation assesment
//      at the end of simulation
//

sc_time WCET_task1(100,SC_MS);
sc_time WCET_task2(200,SC_MS);
sc_time WCET_task3(300,SC_MS);
sc_time WCET_task4(400,SC_MS);
sc_time WCET_task5(500,SC_MS);

// two (non-periodical) tasks with different consumptions
     
BEGIN_TASK(task1)
		  cout << "task 1 " << " t=" << sc_time_stamp() << endl;
END_TASK_AFTER_WCET

BEGIN_TASK(task2)
		  cout << "task 2 " << " t=" << sc_time_stamp() << endl;	
END_TASK_AFTER_WCET

BEGIN_TASK(task3)
		  cout << "task 3 " << " t=" << sc_time_stamp() << endl;		
END_TASK_AFTER_WCET
     
BEGIN_TASK(task4)
		  cout << "task 4 " << " t=" << sc_time_stamp() << endl;
END_TASK_AFTER_WCET

BEGIN_TASK(task5)
		  cout << "task 5 " << " t=" << sc_time_stamp() << endl;
END_TASK_AFTER_WCET
     
int sc_main (int, char *[]) {

   // DESCRIPTION OF THE KISTA MODEL (POST-PROCESSING)
     
   // Definition of a task set (set to be assigned to a processor)
   // with the kista::taskset_by_name_t and task_info_t classes
   
   taskset_by_name_t task_set1;
   
   task_info_t task1_info("task1",task1);
   task_info_t task2_info("task2",task2);
   task_info_t task3_info("task3",task3);
   task_info_t task4_info("task4",task4);
   task_info_t task5_info("task5",task5);
   
   // set WCETS
   task1_info.set_WCET(WCET_task1);
   task2_info.set_WCET(WCET_task2);
   task3_info.set_WCET(WCET_task3);
   task4_info.set_WCET(WCET_task4);
   task5_info.set_WCET(WCET_task5);
   
   // create task set
   task_set1["task1"] = &task1_info;
   task_set1["task2"] = &task2_info;
   task_set1["task3"] = &task3_info;
   task_set1["task4"] = &task4_info;
   task_set1["task5"] = &task5_info;

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
   scheduler1.set_static_schedule({"task2","task5", "task4", "task1", "task2","task1","task3"});
#endif

#ifdef _ALTERNATIVE_2
   static_schedule_by_task_names_t   my_static_schedule = {"task2","task5", "task4", "task1", "task2","task1","task3"};   
   scheduler1.set_static_schedule(my_static_schedule);
#endif

#ifdef _ALTERNATIVE_3
   static_schedule_by_task_info_pointers_t   my_static_schedule = {&task2_info,&task5_info, &task4_info, &task1_info, &task2_info, &task1_info, &task3_info};   
   scheduler1.set_static_schedule_by_task_ptrs(my_static_schedule);
#endif

#ifdef _ALTERNATIVE_4
   static_schedule_by_task_names_t   my_static_schedule2 = {"task5","task1", "task4", "task1", "task3","task2"};   
   scheduler1.set_static_schedule(my_static_schedule2);
#endif

#ifdef _ALTERNATIVE_5
   static_schedule_by_task_names_t   my_static_schedule3 = {"task2","task5","task4"};    // This is detected by kiss code
   scheduler1.set_static_schedule(my_static_schedule3);
#endif

#ifdef _ALTERNATIVE_6
   static_schedule_by_task_names_t   my_static_schedule3 = {"task2","task5", "task4", "task1", "task2","task1","task3", "task6"};    // This is detected by kiss code
   scheduler1.set_static_schedule(my_static_schedule3);
#endif
   
   // Tracing of the tasks and scheduler ocupation signals
   // with the kista::trace method of the kista::scheduler class
   scheduler1.trace_utilizations();
   
//   scheduler1.set_sufficient_global_simulation_time(0);  // provoques KisTA
//   scheduler1.set_sufficient_global_simulation_time(); // equivalent to ...set_sufficient_global_simulation_time(1)
   scheduler1.set_sufficient_global_simulation_time(2);
   
   sketch_report.set_file_name("ex6_sketch");
   sketch_report.enable();
   
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
   
   // (dynamic) energy report
   //   for scheduler
   cout << "*********************************************" << endl;
   cout << "Energy and power report                      " << endl;
   cout << "*********************************************" << endl;
   cout << "scheduler \"" << scheduler1.name() << "\": consumed_energy =" << scheduler1.get_consumed_energy_J() << " J " << endl;
   cout << "task \"" << task1_info.name() << "\": consumed_energy =" << task1_info.get_consumed_energy_J() << " J " << endl;
   cout << "task \"" << task2_info.name() << "\": consumed_energy =" << task2_info.get_consumed_energy_J() << " J " << endl;
   cout << "task \"" << task3_info.name() << "\": consumed_energy =" << task3_info.get_consumed_energy_J() << " J " << endl;
   cout << "task \"" << task4_info.name() << "\": consumed_energy =" << task4_info.get_consumed_energy_J() << " J " << endl;
   cout << "task \"" << task5_info.name() << "\": consumed_energy =" << task5_info.get_consumed_energy_J() << " J " << endl;
   cout << "*********************************************" << endl;
   
   // starvation assessment
   scheduler1.assess_starvation();
   
   cout << "last simulation time:" << sc_time_stamp() << endl;
   return 0;
   
}
