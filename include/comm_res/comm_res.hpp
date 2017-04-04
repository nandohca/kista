/*****************************************************************************

  comm_res.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 August

//
// generic p2p network
// It enables a GENERIC and ABSTRACT model of a PLATFORM COMMUNICATION RESOURCE
// It is conceived as a generic p2p network.
// In deed, in KisTA, the comm_res class is aliased by the gen_network too.
// The services of such a generic communication resource (or generic network)
// depend on the state of the communication resource / network (state-dependent)
// and thus time variant in the most general case,
//
//  On top of this model, more specific communication infrastructures, like buses and specific network types can be modelled
//  in an easier way by inheriting generic characteristics
//
//  It has to consider that
//    - switched networks cannot always keep a link, however, usually its QoS parameters are guaranteed
//    - packet networks let several routings, and QoS can be more fragmented
//
// Being time variant means that some network models will vary the actual response
// of the network (current delays) 
//
//  If the model is set as time invariant the generic network configuration methods cannot being
//  dynamically changed. This is checked by the comm_res class
//
//  Moreover, the comm_res class also checks that the amount of elements connected to the network
//  is not exceeded
//
 *****************************************************************************/

#ifndef COMMUNICATION_RESOURCE_HPP
#define COMMUNICATION_RESOURCE_HPP

#include <systemc.h>

#include "defaults.hpp"

#include <unordered_map>

#include "link_t.hpp"
#include "link_info_t.hpp"
#include "mapped_links_t.hpp"
#include "comm_res/phy_link_t.hpp"

namespace kista {

//
// communication resource template
//    Being a template enables to use the communication resource both, at a phisical level (net) or at the logic level (logic link)
//
template<class address_type>
class comm_res:public sc_module {
public:

	typedef std::vector<address_type>	connected_elements_t; // Either tasks or PEs (depending of the address type)

	// The pointer it can be either link_info_t (directly the link information) or a map to link information
	// ordered by the message size, as follows
	typedef std::map<unsigned int, link_info_t*>		link_info_by_msg_size_t;
	typedef link_info_by_msg_size_t::iterator			linfo_msize_it_t;
	
	//typedef std::unordered_map<link_t<address_type>, link_info_t>	links_table_t;
	typedef std::unordered_map<link_t<address_type>, link_info_by_msg_size_t*>	links_table_t;

//
// Notice that for the communication resource, an unordered map can be used for the links table
// That means that we do not assume a specific order of the links either by the source and/or destination addresses and/or names
//

	// constructor
	comm_res(sc_module_name name = sc_gen_unique_name("comm_res"));
	
	// global limits for the whole network
	// limitation for the total bandwidth (<0=unlimited)
	void set_BW_limit(double BW_limit);  //bps
	double &get_BW_limit();
	
	// limitation for the number of the elements of the network (0=unlimited)
	void set_connections_limit(unsigned int conn_limit_par);				
	unsigned int get_connections_limit(unsigned int BW_limit_par);	//
	
	// Connects an element with address (e.g. processor or task),
	// and returns the same single identifier (net_address) associated to the element in the network
	address_type plug(address_type element_address);
	
	// This class enables to add a link (without properties) and then stablish its properties
	// So far, it has to be done in a separated manner:

	// For creating the logical link object without stating its properties
	// This is useful when the properties will likely change with each call (time variant)
	// This methods enable to state at elaboration time which physical links will be
	// required by the application, regardless if it is possible to state 
	// the properties associated to them
	void add_link(address_type src, address_type dest);
	void add_link(link_t<address_type> link);
	
	// METHODS FOR SETTING AND GETTING LINK PROPERTIES	
	
	// methods to state (modify) link characteristics (either statically or dynamically)
	// (this methods enforce to set all the properties for a link at once!!)
	void set_properties(address_type src, address_type dest, link_info_t *link_info, unsigned int msg_size=1);
	void set_properties(link_t<address_type> &link, link_info_t *link_info, unsigned int msg_size=1);

	// methods to state link characteristics (either statically or dynamically)
	// (These methods let either extract all the properties at once, individually)
	link_info_t *get_properties(address_type src, address_type dest, unsigned int msg_size=1);
	link_info_t *get_properties(link_t<address_type> &link, unsigned int msg_size=1);
	
	// This generic interconnecting model assumes the general dependency in many communication systems of
	// latencies and effective bandwidths with message size
	
	// setters for specific properties
	virtual void set_MinBW(address_type src, address_type dest, double bw, unsigned int msg_size=1); // in bps
	virtual void set_MinBW(link_t<address_type> &link, double bw, unsigned int msg_size=1);

	virtual void set_CurrentBW(address_type src, address_type dest, double bw, unsigned int msg_size=1); // in bps
	virtual void set_CurrentBW(link_t<address_type> &link, double bw, unsigned int msg_size=1);
	
	virtual void set_AvgBW(address_type src, address_type dest, double bw, unsigned int msg_size=1); // in bps
	virtual void set_AvgBW(link_t<address_type> &link, double bw, unsigned int msg_size=1);
		
	virtual void set_MaxP2Pdelay(address_type src, address_type dest, sc_time delay,  unsigned int msg_size=1);  // in s
	virtual void set_MaxP2Pdelay(link_t<address_type> &link, sc_time delay, unsigned int msg_size=1);

	virtual void set_CurrentP2Pdelay(address_type src, address_type dest, sc_time delay, unsigned int msg_size=1);  // in s
	virtual void set_CurrentP2Pdelay(link_t<address_type> &link, sc_time delay, unsigned int msg_size=1);	
	
	virtual void set_AvgP2Pdelay(address_type src, address_type dest, sc_time delay, unsigned int msg_size=1);  // in s
	virtual void set_AvgP2Pdelay(link_t<address_type> &link, sc_time delay, unsigned int msg_size=1);

	//      note: set_methods which are independent from the actual value calculated, and only depend in the actual resource
	//            from a specific calculation
	//
	virtual void set_MinBW(address_type src, address_type dest, unsigned int msg_size=1) {}; // in bps
	virtual void set_MinBW(link_t<address_type> &link, unsigned int msg_size=1) {};

	virtual void set_CurrentBW(address_type src, address_type dest, unsigned int msg_size=1) {}; // in bps
	virtual void set_CurrentBW(link_t<address_type> &link, unsigned int msg_size=1) {};
	
	virtual void set_AvgBW(address_type src, address_type dest, unsigned int msg_size=1) {}; // in bps
	virtual void set_AvgBW(link_t<address_type> &link, unsigned int msg_size=1) {};
		
	virtual void set_MaxP2Pdelay(address_type src, address_type dest, unsigned int msg_size=1) {};  // in s
	virtual void set_MaxP2Pdelay(link_t<address_type> &link, unsigned int msg_size=1) {};

	virtual void set_CurrentP2Pdelay(address_type src, address_type dest, unsigned int msg_size=1) {};  // in s
	virtual void set_CurrentP2Pdelay(link_t<address_type> &link, unsigned int msg_size=1) {};	
	
	virtual void set_AvgP2Pdelay(address_type src, address_type dest, unsigned int msg_size=1) {};  // in s
	virtual void set_AvgP2Pdelay(link_t<address_type> &link, unsigned int msg_size=1) {};
	
	// getter for specific properties
	
	virtual double &get_MinBW(address_type src, address_type dest, unsigned int msg_size=1); // in bps
	virtual double &get_MinBW(link_t<address_type> &link, unsigned int msg_size=1);

	virtual double &get_CurrentBW(address_type src, address_type dest, unsigned int msg_size=1); // in bps
	virtual double &get_CurrentBW(link_t<address_type> &link, unsigned int msg_size=1);
	
	virtual double &get_AvgBW(address_type src, address_type dest, unsigned int msg_size=1); // in bps
	virtual double &get_AvgBW(link_t<address_type> &link, unsigned int msg_size=1);
		
	virtual sc_time &get_MaxP2Pdelay(address_type src, address_type dest, unsigned int msg_size=1);  // in s
	virtual sc_time &get_MaxP2Pdelay(link_t<address_type> &link, unsigned int msg_size=1);

	virtual sc_time &get_CurrentP2Pdelay(address_type src, address_type dest, unsigned int msg_size=1);  // in s
	virtual sc_time &get_CurrentP2Pdelay(link_t<address_type> &link, unsigned int msg_size=1);	
	
	virtual sc_time &get_AvgP2Pdelay(address_type src, address_type dest, unsigned int msg_size=1);  // in s
	virtual sc_time &get_AvgP2Pdelay(link_t<address_type> &link, unsigned int msg_size=1);
	
	void set_time_invariant(bool flag = true); // the communication resource is time invariant
	bool& is_time_invariant();
		
	// The set_time_invariant_connections method involves that the comm_res will allow the connection of elements only statically
	// The comm_res class will check that there dynamic plugs are not done
	void set_time_invariant_dimension(bool flag = true);
	bool& is_time_invariant_dimension();
	unsigned int get_number_of_connected_elements(); // returns the number of elements connected
				
	// The set_time_invariant_connections method states and ensures the modelling of a communication resource where the
	// connection attributes are time invariant
	void set_connections_time_invariant(bool flag = true);
	bool &are_connections_time_invariant();
		
	// When enabled, it prevents that the derived model varies network link characteristics during time
	// (properties are not allowed to be changed through set metods at the simulation phase, so
	//  when this is settled, the comm_res class ensures that the following methods c
	//	 - constructor (no creation of the network at simulation time or later
	//	 - generic network configuration methods ; set BW, set connection limit
	//   - methods for setting link properties
	//			can be called only before the start of simulation (elaboration time)
	//
	// 		Please, notice that a packet switched network is time variant in this sense even if the
	// 		network parameters are fixed. Time invariance in this context refer to the time variance of
	// 		P2P links characteristics, which have involvements in the PE to PE communications and so in its predictability
	// 		By default, a comm_res object is time_variant

	// The set_message_size_invariant_connections method states and ensures the modelling of a communication resource where
	// the connection properties do not depend on the message size
	// (the simulation resources adapt to that situation to make the simulation resources more efficient)
	void set_connections_message_size_invariant(bool flag = true);
	bool &are_connections_message_size_invariant();
	
	// printing functions
	void print_connected_elements();
	void print_links();
	void print_links_table();
	
private:
    // getter for the total amount of tasks
	double BW_limit;
	unsigned int connections_limit;
	
	bool time_invariant;
	// unless the communication resource is time invariant (the most typical case)
	//   then specific attributes can be configured as time invariant
	bool dimension_time_invariant;		// dimension of the network (links) is fixed
	bool connections_time_invariant;	// connection attributes are time invariant
	
	bool connections_message_size_invariant; // this is to stablis message size variance with message size
	
	void check_time_invariance(const char *configured_feature);
	
	//virtual void before_end_of_elaboration();
	//void end_of_elaboration();
	
protected:	
	connected_elements_t		connected_elements; // Either tasks or PEs (depending of the address type)
	links_table_t				links_table;
};


// **************************************************************************************
//
// comm_res IMPLEMENTATION
//   (notice that, since comm_res is a template, it cannot be moved to the .cpp file
//
// **************************************************************************************

// constructor
template<class address_type>
comm_res<address_type>::comm_res(sc_module_name name) : sc_module(name) {
	if ( sc_get_status()&(SC_ELABORATION|SC_BEFORE_END_OF_ELABORATION|SC_END_OF_ELABORATION) ) {
		BW_limit=-1.0;
		connections_limit = 0;
		time_invariant = false;
		dimension_time_invariant = true;
		connections_time_invariant = false;	 // if message size invariant, connection time invariant
		connections_message_size_invariant=true;
	} else {
		SC_REPORT_ERROR("KisTA","A generic platform communication resource (generic network) can be created only at the elaboration ");
	}
}

template<class address_type>
void comm_res<address_type>::set_time_invariant(bool flag) {
	time_invariant=flag;
}

template<class address_type>
bool &comm_res<address_type>::is_time_invariant() {
	return time_invariant;
}

template<class address_type>
void comm_res<address_type>::set_time_invariant_dimension(bool flag) {
	dimension_time_invariant = flag;
}

template<class address_type>
bool &comm_res<address_type>::is_time_invariant_dimension() {
	return dimension_time_invariant;
}


template<class address_type>
void comm_res<address_type>::set_connections_time_invariant(bool flag) {
	connections_time_invariant=flag;
}

template<class address_type>
bool& comm_res<address_type>::are_connections_time_invariant() {
	return connections_time_invariant;
}


template<class address_type>
void comm_res<address_type>::set_connections_message_size_invariant(bool flag) {
	connections_message_size_invariant = flag;
}

template<class address_type>
bool& comm_res<address_type>::are_connections_message_size_invariant() {
	return connections_message_size_invariant;
}

template<class address_type>
void comm_res<address_type>::check_time_invariance(const char *configured_feature) {
	std::string  msg_rpt;
	if (sc_get_status()&(SC_START_OF_SIMULATION|SC_RUNNING|SC_PAUSED|SC_STOPPED|SC_END_OF_SIMULATION) ) {
		msg_rpt = "Trying to dynamically set ";
		msg_rpt += configured_feature;
		msg_rpt += " in communication resource ";
		msg_rpt += name();
		msg_rpt += ", which is time invariant";
		SC_REPORT_ERROR("kisTA",msg_rpt.c_str());
	}
}

template<class address_type>
void comm_res<address_type>::set_BW_limit(double BW_limit_par) {  //bps
	if (time_invariant) check_time_invariance("BW limit");	
	BW_limit = BW_limit;
}

template<class address_type>
double &comm_res<address_type>::get_BW_limit() {	
	return BW_limit;
}
	
// limitation for the number of the elements of the network (0=unlimited)
template<class address_type>
void comm_res<address_type>::set_connections_limit(unsigned int conn_limit_par) {
	if (time_invariant) check_time_invariance("connections limit");		
	connections_limit = conn_limit_par;
}			

template<class address_type>
unsigned int comm_res<address_type>::get_connections_limit(unsigned int BW_limit) {
	return connections_limit;
}

template<class address_type>
address_type comm_res<address_type>::plug(address_type element_address) {
	std::string rpt_msg;
		
	if(time_invariant || dimension_time_invariant) {
		rpt_msg = "a new connection of the element ";
		rpt_msg += element_address->name();
		check_time_invariance(rpt_msg.c_str());
	}
	connected_elements.push_back(element_address);
	return element_address;
}	

template<class address_type>
void comm_res<address_type>::add_link(address_type src, address_type dest) {
	link_t<address_type> link;
	link.src=src;
	link.dest=dest;
	links_table[link]=NULL;
}

template<class address_type>
void comm_res<address_type>::add_link(link_t<address_type> link){
	links_table[link]=NULL;
}
	
// methods to state link characteristics (either statically or dynamically)
// (this methods enforce to set all the properties)
template<class address_type>
void comm_res<address_type>::set_properties(link_t<address_type> &link, link_info_t *link_info, unsigned int msg_size) {
	typename links_table_t::iterator ltable_it;
	
	link_info_by_msg_size_t	*link_info_by_msize_p;
	linfo_msize_it_t		linfo_msize_it_t;
		
	if (this->are_connections_time_invariant()) check_time_invariance("set link properties");
		
	//cout << "Setting Properties (BEFORE):" << endl;
	//print_links_table();

	// first tries to find the link
	ltable_it = links_table.find(link);
		
	if(ltable_it == links_table.end()) { 
		// link not found, 
		// creates the map with link info...
		link_info_by_msize_p = new link_info_by_msg_size_t;
		// ... filling it with the first entry ...
		//link_info_by_msize_p->insert(msg_size,link_info);
		(*link_info_by_msize_p)[msg_size]=link_info;
		// ... and passes it to the links table
		//links_table->insert(std::make_pair<link,link_info_by_msize_p>);
		links_table[link]=link_info_by_msize_p;
	} else {
		// link found
		// link_info_by_msize_p = ltable_it->second;
		// ... then, checks if it contains any link_info by message size table entry
		if(ltable_it->second==NULL) {
			// there was a link, but without associated link info entries, 
			// a new one has to ve created anyhow
			// creates the map with link info...
			link_info_by_msize_p = new link_info_by_msg_size_t;
			// ... filling it with the first entry ...
			//link_info_by_msize_p->insert(msg_size,link_info);
			(*link_info_by_msize_p)[msg_size]=link_info;		
			ltable_it->second = link_info_by_msize_p;
		} else {
			//ltable_it.second->insert(msg_size,link_info);
			(*(ltable_it->second))[msg_size]=link_info;
			// inserting a second entry not allowed if connnections are message size invariant
			if(are_connections_message_size_invariant()) {
				if(ltable_it->second->size()>1) {
					std::string msg;
					std::ostringstream os;					
					msg= "Setting  properties of link ";
					os << link;
					msg += os.str();					
					msg+= " for at least two message sizes, last ";
					msg+= std::to_string(msg_size);
					msg+= " bits, and first  ";
					msg+= std::to_string(ltable_it->second->begin()->first);
					msg+= " bits, for communication resource  ";
					msg+= this->name();
					msg+= ", whose connections have been stated as message size invariant.";
					SC_REPORT_ERROR("KisTA",msg.c_str());
				}
			}		
		}
	}
	
	//cout << "Setting Properties (AFTER):" << endl;
	//print_links_table()	;
	
}

template<class address_type>
void comm_res<address_type>::set_properties(address_type src, address_type dest, link_info_t *link_info, unsigned int msg_size) {
	link_t<address_type> link;	
	link.src= src;
	link.dest = dest;
	this->set_properties(link,link_info,msg_size);
}	

// methods to get link characteristics (either statically or dynamically)
// (These methods let  extract all the properties at once)
template<class address_type>
link_info_t *comm_res<address_type>::get_properties(link_t<address_type> &link, unsigned int msg_size){
	
	std::string msg;
	std::ostringstream os;
	typename links_table_t::iterator it;
	linfo_msize_it_t	linfo_it;

	//cout << "Getting properties:" << endl;

	//print_links_table();
	//cout << "Searching LINK:" << link << endl;
	
	it = links_table.find(link);
	if(it!=links_table.end()) { 
		if(it->second != NULL) {
			linfo_it = it->second->find(msg_size); ;
			if (linfo_it != it->second->end()) {
				return linfo_it->second;
			}
			else {
#ifdef _WARN_LINKS_FOUND_WITHOUT_ASSOCIATED_INFORMATION				
				msg = "Physical Link ";
				os << link;
				msg += os.str();
				msg += " found in communication resource ";
				msg += this->name();
				msg += ", but not associated link information for the size.";
				SC_REPORT_WARNING("KisTA",msg.c_str());
#endif				
				return NULL;
			}			
		}
		else {
#ifdef _WARN_LINKS_FOUND_WITHOUT_ASSOCIATED_INFORMATION					
			msg = "Physical Link ";
			os << link;
			msg += os.str();
			msg += " found in communication resource ";
			msg += this->name();
			msg += ", but not associated link information was found.";
			SC_REPORT_WARNING("KisTA",msg.c_str());
#endif			
			return NULL;
		}
	} else {
		msg = "Physical Link ";
		os << link;
		msg += os.str(),
		msg += " not found in communication resource ";
		msg += this->name();
		SC_REPORT_ERROR("KisTA",msg.c_str());
		return NULL; // never reached, just to eliminate warnings
	}
}

template<class address_type>
link_info_t *comm_res<address_type>::get_properties(address_type src, address_type dest, unsigned int msg_size) {
	link_t<address_type> link;
	link.src= src;
	link.dest = dest;
	return get_properties(link,msg_size);
}



// specific setters implementation
template<class address_type>
void comm_res<address_type>::set_MinBW(address_type src, address_type dest, double bw, unsigned int msg_size) { // in bps
	link_t<address_type> link;
	link.src=src;
	link.dest = dest;
	this->set_MinBW(link,bw,msg_size);
}

template<class address_type>
void comm_res<address_type>::set_MinBW(link_t<address_type> &link, double bw, unsigned int msg_size) {
	link_info_t *link_info_p;
	link_info_p = get_properties(link,msg_size);
	if(link_info_p==NULL) { // no link or no link info available for that msg_size, so it has to be settled again
		link_info_p = new link_info_t;

	}
	link_info_p->setMinBW(bw);
	set_properties(link,link_info_p,msg_size);
}

template<class address_type>
void comm_res<address_type>::set_CurrentBW(address_type src, address_type dest, double bw, unsigned int msg_size) { // in bps
	link_t<address_type> link;
	link.src=src;
	link.dest = dest;
	this->set_CurrentBW(link,bw,msg_size);	
}
template<class address_type>
void comm_res<address_type>::set_CurrentBW(link_t<address_type> &link, double bw, unsigned int msg_size) {
	link_info_t *link_info_p;
	link_info_p = get_properties(link,msg_size);
	if(link_info_p==NULL) { // no link or no link info available for that msg_size, so it has to be settled again
		link_info_p = new link_info_t;

	}
	link_info_p->setCurrentBW(bw);
	set_properties(link,link_info_p,msg_size);
}

template<class address_type>	
void comm_res<address_type>::set_AvgBW(address_type src, address_type dest, double bw, unsigned int msg_size) { // in bps
	link_t<address_type> link;
	link.src=src;
	link.dest = dest;
	this->set_AvgBW(link,bw,msg_size);
}

template<class address_type>
void comm_res<address_type>::set_AvgBW(link_t<address_type> &link, double bw, unsigned int msg_size) {
	link_info_t *link_info_p;
	link_info_p = get_properties(link,msg_size);
	if(link_info_p==NULL) { // no link or no link info available for that msg_size, so it has to be settled again
		link_info_p = new link_info_t;

	}
	link_info_p->setAvgBW(bw);
	set_properties(link,link_info_p,msg_size);
}
		
template<class address_type>		
void comm_res<address_type>::set_MaxP2Pdelay(address_type src, address_type dest, sc_time delay,  unsigned int msg_size) { // in s
	link_t<address_type> link;
	link.src=src;
	link.dest = dest;
	this->set_MaxP2Pdelay(link,delay,msg_size);
}

template<class address_type>
void comm_res<address_type>::set_MaxP2Pdelay(link_t<address_type> &link, sc_time delay, unsigned int msg_size) {
	link_info_t *link_info_p;
	link_info_p = get_properties(link,msg_size);
	if(link_info_p==NULL) { // no link or no link info available for that msg_size, so it has to be settled again
		link_info_p = new link_info_t;

	}
	link_info_p->setMaxP2Pdelay(delay);
	set_properties(link,link_info_p,msg_size);
}

template<class address_type>
void comm_res<address_type>::set_CurrentP2Pdelay(address_type src, address_type dest, sc_time delay, unsigned int msg_size) {  // in s
	link_t<address_type> link;
	link.src=src;
	link.dest = dest;
	this->set_CurrentP2Pdelay(link,delay,msg_size);
}

template<class address_type>
void comm_res<address_type>::set_CurrentP2Pdelay(link_t<address_type> &link, sc_time delay, unsigned int msg_size) {
	link_info_t *link_info_p;
	link_info_p = get_properties(link,msg_size);
	if(link_info_p==NULL) { // no link or no link info available for that msg_size, so it has to be settled again
		link_info_p = new link_info_t;

	}
	link_info_p->setCurrentP2Pdelay(delay);
	set_properties(link,link_info_p,msg_size);
}

template<class address_type>	
void comm_res<address_type>::set_AvgP2Pdelay(address_type src, address_type dest, sc_time delay, unsigned int msg_size) {  // in s
	link_t<address_type> link;
	link.src=src;
	link.dest = dest;
	this->set_AvgP2Pdelay(link,delay,msg_size);
}

template<class address_type>
void comm_res<address_type>::set_AvgP2Pdelay(link_t<address_type> &link, sc_time delay, unsigned int msg_size) {
	link_info_t *link_info_p;
	link_info_p = get_properties(link,msg_size);
	if(link_info_p==NULL) { // no link or no link info available for that msg_size, so it has to be settled again
		link_info_p = new link_info_t;

	}
	link_info_p->setAvgP2Pdelay(delay);
	set_properties(link,link_info_p,msg_size);
}

// specific getters implementation

template<class address_type>
double &comm_res<address_type>::get_MinBW(address_type src, address_type dest, unsigned int msg_size) { // in bps
	return get_properties(src,dest)->getMinBW();
}

template<class address_type>
double &comm_res<address_type>::get_MinBW(link_t<address_type> &link, unsigned int msg_size) {
	return get_properties(link)->getMinBW();
}

template<class address_type>
double &comm_res<address_type>::get_CurrentBW(address_type src, address_type dest, unsigned int msg_size) { // in bps
	return get_properties(src,dest)->getCurrentBW();
}

template<class address_type>
double &comm_res<address_type>::get_CurrentBW(link_t<address_type> &link, unsigned int msg_size) {
	return get_properties(link)->getCurrentBW();
}

template<class address_type>
double &comm_res<address_type>::get_AvgBW(address_type src, address_type dest, unsigned int msg_size) { // in bps
	return get_properties(src,dest)->getAvgBW();
}

template<class address_type>
double &comm_res<address_type>::get_AvgBW(link_t<address_type> &link, unsigned int msg_size) {
	return get_properties(link)->getAvgBW();
}

template<class address_type>
sc_time &comm_res<address_type>::get_MaxP2Pdelay(address_type src, address_type dest, unsigned int msg_size) { // in s
	return get_properties(src,dest)->getMaxP2Pdelay(msg_size);
}

template<class address_type>
sc_time &comm_res<address_type>::get_MaxP2Pdelay(link_t<address_type> &link, unsigned int msg_size) {
	return get_properties(link)->getMaxP2Pdelay(msg_size);
}		

template<class address_type>
sc_time &comm_res<address_type>::get_CurrentP2Pdelay(address_type src, address_type dest, unsigned int msg_size) {  // in s
	return get_properties(src,dest)->getCurrentP2Pdelay(msg_size);
}

template<class address_type>
sc_time &comm_res<address_type>::get_CurrentP2Pdelay(link_t<address_type> &link, unsigned int msg_size) {
	return get_properties(link)->getCurrentP2Pdelay(msg_size);
}	

template<class address_type>
sc_time &comm_res<address_type>::get_AvgP2Pdelay(address_type src, address_type dest, unsigned int msg_size) {  // in s
	return get_properties(src,dest)->getAvgP2Pdelay(msg_size);
}

template<class address_type>
sc_time &comm_res<address_type>::get_AvgP2Pdelay(link_t<address_type> &link, unsigned int msg_size) {
	return get_properties(link)->getAvgP2Pdelay(msg_size);
}

template<class address_type>
unsigned int comm_res<address_type>::get_number_of_connected_elements() {
	return connected_elements.size();
}


template<class address_type>
void comm_res<address_type>::print_connected_elements() {
	unsigned int csize;
	csize = connected_elements.size();
	cout << "Communication Resource " << name() << " -> Connected Elements: ";
	if(csize>0) {
		for(unsigned int i;i<csize;i++) {
			cout << connected_elements[i]->name();
			if(i<(csize-1)) {
				cout << ", ";
			} else {
				cout << endl;
			}
		}
	} else {
		cout << "none." << endl;
	}
}

template<class address_type>
void comm_res<address_type>::print_links() {
	typename links_table_t::iterator it;
	cout << "Communication Resource " << name() << " -> Registered Links: " << endl;
	it=links_table.begin();
	while(it!=links_table.end()) {
		cout << it->first;
		it++;
		if(it!=links_table.end()) cout << ", ";
		else cout << endl;
	}
}

// CUrrently, it only shows the maximum delay
template<class address_type>
void comm_res<address_type>::print_links_table() {
	typename links_table_t::iterator it;
	linfo_msize_it_t	linfo_it;
	
	cout << "Communication Resource " << name() << ": Links Table: " << endl;
	cout << "Link \t message size (bits) \t MaxP2Pdelay" << endl;
	it=links_table.begin();
	while(it!=links_table.end()) {
		if(it->second==NULL) {
			cout << it->first << "\t NO INFO" << endl;
		} else {
			linfo_it=it->second->begin();
			while( linfo_it != it->second->end() ) {
				cout << it->first;		// print out link
				cout << "\t";
				cout << linfo_it->first; // print out message size
				cout << "\t";
				cout << linfo_it->second->getMaxP2Pdelay(); // print out associated info (only max. p2p delay so far)
				cout << endl;
				linfo_it++;
			}
		}
		it++;
	}
}

} // namespace kista

#endif
