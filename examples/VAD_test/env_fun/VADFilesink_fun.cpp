/*****************************************************************************

  VADFileflag_fun.cpp

  Environment Process for the VAD example
  
  Note: sink process getting the VAD expected output data from a file

  Author: F. Herrera
  Institution: KTH
  Deparment: ICT/ES
  Date: 2014 April

 *****************************************************************************/

#include "kista.hpp"

#include "../compacted_types.hpp"

#include "../adse_defines.h"
#include "../fun/vad.h"

#include "../fun/typedef.h"

#include <iostream>
#include <fstream>
#include <strstream>

#define TASK_NAME TASK_VADflag

#define _CHECK_RESULT

// Not longer necessary, test name passed as a parameter from the system configuration file
//#define TEST_NAME "dtx01"

#define TEST_FOLDER "./env_fun/dtx_tests/"

unsigned int frames_proc;
unsigned int frames_checked;

//#define PRINT_SRC_PARSING

extern "C"
void VADFilesink_fun() {
// --------------------------
// init and state section
// --------------------------
	// Declaration and initialization of internal variables
	short in_vad;

	short expected_vad;
		
	ifstream vad_out_file;
	std::string file_name;
	std::string line;
	std::stringstream linestream(line);
	std::string token;
	std::string delimiter = " ";
	
	param_value_t *param_value_p;
	std::string test_name;
	
	std::string msg;
			
	file_name = TEST_FOLDER;
	file_name += "vad_";
param_value_p = get_global_parameter("test_name");
	if(param_value_p==NULL) {
		SC_REPORT_ERROR("Env. user code: VAD source","Error retrieving test name");
	}	
	test_name = (*param_value_p)[0];	
	//file_name += TEST_NAME;
	file_name += test_name;
	file_name += ".out";
	cout << "Opening " << file_name << endl;
	
	vad_out_file.open(file_name);
	
	if( !vad_out_file.is_open() ) {
		cout << "Error: " << file_name << " not open." << endl;
		SC_REPORT_ERROR("User Code","VAD Output test-bench file not open.");
	}
	
//------------------------------------------------
// Environment initialization code
// (to be hidden or autogenerated in the future)
//------------------------------------------------

  	IO_FIFO_BUFFER_SHORT_REF(pin_vad,EDGE_vad);

//--------------------------
// Task initialization code
//--------------------------

	frames_proc = 0;
	frames_checked = 0;


	while(true) {
//--------------
// Task code
//--------------

		pin_vad->read(in_vad);

		PRINT_TASK(TASK_NAME, ": recv (" << in_vad << ")");

//		cout << frames_proc << "th-frame recv: " << in_vad << endl;

#ifdef _CHECK_RESULT
		if(!vad_out_file.eof()) {
			while(getline(vad_out_file,line)) {
#ifdef PRINT_SRC_PARSING			
				cout << "VADsink:line (" << line.size() << ") : " << line << endl;	
#endif
				if(line.size()==0) continue; // empty line, looks for the next line
				
				std::stringstream linestream(line);
				
				linestream >> token;
	//			if(!linestream.good()) SC_REPORT_ERROR("Env user code", "While parsing vad input. Reading first token");
				
				if(token=="--") continue;			
	//			if(!linestream.good()) SC_REPORT_ERROR("Env user code", "While parsing vad input. Reading first token");

				expected_vad = (short)atoi(token.c_str());
				if(in_vad!=expected_vad) set_functional_failure();
				frames_checked++;
				break; // stops parsing the output file until a new output is received
			}
		}		
#endif
		frames_proc++;
	}

}
	


