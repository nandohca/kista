// 
// testvec_gen_f3.hpp
// 
//   SystemC test generation for f1 functionality
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, May
//

#ifndef _TESTVEC_GEN_F3_HPP
#define _TESTVEC_GEN_F3_HPP

//#include "hetsc.h"
#include "systemc.h"

SC_MODULE(producer_f3) {
public:

  sc_fifo_out<unsigned short>	out1;

  void producer_proc();

  SC_CTOR(producer_f3) {
     SC_THREAD(producer_proc);
  }
};

#endif

