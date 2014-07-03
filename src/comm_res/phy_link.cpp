/*****************************************************************************

  phy_link.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/


#ifndef PHY_LINK_CPP
#define PHY_LINK_CPP

#include "global_elements.hpp"

#include "phy_link.hpp"

#include "processing_element.hpp"
#include "task_info_t.hpp"
#include "scheduler.hpp"

namespace kista {
	
phy_address get_phy_address(logic_address laddress) {
	// Currently this implementation relies in that
	// laddress is a task pointer and that naddress is a PE pointer
	laddress->get_scheduler()->get_PE();
}

phy_link_info_t::phy_link_info_t(const char *name_par)
						: phy_link_t(name_par)
						{}
	
phy_link_info_t::phy_link_info_t(const char *name_par, phy_link_t llink_par)
						: phy_link_t(name_par, llink_par.src, llink_par.dest)
						{}
						
phy_link_info_t::phy_link_info_t(const char *name_par, phy_address src_address, phy_address dest_address)
						: phy_link_t(name_par, src_address, dest_address)
						{}

phy_link_info_t::phy_link_info_t(const char *name_par, const char *src_name_par, const char *dest_name_par)
						: phy_link_t(name_par, src_name_par, dest_name_par)
						{}
					
// Specialized function to get physical address by elem name
template<>
phy_address get_address_by_elem_name<phy_address>(std::string elem_name) {
	return PEref_by_name[elem_name];
}

}

#endif

