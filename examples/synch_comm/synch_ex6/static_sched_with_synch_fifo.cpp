/*****************************************************************************

  static_sched_with_fifo.cpp
  
        This example is used to check the combination of
        fifo buffer synch&comm channels with static scheduling.
        
        (This example is extended in comm_res example folder to check the use of generic platform communication resources (generic network)
                
  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 May
 
   Note: 
   * 
 *****************************************************************************/
     
#include "kista.hpp"

#define _SYNCHRONIZE_T1_AND_T3

//#define _ALTERNATIVE_WITHOUT_MACROS_FOR_T1

//#define _WCET_T3_FASTER

//
//      Notice also the results of the reports, and of the starvation assesment
//      at the end of simulation
//
unsigned int data2, data3, data4;

sc_fifo<unsigned int> ch;
//fifo_buffer<unsigned int> ch("inter_ch",1);

//fifo_buffer<unsigned int> ch("ch",1);

//registered_event flag1;
fifo_buffer<unsigned int> inter_ch("inter_ch",1);

#ifdef _SYNCHRONIZE_T1_AND_T3
fifo_buffer<unsigned int> inter_ch2("inter_ch2",1);
#endif


SC_MODULE(prod) {
  void prod_proc() {

    unsigned int var=7;
    
	wait(sc_time(50,SC_MS));
    
	while(true) { // release after each hyperperiod
		ch.write(var);
		wait(sc_time(300,SC_MS));
		var++;
	}
  }

  SC_CTOR(prod) {
	SC_THREAD(prod_proc);
  }
};


sc_time WCET_task1(100,SC_MS);
sc_time WCET_task2(200,SC_MS);

#ifdef _WCET_T3_FASTER
sc_time WCET_task3(275,SC_MS);
#else
sc_time WCET_task3(325,SC_MS);
#endif

// two (non-periodical) tasks with different consumptions

// Following version does not work properly for more than one scheduler, 
// because the data to inter_ch2, and thus its internal events are 
// produced before the actual computation time required by task1 is consumed ... * (follows below)
/*
BEGIN_TASK(task1)
	data3=inter_ch.read();
	cout << "task_1: recv. data=" << data3 << " at t=" << sc_time_stamp() << endl;
#ifdef _SYNCHRONIZE_T1_AND_T3	
	inter_ch2.write(data3);
#endif
END_TASK_AFTER_WCET
*/

// ...* Thus we need the following version, more general
#ifdef _ALTERNATIVE_WITHOUT_MACROS_FOR_T1

BEGIN_TASK(task1)
	data3=inter_ch.read();
	cout << "task_1: recv. data=" << data3 << " at t=" << sc_time_stamp() << endl;
    consume_WCET();
#ifdef _SYNCHRONIZE_T1_AND_T3	
	inter_ch2.write(data3);
#endif		  
	yield();
  }
}

#else

// ... A macro is provided to facilitate the capture of dataflows
BEGIN_TASK(task1,
	data3=inter_ch.read();
)
	cout << "task_1: recv. data=" << data3 << " at t=" << sc_time_stamp() << endl;
END_TASK_AFTER_WCET_AND(
	#ifdef _SYNCHRONIZE_T1_AND_T3	
		inter_ch2.write(data3);
	#endif
)

#endif


BEGIN_TASK(task2)
	data2 = ch.read();
	cout << "task 2: recv. data=" << data2 << " at t=" << sc_time_stamp() << endl;
	data3=data2+10;
	inter_ch.write(data3);
END_TASK_AFTER_WCET

BEGIN_TASK(task3)
#ifdef _SYNCHRONIZE_T1_AND_T3
	data4 = inter_ch2.read();
#endif	
	cout << "task 3: " << "recv. data=" << data4 << " at t=" << sc_time_stamp() << endl;
END_TASK_AFTER_WCET



void reports_for_scheduler(scheduler &sched, taskset_by_name_t &taskset)
{
  // print ocupation of scheduler...
   cout << sched.name() << " scheduler utilization : " << (float)(sched.get_scheduler_utilization()*100.0) << "%" << endl;
   
   // ... and of assigned tasks ...
   taskset_by_name_t::iterator ts_it;
   for(ts_it = taskset.begin(); ts_it != taskset.end(); ts_it++) {
	      cout << ts_it->first << " task utilization : " << (float)(sched.get_task_utilization(ts_it->first)*100.0) << "%" << endl;
   }
   cout << "Total tasks utilization : " << (float)(sched.get_tasks_utilization()*100.0) << "%" << endl;
   
   cout << sched.name() << " number of schedulings: " << (unsigned int)sched.get_number_of_schedulings() << endl;
   cout << sched.name() << " platform utilization : " << (float)(sched.get_platform_utilization()*100.0) << "%" << endl;
   
   sched.assess_starvation();
   
   cout << endl;
 
}

int sc_main (int, char *[]) {

   // DESCRIPTION OF THE KISTA MODEL (POST-PROCESSING)
     
   // Definition of a task set (set to be assigned to a processor)
   // with the kista::taskset_by_name_t and task_info_t classes
   
   taskset_by_name_t task_set1;
   taskset_by_name_t task_set2;
   
   task_info_t task1_info("task1",task1);
   task_info_t task2_info("task2",task2);
   task_info_t task3_info("task3",task3);
   
   // set WCETS
   task1_info.set_WCET(WCET_task1);
   task2_info.set_WCET(WCET_task2);
   task3_info.set_WCET(WCET_task3);

   // create task set
   task_set1["task1"] = &task1_info;
   task_set1["task2"] = &task2_info;
   task_set2["task3"] = &task3_info;
   
   // Declaration of an scheduler and assignation of the task set
   // with the kista::scheduler class
   scheduler scheduler1(&task_set1, "scheduler1"); 
   scheduler scheduler2(&task_set2, "scheduler2"); 
   
   // Equivalently, the scheduler can be declared...
//   scheduler *scheduler1;
	// ... and assigned the task set afterwards
//   scheduler1 = new scheduler(&task_set1, "scheduler1"); // by default, non-preemptive, one processor
   
   // configuring the scheduler 1
   scheduler1.set_policy(STATIC_SCHEDULING);
   scheduler1.set_static_schedule({"task2","task1"});

   // configuring the scheduler 2
   scheduler2.set_policy(STATIC_SCHEDULING);
   scheduler2.set_static_schedule({"task3"});

   // HW architecture
   processing_element PE1("PE1");
   processing_element PE2("PE2");
      
   // Mapping of schedulers to processors
   scheduler1.map_to(&PE1);
   scheduler2.map_to(&PE2);
  
    //gen_network mynet("mynet"); 
    //comm_res mynet("mynet"); 
   
   // Tracing of the tasks and scheduler ocupation signals
   // with the kista::trace method of the kista::scheduler class
   scheduler1.trace_utilizations();
   
//   scheduler1.set_sufficient_global_simulation_time(0);  // provoques KisTA
//   scheduler1.set_sufficient_global_simulation_time(); // equivalent to ...set_sufficient_global_simulation_time(1)
//   scheduler1.set_sufficient_global_simulation_time(2);
   
   set_global_simulation_time_limit(sc_time(1,SC_SEC));
   
   // And the environment!
   prod prod1("prod1");
   
   // SIMULATION START
   sc_start();
   
   // REPORTS (POST-PROCESSING)
   reports_for_scheduler(scheduler1, task_set1);
   reports_for_scheduler(scheduler2, task_set2);
      
   cout << "last simulation time:" << sc_time_stamp() << endl;
   return 0;
   
}
