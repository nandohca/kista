/*****************************************************************************

  parse_kista_xml_command.cpp
  
   Contains the functionality which pases the kista-xml command.
  
   This file belongs to the kista library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 July

 *****************************************************************************/

#ifndef _PARSE_KISTA_XML_COMMAND
#define _PARSE_KISTA_XML_COMMAND

#include <stdio.h>
#include <string.h>

#include <systemc.h>

#include <kista.hpp>

// For the moment, the plugin states at least N_hyperperiods

#define PATH_NAME_SIZE 500

#define CONFIGURATION_OPTION_NAME "xml_system_configuration"
#define SYSTEM_METRICS_OPTION_NAME "xml_system_metrics"
#define REFERENCE_XSD_OPTION_NAME "reference_xsd"
#define SYSTEM_METRICS_DEFINITION_OPTION_NAME "xml_system_metrics_definition"

void check_length_argument(char *argv) {
	std::string rpt_msg;
	if(strlen(argv)+1>PATH_NAME_SIZE) { // +1 to consider that we will need and additional
	                                    // byte in the buffer for the \0
	    rpt_msg = "Argument ";
	    rpt_msg += argv;
	    rpt_msg += " is too long (current limit ";
	    rpt_msg += to_string(PATH_NAME_SIZE);
	    rpt_msg += "characteres).";
		SC_REPORT_ERROR("KisTA-XML",rpt_msg.c_str());
	}
}
	
void append_kista_xml_command_use(std::string &msg) {
	msg += "Usage:\n";
	msg += "kista-xml <XML system file>";
	msg += " [--"; msg += CONFIGURATION_OPTION_NAME; msg += "=sc_path_name]";
	msg += " [--"; msg += SYSTEM_METRICS_OPTION_NAME; msg += "=sm_path_name]";
	msg += " [--"; msg += REFERENCE_XSD_OPTION_NAME; msg += "=xsd_path_name]\n";
	msg += " [--"; msg += SYSTEM_METRICS_DEFINITION_OPTION_NAME; msg += "=smd_path_name]\n";
	msg += "   where optional parameters can be passed in any order, and before or after the XML system file.\n";
	msg += "\t * sc_path_name: path to system configuration file (where KisTA reads input parameters whic complete system definition).\n";
	msg += "\t * sm_path_name: path to system metrics file (where KisTA has to dump its output).\n";
	msg += "\t * xsd_path_name: path to reference schema defined by M3Explorer for M3Explorer/KisTA interface.\n";
	msg += "\t * smd_path_name: path to system metrics definition file (which output paramaters have to be actually reported by KisTA). The design space definition file can be passed for it, otherwise, ALL available metrics reported.\n";
	msg += "Note: argument values have to immediately follow the = character.\n\n";
}

void report_command_error() {
	std::string rpt_msg;
	rpt_msg = "In arguments passed to kista-xml command.\n\n";
	append_kista_xml_command_use(rpt_msg);
	SC_REPORT_ERROR("Kista-XML",rpt_msg.c_str());		
}

void parse_command_line(int argc, char **argv,
						char *system_xml_file_path_name,
						char *sc_path_name, // leave as it is no system configuration file specified in the command line
						char *sm_path_name, // default output file name has to be loaded
						char *xsd_path_name, // parses the name or leave it as is
						char *smd_path_name  // leave as it is no system configuration file specified in the command line
						) {
	
	char *curr_substr;
	unsigned int option_size;
	
	std::string rpt_msg;
	
	bool system_xml_file_path_name_flag = false;
	bool sc_path_name_flag = false;
	bool sm_path_name_flag = false;
	bool xsd_path_name_flag = false;
	bool smd_path_name_flag = false;	
	
	if (argc <= 1) {
		report_command_error();
	}

	for(int i=1; i<argc; i++) {
		
		check_length_argument(argv[i]);
					
		if(!strncmp(argv[i],"--",2)) { // argument
			
			curr_substr = strpbrk(argv[i],"=");
			
			if((curr_substr==NULL) || (curr_substr<=&argv[i][2]) ) {
				report_command_error();
			}

			// calculates size of option
			option_size = (curr_substr - &argv[i][2])/sizeof(char);

			if( (option_size == strlen(CONFIGURATION_OPTION_NAME)) &&
			    !strncasecmp(&argv[i][2],CONFIGURATION_OPTION_NAME, option_size ) ) {
				// detected configuration of configuration path name
				if(global_kista_xml_verbosity) {
					printf("Kista-XML INFO: Read configuration file path name (KisTA input) %s\n",&curr_substr[1]);
				}

				if( strlen(&curr_substr[1]) > PATH_NAME_SIZE) {
					rpt_msg = "The system configuration file";
					rpt_msg += CONFIGURATION_OPTION_NAME;
					rpt_msg += "has a name too big (";
					rpt_msg += strlen(&curr_substr[1]);
					rpt_msg += ") .Current limit of Kista-XML parser is ";
					rpt_msg += to_string(PATH_NAME_SIZE);
					SC_REPORT_ERROR("Kista-XML",rpt_msg.c_str());
				}
				strcpy(sc_path_name,&curr_substr[1]);
				sc_path_name_flag = true;
			}
			else if( (option_size == strlen(SYSTEM_METRICS_OPTION_NAME)) &&
			         !strncasecmp(&argv[i][2],SYSTEM_METRICS_OPTION_NAME, strlen(SYSTEM_METRICS_OPTION_NAME) ) ) {
				// detected configuration of system metric path name
				if(global_kista_xml_verbosity) {
					printf("Kista-XML INFO: Read system metric file path name (KisTA output) %s\n",&curr_substr[1]);
				}

				if( strlen(&curr_substr[1]) > PATH_NAME_SIZE) {
					rpt_msg = "The system metrics file";
					rpt_msg += SYSTEM_METRICS_OPTION_NAME;
					rpt_msg += "has a name too big (";
					rpt_msg += strlen(&curr_substr[1]);
					rpt_msg += ") .Current limit of Kista-XML parser is ";
					rpt_msg += to_string(PATH_NAME_SIZE);
					SC_REPORT_ERROR("Kista-XML",rpt_msg.c_str());
				}
								
				strcpy(sm_path_name,&curr_substr[1]);
				
				sm_path_name_flag = true;
				
			} else if( (option_size == strlen(REFERENCE_XSD_OPTION_NAME)) &&
			           !strncasecmp(&argv[i][2],REFERENCE_XSD_OPTION_NAME,  strlen(REFERENCE_XSD_OPTION_NAME) ) ) {
				// detected configuration of XSD of M3Explorer if

				if(global_kista_xml_verbosity) {
					printf("Kista-XML INFO: Read reference xsd file path name (KisTA input) %s\n",&curr_substr[1]);
				}

				if( strlen(&curr_substr[1]) > PATH_NAME_SIZE) {
					rpt_msg = "The system metrics file";
					rpt_msg += REFERENCE_XSD_OPTION_NAME;
					rpt_msg += "has a name too big (";
					rpt_msg += strlen(&curr_substr[1]);
					rpt_msg += ") .Current limit of Kista-XML parser is ";
					rpt_msg += to_string(PATH_NAME_SIZE);
					SC_REPORT_ERROR("Kista-XML",rpt_msg.c_str());
				}
								
				strcpy(xsd_path_name,&curr_substr[1]);
				
				xsd_path_name_flag = true;
				
			} else if( (option_size == strlen(SYSTEM_METRICS_DEFINITION_OPTION_NAME)) &&
			           !strncasecmp(&argv[i][2],SYSTEM_METRICS_DEFINITION_OPTION_NAME, strlen(SYSTEM_METRICS_DEFINITION_OPTION_NAME) ) ) {
				// detected configuration of configuration path name

				if(global_kista_xml_verbosity) {
					printf("Kista-XML INFO: Read system metric definition file path name (KisTA input) %s\n",&curr_substr[1]);
				}

				if( strlen(&curr_substr[1]) > PATH_NAME_SIZE) {
					rpt_msg = "The system metrics definition file";
					rpt_msg += SYSTEM_METRICS_DEFINITION_OPTION_NAME;
					rpt_msg += "has a name too big (";
					rpt_msg += strlen(&curr_substr[1]);
					rpt_msg += ") .Current limit of Kista-XML parser is ";
					rpt_msg += to_string(PATH_NAME_SIZE);
					SC_REPORT_ERROR("Kista-XML",rpt_msg.c_str());
				}
								
				strcpy(smd_path_name,&curr_substr[1]);
				
				smd_path_name_flag = true;
				
			} else {
				//printf("Kista-XML ERROR: Parameter not recognized\n");
				report_command_error();
			}
		} else {
			if(argv[i]==NULL || (strlen(argv[i])<1) ) {
				report_command_error();
			}

			if(global_kista_xml_verbosity) {
				printf("Kista-XML INFO: Read XML system file: %s\n",argv[i]);
			}

			strcpy(system_xml_file_path_name, argv[i]);
			
			system_xml_file_path_name_flag = true;
		}
	}
	
	if(system_xml_file_path_name_flag == false) {
		report_command_error();
	}
	
	if(sc_path_name_flag == false) {
		sc_path_name = NULL; // Ensure its null value
		if(global_kista_xml_verbosity) {		
			rpt_msg = "System configuration file not provided.\n";
			rpt_msg += "KisTA will look for a parameters section in the <XML system file>.";				
			SC_REPORT_WARNING("KisTA-XML",rpt_msg.c_str());
		}	
	}
	
	if(global_kista_xml_verbosity) {
		if(sm_path_name_flag == false) {
				rpt_msg = "System metrics file name not provided.\n";
				rpt_msg += "\tBy default, the output will be dumped to a file named:";
				rpt_msg += sm_path_name;
				rpt_msg += ".";
				SC_REPORT_WARNING("KisTA-XML",rpt_msg.c_str());
		}
	
		if(xsd_path_name_flag) {
				SC_REPORT_WARNING("Kista","Provided XSD file is currently ignored by KisTA");
		}
	
		if(smd_path_name_flag == false) {
			rpt_msg = "System metrics definition file name not provided.\n";
			rpt_msg += "ALL the available metrics! will be dumped to the output system metrics file:";
			rpt_msg += sm_path_name;
			rpt_msg += ".";
			SC_REPORT_WARNING("KisTA-XML",rpt_msg.c_str());
		}
	}
}

#endif
