/*****************************************************************************

  link_t.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/

#ifndef LINK_INFO_T_HPP
#define LINK_INFO_T_HPP

#include <systemc.h>

namespace kista {
	
class link_info_t {
public:
	link_info_t();	

	// setters
	
	// In the general case each value is associated to a message size
	// (methods can be resused for size independent delays and latencies)
	// then each logic link stores actually a function which provides the 
	// delay for the given size.
	// Then, the call to such a function of several times will let to store
	// a table of delay-size values.
	// Two other (TO BE DONE) alternatives are:
	//    - pass the table as a map
	//    - pass a function (pointer) msg_size -> delay)
	
	// The generic network, just stores and returns fixes values by default
	
	// Current Values (at a given time stamp), 
		// P2P delay (transmission)
	void setCurrentP2Pdelay(sc_time CurrentP2Pdelay_par, int msg_size = 1);

		// sustained transmission rate
	void setCurrentBW(double CurrentBW_par, int msg_size = 1);	// bps

	// worst case characterization of the network
		// P2P delay (transmission)
	void setMaxP2Pdelay(sc_time MaxP2Pdelay_par, int msg_size = 1);
	
		// sustained transmission rate
	void setMinBW(double MinBW_par, int msg_size = 1); // bps
	
	// average characterization
		// P2P delay (transmission)
	void setAvgP2Pdelay(sc_time AvgP2Pdelay_par, int msg_size = 1); 
		// sustained transmission rate
	void setAvgBW(double AvgBW_par, int msg_size = 1);	// bps

	// getters

	// Current Values (at a given time stamp)
			// P2P delay (transmission)
	double &getCurrentBW();		// bps
			// sustained transmission rate
	sc_time &getCurrentP2Pdelay(unsigned int msg_size=1);

	// worst case characterization of the network
		// P2P delay (transmission)
	sc_time &getMaxP2Pdelay(unsigned int msg_size=1);
		// sustained transmission rate
	double &getMinBW();			// bps
	
	// average characterization
			// P2P delay (transmission)
	double &getAvgBW();			// bps
	 	// sustained transmission rate
	sc_time &getAvgP2Pdelay(unsigned int msg_size=1);
		
private:

	void report_warning(std::string call);
	void report_error(std::string call);

	double MinBW; 		
	sc_time MaxP2Pdelay;
	
	double CurrentBW; 		
	sc_time CurrentP2Pdelay;
	
	double AvgBW; 		
	sc_time AvgP2Pdelay;	
};


} // end kista namespace


#endif
