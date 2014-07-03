// 
// Test bench of the FDL 2013 paper example
// 
//
// Author: F. Herrera
// Institution: KTH
// Date: 2013, April
//


//#include "hetsc.h"
#include "systemc.h"

#include <stdio.h>

#include "basic_scope.h"

#include "constants.h"
#include "sc_f1.h"
#include "sc_f2.h"
#include "sc_f3.h"

sc_time t1,t2,t3;


SC_MODULE(producer) {
public:

  sc_fifo_out<unsigned short>	out1;
  sc_fifo_out<unsigned short>	out2;
  sc_fifo_out<unsigned short>	mode;

  void producer_proc();

  SC_CTOR(producer) {
     SC_THREAD(producer_proc);
  }
};

SC_MODULE(consumer) {
public:

  sc_fifo_in<unsigned short>	in;

  void consumer_proc();

  SC_CTOR(consumer) {
     SC_THREAD(consumer_proc);
  }

};


void producer::producer_proc()
{
	unsigned int i;
	unsigned short int test_vec_i1[SIZE_1] = {27,2,1,214,52};
	unsigned short int test_vec_i2[SIZE_1] = {255,254,127,2,1};;
	unsigned short int test_vec_i3 = 0;

	mode.write(test_vec_i3);
	
	for ( i = 0; i < SIZE_1; i++ ) {
		out1.write(test_vec_i1[i]);
		out2.write(test_vec_i2[i]);
	}
}

void consumer::consumer_proc()
{
	do {
		cout << "consumer " << name() << ":" << in.read() << " at time " << sc_time_stamp() << endl;
		t2=sc_time_stamp(); // unefficient, but compatible with SystemC 2.2.0
	} while(true);
}


template<class T>
SC_MODULE(unzip2) {
public:

  sc_fifo_in<T>		in;
  sc_fifo_out<T>	out1;
  sc_fifo_out<T>	out2;

  void unzip2_proc();

  SC_CTOR(unzip2) {
     SC_THREAD(unzip2_proc);
  }

private:
	T data;
};

template <class T>
void unzip2<T>::unzip2_proc() {
	
	while(true) {
		in.read(data);
		out1.write(data);
		out2.write(data);
	}
}



int sc_main(int sc_argc, char *sc_argv[])
{
	t1=sc_time_stamp();
  
    sc_fifo<unsigned short>  ch1("ch1");
    sc_fifo<unsigned short>  ch1_a("ch1_a");
    sc_fifo<unsigned short>  ch1_b("ch1_b");
    sc_fifo<unsigned short>  ch2("ch2");
    sc_fifo<unsigned short>  ch3("ch3");
    sc_fifo<unsigned short>  ch4("ch4");
    sc_fifo<unsigned short>  ch5("ch5");
    sc_fifo<unsigned short>  ch6("ch6");

    // test bench
    producer prod("prod");
	prod.out1(ch1);
	prod.out2(ch2);
	prod.mode(ch3);

    consumer cons1("cons1");
	cons1.in(ch5);

    consumer cons2("cons2");
	cons2.in(ch6);
    
    // system
    sc_f1 scf1("scf1");
	scf1.in1(ch1_a);
	scf1.in2(ch2);
	scf1.mode(ch3);
	scf1.out(ch4);

    sc_f2 scf2("scf2");
	scf2.in(ch4);
	scf2.out(ch5);
	
	unzip2<unsigned short> uzip2("uzip2");
	uzip2.in(ch1);
	uzip2.out1(ch1_a);
	uzip2.out2(ch1_b);
    
    sc_f3 scf3("scf3");
	scf3.in(ch1_b);
	scf3.out(ch6);

uc_segment_time = 0;
uc_segment_instr =0;

    t1=sc_time_stamp();
    sc_start();
	sc_stop();
	
    t3=t2-t1;

cout << "uc_segment_time: " << uc_segment_time << endl;
cout << "uc_segment_instr:" << uc_segment_instr << endl;

    cout<<"Simulation: Start: " << t1 << " ends: "<< t2 <<" latency: " <<t3 << endl;
    //printf("\n\nSystemC simulation of Edge Detector complete. Press the Enter key to continue.\n");
    //c = getc(stdin);

    return(0);
}

