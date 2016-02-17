/*****************************************************************************

  phy_link_t.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2016 February

 *****************************************************************************/


#ifndef PHY_LINK_T_HPP
#define PHY_LINK_T_HPP

#include "link_t.hpp"

#include "phy_link_t.hpp"

#include "types.hpp"

namespace kista {

// function to obtain the physical address from a logical address
phy_address get_phy_address(logic_address laddress);

// Specialization of link_t class for Physical Links

//typedef link_t<phy_address>   phy_link_t; // in types.hpp

/*
 class phy_link_t: public link_t<phy_address> {
public:

	phy_link_t();
	phy_link_t(const char *name_par);
	phy_link_t(const char *name_par, phy_address src_par, phy_address dest_par);
	phy_link_t(const char *name_par, const char  *src_name_par, const char *dest_name_par);
	// inheriting all the methods 
	// does it required using clauses?
	
	// add the method:
	bool is_PE_intracomm();  // return true if the link is done within a processing element
	bool is_PE_PE_comm();    // return true if the link is done between 2 processing elements
	bool is_PE_MEM_comm();   // return true if the link is done between 1 processing element and a memory
	// To do: add method for PE_mem connection
};
*/

bool is_PE_intracomm(phy_link_t &plink);  // return true if the link is done within a processing element
bool is_PE_PE_comm(phy_link_t &plink);    // return true if the link is done between 2 processing elements
bool is_PE_MEM_comm(phy_link_t &plink);   // return true if the link is done between 1 processing element and a memory

// Specialized function to get physical address by elem name
// It assumes that two different platform elements do not have the same name in the model,
// It includes platform elements of the same class (e.g. two PEs) and also
// platform elements of different class, e.g. a PE shall not have the same name
// as  memory
template<>
phy_address get_address_by_elem_name<phy_address>(std::string elem_name);

} // end of kista namespace

#endif
