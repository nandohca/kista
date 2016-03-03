/*****************************************************************************

  hw_resource.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: At home
  Deparment:   
  Date: 2015 August
   
  Class to support an abstract hw resource

 *****************************************************************************/
#ifndef HW_RESOURCE_HPP
#define HW_RESOURCE_HPP

#include <systemc.h>

#include "types.hpp"

namespace kista {
	
enum resource_t {
	PROCESSING_ELEMENT = 0,
	MEMORY_RESOURCE =1
};

class hw_resource: public sc_module {
public:

	hw_resource(sc_module_name name, resource_t type);
	
	resource_t get_type();
	
	// return the pointer in memory to the object of the given type
	operator phy_address () {return get_address();}
	phy_address get_address();
		
	// equality operator
	bool operator==(hw_resource& rha);

private:
	void *address;
	resource_t resource_type;
};


} // namespace kista

#endif
