/*****************************************************************************

  2drivers.cpp
  Suspend Resume example
  This is to check what SystemC does when two drivers (2 processes)
  write to the same signal at different times.

  Institution: KTH
  Deparment:   Electronic Systems

  Institution: KTH Electronic Systems
  Date: 2013 January

 *****************************************************************************/


#include <systemc.h>

sc_signal<int> s;

class producer : public sc_module
{
   public:

     SC_HAS_PROCESS(producer);

     producer(sc_module_name name) : sc_module(name)
     {
       SC_THREAD(producer_proc1);
       SC_THREAD(producer_proc2);     
     }

     void producer_proc1()
     {
		wait(10, SC_NS); // shift
	    while(true) {
			s.write(1);
			wait(20, SC_NS);
		}
     }

     void producer_proc2()
     {
	    while(true) {
			s.write(-1);
			wait(20, SC_NS);
		}
     }

};

class consumer : public sc_module
{
   public:

     SC_HAS_PROCESS(consumer);

     consumer(sc_module_name name) : sc_module(name)
     {
       SC_THREAD(consumer_proc);
       sensitive << s;
     }

     void consumer_proc()
     {
        while (true) {
			cout << "s= " << s.read() << " at time " << sc_time_stamp() << endl;
			wait();
        }
     }
};

class top : public sc_module
{
   public:

     producer *prod_inst;
     consumer *cons_inst;

     top(sc_module_name name) : sc_module(name)
     {
       prod_inst = new producer("Producer1");
       cons_inst = new consumer("Consumer1");
     }
};

int sc_main (int, char *[]) {
   top top1("Top1");
   sc_start(200,SC_NS);
   return 0;
}
