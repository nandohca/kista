/*****************************************************************************

  sus_res.cpp
  Suspend Resume example
  This is to check SystemC features (since 2.3 library does not provide examples on them).

  Institution: KTH
  Deparment:   Electronic Systems

  Institution: KTH Electronic Systems
  Date: 2013 January

 *****************************************************************************/


#include <systemc.h>

sc_process_handle p,c;

bool end_from_consumer = false;

class producer : public sc_module
{
   public:

     SC_HAS_PROCESS(producer);

     producer(sc_module_name name) : sc_module(name)
     {
       SC_THREAD(producer_proc);
       p = sc_get_current_process_handle();
     }

     void producer_proc()
     {
		assert(p.valid());
		cout << "process: " << p.name();
		cout << " of type " << p.proc_kind() << endl;
		
		wait(20, SC_NS);
		
		c.suspend();
		
		wait(20, SC_NS);

		c.resume();
				
		wait(110, SC_NS);
		
		c.suspend();
				
		wait(200, SC_NS);
		
		c.resume();
		
		end_from_consumer = true;
		
     }

};

class consumer : public sc_module
{
   public:

     SC_HAS_PROCESS(consumer);

     consumer(sc_module_name name) : sc_module(name)
     {
       SC_THREAD(consumer_proc);
       c = sc_get_current_process_handle();
     }

     void consumer_proc()
     {
		assert(p.valid());
		cout << "process: " << c.name();
		cout << " of type " << c.proc_kind() << endl;
        while (true) {
			wait (100, SC_NS);
			cout << "consumer: " << sc_time_stamp() << endl;
			if (end_from_consumer) sc_stop();
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
   sc_start();
   return 0;
}
