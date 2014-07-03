/*****************************************************************************

  create_sw_platform.cpp
  
   Functions for generating the hw platform in the kista-xml front-end.
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 July

 *****************************************************************************/

#ifndef CREATE_SW_PLATFORM
#define CREATE_SW_PLATFORM

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

// retruns the first node (should be only one) of scheduler_plicy type
// Coded without XPath, although it could be done with it also
//xmlNodePtr getSchedulerPolicy(xmlDocPtr doc, xmlNodePtr cur) {
	//cur_elem_ptr = cur_elem_ptr->xmlChildrenNode;	
	//while (cur_elem_ptr != NULL) {
		//if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"scheduler_policy"))) {
			//curr_node = curr_node->childrenNode;
			//while (cur != NULL) {
				//if ((!xmlStrcmp(cur->name, (const xmlChar *)"static_scheduling"))){
					//return cur;
				//}
				//cur = cur->next;
			//}		
			//return cur_elem_ptr;
		//}
		//cur_elem_ptr = cur_elem_ptr->next;
	//}
	//SC_REPORT_ERROR("KisTA-XML","No scheduler policy defined.");
//}



// This function is useful asuming only one scheduler!!
/*
 * xmlNodePtr getSchedulerPolicy(xmlDocPtr doc) {	
//	if(getNodeSet (doc, (xmlChar *)"./scheduler_policy/static_scheduling")!=NULL) {
	if(getNodeSet (doc, (xmlChar *)"/system/platform/SW_platform/rtos/scheduler_policy/static_scheduling")!=NULL) {		
	} else {
	}
}
*/

// fill an static schedule (static_schedule_by_task_names_t) with the information
// got from the XML file
// returns the number of items (tasks, edges) of the schedule read
unsigned int getStaticSchedule(xmlNodePtr cur, // Scheduler Policy node
								std::vector<std::string> &static_schedule
								) {
//	xmlNodePtr curChildNodePtr;
	param_value_t *param_value;
	std::string item_name_std_str;
	
	// looks either for a "value" attribure with a parameter value "_xxx"
	// or hanging from the own node

	param_value = xmlGetVectorValueGen(cur, (xmlChar *)"value"); 

	for(unsigned int i=0;i<param_value->size();i++) {
		item_name_std_str =(*param_value)[i];
		static_schedule.push_back(item_name_std_str);
	}
	return param_value->size();
}


bool hasStaticScheduling(xmlNodePtr cur // Scheduler Policy node
								) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"static_scheduling"))){
			return true;
		}
		curChildNodePtr=curChildNodePtr->next;
	}
	return false;
}

// get First Task Scheduler
xmlNodePtr getTaskScheduler(xmlNodePtr cur // RTOS node
								) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"task_scheduler"))){
			return curChildNodePtr;
		}
		curChildNodePtr=curChildNodePtr->next;
	}
	return NULL;
}

bool hasTaskScheduler(xmlNodePtr cur //  RTOS node
								) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"task_scheduler"))){
			return true;
		}
		curChildNodePtr=curChildNodePtr->next;
	}
	return false;
}

xmlNodePtr getCommScheduler(xmlNodePtr cur //  RTOS node
								) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"comm_scheduler"))){
			return curChildNodePtr;
		}
		curChildNodePtr=curChildNodePtr->next;
	}
	return NULL;
}

bool hasCommScheduler(xmlNodePtr cur //  RTOS node
								) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"comm_scheduler"))){
			return true;
		}
		curChildNodePtr=curChildNodePtr->next;
	}
	return false;
}


// generic method to get a Static Scheduling node
xmlNodePtr getStaticSchedulingNode(xmlNodePtr cur
								) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"static_scheduling"))){
			return curChildNodePtr;
		}
		curChildNodePtr=curChildNodePtr->next;
	}
	SC_REPORT_ERROR("KisTA-XML","No Static Scheduling found in the RTOS instance.");
	return NULL;
}

// If found, returns the first scheduling policy node described for an RTOS instance node,
// (indeed there should be only one)
//  Note: currently supports just static scheduling policies
xmlNodePtr getSchedulerPolicyNode(xmlDocPtr doc,
								xmlNodePtr cur // task scheduler or comm scheduler node
								) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode; //to iterate over RTOS child elements
	
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"scheduler_policy"))){
			return curChildNodePtr;
		}
		curChildNodePtr = curChildNodePtr->next;
	}
	SC_REPORT_ERROR("KisTA-XML","No Scheduler Policy for RTOS instance.");
	return NULL;
}

// get all processing elements from the platform
xmlNodeSetPtr getRTOS(xmlDocPtr doc) {
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr  XPathObjectPtr;
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)"/system/platform/SW_platform/rtos");
	if(XPathObjectPtr==NULL) {
		SC_REPORT_ERROR("KisTA-XML","SW platform creation failed. No RTOS was read in the XML input.");
	}
	nodeset = XPathObjectPtr->nodesetval;
	return nodeset;
}

unsigned int getNumberOfRTOS(xmlDocPtr doc) {
	xmlNodeSetPtr setRTOSnodes;
	setRTOSnodes=getRTOS(doc);
	return setRTOSnodes->nodeNr;
}


void create_SW_platform(xmlDocPtr doc, std::vector<scheduler*> &RTOS_vec) {
	std::string rpt_msg;
	unsigned int j;
	xmlNodeSetPtr setRTOSnodes;
	xmlChar *RTOSName;
	
	scheduler *RTOS_ptr;
	
	static_schedule_by_task_names_t *static_schedule;

	static_comm_schedule_by_link_names_t *static_comm_schedule;

	xmlNodePtr curr_node;
		
	if(getNumberOfRTOS(doc)<1) {
		SC_REPORT_ERROR("Kista-XML","Platform creation failed. No RTOS was detected.");
	}
	
	setRTOSnodes = getRTOS(doc);
	// setPENodes should not be NULL here because we have already checked that there were nodes in the tree
	
	for(j=0; j< getNumberOfRTOS(doc) ; j++) {
	   
	   RTOSName = xmlGetValueGen(setRTOSnodes->nodeTab[j],(xmlChar *)"name");

       // ---------------------------------------------------------------
       // Parses task scheduling information associated to the RTOS
       // ---------------------------------------------------------------
	   curr_node = getTaskScheduler(setRTOSnodes->nodeTab[j]);
	   
	   if(curr_node==NULL) {
		   rpt_msg = "RTOS ";
		   rpt_msg += (const char *)RTOSName;
		   rpt_msg += " has no task scheduler.";
		   // No task scheduler
		   SC_REPORT_ERROR("KisTA-XML", rpt_msg.c_str());
	   } else {
	   
		   // read name of RTOS if exists. In case there is one in the XML file it creates the new RTOS elements passing such
		   // a name, otherwise, a no-name constructor which generates unique names is used to generate the task scheduler
		   if(RTOSName==NULL) {
				RTOS_ptr = new scheduler;
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {
					printf("kista-XML INFO: Creating scheduler for RTOS instance.");
					printf("No name found in XML file for RTOS instance. KisTA will assign an unique name for the RTOS instance.\n");
				}
#endif
			} else {
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {
					printf("kista-XML INFO: Creating scheduler %s for RTOS instance %s.\n",RTOSName, RTOSName);
				}
#endif	
			   RTOS_ptr = new scheduler((const char *)RTOSName);
			}

			// Set scheduling policy
			curr_node = getSchedulerPolicyNode(doc,curr_node);

			if(hasStaticScheduling(curr_node)) {
				static_schedule = new static_schedule_by_task_names_t;

				// Fixes the policy of the KisTA scheduler as STATIC
				RTOS_ptr->set_policy(STATIC_SCHEDULING);
#ifdef _VERBOSE_KISTA_XML			
				if(global_kista_xml_verbosity) {
					printf("Kista-xml INFO: Fixing static scheduling policy\n");
				}
#endif			
				// get the static schedule from the XML file	
				curr_node = getStaticSchedulingNode(curr_node);
				if( getStaticSchedule(curr_node,*static_schedule) <1) {
					if(global_kista_xml_verbosity) {
						SC_REPORT_WARNING("KisTA-XML","Static scheduling specified no task");
					}
				} else {
					// ASSIGN THE STATIC SCHEDULE to the KisTA scheduler structure
					RTOS_ptr->set_static_schedule(*static_schedule);
#ifdef _VERBOSE_KISTA_XML
					if(global_kista_xml_verbosity) {
					  rpt_msg = "Configured static scheduling [";
					  for(unsigned int i=0;i<static_schedule->size();i++) {
						  rpt_msg += (*static_schedule)[i];
						  if(i<(static_schedule->size()-1)) rpt_msg += ",";
					  }
					  rpt_msg += "] associated to RTOS ";
					  rpt_msg += (const char *)RTOSName;
					  SC_REPORT_INFO("KisTA-XML", rpt_msg.c_str());
					}
#endif
				}
				// For the moment, the plugin states at least N_hyperperiods
				RTOS_ptr->set_sufficient_global_simulation_time(N_hyperperiods_for_sim_time);	
			} else {
				SC_REPORT_ERROR("KisTA-XML","Currently, XML interface of KisTA supports only static scheduling policies");
			}
			
			// Adding the recently created and configured PE to the created HW platform
			RTOS_vec.push_back(RTOS_ptr);
		}
		
	   // -------------------------------------------------------------------
       // Parses communication scheduling information associated to the RTOS
       // -------------------------------------------------------------------
       curr_node = getCommScheduler(setRTOSnodes->nodeTab[j]);
	   
	   if(curr_node==NULL) {
		   if(global_kista_xml_verbosity) {
				rpt_msg = "RTOS ";
				rpt_msg += (const char *)RTOSName;
				rpt_msg += " has no communication scheduler.";
				// No task scheduler (not mandatory, at least from the XML interface)
				SC_REPORT_WARNING("KisTA-XML", rpt_msg.c_str());
			}
	   } else {

			if(global_kista_xml_verbosity) {		   
			   // there is communication scheduling
			   rpt_msg = "RTOS ";
			   rpt_msg += (const char *)RTOSName;
			   rpt_msg += " has communication scheduling.";
			   // No task scheduler (not mandatory, at least from the XML interface)
			   SC_REPORT_INFO("KisTA-XML", rpt_msg.c_str());
			}

		   // reads the communication scheduling policy
           curr_node = getSchedulerPolicyNode(doc,curr_node);

		   if(hasStaticScheduling(curr_node)) {
				static_comm_schedule = new static_comm_schedule_by_link_names_t;

				// get the static schedule from the XML file	
				curr_node = getStaticSchedulingNode(curr_node);
				if(getStaticSchedule(curr_node,*static_comm_schedule) <1) {
#ifdef _XML_IF_EMPTY_STATIC_SCHEDULE_AS_ERROR					
					SC_REPORT_ERROR("KisTA-XML","Static communication scheduling specified no comm_synch element");
#else
					if(global_kista_xml_verbosity) {
						SC_REPORT_WARNING("KisTA-XML","Static communication scheduling specified no comm_synch element");
					}
#endif
				} 

				// Configure (transfer) the static communication scheduling to the scheduler
				// (in turn, the scheduler will transfer it to the network interface)
				RTOS_ptr->set_static_comm_scheduling(*static_comm_schedule);
				
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {
				  rpt_msg = "Static communication scheduling [";
				  for(unsigned int i=0;i<static_comm_schedule->size();i++) {
					  rpt_msg += (*static_comm_schedule)[i];
					  if(i<(static_comm_schedule->size()-1)) rpt_msg += ",";
				  }
				  rpt_msg += "] associated to RTOS ";
				  rpt_msg += (const char *)RTOSName;
		          // No task scheduler (not mandatory, at least from the XML interface)
		          SC_REPORT_INFO("KisTA-XML", rpt_msg.c_str());
				}
#endif

				// For the moment, the plugin states at least N_hyperperiods
				RTOS_ptr->set_sufficient_global_simulation_time(N_hyperperiods_for_sim_time);	
			} else {
				rpt_msg = "Unsupported communication scheduling for RTOS ";
				rpt_msg += (const char *)RTOSName;
				rpt_msg += ". Currently, XML interface of KisTA supports only static communication scheduling policies.";
				SC_REPORT_ERROR("KisTA-XML", rpt_msg.c_str());
			}
			
		}
		
		
	}
	 
}

#endif
