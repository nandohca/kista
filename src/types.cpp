/*****************************************************************************

  types.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January
   
  Complex (class) Types implementation

 *****************************************************************************/

#ifndef _TYPES_CPP
#define _TYPES_CPP

#include "types.hpp"

namespace kista {

// A comparison function is required for actual comparison of the strings (and not only of string pointers)
bool  const_char_cmp::operator()(const char *str1,const char *str2) const
{
	return strcmp(str1,str2)<0;
}



// Implementation of message class methods

message_t::message_t() {
	host_size = 0;
	datap = NULL;
}

// constructor
message_t::message_t(void *datap_var, size_t size_var) {
	char *charp;
	charp = (char *)datap_var;
	host_size = size_var;
	datap = new char[host_size];
	for(unsigned int i=0;i<host_size;i++) {
		datap[i]=charp[i];
	}
}

// copy constructor
message_t::message_t(const message_t& other) {
	// delete the previous content of the message
	if(datap!=NULL) delete[] datap;
	// create a new value
	host_size = other.host_size;
	datap = new char[this->host_size];
	for(unsigned int i=0;i<this->host_size;i++) {
		this->datap[i]=other.datap[i];
	}
}

message_t& message_t::operator =(const message_t& other) {
	// delete the previous content of the message
	if(datap!=NULL) delete[] datap;
	// create a new value
	host_size = other.host_size;
	datap = new char[this->host_size];
	for(unsigned int i=0;i<this->host_size;i++) {
		this->datap[i]=other.datap[i];
	}	
}


// 
ostream& operator<<(ostream& os, message_t msg) {
	for(unsigned int i=0;i<msg.host_size;i++) {
		os << msg.datap[i];
		if(i!=(msg.host_size-1)) os << ",";
	}	
	return os;
}



// copy constructor implementation of send_message_request_t class

/*
send_message_request_t::send_message_request_t(const &send_message_request_t smr) :
	phy_link(smr.phy_link),
	message_size(smr.message_size),
	transfer_complete_event_p(smr.transfer_complete_event_p),
	transfer_delay(smr.transfer_delay)
	
{
}


send_message_request_t::send_message_request_t(&send_message_request_t smr) :
	phy_link(smr.phy_link),
	message_size(smr.message_size),
	transfer_complete_event_p(smr.transfer_complete_event_p),
	transfer_delay(smr.transfer_delay)
	
{
}
 */

} // end namespace kista

#endif
