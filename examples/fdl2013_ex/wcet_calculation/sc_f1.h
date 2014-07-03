// 
// sc_f1.h
//
//
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, April
//

#ifndef _SC_F1_H
#define _SC_F1_H

SC_MODULE(sc_f1) {
  sc_fifo_in<unsigned short >      in1;
  sc_fifo_in<unsigned short >      in2;
  sc_fifo_in<unsigned short >      mode;
  sc_fifo_out<unsigned short >     out;

  void sc_f1_proc();

  SC_CTOR(sc_f1) {
     SC_THREAD(sc_f1_proc);
  }
};

#endif


