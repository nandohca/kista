/*****************************************************************************

  tdma_bus_accuracy_levels.cpp
  
		Experiment to assess the accuracy - computation effort
		for the different accuracy level supported for the max p2p delay
		assessment of the TDMA bus.
                
  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 April
 
          Currently that is to be completed.
 	
 *****************************************************************************/
      
#include "kista.hpp"

#define _ENABLE_TIME_EFFORT_ASSESSMENT
#define ITERATIONS_TIME_ASSESSMENT 1000000

#ifdef _ENABLE_TIME_EFFORT_ASSESSMENT
#include <ctime>
#endif

void print_syntax() {
     cout << " The syntax of the accuracy levels assesment is the following: "<< endl;
     cout << endl;
     cout << "tdmab_acl_assess	<n_channels>  <slot_time_ns> <payload_bits> <cycle_time_us> [<message_size_bits>] [<slots_assigned_range>]"<< endl;
     cout << endl;
     cout << "  where all the values should be integer, >0, and slots_assigned_range <= n_channels." << endl;
     cout << " The default values for the optional arguments are message_size_bits=512, and slots_assigned_range=n_channels." << endl;
     cout << endl;
}

int sc_main (int argc, char *argv[]) {
   
   unsigned int n_channels;
   unsigned int slot_time_ns;
   unsigned int payload_bits;
   unsigned int cycle_time_us;

   unsigned int message_size_bits;
   unsigned int slots_assigned_range;
   
   // check arguments
   if (argc<5) {
     cout << "Less arguments than expected" << endl;
     print_syntax();
     return -1;
   }

   if (argc>7) {
     cout << "too many arguments" << endl;
     print_syntax();
     return -1;
   }

   // assign to intermediate variables, just only for this code readability
   n_channels = atoi(argv[1]);
   slot_time_ns = atoi(argv[2]);
   payload_bits = atoi(argv[3]);
   cycle_time_us = atoi(argv[4]);
   	
   if (argc>5) {
	   message_size_bits = atoi(argv[5]);
   } else {
	   message_size_bits = 512; // bits
   }
   
   if (argc>6) {
	   slots_assigned_range = atoi(argv[6]);
   } else {
	   slots_assigned_range = n_channels;
   }
   
   if(slots_assigned_range>n_channels) {
     cout << "Trying to sweep delay times for a number of allocated slots beyond the number of slots (" << n_channels << ")" << endl;
     print_syntax();
     return -1;
   }   
   
   // Create the KisTA TDMA bus object
   tdma_bus		tdmab("tdmab");
      
   // configure the bus
   tdmab.set_n_channels(n_channels);
   tdmab.set_slot_time(sc_time(slot_time_ns,SC_NS));
   tdmab.set_channel_payload_bits(payload_bits);
   tdmab.set_cycle_time(sc_time(cycle_time_us,SC_US));

   // Fill the table
   cout << "% Assessment of accuracy levels on TDMA bus. Configuration" << endl;
   cout << "#slots | slot time (ns) | payload/slot (bits) | TDMA cycle (us) | Message size (bits) | # slots allocated range" << endl;
   cout << n_channels << "\t" << slot_time_ns << "\t" << payload_bits << "\t" <<  cycle_time_us << "\t" << message_size_bits << "\t" << slots_assigned_range << endl; 
   cout << endl;   
   
   cout << "********************************************************************" << endl;   
   cout << "Maximum P2P Delay Bound for each Accuracy level and Slots Allocated " << endl;
   cout << "********************************************************************" << endl;
   cout << "Alloc.	Data Tx Delay	P2P BOUNDS						 " << endl;
   cout << "#slots	Level 0		Level 0	Level 1	Level 2 " << endl;
   cout << "----------------------------------------" << endl;
   
   for(unsigned int i =1; i<=slots_assigned_range; i++) {
	   cout << i;
	   cout << "\t";
	   cout << tdmab.calculate_data_tx_delay_L0(message_size_bits,i);
	   cout << "\t";
	   cout << tdmab.calculate_MaxP2Pdelay_L0(message_size_bits,i);
	   cout << "\t";
	   cout << tdmab.calculate_MaxP2Pdelay_L1(message_size_bits,i);
	   cout << "\t";
	   cout << tdmab.calculate_MaxP2Pdelay_L2(message_size_bits,i);
	   cout << endl;
   }
   cout << "********************************************************************" << endl;

#ifdef _ENABLE_TIME_EFFORT_ASSESSMENT
   clock_t begin_time, end_time;
   float offset;
	
   cout << endl;
   cout << "********************************************************************" << endl;   
   cout << "Estimated Native host time required for each assessment " << endl;
   cout << "Iterations for time assessent: " << ITERATIONS_TIME_ASSESSMENT << endl;
   begin_time=clock();
   for(unsigned int i=0; i<ITERATIONS_TIME_ASSESSMENT; i++) {}
   end_time=clock();
   offset = float(end_time-begin_time);
   cout << "offset measured: " << offset << endl;
   cout << "********************************************************************" << endl;
   cout << "    	Host time (s)" << endl;
   cout << "Alloc.	MAX P2P delay              Current TX Delay   Data Tx Delay  Current Offset  " << endl;
   cout << "#slots Level 2  Level 1  Level 0   L0,L0              Level 0        Level 0  Level 1" << endl;
   cout << "-------------------------------------------------------------------------------------" << endl;
   
   for(unsigned int i =1; i<=slots_assigned_range; i++) {
	   cout << i;
	   cout << "\t";
	   begin_time=clock();
	   for(unsigned int j=0; j<ITERATIONS_TIME_ASSESSMENT; j++) {
		 tdmab.calculate_MaxP2Pdelay_L2(message_size_bits,i);
	   }
	   end_time=clock();
	   cout << float(end_time-begin_time)/CLOCKS_PER_SEC/ITERATIONS_TIME_ASSESSMENT-offset;
	   cout << "\t";
	   
	   begin_time=clock();
	   for(unsigned int j=0; j<ITERATIONS_TIME_ASSESSMENT; j++) {
		 tdmab.calculate_MaxP2Pdelay_L1(message_size_bits,i);
	   }
	   end_time=clock();
	   cout << float(end_time-begin_time)/CLOCKS_PER_SEC/ITERATIONS_TIME_ASSESSMENT-offset;
	   cout << "\t";
	   
	   begin_time=clock();
	   for(unsigned int j=0; j<ITERATIONS_TIME_ASSESSMENT; j++) {
		 tdmab.calculate_MaxP2Pdelay_L0(message_size_bits,i);
	   }
	   end_time=clock();
	   cout << float(end_time-begin_time)/CLOCKS_PER_SEC/ITERATIONS_TIME_ASSESSMENT-offset;
	   cout << "\t";
	   
	   sc_time tmp;
	   begin_time=clock();
	   for(unsigned int j=0; j<ITERATIONS_TIME_ASSESSMENT; j++) {
		 tmp = tdmab.current_offset_delay_L0(SC_ZERO_TIME,0) + tdmab.calculate_data_tx_delay_L0(message_size_bits,i);;
	   }
	   end_time=clock();
	   cout << float(end_time-begin_time)/CLOCKS_PER_SEC/ITERATIONS_TIME_ASSESSMENT-offset;	    
	   cout << "\t";   
	   
	   begin_time=clock();
	   for(unsigned int j=0; j<ITERATIONS_TIME_ASSESSMENT; j++) {
		 tdmab.current_offset_delay_L0(SC_ZERO_TIME,0);
	   }
	   end_time=clock();
	   cout << float(end_time-begin_time)/CLOCKS_PER_SEC/ITERATIONS_TIME_ASSESSMENT-offset;
	   cout << "\t";
	    
	   begin_time=clock();
	   for(unsigned int j=0; j<ITERATIONS_TIME_ASSESSMENT; j++) {
		 tdmab.current_offset_delay_L1(slots_assigned_range);
	   }
	   end_time=clock();
	   cout << float(end_time-begin_time)/CLOCKS_PER_SEC/ITERATIONS_TIME_ASSESSMENT-offset;
	   cout << "\t";

	   begin_time=clock();
	   for(unsigned int j=0; j<ITERATIONS_TIME_ASSESSMENT; j++) {
		 tdmab.calculate_data_tx_delay_L0(message_size_bits,i);
	   }
	   end_time=clock();
	   cout << float(end_time-begin_time)/CLOCKS_PER_SEC/ITERATIONS_TIME_ASSESSMENT-offset;
	   cout << endl;
   }
   cout << "********************************************************************" << endl;
#endif

   #define N_POINTS 5
   
   sc_time time_step = tdmab.get_cycle_time()/N_POINTS;
   sc_time curr_time;

   cout << "***********************************************************************" << endl;   
   cout << "Current P2P Offset Delay for Accuracy level 0, first slot vs curr_time (phase) at tx time " << endl;
   cout << "***********************************************************************" << endl;
   cout << "Alloc.				Offset Delay			 " << endl;
   cout << "first slot			curr_time phase from	0s to " << tdmab.get_cycle_time() << " (cycle time) in steps of " << time_step << endl;
   cout << "---------------------------------------------------------------------------------------- " << endl;
    
   for(unsigned int i = 0; i<= slots_assigned_range; i++) {
	   curr_time = SC_ZERO_TIME;
	   cout << i;
	   cout << "\t";
	   for(unsigned int j=0; j<N_POINTS; j++) {
			cout << tdmab.current_offset_delay_L0(curr_time, i);
			curr_time += time_step;
			cout << "\t";	
	   }
	   cout << endl;
   }
   cout << "********************************************************************" << endl;

   cout << "***************************************************************************************" << endl;   
   cout << "Current P2P Delay for Accuracy level 0, vs allocated slots, first slot, and curr_time (phase) at tx time" << endl;
   cout << "**************************************************************************************" << endl;
   cout << "Alloc.\t First			 " << endl;
   cout << "slots \t slot	\t curr_time phase from	0s to " << tdmab.get_cycle_time() << " (cycle time) in steps of " << time_step << endl;
   cout << "---------------------------------------------------------------------------------------- " << endl;
    
   for(unsigned int i = 1; i<= slots_assigned_range; i++) { // iterates in the number of assigned slots slots
	   for(unsigned int j = 0; j< (slots_assigned_range-i); j++) { // iterates in the possible first slot (never greater than slots_assigned_range - required slots (i)
	                                                     // that way we avoid uninteresting solutions
	   	    curr_time = SC_ZERO_TIME;
			for(unsigned int k=0; k<N_POINTS; k++) {
				if(k==0) {
					cout << i; // slots allocated
					cout << "\t";					
					cout << j;
					cout << "\t";		   
				}
				cout << tdmab.current_offset_delay_L0(curr_time, j) + tdmab.calculate_data_tx_delay_L0(message_size_bits,i);
				cout << "\t";			
				curr_time += time_step;
			}
			cout << "\n";
	   }
	   cout << endl;
   }
   cout << "********************************************************************" << endl;

   return 0;
      
}
