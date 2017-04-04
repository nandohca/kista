/*****************************************************************************

  annotation_plugin.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

  Note: 
	Selects the annotation plugin

 *****************************************************************************/

#ifndef ANNOTATION_PLUGIN_HPP
#define ANNOTATION_PLUGIN_HPP

#include "defaults.hpp"

#ifdef _USE_AUTOMATIC_CODE_ANNOTATION

	
// select the plugin
#ifdef _USE_SCOPE_OPCOST_ANNOTATION
#include "annotation_plugin/scope_opcost.hpp"
#include "annotation_plugin/scope_opcost_globals.hpp"
#endif

// From here, selectors to other annotation mechanisms can be included

#endif



#endif
