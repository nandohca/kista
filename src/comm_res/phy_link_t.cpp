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

#include "phy_link_t.hpp"

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

/*
phy_link_t::phy_link_t(): link_t<phy_address>()
 { }

phy_link_t::phy_link_t(const char *name_par): link_t<phy_address>(name_par)
 { }


phy_link_t::phy_link_t(const char *name_par, phy_address src_par, phy_address dest_par): link_t<phy_address>(name_par,src_par,dest_par)
 { }

phy_link_t::phy_link_t(const char *name_par, const char  *src_name_par, const char *dest_name_par): link_t<phy_address>(name_par,src_name_par,dest_name_par)
 { }

 
bool phy_link_t::is_PE_intracomm() { // return true if the link is done between 2 processing elements
	if ( this->is_intracomm() && (src->get_type()==PROCESSING_ELEMENT)) // in this case, dest is obviously also a processing element
		return true;
	else
		return false;
};

// return true if the link is done between 2 processing elements
bool phy_link_t::is_PE_PE_comm() { 
	if ( (src->get_type()==PROCESSING_ELEMENT) && (dest->get_type()==PROCESSING_ELEMENT) )
		return true;
	else
		return false;
};

// return true if the link is done between 1 processing element and a memory resource
bool phy_link_t::is_PE_MEM_comm() { 
	if ( (src->get_type()==PROCESSING_ELEMENT) && (dest->get_type()==MEMORY_RESOURCE) )
		return true;
	else
		return false;
};
 */
 
bool is_PE_intracomm(phy_link_t &plink) { // return true if the link is done between 2 processing elements
	if ( plink.is_intracomm() && (plink.src->get_type()==PROCESSING_ELEMENT)) // in this case, dest is obviously also a processing element
		return true;
	else
		return false;
};

// return true if the link is done between 2 processing elements
bool is_PE_PE_comm(phy_link_t &plink) { 
	if ( (plink.src->get_type()==PROCESSING_ELEMENT) && (plink.dest->get_type()==PROCESSING_ELEMENT) )
		return true;
	else
		return false;
};

// return true if the link is done between 1 processing element and a memory resource
bool is_PE_MEM_comm(phy_link_t &plink) { 
	if ( (plink.src->get_type()==PROCESSING_ELEMENT) && (plink.dest->get_type()==MEMORY_RESOURCE) )
		return true;
	else
		return false;
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

