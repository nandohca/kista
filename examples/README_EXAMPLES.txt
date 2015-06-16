
The following examples are provided with the KisTA library:

Examples for BASIC REAL-TIME MODELLING FEATURES 

/basic_real_time/ FOLDER
-------------------------

brt_ex1/ severaltasks.cpp:
	   Two task example used as a simple example which serves to check 
	   how to model two infinite tasks in kista, and their mapping to
	   a scheduler mapped to a single processor.
	   The example shows the effect of non-preemptive vs RR scheduling.

brt_ex2/ twotasks_with_waits.cpp:
	   Two task example used as a simple example which serves to show 
	   the modelling of periodic task sets in kista, mapped to a scheduler
	   in turn mapped to a single processor, and how to obtain different reporting results

brt_ex3/ two_ptasks_and_static_prio.cpp:
           Two task example used to show some dynamic (static priority) scheduling modelling
	   features of kista, with mapping to an scheduler handling
	   a single processing element.
	   The example also shows some features of sketch report (only_image, set_scale).

brt_ex4/ density_monotonic_ex.cpp:
       This example shows the usage of density monotonic example
	   as an example of other less usual scheduling policies that can
	   be integrated and analyzed in in KisTA, contrasted to two well
	   known dynamic (static priority) scheduling policies, RMS and DMS.

brt_ex5/ three_ptasks_and_dynamic_prio.cpp:
       Three task example used as a simple example which serves to ilustrate 
       dynamic scheduling in KisTA, specifically EDF, with mapping to an scheduler mapped
       to a single processing element.

brt_ex6/ severaltasks_static_sched.cpp:
	These example serves to see static scheduling configuration and
        modelling features, including the modelling of non-idealities
        (static scheduling time), and user support such as automatic calculation
        of simulation times to see at least a number of cycles.

brt_ex7/ two_ptasks_and_static_prio.cpp:
		This example serves to show the possibility to enable common and 
		policy dependent static schedulability analysis. Moreover, it also serves
		to show how a user can attach her/his own schedulability analysis.
		
		
tikz_tracing_and_sched_times_ex1/ two_ptasks_and_static_prio.cpp
  
		This is the brt_ex3 example, extended to show the following features:

		* add scheduling times to the model and extension of scheduling time models which scheduling time calculators
		* TikZ tracing and some of the options enabled by KisTA to control such tracing
				
More specifically and interestingly, the user can play with the examples with
some non-ideal parameterization, such as scheduling times, and see how they
can actually affect. 

/synch_comm/ FOLDER
-----------------------------

synch_ex1/ two_synch_taks.cpp:
		Example of modelling of memoryless event synchronization at a System-Level and
		at a SW modelling level with scheduling (because there is only one processing resource)

synch_ex2/ two_synch_taks_flag.cpp:
		Example of modelling of registered event (flag) synchronization at a System-Level and
		at a SW modelling level with scheduling (because there is only one processing resource)

synch_ex3/ two_synch_taks_semaphore.cpp:
		Example of system-level and SW model with scheduling (because there is only one processing resource),
		where two semaphores are used to synchronize the access to a common shared buffer

synch_ex4/ two_synch_taks_semaphore_fifo_buffer.cpp:
		Example of system-level and SW model  with scheduling (because there is only one processing resource),
		where a blocking fifo buffer is used to synchronize and communicate two asynchronous tasks.
		The example is cheched with different dynamic scheduling policies.
		
synch_ex5/ static_sched_		with_synch.cpp:
		In this exaples, statich scheduling (like in brt_ex6) is combined with 
		static scheduling policies.
		The two possible static schedulings are tried with two different
		synchronization mechanisms.
		It can be checked how coherent schedules and synchronizations, while
		redundant, let the simulation work, while incoherent ones lead to a deadlock.
		The example also let check the effect of "pure SystemC" environment events in the KisTA specification
		
synch_ex6/ static_sched_with_synch.cpp:
        This example is used to check the combination of
        fifo buffer synch&comm channels with static scheduling.


/comm_res/ FOLDER
-----------------------------
 Folder with examples using either generic communication resources or different types of communication resources

/comm_res/ comres_ex1:
		Example showing the utilization of a generic communication resource or a simple configuratio of a TDMA bus

/comm_res/ tdma_bus_acc_levels
		Example which serves to show and measure the accuracy vs simulation speed trade-offs
		which can be configured in the KisTA TDMA bus model.

/fdl2013_ex FOLDER
		Example which shows:
		- combination of synchronization (by means of fifo buffers) with static scheduling policies
		  In this case, the synchronization is not always redundant, because the static schedulings
		  are not timed. So synchronizations guarantee orders (dependencies) in this example
		- generic construction of the model for DSE
		- hook to included automatic annotations of SCoPE

/xml_if/ FOLDER
-----------------------------
 Folder with examples showing capabilities of the KisTA XML front-end
    ex1: A first simple model, showing the main system parts and basic configuration (simulation time, verbosity, sketch report)
    ex2: shows additions of task attributes
    ex3: shows parameterization, no communication yet. Illustrates (as the remaining examples) connection to Multicube compatible exploration tool
    ex4: as ex3 plus communication
	ex5: as ex4 plus a generic communication resource 
	ex6: as ex5 plus a TDMA bus
	ex7: adds parameterized mappings
	ex8: illustrates usage of complex data types in the edges transferred by reference
	ex9: illustrates the attach of functionality to the KisTA XML description
	ex9: uses WCEIs, insted of WCETs, network interfaces, and io attributes for edges, to delimit the I/O communications of the application
	ex_message_t: illustrates a model using complex data types in the edges transferred by value
	vad: illustrates a complex model having features such as feedback loops, system and environment functionality,
	     initialization functions, report of complex metrics


/VAD:
--------------
	Void adctivity detector example showing many features supported by the XML interface, including hook of
	system and environment functionality, functional validation, trace and TiKZ trace report, etc.

/jpeg_encoder
--------------
	Adaptation of the jpeg_encoder example available with the ForSyDe-SystemC distribution

	
