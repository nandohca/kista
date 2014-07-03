/*****************************************************************************

  create_hw_platform.cpp
  
   Functions for generating the hw platform in the kista-xml front-end.
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 July

 *****************************************************************************/

#ifndef _CREATE_HW_PLATFORM_CPP
#define _CREATE_HW_PLATFORM_CPP

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

#include <libxml/tree.h>
/*
 * #include <libxml/xmlversion.h>
#include <libxml/tree.h>
//#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
*/

#include <vector>

#include <systemc.h>

#include <kista.hpp>

#define PE_XPATH "/system/platform/HW_platform/HW_Architecture/PE"
#define COMM_RES_XPATH "/system/platform/HW_platform/HW_Architecture/comm_res"
// relative search
#define CHILD_HW_CONNECTION_XPATH "//HW_Connection"
//#define CHILD_HW_CONNECTION_XPATH "./HW_Connection"
//#define CHILD_HW_CONNECTION_XPATH "./HW_Connection"
//#define CHILD_HW_CONNECTION_XPATH ".//HW_Connection"
// (that would be all the descendants)

// --------------------------------------
// FUNCTIONS FOR PROCESSING ELEMENTS
// --------------------------------------

// read the frequency element
unsigned int getCPI(xmlDocPtr doc,xmlNodePtr cur) {
	xmlChar *valueStr;
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"CPI"))){
			valueStr = xmlGetValueGen(curChildNodePtr,(xmlChar *)"value");
			if(valueStr==NULL) {
				if(global_kista_xml_verbosity) {
					printf("kista-xml WARNING: CPI specified, but not value attribute given.");
					printf("KisTA-xml will assign CPI=1 by default.\n");
				}
				return 1;
			} else {
				return(atoi((const char *)valueStr));
			}
		}
		curChildNodePtr = curChildNodePtr->next;
	}

	if(global_kista_xml_verbosity) {
		printf("kista-XML INFO: No CPI specified for PE.");
		printf("KisTA will assign CPI=1 by default.\n");
	}

	return 1;
}

// detects if the current element has a frequency element
bool has_freq(xmlDocPtr doc, xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"freq"))){
			return true;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return false;
}

double readFreqMHz(xmlDocPtr doc,xmlNodePtr cur) {
	xmlChar *unitStr;
	xmlChar *valueStr;
	double return_value;
	
//	valueStr = xmlGetProp(cur,(xmlChar *)"value");
	valueStr = xmlGetValueGen(cur,(xmlChar *)"value");
	
	if(valueStr==NULL) {
		// Detects a frequency specification without value (in case schema validation for this mandatory field is not done)
		SC_REPORT_ERROR("KisTA-XML","Frequency specification without value.");
	}
	
	return_value = atof((const char *)valueStr);
	
	unitStr = xmlGetValueGen(cur,(xmlChar *)"unit");
	
	// conversion to MHz (only if there is unit specification and it is distinct from MHz
	return_value = conv_value_by_unit(return_value,unitStr,(xmlChar *)"M");
	
	return return_value;
}

// get the (first) frequency element
double get_freq_MHz(xmlDocPtr doc,xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"freq"))){
			return readFreqMHz(doc, curChildNodePtr);
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
}

// --------------------------------------
// NETWORK INTERFACE CONFIGURATION
// --------------------------------------

// detects if the current element has a frequency element
bool has_netif_flag(xmlDocPtr doc, xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"netif"))){
			return true;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return false;
}


bool read_netif_flag(xmlDocPtr doc,xmlNodePtr cur) {
	xmlChar *valueStr;
	
//	valueStr = xmlGetProp(cur,(xmlChar *)"value");
	valueStr = xmlGetValueGen(cur,(xmlChar *)"value");
	
	if(valueStr==NULL) {
		// Detects a frequency specification without value (in case schema validation for this mandatory field is not done)
		SC_REPORT_ERROR("KisTA-XML","network interface specification without value.");
	}
	
	if(!strcmp((const char *)valueStr,"true")) {
		return true;
	} else {
		return false;
	}

}

// get the netif_flag element
xmlNodePtr get_netif_node(xmlDocPtr doc,xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"netif"))){
			return curChildNodePtr;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return NULL;
}

// --------------------------------------------------------------------------------------

// detects if the current element has a frequency element
bool has_tx_buffer_size(xmlDocPtr doc, xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"tx_buffer_size"))){
			return true;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return false;
}

// get the netif_flag element
xmlNodePtr get_tx_buffer_size_node(xmlDocPtr doc,xmlNodePtr cur) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"tx_buffer_size"))){
			return curChildNodePtr;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return NULL;
}

// --------------------------------------
// PROCESSING ELEMENTS
// --------------------------------------

bool ThereIsPEs(xmlDocPtr doc) {
	xmlXPathObjectPtr  XPathObjectPtr;
	XPathObjectPtr=getNodeSet(doc,(xmlChar *)PE_XPATH);
	if(XPathObjectPtr==NULL) return false;
	else return true;
}

// get all processing elements from the platform
xmlNodeSetPtr getPEs(xmlDocPtr doc) {
	xmlNodeSetPtr nodeset;	
	xmlXPathObjectPtr  XPathObjectPtr;
	XPathObjectPtr=getNodeSet(doc,(xmlChar *)PE_XPATH);
	nodeset = XPathObjectPtr->nodesetval;
	return nodeset;
}

unsigned int getNumberOfPEs(xmlDocPtr doc) {
	xmlNodeSetPtr setPEnodes;
	setPEnodes=getPEs(doc);
	return setPEnodes->nodeNr;
}

// --------------------------------------
// FUNCTIONS FOR COMMUNICATION RESOURCES
// --------------------------------------

bool ThereIsCommResources(xmlDocPtr doc) {
	xmlXPathObjectPtr  XPathObjectPtr;
	XPathObjectPtr=getNodeSet(doc,(xmlChar *)COMM_RES_XPATH);
	if(XPathObjectPtr==NULL) return false;
	else return true;
}

// get all processing elements from the platform
xmlNodeSetPtr getCommResources(xmlDocPtr doc) {
	xmlNodeSetPtr nodeset;	
	xmlXPathObjectPtr  XPathObjectPtr;
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)COMM_RES_XPATH);
	nodeset = XPathObjectPtr->nodesetval;	
	return nodeset;
}

unsigned int getNumberOfCommResources(xmlDocPtr doc) {
	xmlNodeSetPtr setCommResnodes;
	setCommResnodes=getCommResources(doc);
	return setCommResnodes->nodeNr;
}


// --------------------------------------
// FUNCTIONS FOR HW CONNECTIONS RESOURCES
// --------------------------------------

bool ThereIsHWConnectionsInNode(xmlNodePtr node_par, xmlDocPtr doc_par) {
	xmlXPathContextPtr	new_context;
	xmlXPathObjectPtr	XPathObjectPtr;
	
	new_context = xmlXPathNewContext(doc_par);
/*
	xmlXPathSetContextNode(node_par,new_context);
 	XPathObjectPtr = xmlXPathEval(
						 (const xmlChar *)CHILD_HW_CONNECTION_XPATH, 
						 new_context);
*/	
 	XPathObjectPtr = xmlXPathNodeEval(node_par, 
						 (const xmlChar *)CHILD_HW_CONNECTION_XPATH, 
						 new_context);

	if(XPathObjectPtr==NULL) return false;
	else return true;
}

// get all processing elements from the platform
xmlNodeSetPtr getHWConnectionsFromNode(xmlNodePtr node_par, xmlDocPtr doc_par) {

	xmlXPathContextPtr	new_context;
	xmlXPathObjectPtr	XPathObjectPtr;
	xmlNodeSetPtr		nodeset;

	new_context = xmlXPathNewContext(doc_par);	
/*
	xmlXPathSetContextNode(node_par,new_context);
 	XPathObjectPtr = xmlXPathEval(
						 (const xmlChar *)CHILD_HW_CONNECTION_XPATH, 
						 new_context);
*/	
 	XPathObjectPtr = xmlXPathNodeEval(node_par, 
						 (const xmlChar *)CHILD_HW_CONNECTION_XPATH, 
						 new_context);
						 						 
	nodeset = XPathObjectPtr->nodesetval;	
	return nodeset;						 
}

unsigned int getNumberOfHWConnections(xmlNodePtr node, xmlDocPtr doc) {
	xmlNodeSetPtr setHWConnections;
	std::string rpt_msg;
	setHWConnections=getHWConnectionsFromNode(node,doc);
	return setHWConnections->nodeNr;
}


void hw_connections_from_node(const char *ComResName_par, 
								const char *ComResType_par,
								phy_comm_res_t *commres_p,
								xmlNodePtr node, xmlDocPtr doc) {
	std::string rpt_msg;
	unsigned int j;
	
	xmlNodeSetPtr setHWConn;
	const char *HwConnName;
	const char *InstanceName;
	const char *PropertyVal;

	if(ThereIsHWConnectionsInNode(node,doc)) {
		setHWConn = getHWConnectionsFromNode(node,doc);

#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {	
			rpt_msg = "Number of HW connections to ";
			rpt_msg += ComResName_par;
			rpt_msg = " communication resource: ";
			rpt_msg += to_string(getNumberOfHWConnections(node,doc));
			SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
		}
#endif	

		for(j=0; j< getNumberOfHWConnections(node,doc) ; j++) {	
			HwConnName = (const char *)xmlGetValueGen(setHWConn->nodeTab[j],(xmlChar *)"name");
			InstanceName = (const char *)xmlGetValueGen(setHWConn->nodeTab[j],(xmlChar *)"instance");
#ifdef _VERBOSE_KISTA_XML
			if(global_kista_xml_verbosity) {
				rpt_msg = "Connection ";
				rpt_msg += HwConnName;
				rpt_msg += " binding processing element ";
				rpt_msg += InstanceName;
				rpt_msg += " to communication resource ";
				rpt_msg += ComResName_par;
				SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
			}
#endif	
			// example: PE1.connect(&tdmab1);
			// now, retrieves the processing element by name
			//PEref_by_name[InstanceName]->connect(phy_commres_by_name[ComResName_par]);
			PEref_by_name[InstanceName]->connect(commres_p);
		}	
		
		// For connections to a TDMA bus 
		if(!strcmp(ComResType_par,"tdma_bus")) {
			// For the connections of PEs to a TDMA bus, the slots associated to the PE for writing is read
			for(j=0; j< getNumberOfHWConnections(node,doc) ; j++) {
				InstanceName = (const char *)xmlGetValueGen(setHWConn->nodeTab[j],(xmlChar *)"instance");
				PropertyVal = (const char *)xmlGetValueGen(setHWConn->nodeTab[j],(xmlChar *)"slots_number");
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {
					rpt_msg = "Allocating ";
					rpt_msg += PropertyVal;
					rpt_msg += " slots to processing element ";
					rpt_msg += InstanceName;
					SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
				}
#endif			
				//	e.g., tdmab1.allocate_channels(&PE1,3);
				((tdma_bus *)commres_p)->allocate_channels(PEref_by_name[InstanceName],atoi(PropertyVal));
			}
		}
		
	} else {
		rpt_msg = "No HW connections from communication resource. Currently this is the only way in KisTA to connect PEs to communication resources.";
		rpt_msg += "So it should appear if a communication resource is specified.";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}

}


// Read TDMA bus attribures assuming that they are provided as atributes (properties) in the same node
void read_and_set_tdma_bus_attributes(tdma_bus *tdma_bus_p, xmlNodePtr node, xmlDocPtr doc) {
	
	xmlChar *unitStr;
	xmlChar *valueStr;
	int int_value;
	double double_value;
	sc_time time_value;
	
	std::string rpt_msg;
	xmlNodePtr	child_node;
	
	// reads number of slots (as a mandatory attribute)
	// --------------------------------------------------

/*
	if(hasChild(node, "slots_number")) {
		child_node = getFirstChild("slots_number");
	}
	
	valueStr = xmlGetValueGen(child_node,(xmlChar *)"slots_number");
*/

	// A more efficient coding
	child_node = getFirstChild(node,"n_channels");
	if(child_node == NULL) { // alias for n_channels
		child_node = getFirstChild(node,"slots_number");
	}
	
	if(child_node != NULL) {
		valueStr = xmlGetValueGen(child_node,(xmlChar *)"value");
		if(valueStr!=NULL) {
			int_value = atoi((const char *)valueStr);
			tdma_bus_p->set_n_channels(int_value);
#ifdef _VERBOSE_KISTA_XML
			if(global_kista_xml_verbosity) {		
				rpt_msg = "Setting TDMA bus ";
				rpt_msg += tdma_bus_p->name();
				rpt_msg += " parameter: number of channels or slots=";
				rpt_msg += (const char *)valueStr;
				SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
			}
#endif
		}

	} else {
		rpt_msg = "The number of slots or channels of the TDMA bus ";
		rpt_msg += tdma_bus_p->name();
		rpt_msg += " has not been stablished. Set n_channels attribute.";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());		
	}
	
	// reads slot time (as a mandatory attribute)
	// --------------------------------------------------
	
	child_node = getFirstChild(node,"slot_time");
		
	if(child_node != NULL) {
		
		time_value = readTime(child_node);
		
		tdma_bus_p->set_slot_time(time_value);
			
#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {		
			rpt_msg = "Setting TDMA bus ";
			rpt_msg += tdma_bus_p->name();
			rpt_msg += " parameter: slot time=";
			rpt_msg += time_value.to_string();
			//~ if(unitStr==NULL) {
				//~ rpt_msg += " (seconds taken as default unit)";
			//~ }
			SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
		}
#endif

	} else {
		rpt_msg = "The slot time of the TDMA bus ";
		rpt_msg += tdma_bus_p->name();
		rpt_msg += " has not been stablished. Set slot_time attribute.";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());		
	}


	// reads slot/channel payload
	// --------------------------------------------------
	child_node = getFirstChild(node,"slot_payload");
	if(child_node == NULL) { // alias for channel payload
		child_node = getFirstChild(node,"payload");
	}
		
	if(child_node != NULL) {
		valueStr = xmlGetValueGen(child_node,(xmlChar *)"value");

		unitStr = xmlGetProp(child_node,(xmlChar *)"unit");
		
		// in case there is actually some unit configuration, executes the code
		// to enable unit parametrization too
		if(unitStr!=NULL) {
			unitStr = xmlGetProp(child_node,(xmlChar *)"unit");
		}
		
		if(valueStr!=NULL) {
			int_value = atoi((const char *)valueStr);
			
			if( (unitStr!=NULL)) {
				if(!strcmp((const char *)unitStr,"bit")) {
					tdma_bus_p->set_channel_payload_bits(int_value);
				} else if (!strcmp((const char *)unitStr,"byte")) {
					tdma_bus_p->set_channel_payload_bits(int_value*8);
				} else {
					rpt_msg = "Unrecognized unit provided for the slot payload attributed of the TDMA bus ";
					rpt_msg += tdma_bus_p->name();
					rpt_msg += ". Valid values are bit or byte. You can also omit the unit setting and bit will be taken as default unit.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
			} else { // if not value given, bits taken as default unit
				tdma_bus_p->set_channel_payload_bits(int_value);	
			}
	
#ifdef _VERBOSE_KISTA_XML
			if(global_kista_xml_verbosity) {		
				rpt_msg = "Setting TDMA bus ";
				rpt_msg += tdma_bus_p->name();
				rpt_msg += " parameter: channel payload=";
				rpt_msg += (const char *)valueStr;
				rpt_msg += " ";
				if(unitStr==NULL) {
					rpt_msg += "bits (default unit)";
				} else {
					rpt_msg += (const char *)unitStr;
				}
				SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
			}
#endif
		}

	}
	
	
	// reads slot/channel capacity
	// --------------------------------------------------
	child_node = getFirstChild(node,"slot_capacity");

	if(child_node != NULL) {
		valueStr = xmlGetValueGen(child_node,(xmlChar *)"value");

		unitStr = xmlGetProp(child_node,(xmlChar *)"unit");
		
		// in case there is actually some unit configuration, executes the code
		// to enable unit parametrization too
		if(unitStr!=NULL) {
			unitStr = xmlGetProp(child_node,(xmlChar *)"unit");
		}
		
		if(valueStr!=NULL) {
			int_value = atoi((const char *)valueStr);
			
			if( (unitStr!=NULL)) {
				if(!strcmp((const char *)unitStr,"bit")) {
					tdma_bus_p->set_channel_capacity(int_value);
				} else if (!strcmp((const char *)unitStr,"byte")) {
					tdma_bus_p->set_channel_capacity(int_value*8);
				} else {
					rpt_msg = "Unrecognized unit provided for the slot capacity attributed of the TDMA bus ";
					rpt_msg += tdma_bus_p->name();
					rpt_msg += ". Valid values are bit or byte. You can also omit the unit setting and bit will be taken as default unit.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
			} else { // if not value given, bits taken as default unit
				tdma_bus_p->set_channel_capacity(int_value);
			}
	
#ifdef _VERBOSE_KISTA_XML
			if(global_kista_xml_verbosity) {		
				rpt_msg = "Setting TDMA bus ";
				rpt_msg += tdma_bus_p->name();
				rpt_msg += " parameter: slot capacity=";
				rpt_msg += (const char *)valueStr;
				rpt_msg += " ";
				if(unitStr==NULL) {
					rpt_msg += "bits (default unit)";
				} else {
					rpt_msg += (const char *)unitStr;
				}
				SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
			}
#endif
		}

	}
	
	// reads slot/channel payload rate
	// --------------------------------------------------
	child_node = getFirstChild(node,"slot_payload_rate");

	if(child_node != NULL) {
		valueStr = xmlGetValueGen(child_node,(xmlChar *)"value");
				
		if(valueStr!=NULL) {
			double_value = atof((const char *)valueStr);
			tdma_bus_p->set_channel_payload_rate(double_value);	
		}

#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {		
			rpt_msg = "Setting TDMA bus ";
			rpt_msg += tdma_bus_p->name();
			rpt_msg += " parameter: slot capacity rate=";
			rpt_msg += (const char *)valueStr;
			SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
		}
#endif

	}
	
	// reads cycle time (if any)
	// --------------------------

	child_node = getFirstChild(node,"cycle_time");

	if(child_node != NULL) {
		
		time_value = readTime(child_node);
		
		tdma_bus_p->set_cycle_time(time_value);
			
#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {		
			rpt_msg = "Setting TDMA bus ";
			rpt_msg += tdma_bus_p->name();
			rpt_msg += " parameter: cycle time=";
			rpt_msg += time_value.to_string();
			if(unitStr==NULL) {
				rpt_msg += " (seconds taken as default unit)";
			}
			SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
		}
#endif

	}
	
	
	// reads accuracy level (if any)
	// -------------------------------
	child_node = getFirstChild(node,"accuracy_level");
	if(child_node == NULL) { // alias for accuracy_level
		child_node = getFirstChild(node,"accuracy");
	}
	
	if(child_node != NULL) {
		valueStr = xmlGetValueGen(child_node,(xmlChar *)"value");
		if(valueStr!=NULL) {
			int_value = atoi((const char *)valueStr);
			tdma_bus_p->set_accuracy_level((accuracy_level_t)int_value);
#ifdef _VERBOSE_KISTA_XML
			if(global_kista_xml_verbosity) {		
				rpt_msg = "Setting TDMA bus ";
				rpt_msg += tdma_bus_p->name();
				rpt_msg += " parameter: accuracy level=";
				rpt_msg += (const char *)valueStr;
				SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
			}
#endif
		}

	} else {
#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {		
			rpt_msg = "Accuracy Level for ";
			rpt_msg += tdma_bus_p->name();
			rpt_msg += " bus not stablished. Default one is 0 (slot resolution for max. p2p delay).";
			SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());		
		}
#endif
	}	
}

// --------------------------------------
// CREATE HW PLATFORM
// --------------------------------------

void create_HW_platform(xmlDocPtr doc,
							std::vector<processing_element*> &PE_cluster
						) {
	unsigned int j;
	std::string rpt_msg;
	
	// variables for PE nodes
	xmlNodeSetPtr setPEnodes;
	xmlChar *PEName;
	unsigned int PE_CPI;
	double	PE_freq_MHz;
	unsigned int PE_clock_period_ns; // currently, KisTA reads clock period, instead frequency
	bool net_if_flag;
	
	processing_element *PE_ptr;
	
	xmlNodePtr curNetIfNode, curNetIfAttribute;
	
	xmlChar *TxBufferSizeName, *TxBufferSizeUnit;
	//xmlChar *unitStr;
	unsigned int tx_buffer_size;
	
	// variables for communication resource nodes
	xmlNodeSetPtr	setCommRes;
	const char *CommResName;
	const char *CommResType;
	

	// communication resources supported by the XML interface
	// (so far only 1 communication resource type modelled)
	 // NOTE: global structures for them is not required in the XML interface
	 //       because KisTA already create and mantains such structures
	phy_comm_res_t *phy_comm_res_p;
	tdma_bus	   *tdma_bus_p;
	
	if(ThereIsPEs(doc)) {
		setPEnodes = getPEs(doc);
		// setPENodes should not be NULL here because we have already checked that there were nodes in the tree
		
		for(j=0; j< getNumberOfPEs(doc) ; j++) {
		   
		   PEName = xmlGetValueGen(setPEnodes->nodeTab[j],(xmlChar *)"name");

		   // read name of PE i exists. In case there is one in the XML file it creates the new PE elements passigng such
		   // a name, otherwise, a no-name constructor which generates unique names is created
		   if(PEName==NULL) {
			   PE_ptr = new processing_element;
#ifdef _VERBOSE_KISTA_XML
			if(global_kista_xml_verbosity) {
				rpt_msg = "Creating PE.";
				rpt_msg += "No name found in XML file, KisTA will assign an unique name for the PE.";
				SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
			}
#endif
			} else {
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {
					rpt_msg = "Creating PE ";
					rpt_msg += (const char *)PEName;
					SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
				}
#endif	
			   PE_ptr = new processing_element((const char *)PEName);
			}

			// Set frequency
			if(has_freq(doc,setPEnodes->nodeTab[j])) {
				PE_freq_MHz = get_freq_MHz(doc,setPEnodes->nodeTab[j]);
				PE_clock_period_ns = (unsigned int)(1000.0*(1/PE_freq_MHz)+0.5);
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {					
					rpt_msg = "PE ";
					rpt_msg += (const char *)PEName;
					rpt_msg += " frequency = ";
					rpt_msg += PE_freq_MHz;
					rpt_msg += " (MHz). Setting clock period to ";
					rpt_msg += std::to_string(PE_clock_period_ns);
					rpt_msg += " ns.";
					SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());					
				}
#endif		
				PE_ptr->set_clock_period_ns(PE_clock_period_ns);
			} else {
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {
					rpt_msg = "No frequency for PE ";
					rpt_msg += (const char *)PEName;
					rpt_msg += " settled. KisTA sets 20ns as default clock PE period.";
					SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());					
				}
#endif
			}		

			// Set cycles per instruction
			PE_CPI = getCPI(doc, setPEnodes->nodeTab[j]);	// read from XML file
			PE_ptr->set_CPI(PE_CPI); 							// configure the element
#ifdef _VERBOSE_KISTA_XML
			if(global_kista_xml_verbosity) {
				rpt_msg = "PE ";
				rpt_msg += (const char *)PEName;
				rpt_msg += " CPI= ";
				rpt_msg += std::to_string(PE_CPI);
				rpt_msg += ".";
				SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
			}
#endif		

			// Set network interface flag
			if(has_netif_flag(doc,setPEnodes->nodeTab[j])) {
				curNetIfNode = get_netif_node(doc,setPEnodes->nodeTab[j]);
				 
				// here curNetifNode is expected to not to be NULL, since it was already checked in the previous if clause
				 
				net_if_flag = read_netif_flag(doc,curNetIfNode);
				
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {
					rpt_msg = "PE ";
					rpt_msg += (const char *)PEName;
					if(!net_if_flag) {
						rpt_msg += " does not have ";
					} else {
						rpt_msg += " has ";
					}
					rpt_msg += "network interface.";
					SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
				}
#endif		
				
				if(net_if_flag) {
					// settle that the PE has a network interface
					PE_ptr->set_has_netif();
			
					// Then, considers if the tx buffer capability had been settled (otherwise, KisTA library models an infinite tx buffer)
									
					if(has_tx_buffer_size(doc,curNetIfNode)) {
						curNetIfAttribute = get_tx_buffer_size_node(doc,curNetIfNode);
						
						TxBufferSizeName = xmlGetValueGen(curNetIfAttribute,(xmlChar *)"value");
						
						TxBufferSizeUnit = xmlGetValueGen(curNetIfAttribute,(xmlChar *)"unit");
						
						if(TxBufferSizeName==NULL) {
							rpt_msg = "Problem reading transmission buffer size of network interface of PE ";
							rpt_msg += PE_ptr->name();
							SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
						}
						
						tx_buffer_size = atoi((const char *)TxBufferSizeName);
						
						if(TxBufferSizeUnit==NULL) {
							if(global_kista_xml_verbosity) {
								rpt_msg = "Unit of transmission buffer size of network interface of PE ";
								rpt_msg += PE_ptr->name();
								rpt_msg += " not provided. The default unit taken by default is -bit-. Suported units (-bit-,-byte-)";
								SC_REPORT_WARNING("KisTA-XML",rpt_msg.c_str());
							}
						} else {

							if(global_kista_xml_verbosity) {							
								rpt_msg = "Setting Tx buffer capabylity of PE ";
								rpt_msg += PE_ptr->name();
								rpt_msg += " network interface to ";
							}
							
							if(!strcmp((const char *)TxBufferSizeUnit,"bit")) {

								if(global_kista_xml_verbosity) {
									rpt_msg += tx_buffer_size;
									rpt_msg += " bits.";
								
									SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
								}
								
								PE_ptr->get_netif()->set_tx_buffer_size(tx_buffer_size);
							} else if(!strcmp((const char *)TxBufferSizeUnit,"byte")) {

								if(global_kista_xml_verbosity) {
									rpt_msg += tx_buffer_size;
									rpt_msg += " bytes.";
									SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
								}
								
								PE_ptr->get_netif()->set_tx_buffer_size(tx_buffer_size*8);
							} else {
								rpt_msg = "Unrecognized unit provided for the transmission buffer size of the PE ";
								rpt_msg += PE_ptr->name();
								rpt_msg += " network interface.";
								SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
							}
						}
					}
				}

			} else {
				if(global_kista_xml_verbosity) {
					printf("kista-XML INFO: No network interface flag for PE %s settled. KisTA assumes no network interface by default.\n",PEName);
				}
			}

			// Adding the recently created and configured PE to the created HW platform
			PE_cluster.push_back(PE_ptr);
					
		}
		
	} else 
	{
		if(global_kista_xml_verbosity) {
			rpt_msg = "No Processing elements where specified. KisTA will assume a PE per scheduler";
			SC_REPORT_WARNING("KisTA-XML",rpt_msg.c_str());
		}
	}
	
	
	if(ThereIsCommResources(doc)) {
		setCommRes = getCommResources(doc);
		
		for(j=0; j< getNumberOfCommResources(doc) ; j++) {
			// get communication resource name
			CommResName = (const char *)xmlGetValueGen(setCommRes->nodeTab[j],(xmlChar *)"name");
									   
			// get communication resource type
			CommResType = (const char *)xmlGetValueGen(setCommRes->nodeTab[j],(xmlChar *)"type");
			
			if(CommResType==NULL) {
				rpt_msg = "Problem reading the Type of the Communication Resource ";
				if(CommResName==NULL) {
					rpt_msg += "(without name)";
				} else { 
					rpt_msg += CommResName;
				}
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
			}
			
			// generate a unique name if a name for the comm resource is not provided
			if(CommResName==NULL) {
				CommResName = sc_gen_unique_name(CommResType);

				if(global_kista_xml_verbosity) {
					rpt_msg = "No name assigned to Communication Resource. KisTA automatically assigns the name ";
					rpt_msg +=  CommResName;
					SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
				}

			}
					
			if(!strcmp(CommResType,"generic")) {
				// creates a generic physical communication resource
				phy_comm_res_p = new phy_comm_res_t(CommResName);
				// the communication resource is added to the list of generic communication resources
				// (mantained by KisTA library, so no need for global elements)

				// (1) look for "binary" HW connections hanging from comm resource
				hw_connections_from_node(CommResName, "generic", phy_comm_res_p, setCommRes->nodeTab[j],doc);
							
			} if(!strcmp(CommResType,"tdma_bus")) {
				tdma_bus_p = new tdma_bus(CommResName);
				// the communication resource is added to the list of tdma bus communication resources
				// (mantained by KisTA library, so no need for global elements)

				read_and_set_tdma_bus_attributes(tdma_bus_p,setCommRes->nodeTab[j],doc);

				// (1) look for "binary" HW connections hanging from comm resource
				hw_connections_from_node(CommResName, "tdma_bus" , tdma_bus_p, setCommRes->nodeTab[j],doc);

			} else {
				if(global_kista_xml_verbosity) {				
					rpt_msg = "Communication Resource Type (";
					rpt_msg +=  CommResType;
					rpt_msg += ") not supported by KisTA XML interface.";
					SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
				}
			}
		        
			if(global_kista_xml_verbosity) {
				rpt_msg = "Creating Communication Resource ";
				rpt_msg +=  CommResName;
				rpt_msg +=  " of type ";
				rpt_msg +=  CommResType;
				SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
			}
			
		}

	} else {
		if(global_kista_xml_verbosity) {
			rpt_msg = "No Communication Resources where specified. KisTA will not consider communication penalties";
			SC_REPORT_WARNING("KisTA-XML",rpt_msg.c_str());
		}
	}
	
	// ATTENTION; connections currently only from comm resources
	// TBC: connections from PE side
	//      "dual connections", src and dest	
	
	 
}

#endif
