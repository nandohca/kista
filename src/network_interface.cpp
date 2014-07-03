/*****************************************************************************

  network_interface.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 February

 *****************************************************************************/

#ifndef _NETWORK_INTERFACE_CPP
#define _NETWORK_INTERFACE_CPP

#include <systemc.h>
#include "defaults.hpp"
#include "global_elements.hpp"
#include "utils.hpp"
#include "network_interface.hpp"
#include "processing_element.hpp"

namespace kista {

network_interface::network_interface(sc_module_name name_par, processing_element *owner_PE_par)
			: sc_module(name_par)
{	
	owner_PE = owner_PE_par;
	finite_buffer_flag = false;
	tx_buffer_size = 0;
	cur_data_in_tx_buffer = 0;
	room_in_tx_buffer = 0; // actually, infinite
	
	SC_THREAD(network_interface_proc);
		
}


network_interface::network_interface(sc_module_name name_par,
                                          processing_element *owner_PE_par,
                                          unsigned int tx_buffer_size_par)
              : sc_module(name_par)
{
	std::string rpt_msg;
	if(cur_data_in_tx_buffer==0) {
		rpt_msg = "Transmission buffer of network interface in PE ";
		rpt_msg += owner_PE->name();
		rpt_msg = " is 0.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	}
#ifdef _WARN_SMALL_NETIF_TX_BUFFER	
	else {
		rpt_msg = "Transmission buffer of network interface in PE ";
		rpt_msg += owner_PE->name();
		rpt_msg = " is 0.";
		SC_REPORT_WARNING("KisTA",rpt_msg.c_str());	
	}
#endif
	owner_PE = owner_PE_par;
	tx_buffer_size = tx_buffer_size_par;
	cur_data_in_tx_buffer = 0;
	room_in_tx_buffer = tx_buffer_size_par;

	SC_THREAD(network_interface_proc);
}

bool& network_interface::finite_tx_buffer() {
	return finite_buffer_flag;
}

void network_interface::set_tx_buffer_size(unsigned int tx_buffer_size_par) {
	std::string rpt_msg;
	if ( sc_get_status()&(SC_START_OF_SIMULATION|SC_RUNNING|SC_STOPPED|SC_PAUSED|SC_END_OF_SIMULATION) ) {
		rpt_msg = "set_CPI, called for PE ";
		rpt_msg += name();
		rpt_msg += ", has to be called before simulation start.";
		SC_REPORT_ERROR("KisTA","");
	}
	finite_buffer_flag = true;
	tx_buffer_size = tx_buffer_size_par;
	cur_data_in_tx_buffer = 0;
	room_in_tx_buffer = tx_buffer_size_par;

}

unsigned int &network_interface::get_tx_buffer_size() {
	return tx_buffer_size;
}

void network_interface::request_sending(
							send_message_request_t send_msg_req // this arg. because currently, the delay is retrieved by the system-level channel (which calls this)
) {
	
#ifdef _REPORT_NETIF_SEND_REQUESTS
	std::string rpt_msg;
	rpt_msg = "Request sending in PE ";
	rpt_msg += owner_PE->name();
	rpt_msg += " network interface at ";
	rpt_msg += sc_time_stamp().to_string();
	SC_REPORT_INFO("KisTA",rpt_msg.c_str());
#endif
	
	// send_queue[sc_time_stamp()] = send_msg_req; // not possible in multimap, so we use the previously seen style
	
	send_request_ev.notify(SC_ZERO_TIME);
	
	// if a finite tx buffer is modeled, then it is possible that the task 
	// requesting the message has to wait if the tx buffer has not sufficient
	// room to cope with the request
	if (finite_buffer_flag) {
		while(send_msg_req.message_size>room_in_tx_buffer) {
			wait(send_request_completed);
		}
	}  // else { // infinite buffer
		
	//}
	
	// stores the sending request (with its request time) in the sending queue
	send_queue.insert(std::pair<sc_time,send_message_request_t>(send_msg_req.request_time,send_msg_req));
	
	// updates the state of the tx buffer
	room_in_tx_buffer -= send_msg_req.message_size;
	cur_data_in_tx_buffer += send_msg_req.message_size;
	
}


//
// implements a non-preemptive (FIFO on request arrivals) communication scheduling.
// Uses static scheduling when the sending requests are just concurrent
// (the very same request time)

void network_interface::network_interface_proc() {
	
	std::string msg;
	
	send_message_request_t cur_msg_req;
	
	sc_time comm_delay;
	
	while(true) {
		
		while(send_queue.size()==0) {
			wait(send_request_ev);
			break;
		}
		
		// get the next transfer/coomunication to be performed
		cur_msg_req = dispatch();
	
		// and notifies it to the request_sending methods to see if they have a chance to be released
		send_request_completed.notify(SC_ZERO_TIME);
		
		// deallocate message request from the send_request queue
		
		// retrieve communication delay for the message request to be attended
		// which depends on the type of analysis on communication penalties
		
		if(worst_case_communication_enabled) {
			comm_delay = cur_msg_req.comm_res->get_MaxP2Pdelay(*cur_msg_req.phy_link , cur_msg_req.message_size);
		} else {
			comm_delay = cur_msg_req.comm_res->get_CurrentP2Pdelay(*cur_msg_req.phy_link , cur_msg_req.message_size);
		}

#ifdef _REPORT_NETIF_DISPATCH		
		msg = "Network interface of PE ";
		msg += owner_PE->name();
		msg = " start to send message for system-level comm&synch ";
		msg += cur_msg_req.logic_link->get_link_name();
		msg += " at ";
		msg += sc_time_stamp().to_string();
#ifdef _REPORT_COMMUNICATION_DELAYS
		if(worst_case_communication_enabled) {
			msg += " with maximum associated delay: ";
			msg += comm_delay.to_string();
			msg += "\n";
		} else {
			msg += " with current associated delay: ";
			msg += comm_delay.to_string();
			msg += "\n";						
		}
#endif		
		SC_REPORT_INFO("KisTA",msg.c_str());
#endif
		
		wait(comm_delay);
		
		// notification to warn the logic channel now that the transfer 
		// has been completed through the physical channel
		cur_msg_req.completed_transactions_p->post();
		
			// notification to let pending send requests because of tx buffer full 
		
		// once the transfer is complete, the buffer releases the corresponfing size...
		room_in_tx_buffer += cur_msg_req.message_size;
		cur_data_in_tx_buffer -= cur_msg_req.message_size;

	}
}


send_message_request_t network_interface::dispatch() {

	std::string msg;
	send_message_request_t 				next_msg;
	send_req_queue_t::iterator			next_msg_it;
	synch_send_requests_t				synch_send_requests;
	
	// get (iterator to) first message in the sending queue
	
	if(send_queue.size()>1) { // some communication scheduling might be required	
	
	    // Here we rely on that a multimap is a weakly ordered container, so 
	    // at least the next element in the queue will be one of the maybe several
	    // earliests sending requests
		next_msg_it = send_queue.begin();
		
		// If there are SYNCHRONOUS COMMUNICATION REQUESTS
		// AND a (STATIC COMM.) SCHEDULING has been specified,
		// then, the (static) communication scheduling is used to schedule the 
		// communication.
		if (
			(send_queue.count(next_msg_it->first) > 1)
			&&
			there_is_static_comm_scheduling()
			)
		{
			synch_send_requests = send_queue.equal_range(next_msg_it->first);
			next_msg_it = static_comm_dispatch(synch_send_requests);						
		}
		// } else {
		// If there are synchronous communication requests, but no (static) communication
		// scheduling has been provided, a FIFO policy, that is, as the requests appear in the
		// structure (so actually non-deterministic polcy) is followed
		// }

	} else if(send_queue.size()==1) {
		// No communication schedule is required, just take the only one in the sending queue
		next_msg_it = send_queue.begin();
	} else {
		msg = "Unexpected situation. Dispatch of a new communication in the PE ";
		msg += owner_PE->name();
		msg += " network interface was triggered while its sending queue is empty.";
		SC_REPORT_ERROR("KisTA",msg.c_str());
	}
	
	next_msg = next_msg_it->second;  // prepares the return value, namely the sending request structure
	send_queue.erase(next_msg_it);	 // erase the requests sending structure from the sending queue
	return next_msg;
}	

send_req_queue_t::iterator network_interface::static_comm_dispatch(synch_send_requests_t synch_send_requests_par) {

	std::string msg;
	unsigned int i;
	send_req_queue_t::iterator send_req_it;
	
	for(i=0;i<static_comm_schedule.size();i++) {
		
		for(send_req_it=synch_send_requests_par.first; 	send_req_it!=synch_send_requests_par.second; send_req_it++) {
				if( static_comm_schedule[i] == send_req_it->second.logic_link ) {
					// The first match found in the static_comm_schedule from the
					// the synchronous request list (synch_send_requests_par) is dispatched
					return send_req_it;	
				}
		}
	}
	
	msg = "Pending synchronous communication send requests:\n";
	for(send_req_it=synch_send_requests_par.first; 	send_req_it!=synch_send_requests_par.second; send_req_it++) {
		msg += "req: ";
		msg += send_req_it->second.logic_link->get_link_name();
		msg += ":";
		msg += std::to_string(send_req_it->second.message_size);
		msg += " bits: at ";
		msg += send_req_it->second.request_time.to_string();
		msg += "\n";
	}
	msg += " for the PE ";
	msg += owner_PE->name(); 
	msg += " network interface, but none of then were found in the static communication schedule!."; 
	SC_REPORT_ERROR("KisTA",msg.c_str());

}


void network_interface::set_static_comm_scheduling(llink_set_t static_comm_schedule_par) {
	check_call_before_sim_start("set_static_comm_scheduling (by llink_set_t)");
	
	static_comm_schedule = static_comm_schedule_par;
	
}

void network_interface::set_static_comm_scheduling(static_comm_schedule_by_link_names_t static_comm_schedule_by_name_par) {
	check_call_before_sim_start("set_static_comm_scheduling (by llink_set_by_name_t)");
	
	static_comm_schedule_by_name = static_comm_schedule_by_name_par;
	
}

void network_interface::set_static_comm_scheduling(name_pair_vector_t static_comm_schedule_by_task_names_par) {
	check_call_before_sim_start("set_static_comm_scheduling (by llink_set_t)");
	
	static_comm_schedule_by_task_names = static_comm_schedule_by_task_names_par;

}


bool network_interface::there_is_static_comm_scheduling() {
	check_call_after_sim_start("there_is_static_comm_scheduling");
	// notice that it is enforced to have the scheduling list in the form of static list of logic links
	// (if the static schedule is provided by names, then, it is produced at the end of the elaboration)
	if(static_comm_schedule.size()==0) {
		return false;
	} else {
		return true;
	}
}

	
unsigned int &network_interface::get_data_size_in_tx_buffer() {
	return cur_data_in_tx_buffer;
}

unsigned int &network_interface::get_room_in_tx_buffer() {
	return room_in_tx_buffer;
}

/*
void network_interface::end_of_elaboration() {
	check_and_prepare_comm_static_schedule();	
}
*/

void network_interface::check_and_prepare_comm_static_schedule() {

	std::string rpt_msg;

	llink_set_t			static_comm_schedule2;
	llink_set_t			static_comm_schedule3;
	unsigned int		size1, size2, size3; //sizes of the provided static communicaiton schedules

	size1 = static_comm_schedule.size();
	size2 = static_comm_schedule_by_name.size();
	size3 = static_comm_schedule_by_task_names.size();
		
	if( (size1==0)  && (size2==0)  && (size3 == 0 ) ) {
#ifdef _REPORT_NO_STATIC_COMM_SCHEDULING_FOR_NETIF
		rpt_msg = " No static communication schedule was provided for the network interface of PE ";
		rpt_msg += this->owner_PE->name();
		SC_REPORT_INFO("KisTA",rpt_msg.c_str());
#endif		
	} else {

		// get schedules as static vectors of logic links from any of the capture forms supported
		if(size2!=0) {
			static_comm_schedule2 = get_static_comm_schedule_from_names(static_comm_schedule_by_name);
		}
		
		if(size3!=0) {
			static_comm_schedule3 = get_static_comm_schedule_from_task_names(static_comm_schedule_by_task_names);
		}
		
		// compare all provided schedules
		if((size1!=0) && (size2!=0) && (size3!=0) ) {
			compare_schedules(static_comm_schedule,static_comm_schedule2, "by logic links and by their names");
			compare_schedules(static_comm_schedule,static_comm_schedule3, "by logic links and by their src and dest task names");
			// leave static_comm_schedule as it is
		} else if ( (size1!=0) && (size2!=0) ) {
			compare_schedules(static_comm_schedule,static_comm_schedule2, "by logic links and by their names");
			// leave static_comm_schedule as it is
		} else if ( (size1!=0) && (size3!=0) ) {
			compare_schedules(static_comm_schedule,static_comm_schedule3, "by logic links and by their src and dest task names");
			// leave static_comm_schedule as it is
		} else if ( (size2!=0) && (size3!=0) ) {
			compare_schedules(static_comm_schedule2,static_comm_schedule3, "by logic link names and by their src and dest task names");
			static_comm_schedule = static_comm_schedule2;
		} else if ( size2!=0 ) {			
			static_comm_schedule = static_comm_schedule2;
		} else if ( size3!=0 ) {
			static_comm_schedule = static_comm_schedule3;
		} else if ( size1!=0 ) {
		  // leave static_comm_schedule as it is
		}
		
	}

}

	
// compare two non-void schedules and return true if they match
void network_interface::compare_schedules(llink_set_t static_com_sched1, llink_set_t static_com_sched2, const char *compared_types) {
	std::string rpt_msg;
	unsigned int i;
	if(static_com_sched1.size()!=static_com_sched2.size()) {
		rpt_msg = "Sizes of the static communication schedules specified for the network interface of PE ";
		rpt_msg += owner_PE->name();
		rpt_msg += ", specified ";
		rpt_msg += compared_types;
		rpt_msg += ". They do not match in size.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	} else {
		for(i=0; i<static_com_sched1.size(); i++ ) {
			if(static_com_sched1[i] != static_com_sched2[i]) {
				rpt_msg = "Mismatch found in the static communication schedules specified for the network interface of PE ";
				rpt_msg += owner_PE->name();
				rpt_msg += ", specified ";
				rpt_msg += compared_types;
				rpt_msg += ", in the ";
				rpt_msg += i;
				rpt_msg += "-th element.";				
				SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
			}
		}
	}

}

// retrieves a communication schedule by pointers to the actual logic links from a communication schedule by names of the logic link	
llink_set_t network_interface::get_static_comm_schedule_from_names(static_comm_schedule_by_link_names_t static_comm_schedule_by_names_par) {
	
	unsigned int i;
	std::string	rpt_msg;
	
	system_level_connection_set_by_name_t::iterator sys_conn_by_name_it;
	
	llink_set_t out_comm_schedule; // created as 0
	
	for(i=0;i<static_comm_schedule_by_names_par.size();i++) {
		
		// first checks that the system level connection has been registered in the model
		sys_conn_by_name_it = sys_conn_by_name.find(static_comm_schedule_by_names_par[i]);
		
		if(sys_conn_by_name_it == sys_conn_by_name.end()) {
			//  system-level connection name not found in the sys_conn global structure
			rpt_msg = "System level connection ";
			rpt_msg += static_comm_schedule_by_names_par[i];
			rpt_msg += " specified in the static communication scheduling of the PE ";
			rpt_msg += owner_PE->name();
			rpt_msg += " network interface doest not exist in the model.";
			SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		}
		
		out_comm_schedule.push_back(sys_conn_by_name_it->second);
	}
	
	return out_comm_schedule;
}

// retrieves a communication schedule by names of the logic linkfrom a communication schedule by src/dest task names of the logic link		
llink_set_t network_interface::get_static_comm_schedule_from_task_names(name_pair_vector_t static_comm_schedule_by_task_names_par) {
	
	unsigned int i,j;
	std::string rpt_msg;

	bool found_scheduled_comm;

	llink_set_t out_comm_schedule; // created as 0
	
	name_pair_t	name_pair;
	
	for(i=0;i<static_comm_schedule_by_task_names_par.size();i++) {
		found_scheduled_comm = false;
		
		for(j=0;j<sys_conn.size();j++) {
			if( (sys_conn[j]->src == get_address_by_elem_name<logic_address>(static_comm_schedule_by_task_names_par[i].first) )
				&&
				(sys_conn[j]->dest == get_address_by_elem_name<logic_address>(static_comm_schedule_by_task_names_par[i].second) )
				)
			{
				out_comm_schedule.push_back(sys_conn[j]);
				found_scheduled_comm = true;
			}
		}
		
		if(!found_scheduled_comm) {
					// (src task,dest task) connection not found in the sys_conn global structure
			rpt_msg = "System level connection (src task=";
			rpt_msg += static_comm_schedule_by_task_names_par[i].first;
			rpt_msg += ",dest task=";
			rpt_msg += static_comm_schedule_by_task_names_par[i].second;
			rpt_msg += ") specified in the static communication scheduling of the PE ";
			rpt_msg += owner_PE->name();
			rpt_msg += " network interface doest not exist in the model.";
			SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		}
	}
	
	return out_comm_schedule;	
	
}

} // namespace kista

#endif
