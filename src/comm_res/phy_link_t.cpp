/*****************************************************************************

  phy_link_t.cpp
  
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
#include "memory_resource.hpp"
#include "task_info_t.hpp"
#include "scheduler.hpp"


namespace kista {
	
phy_address get_phy_address(logic_address laddress) {
	// Currently this implementation relies in that
	// laddress is a task pointer and that naddress is a PE pointer
	laddress->get_scheduler()->get_PE();
}

// implementaton of methods for the phy_link_t class speciazlization
template<>
void phy_link_t<phy_address>::is_PE_PE() { // return true if the link is done between 2 processing elements
	cout << "CALLIN is_PE_PE" << endl;
};

											
// Specialized function to get physical address by elem name
template<>
phy_address get_address_by_elem_name<phy_address>(std::string elem_name) {
	//phy_address curr_address = NULL;
	
	PE_by_name_t::iterator PE_ref_by_name_it;
	MEM_by_name_t::iterator MEM_ref_by_name_it;
	
	processing_element *pe_p;
	memory_resource *mem_p;
	
	// First look for the element in the PE global list
	PE_ref_by_name_it = PEref_by_name.find(elem_name);
	
	if(PE_ref_by_name_it!=PEref_by_name.end()) {
		pe_p=PE_ref_by_name_it->second;
		return pe_p->get_address();
	} 

	// In case it is not found, look for the element in the MEM global list
	MEM_ref_by_name_it = MEMref_by_name.find(elem_name);
	
	if(MEM_ref_by_name_it!=MEMref_by_name.end()) {
		mem_p=MEM_ref_by_name_it->second;
		return mem_p->get_address();
	} 
	
	// so far, reaching here means that the element is not found
	SC_REPORT_ERROR("KisTA","In get_address_by_elem_name. Hw resource not found in global list");
	
	// TO DO: then look for other type of elements in their elements global list
	//   elements to support
	//   IO devices
	
	// 
}

}

#endif

