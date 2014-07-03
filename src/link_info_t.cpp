/*****************************************************************************

  link_info_t.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/

#ifndef LINK_INFO_T_CPP
#define LINK_INFO_T_CPP

#include "defaults.hpp"
#include "link_info_t.hpp"

namespace kista {
	
// constructors
link_info_t::link_info_t() {
	 // Defaults values for link info type
	// <0 BW = infinite BW
	// <0 P2P delay = error
	MinBW = -1.0; 
	MaxP2Pdelay = SC_ZERO_TIME;

	CurrentBW = -1.0; 
	CurrentP2Pdelay = SC_ZERO_TIME;

	AvgBW = -1.0; 
	AvgP2Pdelay = SC_ZERO_TIME;
}



// setters implementation

// Current Values (at a given time stamp), 
	// P2P delay (transmission)
void link_info_t::setCurrentP2Pdelay(sc_time CurrentP2Pdelay_par, int msg_size)
{
#ifdef _REPORT_LINK_ATTRIBUTES_SETTING_FROM_BASE_LINK_AS_WARNING
	report_warning("setCurrentP2Pdelay");
#endif
#ifdef _REPORT_LINK_ATTRIBUTES_SETTING_FROM_BASE_LINK_AS_ERROR
	report_error("setCurrentP2Pdelay");
#endif
	CurrentP2Pdelay = CurrentP2Pdelay_par;
}

// sustained transmission rate
void link_info_t::setCurrentBW(double CurrentBW_par, int msg_size)
{
#ifdef _REPORT_LINK_ATTRIBUTES_SETTING_FROM_BASE_LINK_AS_WARNING
	report_warning("setCurrentBW");
#endif
#ifdef _REPORT_LINK_ATTRIBUTES_SETTING_FROM_BASE_LINK_AS_ERROR
	report_error("setCurrentBW");
#endif	
	CurrentBW = CurrentBW_par;
}

// worst case characterization of the network
	// P2P delay (transmission)
void link_info_t::setMaxP2Pdelay(sc_time MaxP2Pdelay_par, int msg_size)
{
#ifdef _REPORT_LINK_ATTRIBUTES_SETTING_FROM_BASE_LINK_AS_WARNING
	report_warning("setMaxP2Pdelay");
#endif
#ifdef _REPORT_LINK_ATTRIBUTES_SETTING_FROM_BASE_LINK_AS_ERROR
	report_error("setMaxP2Pdelay");
#endif	
	MaxP2Pdelay = MaxP2Pdelay_par;
}

// sustained transmission rate
void link_info_t::setMinBW(double MinBW_par, int msg_size)
{
#ifdef _REPORT_LINK_ATTRIBUTES_SETTING_FROM_BASE_LINK_AS_WARNING
	report_warning("setMinBW");
#endif
#ifdef _REPORT_LINK_ATTRIBUTES_SETTING_FROM_BASE_LINK_AS_ERROR
	report_error("setMinBW");
#endif	
	MinBW = MinBW_par;
}
	
// average characterization
	// P2P delay (transmission)
void link_info_t::setAvgP2Pdelay(sc_time AvgP2Pdelay_par, int msg_size)
{
#ifdef _REPORT_LINK_ATTRIBUTES_SETTING_FROM_BASE_LINK_AS_WARNING
	report_warning("setCurrentP2Pdelay");
#endif
#ifdef _REPORT_LINK_ATTRIBUTES_SETTING_FROM_BASE_LINK_AS_ERROR
	report_error("setCurrentP2Pdelay");
#endif	
	AvgP2Pdelay = AvgP2Pdelay_par;
}

// sustained transmission rate
void link_info_t::setAvgBW(double AvgBW_par, int msg_size)
{
#ifdef _REPORT_LINK_ATTRIBUTES_SETTING_FROM_BASE_LINK_AS_WARNING
	report_warning("setCurrentP2Pdelay");
#endif
#ifdef _REPORT_LINK_ATTRIBUTES_SETTING_FROM_BASE_LINK_AS_ERROR
	report_error("setCurrentP2Pdelay");
#endif	
	AvgBW = AvgBW;
}


// getters implementation

	// Current Values (at a given time stamp)
			// P2P delay (transmission)
double &link_info_t::getCurrentBW() {
#ifdef _REPORT_LINK_ATTRIBUTES_RETRIEVAL_FROM_BASE_LINK_AS_WARNING
	report_warning("getCurrentBW");
#endif
#ifdef _REPORT_LINK_ATTRIBUTES_RETRIEVAL_FROM_BASE_LINK_AS_ERROR
	report_error("getCurrentBW");
#endif
	return CurrentBW;
}				
			// sustained transmission rate
sc_time &link_info_t::getCurrentP2Pdelay(unsigned int msg_size) {
#ifdef _REPORT_LINK_ATTRIBUTES_RETRIEVAL_FROM_BASE_LINK_AS_WARNING
	report_warning("getCurrentP2Pdelay");
#endif
#ifdef _REPORT_LINK_ATTRIBUTES_RETRIEVAL_FROM_BASE_LINK_AS_ERROR
	report_error("getCurrentP2Pdelay");
#endif
	return CurrentP2Pdelay;
}

// worst case characterization of the network
		// P2P delay (transmission)
sc_time &link_info_t::getMaxP2Pdelay(unsigned int msg_size) {
#ifdef _REPORT_LINK_ATTRIBUTES_RETRIEVAL_FROM_BASE_LINK_AS_WARNING
	report_warning("getMaxP2Pdelay");
#endif
#ifdef _REPORT_LINK_ATTRIBUTES_RETRIEVAL_FROM_BASE_LINK_AS_ERROR
	report_error("getMaxP2Pdelay");
#endif
	return MaxP2Pdelay;
}

// sustained transmission rate
double &link_info_t::getMinBW() {
#ifdef _REPORT_LINK_ATTRIBUTES_RETRIEVAL_FROM_BASE_LINK_AS_WARNING
	report_warning("getMinBW");
#endif
#ifdef _REPORT_LINK_ATTRIBUTES_RETRIEVAL_FROM_BASE_LINK_AS_ERROR
	report_error("getMinBW");
#endif
		return MinBW;
}

// average characterization
			// P2P delay (transmission)
double &link_info_t::getAvgBW() {
#ifdef _REPORT_LINK_ATTRIBUTES_RETRIEVAL_FROM_BASE_LINK_AS_WARNING
	report_warning("getAvgBW");
#endif
#ifdef _REPORT_LINK_ATTRIBUTES_RETRIEVAL_FROM_BASE_LINK_AS_ERROR
	report_error("getAvgBW");
#endif
	return AvgBW;
}

// sustained transmission rate
sc_time &link_info_t::getAvgP2Pdelay(unsigned int msg_size) {
#ifdef _REPORT_LINK_ATTRIBUTES_RETRIEVAL_FROM_BASE_LINK_AS_WARNING
	report_warning("getAvgP2Pdelay");
#endif
#ifdef _REPORT_LINK_ATTRIBUTES_RETRIEVAL_FROM_BASE_LINK_AS_ERROR
	report_error("getAvgP2Pdelay");
#endif
	return AvgP2Pdelay;
}

void link_info_t::report_warning(std::string call) {
		std::string msg;
		msg = "The call to ";
		msg += call;
		msg += " was serviced by the link_info_t base class, and not by the specific communication resource.";
		SC_REPORT_WARNING("KisTA",msg.c_str());
}

void link_info_t::report_error(std::string call) {
		std::string msg;
		msg = "The call to ";
		msg += call;
		msg += " was serviced by the link_info_t base class, and not by the specific communication resource.";
		SC_REPORT_ERROR("KisTA",msg.c_str());
}

}
#endif
