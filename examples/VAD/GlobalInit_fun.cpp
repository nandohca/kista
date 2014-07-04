/*****************************************************************************

  GlobalInit_fun.cpp

  For Global initialization before start of simulation

  Author: F. Herrera
  Institution: KTH
  Deparment: ICT/ES
  Date: 2014 February

 *****************************************************************************/

#include "kista.hpp"

#include "fun/vad.h"

extern "C"
void global_init_fun() {
	vad_reset();
	cout << "Global system init did vad_reset" << endl;
}
