/*****************************************************************************

  VADhangover_fun.cpp

  Task function for VADhangover

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
#define TASK_NAME TASK_VADhang

extern "C"
BEGIN_TASK_WITH_TOKEN_INOUT(VADhangover_fun,_T8_IN,_T8_OUT,
// --------------------------
// init and state section
// --------------------------
    // Declaration and initialization of internal variables
    short in_vvad;
    short out_vad;

	// retrieve references to the channels used by the task for communication
	// input channel references
	FIFO_BUFFER_SHORT_REF(pin_vvad,EDGE_vvad);
	// output channel references
	FIFO_BUFFER_SHORT_REF(pout_vad,EDGE_vad);

)

//--------------
// Task code
//--------------

	// input

	pin_vvad->read(in_vvad);

	// computation

	PRINT_TASK(TASK_NAME,"recv");

	out_vad = vad_hangover(
		in_vvad  // vad decision before hangover is added
	);

//	CONSUME_T;

	PRINT_TASK(TASK_NAME,"send");

	// output
	
	pout_vad->write(out_vad);

END_TASK_WITH_TOKEN_INOUT(VADhangover_fun)
