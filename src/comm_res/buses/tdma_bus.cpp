/*****************************************************************************

  tdma_bus.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 August
  
  Last update: 2014 May
  Notes: Added Current delays considering the current status of the TDMA bus
		 Automatic and manual assignation of slots (based on the assignation
		 of contiguous ranges
 *****************************************************************************/

#ifndef TDMA_BUS_CPP
#define TDMA_BUS_CPP

#include <map>
//#include <string>

#include <systemc.h>

#include "defaults.hpp"

#include "global_elements.hpp"

#include "tdma_bus.hpp"

#include "processing_element.hpp"
#include "draw.hpp"

#include "utils.hpp"

//#include <utility>

namespace kista {

// empty constructor
void tdma_bus::init_params() {
  	n_allocated_channels = 0;
		// Values related to unsetting
	n_channels = 0;
	eBW=0; //bps
	channel_BW=0; //bps
	channel_eBW=0; //bps
	channel_capacity=0; // bits
	slot_time=SC_ZERO_TIME; // directly related to BW
	
	ch_payload_rate = 0.0; // rate
	ch_payload_bits = 0; //  bits // set the default, then end_of_elaboration checks any of them has a valid vlaue

	guard_time = SC_ZERO_TIME; // setting optional, default 0, per channel
	cycle_time = SC_ZERO_TIME;

	tx_delay = SC_ZERO_TIME;
	
	// Configuration of the type of communication resource
	
/*	// Default configuration
	set_time_invariant(true);
	set_time_invariant_dimension(true);
	set_connections_time_invariant(true);
	set_connections_message_size_invariant(true); 
*/
	set_connections_message_size_invariant(false); 
			// NOTE: KISTA CONSIDER VARIANCE WITH MESSAGE SIZE AS TIME VARIANT CONNECTION
			
	accuracy_level = 0; // highest by default
	srand(1);			// default for the random generation
	
	manual_slot_allocation = false; // by default the automatic slot allocation is assumed
	
	initial_offset_time = SC_ZERO_TIME;
}

tdma_bus::tdma_bus(sc_module_name name) : phy_comm_res_t(name) {
	init_params();
}

// constructor:
//  Using this constructor ensures to fill enought data

tdma_bus::tdma_bus(unsigned int		n_channels_par,
			  sc_time 			slot_time_par,  // time assigned to the time slot
			  double 			ch_payload_bits_par,			  
			  sc_time 			cycle_time_par,
			  sc_time 			guard_time_par, 
			  sc_module_name 	name) : phy_comm_res_t(name)
{

	init_params();
	// assign parameters				
	//n_channels = n_channels_par;
	set_n_channels(n_channels_par); // to also reset the free slots table
	slot_time = slot_time_par;
	ch_payload_bits = ch_payload_bits_par;	
	cycle_time = cycle_time_par;
	guard_time = guard_time_par;

	// calculate the remaining parameters from these ones
	channel_capacity = eBW*slot_time_par.to_seconds();
	
	std::string comm_res_name;
	comm_res_name = this->name();	
	// updates the global comm_res elements table accessed by name
	phy_commres_by_name[comm_res_name]=this;

}

void tdma_bus::set_n_channels(unsigned int n_channels_par) {
	check_call_before_sim_start("set_n_channels");		
	n_channels = n_channels_par;
	// reset the free slot allocation table
	free_slot_table.push_back(std::make_pair(0,n_channels_par-1));
}

unsigned int tdma_bus::get_n_channels() {
	return n_channels;
}

// total channel bandwidth (excludes guard times, basically addition of channels)
void tdma_bus::set_slot_time(sc_time slot_time_par) {
	check_call_before_sim_start("set_slot_time");		
	slot_time=slot_time_par;
}

sc_time &tdma_bus::get_slot_time() {
	return slot_time;
}

// total channel bandwidth (excludes guard times, basically addition of channels)
void tdma_bus::set_channel_capacity(unsigned int channel_capacity_par) {
	check_call_before_sim_start("set_channel_capacity");	
	channel_capacity=channel_capacity_par;
}

unsigned int tdma_bus::get_channel_capacity() {
	return channel_capacity;
}

void tdma_bus::set_channel_payload_rate(double ch_payload_rate_par) {		// rate payload bits / channel capacity
	check_call_before_sim_start("set_channel_payload_rate");
	ch_payload_rate = ch_payload_rate_par;
}

void tdma_bus::set_channel_payload_bits(unsigned int ch_payload_bits_par) {	// payload bits}
	check_call_before_sim_start("set_channel_payload_bits");
	ch_payload_bits = ch_payload_bits_par;
}

double &tdma_bus::get_channel_payload_rate() {
	return ch_payload_rate;
}

unsigned int &tdma_bus::get_channel_payload_bits() {
	return ch_payload_bits;
}


// guard times between adjacent channels
void tdma_bus::set_guard_time(sc_time guard_time_par) {
	check_call_before_sim_start("set_guard_time");	
	guard_time = guard_time_par;
}


sc_time &tdma_bus::get_guard_time() {
	return guard_time;
}

// Period of the TDMA bus
void tdma_bus::set_cycle_time(sc_time cycle_time_par) {
	check_call_before_sim_start("set_cycle_time");	
	cycle_time = cycle_time_par;
}

sc_time &tdma_bus::get_cycle_time() {
	return cycle_time;
}

unsigned int &tdma_bus::get_channel_BW() {
	return channel_eBW;
}

unsigned int &tdma_bus::get_channel_eBW() {
	return channel_eBW;
}

unsigned int &tdma_bus::get_eBW() {
	return eBW;
}

void tdma_bus::set_tx_delay(sc_time tx_d_par) {
	check_call_before_sim_start("set_tx_delay");	
	tx_delay = tx_d_par;
}

sc_time &tdma_bus::get_tx_delay() {
	return tx_delay;
}


void  tdma_bus::set_initial_offset_time(sc_time par) {
	check_call_before_sim_start("set_initial_offset_time");
	initial_offset_time = par;
}

sc_time &tdma_bus::get_initial_offset_time() {
	return initial_offset_time;
}

void tdma_bus::set_seed(unsigned int seed) {
	check_call_before_sim_start("set_seed");	
	srand(seed); // overrides srand(1) in the contruction
}

// Looks for req_slots slots and provides the first slot position
// if sucessful, or raises an error otherwise
unsigned int tdma_bus::assign_free_slots(unsigned int req_slots) {
	std::string msg;
	free_slot_range_t free_slot_range;
	
	if(free_slot_table.size()<1) {
		msg = "In TDMA bus ";
		msg += name();
		msg += ": Empty free slot table when trying to assign free slots.";
		SC_REPORT_ERROR("KisTA",msg.c_str());
	}
	
	for(auto it = free_slot_table.begin(); it != free_slot_table.end(); it++ ) {
		free_slot_range = *it;
		if( req_slots < (free_slot_range.second - free_slot_range.first + 1)) {
			// slots taken from this range
			// update the range ...
			it->first = it->first + req_slots;
			// ... and first slot from the range selected returned
			return free_slot_range.first; // notice that free_slot_range variable is required to not to return the updated index
		} else if(req_slots == (free_slot_range.second - free_slot_range.first + 1)) {
			// slots taken from this range:
			// eliminate the range entry from the vector...
			free_slot_table.erase(it);
			// ... and first slot from the range selected returned
			return free_slot_range.first;
		} else {
			it++; // no room in this entry, the next entry in the
			      // free slot table is tried
		}
	}
	
	// reaching this point means that no space was found in the free slot
	// table, either because there was no sufficient space or because
	// the table is fragmented in a manner it prevents the allocation
	msg = "In TDMA bus ";
	msg += name();
	msg += ": Trying to allocate ";
	msg += std::to_string(req_slots);
	msg += " slots. Current status of the allocation table:";
	for(auto it = free_slot_table.begin(); it != free_slot_table.end(); it++ ) {
		msg += "(";
		msg += std::to_string(it->first);
		msg += ",";
		msg += std::to_string(it->second);
		msg += ")";
	}	
	SC_REPORT_ERROR("KisTA",msg.c_str());
}

// For allocating a specific (and contiguous) set of slots
// It raises an error if there are busy slots in that region
void tdma_bus::assign_free_slots(unsigned int first_slot, unsigned int req_slots) {
	std::string msg;
	free_slot_range_t free_slot_range;
	if(free_slot_table.size()<1) {
		msg = "In TDMA bus ";
		msg += name();
		msg += ": Empty free slot table when trying to assign free slots.";
		SC_REPORT_ERROR("KisTA",msg.c_str());
	}
	// First searches in the regions if the first slot is comprised in the range
	// If it is not found, it raises the error, if it is found, it is checked
	// that it fits in the range and the free slots table updated accordingly
	for(auto it = free_slot_table.begin(); it != free_slot_table.end(); it++ ) {
		free_slot_range = *it;
		
		if( (first_slot >= free_slot_range.first) && (first_slot <= free_slot_range.second) ) {
			// First slot is in this free range, here is where we have to check if the allocation
			// is possible
			if(req_slots < (free_slot_range.second - free_slot_range.first + 1) ) {
				// slots taken from this range:
				// update the range ...
				it->first = first_slot + req_slots;
				// ... and first slot from the range does not need to be returned, because it is known by the caller
				return;
			} else if ( req_slots == (free_slot_range.second - free_slot_range.first + 1) ) {
				// slots taken from this range:
				// eliminate the range entry from the vector...
				free_slot_table.erase(it);
				// ... and first slot from the range selected returned
				return;
			} else {
				it++; // no room in this entry, the next entry in the
					  // free slot table is tried
			}
		} else {
			it++; // looks for the next free range
		}
	}
	// reaching this point means that the first slot was not found in any
	// free range of the table
	msg = "In TDMA bus ";
	msg += name();
	msg += ": Trying to allocate ";
	msg += std::to_string(req_slots);
	msg += " slots from slot ";
	msg += std::to_string(first_slot);
	msg += " Current status of the allocation table:";
	for(auto it = free_slot_table.begin(); it != free_slot_table.end(); it++ ) {
		msg += "(";
		msg += std::to_string(it->first);
		msg += ",";
		msg += std::to_string(it->second);
		msg += ")";
	}	
	SC_REPORT_ERROR("KisTA",msg.c_str());
}


void tdma_bus::allocate_channels(phy_address src, unsigned int required_channels, unsigned int first_slot) {

	std::string  msg;

	manual_slot_allocation = true;
	
	// check there is enough slots available (this is actually redundant, since the 
	// assgin_free_slots function makes a tighter check, but it allows to early detect
	// bad configurations
	if( (required_channels + n_allocated_channels) > n_channels ) {
		msg= "In TDMA bus ";
		msg += name();
		msg= ": Trying to assign more transmission slots (";
		msg+= std::to_string(required_channels);
		msg+= ") to sender physical address (PE) ";
		msg+= src->name();
		msg+= ") than the available slots (";
		msg+= std::to_string(n_channels-n_allocated_channels);
		msg+= ")";
		SC_REPORT_ERROR("KisTA",msg.c_str());
	}
	
	assign_free_slots(required_channels, first_slot);

	// associates the assigned channels to the PE in the channel allocation table
	channel_allocation_table[src] = required_channels;
	
	// associates the first slot allocated to the PE in the first_slot allocation_table
	first_slot_allocation_table[src] = first_slot;
	
	// update the account of channels assigned 
	n_allocated_channels += required_channels;

#ifdef _REPORT_TDMA_BUS_CONFIGURATION	
	msg = "TDMA bus ";
	msg += name();
	msg += ": Assigning ";
	msg += std::to_string(required_channels);
	msg += " transmission channels (slots) from slot #";
	msg += std::to_string(first_slot);
	msg += " to sender physical address (PE) ";
	msg+= src->name();
	SC_REPORT_INFO("KisTA",msg.c_str());
#endif

}


void tdma_bus::allocate_channels(phy_address src, unsigned int required_channels) {

	std::string  rpt_msg;
	
	unsigned int first_slot;
	
	// check there is enough slots available
	if( (required_channels + n_allocated_channels) > n_channels ) {
		rpt_msg= "Trying to assign ";
		rpt_msg+= std::to_string(required_channels);
		rpt_msg+= " transmission channels (slots) to sender physical address (PE) ";
		rpt_msg+= src->name();
		SC_REPORT_ERROR("KisTA: TDMA bus",rpt_msg.c_str());
	}

	// assign free slots and retrieve automatically first slot
	first_slot = assign_free_slots(required_channels);

	// associates the assigned channels to the PE in the channel allocation table
	channel_allocation_table[src] = required_channels;

	// associates the first slot allocated to the PE in the first_slot allocation_table
	first_slot_allocation_table[src] = first_slot;

	// update the account of channels assigned
	n_allocated_channels += required_channels;

#ifdef _REPORT_TDMA_BUS_CONFIGURATION	
	rpt_msg = "TDMA bus ";
	rpt_msg += name();
	rpt_msg += ": Assigning ";
	rpt_msg += std::to_string(required_channels);
	rpt_msg += " transmission channels (slots) from slot #";
	rpt_msg += std::to_string(first_slot);
	rpt_msg += " to sender physical address (PE) ";
	rpt_msg+= src->name();
	SC_REPORT_INFO("KisTA: TDMA bus",rpt_msg.c_str());
#endif

}

//  redefinition for generic network
unsigned int tdma_bus::allocate_BW(phy_address src, unsigned int required_bps) {

	std::string  rpt_msg;
	unsigned required_slots;

#ifdef _REPORT_TDMA_BUS_CONFIGURATION
	rpt_msg= "Requesting ";
	rpt_msg+= std::to_string(required_bps);
	rpt_msg+= " (bps) to sender physical address (PE) ";
	rpt_msg+= src->name();
	SC_REPORT_INFO("KisTA: TDMA bus",rpt_msg.c_str());
#endif

	// calculated required slots
	required_slots = (unsigned int)((double)required_bps/(double)channel_eBW)+1;
	allocate_channels(src,required_slots);

#ifdef _REPORT_TDMA_BUS_CONFIGURATION
	rpt_msg= "Allocating ";
	rpt_msg+= std::to_string(required_slots*channel_eBW);
	rpt_msg+= " (bps) to sender physical address (PE) ";
	rpt_msg+= src->name();
	SC_REPORT_INFO("KisTA: TDMA bus",rpt_msg.c_str());
#endif
			
	return required_slots*channel_eBW;
}
	
#define MAX_TDMA_BUS_CONF_ERROR 0.001
#define MAX_TDMA_BUS_CONF_TIME_ERROR sc_time(1,SC_PS);

void tdma_bus::report_bus_configuration() {
	cout << "TDMA Bus " << name() << " configuration completed: " << endl;
	cout << "\t\t static channels = " << n_channels << endl;
	if(channel_capacity!=0) { // if configured
		cout << "\t\t static channel capacity (bits) = " << channel_capacity << endl;			
	}
	cout << "\t\t ch. payload_bits = " << ch_payload_bits << endl;
	if(ch_payload_rate!=0.0) { // if configured
		cout << "\t\t ch. payload_rate (ch payload bits/channel capacity) = " << ch_payload_rate << endl;	
	}
	cout << "\t\t slot time (static channel)  = " << slot_time << endl;
	cout << "\t\t guard time (per channel)= " << guard_time << endl;
	cout << "\t\t cycle time = " << cycle_time << endl;
	if(channel_BW!=0.0) { // if configured
		cout << "\t\t channel BW (bps) = " << channel_BW << endl;
	}
	cout << "\t\t channel eBW (bps) = " << channel_eBW << endl;
	cout << "\t\t bus effective BW (bps)= " << eBW << endl;
	
	print_slot_allocation();
}

bool tdma_bus::check_slot_allocation() {
	// checks that there are not overlaps in the slot allocation
	// (the check does not report error if there are unsused slots)
	std::string msg;
	
	unsigned int fs_1, fs_2; // first slot index
	unsigned int n_slots_1, n_slots_2;
	
	print_slot_allocation();
	
	first_slot_allocation_table_t::iterator it1, it2;
	channel_allocation_table_t::iterator nch_it1, nch_it2;
	
	if(first_slot_allocation_table.size() != channel_allocation_table.size() ) {
		msg = "In TDMA bus ";
		msg += name();
		msg += ": Slot allocation check failed because \"first slot\" table size (";
		msg += std::to_string( first_slot_allocation_table.size() );
		msg += ") is different from the \"allocated slots\" table size (";
		msg += std::to_string( channel_allocation_table.size() );
		msg += ").";
		msg += "There might be an unexpected error with the automatic slot allocation.";
		msg += "Please, report the bug to the authors.";
		SC_REPORT_ERROR("KisTA",msg.c_str());
	}

	// here, it is ensured that the sizes of both the first_slot_allocation_table and the channel_allocation_table are the same
	switch(first_slot_allocation_table.size()) {
		case 0:
			msg = "In TDMA bus ";
			msg += name();
			msg += ": Slot allocation check failed because slot allocation is empty.";
			SC_REPORT_ERROR("KisTA",msg.c_str());
			break;
		case 1:
			return true; // Only one transmitter is allocating slots, so   
			break;       // there cannot be any overlap, so the check is sucessful
		//default:
			// more than one transmitter is allocating slots, the check must go on
	}
	
	// Here the channel allocation has been done for more than one transmitter (table sizes >1, and with the same size)

	it1 = first_slot_allocation_table.begin();
	it2 = it1;
	it2++;
				
	// it1: iterator in first slots
	do {
	
		do {
			fs_1 = it1->second;
			fs_2 = it2->second;				
			
			nch_it1 = channel_allocation_table.find(it1->first);
			// check if, due to a failure on the manual assignation,
			// the PE in the n-th allocation is not found in the slots allocated table
			if( nch_it1 == channel_allocation_table.end() ) {
				msg = "In TDMA bus ";
				msg += name();
				msg = "Slot allocation check failed.";
				msg += " PE ";
				msg += it1->first->name();
				msg += " appears in the ""first slot"" allocation table, slot (";
				msg += std::to_string(fs_1);
				msg += "), but it is not found in the ""allocated channels"" table.";
				if(manual_slot_allocation) {
					msg += "Please, check your manual slot allocation.";
				} else {
					msg += "There might be an unexpected error with the automatic slot allocation.";
					msg += "Please, report the bug to the authors.";
				}
				SC_REPORT_ERROR("KisTA",msg.c_str());			
			}
			
			nch_it2 = channel_allocation_table.find(it2->first);
			// check if, due to a failure on the manual assignation,
			// the PE in the (n+1)-th allocation is not found in the slots allocated table			
			if( nch_it2 == channel_allocation_table.end() ) {
				msg = "In TDMA bus ";
				msg += name();
				msg = "Slot allocation check failed.";
				msg += " PE ";
				msg += it2->first->name();
				msg += " appears in the ""first slot"" allocation table, slot (";
				msg += std::to_string(fs_2);
				msg += "), but it is not found in the ""allocated channels"" table.";
				if(manual_slot_allocation) {
					msg += "Please, check your manual slot allocation.";
				} else {
					msg += "There might be an unexpected error with the automatic slot allocation.";
					msg += "Please, report the bug to the authors.";
				}
				SC_REPORT_ERROR("KisTA",msg.c_str());			
			}
						
			// the same PE is found both in the "first slot" table and 
			// in the "channels allocated" table, so the value is retrieved
			n_slots_1 = nch_it1->second;
			n_slots_2 = nch_it2->second;
						
			// check a possible overlap
			// note: it takes into account that fs_1 can be greater or lesser than fs_2,
			//       since the slots table is indexed on phy_address (PEs).
			if( ( (fs_1<fs_2 ) && ( (fs_1+n_slots_1-1) >= fs_2  )  )
				||
				( (fs_2<fs_1 ) && ( (fs_2+n_slots_2-1) >= fs_1  )  )
			  ) {
				msg = "In TDMA bus ";
				msg += name();
				msg += ": Slot allocation check failed. Uncompatible slot assignation.\n";
				msg += " Slot assignation to PE ";
				msg += it1->first->name();
				msg += " slots [";
				msg += std::to_string(fs_1);
				msg += ",";
				msg += std::to_string(fs_1+n_slots_1-1);
				msg += "] overlaps the slot assignation to PE ";
				msg += it2->first->name();
				msg += "[ ";
				msg += std::to_string(fs_2);
				msg += ",";

				msg += std::to_string(fs_2+n_slots_2-1);
				msg += "].\n";
				if(manual_slot_allocation) {
					msg += "Please, check your manual slot allocation.";
				} else {
					msg += "There might be an unexpected error with the automatic slot allocation.";
					msg += "Please, report the bug to the authors.";
				}
				SC_REPORT_ERROR("KisTA",msg.c_str());	
			}
			
			it2++;

		} while(it2 != first_slot_allocation_table.end());
		it1++;
		it2 = it1;
		it2++;
	} while(it2 != first_slot_allocation_table.end());

	return true;
}

bool tdma_bus::print_slot_allocation() {
	channel_allocation_table_t::iterator it2;
	cout << "Slot allocation:" << endl;
	cout << " PE\tfirst slot\tslots allocated:" << endl;
	for(auto it = first_slot_allocation_table.begin(); it!=first_slot_allocation_table.end() ; it++) {
		cout << " " << endl;
		//cout << it->first;
		cout << it->first->name();
		cout << "\t";
		cout << it->second;
		cout << "\t";
		it2 = channel_allocation_table.find(it->first);
		if( it2 == channel_allocation_table.end() ) {
			cout << "?";
		} else {
			cout << it2->second;
		}
		cout << endl;
	}
}

void tdma_bus::before_end_of_elaboration() {

	std::string rpt_msg;
	double error;
	double calc_prate;
			
	// (1) 
	// First check that the configuration is mandatory parameters complete
	
	//  number of channels
	if(n_channels==0) {
		rpt_msg = "Mandatory attribute, ";
		rpt_msg += "number of channels ";
		rpt_msg += "not configured in TDMA bus ";		
		rpt_msg += name();
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	}
	
	//  slot time
	if(slot_time==SC_ZERO_TIME) {
		rpt_msg = "Mandatory attribute, ";
		rpt_msg += "slot_time ";
		rpt_msg += "not configured in TDMA bus ";		
		rpt_msg += name();
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	}
	
	//  payload: direct or indirect configuration
	if( (ch_payload_bits==0) && ( (ch_payload_rate==0.0) || (channel_capacity==0) ) ) {
		// calculate payload rate
		ch_payload_rate = ((double)ch_payload_bits)/((double)channel_capacity);
		rpt_msg = "Payload bits of the slot has not been configured, and remaining data from the configuration of bus  \"";
		rpt_msg += name();
		rpt_msg += " do not allow to extract the it from the current configuration of the tdma bus.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	} else if( (ch_payload_bits==0) && (ch_payload_rate!=0.0) && (channel_capacity!=0)  ) {
		// calculate payload in bits from channel capacity and payload rate
		ch_payload_bits = (unsigned int)(channel_capacity*ch_payload_rate); // be aware of truncation
#ifdef _REPORT_TDMA_BUS_CONFIGURATION	
		rpt_msg = "TDMA bus  \"";
		rpt_msg += name();
		rpt_msg += ": payload bits= ";
		rpt_msg += std::to_string(ch_payload_bits);
		rpt_msg += " calculated from channel capacity and payload rate configured.";
		SC_REPORT_INFO("KisTA",rpt_msg.c_str());
#endif		
	} else if( (ch_payload_bits!=0) && (ch_payload_rate!=0.0) && (channel_capacity!=0)  ) {
		// double configuration of payloads bits, checking consistency
		calc_prate = ((double)ch_payload_bits)/((double)channel_capacity);
		// check the consistency of the payload configuration
		// both settled, the consistency has to be cheked, possibility of error
		if(calc_prate > ch_payload_rate) {
			error = (calc_prate - ch_payload_rate)/ch_payload_rate;
		} else {
			error = (ch_payload_rate - calc_prate)/calc_prate;
		}
	
		if(error>MAX_TDMA_BUS_CONF_ERROR) {
			rpt_msg = "Inconsistent configuration for TDMA bus \"";
			rpt_msg += name();
			rpt_msg += "\".\n The payload of the channel has been configured by two means in an inconsistent way, taking into account channel capacity.";
			SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		}
		
#ifdef _WARN_POSSIBLE_ACCURACY_INCONSISTENCIES_FOR_TDMA_BUS
		if(error>0.0) {
			rpt_msg = "There is an small inconsistency in payload configuration of the channel for TDMA bus ";
			rpt_msg += name();
			rpt_msg += ".\n The inconsistency is lesser than ";
			rpt_msg += std::to_string(MAX_TDMA_BUS_CONF_ERROR*100.0);
			rpt_msg += "%% which could be caused by accuracy reasons.";
			SC_REPORT_WARNING("KisTA",rpt_msg.c_str());
		}
#endif			
	}
	
	
	//  cycle_time: direct or indirect configuration
	if( (cycle_time==SC_ZERO_TIME) ) { // here the code assumes that n_channels and slot_time are set
		// calculate payload rate
		cycle_time = n_channels*(slot_time+guard_time);
#ifdef _WARN_CYCLE_TIME_NOT_CONFIGURED_FOR_TDMA_BUS		
		rpt_msg = "Cycle time of the bus  \"";
		rpt_msg += name();
		rpt_msg += "\" has not been configured.";
		rpt_msg += " KisTA assumes that the bus is composed only of static slots (";
		rpt_msg += std::to_string(n_channels);
		rpt_msg += "), and considers the configured slot time (";
		rpt_msg += slot_time.to_string();
		rpt_msg += ") and guard time (";
		rpt_msg += guard_time.to_string();
		rpt_msg +=  ") to set and implicit TDMA bus cycle equal to ";
		rpt_msg += cycle_time.to_string();
		rpt_msg +=  ".";
		SC_REPORT_WARNING("KisTA",rpt_msg.c_str());
#endif
	}

	
	// calculate the channel bandwidth		
	channel_BW = (unsigned int) (double)channel_capacity/cycle_time.to_seconds();
	channel_eBW = (unsigned int) (double) ch_payload_bits/cycle_time.to_seconds();
	// Calculate effective Bandwidth in bps of the bus
	eBW = channel_eBW * n_channels;

#ifdef _REPORT_CONFIGURATION_COMPLETION_TDMA_BUS	
	rpt_msg = "Completing configuration of TDMA bus ";
	rpt_msg += name();
	rpt_msg += ": channel bandwidth : ";
	rpt_msg += std::to_string(channel_BW);
	rpt_msg += " bps.\n";
	rpt_msg += ": channel effective bandwidth : ";
	rpt_msg += std::to_string(channel_eBW);
	rpt_msg += " bps.\n";
	rpt_msg += ": bus effective bandwidth : ";
	rpt_msg += std::to_string(eBW);
	rpt_msg += " bps.\n";	
	SC_REPORT_INFO("KisTA",rpt_msg.c_str());
#endif
	
	// Check that the amount of allocated channels does not exceeds the available channels
	if(n_allocated_channels>n_channels) {
		rpt_msg= std::to_string(n_allocated_channels);
		rpt_msg+= " channels allocated, to channel";
		rpt_msg+= name();
		rpt_msg+= ", which has a capacity of ";
		rpt_msg+= std::to_string(n_channels);
		rpt_msg+= ".";
		SC_REPORT_ERROR("KisTA: TDMA bus",rpt_msg.c_str());
	}

	// check the cycle time
	if (cycle_time < n_channels*(slot_time+guard_time) ) {
		rpt_msg = "The cycle time ";
		rpt_msg += cycle_time.to_string();
		rpt_msg += " of the TDMA bus \"";
		rpt_msg += name();
		rpt_msg += "\" is smaller than the time required by its ";
		rpt_msg += std::to_string(n_channels);
		rpt_msg += " slots, a time slot of ";
		rpt_msg += slot_time.to_string(); 		
		rpt_msg += ", and an additional guard of ";
		rpt_msg += guard_time.to_string(); 
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	}
	
	// warn a cycle time bigger
#ifdef _WARN_CYCLE_TIME_BIGGER_FOR_TDMA_BUS	
	if (cycle_time > n_channels*(slot_time+guard_time) ) {
		rpt_msg = "The cycle time ";
		rpt_msg += cycle_time.to_string();
		rpt_msg += " of the TDMA bus \"";
		rpt_msg += name();
		rpt_msg += "\" is bigger than the time required by its ";
		rpt_msg += std::to_string(n_channels);
		rpt_msg += " slots, a time slot of ";
		rpt_msg += slot_time.to_string(); 		
		rpt_msg += ", and an additional guard of ";
		rpt_msg += guard_time.to_string();
		rpt_msg += ". Likely you are modelling a TDMA bus with dynamic channels.";
		rpt_msg += " Otherwise, be aware that a longer cycle time involves less channel bandwidth.\n";
		SC_REPORT_WARNING("KisTA",rpt_msg.c_str());
	}
#endif

	// check slot allocation
	check_slot_allocation();
	
#ifdef _REPORT_TDMA_BUS_CONFIGURATION
	report_bus_configuration();
#endif
	
	// The following code, completes the connection of the TDMA model to
	// the KisTA framework through the comm_res (generic network) base class
	//
	// 			(Another part is done so far through the use in the ellaboration phase
	// 			of the constructors and of the inherited "plug" method, and checks)
	//
	// The following code creates a new net link per each allocated connection 
	//
/*	channel_allocation_table_t::iterator it;
	
	for(it=channel_allocation_table.begin();
		it!=channel_allocation_table.end();
		it++) {
	
		//Fill the link information
		// it->first (key) is the connection (@src,@dest)
		// if->second (key) is the number of allocated channels
//		add_link(it->first);
			// calls the add_link of the base class with the same keys
			// Each registered and allocated logic link creates an entry
			// in the table associated to the base class (gen_network)
			// with the generic properties of the virtual link
			
	    // Following we could fill the static properties of each logic link
	    // However, they are not so interesting, since in the TDMA case,
	    // The response time depends on the size of the
	    // message, which involves in any case the recalculation of 
	    // the P2P delay for each invokation of the method Current...
	    // or even Max, every call.
	}
*/

	//
	// The connection of the TDMA model to the comm_res (generic network) base class
	// is completed in the system specification, when bindings between system channels
	// and the communication resources are done
	//
	
	// Following, completes the static timing characterization of the tdma bus for all
	// the registeres physical links, given the slot allocation provided

/*	
	links_table_t::iterator link_it;
	link_info_t	current_link_info;
	
	for(link_it=links_table.begin();
		link_it!=links_table.end();
		link_it++) {
			
		current_link_info = link_it->second;
		
		calculate_and_set_static_MaxP2Pdelay(link_it,msg_size,current_link_info); // refer to plinks info
				
	}
*/

  // to draw the communication resource
  sketch_report.draw(this);

} // end of before_end_of_elaboration

/*
void tdma_bus::end_of_elaboration() {
	
}*/ // end of end_of_elaboration


// HELPER FUNCTIONS

//
// methods for current delay
//

// Assumes slots_allocated >0
sc_time tdma_bus::max_offset_delay(unsigned int slots_allocated) {
	return ( cycle_time - (slots_allocated-1)*slot_time );
}

// povides the actual time dependent offset, by considering the current time, 
// and the first slot assigned
  sc_time tdma_bus::current_offset_delay_L0(sc_time current_time, unsigned int first_slot) {
	sc_time ret_offset_delay;
	sc_time time_first_assigned_slot;
	sc_time current_phase_time;

	unsigned int run_cycles;
	
	time_first_assigned_slot = first_slot * slot_time;
	
	// calculate current phase time
	// current_phase_time = (initial_offset + current_time) mod cycle_time
	
	// number of TDMA cycles completed (assuming that an initial offset can be configured)
	run_cycles = (unsigned int) ( (initial_offset_time + current_time) / cycle_time ); // initial offset time = 0s on default
	
	// 
	current_phase_time = (initial_offset_time + current_time) - cycle_time * run_cycles;

	// the actual delay follows a modular arithmetic

	//if(current_phase_time < time_first_assigned_slot) { // we use this if we consider that reaching exactly at the time slot time 
	                                                    // prevents to use the slot
	if(current_phase_time <= time_first_assigned_slot) {	                                                    
		ret_offset_delay = time_first_assigned_slot - current_phase_time;
	} else {
		ret_offset_delay = cycle_time - current_phase_time + time_first_assigned_slot;
	}
	
	return ret_offset_delay;
}

// povides a random value between 0 and the maximum offset
// This model does not need to handle a list of which slot is allocated to which PE
// However is less accurate and does not let to take into account and design orders in the assignation
// of the slots in the TDMA bus, which could optimize communication in lesser TMDA cycles.
sc_time tdma_bus::current_offset_delay_L1(unsigned int slots_allocated) {
	sc_time	ret_offset_delay;
	
	ret_offset_delay =  max_offset_delay(slots_allocated) * (double)rand() / (double)RAND_MAX;
	
	return ret_offset_delay;
}


//
// It calculates the transmission delay at a bit accurate level. For it, the model assumes that the 
// slots allocated are consecutive.
//
//
sc_time tdma_bus::calculate_data_tx_delay_L0(unsigned int msg_size, unsigned int slots_allocated) {
	double slots_required_double;
	unsigned int slots_required, tdma_cycles_required, slots_last_period;
	sc_time data_tx_delay;
	sc_time send_time_last_slot;

	// it is a communication along the bus
	slots_required_double = (double)msg_size/(double)ch_payload_bits;
	
	if(msg_size%ch_payload_bits) { // there is remainder
		slots_required = msg_size / ch_payload_bits + 1; // +1 for ceiling, once / truncates
	} else {
		slots_required = msg_size / ch_payload_bits;
	}

	// calculate the number of cycles required, given the slots required and the slots allocated
	if(slots_required%slots_allocated) {
		tdma_cycles_required = slots_required/slots_allocated +1;
	} else {
		tdma_cycles_required = slots_required/slots_allocated;
	}

	slots_last_period = slots_required - (tdma_cycles_required-1) *slots_allocated;
				    		
    send_time_last_slot = (slots_required_double - (double)(slots_required-1) ) * slot_time;
	    		
	data_tx_delay = (tdma_cycles_required-1)*cycle_time + (slots_last_period-1)*slot_time + send_time_last_slot;
	
	return data_tx_delay; // transmission delay not taken into account. It is considered embedded in the model	
}


sc_time tdma_bus::calculate_data_tx_delay_L1(unsigned int msg_size, unsigned int slots_allocated) {
	unsigned int slots_required, tdma_cycles_required, slots_last_period;
	sc_time data_tx_delay;

	// it is a communication along the bus

	if(msg_size%ch_payload_bits) { // there is remainder
		slots_required = msg_size / ch_payload_bits + 1; // +1 for ceiling, once / truncates
	} else {
		slots_required = msg_size / ch_payload_bits;
	}

	// calculate the number of cycles required, given the slots required and the slots allocated
	if(slots_required%slots_allocated) {
		tdma_cycles_required = slots_required/slots_allocated +1;
	} else {
		tdma_cycles_required = slots_required/slots_allocated;
	}
	
	slots_last_period = slots_required - (tdma_cycles_required-1) *slots_allocated;
				
	data_tx_delay = (tdma_cycles_required-1)*cycle_time + slots_last_period*slot_time;
	
	return data_tx_delay; // transmission delay not taken into account. It is considered embedded in the model
	
}

//
// methods for maximum delay
//


// LEVEL 0
// a bit accurate bound, capable to consider that the sending might not require the full slot time

sc_time tdma_bus::calculate_MaxP2Pdelay_L0(unsigned int msg_size, unsigned int slots_allocated) {
	sc_time BoundP2PTxDelay;

	BoundP2PTxDelay = max_offset_delay(slots_allocated) + calculate_data_tx_delay_L0(msg_size,slots_allocated);
	
	return BoundP2PTxDelay;	
}

// LEVEL 1
// a slot accurate bound

sc_time tdma_bus::calculate_MaxP2Pdelay_L1(unsigned int msg_size, unsigned int slots_allocated) {
	sc_time BoundP2PTxDelay;

	BoundP2PTxDelay = max_offset_delay(slots_allocated) + calculate_data_tx_delay_L1(msg_size,slots_allocated);
	
	return BoundP2PTxDelay;
}

// LEVEL 2
// a tdma-cycle accurate, but faster calculated bound (over the previous one)
sc_time tdma_bus::calculate_MaxP2Pdelay_L2(unsigned int msg_size, unsigned int slots_allocated) {
	std::string	rpt_msg;
	unsigned int slots_required, tdma_cycles_required;
	sc_time data_tx_delay;
	sc_time BoundP2PTxDelay;

	// calculate slots required
	if(msg_size%ch_payload_bits) { // there is remainder
		slots_required = msg_size / ch_payload_bits + 1; // +1 for ceiling, once / truncates
	} else {
		slots_required = msg_size / ch_payload_bits;
	}

	// one TDMA cycle more added 
	if(slots_required%slots_allocated) { // there is remainder
		tdma_cycles_required = slots_required/slots_allocated + 2;
	} else {
		tdma_cycles_required = slots_required/slots_allocated +1;
	}
	BoundP2PTxDelay = tdma_cycles_required*cycle_time;
	
	return BoundP2PTxDelay; // transmission delay not taken into account. It is considered embedded in the model

}


sc_time tdma_bus::calculate_MaxP2Pdelay(unsigned int msg_size, unsigned int slots_allocated) {
	std::string msg;
	//cout << "CALCULATE MAX (BOUND) P2P DELAY" << endl;

#ifdef _ENABLE_CHECK_ALLOCATED_CHANNELS_IN_CALCULATE_MAX_P2P_DELAY	
	if(slots_allocated>n_channels) {
		msg = "Calling calculate_MaxP2Pdelay in TDMA bus ";
		msg += name();
		msg += " for ";
		msg += std::to_string(slots_allocated);
		msg += " slots, while the bus has ";
		msg += n_channels;
		msg += " slots.";
		SC_REPORT_ERROR("KisTA", msg.c_str());
	}
#endif
	
	switch(accuracy_level){
		case 0:
			return calculate_MaxP2Pdelay_L0(msg_size,slots_allocated);
			break;
		case 1:
			return calculate_MaxP2Pdelay_L1(msg_size,slots_allocated);
			break;
		case 2:
			return calculate_MaxP2Pdelay_L2(msg_size,slots_allocated);
			break;
		default:
			msg= "TDMA bus ";
			msg += name();
			msg += ": unknown accuracy level calculating Max. P2P delay. Supported ones range 0(most accurate, slower) to 2(faster-less accurate)";
			SC_REPORT_ERROR("KisTA",msg.c_str());
	}
	
}


// LINK ATTRIBUTES METHODS


void tdma_bus::set_MaxP2Pdelay(phy_link_t &plink,
								unsigned int msg_size
								) {
	std::string	rpt_msg;
	unsigned int slots_allocated;
	sc_time comm_delay;
	
	//link_info_t *plink_info_p;
	
	// retrieve slots allocated from the channel allocation table
	slots_allocated = channel_allocation_table[plink.src];
			
	if(slots_allocated==0) {
		rpt_msg = "No slot allocated for the physical link (";
		rpt_msg += plink.src->name();
		rpt_msg += ",";
		rpt_msg += plink.dest->name();
		rpt_msg += ").";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	} 

 	comm_delay = calculate_MaxP2Pdelay(msg_size,slots_allocated);
 			
	// NOTE: 
	//  Here it uses the inherited function of the comm_res base class
	// caching of the delay value (for the given message size)
	// This caching is profitable since the delay values depend only on
	// message size and the number of slots allocated (fixed for each plink),
	// so for each link, the delay depends only on the message size.
	
	set_MaxP2Pdelay(plink,comm_delay, msg_size);

}


sc_time &tdma_bus::get_MaxP2Pdelay(phy_link_t &plink,
									unsigned int msg_size
									) {
		
		link_info_t *plink_info_p;
		
		// retrieve properties stored (by the base comm_res)
		plink_info_p = get_properties(plink, msg_size);
		
		if(plink_info_p==NULL) { // property for the link-msg_size combination not found
			this->set_MaxP2Pdelay(plink,msg_size); // refer to plinks info
			
			// retrieve the properties again
			plink_info_p = get_properties(plink, msg_size);
			// ... and it should not fail again
			if(plink_info_p==NULL) { 
				std::string rpt_msg;
				std::ostringstream os;
				rpt_msg = "Setting properties of link ";
				os << plink;
				rpt_msg += os.str();
				rpt_msg += " in TDMA bus ";
				rpt_msg += name();
				rpt_msg += " failed.";
				SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
			}
		}
		
		return plink_info_p->getMaxP2Pdelay(msg_size);

}

void tdma_bus::set_MaxP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size) { 
	phy_link_t plink;
	plink.src=src;
	plink.dest=dest;
	set_MaxP2Pdelay(plink,msg_size);
}

sc_time &tdma_bus::get_MaxP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size) { 
	phy_link_t plink;
	plink.src=src;
	plink.dest=dest;
	return get_MaxP2Pdelay(plink,msg_size);
}

void tdma_bus::set_CurrentP2Pdelay(phy_link_t &plink,
									unsigned int msg_size
								) {
	std::string	rpt_msg;
	unsigned int slots_allocated;
	sc_time data_tx_delay;
	
	//link_info_t *plink_info_p;
	
	// retrieve slots allocated from the channel allocation table
	slots_allocated = channel_allocation_table[plink.src];
			
	if(slots_allocated==0) {
		rpt_msg = "No slot allocated for the physical link (";
		rpt_msg += plink.src->name();
		rpt_msg += ",";
		rpt_msg += plink.dest->name();
		rpt_msg += ").";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	} 

 	data_tx_delay = calculate_data_tx_delay_L0(msg_size,slots_allocated);
			
	// NOTES:
	//  - Here the inherited function of the comm_res base class is used
	//  to cache the transmission delay value (that is, only the time invariant)
	//  part. Therefore, the get_CurrentP2Pdelay has to take that into account
	//  in order to later one compose the accurate time-variant value
	set_CurrentP2Pdelay(plink,data_tx_delay, msg_size);

}

sc_time &tdma_bus::get_CurrentP2Pdelay(phy_link_t &plink,
									unsigned int msg_size
									) {
		
		std::string msg;
		
		unsigned int slots_allocated, first_slot;
		sc_time current_offset_delay, data_tx_delay;
		
		link_info_t *plink_info_p;
		
		// retrieve properties stored (by the base comm_res)
		plink_info_p = get_properties(plink, msg_size);
		
		if(plink_info_p==NULL) { // property for the link-msg_size combination not found
			this->set_CurrentP2Pdelay(plink,msg_size); // refer to plinks info
			
			// retrieve the properties again
			plink_info_p = get_properties(plink, msg_size);
			// ... and it should not fail again
			if(plink_info_p==NULL) { 
				std::string rpt_msg;
				std::ostringstream os;
				rpt_msg = "Setting properties of link ";
				os << plink;
				rpt_msg += os.str();
				rpt_msg += " in TDMA bus ";
				rpt_msg += name();
				rpt_msg += " failed.";
				SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
			}
		}
		
		data_tx_delay = plink_info_p->getMaxP2Pdelay(msg_size);
		
		// NOTE:
		//   Notice that in the CurrentP2P call, only the time invariant component 
		//   was cached.
		// Now, the time dependent component is added.
		// A faster alternative is to use the L1 accuracy level, which provides
		// a random offset bounded by the maximum offset
		switch(accuracy_level) {
			case 0:
				first_slot = first_slot_allocation_table[plink.src];
				current_offset_delay = current_offset_delay_L0(sc_time_stamp(),first_slot);
				break;
			case 1:
				slots_allocated = channel_allocation_table[plink.src];
				current_offset_delay = current_offset_delay_L1(slots_allocated);
				break;
			default:
				msg = "TDMA bus ";
				msg += name();
				msg += ": unknown accuracy level calculating Current P2P delay. Supported ones range 0(most accurate, slower) to 1(faster-less accurate)";
				SC_REPORT_ERROR("KisTA",msg.c_str());					
		}
		current_tx_delay = current_offset_delay + data_tx_delay; // Note that we use a member variable of tdma_bus class (instead a local variable)
		                                                         // to enable the return of a refernce
		return current_tx_delay;
}

void tdma_bus::set_CurrentP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size) { 
	phy_link_t plink;
	plink.src=src;
	plink.dest=dest;
	set_CurrentP2Pdelay(plink,msg_size);
}

sc_time &tdma_bus::get_CurrentP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size) { 
	phy_link_t plink;
	plink.src=src;
	plink.dest=dest;
	return get_CurrentP2Pdelay(plink,msg_size);
}

void  tdma_bus::set_accuracy_level(accuracy_level_t level) {
	check_call_before_sim_start("set_accuracy_level");
	accuracy_level = level;
}

accuracy_level_t &tdma_bus::get_accuracy_level() {
	return accuracy_level;
}

} // namespace kista

#endif
