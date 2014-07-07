/*****************************************************************************

  xml_create_tikz_activity_trace.cpp
  
   Functions for configuring the KisTA TiKZ trace from the kista-xml front-end.
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 June

 *****************************************************************************/

#ifndef _CREATE_TIKZ_ACTIVITY_TRACE_CPP
#define _CREATE_TIKZ_ACTIVITY_TRACE_CPP


#define TIKZ_TRACE_XPATH "/system/kista_configuration/analysis/tikz_trace"

// relative search
#define CHILD_START_TIME_XPATH 			"//start_time"
#define CHILD_MAX_NUMBER_OF_TIME_STAMPS "//max_number_of_time_stamps"

//
// Helper functions to parse TiKZ trace configuration attributes
//

// detects if the current element has a frequency element
bool has_max_number_of_time_stamps(xmlDocPtr doc, xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"max_number_of_time_stamps"))){
			return true;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return false;
}

// gets the start time of the first node whose child is of type start_time
unsigned int get_max_number_of_time_stamps(xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"max_number_of_time_stamps"))){
			return readNumber<unsigned int>(curChildNodePtr);
		}
		curChildNodePtr = curChildNodePtr->next;
	}
	// this point should never be reached
	SC_REPORT_ERROR("KisTA-XML","Unexpected error. No max_number_of_time_stamps found.");
}


// detects if the current element has a frequency element
bool has_start_time(xmlDocPtr doc, xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"start_time"))){
			return true;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return false;
}

// gets the start time of the first node whose child is of type start_time
sc_time get_start_time(xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"start_time"))){
			return readTime(curChildNodePtr);
		}
		curChildNodePtr = curChildNodePtr->next;
	}
	// this point should never be reached
	SC_REPORT_ERROR("KisTA-XML","Unexpected error. No start_time found.");
}

//
// Top Helper functions to parse tikz traces
//

bool ThereIsTiKZtraces(xmlDocPtr doc) {
	xmlXPathObjectPtr  XPathObjectPtr;
	XPathObjectPtr=getNodeSet(doc,(xmlChar *)TIKZ_TRACE_XPATH);
	if(XPathObjectPtr==NULL) return false;
	else return true;
}

// get all processing elements from the platform
xmlNodeSetPtr getTiKZtraces(xmlDocPtr doc) {
	xmlNodeSetPtr nodeset;	
	xmlXPathObjectPtr  XPathObjectPtr;
	XPathObjectPtr=getNodeSet(doc,(xmlChar *)TIKZ_TRACE_XPATH);
	nodeset = XPathObjectPtr->nodesetval;
	return nodeset;
}

unsigned int getNumberOfTiKZtraces(xmlDocPtr doc) {
	xmlNodeSetPtr setPEnodes;
	setPEnodes=getTiKZtraces(doc);
	return setPEnodes->nodeNr;
}

// --------------------------------------
// CREATE TIKZ ACTIVITY TRACE
// --------------------------------------

bool xml_create_tikz_activity_trace(xmlDocPtr doc) {
	
	unsigned int j;
	std::string rpt_msg;
	
	// variables for TiKZ traces nodes
	xmlNodeSetPtr setIiKZ_trace_nodes;
	xmlChar 	  *xmlCharValue, *tikz_trace_name;
	tikz_activity_trace_handler tikz_handler;
	sc_time 		start_time;
	unsigned int 	max_number_of_time_stamps;
	
	
	if(ThereIsTiKZtraces(doc)) {
		setIiKZ_trace_nodes = getTiKZtraces(doc);
		// setIiKZ_trace_nodes should not be NULL here because we have already checked that there were nodes in the tree
		// Notice that we assume that the user can configure several traces, e.g. focused in different points of the model
		
		for(j=0; j< getNumberOfTiKZtraces(doc) ; j++) {
		   
		   xmlCharValue = xmlGetValueGen(setIiKZ_trace_nodes->nodeTab[j],(xmlChar *)"enabled");
		   		   
		   if( (xmlCharValue==NULL) || (strcmp((const char *)xmlCharValue,"true")) ) {
			   // If there is not value, then the trace is skipped
			   break;
		   } else {
			   // CREATE TIKZ TRACE
			   // only if the enabled attributte is settled, the TiKZ trace is settled 
			   tikz_trace_name = xmlGetValueGen(setIiKZ_trace_nodes->nodeTab[j],(xmlChar *)"name");
		   
			   // read name of PE i exists. In case there is one in the XML file it creates the new PE elements passigng such
			   // a name, otherwise, a no-name constructor which generates unique names is created
			   if(tikz_trace_name==NULL) {
				   tikz_handler = create_tikz_activity_trace();
#ifdef _VERBOSE_KISTA_XML
					if(global_kista_xml_verbosity) {
						rpt_msg = "Creating TiKZ activity trace.";
						rpt_msg += "No name found in XML file, KisTA assigns the unique name:";
						rpt_msg += tikz_handler->name();
						SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
					}
#endif
				} else {
#ifdef _VERBOSE_KISTA_XML
					if(global_kista_xml_verbosity) {
						rpt_msg = "Creating TiKZ activity trace ";
						rpt_msg += (const char *)tikz_trace_name;
						SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
					}
#endif	
					tikz_handler = create_tikz_activity_trace((const char *)tikz_trace_name);
				}
				
			   // CONFIGURE THE TIKZ TRACE
				
				// Set start time
				if(has_start_time(doc,setIiKZ_trace_nodes->nodeTab[j])) {					
					start_time = get_start_time(setIiKZ_trace_nodes->nodeTab[j]);
#ifdef _VERBOSE_KISTA_XML
					if(global_kista_xml_verbosity) {					
						rpt_msg = "TiKZ trace ";
						rpt_msg += (const char *)tikz_trace_name;
						rpt_msg += ": Setting start time to ";
						rpt_msg += start_time.to_string();
						rpt_msg += ".";
						SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
					}
#endif		
					set_start_time(tikz_handler,start_time);
				} else {
#ifdef _VERBOSE_KISTA_XML
					if(global_kista_xml_verbosity) {
						rpt_msg = "Default start time for TiKz trace ";
						rpt_msg += (const char *)tikz_trace_name;
						rpt_msg += " settled (0s).";
						SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());					
					}
#endif
				}

				// Set max number of time stamps
				if(has_max_number_of_time_stamps(doc,setIiKZ_trace_nodes->nodeTab[j])) {
					max_number_of_time_stamps = get_max_number_of_time_stamps(setIiKZ_trace_nodes->nodeTab[j]);
#ifdef _VERBOSE_KISTA_XML
					if(global_kista_xml_verbosity) {					
						rpt_msg = "TiKZ trace ";
						rpt_msg += (const char *)tikz_trace_name;
						rpt_msg += ": Setting max. number of time stamps to ";
						rpt_msg += std::to_string(max_number_of_time_stamps);
						rpt_msg += ".";
						SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
					}
#endif		
					set_max_number_of_time_stamps(tikz_handler,max_number_of_time_stamps);
				} else {
#ifdef _VERBOSE_KISTA_XML
					if(global_kista_xml_verbosity) {
						rpt_msg = "Default max. number of time stamps for TiKz trace ";
						rpt_msg += (const char *)tikz_trace_name;
						rpt_msg += " settled (15).";
						SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());					
					}
#endif
				}
		   }		
		}
		return true;
	} else 
	{
		return false;
	}
	
	
	 
}

#endif
