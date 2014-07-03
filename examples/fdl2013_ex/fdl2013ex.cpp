/*****************************************************************************

  fdl2013ex.cpp
        
        FDL13 example:
        
        *  Examples where two independent, but fairly simple independent
        task graphs are mapped to one or two schedulers.
        So, it shows two scheduler instances working together
        
        * It also serves  to check the interfacing with the output
        of the mini-zinc based static-DSE framework developed for the FDL example
        
  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 May

  Note:
 		

        
 *****************************************************************************/

#include <map>

#define FIFO_ENV_CHANNEL sc_fifo<unsigned short>
//#define FIFO_ENV_CHANNEL fifo_buffer<unsigned short>

//#define FIFO_CHANNEL sc_fifo<unsigned short>
#define FIFO_CHANNEL fifo_buffer<unsigned short>


// Notice that, by using defines, this has to be done before the kista header
//#define _USE_AUTOMATIC_CODE_ANNOTATION

//#define _PRINT_INPUTS
//#define _PRINT_OUTPUTS

#define SIZE_1 5
#define SAT_LIMIT 255
#define N_USER_TEST_VECTORS 6
#define N_RANDOM_TEST_VECTORS 10000

#include "wcet_calculation/constants.h"
#include "wcet_calculation/testvec_gen_f1.hpp"

#include "wcet_calculation/testvec_gen_f3.hpp"

// auziliar global data for easy measurement of latency of the direct HSDFG
bool end1_pending = true;
bool end2_pending = true;
bool end3_pending = true;
sc_event end1, end2, end3;
sc_event send_other_input_vector;

/*
void producer_f1::producer_proc()
{
	unsigned int i, test_vec;
	unsigned short int test_vec_i1[N_USER_TEST_VECTORS][SIZE_1] = { 
													{0,0,0,0,0},
													{0,0,0,0,0},
													{1,2,3,4,5},
													{1,2,3,4,5},
													{27,2,1,214,52},
													{27,32,21,123,52}
												  };
												  
	unsigned short int test_vec_i2[N_USER_TEST_VECTORS][SIZE_1] = { 
													{0,0,0,0,0},
													{255,255,255,255,255},
													{1,2,3,4,5},
													{5,4,3,2,1},	
													{27,2,1,214,52},																																																
													{67,22,145,214,52}
												  };
	unsigned short int test_vec_i3 = 0;

	for ( test_vec = 0; test_vec < N_USER_TEST_VECTORS; test_vec++ ) {	
		mode.write(test_vec_i3);
		for ( i = 0; i < SIZE_1; i++ ) {
			out1.write(test_vec_i1[test_vec][i]);
			out2.write(test_vec_i2[test_vec][i]);
		}
		wait(send_other_input_vector);
	}
}

*/


//#ifdef _RANDOM_SCENARIO_1

void producer_f1::producer_proc()
{

	unsigned int i, test_vec;
	unsigned short int test_vec_i1[SIZE_1];
												  
	unsigned short int test_vec_i2[SIZE_1];
	unsigned short int test_vec_i3 = 0;	

    // srand(time(NULL)); // for actual random seeds
    srand(1);
		
	for ( test_vec = 0; test_vec < N_RANDOM_TEST_VECTORS; test_vec++ ) {
		// generate random vectors
		test_vec_i3 = 1; // always multiplications
#ifdef _PRINT_INPUT_VECTORS		
cout << "vector: " << test_vec << endl;		
cout << test_vec_i3 << endl;		
#endif
		for ( i = 0; i < SIZE_1; i++ ) {
            // ensures saturation
			test_vec_i1[i] = 128+rand()%128;
			test_vec_i2[i] = 128+rand()%128;

#ifdef _PRINT_INPUT_VECTORS					
cout << test_vec_i1[i] << ", " << test_vec_i2[i] << endl;
#endif
		}
		
		// send stimuli data
		mode.write(test_vec_i3);
		
		for ( i = 0; i < SIZE_1; i++ ) {
			out1.write(test_vec_i1[i]);
			out2.write(test_vec_i2[i]);
		}
		wait(send_other_input_vector);
	}
}

//#endif

//#ifdef _RANDOM_SCENARIO_1

void producer_f3::producer_proc()
{

	unsigned int i, test_vec;
	unsigned short int test_vec_i1[SIZE_1];
												  
    // srand(time(NULL)); // for actual random seeds
    srand(1);
		
	for ( test_vec = 0; test_vec < N_RANDOM_TEST_VECTORS; test_vec++ ) {
		// generate random vectors
#ifdef _PRINT_INPUT_VECTORS		
cout << "vector: " << test_vec << endl;		
#endif
        // Specific inputs (partially ordered)
        test_vec_i1[0] = 1;
        test_vec_i1[1] = 2;
        test_vec_i1[2] = 3;
		for ( i = 3; i < SIZE_1; i++ ) {
			test_vec_i1[i] = 4+ rand()%251;
#ifdef _PRINT_INPUT_VECTORS					
cout << test_vec_i1[i] << endl;
#endif
		}
		
		// send stimuli data
		for ( i = 0; i < SIZE_1; i++ ) {
			out1.write(test_vec_i1[i]);
		}
		wait(send_other_input_vector);
	}
}

//#endif


 // consumer module
 // currently, does not read data (their correctnes is currently verified at the end of the tasks
 // thenselves, although this could be changed
 // currently it
 // Reports the latency of the HSDF trigger :
 // enforces a synchronization at the end (otherwise, Kista static schedule liks immediately next task, thus makeing an effective pipelinng)
 //
SC_MODULE(consumer) {
public:
	
	sc_time last_time_stamp;
	sc_time max_latency;
	sc_time cur_latency;

	void latency_measurement_poc() {
		last_time_stamp = sc_time_stamp();
		max_latency = SC_ZERO_TIME;
		while(true) {
			wait(end1|end2|end3);
			if(!end1_pending && !end2_pending && !end3_pending ) {
				cur_latency = sc_time_stamp()-last_time_stamp;
				last_time_stamp = sc_time_stamp();
				if( cur_latency > max_latency) {
					max_latency = cur_latency;
				}
				cout << "\t ------------------->  LATENCY : " << cur_latency << endl;
				end1_pending= true;
				end2_pending= true;
				end3_pending= true;
				send_other_input_vector.notify(SC_ZERO_TIME);
			}
		}
	}
	
//	void end_of_simulation() {
//		cout << "\t ------------------->  MAX LATENCY : " << max_latency << endl;
//	}

	SC_CTOR(consumer) {
			SC_THREAD(latency_measurement_poc);
	}
	
	~consumer() {
		cout << "\t -------------------> MAX LATENCY : " << max_latency << endl;
	}
	
};

/*
SC_MODULE(unzip2) {
	sc_fifo_in<unsigned short> in;
	sc_fifo_out<unsigned short> out1;
	sc_fifo_out<unsigned short> out2;
	
	void unzip2_proc() {
		while(true) {
			var = in.read();
			out1.write(var);
			out2.write(var);
		}
	}
	
	SC_CTOR(unzip2) {
		SC_THREAD(unzip2_proc);
	}
private:
	unsigned short var;
};
*/

#include "kista.hpp"

#include "constants.h"

#include "functions.h"


// if automatic code annotation is not used, then we use worst case instruction
// KisTA calculates the WCET automatically from them and from the frequency
// and CPI of the PE
#ifndef USE_AUTOMATIC_CODE_ANNOTATION
#define _USE_WCEI
#endif

// Data shared from the same input provided to the constraint-based program
#define N_PE 2

#define N_TASKS	3

// (Default) clock period of the Processing Elements in NS
#define PE_T_NS 20

unsigned int WCEI[N_TASKS] = {257,118,776};
//unsigned int WCEI[N_TASKS] = {1,2,3};

//
// Global schedule (provided through the starting time of the task in the hyperperiod)
// 		From the mapping and this global schedule local schedules can (and it is derived)
//
//unsigned int starting_time[N_TASKS] = {0,1,0};

//unsigned int PE_clock_period_ns[N_PE] = {5,5};
//unsigned int PE_clock_period_ns[N_PE] = {1,1};
//unsigned int PE_clock_period_ns[N_PE] = {2,2};

#define _SOL_4

#ifdef _SOL_1
unsigned int M[N_TASKS][N_PE] = { {1,0}, {1,0}, {0,1} };
unsigned int starting_time[N_TASKS] = {0,2570,0};
unsigned int PE_clock_period_ns[N_PE] = {10,10};
#endif

#ifdef _SOL_2
// task -> PE mapping
unsigned int M[N_TASKS][N_PE] = { {1,0}, {1,0}, {0,1} };
unsigned int starting_time[N_TASKS] = {0,5140,0};
unsigned int PE_clock_period_ns[N_PE] = {20,10};
#endif

#ifdef _SOL_3
unsigned int M[N_TASKS][N_PE] = { {1,0}, {0,1}, {0,1} };
unsigned int starting_time[N_TASKS] = {0,7760,0};
unsigned int PE_clock_period_ns[N_PE] = {10,10};
#endif

#ifdef _SOL_4
unsigned int M[N_TASKS][N_PE] = { {1,0}, {0,1}, {0,1} };
unsigned int starting_time[N_TASKS] = {0,7760,0};
unsigned int PE_clock_period_ns[N_PE] = {20,10};
#endif

#ifdef _SOL_5
unsigned int M[N_TASKS][N_PE] = { {1,0}, {1,0}, {1,0} };
unsigned int starting_time[N_TASKS] = {7760,10330,0};
unsigned int PE_clock_period_ns[N_PE] = {10,20};
#endif

#ifdef _SOL_6
unsigned int M[N_TASKS][N_PE] = { {1,0}, {1,0}, {1,0} };
unsigned int starting_time[N_TASKS] = {0,10330,2570};
unsigned int PE_clock_period_ns[N_PE] = {10,20};
#endif

#ifdef _SOL_7
unsigned int M[N_TASKS][N_PE] = { {1,0}, {1,0}, {1,0} };
unsigned int starting_time[N_TASKS] = {0,2570,3750};
unsigned int PE_clock_period_ns[N_PE] = {10,20};
#endif

#ifdef _SOL_8
unsigned int M[N_TASKS][N_PE] = { {1,0}, {1,0}, {0,1} };
unsigned int starting_time[N_TASKS] = {0,2570,0};
unsigned int PE_clock_period_ns[N_PE] = {10,20};
#endif

#ifdef _SOL_9
unsigned int M[N_TASKS][N_PE] = { {1,0}, {1,0}, {0,1} };
unsigned int starting_time[N_TASKS] = {0,2570,0};
unsigned int PE_clock_period_ns[N_PE] = {20,20};
#endif



// PE frequencies
//#define CLOCK_PERIOD_NS 20
#define CLOCK_PERIOD_NS 1

//float freqs[N_CONF_FREQ] = {50,100}; // Configuration frequencies MHz


const char* task_name[N_TASKS] = {"task1","task2","task3"};
VOIDFPTR 	task[N_TASKS]; // list of function pointers
// the same as: typedef void (*fwrapper[N_TASKS])();

//
#define CPI 1
// stated by default for KisTA PEs
//

#define N_TASK_SETS N_PE

// buffers for task/function communication

#define SIZE_1 5

//unsigned short in_buff1[SIZE_1];
unsigned short in_buff1[SIZE_1];
unsigned short in_buff2[SIZE_1];
unsigned short in_buff3[SIZE_1];
unsigned short mode;

unsigned short inter_buff_1[SIZE_1];
unsigned short inter_buff_2[SIZE_1];

unsigned short out_buff1;
unsigned short out_buff2[SIZE_1];

//unsigned short expected_out_buff1 = 6;
//unsigned short expected_out_buff2[SIZE_1] = {1,2,3,4,5};

// environment channels

FIFO_ENV_CHANNEL chenv1("chenv1",SIZE_1);
FIFO_ENV_CHANNEL chenv2("chenv2",SIZE_1);
FIFO_ENV_CHANNEL chenv3("chenv3",SIZE_1);
FIFO_ENV_CHANNEL chenv_mode("chenv_mode",1);

// channels for ensuring dependencies
//  (for the moment ad-hoc coding, but likely some part can be generalized,
FIFO_CHANNEL c_0_to_1("c_0_to_1",SIZE_1);

// counters for process computations
unsigned int m = 0; 
unsigned int n = 0;
unsigned int v = 0;


BEGIN_TASK(fwrap1)
for (unsigned int i =0; i<SIZE_1;i++) {
	 in_buff1[i] = chenv1.read(); 
}
for (unsigned int i =0; i<SIZE_1;i++) {
	 in_buff2[i] = chenv2.read(); 
}

mode=chenv_mode.read();

#ifdef _PRINT_INPUTS
cout << "Input vectors" << endl;
cout << "in1:";
for (unsigned int i =0; i<SIZE_1;i++) {
	 cout << in_buff1[i];
	 if(i<(SIZE_1-1)) cout << ", ";
	 else cout << endl;
}
cout << "in2:";
for (unsigned int i =0; i<SIZE_1;i++) {
	 cout << in_buff2[i];
	 if(i<(SIZE_1-1)) cout << ", ";
	 else cout << endl;
}
cout << "mode: ";
if(mode) cout << "true" << endl;
else cout << false << endl;
#endif
cout << "f1 (" << v << ")-th  start t=" << sc_time_stamp() << endl;	
    f1(inter_buff_1,in_buff1,in_buff2,mode);
    CONSUME_T
cout << "f1 (" << v << ")-th  end t=" << sc_time_stamp() << endl;	
	end1_pending = false;
	end1.notify(SC_ZERO_TIME);
	v++;
	
	cout << "f1 " << " t=" << sc_time_stamp() << endl;
	for(unsigned int i=0;i< SIZE_1;i++) {
		c_0_to_1.write(inter_buff_1[i]);
	}
END_TASK

BEGIN_TASK(fwrap2)
	for(unsigned int i=0;i< SIZE_1;i++) {
		c_0_to_1.read(inter_buff_2[i]);
	}
cout << "f2 (" << m << ")-th  start t=" << sc_time_stamp() << endl;	
    f2(&out_buff1,inter_buff_2);
    CONSUME_T
cout << "f2 (" << m << ")-th  end t=" << sc_time_stamp() << endl;
	end2_pending = false;
	end2.notify(SC_ZERO_TIME);       
    m++;

#ifdef _PRINT_OUTPUTS
cout << "out_buff1= " << out_buff1 << end;
//if(out_buff1==expected_out_buff1) cout << "f2 checked, OK" << endl;
//else cout << "ERROR f2 functionality " << endl;
#endif
END_TASK

BEGIN_TASK(fwrap3)
	for (unsigned int i =0; i<SIZE_1;i++) {
		in_buff3[i] = chenv3.read(); 
	}
cout << "f3 (" << n << ")-th  start t=" << sc_time_stamp() << endl;
    f3(out_buff2,in_buff3);
    CONSUME_T
cout << "f3 (" << n << ")-th  end t=" << sc_time_stamp() << endl;
	end3_pending = false;
	end3.notify(SC_ZERO_TIME);
    n++;
#ifdef _PRINT_OUTPUTS
cout << "out_buff2: ";
for (unsigned int i =0; i<SIZE_1;i++) {
	 cout << out_buff2[i];
	 if(i<(SIZE_1-1)) cout << ", ";
	 else cout << endl;
}

bool ok = true;
/*
  for (unsigned int i =0; i<SIZE_1;i++) {
	 if(out_buff2[i]!=expected_out_buff2[i]) {
		 ok=false;
		 cout << "Error in f3 functionality at " << i << " out_buff=" <<out_buff2[i] << " expected_out_buff2=" << expected_out_buff2[i] << endl;
	}
}    		
if(ok) cout << "f3 checked, OK" << endl;
*/
#endif
END_TASK


//
// translating the M[i] vector into a PE index
//   (assuming task i-th is mapped to only one PE j-th) 
//
unsigned int getPE(unsigned int M[N_PE]) {
	for(unsigned int j=0;j<N_PE;j++) {
		if(M[j]!=0) return j;
	}
	return 0;
}

     
int sc_main (int, char *[]) {
	
  unsigned int i;
  unsigned int j;
  std::map<unsigned int, task_info_t*> assigned_schedule_map;

  // environment
  producer_f1 prod_f1("prod_f1");
  prod_f1.out1(chenv1);
  prod_f1.out2(chenv2);
  prod_f1.mode(chenv_mode);
  
  producer_f3 prod_f3("prod_f3");
  prod_f3.out1(chenv3);
  
/*
  unzip2 uzip2("uzip2");
  uzip2.in(chenv1);
  uzip2.out1(chenv1_1);
  uzip2.out2(chenv1_2);
*/

  // auxiliar module instance for easier latency measurement
  consumer cons("cons");

  // For the moment, manual assignement of the wrappers
  task[0] = fwrap1;
  task[1] = fwrap2;
  task[2] = fwrap3; 

/*
cout << task_name[0] << endl;
cout << task_name[1] << endl;
cout << task_name[2] << endl;
*/

   // DESCRIPTION OF THE KISTA MODEL (POST-PROCESSING)
   
   // Declaration of task, task info, scheduler, static schedules and processing element structures  
   taskset_by_name_t *task_set[N_PE];
   task_info_t *task_info[N_TASKS];   // N_TASK info structures
   scheduler *sched[N_PE];
   static_schedule_by_task_info_pointers_t *static_schedule[N_PE];
   processing_element *PE[N_PE];

   // Create N_TASK task info structures
   for( i=0;i<N_TASKS;i++) {   
     // create task info structures
     task_info[i] = new task_info_t(task_name[i], task[i]);
#ifdef _USE_WCEI
     // set WCETS
     task_info[i]->set_WCEI(WCEI[i]); // WCETs are internally calculated by KisTA, at elaboration time
                                      // if only WCEI are provided as a function of the CPI and the mapping!!
#endif                                      
   }
	
   // create N_PE task sets (as many as PEs). All the tasks mapped to the same PE are mapped to the same scheduler
   // and assign tasks to task sets taking into accoutn the mapping matrix
	
   for(j=0; j< N_PE ; j++) {// as many task sets as PEs-schedulers
	
     task_set[j] = new taskset_by_name_t; // create the j-th task sets. 
		
     // Assign tasks to the task set
     for(i=0; i< N_TASKS ; i++) {
       if(M[i][j]!=0) { // the i-th task is mapped to the PE, and thus to its scheduler
			//(*task_set[j])[task_name[i].c_str()] = task_info[i];
			(*task_set[j])[task_name[i]] = task_info[i];
       }
     }
   }


   // Calculation of each local static schedule for each scheduler
   // (taking the output information from mini-zinc, that is, a global schedule in the shape of starting times,
   // and the mapping)
   for(j=0; j< N_PE ; j++) {
		// Instantiation of the schedule
		static_schedule[j] = new static_schedule_by_task_info_pointers_t;

		// assigned_schedule_map is an auxiliar variable, an ordered map to store&ordering the schedule
		// which, therefore, has to be reseted at each loop iteration
		assigned_schedule_map.clear();

		for(i=0; i< N_TASKS ; i++) {
			if(M[i][j]!=0) { // the i-th task is mapped to the PE, and thus to its scheduler
				assigned_schedule_map[starting_time[i]]= task_info[i];
				// inserts in the map the task info pointer using as key the
				// the starting time. This means that at the end of the process
				// the map will automatically present ordered by the key (the starting time)
				// the task info pointer
				// therefore, it should be already ordrederd according to the mapping and global scheduling provided from the mini-zinc output				
			}
		}
		// Not we only need to transfer the static scheduling map to a vector, because KisTA
		// uses the static_schedule_by_task_info_pointers_t type
		
		for (std::map<unsigned int,task_info_t*>::iterator it=assigned_schedule_map.begin(); it!=assigned_schedule_map.end(); ++it) {
			static_schedule[j]->push_back(assigned_schedule_map[it->first]);
		}
		// Here, the local schedule vector should be ready
   }

   // Create (Instatiation) N_PE scheduler instances and assignation of the task sets to them
   // with the kista::scheduler class

   for(j=0; j< N_PE ; j++) {
//	   sched[j] = new scheduler(task_set[j]);  // to pass a scheduler name is convenient to track results, although unique scheduler names are generated by default
//	   sched[j] = new scheduler(task_set[j], sc_module_name(sc_gen_unique_name("sched")));	   

	   // separate style
	   sched[j] = new scheduler;
	   sched[j]->allocate(task_set[j]);
	   
	   // by default, non-preemptive, one processor
	   // configuration of the scheduler for static scheduling
	   sched[j]->set_policy(STATIC_SCHEDULING);
		
		// set schedule
		sched[j]->set_static_schedule_by_task_ptrs(*static_schedule[j]);
		
		// tracings for each scheduler
		// Tracing of the tasks and scheduler ocupation signals
		// with the kista::trace method of the kista::scheduler class		
		sched[j]->trace_utilizations();

//		sched[j]->set_sufficient_global_simulation_time(10000);
//		sched[j]->set_sufficient_global_simulation_time(100);
		sched[j]->set_sufficient_global_simulation_time(2);

	} 

   // Create (Instatiation) of N_PE PE instances, and configure their working frequencies
   for(j=0; j< N_PE ; j++) {
	   PE[j] = new processing_element;
	   PE[j]->set_clock_period_ns(PE_clock_period_ns[j]);
	   //PE[j]->setCPI(1); // set cycles per instruction (1 is already set by default for PE)
	}
	
	// mapping of schedulers to processing elements (one-to-one map)
	for(j=0; j< N_PE ; j++) {
		sched[j]->map_to(PE[j]);
	}
	
//    set_global_simulation_time_limit(sc_time(10,SC_US));
//    set_global_simulation_time_limit(sc_time(1,SC_SEC));
     
   sc_start();
//   sc_start(6,SC_SEC);
   
   // REPORTS (POST-PROCESSING)

   for(j=0; j< N_PE ; j++) {
	   cout << "**********************************************************************************" << endl;
	   // print ocupation of scheduler...
	   cout << sched[j]->name() << " scheduler utilization : " << (float)(sched[j]->get_scheduler_utilization()*100.0) << "%" << endl;
	   
	   // ... and of assigned tasks ...
	   taskset_by_name_t::iterator ts_it;
	   for(ts_it = task_set[j]->begin(); ts_it != task_set[j]->end(); ts_it++) {
			  cout << ts_it->first << " task utilization : " << (float)(sched[j]->get_task_utilization(ts_it->first)*100.0) << "%" << endl;
	   }
	   cout << "Total tasks utilization : " << (float)(sched[j]->get_tasks_utilization()*100.0) << "%" << endl;
	   
	   cout << sched[j]->name() << " number of schedulings: " << (unsigned int)sched[j]->get_number_of_schedulings() << endl;
	   cout << sched[j]->name() << " platform utilization : " << (float)(sched[j]->get_platform_utilization()*100.0) << "%" << endl;
	   cout << "**********************************************************************************" << endl;
	   sched[j]->assess_starvation();
	   sched[j]->report_response_times();
   }
   
   cout << "last simulation time:" << sc_time_stamp() << endl;
   cout << "**********************************************************************************" << endl;
   return 0;
   
}
