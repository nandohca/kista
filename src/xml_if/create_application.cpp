/*****************************************************************************

  create_application.cpp
  
   Functions for generating the application layer in the kista-xml front-end.
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 July

 *****************************************************************************/

#ifndef _CREATE_APPLICATION_CPP
#define _CREATE_APPLICATION_CPP

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

#include <set>
#include <vector>

#include <systemc.h>

#include <kista.hpp>

// for loading task functions
#include <dlfcn.h>

typedef std::set<std::string> supported_communications_t;
typedef std::set<std::string> supported_communications_data_types_t;

//std::string sup_comm[] = {"fifo_buffer","semaphore","registered_event","memoryless_event"};
std::string sup_comm[] = {"fifo_buffer"};
std::string sup_comm_dt[] = {"unsigned","int","ushort","short","float","double","char","voidp","message"};

supported_communications_t supported_communications(sup_comm,sup_comm + sizeof(sup_comm)/sizeof(sup_comm[0]) );
supported_communications_data_types_t supported_communication_data_types(sup_comm_dt,sup_comm_dt + sizeof(sup_comm_dt)/sizeof(sup_comm_dt[0]) );

// detects if the current element has a frequency element
bool hasWCET(xmlNodePtr cur) {
	return hasChild(cur,"WCET");
}

// read WCET element

bool readWCET(xmlNodePtr cur,  task_info_t *Task_info_ptr) {
	
	std::string rpt_msg;
	xmlChar *unitStr;
	xmlChar *valueStr;
	double return_value;
	
	valueStr = xmlGetValueGen(cur,(xmlChar *)"value");
		
	if(valueStr==NULL) return false;
	
	return_value = atof((const char *)valueStr);
	
	unitStr = xmlGetValueGen(cur,(xmlChar *)"unit");

	return_value = conv_value_by_unit(return_value,unitStr,NULL); // convert to second

	Task_info_ptr->set_WCET(sc_time(return_value,SC_SEC));

#ifdef _VERBOSE_KISTA_XML
	if(global_kista_xml_verbosity) {
		rpt_msg = "Read WCET: ";
		rpt_msg +=  sc_time(return_value,SC_SEC).to_string();
		SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
	}
#endif

	return return_value;
}


// detects if the current element has a WCEI element
bool hasWCEI(xmlNodePtr cur) {
	return hasChild(cur,"WCEI");
}

// read WCEI element

bool readWCEI(xmlNodePtr cur,  task_info_t *Task_info_ptr) {
	
	std::string rpt_msg;
	xmlChar *unitStr;
	xmlChar *valueStr;
	unsigned int return_value;
	
	valueStr = xmlGetValueGen(cur,(xmlChar *)"value");
		
	if(valueStr==NULL) return false;
	
	return_value = atoi((const char *)valueStr);
	
	unitStr = xmlGetValueGen(cur,(xmlChar *)"unit");

	if(unitStr==NULL) {
#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {
			rpt_msg = "No WCEI unit setting for task ";
			rpt_msg += Task_info_ptr->name();
			rpt_msg +=  ". Instructions will be assumed by default. ";
			SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
		}
#endif		
	} else {
		if(strcmp((const char *)unitStr,"instr")) { // if the "instr" unit is not specified
			rpt_msg = "Unsupported unit ";
			rpt_msg += (const char *)unitStr;
			rpt_msg += ".";			
			SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());			
		}
	}

	Task_info_ptr->set_WCEI(return_value);

#ifdef _VERBOSE_KISTA_XML
	if(global_kista_xml_verbosity) {
		rpt_msg = "WCEI = ";
		rpt_msg += (const char *)valueStr;
		rpt_msg +=  " instructions.";
		SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
	}
#endif

	return return_value;
}




xmlNodeSetPtr getTaskPropertiesFromApplication(xmlDocPtr doc) {
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr  XPathObjectPtr;
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)"/system/application/task_properties");
	if(XPathObjectPtr==NULL) {
		return NULL;
	} else {
		nodeset = XPathObjectPtr->nodesetval;
		return nodeset;
	}
}
					
// This function gets Task properties
//  - First it looks for them hanging on the current node
//  - In case they are not found there, then it looks for them hanging from the application tab
void getTaskProperties(xmlDocPtr doc,xmlNodePtr currNode, task_info_t *Task_info_ptr) {
	std::string msg;
	// variables for (1)
	xmlNodePtr curChildNodePtr;
	curChildNodePtr = currNode->xmlChildrenNode;

	// variables for (2)	
	xmlNodeSetPtr		TaskPropertiesFromApplication;
	int j;
	xmlChar *TaskName;
	
	bool properties_found = false;
	
	// (1) first looks for task properties hanging from the current task node
	while (curChildNodePtr != NULL) {
		if ((!xmlStrcmp(curChildNodePtr->name, (const xmlChar *)"task_properties"))){
			if(hasWCET(curChildNodePtr)) {
				readWCET(getFirstChild(curChildNodePtr,"WCET"),Task_info_ptr);
				properties_found = true;
			}
			if(hasWCEI(curChildNodePtr)) {
				readWCEI(getFirstChild(curChildNodePtr,"WCEI"),Task_info_ptr);
				properties_found = true;
			}
		}
		curChildNodePtr=curChildNodePtr->next;
	}
	
	// (2) if not found, it looks for task properties directly hanging in the application section
	
	TaskPropertiesFromApplication = getTaskPropertiesFromApplication(doc);
	
	if(TaskPropertiesFromApplication!=NULL) {					
		for(j=0; j<TaskPropertiesFromApplication->nodeNr ; j++) {
			TaskName = xmlGetValueGen(TaskPropertiesFromApplication->nodeTab[j],(xmlChar *)"task_name");
			
			if(TaskName==NULL) {
				msg = "Task Property hanging from application section does not refer to any task.";
				msg += "\tPlease, ensure that the Task Property section has a task_name tag.\n";
				SC_REPORT_ERROR("KisTA-XML",msg.c_str());
			}
			
			if((!xmlStrcmp(TaskName,(xmlChar *)Task_info_ptr->name()))) { // found task properties for the task 
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {
					msg = "Read properties of task ";
					msg += (const char *)TaskName;
					SC_REPORT_WARNING("KisTA-XML",msg.c_str());
				}
#endif
				if(hasWCET(TaskPropertiesFromApplication->nodeTab[j])) {
					readWCET(getFirstChild(TaskPropertiesFromApplication->nodeTab[j],"WCET"),Task_info_ptr);
					properties_found = true;
				}	
			}
		}
	}
	
	if(!properties_found) {
		if(global_kista_xml_verbosity) {
			msg = "No properties found for task ";
			msg += (const char *)TaskName;
			SC_REPORT_WARNING("KisTA-XML",msg.c_str());
		}
	}
	
}


//
// DEFAULT FUNCTIONALITIES
//

void default_raw_functionality() {
#ifdef _VERBOSE_KISTA_XML
	if(global_kista_xml_verbosity) {
//		scheduler *sched_info_p;
		task_info_t *task_info_p;
		sc_process_handle current_task;
		current_task=sc_get_current_process_handle();		
        task_info_p  = task_info_by_phandle[current_task];
//		sched_info_p = task_info_by_phandle[current_task]->get_scheduler();
		printf("kista-XML INFO: Task %s : Default raw functionality at %s.\n",task_info_p->name(), sc_time_stamp().to_string().c_str());
	}
#endif		
}

void default_collaborative_functionality() {
#ifdef _VERBOSE_KISTA_XML
//		scheduler *sched_info_p;
	task_info_t *task_info_p;
	sc_process_handle current_task;
	current_task=sc_get_current_process_handle();		
    task_info_p  = task_info_by_phandle[current_task];
//		sched_info_p = task_info_by_phandle[current_task]->get_scheduler();
	
	while(true) {
		if(global_kista_xml_verbosity) {
			printf("kista-XML INFO: Task %s : Default collaborative functionality at %s.\n",task_info_p->name(), sc_time_stamp().to_string().c_str());
		}
		consume_WCET();
		yield();
	}

#endif		
}
    
// get functionality associated to the node
//		get tasks function pointer from a dynamic library 
//
//      Notice that for tasks such functionality contains both
//      - I/O invokation
//      - functionality (only the latter one need to be instrumented 
//        in case code annotation is used. Because of that, the cleanest
//        way is that such functionality to be instrumented in a separated 
//        file, where "pure C/C++ functionality" is contained)
VOIDFPTR get_node_function(xmlDocPtr doc,xmlNodePtr currNode,
							const char *node_type,
							xmlChar *node_name,
                            const char *function_tag_name // name of the tag of the associated functionality
                           ) {
	std::string rpt_msg;
	const char *dlsym_error;
	
	void* handle; 			// library pointer
	VOIDFPTR fun_p;	//pointer to the task function
	std::string	file_with_path_name;
	
	xmlNodePtr  fun_node;
	xmlChar		*fun_name;
	xmlChar		*fun_file;
	xmlChar		*fun_path;
	
	fun_node=getFirstChild(currNode,function_tag_name);
	
	if(fun_node==NULL) {
			   
		if(global_kista_xml_verbosity) {			
			rpt_msg = "No ";
			rpt_msg += function_tag_name;
			rpt_msg += " was found for ";
			rpt_msg += node_type;
			rpt_msg += " '";
			rpt_msg += (const char *)node_name;
			rpt_msg += "'.";			
			SC_REPORT_WARNING("KisTA-XML",rpt_msg.c_str());
		}

		// function migh not be mandatory, so if it is not provided, then NULL is returned
		return NULL;
	}
	
	fun_name = xmlGetValueGen(fun_node,(xmlChar *)"name");

	if((fun_name==NULL) || (!strcmp((const char *)fun_name,"")) ) {
		rpt_msg = "No name was given to ";
		rpt_msg += function_tag_name;
		rpt_msg += " of ";
		rpt_msg += node_type;
		rpt_msg += " '";
		rpt_msg += (const char *)node_name;
		rpt_msg += "'.";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}
	
/*
 * 	if(fun_name==NULL) {
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}
	*/
			
	fun_file = xmlGetValueGen(fun_node,(xmlChar *)"file");

	if(fun_file==NULL) {
		rpt_msg = "The object file which contains '";
		rpt_msg += (const char *)fun_name;
		rpt_msg += "' ";
		rpt_msg += function_tag_name;
		rpt_msg += " of ";
		rpt_msg += node_type;
		rpt_msg += " '";
		rpt_msg += (const char *)node_name;
		rpt_msg += "' was not provided.";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}
	
	fun_path = xmlGetValueGen(fun_node,(xmlChar *)"path");
	
	if(fun_path==NULL) {
#ifdef _VERBOSE_KISTA_XML			   
		if(global_kista_xml_verbosity) {		
			rpt_msg = "No path for object file which contains '";
			rpt_msg += (const char *)fun_name;
			rpt_msg += "' ";
			rpt_msg += function_tag_name;
			rpt_msg += "' of ";
			rpt_msg += node_type;
			rpt_msg += " '";
			rpt_msg += (const char *)node_name;
			rpt_msg += "' was provided. The file will be searched in the current directory.";
			SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
		}
#endif
		file_with_path_name = (const char *)fun_file;
	} else {
		file_with_path_name = (const char *)fun_path;
		file_with_path_name += (const char *)fun_file;
	}

	handle = dlopen(file_with_path_name.c_str(), RTLD_LAZY);

    if (!handle) {
		rpt_msg = "Failure opening the dynamic library '";
		rpt_msg += file_with_path_name;
		rpt_msg += "' which contains '";
		rpt_msg += (const char *)fun_name;
		rpt_msg += "' ";
		rpt_msg += function_tag_name;
		rpt_msg += " of ";
		rpt_msg += node_type;
		rpt_msg += " '";
		rpt_msg += (const char *)node_name;
		rpt_msg += "'.";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
    }
    
    // load the symbol, in this case the task function name

    // reset errors
    dlerror();
    fun_p = (VOIDFPTR) dlsym(handle, (const char *)fun_name);
    dlsym_error = dlerror();
    if (dlsym_error) {
		rpt_msg = "Symbol '";
		rpt_msg += (const char *)fun_name;
		rpt_msg += "' for ";
		rpt_msg += function_tag_name;
		rpt_msg += " of ";
		rpt_msg += node_type;
		rpt_msg += " '";
		rpt_msg += (const char *)node_name;
		rpt_msg += "' could not be loaded from library ";
		rpt_msg += file_with_path_name.c_str();
		rpt_msg += ".";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
    }
    return fun_p;
}

// get all Tasks from the application
xmlNodeSetPtr getTasks(xmlDocPtr doc) {
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr  XPathObjectPtr;
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)"/system/application/task");
	if(XPathObjectPtr==NULL) {
		SC_REPORT_ERROR("KisTA-XML","Application creation failed. No Task was read in the XML input.");
	}
	nodeset = XPathObjectPtr->nodesetval;
	return nodeset;
}

unsigned int getNumberOfTasks(xmlDocPtr doc) {
	xmlNodeSetPtr setTasknodes;
	setTasknodes=getTasks(doc);
	return setTasknodes->nodeNr;
}

// get all Coomm&&Synchronization nodes from the application
xmlNodeSetPtr getCommSynchNodes(xmlDocPtr doc) {
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr  XPathObjectPtr;
	XPathObjectPtr = getNodeSet(doc,(xmlChar *)"/system/application/comm_synch");
	if(XPathObjectPtr==NULL) {
		if(global_kista_xml_verbosity) {		
			SC_REPORT_WARNING("KisTA-XML","No Comm_Synch elements read from XML input.");
		}
		return NULL;
	} else {
		nodeset = XPathObjectPtr->nodesetval;
		return nodeset;
	}
}

unsigned int getNumberOfCommSynchNodes(xmlDocPtr doc) {
	xmlNodeSetPtr setCommSynchnodes;
	setCommSynchnodes=getCommSynchNodes(doc);
	return setCommSynchnodes->nodeNr;
}

bool supported_communication(const char *CommunicationType) {
	std::string CType;
	CType = CommunicationType;
	if(supported_communications.find(CType)==supported_communications.end()) return false;
	else return true;
}

bool supported_communication_data_type(const char *DataType) {
	std::string DType;
	DType = DataType;
	if(supported_communication_data_types.find(DType)==supported_communication_data_types.end()) return false;
	else return true;	
}

// return the number of IO channels created
unsigned int create_application(xmlDocPtr doc, std::vector<task_info_t*> &task_vec,
												fifo_buffer_int_set_t  &fb_int_map,
												fifo_buffer_uint_set_t  &fb_uint_map,
												fifo_buffer_short_set_t  &fb_short_map,
												fifo_buffer_ushort_set_t  &fb_ushort_map,
												fifo_buffer_float_set_t  &fb_float_map,
												fifo_buffer_double_set_t  &fb_double_map,
												fifo_buffer_char_set_t  &fb_char_map,
												fifo_buffer_voidp_set_t  &fb_voidp_map,
												fifo_buffer_message_set_t  &fb_msg_map,
												bool create_env_flag_par
	) {
	unsigned int j;
	xmlNodeSetPtr setTasknodes;
	xmlChar *TaskName;
	
	xmlNodeSetPtr setCommSynchNodes;
	xmlChar *CommSynchName;
	const char *srcName,*destName;
	const char *CommType,*CommDataType, *CommTokenSize, *IOComm;
	unsigned int token_size;
	bool io_comm_flag;
	comm_synch_role_t ch_role;
	unsigned int n_io_channels;
	bool src_task_in_system;
	bool dest_task_in_system;
	bool src_task_in_env;
	bool dest_task_in_env;
	
	std::string CommSynchNameString;
		
	std::string rpt_msg;
	
	task_info_t *Task_info_ptr;
	//task_info_t *Task_info_ptr_dest;
	
	VOIDFPTR task_fun_p;
	VOIDFPTR comm_synch_init_fun_p;
	
	tasks_info_by_name_t::iterator task_by_name_it;
	tasks_info_by_name_t::iterator env_task_by_name_it;
	
	// for the moment, only these type of channels
	fifo_buffer<unsigned int>	*fifo_buf_uint_p;
    fifo_buffer<int>			    *fifo_buf_int_p;
	fifo_buffer<unsigned short>	*fifo_buf_ushort_p;
    fifo_buffer<short>		    *fifo_buf_short_p;    
    fifo_buffer<float>			*fifo_buf_float_p;
    fifo_buffer<double>			*fifo_buf_double_p;
    fifo_buffer<char>			    *fifo_buf_char_p;
	fifo_buffer<void *>			*fifo_buf_voidp_p;
	fifo_buffer<message_t>		*fifo_buf_msg_p;

//	xmlNodePtr curr_node;
		
	if(getNumberOfTasks(doc)<1) {
		SC_REPORT_ERROR("KisTA-XML","Application creation failed. No Task was detected.");
	}
	
	// read Task nodes
	// ---------------------------------------------
	
	setTasknodes = getTasks(doc);
	// setTasknodes should not be NULL here because we have already checked that there were nodes in the tree
	
	for(j=0; j< getNumberOfTasks(doc) ; j++) {
	   
	   TaskName = xmlGetValueGen(setTasknodes->nodeTab[j],(xmlChar *)"name");

	   if(TaskName==NULL) {
			rpt_msg = "While parsing the System Description file and creating Task instance.";
			rpt_msg += "No name found for a task. This is currently an error in KisTA, which obligues each task to have an identifier.";
			SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
		} else {
			if(global_kista_xml_verbosity) {
				printf("kista-XML INFO: Creating Task instance %s.\n",TaskName);
			}
			
			// get task function
			task_fun_p = get_node_function(doc,setTasknodes->nodeTab[j],
			                                "Task",
			                                TaskName,
			                                "function");
                           		
			if(task_fun_p!=NULL) {
				Task_info_ptr = new task_info_t((const char *)TaskName,task_fun_p); // The default funtionality is used by default
			} else {
				// if not read or provided, then a (void) default_collaborative functionality is used
//	   	   Task_info_ptr = new task_info_t((const char *)TaskName,NULL); // for the moment without the function pointer (or a default function)
#ifdef _VERBOSE_KISTA_XML
				if(global_kista_xml_verbosity) {
					rpt_msg = "No task function was provided for task ";
					rpt_msg += Task_info_ptr->name();
					rpt_msg +=  ". Default collaborative functionality is assigned.";
					SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
				}
#endif
				Task_info_ptr = new task_info_t((const char *)TaskName,default_collaborative_functionality); // The default funtionality is used by default
			}
	   	   // TO DO;
	   	   //    - Support of link to user functionalities
	   	   //    - Default funtionalities for aperiodic collaborative tasks, and for periodic tasks, if possible automating
	   	   //      the selection by detecting the schedulers and the assignations of tasks to them
	   	   //    - Read of more properties
	   	   //
	   	   getTaskProperties(doc,setTasknodes->nodeTab[j],Task_info_ptr);
		}

		// Adding the recently created and configured Task to the created Application Task set
		task_vec.push_back(Task_info_ptr);
	}

/*			
	cout << "SYSTEM TASKS" << endl;
	cout << task_info_by_name.size() << endl;
	for(task_by_name_it=task_info_by_name.begin();task_by_name_it!=task_info_by_name.end(); task_by_name_it++) {
		cout << task_by_name_it->first;
	}
			
	cout << "ENVIRONMENT TASKS" << endl;
	cout << env_tasks_by_name.size() << endl;
	for(env_task_by_name_it=env_tasks_by_name.begin();env_task_by_name_it!=env_tasks_by_name.end(); env_task_by_name_it++) {
		cout << env_task_by_name_it->first;
	}		
*/
	
	// Now read PIM/Application (and I/O) communication nodes 
	// -------------------------------------------------------
	n_io_channels = 0;
	
	setCommSynchNodes = getCommSynchNodes(doc);

	if(setCommSynchNodes!=NULL) { // For a flexible modelling, supporting aperiodic tasksets, there is no obligation to
	                               // capture communication&synchronization channels
		
		if(global_kista_xml_verbosity) {	
			rpt_msg = "Number of System-Level (PIM/Application) communication and synchronization elements: ";
			rpt_msg += to_string(getNumberOfCommSynchNodes(doc));
			SC_REPORT_INFO("kisTA-XML", rpt_msg.c_str());
		}
	
		for(j=0; j< getNumberOfCommSynchNodes(doc) ; j++) {
		   CommSynchName = xmlGetValueGen(setCommSynchNodes->nodeTab[j],(xmlChar *)"name");

		   // read name of Tasks if exists. In case there is one in the XML file it creates the new PE elements passigng such
		   // a name, otherwise, a no-name constructor which generates unique names is created
		   if(CommSynchName==NULL) {
	//			Task_info_ptr = new task_info_t();
				if(global_kista_xml_verbosity) {
					rpt_msg = "While parsing the System Description file and creating a PIM/Application communication instance.";
					rpt_msg += "No name found for the system-level connection.";
					SC_REPORT_WARNING("KisTA-XML",rpt_msg.c_str());
				}
				
			}
			
#ifdef _VERBOSE_KISTA_XML	
			if(global_kista_xml_verbosity) {
					rpt_msg = "Creating Communication instance:";
			}
#endif
			
			srcName = NULL;
			destName = NULL;
			
			// check that source and dest fields are valid and load them into internal variables
			srcName = (const char *)xmlGetValueGen(setCommSynchNodes->nodeTab[j],(xmlChar *)"src");
			destName = (const char *)xmlGetValueGen(setCommSynchNodes->nodeTab[j],(xmlChar *)"dest");
			
#ifdef _VERBOSE_KISTA_XML
			if(global_kista_xml_verbosity) {
				rpt_msg += " src= ";
				rpt_msg += srcName;
				rpt_msg += ", dest= ";
				rpt_msg += destName;
			}
#endif

			IOComm = (const char *)xmlGetValueGen(setCommSynchNodes->nodeTab[j],(xmlChar *)"io");

			if(IOComm!=NULL) {
				if( !strcmp((const char *)IOComm,"true") || !strcmp((const char *)IOComm,"yes")) {
					io_comm_flag = true;
					n_io_channels++;
				} else if( !strcmp((const char *)IOComm,"false") || !strcmp((const char *)IOComm,"no")) {
					io_comm_flag = false; // this actually has not effect given the false default value 
				} else {
					// if the value has been specified but it is neither "true" or "false"
					rpt_msg = "io specification of the communication&synchronization ";
					rpt_msg += (const char *)CommSynchName;
					rpt_msg += " has to be either \"true\" (or \"yes\") or \"false\" (or \"no\") (default unless specified).";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
			} else {
				// If there is no "io" attribute, by default, the edge is assume to be an internal channel
				io_comm_flag = false;
			}
			
			if(io_comm_flag) {
				ch_role = IO_COMMSYNCH;
			} else {
				ch_role = SYSTEM_COMMSYNCH;
			}
			
			// get task function
			comm_synch_init_fun_p =
			     get_node_function(doc,setCommSynchNodes->nodeTab[j],
			                                "Comm&&Synch",
			                                CommSynchName,
			                                "init_function");

			// TODO: Support IO channels wich do not reach the environment. 
			//       (In such a case, an implicit sink or source environment task has to be created, beacause the 
			//        modeller is not interesting in modelling the environment, but whants to:
			//           - associate an IO device to the ouput/input edge
			//           - associate a monitor, e.g. throughput to the ouput/input edge
			//
			// For the moment, this is an error in KisTA-XML, which obligues SRC and DEST in the IO channels too!!
			//
			//if(create_env_flag_par) {
			//} else {
				// Environment is not created, then it is possible that either the source, or the task
				// to not to appear, requiring implicit creation of src/sink task
			//}


			//
			// Checks the source and dest fields and that they are coherent
			// (Likely, this checks can be better moved to KisTA library)
			//
			// check that source field is valid and load it into the corresponding internal variable
			src_task_in_system = false;
			dest_task_in_system = false;
			src_task_in_env = false;
			dest_task_in_env = false;

			task_by_name_it = task_info_by_name.find(srcName);
			
			if(task_by_name_it == task_info_by_name.end()) {
				src_task_in_system = false;
			} else {
				src_task_in_system = true;
			}
			
			task_by_name_it = task_info_by_name.find(destName);
			
			if(task_by_name_it == task_info_by_name.end()) {
				dest_task_in_system = false;
			} else {
				dest_task_in_system = true;
			}

			// if an environment has been created, then, the src and dest provided are also looked 
			if(create_env_flag_par) {
				env_task_by_name_it = env_tasks_by_name.find(srcName);
			
				if(env_task_by_name_it == env_tasks_by_name.end()) {
					src_task_in_env = false;
				} else {
					src_task_in_env = true;
				}
			
				env_task_by_name_it = env_tasks_by_name.find(destName);
			
				if(env_task_by_name_it == env_tasks_by_name.end()) {
					dest_task_in_env = false;
				} else {
					dest_task_in_env = true;
				}
			}			
			
			// Check that the destination and source tasks are in the description
			if( (!src_task_in_system) && (!src_task_in_env) ) {
				rpt_msg = "Source Task ";
				rpt_msg += srcName;
				rpt_msg += " of communication&synchronization ";
				rpt_msg += (const char *)CommSynchName;
				rpt_msg += " was declared neither as a System Task, nor as an Environment Task.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());			
			}

			if( (!dest_task_in_system) && (!dest_task_in_env) ) {    
				rpt_msg = "Destination Task '";
				rpt_msg += destName;
				rpt_msg += "' of communication&synchronization '";
				rpt_msg += (const char *)CommSynchName;
				rpt_msg += "' was declared neither as a System Task, nor as an Environment Task.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());			
			}
			
			// Reaching this point in code means that src and dest tasks are at least either in the environment task list or in the system task list
			
			if(io_comm_flag) {
				// An communication channel for system I/O
				if(src_task_in_system && dest_task_in_system) {
					rpt_msg = "Both, Source Task '";
					rpt_msg += srcName;
					rpt_msg += "', and Destination Task ";
					rpt_msg += destName;
					rpt_msg += " of I/O communication&synchronization '";
					rpt_msg += (const char *)CommSynchName;
					rpt_msg += "' are System Tasks.\n";
					rpt_msg += " Either declare the communication&synchronization channel as a System channel, or either the source or the destination tasks as an Environment Tasks.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());								
				} else if(src_task_in_env && dest_task_in_env) {
					rpt_msg = "Both, Source Task ";
					rpt_msg += srcName;
					rpt_msg += ", and Destination Task ";
					rpt_msg += destName;
					rpt_msg += " of I/O communication&synchronization '";
					rpt_msg += (const char *)CommSynchName;
					rpt_msg += "' are Environment System Tasks.\n";
					rpt_msg += " Either declare the communication&synchronization channel as an Environment channel, or either the source or the destination tasks as a System Tasks.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}	
			} else { // It is a system channel
				// An communication channel for communication within the environment
				if(src_task_in_env && dest_task_in_env) {
					rpt_msg = "Both, Source Task '";
					rpt_msg += srcName;
					rpt_msg += "', and Destination Task '";
					rpt_msg += destName;
					rpt_msg += "' of System (internal) communication&synchronization '";
					rpt_msg += (const char *)CommSynchName;
					rpt_msg += "' are Environment Tasks.\n";
					rpt_msg += " Either declare the communication&synchronization channel as an Environment channel, or both source and the destination task as System Tasks.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());								
				} else if( src_task_in_env ) {
					rpt_msg = "Source Task '";
					rpt_msg += srcName;
					rpt_msg += " of System (internal) communication&synchronization '";
					rpt_msg += (const char *)CommSynchName;
					rpt_msg += "' is an Environment Tasks.\n";
					rpt_msg += " Either declare the communication&synchronization channel as an I/O channel, or the source task as a System Task.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				} else if( dest_task_in_env) {
					rpt_msg = "Destination Task '";
					rpt_msg += srcName;
					rpt_msg += " of System (internal) communication&synchronization '";
					rpt_msg += (const char *)CommSynchName;
					rpt_msg += "' is an Environment Task.\n";
					rpt_msg += " Either declare the communication&synchronization channel as an I/O channel, or the destination task as a System Task.";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());					
				} // the remaining case is that both tasks are in the environment, so it is coherent				
			}
			
			// check the type of communicaiton and data type to be transferred
			CommType = (const char *)xmlGetValueGen(setCommSynchNodes->nodeTab[j],(xmlChar *)"comm_type");
			
			if(!supported_communication(CommType)) {
				rpt_msg = "Communication of type ";
				rpt_msg += CommType;
				rpt_msg += " stated for the the communication&synchronization ";
				rpt_msg += (const char *)CommSynchName;
				rpt_msg += " is not currently supported by the KisTA XML interface.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());			
			}
			
			CommDataType = (const char *)xmlGetValueGen(setCommSynchNodes->nodeTab[j],(xmlChar *)"comm_data_type");

			if(!supported_communication_data_type(CommDataType)) {
				rpt_msg = "Data type ";
				rpt_msg += CommDataType;
				rpt_msg += " stated for the the communication&synchronization ";
				rpt_msg += (const char *)CommSynchName;
				rpt_msg += " is not currently supported by the KisTA XML interface.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());			
			}
			
			CommTokenSize = (const char *)xmlGetValueGen(setCommSynchNodes->nodeTab[j],(xmlChar *)"token_size");

			if(CommTokenSize!=NULL) {
				token_size = atoi((const char *)CommTokenSize);
			
				if(token_size<1) {
					rpt_msg = "Token size ";
					rpt_msg += CommTokenSize;
					rpt_msg += " stated for the communication&synchronization ";
					rpt_msg += (const char *)CommSynchName;
					rpt_msg += " has to be a positive integer number bigger than 0 (bits).";
					SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
				}
			}			
			
#ifdef _VERBOSE_KISTA_XML			
			if(global_kista_xml_verbosity) {	
				rpt_msg += ", comm_type= ";
				rpt_msg += CommType;
				if(CommDataType!=NULL) {
					rpt_msg += ", comm_data_type= ";
					rpt_msg += CommDataType;
				}
				if(CommTokenSize!=NULL) {
					rpt_msg += ", token_size= ";
					rpt_msg += CommTokenSize;
				}
				rpt_msg += ",";
			}	
#endif

			// Some prepared Code in case src and dest are child nodes, and not attributes!!
	/*				
			if(hasChild(setCommSynchNodes->nodeTab[j], "src")) {
				// TBC
				cout << "Retrieving source task" << endl;
			} else {
				rpt_msg = "No source node is found for communication ";
				if(CommSynchName!=NULL) rpt_msg += (const char*)CommSynchName;
				rpt_msg += ", which is mandatory.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
			}

			if(hasChild(setCommSynchNodes->nodeTab[j], "dest")) {
				// TBC
				cout << "Retrieving dest task" << endl;			
			} else {
				rpt_msg = "No dest node is found for communication ";
				if(CommSynchName!=NULL) rpt_msg += (const char*)CommSynchName;
				rpt_msg += ", which is mandatory.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());			
			}


			if(hasChild(setCommSynchNodes->nodeTab[j], "comm_type")) {
				cout << "Retrieving system-level communication type" << endl;			
			} else {
				rpt_msg = "No communication type stated for system-level communication ";
				if(CommSynchName!=NULL) rpt_msg += (const char*)CommSynchName;
				rpt_msg += ". Fifo buffer communication assumed by default";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());			
			}
		*/	


	//	   	   Task_info_ptr = new task_info_t((const char *)TaskName,NULL); // for the moment without the function pointer (or a default function)
	//	   	   Task_info_ptr = new task_info_t((const char *)TaskName,default_collaborative_functionality); // The default funtionality is used by default

		   // In case a name has not been assigned from the XML side, a generic unique name is assigned
		   // relying on SystemC facilities
		   
		   if(CommSynchName==NULL) {
			   CommSynchName = (xmlChar *)sc_gen_unique_name(CommType);
#ifdef _VERBOSE_KISTA_XML			   
			   if(global_kista_xml_verbosity) {	
					rpt_msg += " (auto-gen. name)";
				}
#endif				
		   }

#ifdef _VERBOSE_KISTA_XML
		   if(global_kista_xml_verbosity) {	
			   	rpt_msg += " name= ";
			   	rpt_msg += (const char *)CommSynchName;
			   	rpt_msg += ", io= ";
			   	if(io_comm_flag) {
					rpt_msg += "true";
				} else {
					rpt_msg += "false";
				}
								
				if(comm_synch_init_fun_p!=NULL) {
					rpt_msg += ", with init function.";
				} else {
					rpt_msg += ".,";
				}
				SC_REPORT_INFO("KisTA-XML",rpt_msg.c_str());
		   }
#endif

		   CommSynchNameString = (const char *)CommSynchName;
		   
		   if(!strcmp(CommType,"fifo_buffer")){
			 if( !strcmp(CommDataType, "unsigned") ) {
				fifo_buf_uint_p = new fifo_buffer<unsigned int>((const char *)CommSynchName,16,srcName,destName);
				if(CommTokenSize!=NULL) {
					fifo_buf_uint_p->set_message_size(token_size);
				}
				fifo_buf_uint_p->set_role(ch_role);
				if( (comm_synch_init_fun_p!=NULL) && (ch_role==SYSTEM_COMMSYNCH)) {
					fifo_buf_uint_p->set_init(comm_synch_init_fun_p);
				}
				fb_uint_map[CommSynchNameString]=fifo_buf_uint_p;
			 } else if( !strcmp(CommDataType,"int") ) {
				fifo_buf_int_p = new fifo_buffer<int>((const char *)CommSynchName,16,srcName,destName);
				if(CommTokenSize!=NULL) {
					fifo_buf_int_p->set_message_size(token_size);
				}
				fifo_buf_int_p->set_role(ch_role);
				if( (comm_synch_init_fun_p!=NULL) && (ch_role==SYSTEM_COMMSYNCH)) {
					fifo_buf_int_p->set_init(comm_synch_init_fun_p);
				}				
				fb_int_map[CommSynchNameString]=fifo_buf_int_p;
			 } else if( !strcmp(CommDataType, "ushort") ) {
				fifo_buf_ushort_p = new fifo_buffer<unsigned short>((const char *)CommSynchName,16,srcName,destName);
				if(CommTokenSize!=NULL) {
					fifo_buf_ushort_p->set_message_size(token_size);
				}
				fifo_buf_ushort_p->set_role(ch_role);
				if( (comm_synch_init_fun_p!=NULL) && (ch_role==SYSTEM_COMMSYNCH)) {
					fifo_buf_ushort_p->set_init(comm_synch_init_fun_p);
				}				
				fb_ushort_map[CommSynchNameString]=fifo_buf_ushort_p;
			 } else if( !strcmp(CommDataType,"short") ) {
				fifo_buf_short_p = new fifo_buffer<short>((const char *)CommSynchName,16,srcName,destName);
				if(CommTokenSize!=NULL) {
					fifo_buf_short_p->set_message_size(token_size);
				}
				fifo_buf_short_p->set_role(ch_role);
				if( (comm_synch_init_fun_p!=NULL) && (ch_role==SYSTEM_COMMSYNCH)) {
					fifo_buf_short_p->set_init(comm_synch_init_fun_p);
				}				
				fb_short_map[CommSynchNameString]=fifo_buf_short_p;
			 } else if( !strcmp(CommDataType,"float") ) {
				fifo_buf_float_p = new fifo_buffer<float>((const char *)CommSynchName,16,srcName,destName);
				if(CommTokenSize!=NULL) {
					fifo_buf_float_p->set_message_size(token_size);
				}
				fifo_buf_float_p->set_role(ch_role);
				if( (comm_synch_init_fun_p!=NULL) && (ch_role==SYSTEM_COMMSYNCH)) {
					fifo_buf_float_p->set_init(comm_synch_init_fun_p);
				}										
				fb_float_map[CommSynchNameString]=fifo_buf_float_p;
			 } else if( !strcmp(CommDataType,"double") ) {
				fifo_buf_double_p = new fifo_buffer<double>((const char *)CommSynchName,16,srcName,destName);
				if(CommTokenSize!=NULL) {
					fifo_buf_double_p->set_message_size(token_size);
				}
				fifo_buf_double_p->set_role(ch_role);
				if( (comm_synch_init_fun_p!=NULL) && (ch_role==SYSTEM_COMMSYNCH)) {
					fifo_buf_double_p->set_init(comm_synch_init_fun_p);
				}				
				fb_double_map[CommSynchNameString]=fifo_buf_double_p;
			 } else if( 
			 !strcmp(CommDataType,"char") ) {
				fifo_buf_char_p = new fifo_buffer<char>((const char *)CommSynchName,16,srcName,destName);
				if(CommTokenSize!=NULL) {
					fifo_buf_char_p->set_message_size(token_size);
				}
				fifo_buf_char_p->set_role(ch_role);
				if( (comm_synch_init_fun_p!=NULL) && (ch_role==SYSTEM_COMMSYNCH)) {
					fifo_buf_char_p->set_init(comm_synch_init_fun_p);
				}
				fb_char_map[CommSynchNameString]=fifo_buf_char_p;
			 } else if( !strcmp(CommDataType,"voidp") ) {
				fifo_buf_voidp_p = new fifo_buffer<void *>((const char *)CommSynchName,16,srcName,destName);
				if(CommTokenSize!=NULL) {
					fifo_buf_voidp_p->set_message_size(token_size);
				}
				fifo_buf_voidp_p->set_role(ch_role);
				if( (comm_synch_init_fun_p!=NULL) && (ch_role==SYSTEM_COMMSYNCH)) {
					fifo_buf_voidp_p->set_init(comm_synch_init_fun_p);
				}				
				fb_voidp_map[CommSynchNameString]=fifo_buf_voidp_p;
			 } else if( !strcmp(CommDataType,"message") ) {
				fifo_buf_msg_p = new fifo_buffer<message_t>((const char *)CommSynchName,16,srcName,destName);
				if(CommTokenSize!=NULL) {
					fifo_buf_msg_p->set_message_size(token_size);
				}
				fifo_buf_msg_p->set_role(ch_role);
				if( (comm_synch_init_fun_p!=NULL) && (ch_role==SYSTEM_COMMSYNCH)) {
					fifo_buf_msg_p->set_init(comm_synch_init_fun_p);
				}				
				fb_msg_map[CommSynchNameString]=fifo_buf_msg_p;
			 } else {
				rpt_msg = "Data type ";
				rpt_msg += CommDataType;
				rpt_msg += " stated for the the communication&synchronization ";
				rpt_msg += (const char *)CommSynchName;
				rpt_msg += " is not currently supported by the KisTA XML interface.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
			 }
		   } else {
				rpt_msg = "Communication of type ";
				rpt_msg += CommType;
				rpt_msg += " stated for the the communication&synchronization ";
				rpt_msg += (const char *)CommSynchName;
				rpt_msg += " is not currently supported by the KisTA XML interface.";
				SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());		   
		   }
		}
	}
	
	return n_io_channels;
}

#endif
