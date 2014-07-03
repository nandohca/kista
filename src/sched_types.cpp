/*****************************************************************************

  sched_types.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 May

 *****************************************************************************/

#ifndef SCHED_TYPES_CPP
#define SCHED_TYPES_CPP

#include "sched_types.hpp"
#include "global_elements.hpp"

namespace kista {

std::string scheduler_t::to_string() {
	std::string tmp;
	tmp = "(";
	if((scheduler_policy+1) > scheduler_policy_name.size()) {
		tmp += "user";
	} else {
		tmp += scheduler_policy_name[scheduler_policy];
	}
	if(scheduler_policy==STATIC_PRIORITIES) {
		tmp += ",";
		if((static_priorities_policy+1) > static_priorities_policy_name.size()) {
			tmp += "user";
		} else {
			tmp += static_priorities_policy_name[static_priorities_policy];
		}		
	}
	if(scheduler_policy==DYNAMIC_PRIORITIES) {
		tmp += ",";
		if((dynamic_priorities_policy+1) > dynamic_priorities_policy_name.size()) {
			tmp += "user";
		} else {
			tmp += dynamic_priorities_policy_name[dynamic_priorities_policy];
		}			
	}
	tmp += ",";
	if((scheduler_triggering_policy+1) > scheduler_triggering_policy_name.size()) {
		tmp += "user";
	} else {
		tmp += scheduler_triggering_policy_name[scheduler_triggering_policy];
	}	
	tmp += ")";
	
	return tmp;
}

bool operator==(const scheduler_t &lhs, const scheduler_t &rhs) {
	if( (lhs.scheduler_policy == rhs.scheduler_policy) &&
		(lhs.static_priorities_policy == rhs.static_priorities_policy) &&
		(lhs.dynamic_priorities_policy == rhs.dynamic_priorities_policy) &&
	    (lhs.scheduler_triggering_policy == rhs.scheduler_triggering_policy)
    ) {	
		return true;
	} else {
		return false;
	}
}

bool operator<(const scheduler_t &lhs, const scheduler_t &rhs) {
	if(lhs.scheduler_triggering_policy < rhs.scheduler_triggering_policy) {
		return true;
	} else if(lhs.scheduler_policy < rhs.scheduler_policy) {
		return true;
	} else {
		switch(lhs.scheduler_policy) {
			case STATIC_PRIORITIES:
				if(lhs.static_priorities_policy < rhs.static_priorities_policy) {
					return true;
				} else {
					return false;
				}
				break;
			case DYNAMIC_PRIORITIES:
				if(lhs.dynamic_priorities_policy < rhs.dynamic_priorities_policy) {
					return true;
				} else {
					return false;
				}
				break;
			default:
				return false;
					
		}
	}
}

unsigned int add_scheduler_policy(std::string sched_policy_name) {
		scheduler_policy_name.push_back(sched_policy_name);
		return (scheduler_policy_name.size()-1);
}

unsigned int add_static_scheduler_policy(std::string static_prio_policy_name) {
		static_priorities_policy_name.push_back(static_prio_policy_name);
		return (static_priorities_policy_name.size()-1);
}

unsigned int add_dynamic_scheduler_policy(std::string dynamic_prio_policy_name) {
		dynamic_priorities_policy_name.push_back(dynamic_prio_policy_name);
		return (dynamic_priorities_policy_name.size()-1);
}

} // end namespace kista

#endif

