/*****************************************************************************

  phy_link.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/


#ifndef PHY_LINK_INFO_T_CPP
#define PHY_LINK_INFO_T_CPP

#include "global_elements.hpp"

#include "phy_link_info_t.hpp"

namespace kista {
	
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
											

}

#endif

