/*****************************************************************************

  mapped_links_t.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/

#ifndef MAPPED_LINKS_T_HPP
#define MAPPED_LINKS_T_HPP

#include "logic_link.hpp"
#include "comm_res/phy_link_t.hpp"

namespace kista {

// class for extra informaton arround links
class mapped_llinks_t {
public:
	typedef std::vector<logic_link_t> llinks_vector_t;	
	
	mapped_llinks_t() {}
	
	mapped_llinks_t(phy_link_t plink_par)
						//: phy_link_t(plink_par.src, nlink_par.dest)
						{}
						
	void assign(logic_link_t &llink);	 	// assign a logic link to a physical link
	unsigned int get_number_llinks();  // get the number of mapped logical links into this physical link
						
private:
	llinks_vector_t		llinks_table; // Several logic links can be associated to a physical link

};

}

#endif
