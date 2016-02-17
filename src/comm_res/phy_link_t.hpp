/*****************************************************************************

  phy_link.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/


#ifndef PHY_LINK_HPP
#define PHY_LINK_HPP

#include "link_t.hpp"

#include "link_info_t.hpp"

#include "types.hpp"

namespace kista {

// function to obtain the physical address from a logical address
phy_address get_phy_address(logic_address laddress);

// Specialization of link_t class for Physical Links
template<>
class phy_link_t<> {
public:
	// inheriting all the methods 
	
	bool is_PE_PE(); // return true if the link is done between 2 processing elements
};


class phy_link_info_t : virtual public phy_link_t,
							virtual public link_info_t {
public:

	phy_link_info_t(const char *name_par);
	
	phy_link_info_t(const char *name_par, phy_link_t llink_par);
						
	phy_link_info_t(const char *name_par, phy_address src_address, phy_address dest_address);

	phy_link_info_t(const char *name_par, const char *src_name_par, const char *dest_name_par);

};

// Specialized function to get physical address by elem name
// It assumes that two different platform elements do not have the same name in the model,
// It includes platform elements of the same class (e.g. two PEs) and also
// platform elements of different class, e.g. a PE shall not have the same name
// as  memory
template<>
phy_address get_address_by_elem_name<phy_address>(std::string elem_name);

} // end of kista namespace

#endif
