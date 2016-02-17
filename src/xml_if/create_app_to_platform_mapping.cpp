/*****************************************************************************

  create_app_to_platform_mapping.cpp
  
   Functions for generating the mapping from application elements (tasks)
   to platform elements (RTOS) in the kista-xml front-end.
   The XML fron-end creates a taskset for each RTOS instance generated
   in the platform creation and maps it to the RTOS instance as stated
   by the mapping in the XML file.
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 July

 *****************************************************************************/

#ifndef CREATE_APP_TO_PLATFORM_MAPPING
#define CREATE_APP_TO_PLATFORM_MAPPING

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

// returns the first processing element in the KisTA PE vector structure
// created by former steps of KisTA-xml parser
task_info_t *getTaskInfoByName(std::vector<task_info_t*> &task_vec, const char *name) {
	unsigned int i;	
	std::string rpt_msg;
	
	for(i=0; i< task_vec.size() ; i++) {
		if(!strcmp(task_vec[i]->name().c_str(),name)) {
			return task_vec[i];
		}
	}
	
	rpt_msg = "Application to platform mapping failed. No Task instance called";
	rpt_msg += name;
	rpt_msg += "was found.\n";
	rpt_msg += "Please, check that it has been declared in the application section";
	SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	return NULL;
}

// get all processing elements from the platform
xmlNodeSetPtr get_Task_2_RTOS_Mappings(xmlDocPtr doc) {
	xmlNodeSetPtr nodeset;		
	xmlXPathObjectPtr  XPathObjectPtr;
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)"/system/application_mapping/map_task");
	if(XPathObjectPtr==NULL) {
		SC_REPORT_ERROR("KisTA-XML","Mapping of Application Tasks to platform failed. No task mapping was read in the XML input.");
	}
	nodeset = XPathObjectPtr->nodesetval;
	return nodeset;
}


// create_app_to_platform_mapping maps the application components (tasks)
// into RTOS instances
// NOTE: It requires first to have the tasks and RTOS vectors filled,
//        so it assumes that SW platform and application have been
//        created first. That is, create_application and create_sw_platform have
//        to be invoked first
//
void create_app_to_platform_mapping(xmlDocPtr doc,
							std::vector<task_info_t*> &app_task_vec,
							std::vector<scheduler*> &RTOS_vec,
//							std::map<const char*, taskset_by_name_t*> &taskset_m
							std::map<const char*, taskset_by_name_t*,const_char_cmp> &taskset_m
							  // reminder: the const_char_cmp comparison function required to enable comparing the "string" typed key, and not the char pointer
							) {
	unsigned int i;
	int j;
	xmlNodeSetPtr setMapNodes;
	xmlChar *SrcName;
	xmlChar *TgtName;
	
	std::string rpt_msg;
	
	// declaration for tasks and task sets...
	task_info_t	*task_info_ptr;
//	scheduler	*RTOS_ptr; // not required finally

	taskset_by_name_t *curr_taskset;	
	
	// First, checks the inputs
	if(app_task_vec.size()<1) {
		rpt_msg = "Creation of mapping from application to platform failed.\n";
		rpt_msg += "\tNo task loaded by the XML front-end. Please, check that the create_app_to_platform_mapping function is called after the create_application function.";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}
	
	if(RTOS_vec.size()<1) {
		rpt_msg = "Creation of mapping from application to platform failed.\n";
		rpt_msg += "No RTOS intance loaded by the XML front-end. Please, check that the create_app_to_platform_mapping function is called after the create_sw_platform function.";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}

	for(i=0;i<RTOS_vec.size();i++) {
		taskset_m[RTOS_vec[i]->name()] = new taskset_by_name_t;

		if(global_kista_xml_verbosity) {
			printf("kista-XML INFO: Creating KisTA task set for RTOS %s\n",RTOS_vec[i]->name());
		}
			
	}
			
	setMapNodes = get_Task_2_RTOS_Mappings(doc);
		
	if(setMapNodes==NULL) {
		if(global_kista_xml_verbosity) {
			printf("Kista-XML Warning: No nodes for mapping Application Tasks to RTOS instances detected.\n");
		}
	}

	// setMapNodes should not be NULL here because we have already checked that there were nodes in the tree

	if(global_kista_xml_verbosity) {
		printf("kista-XML INFO: Application mapping...\n");
	}
	
	for(j=0; j< setMapNodes->nodeNr ; j++) {
		SrcName = xmlGetValueGen(setMapNodes->nodeTab[j],(xmlChar *)"source");
		
		if (SrcName==NULL) SC_REPORT_ERROR("KisTA-XML","Error reading application mapping source.");
		
		TgtName = xmlGetValueGen(setMapNodes->nodeTab[j],(xmlChar *)"target");

//		if (SrcName==NULL) SC_REPORT_ERROR("KisTA-XML","Error reading application mapping destination.");


		if(global_kista_xml_verbosity) {
			printf("kista-XML INFO: Application mapping (Task) %s -> (RTOS) %s .\n",SrcName,TgtName);
		}

		task_info_ptr = getTaskInfoByName(app_task_vec,(const char *)SrcName);
		
//		RTOS_ptr = getRTOSByName(RTOS_vec,(const char *)TgtName);
		
		// ... and finally, do the mapping

		// in KisTA...
		// task_set1["task1"] = &task1_info;
		// taskset_map[TgtName] gets the correspondng taskset
		// [SrcName] = assigns the task_info_ptr to the task SrcName in the task set
		//(taskset_map[(const char *)TgtName])[(const char *)SrcName] = task_info_ptr;
		
		//Retrieving current task set
		curr_taskset = taskset_m[(const char *)TgtName]; //get current taskset pointer

		// Loading task info pointer into the current taskset
		(*curr_taskset)[(const char *)SrcName] = task_info_ptr;
	}
	
	// Here the tasksets (one per scheduler) have been created and tasks have been assigned to tasksets
	// We still need to assign task sets to scheduler instances, which is done in the following staements

	if(global_kista_xml_verbosity) {
		printf("kista-XML INFO: Allocate tasksets to RTOS instances...\n");
	}
			
	for(i=0;i<RTOS_vec.size();i++) {
		RTOS_vec[i]->allocate( taskset_m[RTOS_vec[i]->name()] );
	}
}

#endif
