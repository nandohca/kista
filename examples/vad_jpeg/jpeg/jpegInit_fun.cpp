/*****************************************************************************

  GlobalInit_fun.cpp

  For Global initialization before start of simulation

  Author: F. Herrera
  Institution: KTH
  Deparment: ICT/ES
  Date: 2014 February

 *****************************************************************************/

#include "kista.hpp"

//#include "fun/xxx.h"

extern "C"
void jpeg_init_fun() {
	//jpeg_reset();
	cout << "Global initialization for jpeg application done." << endl;
}
