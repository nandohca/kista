/*****************************************************************************

  ThresholdAdaptation_fun.cpp

  Task function for ThresholdAdaptation

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

extern "C"
void init_rvad_fun() {
	
	rvad_t out_rvad;
		
	// retrieve reference to the channel used by the task for communication
	// and where the initial tokens need to be injected
	FIFO_BUFFER_MSG_REF(pout_rvad,EDGE_rvad);
	
	// cout << "Injecting Initial token in rvad channel (rvad coeffs, and scal_rvad)" << endl;
	
    out_rvad.buff[0] = 0x6000;
	for(unsigned int i=1;i<9;i++) out_rvad.buff[i]=0;
	out_rvad.scal= 7;

	message_t msg_out_rvad(&out_rvad,sizeof(out_rvad));
		
	// and send them as initial tokens
	pout_rvad->initial_write(msg_out_rvad);
}


#define TASK_NAME TASK_ThreshAdapt
extern "C"
BEGIN_TASK_WITH_INIT(ThresholdAdaptation_fun,
// --------------------------
// init and state section
// --------------------------
	// Declaration
	// inputs
	short in_stat;
	short in_ptch;
	short in_tone;
	rav1_t *in_rav;
	pvad_acf0_t *in_pvad;
	message_t msg_in_rav;
	message_t msg_pvad_acf0_t;

    // outputs
	rvad_t out_rvad;
	Pfloat out_thvad;
	
	// retrieve references to the channels used by the task for communication
	
	// input channel references

	FIFO_BUFFER_SHORT_REF(pin_stat,EDGE_stat);
	FIFO_BUFFER_SHORT_REF(pin_ptch,EDGE_pitch);
	FIFO_BUFFER_SHORT_REF(pin_tone,EDGE_tone);
	FIFO_BUFFER_MSG_REF(pin_rav,EDGE_rav1_1);
	FIFO_BUFFER_MSG_REF(pin_pvad,EDGE_pvad_1);
	
	// output channel references
	FIFO_BUFFER_MSG_REF(pout_rvad,EDGE_rvad);
	FIFO_BUFFER_MSG_REF(pout_thvad,EDGE_thvad);
	
	// Set Initial state	
	
	/* Initialize rvad variables */    
    //out_rvad.buff[0] = 0x6000;
	//for(unsigned int i=1;i<9;i++) out_rvad.buff[i]=0;
	// out_rvad.scal= 7;

	// and send them as initial tokens
	// pout_rvad->write((void *)&out_rvad);
	//
	// It cannot be done here, because this initialization is
	// done the first time the task is executed and not
	// at the beginning of the simulation
	// Actually, to enable that any scheduling and mapping of the
	// SDF graph works, it is required that initial tokens are
	// injected before the simulation start
	// so it is actually delegateed to the init function associated
	// to the communication channel implementing the edge
	// (in this example, init_rvad_fun
	
	// Moreover, considering the write means to consider the 
	// the communication delay associated to such a first transfer,
	// because of that, initial_write has been provided in KisTA
	// (see init_rvad_fun as an example)
	
	//...
	// but done for avoiding a feedback of the value of rvad from
	// Energy Computation functionality		
    out_rvad.buff[0] = 0x6000;
	for(unsigned int i=1;i<9;i++) out_rvad.buff[i]=0;
	out_rvad.scal= 7;
	
	// and of thvad from VAD_dec ...
	   /* Initialize threshold level */
    out_thvad.e = 20;               /*** exponent ***/
    out_thvad.m = 27083;            /*** mantissa ***/
	
	// since these values were actually provided by the Threshold adaptation func.
	

)

//--------------
// Task code
//--------------

	//input 

	// directly from the input
	pin_ptch->read(in_ptch);

	// from other system tasks
	pin_stat->read(in_stat);
	pin_tone->read(in_tone);
	
	pin_rav->read(msg_in_rav);
	in_rav = (rav1_t *)(void *)msg_in_rav;
	
	pin_pvad->read(msg_pvad_acf0_t);
	in_pvad = (pvad_acf0_t *)(void *)msg_pvad_acf0_t;

	// computation
	
	PRINT_TASK(TASK_NAME,"recv");
/*
cout << "Threshold adaptation" << endl;
      cout << "stat =" << in_stat << endl;
      cout << "ptch =" << in_ptch << endl;
      cout << "tone =" << in_tone << endl;
      cout << "rav1 = {";
	for(unsigned int i=0;i<9; i++) cout << in_rav->buff[i] << ",";
      cout << "}" << endl;    
      cout << "scal_rav1 =" << in_rav->scal << endl;
      cout << "pvad = (" << in_pvad->pvad.m << "," << in_pvad->pvad.e << ")" << endl;
      cout << "acf0 =" << in_pvad->acf0.m << "," << in_pvad->acf0.e << ")"  << endl;
 
       cout << "rav1 = {";
	for(unsigned int i=0;i<9; i++) cout << in_rav->buff[i] << ",";
      cout << "}" << endl;    
      cout << "scal_rav1 =" << in_rav->scal << endl;
  
       cout << "ravd (1) = {";
	for(unsigned int i=0;i<9; i++) cout << out_rvad.buff[i] << ",";
      cout << "}" << endl;    
      cout << "scal_ravd (1) =" << out_rvad.scal << endl;
 	  cout << "out_thvad (1) = (" << out_thvad.m << "," << out_thvad.e << ")" << endl;
*/	                
	threshold_adaptation(
		in_stat,         // flag to indicate spectral stationarity
		in_ptch,         // flag to indicate a periodic signal component
		in_tone,         // flag to indicate a tone signal component
		in_rav->buff,    // ACF obtained from L_av1
		in_rav->scal,    // rav1[] scaling factor
		in_pvad->pvad,   // filtered signal energy (mantissa+exponent)
		in_pvad->acf0,   // signal frame energy (mantissa+exponent)
		out_rvad.buff,   // autocorrelated adaptive filter coefficients
		&out_rvad.scal,  // rvad[] scaling factor
		&out_thvad // decision threshold (mantissa+exponent)
	);

/*
      cout << "ravd (2) = {";
	for(unsigned int i=0;i<9; i++) cout << out_rvad.buff[i] << ",";
      cout << "}" << endl;    
      cout << "scal_ravd (2) =" << out_rvad.scal << endl;
	  cout << "out_thvad (2) = (" << out_thvad.m << "," << out_thvad.e << ")" << endl;
*/
	CONSUME_T;

	PRINT_TASK(TASK_NAME,"send");

	//output
	message_t msg_out_rvad(&out_rvad,sizeof(out_rvad));
	pout_rvad->write(msg_out_rvad);
	
	message_t msg_out_thvad(&out_thvad,sizeof(out_thvad));
	pout_thvad->write(msg_out_thvad);

END_TASK
