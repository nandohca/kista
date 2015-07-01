/*****************************************************************************

  shared_var.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: UC
  Deparment:   TEISA
  Date: 2015 July

 *****************************************************************************/

#ifndef _SHARED_VARIABLE_HPP
#define _SHARED_VARIABLE_HPP

#include "systemc.h"

#include "defaults.hpp"

namespace kista {
	template<class T>
	class shared_var {
	public:
	  shared_var(); // constructor without initialization
	  shared_var(T &val_par); // constructor with initialization (requires the "=" operator)
	  
	  void write(const T &par);
	  T &par read();
	  
	  // method to map to a memory resource
	  void map_to(memory_resource *mem);
	  // method to map to a memory resource and indicate a specific access bus
	  // void map_to(memory_resource *mem, phy_comm_res_t *phy_comm_res_par);
	  
	private:
	  T	val;
	  
	  //Two possible implementations of the shared var can be considered:
	  
	  // A) a scheme considering the implementation of a shared variable on top of
	  // a memory, in turn linked to a bus ... 
	  phy_link_t    	assoc_ms_plink1, assoc_ms_plink2; 	// associated physical link
	  phy_comm_res_t    *assoc_comm_res_p; // associated physical communication resource
      
      // B) A dual port ram connecting two processing elements
      phy_link_t    	assoc_plink;
};	
	
} // namespace KisTA

#endif
