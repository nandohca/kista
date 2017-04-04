/*****************************************************************************

  app_element.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: At home
  Deparment:   
  Date: 2015 Sept.
   
  Class to support an abstract application element

 *****************************************************************************/
#ifndef APP_ELEMENT_HPP
#define APP_ELEMENT_HPP

#include <systemc.h>

#include "types.hpp"

namespace kista {
	
enum app_element_e {
	// modularity
	APPLICATION_ELEMENT =0,
	// computation
	TASK_ELEMENT,
	// communication and synchronization
	COMM_SYNCH_SHARED_VAR,
	COMM_SYNCH_FIFO
};

class app_element: public sc_module {
public:

	app_element(sc_module_name name, app_element_e type = TASK_ELEMENT);
	
	app_element_e get_app_element_type();
	
	std::string name();
	
	// return the pointer in memory to the object of the given type
	operator logic_address () {return get_address();}
	logic_address get_address();
	
	// equality operator
	bool operator==(app_element& rha);

	// 
	void		set_scheduler(scheduler *scheduler_p); 
	scheduler	*get_scheduler();	

private:
	void *address;
	app_element_e type;

    scheduler *sched_p;		// scheduler the application element has been assigned to
	
};


} // namespace kista

#endif
