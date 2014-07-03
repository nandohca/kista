/*****************************************************************************

  kista_version.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 January

 *****************************************************************************/

#ifndef KISTA_VER_CPP
#define KISTA_VER_CPP

#include <string>

#include "kista_version.hpp"

namespace kista {

// defined through root Makefile
//#define KISTA_VERSION_STRING "v1.0"

#define VER2(x) #x
#define VER1(x) VER2(x)
#define KISTA_VERSION_STRING VER1(KISTA_VERSION)

// note: three macros are required to avoid "KISTA_VERSION", instead KISTA_VERSION value converted to string

kista_version::kista_version() {
	print();
}

void kista_version::print() {
	printf(" ***************************************************************************** \n");
	printf(" KisTA Library : %s\n", (char *)KISTA_VERSION_STRING);
	printf(" \n");
	printf(" All rights reserved until further definition of the License.");
	printf(" \n");
	printf(" Author: F. Herrera \n");
	printf(" Institution: KTH \n");
	printf(" Department:   Electronic Systems \n");
	printf(" Date: 2014 March \n");
	printf(" Compilation date: %s at %s\n", __DATE__,__TIME__);
	printf(" ***************************************************************************** \n");
}


static kista_version kista_version_obj;

} // namespace kista

#endif
