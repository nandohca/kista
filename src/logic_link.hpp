/*****************************************************************************

  logic_link.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/

#ifndef LOGIC_LINK_HPP
#define LOGIC_LINK_HPP

#include "types.hpp"
#include "link_t.hpp"
#include "link_info_t.hpp"
//#include "global_elements.hpp"

namespace kista {

typedef link_t<logic_address> logic_link_t;	
	
class logic_link_info_t : public logic_link_t,
							public link_info_t {
public:

	logic_link_info_t(const char *name_par);
	
	logic_link_info_t(const char *name_par, logic_link_t llink_par);
											
	logic_link_info_t(const char *name_par, logic_address src_address, logic_address dest_address);

	logic_link_info_t(const char *name_par, const char *src_name_par, const char *dest_name_par);
	
	//sc_signal<bool> ll_transfer; // rising edge: start transfer falling edge: end transfer

};

// Specialized function to get logic address by elem name
// (It considers all types of tasks, so both, system and environment tasks)
template<>
logic_address get_address_by_elem_name<logic_address>(std::string elem_name);


} // end kista namespace

#endif
