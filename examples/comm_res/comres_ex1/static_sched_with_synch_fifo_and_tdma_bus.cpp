/*****************************************************************************

  static_sched_with_fifo_and_tdma_bus.cpp
  
        Similarly to example 6 of synch&comm examples, this example merges fifo communication
        resources at application level with static scheduling at SW platform level.
        Additionally, it adds the effect of communication resources at HW platform level,
        specifically, a TDMA bus.
        
        (This example can be used also to check the use of generic platform communication resources (generic network)
                
  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 May
 
   Note: This example currently presents an error of accounting with
         the initial environment event. It is because Kista does not
          know that the state of the task is waiting for the event channel
          
          That reqires actually that the environment communicates with
          tKisTA specific channels which sends the corresponding events to the
          KisTA kernel.
          
          Currently that is to be completed.
 	
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
   
   // connect tasks through fifos (required to stablish logic links)      
#define SYS_LEVEL_CHANNEL_BINDING_BY_NAME
#ifdef SYS_LEVEL_CHANNEL_BINDING_BY_NAME
   inter_ch.bind("task2", "task1"); // sender task to receiver task
   inter_ch2.bind("task1", "task3"); //sender task to receiver task
#else   
   inter_ch.bind(&task2_info, &task1_info); // sender task to receiver task
   inter_ch2.bind(&task1_info, &task3_info); //sender task to receiver task
#endif
    
// not suported yet:
//   inter_ch.bind(task2, task1); // sender task to receiver task
//   inter_ch2.bind(task1, task3); //sender task to receiver task
   
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

   // -----------------------------------------------
   // HW architecture
   // -----------------------------------------------

   // processing elements
   processing_element PE1("PE1");
   processing_element PE2("PE2");

//#define _GEN_NETWORK

#ifdef _GEN_NETWORK        
    //gen_network mynet("mynet"); 
    
    phy_comm_res_t mynet("mynet"); 
    PE1.connect(&mynet);
    PE2.connect(&mynet);
    
#else 

   // Communication resources
   
#define PAYLOAD_BYTES 16

   tdma_bus		tdmab1("tdmab1");
   tdmab1.set_n_channels(90); 			//
										// Typical values for: Flexray (src: http://www.ni.com/white-paper/3352/en/)
										     // Tx. Line speed : 2.5, 5, 10Mbps
   tdmab1.set_slot_time(sc_time(1,SC_US));	 // 1 Mbps typical duration of minislot (or macrotick): 1 us 

#define _CONF_PAYLOAD_BY_CAPACITY_AND_RATE
#ifdef _CONF_PAYLOAD_BY_CAPACITY_AND_RATE
   tdmab1.set_channel_capacity((5+PAYLOAD_BYTES+3)*8); // 10 bits/channel
										// Header:  5 bytes
										// Payload: 254 bytes (max) (we take 16bytes)
										// CRC: 	3 bytes
   //tdmab1.set_channel_payload_rate((double)PAYLOAD_BYTES/(double)(5+PAYLOAD_BYTES+3)); // 16/23 = 2/3 = 0.6667
   tdmab1.set_channel_payload_rate(0.6667); // 16/23 = 2/3 = 0.6667
#else
    // directly input the payload
   tdmab1.set_channel_payload_bits(PAYLOAD_BYTES*8);	// 10 bits/channel (payload rate = 1) 
										// 0..254: 254 bytes (max)
#endif
   										
   tdmab1.set_cycle_time(sc_time(1,SC_MS));  // typically, in the range 1m-5ms)
										// This should involve:
										// 10 channels of 1Mbps (channel BW) (no guard time)
										// 
										
   // bind processing elements to communication resources
   PE1.connect(&tdmab1);
   PE2.connect(&tdmab1);
   
#endif
   
   // -----------------------------------------------
   // Mapping
   // -----------------------------------------------
   
   // Mapping of schedulers to processors
   scheduler1.map_to(&PE1);
   scheduler2.map_to(&PE2);
   
   // Mapping of system-level channels to communication resources
   // (in this case, there is only one platform communication resource, the tdma bus instance, tdmab1)
   // inter_ch.map_to(tdmab1);
   // inter_ch2.map_to(tdmab1);
   // In this example it is not required actually since the physical connections can be and are actually inferred
   // once the PEs are connected to a single communication infrastructure
   
   // Allocation of communication resources
   // -------------------------------------------------
   // STYLE 1 (to match A-DSE tool)
   // Mapping (actually, allocation) of communication resources to logical links
#ifndef _GEN_NETWORK     
   tdmab1.allocate_channels(&PE1,3);
#endif

   // STYLE 2 (TBD)
   // Allocation of BW for system-level channels which is automatically converted onto allocation of communication resources
   // ... inter_ch2.allocate_BW();
   
   // configure non ideality for PEs
   PE1.setIntraComMaxP2Pdelay_per_bit(sc_time(1,SC_US));
   
   // sketch report
   sketch_report.set_file_name("comm_res_ex1");
   sketch_report.enable();
   
   
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
