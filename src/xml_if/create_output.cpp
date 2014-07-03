/*****************************************************************************

  create_output.cpp
  
   For generating the output report file in XML format
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 July

 *****************************************************************************/

#ifndef	_CREATE_OUTPUT_CPP
#define _CREATE_OUTPUT_CPP

// getSelectedOutputMetrics, implementation similar to unfold getParameterConfigurations, read_sysconf_parameters in read_sysconf_parameters.cpp

// Either, the to_string C++ function or the sprintf function limit to 6 characters the number of degimal digits,
// so if thevalue is so litle, the value will be 0.000000
// In order to avoid that, the value_to_string function is used.
// Such a function uses de decimal representation only if the value is over the threshold.
// To be effective, the threshold should be 0.0000001 or more. 

// treshold to ensure at least 4 representative decimal digits
#define THRESHOLD_FOR_SCIENTIFIC_NOTATION 0.001
#define VALUE_LENGTH_FOR_SCIENTIFIC_NOTATION 11

char *ext_to_string(double value) {
	char *str;
	int sprintf_ret;
	std::string msg;
	if ((value!=0.0) && (value<THRESHOLD_FOR_SCIENTIFIC_NOTATION)) {
		str = new char[VALUE_LENGTH_FOR_SCIENTIFIC_NOTATION];
		sprintf_ret = sprintf(str, "%e", value);
		if( sprintf_ret < 0 ) {
			msg = "Problem in conversion of double value into scientific format.";
			SC_REPORT_ERROR("KisTA-XML",msg.c_str());
		}
		return str;
	} else {
		//sprintf(str, "%f", value);
		return (char *)std::to_string(value).c_str(); // generate object 
	}
}


// metric definition map
typedef std::map<const char*, system_metric_definition_t *, const_char_cmp> metric_def_t;

//
// This method is used to parse the system metrics definition file and dump it to the 
// metric definitions global structure
//
void getSelectedOutputMetrics(char *smd_file_name,
							   metric_def_t &selected_ometrics) {
	int i;
	// First thing done is to open, parse and extact from smd_file the set of metrics to be reported
	xmlDocPtr doc; // handler
	
	xmlNodeSetPtr OM_nodeset;	 // node set object for the expression (selected parameters)
	xmlXPathObjectPtr  XPathObjectPtr; // path for the selection of the node set (selected parameters)

	xmlChar *metricName;
	xmlChar *metricUnit;
	
	system_metric_definition_t *smd_ptr;
			
	std::string rpt_msg;
	std::string curr_metric;
	
	// parse system metrics definition file
	doc = xmlParseFile(smd_file_name);
	
	if (doc == NULL ) {
		rpt_msg = "While parsing the System Metric Definition file: ";
		rpt_msg += smd_file_name;
		rpt_msg += ".";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}

	// get the sets of system metrics from the smd xml file
	// XPathObjectPtr = getNodeSet(doc,(xmlChar *)"/design_space/system_metrics/system_metric");
	
	// To support default www.multicube.eu namespace
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)"/*[local-name()='design_space']/*[local-name()='system_metrics']/*[local-name()='system_metric']");
	
	if(XPathObjectPtr==NULL) {
		SC_REPORT_ERROR("Kista-XML","No metrics were defined for the output");
	}
	
	OM_nodeset = XPathObjectPtr->nodesetval;

	for(i=0; i< OM_nodeset->nodeNr ; i++) {
		   metricName = xmlGetProp(OM_nodeset->nodeTab[i],(xmlChar *)"name");
		   
		   if(metricName==NULL) {
			   rpt_msg = "The ";
			   rpt_msg += to_string(i);
			   rpt_msg += "-th output system metric definition does not have parameter name.";
               SC_REPORT_ERROR("kista-XML",rpt_msg.c_str());
		   }
		   
		   smd_ptr = new system_metric_definition_t;
		   
		   smd_ptr->name = (char *)metricName;
			   
		   metricUnit = xmlGetProp(OM_nodeset->nodeTab[i],(xmlChar *)"unit");
		   
		   if(metricUnit==NULL) {		
				if(global_kista_xml_verbosity) {
					rpt_msg = "Metric ";
					rpt_msg += (char *)metricName;
					rpt_msg += " definition contains no unit. The reference default unit is assumed";
					SC_REPORT_WARNING("KisTA-XML",rpt_msg.c_str());
				}
			   smd_ptr->unit = NULL;
		   } else {
			   smd_ptr->unit = (char *)metricUnit;
		   }

		   // add the parameter value
		   selected_ometrics[smd_ptr->name] = smd_ptr;		
	}
	
}

//  ---------------------------------------
//  OUTPUT UNDER MULTICUBE FORMAT
//  ---------------------------------------

// defines for the Multicube format
#define OUTPUT_ROOT_NODE_NAME "simulator_output_interface"

// this follows the more constrained style compatible with Multicube

//
// Following function generates an XML file with all the output metrics, but the throughputs
// (there are 8 throughput metric per each edge and the output can be massive)
//

void create_MULTICUBE_full_output(char *xml_out_file, vector<scheduler*> &RTOS, vector<task_info_t*>	&app_task) {

	xmlDocPtr doc = NULL;       		// document pointer
    xmlNodePtr root_node, node = NULL;	// node pointers
//    xmlDtdPtr dtd = NULL;      			// DTD pointer

	std::string metric_name_base,metric_name;

	// temporal variables for
		// reported metrics
	double double_value;
		// value string
	char  *value_string;
		
    doc = xmlNewDoc((xmlChar *)"1.0");
    root_node = xmlNewNode(NULL, (xmlChar *)OUTPUT_ROOT_NODE_NAME);
    xmlNewProp(root_node,(xmlChar *)"xmlns",(xmlChar *)"http://www.multicube.eu/");
    xmlNewProp(root_node,(xmlChar *)"version",(xmlChar *)"1.4");
    xmlDocSetRootElement(doc, root_node);
    
    //
    // SW platform related metrics
    //
    
    for(unsigned int i=0; i<RTOS.size(); i++) {
		
		metric_name_base = "rtos/";
		metric_name_base += RTOS[i]->name();
		metric_name_base += "/";
				
		node = xmlNewChild(root_node, NULL, (xmlChar *)"system_metric", NULL);
		metric_name = metric_name_base + "tasks_utilization";
		xmlNewProp(node,(xmlChar *)"name", (xmlChar *)metric_name.c_str());	
		double_value = RTOS[i]->get_tasks_utilization();
		value_string = ext_to_string(double_value);
		xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);

		node = xmlNewChild(root_node, NULL, (xmlChar *)"system_metric", NULL);
		metric_name = metric_name_base + "sched_utilization";
		xmlNewProp(node,(xmlChar *)"name", (xmlChar *)metric_name.c_str());	
		double_value = RTOS[i]->get_scheduler_utilization();
		value_string = ext_to_string(double_value);
		xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
		
		node = xmlNewChild(root_node, NULL, (xmlChar *)"system_metric", NULL);
		metric_name = metric_name_base + "plat_utilization";
		xmlNewProp(node,(xmlChar *)"name", (xmlChar *)metric_name.c_str());	
		double_value = RTOS[i]->get_platform_utilization();
		value_string = ext_to_string(double_value);
		xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
		
		node = xmlNewChild(root_node, NULL, (xmlChar *)"system_metric", NULL);
		metric_name = metric_name_base + "number_of_schedulings";
		xmlNewProp(node,(xmlChar *)"name", (xmlChar *)metric_name.c_str());
		xmlNewProp(node,(xmlChar *)"value", (xmlChar *)to_string(RTOS[i]->get_number_of_schedulings()).c_str());

		node = xmlNewChild(root_node, NULL, (xmlChar *)"system_metric", NULL);
		metric_name = metric_name_base + "number_of_context_switches";
		xmlNewProp(node,(xmlChar *)"name", (xmlChar *)metric_name.c_str());
		xmlNewProp(node,(xmlChar *)"value", (xmlChar *)to_string(RTOS[i]->get_number_of_context_switches()).c_str());	

		node = xmlNewChild(root_node, NULL, (xmlChar *)"system_metric", NULL);
		metric_name = metric_name_base + "starvation";
		xmlNewProp(node,(xmlChar *)"name", (xmlChar *)metric_name.c_str());
		xmlNewProp(node,(xmlChar *)"value", (xmlChar *)(RTOS[i]->assess_starvation()?"true":"false"));
		
		node = xmlNewChild(root_node, NULL, (xmlChar *)"system_metric", NULL);
		metric_name = metric_name_base + "miss_deadlines";
		xmlNewProp(node,(xmlChar *)"name", (xmlChar *)metric_name.c_str());
		xmlNewProp(node,(xmlChar *)"value", (xmlChar *)(RTOS[i]->miss_deadlines()?"true":"false"));
    }

	//
	// APPLICATION RELATED METRICS:
	//
	
	//-----------------------------------
	// application tasks related metrics
	// ----------------------------------
    for(unsigned int i=0; i<app_task.size(); i++) {
		
		metric_name_base = "task/";
		metric_name_base += app_task[i]->name();
		metric_name_base += "/";
		
		node = xmlNewChild(root_node, NULL, (xmlChar *)"system_metric", NULL);
		metric_name = metric_name_base + "utilization";
		xmlNewProp(node,(xmlChar *)"name", (xmlChar *)metric_name.c_str());
		double_value = app_task[i]->get_scheduler()->get_task_utilization(app_task[i]->name());
		value_string = ext_to_string(double_value);
		xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);

		if(app_task[i]->is_periodic()) {
			// The following metrics are only supported if the task is periodic
			node = xmlNewChild(root_node, NULL, (xmlChar *)"system_metric", NULL);
			metric_name = metric_name_base + "max_response_time";
			xmlNewProp(node,(xmlChar *)"name", (xmlChar *)metric_name.c_str());	
			double_value = app_task[i]->max_response_time().to_seconds();
			value_string = ext_to_string(double_value);
			xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
			xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)"s");

			node = xmlNewChild(root_node, NULL, (xmlChar *)"location_of_worst_case_response_time", NULL);
			metric_name = metric_name_base + "location_of_worst_case_response_time";
			xmlNewProp(node,(xmlChar *)"name", (xmlChar *)metric_name.c_str());	
			double_value = app_task[i]->location_of_worst_case_response_time().to_seconds();
			value_string = ext_to_string(double_value);
			xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
			xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)"s");

#ifdef NO_DEFINED
// note: avg_response_time not implemented yet in KisTA
			node = xmlNewChild(root_node, NULL, (xmlChar *)"avg_response_time", NULL);
			metric_name = metric_name_base + "avg_response_time";
			xmlNewProp(node,(xmlChar *)"name", (xmlChar *)metric_name.c_str());
			double_value = app_task[i]->avg_response_time().to_seconds();	
			value_string = ext_to_string(double_value);
			xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
			xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)"s")

// note: avg_response_time not implemented yet in KisTA	
			node = xmlNewChild(root_node, NULL, (xmlChar *)"std_dev_response_time", NULL);
			metric_name = metric_name_base + "std_dev_response_time";
			xmlNewProp(node,(xmlChar *)"name", (xmlChar *)metric_name.c_str());	
			double_value = app_task[i]->std_dev_response_time().to_seconds();
			value_string = ext_to_string(double_value);
			xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
			xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)"s")
#endif
		}
		
		node = xmlNewChild(root_node, NULL, (xmlChar *)"system_metric", NULL);
		metric_name = metric_name_base + "miss_deadline";
		xmlNewProp(node,(xmlChar *)"name", (xmlChar *)metric_name.c_str());	
		xmlNewProp(node,(xmlChar *)"value", (xmlChar *)(app_task[i]->miss_deadline()?"true":"false"));
	}

	//--------------------------------------------------------------------------------------
	// application edges related metrics
	//    any name of any type of system-level channel is valed (of fifo_buffer type, etc)
	// --------------------------------------------------------------------------------------
    /*
    for(unsigned int i=0; i<sys_level_edges.size(); i++) {
		metric_name_base = "synch_comm/";
		metric_name_base += syn_conn[i]->name();
		metric_name_base += "/";
		
	}
	*/

    // Dumping document to file
    xmlSaveFormatFileEnc(xml_out_file, doc, "UTF-8", 1);
    
    // or to standard output
    // xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);

    xmlFreeDoc(doc);

}


void create_MULTICUBE_selected_output(char *xml_out_file, char *smd_file_name,
                                      vector<scheduler*> &RTOS,
                                      vector<task_info_t*> &app_task,
									  fifo_buffer_int_set_t     &fifo_buffer_int_map,
									  fifo_buffer_uint_set_t    &fifo_buffer_uint_map,
									  fifo_buffer_short_set_t   &fifo_buffer_short_map,
									  fifo_buffer_ushort_set_t  &fifo_buffer_ushort_map, 
									  fifo_buffer_float_set_t   &fifo_buffer_float_map,
									  fifo_buffer_double_set_t  &fifo_buffer_double_map,
									  fifo_buffer_char_set_t    &fifo_buffer_char_map,
									  fifo_buffer_voidp_set_t   &fifo_buffer_voidp_map,
									  fifo_buffer_message_set_t   &fifo_buffer_msg_map
                                      ) {

	xmlDocPtr doc = NULL;       		// document pointer
    xmlNodePtr root_node, node = NULL;	// node pointers
//    xmlDtdPtr dtd = NULL;      			// DTD pointer

	std::string metric_name_base,metric_name;
		
	metric_def_t selected_ometrics;
	metric_def_t::iterator it; // metrics iterator
	
	fifo_buffer_int_set_t::iterator	    fb_int_it;
	fifo_buffer_uint_set_t::iterator	fb_uint_it;
	fifo_buffer_short_set_t::iterator	fb_short_it;
	fifo_buffer_ushort_set_t::iterator	fb_ushort_it;
	fifo_buffer_float_set_t::iterator	fb_float_it;
	fifo_buffer_double_set_t::iterator	fb_double_it;
	fifo_buffer_char_set_t::iterator	fb_char_it;
	fifo_buffer_voidp_set_t::iterator	fb_voidp_it;
	fifo_buffer_message_set_t::iterator	fb_msg_it;
	
	char *curr_tok;
	std::string curr_tok_string;
	
	bool found_elem_with_metric; 	// set to true when an element instance of the selected metric is found
	bool found_fifo_buffer_int_with_metric; // specific flags for metrics report on different types of fifo buffer channel times
	bool found_fifo_buffer_uint_with_metric;
	bool found_fifo_buffer_short_with_metric;
	bool found_fifo_buffer_ushort_with_metric;	
	bool found_fifo_buffer_float_with_metric;
	bool found_fifo_buffer_double_with_metric;
	bool found_fifo_buffer_char_with_metric;
	bool found_fifo_buffer_voidp_with_metric;
	bool found_fifo_buffer_msg_with_metric;
	
	task_info_t	*curr_task;
	scheduler *curr_sched;
	
	fifo_buffer<int>		       *curr_fifo_buffer_int;
	fifo_buffer<unsigned int>   *curr_fifo_buffer_uint;
	fifo_buffer<short>		   *curr_fifo_buffer_short;
	fifo_buffer<unsigned short> *curr_fifo_buffer_ushort;	
	fifo_buffer<float>		   *curr_fifo_buffer_float;
	fifo_buffer<double>		   *curr_fifo_buffer_double;
	fifo_buffer<char>		       *curr_fifo_buffer_char;
	fifo_buffer<void *>		   *curr_fifo_buffer_voidp;
	fifo_buffer<message_t>	   *curr_fifo_buffer_msg;
	// To be added, transfer of doubles and of pointers ...
	
	unsigned int i;
	std::string rpt_msg;
	const char *m_unit; // temporal variable for metric unit
	
	// temporal variables for
		// reported metrics
	double double_value;
		// value string
	char  *value_string;
	
	// Retrieve the selected output metrics
	getSelectedOutputMetrics(smd_file_name, selected_ometrics);
	
	// Start creation of the output metrics file (creating the root node)
    doc = xmlNewDoc((xmlChar *)"1.0");
    root_node = xmlNewNode(NULL, (xmlChar *)OUTPUT_ROOT_NODE_NAME);
    xmlNewProp(root_node,(xmlChar *)"xmlns",(xmlChar *)"http://www.multicube.eu/");
    xmlNewProp(root_node,(xmlChar *)"version",(xmlChar *)"1.4");
    xmlDocSetRootElement(doc, root_node);


	for(it=selected_ometrics.begin(); it != selected_ometrics.end(); ++it ) {
		m_unit = it->second->unit;
#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {
		
			rpt_msg = "Reporting metric: " ;
			rpt_msg += it->second->name;
			rpt_msg += " (";
			if(m_unit==NULL) rpt_msg += "-";
			else rpt_msg += m_unit;
			rpt_msg += ")";
			SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
		}
#endif		
		// create the new system metric node
		node = xmlNewChild(root_node, NULL, (xmlChar *)"system_metric", NULL);
		
		// create the metric name node
		xmlNewProp(node,(xmlChar *)"name", (xmlChar *)it->second->name);	
		
		// the value of the metric itself (this requires parsing the metric name)
		// ... first taking the base name of the metric (type element containing the metric)
		curr_tok = strtok(it->second->name,"/");
		
//		printf("Recognizing base name of the metric %s\n",curr_tok);
		
		found_elem_with_metric = false;
		found_fifo_buffer_int_with_metric = false;
		found_fifo_buffer_uint_with_metric = false;
		found_fifo_buffer_short_with_metric = false;
		found_fifo_buffer_ushort_with_metric = false;		
		found_fifo_buffer_float_with_metric = false;
		found_fifo_buffer_double_with_metric = false;
		found_fifo_buffer_char_with_metric = false;
		found_fifo_buffer_voidp_with_metric = false;
		found_fifo_buffer_msg_with_metric = false;
			
		if (!strcmp("task",curr_tok)) { 
			//----------------------------------------------------------
			// Reporting a task metric
			// ---------------------------------------------------------
			// gets the actual instance name of the metric
			curr_tok = strtok(NULL,"/");
			
			// get the task
			for(i=0;i<app_task.size();i++) {
				if(!strcmp(curr_tok,app_task[i]->name())) {
					found_elem_with_metric = true;
					curr_task = app_task[i];
					break;
				}
			}
			
			if(!found_elem_with_metric) {
				rpt_msg = "Report of task ";
				rpt_msg += curr_tok;
				rpt_msg += " metric failed because such a task does not exist in the description.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
			}
			
			curr_tok = strtok(NULL,"/");
			
			if(!strcmp(curr_tok,"utilization")) {
				double_value = curr_task->get_scheduler()->get_task_utilization(curr_task->name());
				value_string = ext_to_string(double_value);				
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
			} else if (!strcmp(curr_tok,"avg_response_time")) {
				if(curr_task->is_periodic()) {
					double_value = curr_task->avg_response_time().to_seconds();
					double_value = conv_isu_factor(it->second->unit[0]);
					value_string = ext_to_string(double_value);
					xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
					xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)it->second->unit );
				} else {
					rpt_msg = "Report of metric avg_response_time for task ";
					rpt_msg += curr_task->name();
					rpt_msg += ", which is not periodic.";
					rpt_msg += "Kista currently reports response times (avg.) for periodic tasks.\n";
					SC_REPORT_ERROR("Kista-XML",rpt_msg.c_str());
				}
			} else if (!strcmp(curr_tok,"std_dev_response_time")) {
				if(curr_task->is_periodic()) {
					double_value = curr_task->avg_response_time().to_seconds();
					double_value = conv_isu_factor(it->second->unit[0]);					
					value_string = ext_to_string(double_value);		
					xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
					xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)it->second->unit);
				} else {
					rpt_msg = "Report of metric std_dev_response_time for task ";
					rpt_msg += curr_task->name();
					rpt_msg += ", which is not periodic.";
					rpt_msg += "Kista currently reports response times (std. dev.) for periodic tasks.\n";
					SC_REPORT_ERROR("Kista-XML",rpt_msg.c_str());
				}
			} else if (!strcmp(curr_tok,"max_response_time")) {
				if(curr_task->is_periodic()) {
					double_value = curr_task->max_response_time().to_seconds();
					double_value = conv_isu_factor(it->second->unit[0]);
					value_string = ext_to_string(double_value);
					xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
					xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)it->second->unit );					
				} else {
					rpt_msg = "Report of metric max_response_time for task ";
					rpt_msg += curr_task->name();
					rpt_msg += ", which is not periodic.";
					rpt_msg += "Kista currently reports response times (max.) for periodic tasks.\n";
					SC_REPORT_ERROR("Kista-XML",rpt_msg.c_str());
				}	
			} else if (!strcmp(curr_tok,"location_of_worst_case_response_time")) {
				if(curr_task->is_periodic()) {
					double_value = curr_task->location_of_worst_case_response_time().to_seconds();
					double_value = conv_isu_factor(it->second->unit[0]);
					value_string = ext_to_string(double_value);
					xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
					xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)it->second->unit );					
				} else {
					rpt_msg = "Report of metric location_of_worst_case_response_time for task ";
					rpt_msg += curr_task->name();
					rpt_msg += ", which is not periodic.";
					rpt_msg += "Kista currently reports response times (max. location) for periodic tasks.\n";
					SC_REPORT_ERROR("Kista-XML",rpt_msg.c_str());
				}	
			} else if (!strcmp(curr_tok,"miss_deadline")) {
					xmlNewProp(node,(xmlChar *)"value", (xmlChar *)to_string(curr_task->miss_deadline()).c_str());
			} else {
				rpt_msg = "Report of task metric ";
				rpt_msg += curr_tok;
				rpt_msg += " failed. Such task metric name is not recognized by KisTA.\n";
				rpt_msg += " Current supported  task metrics are: utilization, avg_response_time, std_dev_response_time, max_response_time, location_of_worst_case_response_time and miss_deadline.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
			}
				
		} else if (!strcmp("comm_synch",curr_tok)) { // Reporting a system-level synchronization/communication  metric
			//----------------------------------------------------------
			// Reporting a system-level synchronization/communication  metric
			// ---------------------------------------------------------

			// gets the actual instance name of the metric
			curr_tok = strtok(NULL,"/");
			curr_tok_string = curr_tok;
			
			// get the synchronization&communication element
			
			// Currently, the search is done only on fifo_buffer channels supported
			// and the code assumes that each channel instance name is unique among
			// all channel instances (of any kind), so only one match is expected
			//
			
			if( (fb_int_it = fifo_buffer_int_map.find(curr_tok_string)) != fifo_buffer_int_map.end() ) {
				found_fifo_buffer_int_with_metric = true;
				curr_fifo_buffer_int = fb_int_it->second;
			} else
			if( (fb_uint_it = fifo_buffer_uint_map.find(curr_tok_string) ) != fifo_buffer_uint_map.end() ) {
				found_fifo_buffer_uint_with_metric = true;
				curr_fifo_buffer_uint = fb_uint_it->second;
			} else
			if( (fb_short_it = fifo_buffer_short_map.find(curr_tok_string)) != fifo_buffer_short_map.end() ) {
				found_fifo_buffer_short_with_metric = true;
				curr_fifo_buffer_short = fb_short_it->second;
			} else
			if( (fb_ushort_it = fifo_buffer_ushort_map.find(curr_tok_string) ) != fifo_buffer_ushort_map.end() ) {
				found_fifo_buffer_ushort_with_metric = true;
				curr_fifo_buffer_ushort = fb_ushort_it->second;
			} else
			if( (fb_float_it = fifo_buffer_float_map.find(curr_tok_string) ) != fifo_buffer_float_map.end() ) {
				 found_fifo_buffer_float_with_metric = true;
				curr_fifo_buffer_float = fb_float_it->second;
			} else
			if( (fb_double_it = fifo_buffer_double_map.find(curr_tok_string) ) != fifo_buffer_double_map.end() ) {
				found_fifo_buffer_double_with_metric = true;
				curr_fifo_buffer_double = fb_double_it->second;
			} else
			if( (fb_char_it = fifo_buffer_char_map.find(curr_tok_string) ) != fifo_buffer_char_map.end() ) {
				found_fifo_buffer_char_with_metric = true;
				curr_fifo_buffer_char = fb_char_it->second;
			} else
			if( (fb_voidp_it = fifo_buffer_voidp_map.find(curr_tok_string) ) != fifo_buffer_voidp_map.end() ) {
				found_fifo_buffer_voidp_with_metric = true;
				curr_fifo_buffer_voidp = fb_voidp_it->second;
			} else
			if( (fb_msg_it = fifo_buffer_msg_map.find(curr_tok_string) ) != fifo_buffer_msg_map.end() ) {
				found_fifo_buffer_msg_with_metric = true;
				curr_fifo_buffer_msg = fb_msg_it->second;
			} else {
				rpt_msg = "Report of comm&synch element ";
				rpt_msg += curr_tok;
				rpt_msg += " metric failed because such comm&synch element does not exist in the description.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
			}
			
			curr_tok = strtok(NULL,"/");
 
			// ---------------------
			// NORMALIZED THROUGHPUT REPORTS
			// ---------------------
			
			// in KisTA-XML, throughput = write_throughput
			if( (!strcmp(curr_tok,"normalized_throughput")) || (!strcmp(curr_tok,"normalized_write_throughput")) ) {
				if(found_fifo_buffer_int_with_metric) {
					double_value = curr_fifo_buffer_int->get_normalized_write_throughput(m_unit);
				} else if(found_fifo_buffer_uint_with_metric) {
					double_value = curr_fifo_buffer_uint->get_normalized_write_throughput(m_unit);
				} else if(found_fifo_buffer_short_with_metric) {
					double_value = curr_fifo_buffer_short->get_normalized_write_throughput(m_unit);
				} else if(found_fifo_buffer_ushort_with_metric) {
					double_value = curr_fifo_buffer_ushort->get_normalized_write_throughput(m_unit);
				} else if(found_fifo_buffer_float_with_metric) {
					double_value = curr_fifo_buffer_float->get_normalized_write_throughput(m_unit);
				} else if(found_fifo_buffer_double_with_metric) {
					double_value = curr_fifo_buffer_double->get_normalized_write_throughput(m_unit);
				} else if(found_fifo_buffer_char_with_metric) {
					double_value = curr_fifo_buffer_char->get_normalized_write_throughput(m_unit);
				} else if(found_fifo_buffer_voidp_with_metric) {
					double_value = curr_fifo_buffer_voidp->get_normalized_write_throughput(m_unit);
				} else if(found_fifo_buffer_voidp_with_metric) {
					double_value = curr_fifo_buffer_msg->get_normalized_write_throughput(m_unit);
				} else {
					rpt_msg = "Unexpected situation in throughput metric report.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
				value_string = ext_to_string(double_value);				
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
				xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)it->second->unit );
			}
			else if(!strcmp(curr_tok,"normalized_read_throughput")) {
				if(found_fifo_buffer_int_with_metric) {
					double_value = curr_fifo_buffer_int->get_normalized_read_throughput(m_unit);
				} else if(found_fifo_buffer_uint_with_metric) {
					double_value = curr_fifo_buffer_uint->get_normalized_read_throughput(m_unit);
				} else if(found_fifo_buffer_short_with_metric) {
					double_value = curr_fifo_buffer_short->get_normalized_read_throughput(m_unit);
				} else if(found_fifo_buffer_ushort_with_metric) {
					double_value = curr_fifo_buffer_ushort->get_normalized_read_throughput(m_unit);
				} else if(found_fifo_buffer_float_with_metric) {
					double_value = curr_fifo_buffer_float->get_normalized_read_throughput(m_unit);
				} else if(found_fifo_buffer_double_with_metric) {
					double_value = curr_fifo_buffer_double->get_normalized_read_throughput(m_unit);
				} else if(found_fifo_buffer_char_with_metric) {
					double_value = curr_fifo_buffer_char->get_normalized_read_throughput(m_unit);
				} else if(found_fifo_buffer_voidp_with_metric) {
					double_value = curr_fifo_buffer_voidp->get_normalized_read_throughput(m_unit);
				} else if(found_fifo_buffer_msg_with_metric) {
					double_value = curr_fifo_buffer_msg->get_normalized_read_throughput(m_unit);
				} else {
					rpt_msg = "Unexpected situation in normalized_read_throughput metric report.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
				value_string = ext_to_string(double_value);				
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
				xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)it->second->unit);
			}
			else if ( (!strcmp(curr_tok,"normalized_stationary_throughput")) || (!strcmp(curr_tok,"normalized_write_stationary_throughput")) ) {
				if(found_fifo_buffer_int_with_metric) {
					double_value = curr_fifo_buffer_int->get_normalized_write_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_uint_with_metric) {
					double_value = curr_fifo_buffer_uint->get_normalized_write_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_short_with_metric) {
					double_value = curr_fifo_buffer_short->get_normalized_write_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_ushort_with_metric) {
					double_value = curr_fifo_buffer_ushort->get_normalized_write_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_float_with_metric) {
					double_value = curr_fifo_buffer_float->get_normalized_write_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_double_with_metric) {
					double_value = curr_fifo_buffer_double->get_normalized_write_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_char_with_metric) {
					double_value = curr_fifo_buffer_char->get_normalized_write_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_voidp_with_metric) {
					double_value = curr_fifo_buffer_voidp->get_normalized_write_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_msg_with_metric) {
					double_value = curr_fifo_buffer_msg->get_normalized_write_stationary_throughput(m_unit);
				} else {
					rpt_msg = "Unexpected situation in normalized_stationary_throughput metric report.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
				value_string = ext_to_string(double_value);				
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
				xmlNewProp(node,(xmlChar *)"unit", (const xmlChar *)it->second->unit);
			}
			else if (!strcmp(curr_tok,"normalized_read_stationary_throughput"))  {
				if(found_fifo_buffer_int_with_metric) {
					double_value = curr_fifo_buffer_int->get_normalized_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_uint_with_metric) {
					double_value = curr_fifo_buffer_uint->get_normalized_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_short_with_metric) {
					double_value = curr_fifo_buffer_short->get_normalized_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_ushort_with_metric) {
					double_value = curr_fifo_buffer_ushort->get_normalized_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_float_with_metric) {
					double_value = curr_fifo_buffer_float->get_normalized_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_double_with_metric) {
					double_value = curr_fifo_buffer_double->get_normalized_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_char_with_metric) {
					double_value = curr_fifo_buffer_char->get_normalized_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_voidp_with_metric) {
					double_value = curr_fifo_buffer_voidp->get_normalized_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_voidp_with_metric) {
					double_value = curr_fifo_buffer_msg->get_normalized_read_stationary_throughput(m_unit);
				} else {
					rpt_msg = "Unexpected situation in throughput metric report.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
				value_string = ext_to_string(double_value);				
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
				xmlNewProp(node,(xmlChar *)"unit", (const xmlChar *)it->second->unit);
			}
			
			// -------------------------------
			// THROUGHPUT REPORTS
			// -------------------------------
			
					// in KisTA-XML, throughput = write_throughput
			else if( (!strcmp(curr_tok,"throughput")) || (!strcmp(curr_tok,"write_throughput")) ) {
				if(found_fifo_buffer_int_with_metric) {
					double_value = curr_fifo_buffer_int->get_write_throughput(m_unit);
				} else if(found_fifo_buffer_uint_with_metric) {
					double_value = curr_fifo_buffer_uint->get_write_throughput(m_unit);
				} else if(found_fifo_buffer_short_with_metric) {
					double_value = curr_fifo_buffer_short->get_write_throughput(m_unit);
				} else if(found_fifo_buffer_ushort_with_metric) {
					double_value = curr_fifo_buffer_ushort->get_write_throughput(m_unit);
				} else if(found_fifo_buffer_float_with_metric) {
					double_value = curr_fifo_buffer_float->get_write_throughput(m_unit);
				} else if(found_fifo_buffer_double_with_metric) {
					double_value = curr_fifo_buffer_double->get_write_throughput(m_unit);
				} else if(found_fifo_buffer_char_with_metric) {
					double_value = curr_fifo_buffer_char->get_write_throughput(m_unit);
				} else if(found_fifo_buffer_voidp_with_metric) {
					double_value = curr_fifo_buffer_voidp->get_write_throughput(m_unit);
				} else if(found_fifo_buffer_msg_with_metric) {
					double_value = curr_fifo_buffer_msg->get_write_throughput(m_unit);
				} else {
					rpt_msg = "Unexpected situation in write_throughput metric report.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
				value_string = ext_to_string(double_value);				
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
				xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)it->second->unit );
			}
			else if(!strcmp(curr_tok,"read_throughput")) {
				if(found_fifo_buffer_int_with_metric) {
					double_value = curr_fifo_buffer_int->get_read_throughput(m_unit);
				} else if(found_fifo_buffer_uint_with_metric) {
					double_value = curr_fifo_buffer_uint->get_read_throughput(m_unit);
				} else if(found_fifo_buffer_short_with_metric) {
					double_value = curr_fifo_buffer_short->get_read_throughput(m_unit);
				} else if(found_fifo_buffer_ushort_with_metric) {
					double_value = curr_fifo_buffer_ushort->get_read_throughput(m_unit);
				} else if(found_fifo_buffer_float_with_metric) {
					double_value = curr_fifo_buffer_float->get_read_throughput(m_unit);
				} else if(found_fifo_buffer_double_with_metric) {
					double_value = curr_fifo_buffer_double->get_read_throughput(m_unit);
				} else if(found_fifo_buffer_char_with_metric) {
					double_value = curr_fifo_buffer_char->get_read_throughput(m_unit);
				} else if(found_fifo_buffer_voidp_with_metric) {
					double_value = curr_fifo_buffer_voidp->get_read_throughput(m_unit);
				} else if(found_fifo_buffer_msg_with_metric) {
					double_value = curr_fifo_buffer_msg->get_read_throughput(m_unit);
				} else {
					rpt_msg = "Unexpected situation in read_throughput metric report.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
				value_string = ext_to_string(double_value);				
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
				xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)it->second->unit);
			}
			else if ( (!strcmp(curr_tok,"stationary_throughput")) || (!strcmp(curr_tok,"write_stationary_throughput")) ) {
				if(found_fifo_buffer_int_with_metric) {
					double_value = curr_fifo_buffer_int->get_write_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_uint_with_metric) {
					double_value = curr_fifo_buffer_uint->get_write_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_short_with_metric) {
					double_value = curr_fifo_buffer_short->get_write_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_ushort_with_metric) {
					double_value = curr_fifo_buffer_ushort->get_write_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_float_with_metric) {
					double_value = curr_fifo_buffer_float->get_write_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_double_with_metric) {
					double_value = curr_fifo_buffer_double->get_write_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_char_with_metric) {
					double_value = curr_fifo_buffer_char->get_write_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_voidp_with_metric) {
					double_value = curr_fifo_buffer_voidp->get_write_stationary_throughput(m_unit);
				}  else if(found_fifo_buffer_msg_with_metric) {
					double_value = curr_fifo_buffer_msg->get_write_stationary_throughput(m_unit);
				} else {
					rpt_msg = "Unexpected situation in write_stationary_throughput metric report.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
				value_string = ext_to_string(double_value);				
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
				xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)it->second->unit);
			}
			else if (!strcmp(curr_tok,"read_stationary_throughput"))  {
				if(found_fifo_buffer_int_with_metric) {
					double_value = curr_fifo_buffer_int->get_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_uint_with_metric) {
					double_value = curr_fifo_buffer_uint->get_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_short_with_metric) {
					double_value = curr_fifo_buffer_short->get_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_ushort_with_metric) {
					double_value = curr_fifo_buffer_ushort->get_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_float_with_metric) {
					double_value = curr_fifo_buffer_float->get_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_double_with_metric) {
					double_value = curr_fifo_buffer_double->get_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_char_with_metric) {
					double_value = curr_fifo_buffer_char->get_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_voidp_with_metric) {
					double_value = curr_fifo_buffer_voidp->get_read_stationary_throughput(m_unit);
				} else if(found_fifo_buffer_msg_with_metric) {
					double_value = curr_fifo_buffer_msg->get_read_stationary_throughput(m_unit);
				} else {
					rpt_msg = "Unexpected situation in read_stationary_throughput metric report.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
				value_string = ext_to_string(double_value);				
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
				xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)it->second->unit);
			}
			else if (!strcmp(curr_tok,"write_period"))  {
				if(found_fifo_buffer_int_with_metric) {
					double_value = curr_fifo_buffer_int->get_write_period(m_unit);
				} else if(found_fifo_buffer_uint_with_metric) {
					double_value = curr_fifo_buffer_uint->get_write_period(m_unit);
				} else if(found_fifo_buffer_short_with_metric) {
					double_value = curr_fifo_buffer_short->get_write_period(m_unit);
				} else if(found_fifo_buffer_ushort_with_metric) {
					double_value = curr_fifo_buffer_ushort->get_write_period(m_unit);
				} else if(found_fifo_buffer_float_with_metric) {
					double_value = curr_fifo_buffer_float->get_write_period(m_unit);
				} else if(found_fifo_buffer_double_with_metric) {
					double_value = curr_fifo_buffer_double->get_write_period(m_unit);
				} else if(found_fifo_buffer_char_with_metric) {
					double_value = curr_fifo_buffer_char->get_write_period(m_unit);
				} else if(found_fifo_buffer_voidp_with_metric) {
					double_value = curr_fifo_buffer_voidp->get_write_period(m_unit);
				} else if(found_fifo_buffer_msg_with_metric) {
					double_value = curr_fifo_buffer_msg->get_write_period(m_unit);
				} else {
					rpt_msg = "Unexpected situation in write_period metric report.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
				value_string = ext_to_string(double_value);		
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
				xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)it->second->unit);
			}
			else if (!strcmp(curr_tok,"read_period"))  {
				if(found_fifo_buffer_int_with_metric) {
					double_value = curr_fifo_buffer_int->get_read_period(m_unit);
				} else if(found_fifo_buffer_uint_with_metric) {
					double_value = curr_fifo_buffer_uint->get_read_period(m_unit);
				} else if(found_fifo_buffer_short_with_metric) {
					double_value = curr_fifo_buffer_short->get_read_period(m_unit);
				} else if(found_fifo_buffer_ushort_with_metric) {
					double_value = curr_fifo_buffer_ushort->get_read_period(m_unit);
				} else if(found_fifo_buffer_float_with_metric) {
					double_value = curr_fifo_buffer_float->get_read_period(m_unit);
				} else if(found_fifo_buffer_double_with_metric) {
					double_value = curr_fifo_buffer_double->get_read_period(m_unit);
				} else if(found_fifo_buffer_char_with_metric) {
					double_value = curr_fifo_buffer_char->get_read_period(m_unit);
				} else if(found_fifo_buffer_voidp_with_metric) {
					double_value = curr_fifo_buffer_voidp->get_read_period(m_unit);
				}  else if(found_fifo_buffer_msg_with_metric) {
					double_value = curr_fifo_buffer_msg->get_read_period(m_unit);
				} else {
					rpt_msg = "Unexpected situation in read_period metric report.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
				value_string = ext_to_string(double_value);		
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
				xmlNewProp(node,(xmlChar *)"unit", (xmlChar *)it->second->unit);
			}			
			// -------------------------------
			// NON-RECOGNIZED METRICS
			// -------------------------------	
					
			else {
				rpt_msg = "Report of synch&comm metric ";
				rpt_msg += curr_tok;
				rpt_msg += " failed. Such syn&comm metric name is not recognized by KisTA.\n";
				rpt_msg += " Current supported syn&comm metrics are: [write_(def.)|read_]throughput (bps/Bps/Kbps/Mbps), [write_(def.)|read_]stationary_throughput (bps/Bps/Kbps/Mbps), normalized_[write_(def.)|read_]throughput (Hz/KHz/MHz/GHz/THz), normalized_[write(def.)|read]_stationary_throughput (Hz/KHz/MHz/GHz/THz).";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
			}
			
		} else if (!strcmp("rtos",curr_tok)) { // Reporting a scheduler metric
			//----------------------------------------------------------
			// Reporting a scheduler metric
			// ---------------------------------------------------------

			// gets the actual instance name of the metric
			curr_tok = strtok(NULL,"/");
			
			// get the task
			for(i=0;i<RTOS.size();i++) {
				if(!strcmp(curr_tok,RTOS[i]->name())) {
					found_elem_with_metric = true;
					curr_sched = RTOS[i];
					break;
				}
			}
			
			if(!found_elem_with_metric) {
				rpt_msg = "Report of sched ";
				rpt_msg += curr_tok;
				rpt_msg += " metric failed because such an schedulerdoes not exit in the description.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
			}
			
			curr_tok = strtok(NULL,"/");
			
			if(!strcmp(curr_tok,"tasks_utilization")) {
				double_value = curr_sched->get_tasks_utilization();
				value_string = ext_to_string(double_value);
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
			} else if(!strcmp(curr_tok,"sched_utilization")) {
				double_value = curr_sched->get_scheduler_utilization();
				value_string = ext_to_string(double_value);					
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
			} else if (!strcmp(curr_tok,"plat_utilization")) {
				double_value = curr_sched->get_platform_utilization();
				value_string = ext_to_string(double_value);					
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)value_string);
			} else if (!strcmp(curr_tok,"number_of_schedulings")) {
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)to_string(curr_sched->get_number_of_schedulings()).c_str());		
			} else if (!strcmp(curr_tok,"number_of_context_switches")) {
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)to_string(curr_sched->get_number_of_context_switches()).c_str());
			} else if (!strcmp(curr_tok,"starvation")) {
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)to_string(curr_sched->assess_starvation()).c_str());
			} else if (!strcmp(curr_tok,"miss_deadlines")) {
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)to_string(curr_sched->miss_deadlines()).c_str());
			} else {
				rpt_msg = "Report of task metric ";
				rpt_msg += curr_tok;
				rpt_msg += " failed. Such task metric name is not recognized by KisTA.\n";
				rpt_msg += " Current supported rtos metrics are: sched_utilization, plat_utilization, number_of_schedulings , number_of_context_switches, starvation and miss_deadlines.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
			}
		} else {
			rpt_msg = "Element ";
			rpt_msg += curr_tok;
			rpt_msg += " not recognized as base element with metric.";
			SC_REPORT_ERROR("Kista-XML",rpt_msg.c_str());
		}
	}
    
    // Dumping document to file
    xmlSaveFormatFileEnc(xml_out_file, doc, "UTF-8", 1);
    
    // or to standard output
    // xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);

    xmlFreeDoc(doc);

}

void enable_measurements_for_MULTICUBE_selected_output(char *smd_file_name,
                                      vector<scheduler*> &RTOS,
                                      vector<task_info_t*> &app_task,
									  fifo_buffer_int_set_t     &fifo_buffer_int_map,
									  fifo_buffer_uint_set_t    &fifo_buffer_uint_map,
									  fifo_buffer_short_set_t   &fifo_buffer_short_map,
									  fifo_buffer_ushort_set_t  &fifo_buffer_ushort_map,									  
									  fifo_buffer_float_set_t   &fifo_buffer_float_map,
									  fifo_buffer_double_set_t  &fifo_buffer_double_map,
									  fifo_buffer_char_set_t    &fifo_buffer_char_map,
									  fifo_buffer_voidp_set_t   &fifo_buffer_voidp_map,
									  fifo_buffer_message_set_t &fifo_buffer_msg_map
                                      ) {

	xmlDocPtr doc = NULL;       		// document pointer
    xmlNodePtr root_node, node = NULL;	// node pointers
//    xmlDtdPtr dtd = NULL;      			// DTD pointer

	std::string metric_name_base,metric_name;
	std::string rpt_msg;
		
	metric_def_t selected_ometrics;
	metric_def_t::iterator it; // metrics iterator
	
	fifo_buffer_int_set_t::iterator	    fb_int_it;
	fifo_buffer_uint_set_t::iterator	    fb_uint_it;
	fifo_buffer_short_set_t::iterator	fb_short_it;
	fifo_buffer_ushort_set_t::iterator	fb_ushort_it;	
	fifo_buffer_float_set_t::iterator	fb_float_it;
	fifo_buffer_double_set_t::iterator	fb_double_it;
	fifo_buffer_char_set_t::iterator	    fb_char_it;
	fifo_buffer_voidp_set_t::iterator	fb_voidp_it;
	fifo_buffer_message_set_t::iterator	fb_msg_it;
		
	char         *curr_tok;
	std::string  curr_tok_string;
	const char	*m_unit; // metric unit
	
	//bool found_elem_with_metric; 	// set to true when an element instance of the selected metric is found
	bool found_fifo_buffer_int_with_metric; // specific flags for metrics report on different types of fifo buffer channel times
	bool found_fifo_buffer_uint_with_metric;
	bool found_fifo_buffer_short_with_metric;
	bool found_fifo_buffer_ushort_with_metric;
	bool found_fifo_buffer_float_with_metric;
	bool found_fifo_buffer_double_with_metric;
	bool found_fifo_buffer_char_with_metric;
	bool found_fifo_buffer_voidp_with_metric;
	bool found_fifo_buffer_msg_with_metric;
	
//	task_info_t	*curr_task;
//	scheduler *curr_sched;
	
	fifo_buffer<int>			   *curr_fifo_buffer_int;
	fifo_buffer<unsigned int>    *curr_fifo_buffer_uint;
	fifo_buffer<short>		       *curr_fifo_buffer_short;
	fifo_buffer<unsigned short>  *curr_fifo_buffer_ushort;
	fifo_buffer<float>		       *curr_fifo_buffer_float;
	fifo_buffer<double>		   *curr_fifo_buffer_double;
	fifo_buffer<char>			   *curr_fifo_buffer_char;
	fifo_buffer<void *>		  	   *curr_fifo_buffer_voidp;
	fifo_buffer<message_t>		   *curr_fifo_buffer_msg;
	// To be added, transfer of doubles and of pointers ...

	// Retrieve the selected output metrics
	getSelectedOutputMetrics(smd_file_name, selected_ometrics);
	
	// Start creation of the output metrics file (creating the root node)
    doc = xmlNewDoc((xmlChar *)"1.0");
    root_node = xmlNewNode(NULL, (xmlChar *)OUTPUT_ROOT_NODE_NAME);
    xmlNewProp(root_node,(xmlChar *)"xmlns",(xmlChar *)"http://www.multicube.eu/");
    xmlNewProp(root_node,(xmlChar *)"version",(xmlChar *)"1.4");
    xmlDocSetRootElement(doc, root_node);


	for(it=selected_ometrics.begin(); it != selected_ometrics.end(); ++it ) {
		
		m_unit = it->second->unit;
		
#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {
			rpt_msg = "Enabling measurement of metric: " ;
			rpt_msg += it->second->name;
			rpt_msg += " (";
			if(m_unit==NULL) rpt_msg += "-";
			else rpt_msg += m_unit;
			rpt_msg += ")";
			SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
		}
#endif		
		// create the new system metric node
		node = xmlNewChild(root_node, NULL, (xmlChar *)"system_metric", NULL);
		
		// create the metric name node
		xmlNewProp(node,(xmlChar *)"name", (xmlChar *)it->second->name);	
		
		// the value of the metric itself (this requires parsing the metric name)
		// ... first taking the base name of the metric (type element containing the metric)
		curr_tok = strtok(it->second->name,"/");
		
//		printf("Recognizing base name of the metric %s\n",curr_tok);
		
//		found_elem_with_metric = false;
		found_fifo_buffer_int_with_metric = false;
		found_fifo_buffer_uint_with_metric = false;
		found_fifo_buffer_short_with_metric = false;
		found_fifo_buffer_ushort_with_metric = false;		
		found_fifo_buffer_float_with_metric = false;
		found_fifo_buffer_double_with_metric = false;
		found_fifo_buffer_char_with_metric = false;
		found_fifo_buffer_voidp_with_metric = false;
		found_fifo_buffer_msg_with_metric = false;
		
// For the moment, in KisTA,only throughput realted metrics require
// enabling before simulations start
			
/*
 		if (!strcmp("task",curr_tok)) { 
			//----------------------------------------------------------
			// Reporting a task metric
			// ---------------------------------------------------------
			// gets the actual instance name of the metric
			curr_tok = strtok(NULL,"/");
			
			// get the task
			for(i=0;i<app_task.size();i++) {
				if(!strcmp(curr_tok,app_task[i]->name())) {
					found_elem_with_metric = true;
					curr_task = app_task[i];
					break;
				}
			}
			
			if(!found_elem_with_metric) {
				rpt_msg = "Report of task ";
				rpt_msg += curr_tok;
				rpt_msg += " metric failed because such a task does not exist in the description.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
			}
			
			curr_tok = strtok(NULL,"/");
			
			if(!strcmp(curr_tok,"utilization")) {
				xmlNewProp(node,(xmlChar *)"value", (xmlChar *)to_string(curr_task->get_scheduler()->get_task_utilization(curr_task->name())).c_str());
			} else if (!strcmp(curr_tok,"avg_response_time")) {
				if(curr_task->is_periodic()) {
					xmlNewProp(node,(xmlChar *)"value", (xmlChar *)to_string(curr_task->avg_response_time().to_seconds()).c_str());
				} else {
					rpt_msg = "Report of metric avg_response_time for task ";
					rpt_msg += curr_task->name();
					rpt_msg += ", which is not periodic.";
					rpt_msg += "Kista currently reports response times (avg.) for periodic tasks.\n";
					SC_REPORT_ERROR("Kista-XML",rpt_msg.c_str());
				}
			} else if (!strcmp(curr_tok,"std_dev_response_time")) {
				if(curr_task->is_periodic()) {
					xmlNewProp(node,(xmlChar *)"value", (xmlChar *)to_string(curr_task->std_dev_response_time().to_seconds()).c_str());
				} else {
					rpt_msg = "Report of metric std_dev_response_time for task ";
					rpt_msg += curr_task->name();
					rpt_msg += ", which is not periodic.";
					rpt_msg += "Kista currently reports response times (std. dev.) for periodic tasks.\n";
					SC_REPORT_ERROR("Kista-XML",rpt_msg.c_str());
				}
			} else if (!strcmp(curr_tok,"max_response_time")) {
				if(curr_task->is_periodic()) {
					xmlNewProp(node,(xmlChar *)"value", (xmlChar *)to_string(curr_task->max_response_time().to_seconds()).c_str());
				} else {
					rpt_msg = "Report of metric max_response_time for task ";
					rpt_msg += curr_task->name();
					rpt_msg += ", which is not periodic.";
					rpt_msg += "Kista currently reports response times (max.) for periodic tasks.\n";
					SC_REPORT_ERROR("Kista-XML",rpt_msg.c_str());
				}	
			} else if (!strcmp(curr_tok,"location_of_worst_case_response_time")) {
				if(curr_task->is_periodic()) {
					xmlNewProp(node,(xmlChar *)"value", (xmlChar *)to_string(curr_task->location_of_worst_case_response_time().to_seconds()).c_str());
				} else {
					rpt_msg = "Report of metric location_of_worst_case_response_time for task ";
					rpt_msg += curr_task->name();
					rpt_msg += ", which is not periodic.";
					rpt_msg += "Kista currently reports response times (max. location) for periodic tasks.\n";
					SC_REPORT_ERROR("Kista-XML",rpt_msg.c_str());
				}	
			} else if (!strcmp(curr_tok,"miss_deadline")) {
					xmlNewProp(node,(xmlChar *)"value", (xmlChar *)to_string(curr_task->miss_deadline()).c_str());
			} else {
				rpt_msg = "Report of task metric ";
				rpt_msg += curr_tok;
				rpt_msg += " failed. Such task metric name is not recognized by KisTA.\n";
				rpt_msg += " Current supported  task metrics are: utilization, avg_response_time, std_dev_response_time, max_response_time, location_of_worst_case_response_time and miss_deadline.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
			}
				
		} else
		*/
	    if (!strcmp("comm_synch",curr_tok)) { // Reporting a system-level synchronization/communication  metric
			//----------------------------------------------------------
			// Reporting a system-level synchronization/communication  metric
			// ---------------------------------------------------------

			// gets the actual instance name of the metric
			curr_tok = strtok(NULL,"/");
			curr_tok_string = curr_tok;
			
			// get the synchronization&communication element
			
						// Currently, the search is done only on fifo_buffer channels supported
			// and the code assumes that each channel instance name is unique among
			// all channel instances (of any kind), so only one match is expected
			//
			if( (fb_int_it = fifo_buffer_int_map.find(curr_tok_string)) != fifo_buffer_int_map.end() ) {
				found_fifo_buffer_int_with_metric = true;
				curr_fifo_buffer_int = fb_int_it->second;
			} else
			if( (fb_uint_it = fifo_buffer_uint_map.find(curr_tok_string) ) != fifo_buffer_uint_map.end() ) {
				found_fifo_buffer_uint_with_metric = true;
				curr_fifo_buffer_uint = fb_uint_it->second;
			} else
			if( (fb_short_it = fifo_buffer_short_map.find(curr_tok_string)) != fifo_buffer_short_map.end() ) {
				found_fifo_buffer_short_with_metric = true;
				curr_fifo_buffer_short = fb_short_it->second;
			} else
			if( (fb_ushort_it = fifo_buffer_ushort_map.find(curr_tok_string) ) != fifo_buffer_ushort_map.end() ) {
				found_fifo_buffer_ushort_with_metric = true;
				curr_fifo_buffer_ushort = fb_ushort_it->second;
			} else
			if( (fb_float_it = fifo_buffer_float_map.find(curr_tok_string) ) != fifo_buffer_float_map.end() ) {
				 found_fifo_buffer_float_with_metric = true;
				curr_fifo_buffer_float = fb_float_it->second;
			} else
			if( (fb_double_it = fifo_buffer_double_map.find(curr_tok_string) ) != fifo_buffer_double_map.end() ) {
				found_fifo_buffer_double_with_metric = true;
				curr_fifo_buffer_double = fb_double_it->second;
			} else
			if( (fb_char_it = fifo_buffer_char_map.find(curr_tok_string) ) != fifo_buffer_char_map.end() ) {
				found_fifo_buffer_char_with_metric = true;
				curr_fifo_buffer_char = fb_char_it->second;
			} else
			if( (fb_voidp_it = fifo_buffer_voidp_map.find(curr_tok_string) ) != fifo_buffer_voidp_map.end() ) {
				found_fifo_buffer_voidp_with_metric = true;
				curr_fifo_buffer_voidp = fb_voidp_it->second;
			} else
			if( (fb_msg_it = fifo_buffer_msg_map.find(curr_tok_string) ) != fifo_buffer_msg_map.end() ) {
				found_fifo_buffer_msg_with_metric = true;
				curr_fifo_buffer_msg = fb_msg_it->second;
			} else {
				rpt_msg = "Report of comm&synch element ";
				rpt_msg += curr_tok;
				rpt_msg += " metric failed because such comm&synch element does not exist in the description.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
			}

			curr_tok = strtok(NULL,"/");
			
			// in KisTA-XML, throughput = write_throughput
			if( (!strcmp(curr_tok,"throughput")) || (!strcmp(curr_tok,"write_throughput")) ||
				(!strcmp(curr_tok,"stationary_throughput")) || (!strcmp(curr_tok,"write_stationary_throughput")) ||
				(!strcmp(curr_tok,"normalized_throughput")) || (!strcmp(curr_tok,"normalized_write_throughput")) ||
				(!strcmp(curr_tok,"normalized_stationary_throughput")) || (!strcmp(curr_tok,"normalized_write_stationary_throughput")) ||
				(!strcmp(curr_tok,"write_period"))
			  ) {
				if(found_fifo_buffer_int_with_metric) {
					curr_fifo_buffer_int->measure_write_throughput();
				} else if(found_fifo_buffer_uint_with_metric) {
					curr_fifo_buffer_uint->measure_write_throughput();
				} else if(found_fifo_buffer_short_with_metric) {
					curr_fifo_buffer_short->measure_write_throughput();
				} else if(found_fifo_buffer_ushort_with_metric) {
					curr_fifo_buffer_ushort->measure_write_throughput();
				} else if(found_fifo_buffer_float_with_metric) {
					curr_fifo_buffer_float->measure_write_throughput();
				} else if(found_fifo_buffer_double_with_metric) {
					curr_fifo_buffer_double->measure_write_throughput();
				} else if(found_fifo_buffer_char_with_metric) {
					curr_fifo_buffer_char->measure_write_throughput();
				} else if(found_fifo_buffer_voidp_with_metric) {
					curr_fifo_buffer_voidp->measure_write_throughput();
				}  else if(found_fifo_buffer_msg_with_metric) {
					curr_fifo_buffer_msg->measure_write_throughput();
				} else {
					rpt_msg = "Unexpected situation in throughput metric report.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
			}
			else if( (!strcmp(curr_tok,"read_throughput")) || (!strcmp(curr_tok,"read_stationary_throughput")) ||
					 (!strcmp(curr_tok,"normalized_read_throughput")) || (!strcmp(curr_tok,"normalized_read_stationary_throughput")) ||
					 (!strcmp(curr_tok,"read_period"))
			        ) {
				if(found_fifo_buffer_int_with_metric) {
					curr_fifo_buffer_int->measure_read_throughput();
				} else if(found_fifo_buffer_uint_with_metric) {
					curr_fifo_buffer_uint->measure_read_throughput();
				} else if(found_fifo_buffer_short_with_metric) {
					curr_fifo_buffer_short->measure_read_throughput();
				} else if(found_fifo_buffer_ushort_with_metric) {
					curr_fifo_buffer_ushort->measure_read_throughput();
				} else if(found_fifo_buffer_float_with_metric) {
					curr_fifo_buffer_float->measure_read_throughput();
				} else if(found_fifo_buffer_double_with_metric) {
					curr_fifo_buffer_double->measure_read_throughput();
				} else if(found_fifo_buffer_char_with_metric) {
					curr_fifo_buffer_char->measure_read_throughput();
				} else if(found_fifo_buffer_voidp_with_metric) {
					curr_fifo_buffer_voidp->measure_read_throughput();
				} else if(found_fifo_buffer_msg_with_metric) {
					curr_fifo_buffer_msg->measure_read_throughput();
				} else {
					rpt_msg = "Unexpected situation in throughput metric report.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
			}
			else {
				rpt_msg = "Enabling of comm&synch metric ";
				rpt_msg += curr_tok;
				rpt_msg += " failed. Such comm&synch metric name is not recognized by KisTA.\n";
				rpt_msg += " Current supported comm&synch metrics are: (write) throughput (bps), (write) stationary_throughput (bps), read_throughput (bps), and read_stationary_throughput (bps).";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
			}
			
		}
	}

    xmlFreeDoc(doc);
										  
}

void create_MULTICUBE_error_output(char *xml_out_file, 
									bool fatal_for_exploration,
									const char *error_reason_msg) {

	xmlDocPtr doc = NULL;       		// document pointer
    xmlNodePtr root_node, node = NULL;	// node pointers
//    xmlDtdPtr dtd = NULL;      			// DTD pointer
	
    doc = xmlNewDoc((xmlChar *)"1.0");
    root_node = xmlNewNode(NULL, (xmlChar *)OUTPUT_ROOT_NODE_NAME);
    xmlNewProp(root_node,(xmlChar *)"xmlns",(xmlChar *)"http://www.multicube.eu/");
    xmlNewProp(root_node,(xmlChar *)"version",(xmlChar *)"1.4");
    xmlDocSetRootElement(doc, root_node);
    
    node = xmlNewChild(root_node, NULL, (xmlChar *)"error", NULL);
    
	xmlNewProp(node,(xmlChar *)"reason", (xmlChar *)error_reason_msg);	
	xmlNewProp(node,(xmlChar *)"kind", (xmlChar *)(fatal_for_exploration?"fatal":"non-fatal"));
    
    // Dumping document to file
    xmlSaveFormatFileEnc(xml_out_file, doc, "UTF-8", 1);
    
    // or to standard output
    // xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);

    xmlFreeDoc(doc);
}




//
//  The following code shows an example excerpt fo generate 
// an XML output enabling extensible metrics
// (However, it is not Multicube compatible
//

void create_KisTA_XML_output(char *xml_out_file, vector<scheduler*> &RTOS, vector<task_info_t*>	&app_task) {

	xmlDocPtr doc = NULL;       		// document pointer
    xmlNodePtr root_node, node = NULL;	// node pointers
 //   xmlDtdPtr dtd = NULL;      			// DTD pointer

    doc = xmlNewDoc((xmlChar *)"1.0");
    root_node = xmlNewNode(NULL, (xmlChar *)"output");
    xmlNewProp(root_node,(xmlChar *)"xmlns",(xmlChar *)"location to be defined");
    xmlNewProp(root_node,(xmlChar *)"version",(xmlChar *)"1.4");
    xmlDocSetRootElement(doc, root_node);
    
    for(unsigned int i=0; i<RTOS.size(); i++) {
		node = xmlNewChild(root_node, NULL, (xmlChar *)"rtos_rpt", NULL);
		xmlNewChild(node, NULL, (xmlChar *)"name", (xmlChar *)RTOS[i]->name());	
		xmlNewChild(node, NULL, (xmlChar *)"sched_utilization", (xmlChar *)to_string(RTOS[i]->get_scheduler_utilization()).c_str() );
		xmlNewChild(node, NULL, (xmlChar *)"plat_utilization", (xmlChar *)to_string(RTOS[i]->get_platform_utilization()).c_str() );
		xmlNewChild(node, NULL, (xmlChar *)"number_of_schedulings", (xmlChar *)to_string(RTOS[i]->get_number_of_schedulings()).c_str() );
//		xmlNewChild(node, NULL, (xmlChar *)"number_of_context_switches", (xmlChar *)to_string(RTOS[i]->get_number_of_context_switches()).c_str() );
// number_of_context_switches, not implemented yet in KisTA
		
		xmlNewChild(node, NULL, (xmlChar *)"starvation", (xmlChar *)(RTOS[i]->assess_starvation()?"true":"false"));
		xmlNewChild(node, NULL, (xmlChar *)"miss_deadlines", (xmlChar *)(RTOS[i]->miss_deadlines()?"true":"false"));
    }

    for(unsigned int i=0; i<app_task.size(); i++) {
		node = xmlNewChild(root_node, NULL, (xmlChar *)"task_rpt", NULL);
		xmlNewChild(node, NULL, (xmlChar *)"name", (xmlChar *)app_task[i]->name());	
		xmlNewChild(node, NULL, (xmlChar *)"task_utilization", (xmlChar *)to_string(app_task[i]->get_scheduler()->get_task_utilization(app_task[i]->name())).c_str() );
		
//		xmlNewChild(node, NULL, (xmlChar *)"avg_response_time", (xmlChar *)app_task[i]->avg_response_time().to_string().c_str() );
// avg_response_time not implemented yet
//		xmlNewChild(node, NULL, (xmlChar *)"std_dev_response_time", (xmlChar *)app_task[i]->std_dev_response_time().to_string().c_str() );
// std_dev_response_time not implemented yet
//		xmlNewChild(node, NULL, (xmlChar *)"max_response_time", (xmlChar *)app_task[i]->max_response_time().to_string().c_str() );

//		xmlNewChild(node, NULL, (xmlChar *)"location_of_worst_case_response_time", (xmlChar *)app_task[i]->location_of_worst_case_response_time().to_string().c_str() );
		
		xmlNewChild(node, NULL, (xmlChar *)"miss_deadline", (xmlChar *)(app_task[i]->miss_deadline()?"true":"false"));		
	}

    // Dumping document to file
    xmlSaveFormatFileEnc(xml_out_file, doc, "UTF-8", 1);
    
    // or to standard output
    // xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);

    xmlFreeDoc(doc);

}

#endif

