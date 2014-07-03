/*****************************************************************************

  create_platform_mapping.cpp
  
   Functions for generating the mapping from SW platform elements to HW platform elements
   in the kista-xml front-end.
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 July

 *****************************************************************************/

#ifndef _CREATE_PLATFORM_MAPPING_CPP
#define _CREATE_PLATFORM_MAPPING_CPP

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <libxml/xmlschemas.h>

#include <vector>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <vector>

#include <systemc.h>

#include <kista.hpp>

// Creates the KisTA mapping from SW RTOS components to PE components
// from the mapping entries in the XML file

// returns the first scheduler in the KisTA scheduler vector structure
// created by former steps of KisTA-xml parser
scheduler *getRTOSByName(std::vector<scheduler*> &RTOS_vec, const char *name) {
	unsigned int i;
	std::string rpt_msg;
	for(i=0; i< RTOS_vec.size() ; i++) {
		if(!strcmp(RTOS_vec[i]->name(),name)) {
			return RTOS_vec[i];
		}
	}
	rpt_msg = "RTOS mapping failed. No RTOS instance called";
	rpt_msg += name;
	rpt_msg += "was found.\n";
	rpt_msg += "Please, check that it has been declared in the SW platform section.";
	SC_REPORT_ERROR("KisTA-XML", rpt_msg.c_str());
	return NULL;
}

// returns the first processing element in the KisTA PE vector structure
// created by former steps of KisTA-xml parser
processing_element *getPEByName(std::vector<processing_element*> &PE_cluster, const char *name) {
	unsigned int i;	
	std::string rpt_msg;
	for(i=0; i< PE_cluster.size() ; i++) {
		if(!strcmp(PE_cluster[i]->name(),name)) {
			return PE_cluster[i];
		}
	}
	rpt_msg = "RTOS mapping failed. No processing element instance called ";
	rpt_msg += name;
	rpt_msg += "was found.\n";
	rpt_msg += "Please, check that it has been declared in the HW platform section.";
	SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	return NULL;
}


// get all processing elements from the platform
xmlNodeSetPtr getRTOS2PEMappings(xmlDocPtr doc) {
	xmlNodeSetPtr nodeset;		
	xmlXPathObjectPtr  XPathObjectPtr;
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)"/system/platform/platform_mapping/map_rtos");
	if(XPathObjectPtr==NULL) {
		SC_REPORT_ERROR("KisTA-XML","Mapping of SW platform to HW platform failed. No RTOS mapping was read in the XML input.");
	}
	nodeset = XPathObjectPtr->nodesetval;
	return nodeset;
}

// create_platform_mapping maps the SW platform components into
// HW processing elements
// NOTE: It requires first to have the RTOS and PE vectors filled,
//        so it assumes that SW platform and HW platform have been
//        created first.
void create_platform_mapping(xmlDocPtr doc,
							std::vector<scheduler*> &RTOS_vec,
							std::vector<processing_element*> &PE_cluster) {
	int j;
	xmlNodeSetPtr setMapNodes;
	xmlChar *SrcName;
	xmlChar *TgtName;
	
	scheduler *RTOS_ptr;
	processing_element *PE_ptr;
	
    std::string rpt_msg;

	// First, checks the inputs	
	if(RTOS_vec.size()<1) {
		rpt_msg = "Creation of mapping from SW platform to HW platform failed.";
		rpt_msg += "\tNo RTOS intance loaded by the XML front-end. Please, check that the create_platform_mapping function is called after the create_sw_platform function.";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}
	
	if(PE_cluster.size()<1) {
		rpt_msg = "Creation of mapping from SW platform to HW platform failed.\n";
		rpt_msg += "No PE loaded by the XML front-end. Please, check that the create_platform_mapping function is called after the create_HW_platform function.";
		SC_REPORT_ERROR("Kista-XML",rpt_msg.c_str());
	}
		
	setMapNodes = getRTOS2PEMappings(doc);
		
	if(setMapNodes==NULL) {
		printf("Kista-XML Warning: No nodes for mapping RTOS to processing element instances detected.\n");
	}

	// setPENodes should not be NULL here because we have already checked that there were nodes in the tree
#ifdef _VERBOSE_KISTA_XML
	if(global_kista_xml_verbosity) {
		printf("kista-XML INFO: Platform mapping...\n");
	}
#endif	
	for(j=0; j< setMapNodes->nodeNr ; j++) {
		SrcName = xmlGetValueGen(setMapNodes->nodeTab[j],(xmlChar *)"source");
		TgtName = xmlGetValueGen(setMapNodes->nodeTab[j],(xmlChar *)"target");

		RTOS_ptr = getRTOSByName(RTOS_vec,(const char *)SrcName);
		PE_ptr = getPEByName(PE_cluster,(const char *)TgtName);
		
		// ... and finally, do the mapping
		RTOS_ptr->map_to(PE_ptr);
		
#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {
			printf("kista-XML INFO: Platform mapping (RTOS) %s -> (PE) %s .\n",SrcName,TgtName);
		}
#endif
	}
	 
}

#endif
