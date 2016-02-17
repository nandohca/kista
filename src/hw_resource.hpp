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

enum resource_t {
	PROCESSING_ELEMENT = 0,
	MEMORY_RESOURCE =1
};

class hw_resource {
public:

	hw_resource(resource_t type = PROCESSING_ELEMENT);
	
	resource_t get_resource_type();
	
	// return the pointer in memory to the object of the given type
	void *operator ();
	void *get_address();
	
	// equality operator
	bool operator==(hw_resource& rha);

private:
	void *address;
	resource_t type;
};

#endif
