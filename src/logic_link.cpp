/*****************************************************************************

  logic_link.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 January

 *****************************************************************************/

#ifndef LOGIC_LINK_CPP
#define LOGIC_LINK_CPP

#include "global_elements.hpp"

#include "logic_link.hpp"

namespace kista {



logic_link_info_t::logic_link_info_t(const char *name_par)
						: logic_link_t(name_par)
						{
							sys_conn.push_back(this);
							sys_conn_by_name[std::string(name_par)]= this;
							//ll_transfer_completed.write(false);
						}
	
logic_link_info_t::logic_link_info_t(const char *name_par, logic_link_t llink_par)
						: logic_link_t(name_par, llink_par.src, llink_par.dest)
						{
							sys_conn.push_back(this);
							sys_conn_by_name[std::string(name_par)]= this;
							//ll_transfer_completed.write(false);
						}
											
logic_link_info_t::logic_link_info_t(const char *name_par, logic_address src_address, logic_address dest_address)
						: logic_link_t(name_par, src_address, dest_address)
						{
							sys_conn.push_back(this);
							sys_conn_by_name[std::string(name_par)]= this;
							//ll_transfer_completed.write(false);
						}

logic_link_info_t::logic_link_info_t(const char *name_par, const char *src_name_par, const char *dest_name_par)
						: logic_link_t(name_par, src_name_par, dest_name_par)
						{
							sys_conn.push_back(this);
							sys_conn_by_name[std::string(name_par)]= this;
							//ll_transfer_completed.write(false);
						}

// Specialized function to get logic address by elem name
template<>
logic_address get_address_by_elem_name<logic_address>(std::string elem_name) {
	std::string msg;
	
	tasks_info_by_name_t::iterator task_info_it;
	
/*	
	cout << "A) SYSTEM TASKS: " << task_info_by_name.size() << endl;
	for(task_by_name_it=task_info_by_name.begin();task_by_name_it!=task_info_by_name.end(); task_by_name_it++) {
		cout << task_by_name_it->first << endl;
	}

	cout << "B) ENV TASKS: " << env_tasks_by_name.size() << endl;
	for(etn_it=env_tasks_by_name.begin();etn_it!=env_tasks_by_name.end(); etn_it++) {
		cout << etn_it->first << endl;
	}
*/

	// First, looks for the logic address (TASK) in the declared system-level tasks
	task_info_it = task_info_by_name.find(elem_name);
	
	// if found, it returns the logic address of the system task
	if(task_info_it != task_info_by_name.end()) return (logic_address)task_info_it->second;
	
	// Reaching this point means that no task with such a name is found in the system task set,
	// so it tries to find it in the environment set
	
	task_info_it = env_tasks_by_name.find(elem_name);
	
	if(task_info_it != env_tasks_by_name.end()) return task_info_it->second; // logic_address is task_base_t pointer
	else {
		msg = "Logic address (task info pointer) could not be obtained for task ";
		msg += elem_name;
		SC_REPORT_ERROR("KisTA", msg.c_str());
	}
}


} // end kista namespace

#endif
