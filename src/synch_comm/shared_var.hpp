/*****************************************************************************

  shared_var.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: UC
  Deparment:   TEISA
  Date: 2015 July


// ME SURGE LA DUDA DE SI HAY QUE SINCRONIZAR EN LOS ACCESOS/COMPLETION
// DE LOS SHARED VAR (QUIZA UNA IPC SHARED_MEM SÍ LO IMPLICA)
// DE MOMENTO, LA IMPLEMENTACIÓN NO INCLUYE CÓDIGO DE SINCRONIZACIÓN

 *****************************************************************************/

#ifndef _SHARED_VARIABLE_HPP
#define _SHARED_VARIABLE_HPP

#include "systemc.h"

#include "defaults.hpp"

namespace kista {
	
template<class T>
class shared_var {
	// NOTICE THAT shared var does not inherit logic_link_info_T
	//     because a shared var can have associate several tasks, and thus logic links!!
	
	public:
	  shared_var(); // constructor without initialization
	  shared_var(T &val_par); // constructor with initialization (requires the "=" operator)
	  
	  void write(const T &par);
	  
	  virtual void read(T&);
	  virtual T read();
	  
	  // method to map to a memory resource
	  void map_to(memory_resource *mem);
	  // method to map to a memory resource and indicate a specific access bus
	  // void map_to(memory_resource *mem, phy_comm_res_t *phy_comm_res_par);
	  
	private:
	  T	val;
	  	  
	  //Two possible implementations of the shared var can be considered:
	  
	  // A) a scheme considering the implementation of a shared variable on top of
	  // a memory, in turn linked to a bus ... 
	  phy_link_t    	*assoc_plink; 	// associated physical link (PE-mem link, via the comm resource)
	    // Currently the KisTA shared var can have associated SEVERAL physical links, one per 
	    // associated PE (master) -MEM link
	    
	  phy_comm_res_t    *assoc_comm_res_p; // associated physical communication resource
										// (currently it should be a shared bus)
										// (currently a single grade path is supported!! PE-commresource (shared bus)-memory)
											
	  memory_resource	*assoc_memres_p; // associated hw resource
											// (memory resource)
	    
      // B) A dual port ram connecting two processing elements
 //     phy_link_t    	assoc_plink;
 
	unsigned int message_size;
	
	void before_end_of_elaboration(); // for the sketch report
	void end_of_elaboration();
	void auto_map() ; // it creates a new logic link in the logical network associated to the fifo channel
					// and then automatically maps the system-level channel to a corresponding and unique logical link
					// taking into account the task to PE mapping
					// It is called from end_of elaboration, in order to enable the completion of the mapping
					// phase ONLY if there is no mapping of the system-level link done.
					
	// system-level measurements
	throughput_monitor *read_th_mon;
	throughput_monitor *write_th_mon;
	
	comm_synch_role_t role;
	
	io_comm_synch_sense_t io_sense; // Only relevant when the channel has  I/O role
 
	void report_channel_event(std::string event_description);
 
};	


template <class T>
shared_var<T>::shared_var()
{
	
	assoc_comm_res_p = NULL;
	message_size = 0;
	write_th_mon = NULL;
    read_th_mon = NULL;
    role = SYSTEM_COMMSYNCH;
    io_sense = UNSET_IO_SENSE;
	
}


template <class T>
shared_var<T>::shared_var(T &val_par)
{
	assoc_comm_res_p = NULL;
	message_size = 0;
	write_th_mon = NULL;
    read_th_mon = NULL;
    role = SYSTEM_COMMSYNCH;
    io_sense = UNSET_IO_SENSE;
    
    //
	val = val_par;
}

template <class T>
void shared_var<T>::end_of_elaboration()
{
	std::string msg;
	// Performs a default mapping in case that
	// - no mapping has been done
	// - the memory resource list is not empty


	std::string rpt_msg;
	
	// Calculate (if required) message size associated to tokens...
	rpt_msg = "Shared Var. Channel ";
	rpt_msg += this->name();
	rpt_msg += ": message size = ";
	if(message_size==0) { // unset, then default message size used
		message_size = sizeof(T)*8;
		rpt_msg += std::to_string(message_size);
		rpt_msg += " bits (inferred from data type)";		
	} else {
		rpt_msg += std::to_string(message_size);
		rpt_msg += " bits (explicitly stated).";
	}
	
	if(write_th_mon!=NULL) {
		write_th_mon->set_data_size(message_size);
	}

	if(read_th_mon!=NULL) {
		read_th_mon->set_data_size(message_size);
	}

	// if there are any throughput monitor, the size is associated
/*	if(read_th_mon!=NULL) {
		read_th_mon->set_data_size(message_size);
	}
	if(write_th_mon!=NULL) {
		write_th_mon->set_data_size(message_size);
	}
*/

#ifdef _REPORT_SHARED_VAR_TOKEN_SIZE_DETERMINATION
	// ... and report it
	SC_REPORT_INFO("KisTA",rpt_msg.c_str());
#endif

#ifdef TRY_DEFAULT_MAPPING_FOR_SHARED_VAR	
	if(assoc_memres_p == NULL) {
		msg = "Shared variable channel ";
		msg += this->name();
		msg += "not associated to any memory hardware resource. Trying automatic mapping...\n";
		// the shared var has not been assigned to a memory resource
		// The allocation to a hw memory resource is tried
		if(mems.size()>0) {
			assoc_memres_p = mems[0];
			msg += this->name();
			msg += " shared var mapped to ";
			msg += assoc_memres_p->name();
			msg += " hw resource";
			if(mems.size()==1) {
				msg += " (the only one)\n";
			} else {
				msg += " (first in the list taken)\n";
			} 
		} else {		
			msg += "No memory resources in the model. Ideal shared var modelled.\n";			
		}
		SC_REPORT_WARNING("KisTA",msg.c_str());
	}
#endif	
}


// Currently, a single association is possible!!
template <class T>
void shared_var<T>::map_to(memory_resource *mem) {
	
	std::string rpt_msg;
		
	if ( sc_get_status()&(SC_ELABORATION|SC_BEFORE_END_OF_ELABORATION) ) {	
		if(	(assoc_memres_p != NULL) && (assoc_memres_p!=mem) ) {
			// a previously associated memory resource already found by the automatic map
			// different from the one that we want to state
			rpt_msg = "Explicit association (via -map_to-) of shared_var channel \"";
			rpt_msg += this->name();				
			rpt_msg += "\" to memory resource ";
			rpt_msg += mem->name();
			rpt_msg += " overrides a previous assignation to the communication resource \"";
			rpt_msg += assoc_memres_p->name();
			rpt_msg += "\"\n";			
			rpt_msg += " KisTA uses a single memory resource, regardless the actual modelled network";
			rpt_msg += " is composed of several subnetworks or communication resources.";
			SC_REPORT_WARNING("KisTA",rpt_msg.c_str());			
		} 
		// here the explicit association via -map to- is done
		assoc_memres_p = mem;
	} else {
		rpt_msg = "Explicit association (via -map_to-) of shared_var channel \"";
		rpt_msg += this->name();				
		rpt_msg += "\" to memory resource ";
		rpt_msg += mem->name();			
		rpt_msg += " can be done only at elaboration or before the end of elaboration.";
		SC_REPORT_ERROR("KisTA","map ");
	}

}

template <class T>
void shared_var<T>::write(const T &par) {
	
	std::string			rpt_msg;
	sc_process_handle 	current_task;
	task_info_t			*current_task_info;
	
	// src processing element and dest memory resource the transaction is associated to
	processing_element	*pe_src;
	memory_resource		*mem_dest;
	
	sc_time				comm_delay;
	
#ifdef _REPORT_SYSTEM_COMMUNICATION_WRITE_ACCESSES
	if(this->role==SYSTEM_COMMSYNCH ) {
		report_channel_event("shared var WRITE ACCESS");
	}
#endif

#ifdef _REPORT_IO_WRITE_ACCESS
	if( this->role==IO_COMMSYNCH ) {
		if(this->io_sense==INPUT ) {
			report_channel_event("shared var IO INPUT WRITE ACCESS");
		} else if (this->io_sense==OUTPUT ) {
			report_channel_event("shared var IO OUTPUT WRITE ACCESS");
		}		
	} // no report for environment channels (internal to the environment)		
#endif

	
	// collect write event if write throughput monitoring has been enabled
 	if(write_th_mon!=NULL) {
		write_th_mon->annotate_data_event();
	}
	
	if(this->role==SYSTEM_COMMSYNCH) {

		// is an internal system level channel, thus which can consume platform communication resources

#ifdef _CONSIDER_COMMUNICATION_DELAYS

#ifdef _REPORT_COMMUNICATION_DELAYS
		// now access the communication resource (which could require schedulable)
		rpt_msg = "Shared Var. Channel \" ";
		rpt_msg += this->name();
		rpt_msg += "\": WRITE access ";
		// by now uses the physical link
		rpt_msg += "(msg. size : ";
		rpt_msg += std::to_string(message_size);
		rpt_msg += " bits)\n";
#endif

		// Only if there is a resolved physical link associated to the system level channel a delay will be associated to it		
		if( assoc_plink->resolved() ) {
			if( is_PE_intracomm(*assoc_plink) ) {
				
				if(assoc_plink->src->get_type()!=PROCESSING_ELEMENT) {
					rpt_msg += ". PE intra-communication. Source PE not associated to a processing element";
					SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
				}					
				
				pe_src=static_cast<processing_element *>(assoc_plink->src);
				//comm_delay = assoc_plink->src->getIntraComMaxP2Pdelay(message_size);
				comm_delay = pe_src->getIntraComMaxP2Pdelay(message_size);
				
#ifdef _REPORT_COMMUNICATION_DELAYS
				rpt_msg += "\tinvolves intracomunication in PE ";
				//rpt_msg +=  assoc_plink.src->name();
				rpt_msg +=  pe_src->name();
				rpt_msg += ", associated delay: ";
				rpt_msg +=  comm_delay.to_string();
				rpt_msg += "\n";
#endif
				wait( comm_delay );
			} else if (is_PE_MEM_comm(*assoc_plink)) {

				if(assoc_plink->src->get_type()!=PROCESSING_ELEMENT) {
					rpt_msg += ". PE-MEM communication. Source PE not associated to a processing element";
					SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
				}					
				pe_src=static_cast<processing_element *>(assoc_plink->src);
				
				if(assoc_plink->src->get_type()!=MEMORY_RESOURCE) {
					rpt_msg += ". PE-MEM communication. Destination memory not associated to a memory resource element";
					SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
				}					
				mem_dest=static_cast<memory_resource *>(assoc_plink->dest);
								
#ifdef _REPORT_COMMUNICATION_DELAYS
				rpt_msg += "\tinvolves communication over the physical link (";
				//rpt_msg += assoc_plink.src->name();
				rpt_msg += pe_src->name();
				rpt_msg += ",";
				//rpt_msg +=  assoc_plink.dest->name();
				rpt_msg +=  mem_dest->name();
				rpt_msg += ")";
#endif	
   // TO DO : Delays on the shared var!!
	/*			
				if(assoc_plink.src->has_netif()) {
#ifdef _REPORT_COMMUNICATION_DELAYS
					rpt_msg += "Communication delegated to network interface.\n";
#endif

					// note: this declaration and at least three of the followin assignatiosn can be likely moved
					//       to improve efficiency
					send_message_request_t send_message_request;
					
					send_message_request.logic_link = this;
					send_message_request.comm_res = assoc_comm_res_p;
					send_message_request.phy_link = &assoc_plink;
					send_message_request.message_size = message_size;
					send_message_request.completed_transactions_p = completed_transactions;
					send_message_request.request_time = sc_time_stamp();

					// The source processing element (src physical address) has network interface
					// Then, the processing resources of the PE, and so the task can get released from the communication delay
					assoc_plink.src->get_netif()->request_sending(send_message_request);

					// now, for the reader side the delay is 0 (infinite buffer) or some delay depending the buffer capability
					// and the number of requests
					// for the reader side, the delay will depend on the number of requests and on the policy of the netif,
					// as well as the delays in the communication resource
					
				} else {
					// The source processing element (src physical address) has not network interface
					// then the processing element is assumed to be in charge of the transfer effort and
					// cannot ve released until sending all the data

					if(worst_case_communication_enabled) {
						comm_delay = assoc_comm_res_p->get_MaxP2Pdelay(assoc_plink,message_size);
					} else {
						comm_delay = assoc_comm_res_p->get_CurrentP2Pdelay(assoc_plink,message_size);
					}

#ifdef _REPORT_COMMUNICATION_DELAYS
					rpt_msg += "Communication performed by PE.\n";
					if(worst_case_communication_enabled) {
						rpt_msg += " with maximum associated delay: ";
						rpt_msg += comm_delay.to_string();
						rpt_msg += "\n";
					} else {
						rpt_msg += " with current associated delay: ";
						rpt_msg += comm_delay.to_string();
						rpt_msg += "\n";						
					}
#endif					
					wait( comm_delay );
					completed_transactions->post();
				}
*/ 
			}

		}		
		
#ifdef _REPORT_COMMUNICATION_DELAYS
		else {
			rpt_msg += "\tAssociated physical link not resolved. No delay will be associated to the write access.";
		}
		SC_REPORT_INFO("KisTA",rpt_msg.c_str());
#endif

#endif
	}
	
	// Note: For the moment, no communication delay associated to the OUTPUT I/O transfers

	val = par;
	

#ifdef _REPORT_SYSTEM_COMMUNICATION_WRITE_COMPLETION
	if (this->role==SYSTEM_COMMSYNCH) {
		report_channel_event("shared var WRITE COMPLETION");
	}
#endif	
#ifdef _REPORT_IO_WRITE_COMPLETION
	if( this->role==IO_COMMSYNCH ) {
		if(this->io_sense==INPUT ) {
			report_channel_event("shared var  IO INPUT WRITE COMPLETION");
		} else if (this->io_sense==OUTPUT ) {
			report_channel_event("shared var  IO OUTPUT WRITE COMPLETION");
		}
	}
#endif
	
}


template <class T>
inline void shared_var<T>::read(T& val_) {

	// src processing element and dest memory resource the transaction is associated to
	processing_element	*pe_src;
	memory_resource		*mem_dest;

	// TO DO: add associated read delays

	
#ifdef _REPORT_READ_IO_ACCESS
	std::string rpt_msg;
#endif	

	sc_process_handle 	current_task;
	task_info_t		*current_task_info;
					
	// collect write event if write throughput monitoring has been enabled

#ifdef _REPORT_SYSTEM_COMMUNICATION_READ_ACCESSES
	if(this->role==SYSTEM_COMMSYNCH ) {
		report_channel_event("Internal READ ACCESS");
	}
#endif

#ifdef _REPORT_IO_READ_ACCESS
	if( this->role==IO_COMMSYNCH ) {
		if(this->io_sense==INPUT ) {
			report_channel_event("IO INPUT READ ACCESS");
		} else if (this->io_sense==OUTPUT ) {
			report_channel_event("IO OUTPUT READ ACCESS");
		}
	} // no report for environment channels (internal to the environment)		
#endif

	// Value to be read (return)
	val_ = val;
	
	// read event is annotated immediately after the read is completed (call unblocked)
	if(read_th_mon!=NULL) {
		read_th_mon->annotate_data_event();
	}

#ifdef _REPORT_SYSTEM_COMMUNICATION_READ_COMPLETION
	if (this->role==SYSTEM_COMMSYNCH) {
		report_channel_event("READ COMPLETION");
	}
#endif	
#ifdef _REPORT_IO_READ_COMPLETION
	if( this->role==IO_COMMSYNCH ) {
		if(this->io_sense==INPUT ) {
			report_channel_event("IO INPUT READ COMPLETION");
		} else if (this->io_sense==OUTPUT ) {
			report_channel_event("IO OUTPUT READ COMPLETION");
		}
	}
#endif	
	
}

template <class T>
inline T shared_var<T>::read()
{
    T tmp;
    read( tmp );
    return tmp;
}


template <class T>
void shared_var<T>::report_channel_event(std::string event_description) {
	std::string rpt_msg;
/*
 * 	if (this->role==SYSTEM_COMMSYNCH) {
		// now access the communication resource (which could require schedulable)
		rpt_msg = "INTERNAL Channel: ";
	} else if (this->role==IO_COMMSYNCH) {
		rpt_msg = "I/O Channel: ";
	}
	*/
	rpt_msg = "Shared Var. Channel \"";
	rpt_msg += this->name();
	rpt_msg += "\": ";
	rpt_msg += event_description;
	rpt_msg += " at ";
	rpt_msg += sc_time_stamp().to_string();
	SC_REPORT_INFO("KisTA",rpt_msg.c_str());
}


// TODO: Timing annotations for av time and WCET!!!  

	
} // namespace KisTA

#endif
