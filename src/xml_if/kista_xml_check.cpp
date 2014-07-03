/*****************************************************************************

  kista_xml_check.hpp
  
   Program for traversing the XML file, dump it to console and to perform some checks
   (as well as calling to the validation vs the schema)
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 July

 *****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <libxml/xmlschemas.h>

#include <vector>

#define _VERBOSE_KISTA_XML


void
parseTask (xmlDocPtr doc, xmlNodePtr cur_elem_ptr) {

	xmlChar *TaskName;

	TaskName = xmlGetProp(cur_elem_ptr, (const xmlChar *)"name");
#ifdef _VERBOSE_KISTA_XML
		    printf("Kista-xml INFO: Task name: %s\n", TaskName);
#endif		    

/*		
	cur_elem_ptr = cur_elem_ptr->xmlChildrenNode;
	while (cur_elem_ptr != NULL) {
	    if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"OTHER TYPE OF TASK INFO"))) {
		    TaskName = xmlNodeListGetString(doc, cur_elem_ptr->xmlChildrenNode, 1);
		    xmlFree(TaskName);
 	    }
	    cur_elem_ptr = cur_elem_ptr->next;
	}
*/
}

void
parseApplication (xmlDocPtr doc, xmlNodePtr cur_elem_ptr) {

	xmlChar *AppName;
	
	AppName = xmlGetProp(cur_elem_ptr, (const xmlChar *)"name");
#ifdef _VERBOSE_KISTA_XML			
	printf("Kista-xml INFO: Found Application %s\n", AppName);
#endif	
	xmlFree(AppName);
	
	cur_elem_ptr = cur_elem_ptr->xmlChildrenNode;
	while (cur_elem_ptr != NULL) {
 	    if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"task"))) {
			parseTask (doc, cur_elem_ptr);
		}
	    cur_elem_ptr = cur_elem_ptr->next;
	}
    return;
}

void
parseApplicationMapping (xmlDocPtr doc, xmlNodePtr cur_elem_ptr) {
	xmlChar *sourceAppName;
	xmlChar *sourceTaskName;	
	xmlChar *targetName;
	
	cur_elem_ptr = cur_elem_ptr->xmlChildrenNode;
	
	while (cur_elem_ptr != NULL) {
		
	    if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"map_task"))) {
			sourceAppName = xmlGetProp(cur_elem_ptr, (const xmlChar *)"source_app");
			sourceTaskName = xmlGetProp(cur_elem_ptr, (const xmlChar *)"source_task");			
			targetName = xmlGetProp(cur_elem_ptr, (const xmlChar *)"target");
			if(sourceAppName==NULL) {
				printf("Kista-xml ERROR: No source node in application mapping\n");
				exit(-1);
			}
			if(sourceTaskName==NULL) {
				printf("Kista-xml ERROR: No source node in application mapping\n");
				exit(-1);
			}			
			if(targetName==NULL) {
				printf("Kista-xml ERROR: No target node in application mapping\n");
				exit(-1);
			}
			
#ifdef _VERBOSE_KISTA_XML			
			printf("Kista-xml INFO: Task Map: %s.%s -> %s\n", sourceAppName, sourceTaskName, targetName);
#endif			
			xmlFree(sourceAppName);
			xmlFree(sourceTaskName);			
			xmlFree(targetName);
 	    }
	    cur_elem_ptr = cur_elem_ptr->next;
	}
}


void
parseStaticSchedule (xmlDocPtr doc, xmlNodePtr cur_elem_ptr) {
	


}

void
parseStaticSchedulingPolicy (xmlDocPtr doc, xmlNodePtr cur_elem_ptr) {
	
	bool Static_Schedule_stated = false;
	bool Static_Cyclic_Schedule_stated = false;
	
	// variables for the static schedule definition
	std::vector<xmlChar *> static_schedule;
				
	cur_elem_ptr = cur_elem_ptr->xmlChildrenNode;	
	while (cur_elem_ptr != NULL) {		
		if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"task"))) {			
			static_schedule.push_back( xmlGetProp(cur_elem_ptr, (const xmlChar *)"name") );
			Static_Schedule_stated = true;
		}
		
		if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"cyclic"))) { // ciclic around a set of tasks, so it must address the task set
			printf("Kista-xml ERROR: Cyclic schedule specification TO BE SUPPORTED. You can however use tasks entries to specifiy it by now.");
			exit(-1);
			Static_Cyclic_Schedule_stated = true;
		}

	    cur_elem_ptr = cur_elem_ptr->next;
	}
	
	if(Static_Schedule_stated && Static_Cyclic_Schedule_stated) {
		printf("Kista-xml ERROR: Both, a static schedule and a cyclic static schedule have been stated. Only should be specified.\n");
	}
	
	if(!Static_Schedule_stated && !Static_Cyclic_Schedule_stated) {
		printf("Kista-xml ERROR:Static schedule has not been precised.\n");
		printf("At least one of the following tags has to appear in the XML file, hanging from the <static_scheduling> tag:\n");
		printf("A set of <task> tags, which define the schedule, or a <cyclic> stating the task set.\n");
		exit(-1);
	}

	if(Static_Schedule_stated) {
#ifdef _VERBOSE_KISTA_XML			
		printf("Static Schedule: {");
		for(unsigned int i=0;i<static_schedule.size();i++) {
			printf("%s", static_schedule[i]);
			if(i==(static_schedule.size()-1)) printf("}\n");
			else printf(",");
		}
#endif
	}
	
}

void
parseDynamicSchedulingPolicy (xmlDocPtr doc, xmlNodePtr cur_elem_ptr) {
	
}

void
parseSchedPolicy (xmlDocPtr doc, xmlNodePtr cur_elem_ptr) {
	
	bool static_policy_stated = false;
	bool dynamic_policy_stated = false;
	
	cur_elem_ptr = cur_elem_ptr->xmlChildrenNode;	
	while (cur_elem_ptr != NULL) {
		if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"static_scheduling"))) {					
			parseStaticSchedulingPolicy(doc, cur_elem_ptr);
			static_policy_stated = true;
		}
		
		if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"dynamic_scheduling"))) {
			parseDynamicSchedulingPolicy(doc, cur_elem_ptr);
			dynamic_policy_stated = true;
		}

	    cur_elem_ptr = cur_elem_ptr->next;
	}
	
	if(static_policy_stated && dynamic_policy_stated) {
		printf("Kista-xml ERROR: Both, static and dynamic policies have been stated\n");
	}
	
	if(!static_policy_stated && !dynamic_policy_stated) {
		printf("Kista-xml ERROR: Scheduler Policy has not been specifically stated in the XML file\n");
		printf("At least one of the following tags has to appear in the XML file, hanging from the <scheduler_policy> tag:\n");
		printf("static_scheduling or dynamic_scheduling\n");
		exit(-1);
	}
	
}

void
parseRTOS (xmlDocPtr doc, xmlNodePtr cur_elem_ptr) {

	xmlChar *RTOSinstanceName;
	
	bool found_scheduler_policy = false;
	
	RTOSinstanceName = xmlGetProp(cur_elem_ptr, (const xmlChar *)"name");
#ifdef _VERBOSE_KISTA_XML			
	printf("Kista-xml INFO: (RTOS) id: %s\n", RTOSinstanceName);
#endif	
	
	cur_elem_ptr = cur_elem_ptr->xmlChildrenNode;	
	while (cur_elem_ptr != NULL) {
		if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"scheduler_policy"))) {			
			parseSchedPolicy(doc, cur_elem_ptr);
			found_scheduler_policy = true;
		}
	    cur_elem_ptr = cur_elem_ptr->next;
	}
	
	if(!found_scheduler_policy) {
		printf("Kista-xml ERROR: Scheduler Policy not defined in the XML file for RTOS %s\n", RTOSinstanceName);
		exit(-1);
	}
	
	xmlFree(RTOSinstanceName);
}

void
parseSWPlatform (xmlDocPtr doc, xmlNodePtr cur_elem_ptr) {
	unsigned int found_RTOS = 0;
	bool right_SW_platform = false;

	cur_elem_ptr = cur_elem_ptr->xmlChildrenNode;
	
	while (cur_elem_ptr != NULL) {
	    if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"rtos"))) {
#ifdef _VERBOSE_KISTA_XML
			printf("Kista-xml INFO: Found RTOS.\n");
#endif			
			parseRTOS(doc, cur_elem_ptr);
			found_RTOS++;
		}

	    cur_elem_ptr = cur_elem_ptr->next;
	}
	
	right_SW_platform = found_RTOS>0;
	
	if(!right_SW_platform) {
			printf("Kista-xml WARNING: Error in the SW platform.\n");
			if(found_RTOS==0) {
				printf("\t\tNo RTOS.");		
			}
	}	
}


void
parsePE (xmlDocPtr doc, xmlNodePtr cur_elem_ptr) {
	xmlChar *PEName;
	xmlChar *PEType;
	xmlChar *freq_sval;

	PEName = xmlGetProp(cur_elem_ptr, (const xmlChar *)"name");
#ifdef _VERBOSE_KISTA_XML			
			printf("Kista-xml INFO: (PE) id=%s\n", PEName);
#endif			
	xmlFree(PEName);
				
	PEType = xmlGetProp(cur_elem_ptr, (const xmlChar *)"type");
#ifdef _VERBOSE_KISTA_XML			
			printf("Kista-xml INFO: (PE) type=%s\n", PEType);
#endif			
	xmlFree(PEType);				
				
	cur_elem_ptr = cur_elem_ptr->xmlChildrenNode;
	
	while (cur_elem_ptr != NULL) {
		
		if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"freq"))) {
			freq_sval = xmlGetProp(cur_elem_ptr, (const xmlChar *)"value");
#ifdef _VERBOSE_KISTA_XML			
			printf("Kista-xml INFO: (PE) freq. value=%s\n", freq_sval);
#endif			
			xmlFree(freq_sval);
		}
	    cur_elem_ptr = cur_elem_ptr->next;
	}		

}

void
parseHWPlatform (xmlDocPtr doc, xmlNodePtr cur_elem_ptr) {
	unsigned int found_PEs = 0;
	bool right_HW_platform = false;

	cur_elem_ptr = cur_elem_ptr->xmlChildrenNode;
	
	while (cur_elem_ptr != NULL) {
	    if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"PE"))) {
#ifdef _VERBOSE_KISTA_XML
			printf("Kista-xml INFO: Found PE.\n");
#endif			
			parsePE(doc, cur_elem_ptr);
			found_PEs++;
		}

	    cur_elem_ptr = cur_elem_ptr->next;
	}
	
	right_HW_platform = found_PEs>0;
	
	if(!right_HW_platform) {
			printf("Kista-xml WARNING: Error in the HW platform.\n");
			if(found_PEs==0) {
				printf("\t\tNo Processing Elements.");		
			}
	}
}

void
parsePlatformMapping (xmlDocPtr doc, xmlNodePtr cur_elem_ptr) {
	xmlChar *sourceName;
	xmlChar *targetName;

	cur_elem_ptr = cur_elem_ptr->xmlChildrenNode;
	
	while (cur_elem_ptr != NULL) {
		
	    if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"map_rtos"))) {
			sourceName = xmlGetProp(cur_elem_ptr, (const xmlChar *)"source");
			targetName = xmlGetProp(cur_elem_ptr, (const xmlChar *)"target");
			if(sourceName==NULL) {
				printf("Kista-xml ERROR: No source node in platform mapping\n");
				exit(-1);
			}
			if(targetName==NULL) {
				printf("Kista-xml ERROR: No target node in platform mapping\n");
				exit(-1);
			}
#ifdef _VERBOSE_KISTA_XML			
			printf("Kista-xml INFO: Platform Map: %s -> %s\n", sourceName, targetName);
#endif			
			xmlFree(sourceName);
			xmlFree(targetName);
 	    }
	    cur_elem_ptr = cur_elem_ptr->next;
	}	
}

void
parsePlatform (xmlDocPtr doc, xmlNodePtr cur_elem_ptr) {

	unsigned int found_HW_platform = 0;
	unsigned int found_SW_platform = 0;
	unsigned int found_platform_mapping = 0;
	bool right_platform = false;

	cur_elem_ptr = cur_elem_ptr->xmlChildrenNode;
	
	while (cur_elem_ptr != NULL) {
	    if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"SW_platform"))) {
#ifdef _VERBOSE_KISTA_XML
			printf("Kista-xml INFO: Found SW platform.\n");
#endif			
			parseSWPlatform(doc, cur_elem_ptr);
			found_SW_platform++;
		}

	    if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"HW_platform"))) {
#ifdef _VERBOSE_KISTA_XML
			printf("Kista-xml INFO: Found HW platform.\n");
#endif			
			parseHWPlatform(doc, cur_elem_ptr);
			found_HW_platform++;
		}

	    if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"platform_mapping"))) {
#ifdef _VERBOSE_KISTA_XML
			printf("Kista-xml INFO: Found SW to HW platform mapping.\n");
#endif			
			parsePlatformMapping(doc, cur_elem_ptr);
			found_platform_mapping++;
		}
	    cur_elem_ptr = cur_elem_ptr->next;
	}
	if(found_SW_platform>1) {
			printf("Kista-xml WARNING: Found more than one SW platform. Current Kista-xml version will only take one into account.\n");		
	}
	if(found_HW_platform>1) {
			printf("Kista-xml WARNING: Found more than one HW platform. Current Kista-xml version will only take one into account.\n");		
	}
	if(found_platform_mapping>1) {
			printf("Kista-xml WARNING: Found more than one platform mapping. Current Kista-xml version will only take one into account.\n");
	}
	
	right_platform = (found_HW_platform>0) && (found_HW_platform>0) && (found_HW_platform>0);
	if(!right_platform) {
			printf("Kista-xml WARNING: Error parsing the platform.\n");
			if(found_SW_platform==0) {
				printf("\t\tSW platform is missing.");		
			}
			if(found_HW_platform==0) {
				printf("\t\tHW platform is missing.");		
			}
			if(found_platform_mapping==0) {
				printf("\t\tPlatform mapping is missing.");
			}					
	}
}

static void
parseXMLfile(char *docname) {

	xmlDocPtr doc;
	xmlNodePtr cur_elem_ptr;

	doc = xmlParseFile(docname);
	
	if (doc == NULL ) {
		fprintf(stderr,"KisTa ERROR: XML file parsing error. \n");
		return;
	}
	
	cur_elem_ptr = xmlDocGetRootElement(doc);
	
	if (cur_elem_ptr == NULL) {
		fprintf(stderr,"Kista ERROR: Root element not found, empty XML file\n");
		xmlFreeDoc(doc);
		return;
	}
	
	if (xmlStrcmp(cur_elem_ptr->name, (const xmlChar *) "system")) {
		fprintf(stderr,"Kista ERROR: root element is not of -system- type");
		xmlFreeDoc(doc);
		return;
	}
	
	// Now look for the nodes hanging from the root element and parse them
	cur_elem_ptr = cur_elem_ptr->xmlChildrenNode;
	while (cur_elem_ptr != NULL) {
		if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"application"))){
			parseApplication (doc, cur_elem_ptr);
		}

		if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"application_mapping"))){
			parseApplicationMapping (doc, cur_elem_ptr);
		}
		
		if ((!xmlStrcmp(cur_elem_ptr->name, (const xmlChar *)"platform"))){
			parsePlatform (doc, cur_elem_ptr);
		}
		 
		cur_elem_ptr = cur_elem_ptr->next;
	}
	
	xmlFreeDoc(doc);
	return;
}

int
main(int argc, char **argv) {

	char *xmlFileName;
		
	if (argc <= 1) {
		printf("Usage: %s <XML file>\n", argv[0]);
		return(0);
	}

	xmlFileName = argv[1];
	
	// TBC: a call for validating vs the schema
	
	parseXMLfile (xmlFileName);

	return (1);
}


