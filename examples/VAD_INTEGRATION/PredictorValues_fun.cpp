/*****************************************************************************

  PredictorValues_fun.cpp

  Task function for PredictorValues

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
#define TASK_NAME TASK_PredictVal

extern "C"
BEGIN_TASK_WITH_TOKEN_INOUT(PredictorValues_fun,_T4_IN,_T4_OUT,
// --------------------------
// init and state section
// --------------------------
    // Declaration
	int in_L_av[9];
	L_av_t *in_L_av_comp;
	message_t msg_in_L_av_comp;

	rav1_t out_rav;

	unsigned int i;
	
	// retrieve references to the channels used by the task for communication
	
	// input channel references
  	FIFO_BUFFER_MSG_REF(pin_L_av,EDGE_L_av1);
 
	// output channel references  	
  	FIFO_BUFFER_MSG_REF(pout_rav0,EDGE_rav1_0);
  	FIFO_BUFFER_MSG_REF(pout_rav1,EDGE_rav1_1);

)

//--------------
// Task code
//--------------

	//input
	pin_L_av->read(msg_in_L_av_comp);
	
	in_L_av_comp = (L_av_t *)(void *)msg_in_L_av_comp;
	for (i=0; i<9; i++) {
		in_L_av[i] = in_L_av_comp->item[i];
	}

	// computation
	
	PRINT_TASK(TASK_NAME,"send");

/*
	cout << "L_av1 = {";
    for(unsigned int i=0;i<9;i++) cout << in_L_av[i] << ".";
    cout << "}" << endl;
*/
      
	predictor_values(
		in_L_av,      // ACF averaged over previous four frames
		out_rav.buff, // ACF obtained from in_buff
		&out_rav.scal // out scaling factor
	);

/*
    cout << "rav1 = {";
    for(unsigned int i=0;i<9; i++) cout << out_rav.buff[i] << ",";
    cout << "}" << endl;
    cout << "scal_rav1 =" << out_rav.scal << endl;
*/
    
	CONSUME_T;

	PRINT_TASK(TASK_NAME,"send");

	// output
	message_t msg_out_rav0(&out_rav,sizeof(out_rav));
	pout_rav0->write(msg_out_rav0);
	
	message_t msg_out_rav1(&out_rav,sizeof(out_rav));
	pout_rav1->write(msg_out_rav1);

END_TASK_WITH_TOKEN_INOUT(PredictorValues_fun)
