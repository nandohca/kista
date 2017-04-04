/*****************************************************************************

  semaphore.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

  Notes: 

 *****************************************************************************/

#ifndef _SEMAPHORE_HPP
#define _SEMAPHORE_HPP

#include "systemc.h"

#include "defaults.hpp"

namespace kista {

	class semaphore : public sc_semaphore {
	public:
		explicit semaphore( int _size);
		semaphore( const char* _name, int _size);
	
		int wait(); // overloaded to support notification to kista
					// virtual int wait in the sc_semaphore interface
	};

	typedef semaphore registered_event_queue;

} // namespace KisTA

#endif
