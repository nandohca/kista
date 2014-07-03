// 
// testvec_gen_f1.cpp
// 
//   SystemC test generation for f1 functionality
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, May
//

#ifndef _TESTVEC_GEN_F1_CPP
#define _TESTVEC_GEN_F1_CPP

//#include "hetsc.h"
#include "systemc.h"

#include "constants.h"
#include "wcet_est.h"

#include "testvec_gen_f1.hpp"

#ifdef _FULL_RANDOM_SCENARIO
 
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
		test_vec_i3 = rand()%2;

#ifdef _PRINT_INPUT_VECTORS		
cout << "vector: " << test_vec << endl;		
cout << test_vec_i3 << endl;		
#endif
		for ( i = 0; i < SIZE_1; i++ ) {
			test_vec_i1[i] = rand()%256;
			test_vec_i2[i] = rand()%256;

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
	}
}
#endif

#ifdef _RANDOM_SCENARIO_1

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
	}
}

#endif

#ifdef _USER_VECTORS
 // user vectors

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
		
		for ( test_vec_i3 = 0; test_vec_i3 < 2; test_vec_i3++ ) {	
			mode.write(test_vec_i3);
			for ( i = 0; i < SIZE_1; i++ ) {
				out1.write(test_vec_i1[test_vec][i]);
				out2.write(test_vec_i2[test_vec][i]);
			}
		}
	}
}

#endif
  //  endif of user vectors
  
#endif
//  endif of file
