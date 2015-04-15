/*****************************************************************************

  types.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January
   
  Types and forward declarations 

 *****************************************************************************/

#ifndef _TYPES_HPP
#define _TYPES_HPP

#include <map>

#include "systemc.h"

namespace kista {

// forward declarations
// -------------------------
class sketch_report_t;
class application_t;
class task_info_t;
typedef task_info_t* logic_address;  		// let define task links
template<class address_type> class link_t;
typedef link_t<logic_address> logic_link_t;
class logic_link_info_t;
class processing_element;
typedef processing_element*	  phy_address;	// let define PE links 
typedef link_t<phy_address>   phy_link_t;
class scheduler;
class phy_comm_res_t;
class tdma_bus;
template<class T> class fifo_buffer;

// A comparison function is required for actual comparison of the strings (and not only of string pointers)
struct const_char_cmp {
	bool operator()(const char *str1,const char *str2) const;
};
/*
struct const_char_cmp {
	bool operator()(const char *str1,const char *str2) const
	{
		return strcmp(str1,str2)<0;
	}
};
* */

// types
//--------------------------

// declaration of task function interface
typedef void (*VOIDFPTR)();

// Types for system applications
typedef std::map<std::string, application_t*>		applications_by_name_t;

// Types for system tasks (the basic functional unit mapped to the platfom)
typedef std::vector<task_info_t*> 					taskset_t;
typedef std::map<sc_process_handle, task_info_t*>	tasks_info_by_phandle_t;
typedef std::map<std::string, task_info_t*>			tasks_info_by_name_t;

// Declare a map table (log times for insertion and look up of tasks information based on task name)
typedef std::map<const char*, task_info_t*, const_char_cmp> taskset_by_name_t;


// similar for PEs
typedef std::map<std::string, processing_element*>	PE_by_name_t;

// global map types for communication resources
typedef std::map<std::string, phy_comm_res_t*>		phy_comm_res_by_name_t;

  // note: the const_char_cmp comparison function required to enable comparing the "string" typed key, and not the char pointer

// type for list of logic links (generic system-level connections) by name (it includes system, io, and environment links)
typedef std::vector<logic_link_info_t*> 			system_level_connection_set_t; // or logic link set
typedef std::map<std::string, logic_link_info_t*> 	system_level_connection_set_by_name_t; // or logic link set

// type for list of types from the logic link
typedef std::map<logic_link_t, std::string> 		system_level_connection_type_set_t;

typedef std::vector<scheduler*> 					sched_set_t;
typedef std::map<std::string, scheduler*> 			sched_set_by_name_t;

typedef std::vector<processing_element*> 			pe_set_t;
typedef std::vector<phy_comm_res_t*> 				phy_commres_set_t;
typedef std::vector<tdma_bus*> 					tdma_bus_set_t;


// typedefs for channel types
typedef std::map<std::string, fifo_buffer<int> *>              fifo_buffer_int_set_t;
typedef std::map<std::string, fifo_buffer<unsigned int> *>    fifo_buffer_uint_set_t;
typedef std::map<std::string, fifo_buffer<short> *>            fifo_buffer_short_set_t;
typedef std::map<std::string, fifo_buffer<unsigned short> *>  fifo_buffer_ushort_set_t;
typedef std::map<std::string, fifo_buffer<float> *>	          fifo_buffer_float_set_t;
typedef std::map<std::string, fifo_buffer<double> *>           fifo_buffer_double_set_t;
typedef std::map<std::string, fifo_buffer<char> *>             fifo_buffer_char_set_t;
typedef std::map<std::string, fifo_buffer<void *> *>           fifo_buffer_voidp_set_t;

class message_t {
	friend ostream& operator<<(ostream& os, message_t msg);
public:
	message_t();
	message_t(void *datap_var, size_t size_var); // pointer to the data and size
	
	// copy constructor
	message_t(const message_t& other);
	
	// Asignation
	message_t& operator =(const message_t& other);
	
	operator void* () {
		return (void *)this->datap;
	}
	
private:	
    char *datap;     // pointer to the data
	size_t host_size; // size of the message used to transfer the data by native host
};

// ostream operator to enable application to sc_fifo-based channels of message_t
ostream& operator<<(ostream& os, message_t msg);


typedef std::map<std::string, fifo_buffer<message_t> *>           fifo_buffer_message_set_t;

// Types for network interface modelling, and logic system level communication sets

class send_message_request_t {
public:

// the request id is defined bcomes with the destination PE,
	logic_link_info_t	*logic_link;
	phy_comm_res_t		*comm_res;
	phy_link_t			*phy_link;
	unsigned int		message_size;
	sc_semaphore		*completed_transactions_p;
	sc_time				request_time;
	
	// copy constructor
//	send_message_request_t(const &send_message_request_t other);
//	send_message_request_t(&send_message_request_t other);
};

// queue of sending request ordered by the request time, allowing several 
typedef std::multimap<sc_time, send_message_request_t>	send_req_queue_t;

// type for iterator range marking synchronous requests
typedef std::pair< send_req_queue_t::iterator, send_req_queue_t::iterator> synch_send_requests_t;

// types for vectors of logic links (to represent static communication schedule)
typedef std::vector<logic_link_info_t *>                               llink_set_t;
typedef std::map<const char*,logic_link_info_t *, const_char_cmp>	  llink_set_by_name_t;

// task static schedules
typedef std::vector<std::string>				static_schedule_by_task_names_t;
typedef std::vector<task_info_t*>				static_schedule_by_task_info_pointers_t;

// communication static schedules
typedef std::vector<std::string>				static_comm_schedule_by_link_names_t;
typedef std::pair<std::string,std::string>		name_pair_t;
typedef std::vector<name_pair_t>				name_pair_vector_t;
typedef std::vector<name_pair_t>				name_pair_vector_t;

enum comm_synch_role_t	{ SYSTEM_COMMSYNCH = 0, 
                          IO_COMMSYNCH = 1,
                          ENV_COMMSYNCH = 2
};

enum io_comm_synch_sense_t	{ INPUT = 0, 
                          OUTPUT = 1,
                          INPUT_AND_OUTPUT = 2,
                          UNSET_IO_SENSE = 3
};

enum task_role_t	{ SYSTEM_TASK = 0, 
                      ENV_TASK = 1
                      };

enum app_visibility_t	{ UNSET_APP_VISIBILITY = 0, 
						  SET_IMPLICIT_APPS = 1,
                          SET_EXPLICIT_APPS = 2
                      };
                      
// METRIC DEFINITIONS

struct system_metric_definition_t {
	char *name;
	const char *unit;  // Interpretation is done based on the first char
						// if the string is null, default unit can be interpreted
				// Throughput:   'b' (bps), 'B' (bytes/s), 'k' or 'K' (Kbps) , 'M' (Mbps), 'G' (Gbps), 'T '(Tbps)				
				// Data size:    b (bits),  B (bytes), K/k (kilobits), M (Megabits), 'G' (Gbits), 'T '(Tbits)
				// Normalized Throughput:
				// ...othes
				//                         '-' '\0' (1), 'k' or 'K' (K) , 'M' (Mega), 'G' (Giga), 'T '(Tera)
				//                                       'm' (mili), 'u' (micro), 'n' (nano), 'p' (pico), 'f' (femto)
//	char *description;
};

// ACCURACY LEVELS

typedef int accuracy_level_t;

// Currently: 0 highest!

//
// Types for enabling define global parameters in the user code, and let then explore then from the XML interface
//
typedef std::vector<const char *> param_value_t;

// key : string with the param
// value: vector of strings. 
//         A vector of size one represents a scalar parameter
//         A vector of size>1 reflects a vector parameter
// Anyhow, the structure is valid for both cases
typedef std::map<const char *, param_value_t *, const_char_cmp> global_parameters_t;
	
} // end namespace

#endif
