/*****************************************************************************

  logic_comm_res.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 March


 logical communication resource

 *****************************************************************************/

#ifndef _LOGIC_COMMUNICATION_RESOURCE_HPP
#define _LOGIC_COMMUNICATION_RESOURCE_HPP

#include <systemc.h>

#include "defaults.hpp"

#include "comm_res.hpp"

namespace kista {

typedef comm_res<logic_address> logic_comm_res_t;

} // namespace kista

#endif
