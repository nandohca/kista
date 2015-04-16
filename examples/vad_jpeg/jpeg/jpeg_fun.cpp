/*****************************************************************************

  jpeg_fun.cpp

  Task function for jpeg

  Task function contains the task functionality and also communication and
  synchronization calls (therefore, it is not "pure" functionality)

  Task function is a very generic mechanism for supporting any task
  "processing and communication structure".

  From the XML interface, specific templates of "processing and communication structure"
  can be used to automate the generation of this structure, so to hide this
  from the user.

  Author: F. Herrera
  Institution: KTH
  Deparment: ICT/ES
  Date: 2014 February

 *****************************************************************************/

#include "kista.hpp"

//#include "compacted_types.hpp"

//#include "adse_defines.h"
//#include "fun/vad.h"


extern "C"
BEGIN_TASK_WITH_INIT(jpeg_fun,
// --------------------------
// init and state section
// --------------------------
    // Declaration

	
	// retrieve references to the channels used by the task for communication
	
	// input channel references
//  	FIFO_BUFFER_MSG_REF(pin_,);
 
	// output channel references  	
//  	FIFO_BUFFER_MSG_REF(pout_,);

)

//--------------
// Task code
//--------------
	//input


	// computation
	
	printf("JPEG DECODER EXECUTING\n");

    
	CONSUME_T;

	// output
	//message_t msg_out_rav0(&out_rav,sizeof(out_rav));

END_TASK
