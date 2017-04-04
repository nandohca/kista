/*****************************************************************************

  phy_comm_res.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 March


 physical communication resource

 *****************************************************************************/

#ifndef _PHY_COMMUNICATION_RESOURCE_HPP
#define _PHY_COMMUNICATION_RESOURCE_HPP

#include <systemc.h>

#include "defaults.hpp"

#include "comm_res.hpp"

namespace kista {

//typedef comm_res<phy_address>	phy_comm_res_t;

class phy_comm_res_t: public comm_res<phy_address> {
public:		
	typedef std::map<phy_link_t, mapped_llinks_t *>	plinks_llinks_table_t;
	
	phy_comm_res_t(sc_module_name name = sc_gen_unique_name("phy_comm_res"));
	
	// map a logic link into a physical link associated to the communication resource
	void map(logic_link_t &llink, phy_link_t &plink);
	
	// method to return the number of logic links allocated to a physical link
	// (this enables to detect if the llinks need some scheduling over the nlinks)
	unsigned int number_of_llinks(phy_link_t plink);

private:

	virtual void before_end_of_elaboration();
	// Several logic links can be mapped to the net link,
	//  the order in the vector gives the order in which the communication is scheduled
	//allocated_llinks_t allocated_llinks;
	plinks_llinks_table_t plinks_llinks_table;
	
};

} // namespace kista

#endif
