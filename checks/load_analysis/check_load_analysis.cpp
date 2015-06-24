
//#include "load_analysis/load_analysis.hpp"

#include "kista.hpp"

//void main(int argc, char *argv[]) {
int sc_main(int argc, char *argv[]) {

//	load_unit_set_t loaders_set;
//	load_unit_set_t::iterator load_it;

	load_unit_set_with_unique_priorities_t loaders_set;
	load_unit_set_with_unique_priorities_t::iterator load_it;
		
	load_unit loader1, loader2, loader3, loader4;
	load_unit curr_load_unit;
	
    // This case, assumes priority assigned by period (Rate monotonic) = deadline

	// loader 1
	loader1.name          = "loader1";
	loader1.wc_load       = sc_time(1.0,SC_SEC);
	loader1.period        = sc_time(3.0,SC_SEC);
	loader1.rel_deadline  = sc_time(3.0,SC_SEC);
	loader1.priority      = 1;

	// loader 2
	loader2.name          = "loader2";
	loader2.wc_load       = sc_time(1.0,SC_SEC);
	loader2.period        = sc_time(5.0,SC_SEC);
	loader2.rel_deadline  = sc_time(5.0,SC_SEC);
	loader2.priority      = 2;

	// loader 3
	loader3.name          = "loader3";
	loader3.wc_load       = sc_time(1.0,SC_SEC);
	loader3.period        = sc_time(6.0,SC_SEC);
	loader3.rel_deadline  = sc_time(6.0,SC_SEC);
	loader3.priority      = 3;

	// loader 4
	loader4.name          = "loader4";
	loader4.wc_load       = sc_time(2.0,SC_SEC);
	loader4.period        = sc_time(10.0,SC_SEC);
	loader4.rel_deadline  = sc_time(10.0,SC_SEC);
	loader4.priority      = 4;
	
	loaders_set.insert(make_pair(1,loader1));
	loaders_set.insert(make_pair(2,loader2));
	loaders_set.insert(make_pair(3,loader3));
	loaders_set.insert(make_pair(4,loader4));
	
	// Test a schedulable set
	// 
	// assessment exiting as soon as an assessment is done 
	if( assess_precise_bound(loaders_set, true) ) {
		cout << "Task set passes the precise bound check" << endl;
	} else {
		cout << "Task set passes does not pass the precise bound check" << endl;
	}
	
	cout << endl;
	
	// assessment exiting only after the calculation of all the response times 
	if( assess_precise_bound(loaders_set) ) { //  like assess_precise_bound(loaders_set, false)
		cout << "Task set passes the precise bound check" << endl;
		cout << "Immediate exit." << endl;
	} else {
		cout << "Task set passes does not pass the precise bound check" << endl;
		cout << "Complete responses calculation." << endl;		
	}
		
	// Shows the response times of the schedulable set
	load_it = loaders_set.begin();
	
	while(load_it!=loaders_set.end()) {
		curr_load_unit = load_it->second;
		cout << curr_load_unit.name;
		cout << ": worst-case response time = ";
		cout << curr_load_unit.wc_response;
		cout << ", deadline = ";
		cout << curr_load_unit.rel_deadline;
		cout << endl;
		load_it++;
	}
	
	cout << endl;	
	
	// Now a non schedulable task set will be passed to the assessment function
	loaders_set.find(4)->second.wc_load       = sc_time(3.0,SC_SEC);
	
	// Test a non-schedulable set
	// 
	// assessment exiting as soon as an assessment is done 
	if( assess_precise_bound(loaders_set, true) ) {
		cout << "Task set passes the precise bound check" << endl;
	} else {
		cout << "Task set passes does not pass the precise bound check." << endl;
		cout << "Immediate exit." << endl;
	}

	cout << endl;
		
	// assessment exiting only after the calculation of all the response times 
	if( assess_precise_bound(loaders_set) ) { //  like assess_precise_bound(loaders_set, false)
		cout << "Task set passes the precise bound check" << endl;
	} else {
		cout << "Task set passes does not pass the precise bound check" << endl;
		cout << "Complete responses calculation." << endl;
	}
	
	// Shows the response times of the schedulable set
	load_it = loaders_set.begin();
	
	while(load_it!=loaders_set.end()) {
		curr_load_unit = load_it->second;
		cout << curr_load_unit.name;
		cout << ": worst-case response time = ";
		cout << curr_load_unit.wc_response;
		cout << ", deadline = ";
		cout << curr_load_unit.rel_deadline;
		cout << endl;
		load_it++;
	}

	return 0;
	
}
