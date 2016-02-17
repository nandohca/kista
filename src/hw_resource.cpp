/*****************************************************************************

  hw_resource.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: At home
  Deparment:   
  Date: 2015 August
   
  Class to support an abstract hw resource implementation
  
 *****************************************************************************/
#ifndef HW_RESOURCE_CPP
#define HW_RESOURCE_CPP

hw_resource::hw_resource(resource_t type = PROCESSING_ELEMENT) {
	address = this;
	type = type;
}

resource_t hw_resource::get_resource_type() {
	return type;
}
	
// return the pointer in memory to the object of the given type
void* hw_resource::operator() {
	return (void *)this;
}

void hw_resource*get_address() {
	return (void *)this;
}

bool operator==(hw_resource& rha) {
	if ( (this->address == rha.address) && (this->type == rha.type) ) return true;
	else return false;
}

#endif
