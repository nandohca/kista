/*****************************************************************************

  SpectralComparison_fun.cpp

  Task function for SpectralComparison

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

#define TASK_NAME TASK_SpectrComp

extern "C"
BEGIN_TASK_WITH_INIT(SpectralComparison_fun,
// --------------------------
// init and state section
// --------------------------
	// Declaration and initialization of internal variables
	int in_L_av[9];
	L_av_t *in_L_av_comp;
	message_t msg_in_L_av;
	
	rav1_t *in_rav;
	message_t msg_in_rav;

	short out_stat;
	
	unsigned int i;

	// retrieve references to the channels used by the task for communication
	
	// input channel references
  	FIFO_BUFFER_MSG_REF(pin_L_av,EDGE_L_av0);
  	FIFO_BUFFER_MSG_REF(pin_rav,EDGE_rav1_0);

	// output channel references
  	FIFO_BUFFER_SHORT_REF(pout_stat,EDGE_stat);

)

//--------------
// Task code
//--------------

	// input
	pin_L_av->read(msg_in_L_av);
	in_L_av_comp = (L_av_t *)(void *)msg_in_L_av;
	for (i=0; i<9; i++) {
		in_L_av[i] = in_L_av_comp->item[i];
	}

	pin_rav->read(msg_in_rav);		
	in_rav = (rav1_t *)(void *)msg_in_rav;

	// computation
	
	PRINT_TASK(TASK_NAME,"send");

	// Returns flag to indicate spectral stationarity
	out_stat = spectral_comparison(
		in_rav->buff, // ACF obtained from L_av1
		in_rav->scal, // rav1[] scaling factor
		in_L_av);     // ACF averaged over last four frames

	CONSUME_T;

	PRINT_TASK(TASK_NAME,"send");

	//output
	
	pout_stat->write(out_stat);

END_TASK
