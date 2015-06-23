
//#include "load_analysis/load_analysis.hpp"

#include "kista.hpp"

//void main(int argc, char *argv[]) {
int sc_main(int argc, char *argv[]) {

	load_unit_set_t loaders_set;
	
	load_unit loader1, loader2, loader3, loader4;
	
	load_unit_set_t::iterator load_it;
	load_unit curr_load_unit;
	
    // This case, assumes priority assigned by period (Rate monotonic) = deadline

	// loader 1
	loader1.name          = "loader1";
	loader1.wc_load       = sc_time(1.0,SC_SEC);
	loader1.period        = sc_time(3.0,SC_SEC);
	loader1.rel_deadline  = sc_time(3.0,SC_SEC);
	loader1.priority      = 1;

	// loader 2
	loader1.name          = "loader2";
	loader1.wc_load       = sc_time(1.0,SC_SEC);
	loader1.period        = sc_time(5.0,SC_SEC);
	loader1.rel_deadline  = sc_time(5.0,SC_SEC);
	loader1.priority      = 2;

	// loader 3
	loader1.name          = "loader3";
	loader1.wc_load       = sc_time(1.0,SC_SEC);
	loader1.period        = sc_time(6.0,SC_SEC);
	loader1.rel_deadline  = sc_time(6.0,SC_SEC);
	loader1.priority      = 3;

	// loader 4
	loader1.name          = "loader4";
	loader1.wc_load       = sc_time(2.0,SC_SEC);
	loader1.period        = sc_time(10.0,SC_SEC);
	loader1.rel_deadline  = sc_time(10.0,SC_SEC);
	loader1.priority      = 4;
	
	loaders_set.insert(make_pair(1,loader1));
	loaders_set.insert(make_pair(2,loader2));
	loaders_set.insert(make_pair(3,loader3));
	loaders_set.insert(make_pair(4,loader4));
	
	// Observation: maybe a priority_queue could be used also for the implementation of the assess_precise_bound function
		
	if( assess_precise_bound(loaders_set, true) ) {
		cout << "Task set pass the precise bound check" << endl;
	} else {
		cout << "Task set pass does not pass the precise bound check" << endl;
	}
	
	if( assess_precise_bound(loaders_set) ) { //  like assess_precise_bound(loaders_set, false)
		cout << "Task set pass the precise bound check" << endl;
	} else {
		cout << "Task set pass does not pass the precise bound check" << endl;
	}
	
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
	
}
