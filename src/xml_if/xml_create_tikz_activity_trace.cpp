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

#define CHILD_CLUSTER_XPATH 			"//cluster"


//
// Helper functions to parse TiKZ trace configuration attributes
//


// detects if the current element has a frequency element
bool has_cluster(xmlDocPtr doc, xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"cluster"))){
			return true;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return false;
}

// gets the start time of the first node whose child is of type start_time
bool get_cluster_value(xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	xmlChar *valueStr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"cluster"))){
			valueStr = xmlGetProp(curChildNodePtr, (const xmlChar *)"value");
			if (!xmlStrcmp(valueStr, (const xmlChar *)"true")) {
				return true;
			} else {
				return false;
			}			
		}
		curChildNodePtr = curChildNodePtr->next;
	}
	// this point should never be reached
	SC_REPORT_ERROR("KisTA-XML","Unexpected error. No cluster found.");
}

bool get_cluster_compact(xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	xmlChar *valueStr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"cluster"))){
			valueStr = xmlGetProp(curChildNodePtr,(const xmlChar *) "compact");
			if (!xmlStrcmp(valueStr, (const xmlChar *)"true")) {
				return true;
			} else {
				return false;
			}			
		}
		curChildNodePtr = curChildNodePtr->next;
	}
	// this point should never be reached
	SC_REPORT_ERROR("KisTA-XML","Unexpected error. No cluster found.");
}

// detects if the current element has a frequency element
bool has_time_stamps_max_separation(xmlDocPtr doc, xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"time_stamps_max_separation"))){
			return true;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return false;
}

// gets the start time of the first node whose child is of type start_time
unsigned int get_time_stamps_max_separation(xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"time_stamps_max_separation"))){
			return readNumber<unsigned int>(curChildNodePtr);
		}
		curChildNodePtr = curChildNodePtr->next;
	}
	// this point should never be reached
	SC_REPORT_ERROR("KisTA-XML","Unexpected error. No time_stamps_max_separation found.");
}


// detects if the current element has a max_number_of_time_stamps element
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

// gets the max_number_of_time_stamps  of the first node whose child is of type max_number_of_time_stamps
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

// detects if the current element has a base_line_separation element
bool has_base_lines_separation(xmlDocPtr doc, xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"base_lines_separation"))){
			return true;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return false;
}

// gets the base line separation value
unsigned int get_base_lines_separation(xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"base_lines_separation"))){
			return readNumber<unsigned int>(curChildNodePtr);
		}
		curChildNodePtr = curChildNodePtr->next;
	}
	// this point should never be reached
	SC_REPORT_ERROR("KisTA-XML","Unexpected error. No base_lines_separation found.");
}

// detects if the current element has a no_text_in_traces element
bool has_no_text_in_traces(xmlDocPtr doc, xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"no_text_in_traces"))){
			return true;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return false;
}

// gets no_text_in_traces flag value
bool get_no_text_in_traces_flag(xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	xmlChar *valueStr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"no_text_in_traces"))){
			valueStr = xmlGetProp(curChildNodePtr, (const xmlChar *)"value");
			if (!xmlStrcmp(valueStr, (const xmlChar *)"true")) {
				return true;
			} else {
				return false;
			}			
		}
		curChildNodePtr = curChildNodePtr->next;
	}
	// this point should never be reached
	SC_REPORT_ERROR("KisTA-XML","Unexpected error. No do_not_show_unactive_boxes found.");
}



// detects if the current element has a do_not_show_unactive_boxes element
bool has_do_not_show_unactive_boxes(xmlDocPtr doc, xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"do_not_show_unactive_boxes"))){
			return true;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return false;
}

// gets do_not_show_unactive_boxes flag value
bool get_do_not_show_unactive_boxes_flag(xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	xmlChar *valueStr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"do_not_show_unactive_boxes"))){
			valueStr = xmlGetProp(curChildNodePtr, (const xmlChar *)"value");
			if (!xmlStrcmp(valueStr, (const xmlChar *)"true")) {
				return true;
			} else {
				return false;
			}			
		}
		curChildNodePtr = curChildNodePtr->next;
	}
	// this point should never be reached
	SC_REPORT_ERROR("KisTA-XML","Unexpected error. No do_not_show_unactive_boxes found.");
}


// detects if the current element has a do_not_show_schedulers element
bool has_do_not_show_schedulers(xmlDocPtr doc, xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"do_not_show_schedulers"))){
			return true;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return false;
}

// gets do_not_show_schedulers flag value
bool get_do_not_show_schedulers_flag(xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	xmlChar *valueStr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"do_not_show_schedulers"))){
			valueStr = xmlGetProp(curChildNodePtr, (const xmlChar *)"value");
			if (!xmlStrcmp(valueStr, (const xmlChar *)"true")) {
				return true;
			} else {
				return false;
			}			
		}
		curChildNodePtr = curChildNodePtr->next;
	}
	// this point should never be reached
	SC_REPORT_ERROR("KisTA-XML","Unexpected error. No do_not_show_schedulers found.");
}

// detects if the current element has a scale element
bool has_set_scale(xmlDocPtr doc, xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"set_scale"))){
			return true;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return false;
}

// gets the scale value
double get_scale(xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"set_scale"))){
			return readRealNumber<double>(curChildNodePtr);
		}
		curChildNodePtr = curChildNodePtr->next;
	}
	// this point should never be reached
	SC_REPORT_ERROR("KisTA-XML","Unexpected error. No set_scale found.");
}

// detects if the current element has a set_landscape element
bool has_set_landscape(xmlDocPtr doc, xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"set_landscape"))){
			return true;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return false;
}

// gets set_landscape flag value
bool get_landscape_flag(xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	xmlChar *valueStr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"set_landscape"))){
			valueStr = xmlGetProp(curChildNodePtr, (const xmlChar *)"value");
			if (!xmlStrcmp(valueStr, (const xmlChar *)"true")) {
				return true;
			} else {
				return false;
			}			
		}
		curChildNodePtr = curChildNodePtr->next;
	}
	// this point should never be reached
	SC_REPORT_ERROR("KisTA-XML","Unexpected error. No set_landscape found.");
}

// detects if the current element has a only_image element
bool has_only_image(xmlDocPtr doc, xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"only_image"))){
			return true;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return false;
}

// gets only_image flag value
bool get_only_image_flag(xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	xmlChar *valueStr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"only_image"))){
			valueStr = xmlGetProp(curChildNodePtr, (const xmlChar *)"value");
			if (!xmlStrcmp(valueStr, (const xmlChar *)"true")) {
				return true;
			} else {
				return false;
			}			
		}
		curChildNodePtr = curChildNodePtr->next;
	}
	// this point should never be reached
	SC_REPORT_ERROR("KisTA-XML","Unexpected error. No only_image found.");
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
	bool		  cluster_flag, compact_flag;
	tikz_activity_trace_handler tikz_handler;
	sc_time 		start_time;
	unsigned int 	max_number_of_time_stamps, time_stamps_max_separation;
	unsigned int	base_lines_separation;
	double			scale_value;
	bool			no_text_in_traces_flag, do_not_show_unactive_boxes_flag,
	                do_not_show_schedulers_flag, set_landscape_flag;
	bool 			only_image_flag;
	
	
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
						rpt_msg += " settled (";
						rpt_msg += std::to_string(DEFAULT_TIKZ_TRACE_EVENTS_LIMIT);
						rpt_msg += ").";
						SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());					
					}
#endif
				}

				// Set time stamps max separation
				if(has_time_stamps_max_separation(doc,setIiKZ_trace_nodes->nodeTab[j])) {
					time_stamps_max_separation = get_time_stamps_max_separation(setIiKZ_trace_nodes->nodeTab[j]);
#ifdef _VERBOSE_KISTA_XML
					if(global_kista_xml_verbosity) {					
						rpt_msg = "TiKZ trace ";
						rpt_msg += (const char *)tikz_trace_name;
						rpt_msg += ": Setting time stamps max. separation to ";
						rpt_msg += std::to_string(time_stamps_max_separation);
						rpt_msg += ".";
						SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
					}
#endif		
					set_time_stamps_max_separation(tikz_handler,time_stamps_max_separation);
				} else {
#ifdef _VERBOSE_KISTA_XML
					if(global_kista_xml_verbosity) {
						rpt_msg = "Default time stamps max. separation of ";
						rpt_msg += (const char *)tikz_trace_name;
						rpt_msg += " settled (";
						rpt_msg += std::to_string(DEFAULT_TIKZ_TIME_STAMPS_MAX_SEPARATION);
						rpt_msg += ").";
						SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());					
					}
#endif
				}
							
				// cluster (if specified) and compact (if specified)
				
				if(has_cluster(doc,setIiKZ_trace_nodes->nodeTab[j])) {
					cluster_flag = get_cluster_value(setIiKZ_trace_nodes->nodeTab[j]);

					if(cluster_flag) {
						// command the effective clustering
						cluster(tikz_handler);
						
						// checks if, as well, the compacting is commanded
						compact_flag = get_cluster_compact(setIiKZ_trace_nodes->nodeTab[j]);
						
						if(compact_flag) {
							compact(tikz_handler);							
						}
					}
					
				}			
				
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {					
					rpt_msg = "TiKZ trace ";
					rpt_msg += (const char *)tikz_trace_name;
					if(cluster_flag) {
						rpt_msg += ": clustered";
						if(compact_flag) {
							rpt_msg += " and compact.";
						} else {
							rpt_msg += ".";
						}
					} else {
						rpt_msg += ": undolded.";
					}
					SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
				}						
#endif


				// Set base lines separation
				if(has_base_lines_separation(doc,setIiKZ_trace_nodes->nodeTab[j])) {
					base_lines_separation = get_base_lines_separation(setIiKZ_trace_nodes->nodeTab[j]);
#ifdef _VERBOSE_KISTA_XML
					if(global_kista_xml_verbosity) {					
						rpt_msg = "TiKZ trace ";
						rpt_msg += (const char *)tikz_trace_name;
						rpt_msg += ": Setting base lines separation ";
						rpt_msg += std::to_string(base_lines_separation);
						rpt_msg += ".";
						SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
					}
#endif		
					set_base_lines_separation(tikz_handler,base_lines_separation);
				} else {
#ifdef _VERBOSE_KISTA_XML
					if(global_kista_xml_verbosity) {
						rpt_msg = "Default base lines separation of ";
						rpt_msg += (const char *)tikz_trace_name;
						rpt_msg += " settled (";
						rpt_msg += std::to_string(DEFAULT_TIKZ_TRACE_BASE_LINES_SEPARATION);
						rpt_msg += ").";
						SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());					
					}
#endif
				}
				
				// no text in traces
				// ----------------------------------------------------------------
				if(has_no_text_in_traces(doc,setIiKZ_trace_nodes->nodeTab[j])) {
					no_text_in_traces_flag = get_no_text_in_traces_flag(setIiKZ_trace_nodes->nodeTab[j]);

					if(no_text_in_traces_flag) {
						// command the effective clustering
						no_text_in_traces(tikz_handler);
					}
					
				}			
				
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {					
					rpt_msg = "TiKZ trace ";
					rpt_msg += (const char *)tikz_trace_name;
					if(no_text_in_traces_flag) {
						rpt_msg += ": no text in traces.";
					} else {
						rpt_msg += ": with text in traces (by default).";
					}
					SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
				}						
#endif

				// do not show unactive boxes
				// ----------------------------------------------------------------
				if(has_do_not_show_unactive_boxes(doc,setIiKZ_trace_nodes->nodeTab[j])) {
					do_not_show_unactive_boxes_flag = get_do_not_show_unactive_boxes_flag(setIiKZ_trace_nodes->nodeTab[j]);

					if(do_not_show_unactive_boxes_flag) {
						// command the effective clustering
						do_not_show_unactive_boxes(tikz_handler);
					}
					
				}			
				
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {					
					rpt_msg = "TiKZ trace ";
					rpt_msg += (const char *)tikz_trace_name;
					if(do_not_show_unactive_boxes_flag) {
						rpt_msg += ": do not show unactive_boxes.";
					} else {
						rpt_msg += ": show unactive boxes (by default).";
					}
					SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
				}						
#endif

				// do not show schedulers
				// ----------------------------------------------------------------
				if(has_do_not_show_schedulers(doc,setIiKZ_trace_nodes->nodeTab[j])) {
					do_not_show_schedulers_flag = get_do_not_show_schedulers_flag(setIiKZ_trace_nodes->nodeTab[j]);

					if(do_not_show_schedulers_flag) {
						// command the effective clustering
						do_not_show_schedulers(tikz_handler);
					}
					
				}			
				
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {					
					rpt_msg = "TiKZ trace ";
					rpt_msg += (const char *)tikz_trace_name;
					if(do_not_show_schedulers_flag) {
						rpt_msg += ": do not show schedulers.";
					} else {
						rpt_msg += ": show schedulers (by default).";
					}
					SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
				}						
#endif

				// Set scale
				// -------------------------------
				if(has_set_scale(doc,setIiKZ_trace_nodes->nodeTab[j])) {
					scale_value = get_scale(setIiKZ_trace_nodes->nodeTab[j]);
#ifdef _VERBOSE_KISTA_XML
					if(global_kista_xml_verbosity) {					
						rpt_msg = "TiKZ trace ";
						rpt_msg += (const char *)tikz_trace_name;
						rpt_msg += ": Setting scale to ";
						rpt_msg += std::to_string(scale_value);
						rpt_msg += ".";
						SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
					}
#endif		
					set_scale(tikz_handler,scale_value);
				} else {
#ifdef _VERBOSE_KISTA_XML
					if(global_kista_xml_verbosity) {
						rpt_msg = "Default scale for TiKz trace ";
						rpt_msg += (const char *)tikz_trace_name;
						rpt_msg += " settled (";
						rpt_msg += std::to_string(DEFAULT_TIKZ_TRACE_SCALE);
						rpt_msg += ").";
						SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());					
					}
#endif
				}
				
				
				// set landscape
				// ----------------------------------------------------------------
				if(has_set_landscape(doc,setIiKZ_trace_nodes->nodeTab[j])) {
					set_landscape_flag = get_landscape_flag(setIiKZ_trace_nodes->nodeTab[j]);

					if(set_landscape_flag) {
						// command the effective clustering
						set_landscape(tikz_handler);
					}
					
				}			
				
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {					
					rpt_msg = "TiKZ trace ";
					rpt_msg += (const char *)tikz_trace_name;
					if(set_landscape_flag) {
						rpt_msg += ": landscape format.";
					} else {
						rpt_msg += ": portrait format (by default).";
					}
					SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
				}						
#endif

				// only image
				// ----------------------------------------------------------------
				if(has_only_image(doc,setIiKZ_trace_nodes->nodeTab[j])) {
					only_image_flag = get_only_image_flag(setIiKZ_trace_nodes->nodeTab[j]);

					if(only_image_flag) {
						// command the only image
						only_image(tikz_handler);
					}
					
				}			
				
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {					
					rpt_msg = "TiKZ trace ";
					rpt_msg += (const char *)tikz_trace_name;
					if(only_image_flag) {
						rpt_msg += ": only image will be exported.";
					} else {
						rpt_msg += ": header, image, time spans and time spans (by default).";
					}
					SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
				}						
#endif

		   } // end create tikz trace
		   		
		}
		
		return true;
	} else 
	{
		return false;
	}
	
	
	 
}

#endif
