/*****************************************************************************

  kista_xml.cpp
  
   For generating the kista-xml program, which generates an executable
   kista description, capable to read the xml description file(s) to 
   generate the platform and parametric inputs and outputs.
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 July

 *****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <libxml/xmlschemas.h>

#include <vector>
#include <map>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <vector>

using namespace std;

#include <systemc.h>

#include <kista.hpp>

// If undefined info and warnings of KisTA front-end disabled at compilation time
#define _VERBOSE_KISTA_XML

// To control verbosity when KisTA and KisTA front-end (when the libraries are compiled to enable it)
bool global_kista_verbosity;
bool global_kista_xml_verbosity;

// For the moment, the plugin states at least N_hyperperiods
unsigned N_hyperperiods_for_sim_time=1000000000; // for 1ns of hyperperiod, this means 10s

// For the moment, also a global map structure for parameters


// Now, provided by KisTA library
// typedef void (*VOIDFUNP)();


// Environment
SC_MODULE(sc_env) {
//class sc_env: public sc_module {
public:
	SC_CTOR(sc_env) {
#ifdef _REPORT_ENV_MESSAGES		
		if(global_kista_xml_verbosity) {
			cout << "KisTA-XML: Creating environment at construction phase" << endl;
		}
#endif
	}
	
	// call back functions for initialization and wrap up
	void set_global_env_init(VOIDFPTR _fp);
	void set_global_env_end(VOIDFPTR _fp);
	
private:	
	void before_end_of_elaboration();
	void end_of_simulation();
	VOIDFPTR global_env_init;
	VOIDFPTR global_env_end;
};

void sc_env::before_end_of_elaboration() {
	//sc_process_handle created_process_handle;
	tasks_info_by_name_t::iterator env_tasks_it;
	
	// uncomment for supporting environment task options
//	sc_spawn_options spawn_options;
			
	// do not initialize environment task
	// spawn_options.dont_initialize(); 
							
	// state sensitivity of environment task
	// spawn_options.set_sensitivity();
	
#ifdef _REPORT_ENV_MESSAGES		
	if(global_kista_xml_verbosity) {
		cout << "KisTA-XML: Continue environment creation at 'before end of elaboration' phase" << endl;
		cout << "KisTA-XML: Number of environment tasks: " << env_tasks_by_name.size() << endl;	
	}
#endif

	for(env_tasks_it = env_tasks_by_name.begin(); env_tasks_it != env_tasks_by_name.end(); env_tasks_it++) {
#ifdef _REPORT_ENV_MESSAGES		
		if(global_kista_xml_verbosity) {
			cout << "KisTA-XML: Creating Environment task " << env_tasks_it->first << "." << endl;
			//cout << "KisTA-XML: Creating Environment task " << env_tasks_it->first << " tied to function " << env_tasks_it->second->name() << endl;
			// notice that the previous line gives the name of the task twice, and not of the function
			// Currently, task_info_t does not stores the name of the function associated to the task
		}
#endif
		//created_process_handle =
				sc_core::sc_spawn(
					    env_tasks_it->second->get_functionality(),
						env_tasks_it->first.c_str()
						   // remind that, unlike system tasks, environment tasks start initialized,
						   // since environment tasks are naturally concurrent with the system
						//(const sc_core::sc_spawn_options*)NULL
//						&spawn_options // all environment tasks created with the same options 
							);
	}
	
	if (global_env_init!=NULL) {
		global_env_init();
	}
}

void sc_env::set_global_env_init(VOIDFPTR _fp) {
	this->global_env_init = _fp;
}

void sc_env::set_global_env_end(VOIDFPTR _fp) {
	this->global_env_end = _fp;
}

void sc_env::end_of_simulation() {
	if (global_env_end!=NULL) {
		global_env_end();
	}
}
	
// include of XML front-end files

#include "xml_defaults.hpp"
#include "parse_kista_xml_command.cpp"
#include "kista_xml_utils.cpp"
#include "read_sysconf_parameters.cpp"
#include "create_output.cpp"
#include "create_application.cpp"
#include "create_sw_platform.cpp"
#include "create_hw_platform.cpp"
#include "create_platform_mapping.cpp"
#include "create_app_to_platform_mapping.cpp"
#include "create_environment.cpp"
#include "kista_xml_configuration.cpp"
#include "kista_xml_analysis.cpp"
#include "create_global_init.cpp"
#include "xml_create_tikz_activity_trace.cpp"

// Creates the KisTA SystemC-based executable
int sc_main(int argc, char **argv) {

	char xmlFileName[PATH_NAME_SIZE];
	xmlDocPtr doc;
	std::string rpt_msg;	
	sc_time sim_time = SC_ZERO_TIME;

	// variables regarding the configuration of the sketch report
	bool enable_sketch_export = false;
	const char *sketch_report_file = NULL;

	// variables for the interface with M3Explorer
	char sc_path_name[PATH_NAME_SIZE] = "";
	char sm_path_name[PATH_NAME_SIZE] = "output.xml";
	char xsd_path_name[PATH_NAME_SIZE] = "";
	char smd_path_name[PATH_NAME_SIZE] = "";
	
    parse_command_line(argc,argv, xmlFileName, sc_path_name, sm_path_name, xsd_path_name, smd_path_name);

//	char xmlFileName[PATH_NAME_SIZE] = "./examples/ex3/sample3.xml";
//	char sc_path_name[PATH_NAME_SIZE] = "./examples/ex3/sample3_params.xml";
//	char sm_path_name[PATH_NAME_SIZE] = "output.xml";
//	char xsd_path_name[PATH_NAME_SIZE] = "";
//	char smd_path_name[PATH_NAME_SIZE] = "./examples/ex3/sample3_design_space_def.xml";
//
//cout << "*************************************" << endl;
//cout << sc_path_name << endl;
//cout << sm_path_name << endl;
//cout << smd_path_name << endl;
//cout << "*************************************" << endl;	
	
	// Create platform variables
//	taskset_by_name_t						Application;
	vector<task_info_t*>		app_task;
	vector<processing_element*>	PE;
	vector<scheduler*> 			RTOS;

    // environment variables
    sc_env *env;
    // global structure available from KisTA:
    // extern env_tasks_by_name_t env_tasks_by_name;
    //
    
	bool create_env_flag; // loaded to true when env_tasks.size()>0
	unsigned int number_of_io_channels;

	// The XML front-end will create as many KisTA task sets as RTOS instances
	// It does it by creating a map of tasksets pointers, where the key 
	// is the taskset name, which has to be unique
	// The scheduler name is taken as taskset key
	std::map<const char*, taskset_by_name_t*,const_char_cmp> taskset_map; //map of pointers to KisTA task sets
	  // note: the const_char_cmp comparison function required to enable comparing the "string" typed key, and not the char pointer
	  //    thus, the following would not work as we desired
//	std::map<const char*, taskset_by_name_t*> taskset_map;
	
	doc = xmlParseFile(xmlFileName);
	
	if (doc == NULL ) {
		rpt_msg = "While parsing the System Description file: ";
		rpt_msg += xmlFileName;
		rpt_msg += ".";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}

	// Read KisTA configuration
	//-------------------------------------------------------
	// configure verbosity of the KisTA-XML front-end
	global_kista_xml_verbosity = get_configured_kista_xml_verbosity(doc);

	global_kista_verbosity = get_configured_kista_verbosity(doc);

	if(global_kista_xml_verbosity) {
		cout << "KisTA-XML verbose." << endl;
		if(global_kista_verbosity) cout << "KisTA verbose." << endl;
		else cout << "KisTA silent." << endl;
	}

	enable_sketch_export = get_configured_export_sketch(doc);	
	sketch_report_file   = get_configured_sketch_file_name(doc);	
	
			//
			//   NOTE: global initialization hook functions are placed
			//         before read_sysconf_parameters in order to enable 
			//         the definition of parameters in the user code
			//         through the declare_global_parameter function
			//         
			//

	hook_for_param_declaration(doc);
		
	// read SYSTEM configuration parameters
	//-------------------------------------------------------
	try { // this is to catch any sc_report exception launched by KisTA library

/*
//
// In this version, a params file completely hides the parameters section of the system configuration file
// This version is maybe less confusing, but prevents the feature of having some parameters estable 
// while moving others with the parameters file. This is useful for explorations on different test benches
// 
		if(strlen(sc_path_name)<1) { // system configuration file not provided by command line
								// (remind that parse_command_line enforces it to NULL in such a case)
								
			read_sysconf_parameters(doc,"/system/parameters/parameter",global_parameters);
		} else {
			// otherwise, the parameters are read from a separated parameters file
			// read with the overloaded function read_parameters

//cout << sc_path_name << endl;										
			read_sysconf_parameters(sc_path_name,global_parameters);
		}
*/

	// In this version, a params file overrides definitions of parameters done in the parameters
	// section of the configuration file. However it does not removes the definitions done in such
	// a section which are not fixed by the parameters file.

		read_sysconf_parameters(doc,"/system/parameters/parameter",global_parameters);
		if(strlen(sc_path_name)>0) { // system configuration file provided via command line
										// (remind that parse_command_line enforces it to NULL in case it is not provided)
//cout << sc_path_name << endl;
			read_sysconf_parameters(sc_path_name,global_parameters);
		}


#ifdef _VERBOSE_KISTA_XML		
		if(global_kista_xml_verbosity) {
			show(global_parameters);
		}
#endif

		//
		// Create environemt in case any environemt task has been declared
		// -----------------------------------------------------------------
		create_env_flag = create_environment(doc, env_tasks_by_name
							  /* fifo_buffer_int, fifo_buffer_uint,
		                       fifo_buffer_short,fifo_buffer_ushort,
		                       fifo_buffer_float, fifo_buffer_double,
		                       fifo_buffer_char, fifo_buffer_voidp*/
		                        );

#ifdef _VERBOSE_KISTA_XML		
		if(global_kista_xml_verbosity) {
			SC_REPORT_INFO("KisTA-XML", "Environment built.");
		}
#endif
		
		// create application (return the number of io channels created
		number_of_io_channels = create_application(doc,app_task,
													fifo_buffer_int, fifo_buffer_uint,
													fifo_buffer_short,fifo_buffer_ushort,
													fifo_buffer_float, fifo_buffer_double,
													fifo_buffer_char, fifo_buffer_voidp,
													fifo_buffer_msg,
													create_env_flag);

#ifdef _VERBOSE_KISTA_XML		
		if(global_kista_xml_verbosity) {
			SC_REPORT_INFO("KisTA-XML", "Application built.");
		}
#endif

		create_SW_platform(doc,RTOS);

#ifdef _VERBOSE_KISTA_XML		
		if(global_kista_xml_verbosity) {
			SC_REPORT_INFO("KisTA-XML", "SW Platform built.");
		}
#endif
		
		create_HW_platform(doc,PE);

#ifdef _VERBOSE_KISTA_XML		
		if(global_kista_xml_verbosity) {
			SC_REPORT_INFO("KisTA-XML", "HW Platform built.");
		}
#endif
		// note: create_platform_mapping has to be called after SW and HW platform creation functions
		create_platform_mapping(doc,RTOS,PE);

#ifdef _VERBOSE_KISTA_XML		
		if(global_kista_xml_verbosity) {
			SC_REPORT_INFO("KisTA-XML", "Platform Mapping built.");
		}
#endif
		// note: create_app_to_platform_mapping has to be called after SW platform creation and after create application functions
		create_app_to_platform_mapping(doc,app_task,RTOS,taskset_map);

#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {
			SC_REPORT_INFO("KisTA-XML", "System Created.");
			SC_REPORT_INFO("KisTA-XML", "Start Environment Initializations.");
		}
#endif

		if(  create_env_flag && (number_of_io_channels>0) )
		{ // environment can be created
			env = new sc_env("env");
		} else if( (!create_env_flag) && (number_of_io_channels>0) ) {
			SC_REPORT_ERROR("KisTA-XML","There are I/O channels but no environment tasks.");
		} else if( create_env_flag && (number_of_io_channels==0) ) {
			SC_REPORT_ERROR("KisTA-XML","There are environment tasks, but not I/O channels.");
		} else {
			SC_REPORT_INFO("KisTA-XML","Creating model without environment (neither environment tasks, nor I/O channels)");
		}

		// hook to a global end functionality for reports of the environment
		create_global_env_init(doc,env);
		
		// hook to a global end functionality for reports of the environment
		create_global_env_end(doc,env);
		                       
		// enable measurements
		// (for those measurements in KisTA which need to be enabled before the end of simulation,
		//  it is usually done in order to save monitory requirements, both in memory, and specially in simulation time)
		// Just reads the syste metrics definition file and enable measurements for all those metrics which require
		// such enabling before the simulation)
		
		if(strlen(smd_path_name)>=1) {
			// For the moment, metrics that require activation before simulation start
			// are reported only if the smd file is porived
			// system metric definition file not provided by command line

			enable_measurements_for_MULTICUBE_selected_output(smd_path_name,
											RTOS,
											app_task,
											fifo_buffer_int,fifo_buffer_uint,
											fifo_buffer_short,fifo_buffer_ushort,
											fifo_buffer_float,fifo_buffer_double,
											fifo_buffer_char, fifo_buffer_voidp, fifo_buffer_msg);
		}

		// Configure analysis
		configure_kista_analysis(doc);
		
		// enable sketch report
		if(enable_sketch_export) {		
			if(sketch_report_file!=NULL) {
				sketch_report.set_file_name(sketch_report_file);
			}
			sketch_report.enable();
			if(get_configured_sketch_draw_sys_level_conn_names(doc)) {
				sketch_report.draw_sys_level_conn();
			}
			if(get_configured_sketch_highlight_environment(doc)) {
				sketch_report.highlight_environment();
			}
			if(get_configured_sketch_highlight_system(doc)) {
				sketch_report.highlight_system();
			} else {
				if(get_configured_sketch_highlight_application(doc)) {
					sketch_report.highlight_application();
				}
				if(get_configured_sketch_highlight_platform(doc)) {
					sketch_report.highlight_platform();
				}
			}
		}

		if ( get_configured_functional_validation(doc) ) {
			enable_functional_validation();
		}
		
		if ( get_configured_error_fatality_for_exploration(doc) ) {
			set_error_stop_exploration();
		} else {
			set_error_does_not_stop_exploration();
		}

#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {
			printf("kista-XML INFO: Anaylsis and Report configuration completed.\n");
		}
#endif
		
		// hook to a global init functionality for the system
		create_global_system_init(doc);

#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {
			SC_REPORT_INFO("KisTA-XML","Global System Intialization completed.");
		}
#endif

#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity && xml_create_tikz_activity_trace(doc)) {
			SC_REPORT_INFO("KisTA-XML","Activity TiKZ trace report configuration completed.");
		}
#endif

		// start the simulation
		// -------------------------------------------------------------
		if (get_configured_sim_time(doc,sim_time)) {
			sc_start(sim_time);
		} else {
			sc_start();
		}

		sc_stop(); // to ensure call to call backs (end_of_simulation),
				   //  this is requires for a right accounting of execution times, and starvation assesment

#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {
			rpt_msg = "End of simulation at ";
			rpt_msg += sc_time_stamp().to_string();
			rpt_msg = "\n";
			SC_REPORT_INFO("KisTA-XML", rpt_msg.c_str());
		}
#endif
		// If functionality monitor has been created and a report of the functional
		// test indicates a failure, this point is not reached, since the functionality
		// monitor reports a validation error at the end of the simulation

#ifdef _VERBOSE_KISTA_XML
		if(global_kista_xml_verbosity) {
			rpt_msg = "EDumping output to file ";
			rpt_msg += sm_path_name;
			rpt_msg = "\n";
			SC_REPORT_INFO("KisTA-XML", rpt_msg.c_str());
		}
#endif
		if(strlen(smd_path_name)<1) { // system metric definition file not provided by command line
								  //// (remind that parse_command_line enforces smd_path_name to NULL by default)

			create_MULTICUBE_full_output(sm_path_name,RTOS,app_task); // only reports metrics which do not require activation before start of simulation
		} else {		
			// takes the smd file into account in order to generate only the metrics selected for DSE
			create_MULTICUBE_selected_output(sm_path_name,smd_path_name,
			                                 RTOS,
			                                 app_task,
			                                 fifo_buffer_int,fifo_buffer_uint,
			                                 fifo_buffer_short,fifo_buffer_ushort,
										 	 fifo_buffer_float,fifo_buffer_double,
											 fifo_buffer_char, fifo_buffer_voidp,
											 fifo_buffer_msg);
		}
	
		return (0);	

	} catch (sc_report rpt) {		

		cout << rpt.what() << endl;

		if (rpt.get_severity()==SC_ERROR) {
			if(global_kista_xml_verbosity) {			
				cout << "ERROR reported by KisTA library (File: " << rpt.get_file_name() << ", Line: " <<  rpt.get_line_number() << ")." << endl;
			}
			create_MULTICUBE_error_output(sm_path_name,
			                              error_stop_exploration(),   // by default, KisTA states that an error is non-fatal
			                                                          // The user can state and unset the fatality, from the C++-API, through set_error_fatal_for_exploration, at any time,
			                                                          // and from the XML input before the simulation start
			                              rpt.what());
		}
		
		return(-1);
		
	}

}

