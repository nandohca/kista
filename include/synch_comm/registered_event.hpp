/*****************************************************************************

  registered_event.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

 *****************************************************************************/

#ifndef _REGISTERED_EVENT_HPP
#define _REGISTERED_EVENT_HPP

#include "systemc.h"

#include "defaults.hpp"

namespace kista {

	class registered_event : public sc_event {
	public:
	  registered_event(bool wait_release_unset_flag_par = true); // By default, a call to wait unset the flag
	  
	  void wait();

	  void set();
	  void unset();
	  
	private:
	  bool	flag;
	  bool  wait_release_unset_flag;
	};

	void wait(registered_event &re);

} // namespace KisTA

#endif
