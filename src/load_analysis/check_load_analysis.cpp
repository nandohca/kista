

void main(int argc, char *argv[]) {

	load_unit_set_t loaders_set;
	
	load_unit loader1, loader2, loader3, loader4;
	
    // This case, assumes priority assigned by period (Rate monotonic) = deadline

	// loader 1
	loader1.name          = "loader1";
	loader1.wc_load       = SC_TIME(1.0,SC_SEC);
	loader1.period        = SC_TIME(3.0,SC_SEC);
	loader1.rel_deadline  = SC_TIME(3.0,SC_SEC);
	loader1.priority      = 1;

	// loader 2
	loader1.name          = "loader2";
	loader1.wc_load       = SC_TIME(1.0,SC_SEC);
	loader1.period        = SC_TIME(5.0,SC_SEC);
	loader1.rel_deadline  = SC_TIME(5.0,SC_SEC);
	loader1.priority      = 2;

	// loader 3
	loader1.name          = "loader3";
	loader1.wc_load       = SC_TIME(1.0,SC_SEC);
	loader1.period        = SC_TIME(6.0,SC_SEC);
	loader1.rel_deadline  = SC_TIME(6.0,SC_SEC);
	loader1.priority      = 3;

	// loader 4
	loader1.name          = "loader4";
	loader1.wc_load       = SC_TIME(2.0,SC_SEC);
	loader1.period        = SC_TIME(10.0,SC_SEC);
	loader1.rel_deadline  = SC_TIME(10.0,SC_SEC);
	loader1.priority      = 4;
	
	loaders_set.insert(1,loader1);
	loaders_set.insert(2,loader2);
	loaders_set.insert(3,loader3);
	loaders_set.insert(4,loader4);
	
	// Observation: maybe a priority_queue could be used also for the implementation of the assess_precise_bound function
		
	if( assess_precise_bound(loaders_set, true) ) {
		cout << "Task set pass the precise bound check" << endl;
	}
	
	if( assess_precise_bound(loaders_set, true) ) {
		cout << "Task set pass the precise bound check" << endl;
	}
	
}
