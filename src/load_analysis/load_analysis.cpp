/*****************************************************************************

  load_analysis.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: University of Cantabria
  Deparment:   TEISA Dpt
  Date: 2015 June

 *****************************************************************************/

#ifndef ANALYSIS_CPP
#define ANALYSIS_CPP

//#define _DEBUG_LOAD_ANALYSIS

#include "load_analysis.hpp"

namespace kista {
	
// orders the load units just by priority (does not presume rate monotonic or similar)
bool operator<(const load_unit &lhs, const load_unit &rhs) {
	if (lhs.priority < rhs.priority) return true;
	else return false;
}

#define MAX_RELATIVE_DIFF_FOR_PRECISE_BOUND  0.01
#define MAX_NUMBER_FIXED_POINT_ITERATIONS    1000

// 
// Considers iterative solution for Joseph&Pandya equations
//
	
bool assess_precise_bound(load_unit_set_with_unique_priorities_t &loaders, bool immediate_exit) {	
	load_unit_set_with_unique_priorities_t::iterator load_it;
	load_unit_set_with_unique_priorities_t::iterator hp_load_it; // index pointing to higher (or same) priority loads
		
	sc_time R; 		// response
	sc_time R_next; // next response calculation (for iterative solving)
	sc_time I; // interference
	double rel_diff;
	
	// to store the bound for the number of repetitions (triggers)
	// of a task of higher priority
	double b_doub;
	unsigned int b;
	
	unsigned int iterations;
	
	bool assessment_result;
	
	if(loaders.size()<1) {
		SC_REPORT_ERROR("KisTA","Loaders list empty when calling precise load analysis");
	}
	
	assessment_result=true; // assessment result: passed by default

	load_it = loaders.begin();    // points to the current assessed loader
	
#ifdef _DEBUG_LOAD_ANALYSIS	
	cout << "START Load Analysis (LA): " << endl;		
#endif

	// since loader is an ordered set and the oders for the load units is
	// stablished by priority, this loops from the higher priority (lesser priority number)
	// to the lower priority (bigger priority number)
	while(load_it != loaders.end()) {

#ifdef _DEBUG_LOAD_ANALYSIS			
cout << "\tLA: " << load_it->second.name << "loader" <<endl;
#endif	
		// fixed point solution for Response time
		// first guess  
		// the own wc load of the loader...
		R_next = load_it->second.wc_load;
//cout << "\t\t\t R0= " << R_next << endl;		
		//... plus all the wc load of the more prioritary loaders
		// first rest of index to Hither Priority loaders
		hp_load_it = loaders.begin(); // to point to higher priority loaders
		while (hp_load_it->first < load_it->first)
		{
//cout << " add " << hp_load_it->second.wc_load << endl;			
			R_next += hp_load_it->second.wc_load;
			hp_load_it++;
		} 

#ifdef _DEBUG_LOAD_ANALYSIS
cout << "\t\tStart with R= " << R_next << endl;
#endif
	
		//reset hp_load_it index
		hp_load_it = loaders.begin(); // to point to higher priority loaders	

		iterations = 0;
		// do one guess more at least, and so until the error is lesser than
		// a quantity (ideally no error)
		do {
			// the response time is updated either with the initial guess
			// or with the last calculated update
			R = R_next;
			
			// calculates the interference, as a function of the current guess of the response time
			// and of the periods and loads of the higher priority loads
			I=SC_ZERO_TIME;
			
			// first calculate the interference of all the loaders with higher priority
			while(hp_load_it->first < load_it->first) {
				// calculates Inteference of this load unit of higher priority
				// on the lower priority task
				b_doub = R/hp_load_it->second.period;
				b=ceil(b_doub); // the idea is to get the highest integer bounging
								// the real value (it may require a check)
				I += b*hp_load_it->second.wc_load;
				hp_load_it++;
			}
			
//cout << "ITERATE" << endl;
//cout << "  I= " << I << endl;

			// calculates the next guess of the response, once the interference
			// is calculated
			R_next = R + I;
			
//cout << "  R (updated)= " << R_next << endl;			

			// calculate the relative distance in the error
			if(R_next>R) {
				rel_diff = (R_next-R)/R_next;
			} else {
				rel_diff = (R-R_next)/R;
			}
			// check if a limit in the interations for the fixed point solution is reached
			iterations++;
			if(iterations>MAX_NUMBER_FIXED_POINT_ITERATIONS) {
				printf("KisTA Error: Reached the maximum number of iterations %d for fixed point equation solving in precise load analysis.\n",iterations-1);
				exit(-1);
			}
			
		} while(rel_diff>MAX_RELATIVE_DIFF_FOR_PRECISE_BOUND);
		
#ifdef _DEBUG_LOAD_ANALYSIS
cout << "\t\tFinish with R= " << R_next << " after " << iterations << " iterations." << endl;
#endif	
		// reaching this point means that the fixed point equuation is
		// solved with a satisfactory accuracy.
		
		// taking into account the precision of the estimate, we ensure to take the highest value
		// For it, a final update of R is done only if R_next is a bigger value
		if(R_next > R) R=R_next;
		
		// checks if the response exceeds the relative deadline
		if(R>load_it->second.rel_deadline) assessment_result= false;
				
		// The responde time is stored
		load_it->second.wc_response = R;
		
		// in case that immediate exit is configured and the analysis leads to non-schedulability
		// the function returns immediately and stops the calculation of responses
		if(immediate_exit && (!assessment_result)) return false;

		load_it++;
	}
	
	// no more responses to calculate, return the result of the assessment
	return assessment_result;
	
}


bool assess_precise_bound(load_unit_set_t &loaders, bool immediate_exit) {	
	
	SC_REPORT_ERROR("KisTA","Precisse bound for a load set supporting loaders with same priorities not implemented yet.");
// Code similar to the previous one adding something like	
			
			// This is a generic algorithm, which allows for a case where
			// there are loaders with the same priority.
			// Then, the worst case considers the possibility of interference
			// from same priority loaders
//			while(hp_load_it->first < load_it->first) {}
			
}

} // namespace kista
#endif

