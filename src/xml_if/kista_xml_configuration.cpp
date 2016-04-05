/*****************************************************************************

  kista_xml_configuration.cpp
  
   Functions for supported by kista-xml front-end to configure KisTA simulation.
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 August

 *****************************************************************************/

#ifndef _KISTA_XML_CONFIGURATION_CPP
#define _KISTA_XML_CONFIGURATION_CPP

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <vector>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <systemc.h>

#include <kista.hpp>

// If a simulation time configuration is configured, it creates an sc_time object and returns true
// return false otherwise
bool get_configured_sim_time(xmlDocPtr doc, // the XML "handler"
							  sc_time &sim_time
							) {	
	xmlNodeSetPtr simulation_time_nodes;
	xmlXPathObjectPtr  XPathObjectPtr;
	
	std::string msg_rpt;
	
	// get the set of simulation_time configurations
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)"/system/kista_configuration/simulation_time");
	if(XPathObjectPtr==NULL) {
		if(global_kista_xml_verbosity) {
			SC_REPORT_WARNING("Kista-XML","No simulation time configuration available. An sc_start() will be done (simulation ends with ends of events)");
		}
		return false;
	} else {
		simulation_time_nodes = XPathObjectPtr->nodesetval;
		
		if(global_kista_xml_verbosity) {
			if(simulation_time_nodes->nodeNr>1) {
				SC_REPORT_WARNING("Kista-XML","More than one simulation set detected. KisTA-XML will take the former one.");
			}
		}

		if(simulation_time_nodes->nodeNr<1) {
			SC_REPORT_ERROR("Kista-XML","Unexpected situation on simulation time configuration.");
		}
		sim_time = readTime(simulation_time_nodes->nodeTab[0]);	

		if(global_kista_xml_verbosity) {
			cout << "kista-XML INFO: Configuring simulation time: " << sim_time << endl;
		}

		return true;
	}
}

bool get_configured_kista_xml_verbosity(xmlDocPtr doc) {
	
	get_configured_boolean_option(doc,
								"/system/kista_configuration/xml_front_end_verbose", // where to find boolean property
								"verbosity of KisTA XML front-end", // description of the boolean property
								false // default value if not found in XML system description
								);
}

bool get_configured_kista_verbosity(xmlDocPtr doc) {
	
	get_configured_boolean_option(doc,
								"/system/kista_configuration/verbose", // where to find boolean property
								"verbosity of KisTA library", // description of the boolean property
								false // default value if not found in XML system description
								);
}

bool get_configured_export_sketch(xmlDocPtr doc) {
	
	get_configured_boolean_option(doc,
								"/system/kista_configuration/export_sketch", // where to find boolean property
								"export sketch enabled", // description of the boolean property
								false // default value if not found in XML system description
								);
}

const char *get_configured_sketch_file_name(xmlDocPtr doc) {
	
	get_configured_string_option(doc,
								"/system/kista_configuration/export_sketch/file_name", // where to find the property
								"sketch file name", // description of the property
								"system_sketch" // default value if not found in XML system description
								);
}

bool get_configured_sketch_draw_sys_level_conn_names(xmlDocPtr doc) {
	
	get_configured_boolean_option(doc,
								"/system/kista_configuration/export_sketch/draw_sys_level_conn_names", // where to find boolean property
								"draw system-level connection names when exporting a sketch of the KisTA model", // description of the boolean property
								false // default value if not found in XML system description
								);
}

bool get_configured_sketch_highlight_environment(xmlDocPtr doc) {
	
	get_configured_boolean_option(doc,
								"/system/kista_configuration/export_sketch/highlight_environment", // where to find boolean property
								"draw box around environment tasks when exporting a sketch of the KisTA model", // description of the boolean property
								false // default value if not found in XML system description
								);
}

bool get_configured_sketch_highlight_system(xmlDocPtr doc) {
	
	get_configured_boolean_option(doc,
								"/system/kista_configuration/export_sketch/highlight_system", // where to find boolean property
								"draw box around system elements when exporting a sketch of the KisTA model", // description of the boolean property
								false // default value if not found in XML system description
								);
}


bool get_configured_sketch_highlight_application(xmlDocPtr doc) {
	
	get_configured_boolean_option(doc,
								"/system/kista_configuration/export_sketch/highlight_application", // where to find boolean property
								"draw box around application tasks when exporting a sketch of the KisTA model", // description of the boolean property
								false // default value if not found in XML system description
								);
}


bool get_configured_sketch_highlight_platform(xmlDocPtr doc) {
	
	get_configured_boolean_option(doc,
								"/system/kista_configuration/export_sketch/highlight_platform", // where to find boolean property
								"draw box around platform elements when exporting a sketch of the KisTA model", // description of the boolean property
								false // default value if not found in XML system description
								);
}

bool get_configured_sketch_only_image(xmlDocPtr doc) {
	
	get_configured_boolean_option(doc,
								"/system/kista_configuration/export_sketch/only_image", // where to find boolean property
								"draw only the image, without the text reporting how the image was generated", // description of the boolean property
								false // default value if not found in XML system description
								);
}

const char *get_configured_sketch_scale(xmlDocPtr doc) {
	get_configured_string_option(doc,
								"/system/kista_configuration/export_sketch/set_scale", // where to find the property
								"sketch scale", // description of the property
								"1.0" // default value if not found in XML system description
								);
}

bool get_configured_functional_validation(xmlDocPtr doc) {

	get_configured_boolean_option(doc,
								"/system/kista_configuration/functional_validation", // where to find boolean property
								"functional validation enabled", // description of the boolean property
								false // default value if not found in XML system description
								);
}

bool get_configured_error_fatality_for_exploration(xmlDocPtr doc) {

	get_configured_boolean_option(doc,
								"/system/kista_configuration/error_fatality_for_exploration", // where to find boolean property
								"error fatality for exploration", // description of the boolean property
								false // default value if not found in XML system description
								);
}

bool get_configured_energy_power_analysis(xmlDocPtr doc) {
	
	get_configured_boolean_option(doc,
								"/system/kista_configuration/analysis/energy_power", // where to find boolean property
								"energy power analysis enabled", // description of the boolean property
								false // default value if not found in XML system description
								);
}

// If an average time for power configuration is provided
// it an sc_time object and returns true
// return false otherwise
bool get_configured_global_power_averaging_time(xmlDocPtr doc, // the XML "handler"
							  sc_time &averaging_time
							) {	
	xmlNodeSetPtr global_power_averaging_time_nodes;
	xmlXPathObjectPtr  XPathObjectPtr;
	
	std::string msg_rpt;
	
	// get the set of simulation_time configurations
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)"/system/kista_configuration/analysis/energy_power/power_averaging_time");
	if(XPathObjectPtr==NULL) {
		if(global_kista_xml_verbosity) {
			msg_rpt = "No global averaging time was configured. Taking the default: ";
			msg_rpt += averaging_time.to_string();
			msg_rpt += " s.";
			SC_REPORT_WARNING("Kista-XML",msg_rpt.c_str());
		}
		return false;
	} else {
		global_power_averaging_time_nodes = XPathObjectPtr->nodesetval;
		
		if(global_kista_xml_verbosity) {
			if(global_power_averaging_time_nodes->nodeNr>1) {
				SC_REPORT_WARNING("Kista-XML","More than one setting of averaging time for power measurement detected. KisTA-XML will take the former one.");
			}
		}

		if(global_power_averaging_time_nodes->nodeNr<1) {
			SC_REPORT_ERROR("Kista-XML","Unexpected situation on configuration of averaging time for power measurement.");
		}
		averaging_time = readTime(global_power_averaging_time_nodes->nodeTab[0]);	

		if(global_kista_xml_verbosity) {
			cout << "kista-XML INFO: Configuring avaraging time for global power measurement: " << averaging_time << endl;
		}
		
		return true;
	}
}

#endif
