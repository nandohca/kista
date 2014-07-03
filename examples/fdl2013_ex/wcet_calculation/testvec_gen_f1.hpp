// 
// testvec_gen_f1.hpp
// 
//   SystemC test generation for f1 functionality
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, May
//

#ifndef _TESTVEC_GEN_F1_HPP
#define _TESTVEC_GEN_F1_HPP

//#include "hetsc.h"
#include "systemc.h"

SC_MODULE(producer_f1) {
public:

  sc_fifo_out<unsigned short>	out1;
  sc_fifo_out<unsigned short>	out2;
  sc_fifo_out<unsigned short>	mode;

  void producer_proc();

  SC_CTOR(producer_f1) {
     SC_THREAD(producer_proc);
  }
};
  
#endif
//  endif of file
