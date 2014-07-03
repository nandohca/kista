/*****************************************************************************

  two_synch_tasks_semaphore.cpp
  
		Example of system-level and SW model with scheduling (because there is only one processing resource),
		where two semaphores are used to synchronize the access to a common shared buffer
 
  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February
   
  Notes:
     The example ilustrates the use of two semaphores (which can be understood
     as equivalent to a registered event queue synchronization mechanism),
     both at system-level and at SW modelling level, where the tasks are
     assigned to one (single processor) scheduler.
     
     In the example, the two semaphores perform a synchronization which
     avoid races and thus read of invalid data and step over data, which
     provokes actual data loss.
     Specifically, the read semaphore is used to void read of invalid data,
     and the write semaphore to avoid overstep the circular buffer.
     
     At the system-level, tasks are SystemC process which can run in truly
     parallelism.
      
     At the SW modelling level, notifications to the KisTA kernel
     obligue the parallelization.
      
     Several scheduling policies and alternatives are checked to see that
     the synchronization primitive fulfils the expected purpose
     regardless the scheduling policies and the consuming policies
     
 *****************************************************************************/

// The following variables enable to define 16 experiments...

// ... depending on the synchronization mechanisms...
#define _USING_SEMAPHORE_FOR_PROTECTING_THE_ACCESS
//#define _USING_FLAG_FOR_PROTECTING_THE_ACCESS

// ... on the relative rates of the consumer processers
#define _CONSUMER_PROCESS_FASTER

// ... and system-level (EXPERIMENT_1) of KISTA MODELS with
// some scheduling policies: static cycle executive (EXPERIMENT_2),
// static user priorities with task1 more priority (EXPERIMENT_3) and
// static user priorities with task2 more priority (EXPERIMENT_4) and
//  Uncomment only one experiment variable at once!!
//define EXPERIMENT_1
#define EXPERIMENT_2
//#define EXPERIMENT_3
//#define EXPERIMENT_4


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
// cycle executive is used
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

	template<class T>
	class unprotected_circular_buffer {
		public:
			unprotected_circular_buffer(unsigned int _size) {
				size = _size;
				data_container = new T[_size];
				push_index=0;
				pop_index=0;
				empty_flag = true;
			}
			
			void push(T& data_token) {
				data_container[push_index] = data_token;
				if(push_index == (size-1)) push_index=0;
				else push_index++;
				empty_flag = false;
			}

			T pop() {
				T token;
				token = data_container[pop_index];
				if(pop_index == (size-1)) pop_index=0;
				else pop_index++;
				if(pop_index == push_index) empty_flag = true;
				return token;
			}
			
			inline bool empty() {return empty;}
			
			inline bool full() {
				return (!empty())&&(pop_index==push_index);
			}
			
		private:
			unsigned int size;
			unsigned int push_index, pop_index;
			T* data_container;
			bool empty_flag;
	};


	// instance of the unprotected circular buffer
	unprotected_circular_buffer<unsigned int> unprotected_cbuff(3);

#ifdef _USING_FLAG_FOR_PROTECTING_THE_ACCESS
	protected_event flag1;
#endif	

#ifdef _USING_SEMAPHORE_FOR_PROTECTING_THE_ACCESS
	semaphore write_sem(3);
	semaphore read_sem(0);
#endif	
	
    void task1()   // producer task
     {
		unsigned int data_token = 0;
		
		while(true) {
		  cout << "\ttask 1 " << " starts some computation at t=" << sc_time_stamp() << endl;
		  consume(t_prod);
          cout << "\ttask 1 " << " dumps data on the circular buffer at t=" << sc_time_stamp() << endl;		  
		  
#ifdef _USING_SEMAPHORE_FOR_PROTECTING_THE_ACCESS
		  write_sem.wait();
#endif		
		  unprotected_cbuff.push(data_token);

#ifdef _USING_SEMAPHORE_FOR_PROTECTING_THE_ACCESS
		  read_sem.post();
#endif				  
		  data_token++;
		  
#ifdef _USING_FLAG_FOR_PROTECTING_THE_ACCESS
		  flag1.set();
#endif		  
  
		}
     }

     void task2()  // consumer task
     {
		while(true) {
			cout << "Task 2 waits for data at time " << sc_time_stamp() << endl;
#ifdef _USING_FLAG_FOR_PROTECTING_THE_ACCESS			
			wait(flag1);
#endif

#ifdef _USING_SEMAPHORE_FOR_PROTECTING_THE_ACCESS
			read_sem.wait();
#endif
			// KisTA model
		    cout << "task 2 " << "READ " << unprotected_cbuff.pop() << " from circular buffer at t=" << sc_time_stamp() << endl;
			consume(t_cons);
			cout << "task 2 " << "ends some processing at t=" << sc_time_stamp() << endl;
#ifdef _USING_SEMAPHORE_FOR_PROTECTING_THE_ACCESS
			write_sem.post();
#endif		    
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

   set_global_simulation_time_limit(sc_time(1.2,SC_SEC));

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
