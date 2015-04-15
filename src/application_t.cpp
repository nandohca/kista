/*****************************************************************************

  application.cpp
  
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


#ifndef APPLICATION_CPP
#define APPLICATION_CPP

#include "types.hpp"

#include "global_elements.hpp"

#include "application_t.hpp"

namespace kista {
	
application_t::application_t(sc_module_name name) : sc_module(name){
	applications_by_name[this->name()] = this;
}

	
} // namespace KisTA

#endif

