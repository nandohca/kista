/*****************************************************************************

  ToneDetection_fun.cpp

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
#define TASK_NAME TASK_ToneDetec

extern "C"
BEGIN_TASK_WITH_TOKEN_INOUT(ToneDetection_fun,_T1_IN,_T1_OUT,
// --------------------------
// init and state section
// --------------------------
    // Declaration and initialization of internal variables
	short in_rc[4];
	rc_t *in_rc_comp;
	message_t msg_in_rc;
	
	short out_tone;
	unsigned int i;

	// retrieve references to the channels used by the task for communication
	
	// input channel references
	FIFO_BUFFER_MSG_REF(pin_rc,EDGE_rc_1);
	
	// output channel references
	FIFO_BUFFER_SHORT_REF(pout_tone,EDGE_tone);

)

//--------------
// Task code
//--------------

	// input
	pin_rc->read(msg_in_rc);
	in_rc_comp = (rc_t *)(void *)msg_in_rc;
	for (i=0; i<4; i++) in_rc[i] = in_rc_comp->item[i];

	// computation

	PRINT_TASK(TASK_NAME,"recv");

	tone_detection(in_rc, &out_tone);

	//CONSUME_T;

	PRINT_TASK(TASK_NAME,"send");

	// output
	
	pout_tone->write(out_tone);
END_TASK_WITH_TOKEN_INOUT(ToneDetection_fun)
//END_TASK
