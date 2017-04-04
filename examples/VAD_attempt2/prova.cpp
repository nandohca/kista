/*****************************************************************************

  EnergyComputation_fun.cpp

  Task function for EnergyComputation

  Task function contains the task functionality and also communication and
  synchronization calls (therefore, it is not "pure" functionality)

  Task function is a very generic mechanism for supporting any task
  "processing and communication structure".

  From the XML interface, specific templates of "processing and communication structure"
  can be used to automate the generation of this structure, so to hide this
  from the user.

  Author: E. Paone
  Institution: Polimi
  Deparment: DEIB
  Date: 2014 January

  Edtions for last KisTA version:
  Author: F. Herrera
  Institution: KTH
  Deparment: ICT/ES
  Date: 2014 February

 *****************************************************************************/

#include "kista.hpp"

extern "C"
void t2_fun() {
		cout << "T2: exec. at time " << sc_time_stamp() << endl;
}

extern "C" 
PERIODIC_TASK(T2_periodic_task_wrapper,t2_fun);
