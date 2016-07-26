/*****************************************************************************

  ACFAveraging_fun.cpp

  Task function for ACFAveraging

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
#define TASK_NAME TASK_ACFavg

extern "C"

BEGIN_TASK_WITH_TOKEN_INOUT(ACFAveraging_fun,_T3_IN,_T3_OUT,
// --------------------------
// init and state section
// --------------------------
	// Declaration and initialization of internal variables
 	short in_r_h[9];
	short in_r_l[9];
	
	message_t msg_in_r_h;
	message_t msg_in_r_l;
	
	r_t   *in_r_h_comp;
	r_t   *in_r_l_comp;
	
	short in_scal_acf;

	int out_L_av0[9];
	int out_L_av1[9];	
	L_av_t out_L_av0_comp;
	L_av_t out_L_av1_comp;

	// retrieve references to the channels used by the task for communication
	
	// input channel references
  	FIFO_BUFFER_MSG_REF(pin_r_h,EDGE_r_h_1);
  	FIFO_BUFFER_MSG_REF(pin_r_l,EDGE_r_l);
	FIFO_BUFFER_SHORT_REF(pin_scal_acf,EDGE_scal_acf_2);

	// output channel references
	FIFO_BUFFER_MSG_REF(pout_L_av0,EDGE_L_av0);
	FIFO_BUFFER_MSG_REF(pout_L_av1,EDGE_L_av1);
)

//--------------
// Task code
//--------------

	unsigned int i;

	// input
	
	// read r_h
	pin_r_h->read( msg_in_r_h );
	in_r_h_comp = (r_t *)(void *)msg_in_r_h;
	for (i=0; i<9; i++) {
		in_r_h[i] = in_r_h_comp->item[i];
	}

	// read r_l	
	pin_r_l->read( msg_in_r_l );
	in_r_l_comp = (r_t *)(void *)msg_in_r_l;
	for (i=0; i<9; i++) {
		in_r_l[i] = in_r_l_comp->item[i];
	}
	
	// read scal_acf
	pin_scal_acf->read(in_scal_acf);

	// computation
	
	PRINT_TASK(TASK_NAME,"recv");

	acf_averaging(
		in_r_h,      // autocorrelation of input signal frame (msb)
		in_r_l,      // autocorrelation of input signal frame (lsb)
		in_scal_acf, // scaling factor for the autocorrelations
		out_L_av0,   // ACF averaged over last four frames
		out_L_av1);  // ACF averaged over previous four frames
		// Note: there is an implicit conversion from Word32 array (acf_averaging prototype)
		//       the short array in this wrapper function, which enables the match to the
		//       short array declared at the corresponding output edges (E1 and E2)
		//       The conversion does not produces any change of the VAD functionality,
		//       since the outputs are flags, result of the voice detection (or not),
		//       and only the LSB is used actually.
		//       Thanks Hosein for the observation!
	//CONSUME_T;

	PRINT_TASK(TASK_NAME,"send");

	// output
	
	// write L_av0 and L_av1
	for (i=0; i<9; i++) {
		out_L_av0_comp.item[i] = out_L_av0[i];
	}
	for (i=0; i<9; i++) {
		out_L_av1_comp.item[i] = out_L_av1[i];
	}
	
	message_t msg_out_L_av0(&out_L_av0_comp,sizeof(out_L_av0_comp));
	message_t msg_out_L_av1(&out_L_av1_comp,sizeof(out_L_av1_comp));
	
	pout_L_av0->write(msg_out_L_av0);
	pout_L_av1->write(msg_out_L_av1);

/*
	for (i=0; i<9; i++)
		pout_L_av0->write(out_L_av0[i]);
	for (i=0; i<9; i++)
		pout_L_av1->write(out_L_av1[i]);
*/
END_TASK_WITH_TOKEN_INOUT(ACFAveraging_fun)
