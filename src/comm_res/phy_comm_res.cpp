/*****************************************************************************

  phy_comm_res.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 March

 *****************************************************************************/

#ifndef _PHY_COMMUNICATION_RESOURCE_CPP
#define _PHY_COMMUNICATION_RESOURCE_CPP

#include <systemc.h>

#include "defaults.hpp"

#include "global_elements.hpp"

#include "phy_comm_res.hpp"

#include "processing_element.hpp"
#include "draw.hpp"

namespace kista {

//~ template<class address_type>
//~ void comm_res<address_type>::before_end_of_elaboration() {
	//~ sketch_report.draw(this);
//~ }

// ------------------------------------------------
// phy_comm_res_t IMPLEMENTATION
// ------------------------------------------------

phy_comm_res_t::phy_comm_res_t(sc_module_name name) : comm_res<phy_address>(name) {
	// updates the global comm_res elements table accessed by name
	std::string comm_res_name;
	comm_res_name = this->name();	
	phy_commres_by_name[comm_res_name]=this;
}

void phy_comm_res_t::map(logic_link_t &llink,phy_link_t &plink) {
	// check if the physical link is already in the table
	
	mapped_llinks_t *mapped_llinks;
	plinks_llinks_table_t::iterator pl_it;
	std::string rpt_msg;

//cout << "START OF LLINK - PLINK MAP!" << endl;
	
	if(plinks_llinks_table.size()<1) { // plinks_llinks_table is empty,
		//  so necessarily we need to add this physical link
		//
		//    ...for it, 
		// ... create vector of mapped links and fill it with the current llink
		mapped_llinks = new mapped_llinks_t;
		mapped_llinks->assign(llink);
		// ... and assigns it to the 
		//plinks_llinks_table.insert(make_pair<plink,mapped_llinks>);
		plinks_llinks_table[plink]= mapped_llinks;
		
		//
//		cout << "Add first created plink-llink connection." << endl;
		
	} else { // plinks_llinks_table is empty,
		// so we need to look if the physical link is already present in the
		// plinks_llinks_table (with at least one asssociated llink)
		pl_it  = plinks_llinks_table.find(plink);
		if (pl_it==plinks_llinks_table.end()) { // physical link not found, we need to add
		                                         // the link anyhow, as we did before
		    // create vector of mapped links and fill it with the current llink
			mapped_llinks = new mapped_llinks_t;
			mapped_llinks->assign(llink);
			// ... and assigns it to the 
			//plinks_llinks_table.insert(plink,mapped_llinks);
			plinks_llinks_table[plink]= mapped_llinks;
		} else { // plink found, so
			// we check first that there is a non-empty list of logical links associated
			mapped_llinks = plinks_llinks_table[plink];
			if (mapped_llinks->get_number_llinks()>0) {
				// check if the logical link stored is the same as the one is going
				// to be registered (that is considered an error condition
				
				// ... and finally, since it is not the same, we add the logic link
				mapped_llinks->assign(llink);
				
			} else {
				rpt_msg = "In communication resource ";
				rpt_msg += this->name();
				rpt_msg += "\n Unexpected situation. Physical link (";
				rpt_msg += plink.src->name();
				rpt_msg += ",";
				rpt_msg += plink.dest->name();
				rpt_msg += " ) entry found in the table for association";			
				rpt_msg += " of Physical and Logical links, without any associated logical link.";			
				SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
			}
		}	
	}
}
	
// method to return the number of logic links allocated to a physical link
// (this enables to detect if the llinks need some scheduling over the nlinks)
unsigned int phy_comm_res_t::number_of_llinks(phy_link_t plink) {
	return plinks_llinks_table[plink]->get_number_llinks();
}	

void phy_comm_res_t::before_end_of_elaboration() {

	// to draw the communication resource
	sketch_report.draw(this);
}

} // namespace kista

#endif
