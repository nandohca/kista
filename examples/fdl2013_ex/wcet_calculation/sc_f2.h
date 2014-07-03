// 
// sc_f2.cpp
//
//
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, April
//

#ifndef _SC_F2_H
#define _SC_F2_H

SC_MODULE(sc_f2) {
  sc_fifo_in<unsigned short >      in;
  sc_fifo_out<unsigned short >     out;

  void sc_f2_proc();

  SC_CTOR(sc_f2) {
     SC_THREAD(sc_f2_proc);
  }
};

#endif

