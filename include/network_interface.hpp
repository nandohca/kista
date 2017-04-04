/*****************************************************************************

  network_interface.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 February

 *****************************************************************************/

#ifndef _NETWORK_INTERFACE_HPP
#define _NETWORK_INTERFACE_HPP

#include <systemc.h>

#include "defaults.hpp"

#include "types.hpp"

#include "comm_res/comm_res.hpp"

namespace kista {

class network_interface: public sc_module {
public:
	
	SC_HAS_PROCESS(network_interface);
	
	// creates a network interface ... 
	network_interface( sc_module_name name_par, processing_element *owner_PE_par);  // ...with infinite tx buffer capability
	network_interface( sc_module_name name_par, processing_element *owner_PE_par, unsigned int tx_buffer_size_par);
							// ...with finite tx buffer capability
	
	bool &finite_tx_buffer(); // by default, the network interface is constructed with infinite buffer capability, so
	                           // this call should return false
	
	void set_tx_buffer_size(unsigned int tx_buffer_size_par);
							// set tx buffer capability (in bits) (to be called before simulation start)
							// It immediately sets the tx buffer to finite
	
	unsigned int &get_tx_buffer_size(); // in bits
	
	unsigned int &get_data_size_in_tx_buffer(); // bits
	unsigned int &get_room_in_tx_buffer();      // bits
	
	// set static communiction schedulign (used to fixe dispatching for synchronous sending requests)
	//   It can be specified by means of a vector to logic links (system,level channels)...
	void set_static_comm_scheduling(llink_set_t static_comm_schedule_par);
	void set_static_comm_scheduling(static_comm_schedule_by_link_names_t static_comm_schedule_by_name_par);
	
	//   ... or by means of a vector reflecting the logic destination addresses (tasks)
	//       from which the llink is extracted.
	//       This methods works under the assumption that there is only one communication
	//       synchronization edge for 
	void set_static_comm_scheduling(name_pair_vector_t comm_schedule_by_task_names_par); 
	
	// to be called at simulation time
	bool there_is_static_comm_scheduling();
	
	// request data sending to the network interface
	// 
	void request_sending(send_message_request_t send_msg_req);

	// provides the next send. msg. request to be satisfied:
	// send requests are served in a FIFO basis (former requests are served first)
	//
	// For simultaneous requests, the same aplies (this introduces non-determinism on the simulation)  
	// or, if a static communication scheduling has been provided, it is used to order the 
	// serving of the synchronous requests
	//
	send_message_request_t dispatch();
	
	// Selects the next task from a set of synchronous requests relying on 
	send_req_queue_t::iterator static_comm_dispatch(synch_send_requests_t synch_send_requests_par);
						
	// To be calles from the scheduler class at end of elaboration time
	void check_and_prepare_comm_static_schedule();						
						
	// process modelling network interface
	void network_interface_proc();
		
private:
	bool finite_buffer_flag;
	unsigned int tx_buffer_size;
	unsigned int cur_data_in_tx_buffer;  // current amount of bits in the transmission buffer	
	unsigned int room_in_tx_buffer; 		// current amount of bits available in the transmission buffer	
	processing_element *owner_PE;
	
	send_req_queue_t send_queue; // where network interface class stores the 
	                             // the queue of pending transfers
	
	// internal variables for communication scheduling
	llink_set_t								static_comm_schedule; // by logic links
	static_comm_schedule_by_link_names_t 	static_comm_schedule_by_name;
	name_pair_vector_t 						static_comm_schedule_by_task_names;
	
	// events related to network interface
	sc_event send_request_ev;
	sc_event send_request_completed;
	sc_event complete_sending;
	
	//void end_of_elaboration();
	
	// auxiliar methods for checking and generating static communication schedules provided to configure the netif
	
	// compare two non-void schedules and return true if they match
	void compare_schedules(llink_set_t static_com_sched1, llink_set_t static_com_sched2, const char *compared_types);
	
	// retrieves a communication schedule by pointers to the actual logic links from a communication schedule by names of the logic link
	llink_set_t get_static_comm_schedule_from_names(static_comm_schedule_by_link_names_t static_comm_schedule_by_names_par);

	// retrieves a communication schedule by names of the logic linkf rom a communication schedule by src/dest task names of the logic link
	llink_set_t get_static_comm_schedule_from_task_names(name_pair_vector_t static_comm_schedule_by_task_names_par);
	
};

} // namespace kista

#endif
