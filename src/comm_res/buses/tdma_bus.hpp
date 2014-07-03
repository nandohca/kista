/*****************************************************************************

  tdma_bus.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 August
  Last Date revision: 2014 May
  
   Notes: TDMA bus model relying on the generic network model network
			* Notice that in principle, the allocation of channels could be either static or dynamic
			* Current version supports static allocation of channels
			* The configuration of the TDMA bus characteristics can be done
			  in many ways. The class will check at the end of the elaboration
			  if the minimum set of configured values has been provided,
			  to provide default values if it is possible (and warn the user) and 
			  to ensure their consistency
			* transmission delay is associated either to the PE (if there is no netif)
			  or to the netif itself (the TDMA bus model does not add such delays)
			* Levels of accuracy for the bounds on the p2p delay is supported
			* Current delays considering the current status of the TDMA bus
			* Automatic and manual assignation of slots (based on the assignation
			  of contiguous ranges)

 *****************************************************************************/

#ifndef TDMA_BUS_HPP
#define TDMA_BUS_HPP

#include <systemc.h>

#include "defaults.hpp"
#include "types.hpp"

#include "comm_res/phy_link.hpp"
#include "comm_res/phy_comm_res.hpp"

#include <list>

namespace kista {

typedef std::map<phy_address,unsigned int> channel_allocation_table_t;
typedef std::map<phy_address,unsigned int> first_slot_allocation_table_t;
typedef std::pair<unsigned int, unsigned int>	free_slot_range_t; // range [2,4] means slots 2, 3 and 4
typedef std::list<free_slot_range_t> free_slot_allocation_table_t;

// typedef std::map<net_link_t,unsigned int> net_channel_allocation_t;
// typedef std::map<logic_link_t,unsigned int> logic_channel_allocation_t;

//class tdma_bus : public gen_network {


class tdma_bus : public	phy_comm_res_t {
	//class tdma_bus : public comm_res<phy_address> {
public:
	// constructor
	// Generation of the tdma_bus object for a separated configuration (useful for XML front-end)
	tdma_bus(sc_module_name name = sc_gen_unique_name("tdma_bus"));

	// Four ways for fast full specification of the tdma_bus are given
	//   Note that the following parameters are always mandatory:
	//          * n_channels_par: Number of channels (slots) of the TDMA bus
	//			* BW_par		: Bandwidth of the channels (transmission rate in bps)
	//                            (that is, it does not refer to the effective BW, after considering guard times,
	//                             but it refers to the transmission speed in bps of the underlying communication media)
	//
	
	tdma_bus(unsigned int		n_channels_par,
			  sc_time 			slot_time_par,  // time assigned to the time slot
			  double 			ch_payload_bits_par, // payloads bits transferred in each slot (they can reflect any payload rate in (0-1) )
			  sc_time 			cycle_time_par,
			  sc_time 			guard_time_par = SC_ZERO_TIME,
			  sc_module_name 	name = sc_gen_unique_name("tdma_bus"));

	// MANDATORY ATTRIBUTES			
	void set_n_channels(unsigned int n_channels_par); // number of channels or time slots
	unsigned int get_n_channels();

	void set_slot_time(sc_time slot_time_par);  // duration of slot
	sc_time &get_slot_time();

	// REST OF CONFIGURATION:
	// channel/slot bandwidth is defined once the slot time is defined plus
	// any of the two following alternatives:
	
		// (a) payload bits transmitted in the slot/channel within the slot time
	void set_channel_payload_bits(unsigned int ch_payload_bits_par);	// payload bits
	unsigned int &get_channel_payload_bits();
		// ... alternative (a) abstracts from the actual bit tx capacity of the TDMA bus

		// (b) total number of bits transmitted in the slot/channel within the slot time...
	void set_channel_capacity(unsigned int channel_capacity_par); // bits
	unsigned int get_channel_capacity();
		//     ... plus the payload rate, which has to be provided to enable the calculation
		//     of the effective bandwidth of the slot, that is the actual user information
		//     that can be transferred within one slot
	void set_channel_payload_rate(double ch_payload_rate_par);	// payload rate	
	double &get_channel_payload_rate();
	
		// guard times between adjacent channels
	void set_guard_time(sc_time guard_time);
	sc_time &get_guard_time();

		// Cycle of the TDMA bus
						// Notice that can be longer to cope with non static channels
						// As happens in Flexray, TT-Ethernet, etc
						// (This model currently does not support dynamic chanels in the bus)
	void set_cycle_time(sc_time period_par);
	sc_time &get_cycle_time();

	unsigned int &get_channel_BW(); // get bandwidth of each static channel
	unsigned int &get_channel_eBW(); // get effective bandwidth of each static channel
	unsigned int &get_eBW(); // get effective bandwidth of the bus
	
	void set_tx_delay(sc_time tx_d_par); // Refers to the transmission delay
	sc_time &get_tx_delay(); 

	// configuration of initial state
	void    set_initial_offset_time(sc_time par); // enables to start the simulation assuming a specific initial state of the bus
	sc_time  &get_initial_offset_time();
	
	void    set_seed(unsigned int seed); // enables to set the initial seed to reproduce deterministacally any part of the
	                                       // the model which relies on randomization. For the KisTA TDMA model it happens
	                                       // when the simulation uses current delays (no max.) and the L1 currrent_offset calculation 
	                                       // is used.

	// Connects a procesing element, returns a single identifier (net_address) associated to the element in the network
// directly inherited from gen_network, in principle, no need to override it
//	net_address plug(processing_element *PE); // on default, assumed that any task of PE has full potential for accessing all the TDMA channels

//	net_address plug(processing_element *PE, unsigned int n_channels_limit); // stablish a limit in the number that can be 
																				// assigned to a PE, at the time it is plug
																				// Moreover, the limit has to be coherent with 
																				// the capacity of the bus and other connections
	
	// Specific allocation methods for slot allocation in a TDMA bus
	//    Allocation is performed in a transmitter processor basis (since the information transmitted
	//    within a slot is broadcasted to the rest of processors,which can read, as it is a bus)
	//
	//  These methods check that the BW allocated (or equivalently, slots) is not higher than the BW limit of the TDMA bus
	//
	void allocate_channels(phy_address src, unsigned int required_channels); 
	void allocate_channels(phy_address src, unsigned int required_channels, unsigned int first_slot);  // enables manual assignation of the user
		// raises a KisTA error if the number of slots cannot be allocated
	unsigned int allocate_BW(phy_address src, unsigned int required_bps); // (onyl available with automatic allocation)
		// returns the amount of allocated BW according the number of allocated slots and bus configuration, raises KisTA error if the BW cannot be allocated
	
//  redefinition for generic network
//	unsigned int allocate_BW(net_address src, net_address src, unsigned int required_bps); 
	
				// reports error if no sufficient BW can be allocated,
				// otherwise, the call succeedes and returns the actual BW allocated if it is the same or more.
	            // Take into account that some specific channels make a discrete parition of the
	            // BW which can mean that more BW than strictly required is allocated, as
	             // it can be the case of the TDMA buses.
	
	// Overload of setters independent on the specific value
	using phy_comm_res_t::set_MaxP2Pdelay; // this is required to enable the use of the non-overloaded methods with the same function name
	
	void set_MaxP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size=1);
	void set_MaxP2Pdelay(phy_link_t &link, unsigned int msg_size=1);

	// redefinitions of inherited methods	
	sc_time &get_MaxP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size=1);
	sc_time &get_MaxP2Pdelay(phy_link_t &link, unsigned int msg_size=1);
	
	// Overload of setters independent on the specific value
	using phy_comm_res_t::set_CurrentP2Pdelay; // this is required to enable the use of the non-overloaded methods with the same function name
	
	void set_CurrentP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size=1);
	void set_CurrentP2Pdelay(phy_link_t &link, unsigned int msg_size=1);

	// redefinitions of inherited methods	
	sc_time &get_CurrentP2Pdelay(phy_address src, phy_address dest, unsigned int msg_size=1);
	sc_time &get_CurrentP2Pdelay(phy_link_t &link, unsigned int msg_size=1);
	

	void report_bus_configuration();
	
	// CONTROL OF THE ACCURACY OF THE MODEL
	// 	Currently supported levels:
	//			0: bit-level accurate model for the bound on the MAX P2P delay 
	//			1: slot-level accurate model for the bound on the MAX P2P delay
	//			2: TDMA cycle level accurate model for the bound on the MAX P2P delay
	// (*) see notes below in this file
	//
	void 			  set_accuracy_level(accuracy_level_t level);
	accuracy_level_t& get_accuracy_level();

	// Models for different levels of accuracy
	//  These functions are made public to enable the experimentation of the
	//  the trade offs of the accuracy and simulation speed for the different levels
	
	// common
	sc_time 	max_offset_delay(unsigned int slots_allocated); // time invariant
	
	// for CURRENT P2P transmission delays
	
	//  offsets
	sc_time     current_offset_delay_L0(sc_time current_time, unsigned int first_slot); // time variant
	sc_time     current_offset_delay_L1(unsigned int slots_allocated);                   // random value between [0,max_offset_delay(slots_allocated)]
	
	// transmission delays without offsets
	sc_time     calculate_data_tx_delay_L0(unsigned int msg_size, unsigned int slots_allocated); // bit-accurate (time invariant)
	sc_time     calculate_data_tx_delay_L1(unsigned int msg_size, unsigned int slots_allocated); // slot accurate (time invariant) NOT USED CURRENTLY
	  // For the TDMA bus, the data tx delay depends only on the message size and the slots allocated, 
	  // and not on the current state of the bus
	
//	sc_time     calculate_data_tx_delay(unsigned int msg_size, unsigned int slots_allocated);
		
	//  Fpr MAX P2P delays
	sc_time     calculate_MaxP2Pdelay_L0(unsigned int msg_size, unsigned int slots_allocated); // bit-accurate bound (time invariant)
	sc_time     calculate_MaxP2Pdelay_L1(unsigned int msg_size, unsigned int slots_allocated); // slot-accurate bound (time invariant)
	sc_time     calculate_MaxP2Pdelay_L2(unsigned int msg_size, unsigned int slots_allocated); // TDMA cycle accurate bound (time invariant)
	
	// Max P2P delay as a function of the message size, allocated tx slots (per TDMA cycle),
	// and the remaining parameters of the bus assumes slots _allocated>0
	sc_time     calculate_MaxP2Pdelay(unsigned int msg_size, unsigned int slots_allocated);	

private:
	unsigned int n_channels; // Number of available slots
	sc_time slot_time; // directly related to BW
	
	unsigned int channel_capacity; // bits / slot
	sc_time guard_time; // optional, considerer per channel
	sc_time cycle_time; // cycle of the TDMA bus:
						// Notice that can be longer to cope with non static channels
						// As happens in Flexray, TT-Ethernet, etc
	
	unsigned int 	ch_payload_bits; // bits
	double		 	ch_payload_rate; // rate
	
	unsigned int channel_BW;
	unsigned int channel_eBW;
	unsigned int eBW; // Effective BW of the bus (different from the TX Line speed in general!, considers cycle time, and payloads)
		
	sc_time initial_offset_time; // enables to start the simulation assuming a specific initial state of the bus

	sc_time tx_delay;
	sc_time current_tx_delay;
	
	unsigned int n_allocated_channels;
	
	accuracy_level_t accuracy_level;
	
	void init_params();
		
	// helper functions


	// calculate current delay as a function of the message size, allocated tx slots (per TDMA cycle), 
	// the remaining parameters of the bus, and the current state (phase) of the TDMA bus
//	sc_time     calculate_P2Pdelay(unsigned int msg_size);
	
	// calculate Max P2P delay for link for msg size and settle it into plink_info
	void calculate_and_set_static_MaxP2Pdelay(phy_link_t &link,	unsigned int msg_size);
	
		// a register of the # of allocated channels for each PE_i for transmission TX
		// (which it is equivalent to say for each p2p net connection from PE_i to any remainder PE_j. such i/=j)
	channel_allocation_table_t		channel_allocation_table;
	    // is used in synchronization with a table with the first allocated slot for each PE...
	first_slot_allocation_table_t	first_slot_allocation_table;
		// ... and a list of free slots, to internally handle the slot allocation 
		// (allowing safe manual/external allocation, and a commbination
		// of manual/external allocation with automated allocation)
		//
	free_slot_allocation_table_t	free_slot_table; // stores a table with the free slot intervals, e.g.
															 // (3,7) (9,10)
															 // At the beginning the table contains only one entry (0,n_channels-1)
															 // The bigger the amount of entries, the more fragmented the table
	
	unsigned int assign_free_slots(unsigned int req_slots); // Looks for req_slots slots and provides the first slot position
	                                                           // if sucessful, or raises an error otherwise

	void assign_free_slots(unsigned int req_slots,unsigned int first_slot); // For allocating a specific (and contiguous) set of slots
	                                                                            // It raises an error if there are busy slots in that region
	
	bool manual_slot_allocation;	// flag enabled if there is at least one manual slot assignation
	bool check_slot_allocation();	// checks that there are not overlaps in the slot allocation, e.g. [1,2][2,3] is an overlap in the slot 2
	                                // (the check does not report error if there are unsused slots)
	bool print_slot_allocation();
		
	void before_end_of_elaboration();	
	//void end_of_elaboration();

};

} // namespace kista

#endif

	//
	// There are two levels of accuracy for the bound on max p2p 
	//
	//		Level 0:
	//
	// maximum delay is given by
	//
	//   - the time that it is required to wait till reaching the first allocated slot
	//
	//			min. offset_delay: 0  (best case)
	//			max. offset_delay: period - (n_allocated_channels -1) (time_slot+guard_time) (worst case)
	//
	//		NOTE: a more precise model can calculate this by calculating the precise offset of the appication running on the PE
	//            on the sime time basis over the TDMA bus clock
	//
	//    - assuming that assigned slots are contiguous, the delay corresponding to 
	//      the numberof slots required, so
	//			slots_required = ceil ( size_of_message / (slots_allocated-1)*slot_capacity) )
	//			complete periods_required = slots_required / slots_allocated (notice that a truncation is assumes here)
	//			tail_slots = remainder(slots_required/slots_allocate)
	//			data_tx_delay = complete_periods_required*tdma_period + remainder_slots*slot_time
	//
	//		Therefore the total transmission time is:
	//			total_tx_time = offset_delay + data_tx_delay
	//			
	//
	//		in the worst case
	//			max_total_tx_time = max_offset_delay + data_tx_delay (it is actually max_tx_delay whenever we take WCET)
	//
	//          Notice that his bound is valid for the general case, where it can be a "shift" alfa, reflecting the current
	//          state or phase of the system regarding the time axis. Then, it fulfils:
	//             
	//				max_total_tx_time = (max_offset_delay -alpha) + complete_periods_required*tdma_period + alpha remainder_slots*slot_time
	//       
	//			which yields in the same formula
	//
	//
	//		Level 1:
	//
	// maximum delay is given by
	//   - the time that it is required to wait till reaching the first allocated slot
	//
	//			min. offset_delay: 0  (best case)
	//			max. offset_delay: period - (n_allocated_channels-1) (time_slot+guard_time) (worst case)
	//
	//		NOTE: a more precise model can calculate this by calculating the precise offset of the appication running on the PE
	//            on the sime time basis over the TDMA bus clock
	//
	//    - assuming that assigned slots are contiguous, the delay corresponding to 
	//      the numberof slots required, so
	//			slots_required = ceil ( size_of_message / (slots_allocated-1)*slot_capacity) )
	//			complete periods_required = slots_required / slots_allocated (notice that a truncation is assumes here)
	//			tail_slots = remainder(slots_required/slots_allocate)
	//			data_tx_delay = complete_periods_required*tdma_period + remainder_slots*slot_time
	//
	//		Therefore the total transmission time is:
	//			total_tx_time = offset_delay + data_tx_delay
	//			
	//
	//		in the worst case
	//			max_total_tx_time = max_offset_delay + data_tx_delay (it is actually max_tx_delay whenever we take WCET)
	//
	//          Notice that his bound is valid for the general case, where it can be a "shift" alfa, reflecting the current
	//          state or phase of the system regarding the time axis. Then, it fulfils:
	//             
	//				max_total_tx_time = (max_offset_delay -alpha) + complete_periods_required*tdma_period + alpha remainder_slots*slot_time
	//       
	//			which yields in the same formula
	//
	//		Level 2: 
	//				periods_required = ceil ( (float)slots_required / (float)slots_allocated )
	//				max_total_tx_time = (complete_periods_required+1) *tdma_period
	//				Note: the "+1" is a rough bound to consider the initial offset
	//
