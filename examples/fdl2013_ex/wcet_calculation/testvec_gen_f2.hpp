// 
// testvec_gen_f2.hpp
// 
//   SystemC test generation for f1 functionality
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, May
//


#ifndef _TESTVEC_GEN_F2_HPP
#define _TESTVEC_GEN_F2_HPP

//#include "hetsc.h"
#include "systemc.h"

SC_MODULE(producer_f2) {
public:

  sc_fifo_out<unsigned short>	out1;

  void producer_proc();

  SC_CTOR(producer_f2) {
     SC_THREAD(producer_proc);
  }
};

#endif
