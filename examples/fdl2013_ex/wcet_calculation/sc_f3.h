// 
// sc_f3.cpp
//
//
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, April
//

#ifndef _SC_F3_H
#define _SC_F3_H

SC_MODULE(sc_f3) {
  sc_fifo_in<unsigned short >      in;
  sc_fifo_out<unsigned short >     out;

  void sc_f3_proc();

  SC_CTOR(sc_f3) {
     SC_THREAD(sc_f3_proc);
  }
};

#endif

