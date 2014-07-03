/*****************************************************************************

  mapped_links_t.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/

#ifndef MAPPED_LINKS_T_CPP
#define MAPPED_LINKS_T_CPP

#include "mapped_links_t.hpp"

namespace kista {

// phy_link_info_t IMPLEMENTATION
void mapped_llinks_t::assign(logic_link_t &llink) {	 	// map a logic link to a physical link
	llinks_table.push_back(llink);
}

unsigned int mapped_llinks_t::get_number_llinks() {  // get the number of mapped logical links into this physical link
	return llinks_table.size();
}

}

#endif
