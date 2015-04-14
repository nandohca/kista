/*****************************************************************************

  create_environment.cpp
  
   Functions for generating the environment if any in the kista-xml front-end.
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 February

 *****************************************************************************/



#ifndef _CREATE_ENVIRONMENT_CPP
#define _CREATE_ENVIRONMENT_CPP

// get task functionality
//		get tasks function pointer from a dynamic library 
//      Notice that such functionality contains both
//      - I/O invokation
//      - functionality (only the latter one need to be instrumented 
//        in case code annotation is used. Because of that, the cleanest
//        way is that such functionality to be instrumented in a separated 
//        file, where "pure C/C++ functionality" is contained)
VOIDFPTR get_env_task_function(xmlDocPtr doc,xmlNodePtr currNode,xmlChar *env_task_name) {
	std::string rpt_msg;
	const char *dlsym_error;
	
	void* handle; 			// library pointer
	VOIDFPTR	env_task_fun_p;
	std::string	file_with_path_name;
	
	xmlNodePtr  env_task_fun_node;
	xmlChar		*env_task_fun_name;
	xmlChar		*env_task_fun_file;
	xmlChar		*env_task_fun_path;
	
	env_task_fun_node=getFirstChild(currNode,"function");
	
	if(env_task_fun_node==NULL) {
		rpt_msg = "No functionality for environment task ";
		rpt_msg += (const char *)env_task_name;
		rpt_msg += " was provided.";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
		return NULL;
	}
	
	env_task_fun_name = xmlGetValueGen(env_task_fun_node,(xmlChar *)"name");

	if(env_task_fun_name==NULL) {
		rpt_msg = "No name for environment task ";
		rpt_msg += (const char *)env_task_name;
		rpt_msg += " function was provided.";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}
	
/*
 * 	if(task_fun_name==NULL) {
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}
	*/
			
	env_task_fun_file = xmlGetValueGen(env_task_fun_node,(xmlChar *)"file");

	if(env_task_fun_file==NULL) {
		rpt_msg = "No object file containing environment task ";
		rpt_msg += (const char *)env_task_name;
		rpt_msg += " function was provided.";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}
	
	env_task_fun_path = xmlGetValueGen(env_task_fun_node,(xmlChar *)"path");
	
	if(env_task_fun_path==NULL) {
		if(global_kista_xml_verbosity) {
			rpt_msg = "No path for the object file containing environment task function ";
			rpt_msg += (const char *)env_task_name;
			rpt_msg += " function was provided. The file will be searched in the current directory.";
			SC_REPORT_WARNING("KisTA-XML",rpt_msg.c_str());
		}
		file_with_path_name = (const char *)env_task_fun_file;
	} else {
		file_with_path_name = (const char *)env_task_fun_path;
		file_with_path_name += "/";
		file_with_path_name += (const char *)env_task_fun_file;
	}

	handle = dlopen(file_with_path_name.c_str(), RTLD_LAZY);

    if (!handle) {
		rpt_msg = "The dynamic library ";
		rpt_msg += file_with_path_name;
		rpt_msg += ", containing environment task ";
		rpt_msg += (const char *)env_task_name;
		rpt_msg += " function '";
		rpt_msg += (const char *)env_task_fun_name;
		rpt_msg += "', could not be opened.";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
    }
    
    // load the symbol, in this case the task function name

    // reset errors
    dlerror();
    env_task_fun_p = (VOIDFPTR) dlsym(handle, (const char *)env_task_fun_name);
    dlsym_error = dlerror();
    if (dlsym_error) {
		rpt_msg = "Environment task function symbol ";
		rpt_msg += (const char *)env_task_fun_name;
		rpt_msg += " for environment task ";
		rpt_msg += (const char *)env_task_name;
		rpt_msg += " could not be loaded from library ";
		rpt_msg += file_with_path_name.c_str();
		rpt_msg += ".";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
    }
    return env_task_fun_p;
}



xmlNodeSetPtr getEnvTasks(xmlDocPtr doc) {
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr  XPathObjectPtr;
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)"/system/environment/task");
	if(XPathObjectPtr==NULL) {
		if(global_kista_xml_verbosity) {	
			SC_REPORT_WARNING("KisTA-XML","No Environment Task was read in the XML input.");
		}
		return NULL;
	}
	nodeset = XPathObjectPtr->nodesetval;
	return nodeset;
}

unsigned int getNumberOfEnvTasks(xmlDocPtr doc) {
	xmlNodeSetPtr setEnvTasknodes;
	setEnvTasknodes=getEnvTasks(doc);
	if(setEnvTasknodes==NULL) return 0;
	else return setEnvTasknodes->nodeNr;
}

bool create_environment(xmlDocPtr doc,
						tasks_info_by_name_t &env_tasks_par
/*
 // These data will be likely required to automate the declaration&initializations to let
 // environment tasks access the system

 						fifo_buffer_int_set_t     &fifo_buffer_int_map,
						fifo_buffer_uint_set_t    &fifo_buffer_uint_map,
						fifo_buffer_short_set_t   &fifo_buffer_short_map,
						fifo_buffer_ushort_set_t  &fifo_buffer_ushort_map, 
						fifo_buffer_float_set_t   &fifo_buffer_float_map,
						fifo_buffer_double_set_t  &fifo_buffer_double_map,
						fifo_buffer_char_set_t    &fifo_buffer_char_map,
						fifo_buffer_voidp_set_t   &fifo_buffer_voidp_map
*/
)
{
	unsigned int j;
	xmlNodeSetPtr setEnvTasknodes;
	xmlChar *EnvTaskName;
	VOIDFPTR EnvTask_fun_p;
	task_info_t *env_task_info_p;	// pointer to the task info. structure for the environment task
		
	std::string rpt_msg;

	if(getNumberOfEnvTasks(doc)<1) {
		if(global_kista_xml_verbosity) {
			SC_REPORT_WARNING("KisTA-XML","No Environment tasks were read, environment will not be created.");
		}
	} else {
		
		
		// read Task nodes
		// ---------------------------------------------
		setEnvTasknodes = getEnvTasks(doc);
		// setTasknodes should not be NULL here because we have already checked that there were nodes in the tree
		
		for(j=0; j< getNumberOfEnvTasks(doc) ; j++) {
		   
		   EnvTaskName = xmlGetValueGen(setEnvTasknodes->nodeTab[j],(xmlChar *)"name");

		   if(EnvTaskName==NULL) {
				rpt_msg = "While parsing the System Description file and creating Task instance.";
				rpt_msg += "No name found for an environment task. This is currently an error in KisTA, which obligues each environment task to have an identifier.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
			} else {
				if(global_kista_xml_verbosity) {
					printf("kista-XML INFO: Reading Environment Task name %s.\n",EnvTaskName);
				}
				
				// Get the associated environment task functionality (via task pointer)
				EnvTask_fun_p = get_env_task_function(doc,setEnvTasknodes->nodeTab[j],EnvTaskName);

			}

			// Creates the task infor structure for the current parsed task...
			env_task_info_p = new task_info_t((const char*)EnvTaskName,EnvTask_fun_p,ENV_TASK);

			// and adds it to the environment task list, adding its associated function pointer
			env_tasks_par[std::string((const char *)EnvTaskName)]=env_task_info_p;
		}
	}
	
	// TO DO:
	//
	// Support/Generation of environment channels, and also of the initialization of the environment tasks to allow
	// environment task functions directly access them
	//
	
/*
 * // Part of this code could be reused for checking the coherence	
			if(io_comm_flag&&env_comm_flag) {
					rpt_msg = "Unexpected situation: The communication&synchronization '";
					rpt_msg += CommSynchName;
					rpt_msg += "' has been declared both as an I/O and as an Environment task. Please, report the issue to KisTA authors.\n";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());												
			} else	if(io_comm_flag) {
				// An communication channel for system I/O
				if(src_task_in_system&&dest_task_in_system) {
					rpt_msg = "Both, Source Task '";
					rpt_msg += srcName;
					rpt_msg = "', and Destination Task ";
					rpt_msg += destName;
					rpt_msg += " of I/O communication&synchronization '";
					rpt_msg += CommSynchName;
					rpt_msg += "' are System Tasks.\n";
					rpt_msg += " Either declare the communication&synchronization channel as a System channel, or either the source or the destination tasks as an Environment Tasks.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());								
				} else if(src_task_in_env&&dest_task_in_env) {
						rpt_msg = "Both, Source Task ";
						rpt_msg += srcName;
						rpt_msg = ", and Destination Task ";
						rpt_msg += destName;
						rpt_msg += " of I/O communication&synchronization '";
						rpt_msg += CommSynchName;
						rpt_msg += "' are Environment System Tasks.\n";
						rpt_msg += " Either declare the communication&synchronization channel as an Environment channel, or either the source or the destination tasks as a System Tasks.";
						SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}	
			} else if(env_comm_flag) {
				// An communication channel for communication within the environment
				if(src_task_in_system&&dest_task_in_system) {
					rpt_msg = "Both, Source Task '";
					rpt_msg += srcName;
					rpt_msg = "', and Destination Task '";
					rpt_msg += destName;
					rpt_msg += "' of Environment communication&synchronization '";
					rpt_msg += CommSynchName;
					rpt_msg += "' are System Tasks.\n";
					rpt_msg += " Either declare the communication&synchronization channel as a System channel, or both source and the destination task as Environment Tasks.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());								
				} else if( (src_task_in_system ) {
					rpt_msg = "Source Task '";
					rpt_msg += srcName;
					rpt_msg += " of Environment communication&synchronization '";
					rpt_msg += CommSynchName;
					rpt_msg += "' is a System Tasks.\n";
					rpt_msg += " Either declare the communication&synchronization channel as an I/O channel, or the source task as an Environment Task.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				} else if( (dest_task_in_system) {
					rpt_msg = "Destination Task '";
					rpt_msg += srcName;
					rpt_msg += " of Environment communication&synchronization '";
					rpt_msg += CommSynchName;
					rpt_msg += "' is a System Tasks.\n";
					rpt_msg += " Either declare the communication&synchronization channel as an I/O channel, or the destination task as an Environment Task.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());					
				} // the remaining case is that both tasks are in the environment, so it is coherent
			} else { // It is a system channel
				// An communication channel for communication within the environment
				if(src_task_in_env&&dest_task_in_env) {
					rpt_msg = "Both, Source Task '";
					rpt_msg += srcName;
					rpt_msg = "', and Destination Task '";
					rpt_msg += destName;
					rpt_msg += "' of System (internal) communication&synchronization '";
					rpt_msg += CommSynchName;
					rpt_msg += "' are Environment Tasks.\n";
					rpt_msg += " Either declare the communication&synchronization channel as an Environment channel, or both source and the destination task as System Tasks.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());								
				} else if( (src_task_in_env ) {
					rpt_msg = "Source Task '";
					rpt_msg += srcName;
					rpt_msg += " of System (internal) communication&synchronization '";
					rpt_msg += CommSynchName;
					rpt_msg += "' is an Environment Tasks.\n";
					rpt_msg += " Either declare the communication&synchronization channel as an I/O channel, or the source task as a System Task.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				} else if( (dest_task_in_env) {
					rpt_msg = "Destination Task '";
					rpt_msg += srcName;
					rpt_msg += " of System (internal) communication&synchronization '";
					rpt_msg += CommSynchName;
					rpt_msg += "' is an Environment Task.\n";
					rpt_msg += " Either declare the communication&synchronization channel as an I/O channel, or the destination task as a System Task.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());					
				} // the remaining case is that both tasks are in the environment, so it is coherent				
			}
*/	
	
	// if(getNumberOfEnvTasks(doc)>0) {
	// equals to
	if(env_tasks_par.size()>0) {
		return true;
	} else {
		return false;
	}
}

#endif
