/*****************************************************************************

  VADdecision_fun.cpp

  Task function for VADdecision

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

#include "compacted_types.hpp"

#include "adse_defines.h"
#include "fun/vad.h"
#include "prova_header.hpp"
#define TASK_NAME TASK_VADdec

extern "C"
BEGIN_TASK_WITH_TOKEN_INOUT(VADdecision_fun,_T7_IN,_T7_OUT,
// --------------------------
// init and state section
// --------------------------
	// Declaration and initialization of internal variables
	Pfloat *in_pvad;
	Pfloat *in_thvad;
	message_t msg_in_pvad;
	message_t msg_in_thvad;

	short out_vvad;

	// retrieve references to the channels used by the task for communication
	// input channel references
	FIFO_BUFFER_MSG_REF(pin_pvad,EDGE_pvad_0);
	FIFO_BUFFER_MSG_REF(pin_thvad,EDGE_thvad);	
	// output channel references
	FIFO_BUFFER_SHORT_REF(pout_vvad,EDGE_vvad);

)

//--------------
// Task code
//--------------

	// input
	pin_pvad->read(msg_in_pvad);
	in_pvad = (Pfloat *)(void *)msg_in_pvad;
	
	pin_thvad->read(msg_in_thvad);
	in_thvad = (Pfloat *)(void *)msg_in_thvad;

	// computation
	
	PRINT_TASK(TASK_NAME,"recv");
/*
cout << "vad_dec: in_pavd = (" << in_pvad->m << "," << in_pvad->e << ")" << endl;
cout << "vad_dec: in_thvad = (" << in_thvad->m << "," << in_thvad->e << ")" << endl;
*/
	out_vvad = vad_decision(
		*in_pvad,  // filtered signal energy (mantissa+exponent)
		*in_thvad  // decision threshold (mantissa+exponent)
	);
/*
cout << "vad_dec: out_vvad = (" << out_vvad << ")" << endl;
*/
	//CONSUME_T;

	PRINT_TASK(TASK_NAME,"send");

	// output
	
	pout_vvad->write(out_vvad);

END_TASK_WITH_TOKEN_INOUT(VADdecision_fun)
