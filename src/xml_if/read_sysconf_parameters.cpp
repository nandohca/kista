/*****************************************************************************

  read_parameters.cpp
  
   Functions for reading XML parameters
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 July

 *****************************************************************************/

#ifndef _READ_SYSCONF_PARAMETERS_CPP
#define _READ_SYSCONF_PARAMETERS_CPP

xmlNodeSetPtr getParameterConfigurations(xmlDocPtr doc, const char *XPathExpression) {
	xmlNodeSetPtr nodeset;	
	xmlXPathObjectPtr  XPathObjectPtr;
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)XPathExpression);
	if(XPathObjectPtr==NULL) {
		if(global_kista_xml_verbosity) {
			printf("Kista-XML INFO: No parameter configuration was read in the XML input.\n");
		}
		return NULL;
	} else {
		nodeset = XPathObjectPtr->nodesetval;
		return nodeset;
	}
}

void read_sysconf_parameters(xmlDocPtr doc, // the XML "handler"
				const char *XPathExpression, // Expression of the XML tree where to search parameters
				global_parameters_t &global_params // the global structure where kista-xml stores the parameters,
				                                   // which has to be filled before constructing the system
				) {
	
	int i;
	xmlNodeSetPtr setParamConfnodes;
	xmlNodePtr ItemNodePtr;
	xmlChar *ParamName;
	xmlChar *ParamValue;
	param_value_t *ParamValueSetPtr;
	
	std::string msg_rpt;
	
	if(global_kista_xml_verbosity) {
		printf("kista-XML INFO: Scanning Parameters.\n");
	}

	// get the set of parameter configurations
	setParamConfnodes = getParameterConfigurations(doc,XPathExpression);

	
	if(setParamConfnodes!=NULL) {
		
		for(i=0; i< setParamConfnodes->nodeNr ; i++) {
		   
		   ParamName = xmlGetProp(setParamConfnodes->nodeTab[i],(xmlChar *)"name");
		   
		   if(ParamName==NULL) {
			   msg_rpt = "The ";
			   msg_rpt += to_string(i);
			   msg_rpt += "-th parameter configuration does not have parameter name.";
               SC_REPORT_ERROR("kista-XML",msg_rpt.c_str());
		   }
			   
		   ParamValue = xmlGetProp(setParamConfnodes->nodeTab[i],(xmlChar *)"value");
		
		   ParamValueSetPtr = new param_value_t; // in the creation it takes size=0
					      
		   // If the parsers sees a "value" filled, it takes it and overrides any other
		   // parameterization 
		   if(ParamValue!=NULL) {
			   // add the parameter value

			   ParamValueSetPtr->push_back((const char *)ParamValue);
			   
			   global_params[(const char *)ParamName] = ParamValueSetPtr;
			   
		   } else {
				if(global_kista_xml_verbosity) {
					msg_rpt = "In the configuration of parameter ";
					msg_rpt += (const char *)ParamName;			   
					msg_rpt += ". There is no parameter value.";
					msg_rpt += "KisTA-XML will look for a vector definition.";
					SC_REPORT_INFO("kista-XML",msg_rpt.c_str());
				}

				if(hasChild(setParamConfnodes->nodeTab[i] , "item")) {
					ItemNodePtr=setParamConfnodes->nodeTab[i]->xmlChildrenNode;
					while (ItemNodePtr != NULL) {
						if ((!xmlStrcmp(ItemNodePtr->name, (const xmlChar *)"item"))){
							ParamValue = xmlGetValueGen(ItemNodePtr,(xmlChar *)"value");
							if(ParamValue==NULL) {
								msg_rpt = "In the configuration of parameter ";
								msg_rpt += (const char *)ParamName;
								msg_rpt += ". No value found in an item of the vector value.";
								SC_REPORT_ERROR("KisTA-XML",msg_rpt.c_str());
							} else {
								ParamValueSetPtr->push_back((const char *)ParamValue);
							}
						}
						ItemNodePtr = ItemNodePtr->next;
					}
					global_params[(const char *)ParamName] = ParamValueSetPtr;
				}
				else {
					if(global_kista_xml_verbosity) {					
						msg_rpt = "In the configuration of parameter ";
						msg_rpt += (const char *)ParamName;	
						msg_rpt += ". Neither a scalar value, nor a valid vector description (item-value pair list) has been provided.";
						SC_REPORT_WARNING("kista-XML",msg_rpt.c_str());					
					}
				}	
		   }		   
		}
	}
}


// Overload to read the parameters directly from an independent file

void read_sysconf_parameters(const char *sc_file_name,
							 global_parameters_t &global_params) {
	xmlDocPtr sc_doc;
	std::string rpt_msg;
	
	sc_doc = xmlParseFile(sc_file_name);
	
	if (sc_doc == NULL ) {
		rpt_msg = "While parsing the System Configuration file (which defines system parameters): ";
		rpt_msg += sc_file_name;
		rpt_msg += ".";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}
	
	// As can be seen, in this case, multicube compatibility
	// read_sysconf_parameters(sc_doc,"/simulator_input_interface/parameter",global_params);
	
	// A first alternative would be to perform
	// a xpath Query aware of the multicube namespace 
	//
	// read_sysconf_parameters(sc_doc,"/m3:simulator_input_interface/m3:parameter",global_params);
	//
	//  ... this requires to use xpath to set the m3 prefix associated to the "www.multicube.eu" namespace
	
	// A second alternative is to perform a query regardless the
	// "www.multicube.eu" namespace
	read_sysconf_parameters(sc_doc,"/*[local-name()='simulator_input_interface']/*[local-name()='parameter']",global_params);
}

#endif
