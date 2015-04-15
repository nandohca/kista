/*****************************************************************************

  application.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: University of Cantabria
  Deparment:   GIM
  Date: 2015 April
   
  The application in KisTA is basically a logical container, which enables
  the clustering of tasks and the association to such a cluster of a set
  of attributes:
		- Criticality (criticality associated to the inner tasks should be
		               coherent)
		- Constraints (associated to the application and which, somehow,
		               involve the tasks)

  For the simplests cases, 1 application KisTa models can be build.
  The modeller uses only tasks and all of them are assumed to belong to
  a default application application.

 *****************************************************************************/


#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "types.hpp"

namespace kista {
	
class application_t: public sc_module {
public:

	application_t(sc_module_name name);
 
	//taskset_t tasks; // tasks assigned to the application
	tasks_info_by_name_t	tasks_by_name;
		
	unsigned int criticality;
	
	// TBD: add links to constraints
};
	
} // namespace KisTA

#endif

