/*****************************************************************************

  compacted_types.hpp
  
  File belonging to KisTA XML VAD example.

  Author: E. Paone
  Institution: Polimi
  Deparment: DEIB
  Date: 2014 Februrary
  
  Editions: F. Herrera
  Institution: Polimi
  Deparment: DEIB
  Date: 2014 February 
 
  Edtions for last KisTA version:
  Author: F. Herrera
  Institution: KTH
  Deparment: ICT/ES
  Date: 2014 February
 
  Notes: File name has been changed to compacted_types.hpp to:
  * 	To reflect that these types arise from considering the compactation transformation done
        by A-DSE tool (which merges "parallel" edges into a single one
  * 	Avoid clash chances with the types.hpp Kista header(although in principle should be no clash) 

// The "original model" suffers two types of transformations by A-DSE tool
//
// 1 - SDF -> to HSDF : means that a couple of writes/read have to become one write/read in the actor
// 2 - Compactation   : means that parallel edges (with the same source and destination actors, are compacted into a single edge

 *****************************************************************************/
#ifndef _COMPACTED_TYPES_H
#define _COMPACTED_TYPES_H

#include "fun/vad.h"

// The "original model" suffers two types of transformations by A-DSE tool
//
// 1 - SDF -> to HSDF : means that a couple of writes/read have to become one write/read in the actor
// 2 - Compactation   : means that parallel edges (with the same source and destination actors, are compacted into a single edge

struct my_type {
	unsigned int	a[8];
	char	b;
};

// Types because of SDF to HSDF
struct r_t {
	short item[9];
};

struct L_av_t {
	short item[9];
};

struct rc_t {
	short item[4];
};


// Types because of compactation 

struct rav1_t {
	short buff[9];
	short scal;
};

struct rvad_t {
	short buff[9];
	short scal;
};

struct pvad_acf0_t {
	Pfloat acf0;
	Pfloat pvad;
};

#endif
