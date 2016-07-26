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

#include "compacted_types.hpp"

#include "adse_defines.h"
#include "fun/vad.h"
#include "prova_header.hpp"
#define TASK_NAME TASK_EnergComp
	
extern "C"
BEGIN_TASK_WITH_TOKEN_INOUT(EnergyComputation_fun,_T2_IN,_T2_OUT,
// --------------------------
// init and state section
// --------------------------
	// Declarations
	// input
	r_t	*in_r_h_comp;
	short in_r_h[9];
	message_t msg_in_r_h;
	
	short in_scal_acf;
	rvad_t *in_rvad;
	message_t msg_in_rvad;
	
	// output types
	Pfloat val_acf0;
	Pfloat val_pvad;
	pvad_acf0_t out_pvad_acf0;
	
	unsigned int i;
	
	// retrieve references to the channels used by the task for communication
	
	// input channel references
  	FIFO_BUFFER_MSG_REF(pin_r_h,EDGE_r_h_2);
	FIFO_BUFFER_SHORT_REF(pin_scal_acf,EDGE_scal_acf_1);
	FIFO_BUFFER_MSG_REF(pin_rvad,EDGE_rvad);

	// output channel references
	FIFO_BUFFER_MSG_REF(pout_pvad,EDGE_pvad_0);
	FIFO_BUFFER_MSG_REF(pout_pvad_acf0,EDGE_pvad_1);
)

//--------------
// Task code
//--------------

	// input
	// read in_r_h
	pin_r_h->read(msg_in_r_h);
	in_r_h_comp = (r_t *) (void *) msg_in_r_h;	
	for (i=0; i<9; i++) 
		in_r_h[i] = in_r_h_comp->item[i];
    // read scal_acf
	pin_scal_acf->read(in_scal_acf);	
	// read rvad
	pin_rvad->read(msg_in_rvad);
	in_rvad = (rvad_t *)(void *)msg_in_rvad;

	// computation
	
	PRINT_TASK(TASK_NAME,"recv");
/*
	  cout << "EngC: r_h_1 = {";
      for(unsigned int i=0;i<9;i++) cout << in_r_h[i] << ",";
      cout << "}" << endl;
      cout << "EngC: scal_acf_var = " << in_scal_acf << endl;
	  cout << "EngC: rvad = {";
      for(unsigned int i=0;i<9;i++) cout << in_rvad->buff[i] << ",";
      cout << "}" << endl;      
      cout << "EngC: scal_rvad = " << in_rvad->scal << endl;
 */     
	energy_computation(
		in_r_h,        // autocorrelation of input signal frame (msb)
		in_scal_acf,   // scaling factor for the autocorrelations		
		in_rvad->buff, // autocorrelated adaptive filter coefficients
		in_rvad->scal, // scaling factor for rvad[]
		&val_acf0,     // signal frame energy (mantissa+exponent)
		&val_pvad      // filtered signal energy (mantissa+exponent)
	);
/*
	cout << "EngC: out_pavd = (" << val_pvad.m << "," << val_pvad.e << ")" << endl;
	cout << "EngC: out_acf0 = (" << val_acf0.m << "," << val_acf0.e << ")" << endl;
	cout << endl;
	*/
	//CONSUME_T;

	PRINT_TASK(TASK_NAME,"send");
	
	//output
	message_t msg_out_pvad(&val_pvad, sizeof(val_pvad));
	pout_pvad->write(msg_out_pvad);
	
	out_pvad_acf0.acf0 = val_acf0;
	out_pvad_acf0.pvad = val_pvad;
	
	message_t msg_out_pvad_acf0(&out_pvad_acf0, sizeof(out_pvad_acf0));
	pout_pvad_acf0->write(msg_out_pvad_acf0);
	
END_TASK_WITH_TOKEN_INOUT(EnergyComputation_fun)
