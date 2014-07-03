// 
// testvec_gen_f2.cpp
// 
//   SystemC test generation for f1 functionality
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, May
//

#ifndef _TESTVEC_GEN_F2_CPP
#define _TESTVEC_GEN_F2_CPP

//#include "hetsc.h"
#include "systemc.h"

#include "constants.h"
#include "wcet_est.h"

#include "testvec_gen_f2.hpp"


#ifdef _FULL_RANDOM_SCENARIO
 
void producer_f2::producer_proc()
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

#endif

#ifdef _USER_VECTORS

 // user vectors

void producer_f2::producer_proc()
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

#endif
