/*****************************************************************************

  annotation.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

  Note: 
	Generic annotation header (which selects the specific annotation headers)

 *****************************************************************************/

#ifndef _KISTA_ANNOTATION_HPP
#define _KISTA_ANNOTATION_HPP

#include "defaults.hpp"

#ifdef _USE_AUTOMATIC_CODE_ANNOTATION

// select the plugin
#ifdef _USE_SCOPE_OPCOST_ANNOTATION
//#include "annotation_plugin/scope_opcost.hpp"
// While not separate compilation for the library
#include "annotation_plugin/scope_annotation.hpp"
#endif

// From here, selectors to other annotation mechanisms can be included

#endif



#endif
