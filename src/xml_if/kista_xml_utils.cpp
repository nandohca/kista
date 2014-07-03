/*****************************************************************************

  kista_xml_utils.cpp
  
   Query and util functions for kista-xml.
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 July

 *****************************************************************************/

#ifndef _KISTA_XML_UTILS_CPP
#define _KISTA_XML_UTILS_CPP

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

// See Makefile .kista-xml for definition of this variable
#ifdef REQUIRES_XML_COMPLEMENTS

// reproduce some recent libxml code from /libxml2-2.9.1
/**
 * xmlXPathSetContextNode:
 * @node: the node to to use as the context node
 * @ctx:  the XPath context
 *
 * Sets 'node' as the context node. The node must be in the same
 * document as that associated with the context.
 *
 * Returns -1 in case of error or 0 if successful
 */
int
xmlXPathSetContextNode(xmlNodePtr node, xmlXPathContextPtr ctx) {
    if ((node == NULL) || (ctx == NULL))
        return(-1);

    if (node->doc == ctx->doc) {
        ctx->node = node;
	return(0);
    }
    return(-1);
}

/**
 * xmlXPathNodeEval:
 * @node: the node to to use as the context node
 * @str:  the XPath expression
 * @ctx:  the XPath context
 *
 * Evaluate the XPath Location Path in the given context. The node 'node'
 * is set as the context node. The context node is not restored.
 *
 * Returns the xmlXPathObjectPtr resulting from the evaluation or NULL.
 *         the caller has to free the object.
 */
xmlXPathObjectPtr
xmlXPathNodeEval(xmlNodePtr node, const xmlChar *str, xmlXPathContextPtr ctx) {
    if (str == NULL)
        return(NULL);
    if (xmlXPathSetContextNode(node, ctx) < 0)
        return(NULL);
    return(xmlXPathEval(str, ctx));
}

#endif

// simplified function for relative navigations with XPath
// inputs: the document, the reference node (has to be in the document)
// and the Xpath expresion

xmlXPathObjectPtr
getNodeSet_from_context(xmlDocPtr doc, xmlXPathContextPtr context, xmlChar *xpath){
	
	xmlXPathObjectPtr result;

	if (context == NULL) {
		SC_REPORT_ERROR("KisTA-XML","Context passed to getNodeset_from_context.");
	}
	result = xmlXPathEvalExpression(xpath, context);
	if (result == NULL) {
		SC_REPORT_ERROR("KisTA-XML","xmlXPathEvalExpression return NULL value");
		return NULL;
	}
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		xmlXPathFreeObject(result);
		if(global_kista_xml_verbosity) {
			printf("Warning, NULL node set obtained from getNodeset_from_context\n");
		}
	}
	return result;
}


// simplified function for absolute navigations with XPath
xmlXPathObjectPtr
getNodeSet (xmlDocPtr doc, xmlChar *xpath){
	
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	context = xmlXPathNewContext(doc);
	if (context == NULL) {
		if(global_kista_xml_verbosity) {
			printf("Error in xmlXPathNewContext\n");
		}
		return NULL;
	}
	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL) {
		if(global_kista_xml_verbosity) {
			printf("Error in xmlXPathEvalExpression\n");
		}
		return NULL;
	}
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		xmlXPathFreeObject(result);
#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {		
			printf("Warning, NULL node set obtained from getNodeset\n");
		}
#endif        
		return NULL;
	}
	return result;
}

// simplified function for absolute navigations with XPath
// aware of a given namespace

/*
 * TBC
xmlXPathObjectPtr
getNodeSet (xmlDocPtr doc, xmlChar *xpath, xmlNsPtr xmlNsPtr_par){
	
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	context = xmlXPathNewContext(doc);
	if (context == NULL) {
		if(global_kista_xml_verbosity) {
			printf("Error in xmlXPathNewContext\n");
		}
		return NULL;
	}
	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL) {
		if(global_kista_xml_verbosity) {
			printf("Error in xmlXPathEvalExpression\n");
		}
		return NULL;
	}
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		xmlXPathFreeObject(result);
#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {		
			printf("Warning, NULL node set obtained from getNodeset\n");
		}
#endif        
		return NULL;
	}
	return result;
}

*/

// Auxiliar functions...
// ... to check if there is a child node with a given name
bool hasChild(xmlNodePtr cur, const char *elem_name) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)elem_name))){
			return true;
		}
		curChildNodePtr = curChildNodePtr->next;
	}	
	return false;
}

// ... to get the a child node with a given name
xmlNodePtr getFirstChild(xmlNodePtr cur, const char *elem_name) {
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = cur->xmlChildrenNode;
	while (curChildNodePtr != NULL) {
		if( !xmlStrcmp(curChildNodePtr->name, (const xmlChar *)elem_name) ) {
			return curChildNodePtr;
		}
		curChildNodePtr = curChildNodePtr->next;
	}
	return NULL;	
}

// Function which acts as xmlGetProp, but also detects if the property value
// is a parameter to get it from the global parameter map
 	
xmlChar *xmlGetValueGen(xmlNodePtr cur, xmlChar *property_val) {
	xmlChar *valueStr;
	xmlChar *paramStr;
	param_value_t *ParamValueSet;
	global_parameters_t::iterator it;
	
	std::string msg_rpt;
	
	valueStr = xmlGetProp(cur, property_val);
	
	if(valueStr==NULL) {
#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {
			msg_rpt = "No value could be retrieved for property ";
			msg_rpt += (const char *)property_val;
			msg_rpt += " of node ";
			msg_rpt += (const char *)cur->name;
			msg_rpt += ". Null value is returned.";
			SC_REPORT_WARNING("KisTA-XML",msg_rpt.c_str());
		}
#endif		
		return NULL;
	}
	
	if( !xmlStrncmp(valueStr, (const xmlChar *)"_",1) ) {
		// it is not a value, but a referebce to a parameter,
		// the actual value has to be extracted from the global list of parameters
		
//		cout << (const char *)valueStr << " parameter recognized." << endl;
	
		paramStr = &valueStr[1];
		
//		cout << (const char *)paramStr << " : parameter searched." << endl;
		
		it = global_parameters.find((const char *)paramStr);
		
		if(it==global_parameters.end()) {
			// the key, that is the parameter name was not found in the global parameters_map structure
			msg_rpt = "Parameter ";
			msg_rpt += (const char *)paramStr;
			msg_rpt += " was not found in the parameter list.";
			SC_REPORT_ERROR("KisTA-XML",msg_rpt.c_str());
		}
		
		ParamValueSet = global_parameters[(const char *)paramStr];
		
		if(ParamValueSet->size()==1){
			return (xmlChar *)(*ParamValueSet)[0];
		} else if(ParamValueSet->size()==0) {
			msg_rpt = "Parameter ";
			msg_rpt += (const char *)paramStr;
			msg_rpt += " not resolved in params file.";
			SC_REPORT_ERROR("KisTA-XML","");
		} else {
			msg_rpt = "Parameter ";
			msg_rpt += (const char *)paramStr;
			msg_rpt += " of scalar type expected.";
			SC_REPORT_ERROR("KisTA-XML","");			
		}
	} else {
		// it is a string interpreted as the value
		return valueStr;
	}
}

//
// Function which acts as xmlGetValueGen, but for vector values of the type
//    <item value="">
//    <item value="">
//    ....
//    <item value="">
//
// 
// As xmlGetValueGen, it also detects if there is a property value
// as a parameter.
//
// In case that no item list is found in the global parameter map,
// in turn loaded from the params (configuration) file
// item list is searched if it is hanging from the cur node.
//
// Notice that the read from the global parameter map overrides
// which overrides the read of the item list hanging from the cur node
//
//
// Unlike scalar parameters, vector parameters support default vector value, hanging from the node
//
// So the precedence for the values is:
//    1 . vector values from confuiiguration file
//    2 . vector values from the params section in the system definition file
//    3 . default vector values (hanging from node)
//
//  The param_value_t vector can indeed be a 0-sized vector
//
param_value_t *xmlGetVectorValueGen(xmlNodePtr cur, // node with a child parameter value (<item value > list)
									xmlChar *property_val
									) {
	xmlNodePtr curChildNodePtr;
	xmlChar *valueStr;
	xmlChar *paramStr;
	std::string comm_synch_name_std_str;

	param_value_t *ParamValueSetPtr;
	global_parameters_t::iterator it;
		
	std::string msg_rpt;
	
	valueStr = xmlGetProp(cur, property_val);
	
	if(valueStr!=NULL) {
		// We first check if the property has been defined as a vector
		// parameter, and moreover if it is in the global params list
		// In that case, the vector is read from the global params list ( so from the configuration file)
		if( !xmlStrncmp(valueStr, (const xmlChar *)"_",1) ) {
			
			paramStr = &valueStr[1];
		
			//	cout << (const char *)paramStr << " : parameter searched." << endl;
			
			it = global_parameters.find((const char *)paramStr);
			
			if(it!=global_parameters.end()) {
				// Vector parameter foun din the global parameters list
				// So, either it was defined in the system configuration file or in the 
				// parameters section of the system definition file
				ParamValueSetPtr = global_parameters[(const char *)paramStr];
				return ParamValueSetPtr;
			}

		} else {
			msg_rpt = "Property '";
			msg_rpt += (const char *)property_val;
			msg_rpt += "' of node '";
			msg_rpt += (const char *)cur->name;
			msg_rpt += "' is expected to be a vector value.";
			msg_rpt += "For it, either specify such property through a vector param, starting by '_', and defining the vector param in the configuration file,";
			msg_rpt += " or remove property '";
			msg_rpt += (const char *)property_val;
			msg_rpt += "' from node '";
			msg_rpt += (const char *)cur->name;
			msg_rpt += "' and add the <item value=""> list hanging from such node.";
			SC_REPORT_ERROR("KisTA-XML",msg_rpt.c_str());			
		}
		
	}
	
#ifdef _VERBOSE_KISTA_XML			
	if(global_kista_xml_verbosity) {		
		msg_rpt = "No vector value could be retrieved for parameter";
		if(valueStr!=NULL) {
			msg_rpt += "'";
			msg_rpt += (const char *)valueStr;
			msg_rpt += "'";
		}
		msg_rpt += " (associated to property '";
		msg_rpt += (const char *)property_val;
		msg_rpt += "' of node '";
		msg_rpt += (const char *)cur->name;
		msg_rpt += "') either, because no vector parameter has been associated to the static scheduling node through a \"value\" property,";
		msg_rpt += " or because the vector has not been defined (as a vector parameter, with an <item value=\"...\"> list)";
		msg_rpt += " either in the system configuration file or in the parameters section in the system definition file.";
		msg_rpt += "Vector value will be looked hanging from the node,.";
		SC_REPORT_INFO("KisTA-XML",msg_rpt.c_str());
	}
#endif

	// Reached this point, we need to look for the 3-th alternative, that is
	// that the item value list is hanging from the current node

	ParamValueSetPtr = new param_value_t; // In this case, we need to create the param_value_set because it was not created by read_sysconf_paramenters method

	curChildNodePtr=cur->xmlChildrenNode;
	
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"item"))){
			valueStr = xmlGetValueGen(curChildNodePtr,(xmlChar *)"value");
			if(valueStr==NULL) {
				msg_rpt = "No value found in an item of the vector value associated to a '";
				msg_rpt += (const char *)cur->name;
				msg_rpt += "' node.";
				SC_REPORT_ERROR("KisTA-XML",msg_rpt.c_str());
			} else {
				ParamValueSetPtr->push_back((const char *)valueStr);
			}
		}
		curChildNodePtr = curChildNodePtr->next;
	}
		
	return ParamValueSetPtr;

}

// Convert value from one unit to another unit
double conv_value_by_unit(double value, xmlChar *src_unit, xmlChar *tgt_unit) {
	double return_value;
	
	if(src_unit==NULL) {
		// if there is no unit specified (NULL value read), then it assumes the unit
		return_value = 1.0;
	} else {
		if( !xmlStrncmp(src_unit, (const xmlChar *)"T",1) ) { // tera
			return_value = 1000000000000.0;	
		} else if ( !xmlStrncmp(src_unit, (const xmlChar *)"G",1) ) { // giga
			return_value = 1000000000.0;		
		} else if ( !xmlStrncmp(src_unit, (const xmlChar *)"M",1) ) { // mega	
			return_value = 1000000.0;
		} else if ( ( !xmlStrncmp(src_unit, (const xmlChar *)"K",1) ) ||
		            ( !xmlStrncmp(src_unit, (const xmlChar *)"k",1) ) ) { // kilo
			return_value = 1000.0;
		} else if ( !xmlStrncmp(src_unit, (const xmlChar *)"m",1) ) { // mili
			return_value = 0.001;
		} else if ( !xmlStrncmp(src_unit, (const xmlChar *)"u",1) ) { // micro
			return_value = 0.000001;			
		} else if ( !xmlStrncmp(src_unit, (const xmlChar *)"n",1) ) { // nano
			return_value = 0.000000001;			
		} else if ( !xmlStrncmp(src_unit, (const xmlChar *)"p",1) ) { // pico
			return_value = 0.000000000001;		
		} else if ( !xmlStrncmp(src_unit, (const xmlChar *)"f",1) ) { // femto
			return_value = 0.000000000000001;
		} else { // by default, non-recognized unit taken as the standard reference unit
			return_value = 1.0;
#ifdef _VERBOSE_KISTA_XML	
			if(global_kista_xml_verbosity) {	
				printf("kista-XML Warning: Frequency specification with a non-recognized unit.\n");
			}
#endif
		}
	}

	return_value = value*return_value;
	
	if(tgt_unit==NULL) {
		// if there is no unit specified (NULL value read), then it assumes the unit
		return return_value;
	} else {	
		if( !xmlStrncmp(tgt_unit, (const xmlChar *)"T",1) ) { // tera
			return (0.000000000001*return_value);
		} else if ( !xmlStrncmp(tgt_unit, (const xmlChar *)"G",1) ) { // giga
			return (0.000000001*return_value);		
		} else if ( !xmlStrncmp(tgt_unit, (const xmlChar *)"M",1) ) { // mega
			return (0.000001*return_value);
		} else if ( !xmlStrncmp(tgt_unit, (const xmlChar *)"K",1) ) { // kilo
			return (0.001*return_value);
		} else if ( !xmlStrncmp(tgt_unit, (const xmlChar *)"m",1) ) { // mili
			return (1000.0*return_value);
		} else if ( !xmlStrncmp(tgt_unit, (const xmlChar *)"u",1) ) { // micro
			return (1000000.0*return_value);
		} else if ( !xmlStrncmp(tgt_unit, (const xmlChar *)"n",1) ) { // nano
			return (1000000000.0*return_value);	
		} else if ( !xmlStrncmp(tgt_unit, (const xmlChar *)"p",1) ) { // pico
			return (1000000000000.0*return_value);
		} else if ( !xmlStrncmp(tgt_unit, (const xmlChar *)"f",1) ) { // femto
			return (1000000000000000.0*return_value);
		} else { // by default, non-recognized unit taken as the standard reference unit
			return return_value;
#ifdef _VERBOSE_KISTA_XML	
			if(global_kista_xml_verbosity) {	
				printf("kista-XML Warning: Frequency specification with a non-recognized unit.\n");
			}
#endif
		}
	}
}

// Read a time when it is specified as two properties hanging from a node	
sc_time readTime(xmlNodePtr cur) {
	
	sc_time return_time;
	xmlChar *unitStr;
	xmlChar *valueStr;
	double time_value;
	
	valueStr = xmlGetValueGen(cur,(xmlChar *)"value");
		
	if(valueStr==NULL) {
		SC_REPORT_ERROR("Kista-XML","Error reading time value");
	}
	
	time_value = atof((const char *)valueStr);
	
	// Assumes that there could not be unit, so, in such a case seconds are assume
	unitStr = xmlGetValueGen(cur,(xmlChar *)"unit");

	if(unitStr==NULL) {
		if(global_kista_xml_verbosity) {
			SC_REPORT_WARNING("Kista-XML","Reading time without unit (seconds taken by default)");
		}
	} else { // if unit is provided, a conversion to seconds value is done
		time_value = conv_value_by_unit(time_value,unitStr,NULL); // convert to second
	}

	return sc_time(time_value,SC_SEC);

}


// Read a time when it is specified as two properties hanging from a node	
template<class T>
T readNumber(xmlNodePtr cur) {

	xmlChar *valueStr;	
	T number;

	valueStr = xmlGetValueGen(cur,(xmlChar *)"value");
		
	if(valueStr==NULL) {
		SC_REPORT_ERROR("Kista-XML","Error reading value");
	}
	
	number = atoi((const char *)valueStr);
	
	return number;

}

// reads the configured boolean option, described by boolean_option_description,
// in the doc XML document, in pathExpression, and returns its value.
// If the value configuration is not found, the default_value_if_no_opt is returned

bool get_configured_boolean_option(xmlDocPtr doc,
									const char *pathExpression,
									const char *boolean_option_description,
									bool default_value_if_no_opt) {	
	xmlNodeSetPtr found_nodes;
	xmlXPathObjectPtr  XPathObjectPtr;
	xmlChar *valueStr;
	std::string msg_rpt;
	
	// get the set of nodes abiding the path expression
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)pathExpression);
	if(XPathObjectPtr==NULL) {
		msg_rpt = "No configuration of ";
		msg_rpt += boolean_option_description;
		if(default_value_if_no_opt) {
			msg_rpt += ". The option is enabled by default.";
		} else {
			msg_rpt += ". The option is disabled by default.";
		}
		if(global_kista_xml_verbosity) {
			SC_REPORT_WARNING("Kista-XML", msg_rpt.c_str());
		}
		return default_value_if_no_opt;
	} else {
		found_nodes = XPathObjectPtr->nodesetval;

		if(global_kista_xml_verbosity) {		
			if(found_nodes->nodeNr>1) {
				msg_rpt = "More than one configuration of";
				msg_rpt += boolean_option_description;
				msg_rpt += ". The first one is taken.";
				if(global_kista_xml_verbosity) {
					SC_REPORT_WARNING("Kista-XML", msg_rpt.c_str());
				}
			}
		}

		if(found_nodes->nodeNr<1) {
			msg_rpt = "Unexpected situation on the configuration of ";
			msg_rpt += boolean_option_description;
			SC_REPORT_ERROR("Kista-XML", msg_rpt.c_str());
		}

		valueStr = xmlGetValueGen(found_nodes->nodeTab[0],(xmlChar *)"value");
		
		if(valueStr==NULL) {
			msg_rpt = "Error reading the configuration of";
			msg_rpt += boolean_option_description;
			SC_REPORT_ERROR("Kista-XML", msg_rpt.c_str());
		}
		
		if(!strcmp((const char *)valueStr,"true")) {
			return true;
		} else {
			return false;
		}
		
	}
}

// reads the configured boolean option, described by boolean_option_description,
// in the doc XML document, in pathExpression, and returns its value.
// If the value configuration is not found, the default_value_if_no_opt is returned

const char *get_configured_string_option(xmlDocPtr doc,
									const char *pathExpression,
									const char *string_option_description,
									const char *default_value) {	
			
	xmlNodeSetPtr found_nodes;
	xmlXPathObjectPtr  XPathObjectPtr;
	xmlChar *valueStr;
	std::string msg_rpt;
	
	// get the set of nodes abiding the path expression
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)pathExpression);
	if(XPathObjectPtr==NULL) {
		msg_rpt = "No configuration of ";
		msg_rpt += string_option_description;
		msg_rpt += "done. The default value is ";
		msg_rpt += default_value;
		msg_rpt += ".";
				
		if(global_kista_xml_verbosity) {
			SC_REPORT_WARNING("Kista-XML", msg_rpt.c_str());
		}
		return default_value;
	} else {
		found_nodes = XPathObjectPtr->nodesetval;

		if(global_kista_xml_verbosity) {		
			if(found_nodes->nodeNr>1) {
				msg_rpt = "More than one configuration of ";
				msg_rpt += string_option_description;
				msg_rpt += ". The first one is taken.";
				if(global_kista_xml_verbosity) {
					SC_REPORT_WARNING("Kista-XML", msg_rpt.c_str());
				}
			}
		}

		if(found_nodes->nodeNr<1) {
			msg_rpt = "Unexpected situation on the configuration of ";
			msg_rpt += string_option_description;
			SC_REPORT_ERROR("Kista-XML", msg_rpt.c_str());
		}

		valueStr = xmlGetValueGen(found_nodes->nodeTab[0],(xmlChar *)"value");
		
		if(valueStr==NULL) {
			msg_rpt = "Error reading the configuration of";
			msg_rpt += string_option_description;
			SC_REPORT_ERROR("Kista-XML", msg_rpt.c_str());
		}
		
		return (const char *)valueStr;
		
	}
}
#endif


