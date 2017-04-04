/*****************************************************************************

  auxiliar.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February

  Notes: Auxiliar functions. These mostly serve to avoid dependencies of the
         KisTA library and thus easy installation, utilization and enhance
         its standalone  character
          
         // gcd2 function based on the gcd example in http://rosettacode.org/wiki/Least_common_multiple#C.2B.2B
         
 *****************************************************************************/

#ifndef AUXILIAR_HPP
#define AUXILIAR_HPP

#include <vector>
#include "systemc.h"
#include "taskset_by_name_t.hpp"

namespace kista {
	
// greatest common division of two natural numbers
template<class T> T gcd2(T m, T n);

// least common multiple of two natural numbers
template<class T> T lcm2(T m, T n);

//
// least common multiple of a list (actually a vector in C++ data structure terms) of numbers
//
// (note: this could easily present some overflow due to the chain of multiplications)
// A check can be implemented here or directly in the lcm2 function. Likely SystemC data types'
// can be used for it)
// 
template<class T> T lcm(std::vector<T> &taskset_periods);


// --------------------------------------------------------
// TEMPLATE IMPLEMENTATION
// --------------------------------------------------------
// greatest common division of two natural numbers
template<class T>
T gcd2(T m, T n) {
  T tmp;
  while(m) {
    tmp = m;
    m = n % m;
    n = tmp;
  }
  return n;
}

// least common multiple of two natural numbers
template<class T>
T lcm2(T m, T n) {
  // to avoid saturation effect, we check which is the biggest number, 
  // and perform the division first
  unsigned long long int tmp;
  if (m>n) {
	tmp = m / gcd2<T>(m, n);
	return (tmp * n);
  } else {
	tmp = n / gcd2<T>(m, n);
    return (tmp * m);
  }
}

//
// least common multiple of a list (actually a vector in C++ data structure terms) of numbers
//
// (note: this could easily present some overflow due to the chain of multiplications)
// A check can be implemented here or directly in the lcm2 function. Likely SystemC data types'
// can be used for it)
// 
template<class T>
T lcm(std::vector<T> &taskset_periods) {
  unsigned int i;
  T result;
  
  if(taskset_periods.size()==0) {
	SC_REPORT_ERROR("KisTA","Trying to obtain the least common multiple of an empty list of periods.");
  } else if(taskset_periods.size()==1) {
	SC_REPORT_WARNING("KisTA","Trying to obtain the least common multiple of a list of only one period. Such an unique value will be returned");
	return taskset_periods[0];
  } else {
	result = lcm2<T>(taskset_periods[0],taskset_periods[1]);  
    for(i=2; i<taskset_periods.size();i++) {
	  result = lcm2<T>(result,taskset_periods[i]);  
    }
    return result;
  }
}

} // end namespace kista

#endif
