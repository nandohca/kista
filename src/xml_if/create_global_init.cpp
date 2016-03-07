/*****************************************************************************

  create_global_init.cpp
  
   Functions for supporting to hook a global init function
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 April

 *****************************************************************************/



#ifndef _CREATE_GLOBAL_INIT_CPP
#define _CREATE_GLOBAL_INIT_CPP

//
//		get global init function pointer from a dynamic library 
//
VOIDFPTR get_function(xmlDocPtr doc,xmlNodePtr currNode, std::string function_purpose) {
	std::string rpt_msg;
	const char *dlsym_error;
	
	void* handle; 			// library pointer
	VOIDFPTR	fun_p;
	std::string	file_with_path_name;
	
	xmlChar		*fun_name;
	xmlChar		*fun_file;
	xmlChar		*fun_path;
	
	fun_name = xmlGetValueGen(currNode,(xmlChar *)"name");

	if(fun_name==NULL) {
		rpt_msg = "Functionality for";
		rpt_msg += function_purpose;
		rpt_msg += " not provided. Please, use the \"function\" attribute for it.";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}
			
	fun_file = xmlGetValueGen(currNode,(xmlChar *)"file");

	if(fun_file==NULL) {
		rpt_msg = "No object file containing function ";
		rpt_msg += (const char *)fun_name;
		rpt_msg += " for ";
		rpt_msg += function_purpose;
		rpt_msg += " was provided.";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}
	
	fun_path = xmlGetValueGen(currNode,(xmlChar *)"path");
	
	if(fun_path==NULL) {
		if(global_kista_xml_verbosity) {
			rpt_msg = "No path for the object file containing function ";
			rpt_msg += (const char *)fun_name;
			rpt_msg += " for ";
			rpt_msg += function_purpose;
			rpt_msg += " was provided. The file will be searched in the current directory.";
			SC_REPORT_WARNING("KisTA-XML",rpt_msg.c_str());
		}
		file_with_path_name = ".";
	} else {
		file_with_path_name = (const char *)fun_path;
	}
	file_with_path_name += "/";
	file_with_path_name += (const char *)fun_file;

	handle = dlopen(file_with_path_name.c_str(), RTLD_LAZY);

    if (!handle) {
		rpt_msg = "The dynamic library ";
		rpt_msg += file_with_path_name;
		rpt_msg += ", containing function '";
		rpt_msg += (const char *)fun_name;
		rpt_msg += "' for ";
		rpt_msg += function_purpose;
		rpt_msg = " could not be opened. Try by adding a path attribute or setting LD_LIBRARY_PATH";
		
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
    }
    
    // load the symbol, in this case the task function name

    // reset errors
    dlerror();
    fun_p = (VOIDFPTR) dlsym(handle, (const char *)fun_name);
    dlsym_error = dlerror();
    if (dlsym_error) {
		rpt_msg = "Function symbol ";	
		rpt_msg += (const char *)fun_name;
		rpt_msg += " for ";
		rpt_msg += function_purpose;		
		rpt_msg += " could not be loaded from library ";
		rpt_msg += file_with_path_name.c_str();
		rpt_msg += ".";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
    }
    return fun_p;
}

// create and call global init function
void create_global_system_init(xmlDocPtr doc)
{
	
	xmlXPathObjectPtr  XPathObjectPtr;
	xmlNodeSetPtr GlobalInitNodeSet;
	VOIDFPTR GlobalInit_fun_p;
		
	std::string rpt_msg;	
	
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)"/system/application/global_init");
	if(XPathObjectPtr==NULL) {
		if(global_kista_xml_verbosity) {	
			SC_REPORT_WARNING("KisTA-XML","No global init specified for the system in the XML input.");
		}
	} else {
		GlobalInitNodeSet = XPathObjectPtr->nodesetval;
		if (GlobalInitNodeSet->nodeNr > 1) {
			rpt_msg = to_string(GlobalInitNodeSet->nodeNr);
			rpt_msg += " global initialization nodes have been found. We recomment there is one per application.";
			SC_REPORT_WARNING("KisTA-XML", rpt_msg.c_str());
		}
		// call the global init functionalities
		for(int i=0; i<GlobalInitNodeSet->nodeNr; i++) {
			// Get the global init functionality (via task pointer)
			GlobalInit_fun_p = get_function(doc,GlobalInitNodeSet->nodeTab[i],"application global init");
			
			// call the current global init functionality
			GlobalInit_fun_p();
		}
	}

}

// create and call global init function
void create_global_env_init(xmlDocPtr doc, sc_env *_env)
{
	
	xmlXPathObjectPtr  XPathObjectPtr;
	xmlNodeSetPtr GlobalInitNodeSet;
	VOIDFPTR GlobalInit_fun_p;
		
	std::string rpt_msg;	
	
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)"/system/environment/global_init");
	if(XPathObjectPtr==NULL) {
		if(global_kista_xml_verbosity) {	
			SC_REPORT_WARNING("KisTA-XML","No global init specified for the environment in the XML input.");
		}
	} else {
		GlobalInitNodeSet = XPathObjectPtr->nodesetval;
		if (GlobalInitNodeSet->nodeNr > 1) {
			SC_REPORT_ERROR("KisTA-XML","More than one global init node has been specified for the environment. You should specify only one.");
		} else {
			// Get the global init functionality (via task pointer)
			GlobalInit_fun_p = get_function(doc,GlobalInitNodeSet->nodeTab[0],"environment global init");
			
			// set the global init functionality
			_env->set_global_env_init(GlobalInit_fun_p);
		}
	}

}

void create_global_env_end(xmlDocPtr doc, sc_env *_env)
{
	
	xmlXPathObjectPtr  XPathObjectPtr;
	xmlNodeSetPtr GlobalEndNodeSet;
	VOIDFPTR GlobalEnd_fun_p;
		
	std::string rpt_msg;	
	
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)"/system/environment/global_end");
	if(XPathObjectPtr==NULL) {
		if(global_kista_xml_verbosity) {	
			SC_REPORT_WARNING("KisTA-XML","No global env specified for the environment in the XML input.");
		}
	} else {
		GlobalEndNodeSet = XPathObjectPtr->nodesetval;
		if (GlobalEndNodeSet->nodeNr > 1) {
			SC_REPORT_ERROR("KisTA-XML","More than one global env node has been specified for the environment. You should specify only one.");
		} else {
			// Get the global init functionality (via task pointer)
			GlobalEnd_fun_p = get_function(doc,GlobalEndNodeSet->nodeTab[0]," environment end call back ");
			
			// set the global init functionality
			_env->set_global_env_end(GlobalEnd_fun_p);
		}
	}

}

void hook_for_param_declaration(xmlDocPtr doc)
{
	
	xmlXPathObjectPtr  XPathObjectPtr;
	xmlNodeSetPtr ParDecNodeSet;
	VOIDFPTR ParDec_fun_p;
		
	std::string rpt_msg;	
	
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)"/system/parameters/src_param_declaration");
	if(XPathObjectPtr==NULL) {
		if(global_kista_xml_verbosity) {	
			SC_REPORT_WARNING("KisTA-XML","No hook function for global parameters declaration from sources specified.");
		}
	} else {
		ParDecNodeSet = XPathObjectPtr->nodesetval;
		if (ParDecNodeSet->nodeNr > 1) {
			SC_REPORT_ERROR("KisTA-XML","More than one hook function for global parameters declaration from sources specified. You should specify only one.");
		} else {
			// Get the global init functionality (via task pointer)
			ParDec_fun_p = get_function(doc,ParDecNodeSet->nodeTab[0]," hook function for global parameters declaration from sources ");
			
			// set the global init functionality
			ParDec_fun_p();
		}
	}

}

#endif
