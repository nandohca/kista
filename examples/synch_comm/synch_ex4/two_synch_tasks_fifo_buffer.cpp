/*****************************************************************************

  two_synch_tasks_fifo_buffer.cpp
  
		Example of system-level and SW model  with scheduling (because there is only one processing resource),
		where a blocking fifo buffer is used to synchronize and communicate two asynchronous tasks.

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February
   
  Notes:
  * 
    The example serves to show how the code structure and synchronization semantics
  of the blocking fifo serve to preserve functional determinism, whatever
  the scheduling policies configured and consumption times.
   
   However, the scheduling policies have an impact on the time behaviour.
   In this example, KisTA serves to show that when the consumer is given
   the higest priority (EXPERIMENT 4), then the fifo buffer is immpediately
   emptied since task2 (consumer) starts to execute once there is data.
   That means that there will not be more than one data token in the buffer.
    
   The EXPERIMENT 3 shows the opposite thing. The producer is given priority,
   which means that it works always till completely fill the buffer, regardless
   the relative speed of tasks.
   
   EXPERIMENT 2 uses CYCLE_EXECUTIVE (default) dynamic policy of the KisTA
   scheduler model. In this example shows a behaviour similar (not equal) 
   to the one of experiment 3. That happens because the scheduler just makes a cyclic
   disptaching triggered AFTER EACH NEW SCHEDULING, and because the consumer
   task2, once it reads the token, immediately releases task 1,
   which was in communication state.
   Since there are only two tasks, and it is cyclic without minding if the
   task 2 performs some consumption after reading the data, it returns the
   control to task1.
   Notice that such "immediately" is because in the example there is not
   time advance for the communication (communication penalty), since the
   example does not include those penalties.
  
  (An extension of the example with an independent task can show how the
  scheduling policy can actually have involvements on response times,
  thus in time behaviour)* 
    
     
 *****************************************************************************/

// The following variables enable to define 16 experiments...


// ... on the relative rates of the consumer processers
//#define _CONSUMER_PROCESS_FASTER

// ... and system-level (EXPERIMENT_1) of KISTA MODELS with
// some scheduling policies: static cycle executive (EXPERIMENT_2),
// static user priorities with task1 more priority (EXPERIMENT_3) and
// static user priorities with task2 more priority (EXPERIMENT_4) and
//  Uncomment only one experiment variable at once!!
//#define EXPERIMENT_1
#define EXPERIMENT_2
//#define EXPERIMENT_3
//#define EXPERIMENT_4

#ifdef EXPERIMENT_1
#define _SYSTEM_LEVEL_BYPASS
#endif

#ifdef EXPERIMENT_2
// cycle executive is used. In this case, KisTA selects the task1 first
// as it could happen in the reality, and it is seems it does not work
#undef _WITH_USER_PRIORITIES_A
#undef _WITH_USER_PRIORITIES_B
#endif

#ifdef EXPERIMENT_3
#define _WITH_USER_PRIORITIES_A
#define _SET_STATIC_PRIORITIES_POLICY
#endif

#ifdef EXPERIMENT_4
#define _WITH_USER_PRIORITIES_B
#define _SET_STATIC_PRIORITIES_POLICY
#endif

#include "kista.hpp"

sc_time t_prod(100,SC_MS);
#ifdef _CONSUMER_PROCESS_FASTER
   // consumer faster
sc_time t_cons(50,SC_MS);
#else
   // producer faster
sc_time t_cons(200,SC_MS);
#endif

	// instance of the unprotected circular buffer
	// In this example any of the following styles will provoke the same result
	//fifo_buffer<unsigned int> my_fifobuf(3);
	fifo_buffer<unsigned int> my_fifobuf("my_fifo_buf",3);
	//    fifo_buffer<unsigned int> my_fifobuf("my_fifo_buf",3, task1, task2); // this is possible too, but then you need a forward declaration of
	//                                                                         // tasks or putting the statement after tasks declaration
	//fifo_buffer<unsigned int> my_fifobuf("my_fifo_buf", 3, "task1","task2");
	// the latter style gives information of the system-level link, which can be
	// automatically associated to a physical communication resource.
	// However, in this example there is no physical communication resources,
	// so there is no need for requiring such an information to the user
	
    void task1()   // producer task
     {
		unsigned int data_token = 0;
		
		while(true) {
		  cout << "\ttask 1 " << " starts some computation at t=" << sc_time_stamp() << endl;
		  consume(t_prod);
          cout << "\ttask 1 " << " send data at t=" << sc_time_stamp() << endl;		  
		  
		  my_fifobuf.write(data_token);	

		  data_token++;
  
		}
     }

     void task2()  // consumer task
     {
		unsigned int tmp;
		while(true) {
			cout << "Task 2 waits for data at time " << sc_time_stamp() << endl;
			tmp = my_fifobuf.read();;
			// KisTA model
		    cout << "task 2 " << "READ " << tmp << " at t=" << sc_time_stamp() << endl;
			consume(t_cons);
			cout << "task 2 " << "ends some processing at t=" << sc_time_stamp() << endl;	    
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
 
   // configuring the scheduler
	// enabling time slicing
   
   scheduler1.set_preemptive();
#ifdef _SET_STATIC_PRIORITIES_POLICY
   scheduler1.set_static_priorities_policy(USER_PRIORITIES);
#endif

   // Tracing of the tasks and scheduler ocupation signals
   // with the kista::trace method of the kista::scheduler class
   scheduler1.trace_utilizations();

   set_global_simulation_time_limit(sc_time(10,SC_SEC));

   // activate throughput measurement on the fifo channel
   my_fifobuf.measure_write_throughput();

#ifdef _SET_ROUND_ROBIN   
   sc_start(); // here simulation ends through the simulation limit enabled by default by KisTA library
               // That limit can be changed through the set_global_simulation_time_limit function
#else   
   sc_start(1,SC_SEC); // otherwise, the simulation never stop due to starvation
#endif  
   sc_stop(); // to ensure call to call backs (end_of_simulation),
              //  this is requires for a right accounting of execution times, and starvation assesment
   taskset_by_name_t::iterator ts_it;

   cout << "*****************************************" << endl;
   cout << "Simulation reports" << endl;
   cout << "*****************************************" << endl;
   cout << "Application related metrics:" << endl;
   cout << "-----------------------------" << endl;
   for(ts_it = task_set1.begin(); ts_it != task_set1.end(); ts_it++) {
	      cout << ts_it->first << " task utilization : (after simulation): " << (float)(scheduler1.get_task_utilization(ts_it->first)*100.0) << "%" << endl;
   }
   
   cout << "Task Set utilization :  (after simulation) " << (float)(scheduler1.get_tasks_utilization()*100.0) << "%" << endl;

   cout << "Fifo buffer channel " << my_fifobuf.name() << " throughput: " << (float)my_fifobuf.get_write_throughput() << " (bps)" << endl;
   cout << "Fifo buffer channel " << my_fifobuf.name() << " stationary throughput: " << (float)my_fifobuf.get_write_stationary_throughput() << " (bps)" << endl;
   
   cout << endl;
   cout << "Platform related metrics:" << endl;
   cout << "-------------------------" << endl;
         
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
