// 
// wcet_f2.cpp
// 
//   SystemC configuration using op-cost SCoPE method and random-tests for estimation of WCET
//   of function f2
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, April
//


//#include "hetsc.h"
#include "systemc.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "basic_scope.h"
#include "wcet_est.h"

#include "constants.h"
#include "sc_f2.h"


sc_time t1,t2,t3;

#ifdef _RANDOM_VECTORS
unsigned long long results[N_RANDOM_TEST_VECTORS];
unsigned int hist[HISTOGRAM_SIZE];
#endif

SC_MODULE(producer) {
public:

  sc_fifo_out<unsigned short>	out1;

  void producer_proc();

  SC_CTOR(producer) {
     SC_THREAD(producer_proc);
  }
};

SC_MODULE(consumer) {
public:

  sc_fifo_in<unsigned short>	in;

  void consumer_proc();

#ifdef _RANDOM_VECTORS  
  void end_of_simulation();
#endif

  SC_CTOR(consumer) {
     SC_THREAD(consumer_proc);
     outfile = fopen("times_f2.dat","w");
  }
  
  ~consumer() {
  		fclose(outfile);
  }
  
private:  
  FILE * outfile;
};


#ifdef _RANDOM_VECTORS
 
void producer::producer_proc()
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
		for ( i = 0; i < SIZE_1; i++ ) {
			test_vec_i1[i] = rand()%256;
#ifdef _PRINT_INPUT_VECTORS					
cout << test_vec_i1[i] << endl;
#endif
		}
		
		// send stimuli data
		for ( i = 0; i < SIZE_1; i++ ) {
			out1.write(test_vec_i1[i]);
		}
	}
}

#else

 // user vectors

void producer::producer_proc()
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

	for ( test_vec = 0; test_vec < N_TEST_USER_VECTORS; test_vec++ ) {	
		for ( i = 0; i < SIZE_1; i++ ) {
			out1.write(test_vec_i1[test_vec][i]);
		}
	}
}

#endif


void consumer::consumer_proc()
{
	unsigned int i;
	unsigned short recv_buff;
	
	do {
		recv_buff = in.read();
/*		
		cout << "consumer " << name() << ": ";
		cout << recv_buff;
		cout << " reception complete at time " << sc_time_stamp() << endl;; 
*/		
		t2=sc_time_stamp();
		
	} while(true);
}

#ifdef _RANDOM_VECTORS
void consumer::end_of_simulation() {
	unsigned int i;
	unsigned long long max_instr;
	unsigned long long min_instr;
	unsigned long long diff_instr;

	max_instr = results[0];
	min_instr = results[0];
	
	// find maximum value:
	for(i=1;i<N_RANDOM_TEST_VECTORS; i++) {
		if (results[i]>max_instr) {
			max_instr = results[i];
		}
		if (results[i]<min_instr) {
			min_instr = results[i];
		}
	}
	
	diff_instr = max_instr-min_instr;
	
	// produce histogram
	for(i=0;i<N_RANDOM_TEST_VECTORS;i++) {
		if(results[i]==max_instr) hist[HISTOGRAM_SIZE-1]++; // The maximum would fall in hist[N_RANDOM_TEST_VECTORS]. Instead, we accumulate it in hist[N_RANDOM_TEST_VECTORS-1]
		else hist[((results[i]-min_instr)*HISTOGRAM_SIZE)/diff_instr]++;
//		hist[((results[i]-min_instr)*HISTOGRAM_SIZE)/diff_instr]++; // Notice that one more is used for the case of the max
	}

// printout histogram
//	for(i=0;i<HISTOGRAM_SIZE;i++) {
//		cout << hist[i] << endl;
//	}
	
//	cout << "max_instr=" << max_instr << endl;
//	cout << "min_instr=" << min_instr << endl;	
	
	fprintf(outfile,"# instr freq.\n");
	for(i=0;i<HISTOGRAM_SIZE;i++) {
		if(i<(HISTOGRAM_SIZE-1)) {
			fprintf(outfile,"[%llu-%llu) %u\n", min_instr+i*diff_instr/HISTOGRAM_SIZE,min_instr+(i+1)*diff_instr/HISTOGRAM_SIZE,hist[i]);
		} else {
			fprintf(outfile,"[%llu-%llu] %u\n", min_instr+i*diff_instr/HISTOGRAM_SIZE,min_instr+(i+1)*diff_instr/HISTOGRAM_SIZE,hist[i]);	
		}
	}
	
}

#endif



int sc_main(int sc_argc, char *sc_argv[])
{
	t1=sc_time_stamp();
  
    sc_fifo<unsigned short>  ch1("ch1");
    sc_fifo<unsigned short>  ch2("ch2");    

    // test bench
    producer prod("prod");
	prod.out1(ch1);

    consumer cons("cons1");
	cons.in(ch2);
    
    // system
    sc_f2 scf2("scf2");
	scf2.in(ch1);
	scf2.out(ch2);

	// Initialization of annotated variables
	uc_segment_time = 0;
	uc_segment_instr =0;

    t1=sc_time_stamp();
    sc_start();
	sc_stop();
	
    t3=t2-t1;

//    cout<<"Simulation: Start: " << t1 << " ends: "<<t2 <<" latency: " <<t3 << endl;
    //printf("\n\nSystemC simulation of Edge Detector complete. Press the Enter key to continue.\n");
    //c = getc(stdin);

    return(0);
}

