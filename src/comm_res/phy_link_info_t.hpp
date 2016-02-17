/*****************************************************************************

  phy_link_info_t.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2016 February

 *****************************************************************************/


#ifndef PHY_LINK_INFO_T_HPP
#define PHY_LINK_INFO_T_HPP

#include "link_t.hpp"

#include "phy_link_t.hpp"

#include "link_info_t.hpp"

#include "types.hpp"

namespace kista {

class phy_link_info_t : virtual public phy_link_t,
							virtual public link_info_t {
public:

	phy_link_info_t(const char *name_par);
	
	phy_link_info_t(const char *name_par, phy_link_t llink_par);
						
	phy_link_info_t(const char *name_par, phy_address src_address, phy_address dest_address);

	phy_link_info_t(const char *name_par, const char *src_name_par, const char *dest_name_par);

};

} // end of kista namespace

#endif
