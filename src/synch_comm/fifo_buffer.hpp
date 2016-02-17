/*****************************************************************************

  fifo_buffer.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 March

  Notes: 
          This channel behaves as a sc_fifo plus throughput monitor and logic link
          inheritance when it is configured as an IO channel.
          
          If it is configured as a system channel (default construction settings)
          then it serves to communicate scheduled tasks, and can consider the impact
          of the communication platform infrastructure.
          
          The fifo_buffer implementation has been changed from inheriting sc_fifo to
          wrap a sc_fifo because currently, sc_fifo does not inherits sc_prim_channel
          as virtual. Then, the double inheritance of sc_prim_channel produces a class.
          In order to avoid it, we 

 *****************************************************************************/

#ifndef _FIFO_BUFFER_HPP
#define _FIFO_BUFFER_HPP

#include "systemc.h"

#include "defaults.hpp"

#include "throughput_monitor.hpp"

#include "utils.hpp"

namespace kista {


// fifo buffer class

template<class T>
class fifo_buffer : public sc_fifo<T>,
					  public logic_link_info_t {
public:
	explicit fifo_buffer( int _size);
	fifo_buffer( const char* _name, int _size);
	fifo_buffer( const char* _name);    // using the same default for size as in SystemC
	fifo_buffer( const char* _name, int _size, task_info_t* src_par, task_info_t* dest_par);
    fifo_buffer( const char* _name, int _size, const char *src_name_par, const char *dest_name_par);
		
	// destructor
	virtual ~fifo_buffer()
	{}

	// interface methods
	
	// initialization hook for the channel
	void set_init(VOIDFPTR init_f_par); // set function which will be called for channel initialization
	bool has_init();				    // returns true if channel init function has been settled

	void initial_write( const T& ); // function to inject tokens at init time
	
//    virtual void register_port( sc_port_base&, const char* );

	// blocking read
	virtual void read( T& );
	virtual T read();

	// non-blocking read
	//    virtual bool nb_read( T& );


	// get the number of available samples

	virtual int num_available() const
	{ return ( this->sc_fifo<T>::num_available() ); }


	// get the data written event

	virtual const sc_event& data_written_event() const
	{ return this->sc_fifo<T>::data_written_event(); }


	// blocking write
	virtual void write( const T& );

		// non-blocking write
	//    virtual bool nb_write( const T& );


		// get the number of free spaces

	virtual int num_free() const
	{ return ( this->sc_fifo<T>::num_free() ); }


	// get the data read event

	virtual const sc_event& data_read_event() const
	{ return this->sc_fifo<T>::data_read_event(); }


	// other methods

	operator T ()
	{ return read(); }


	sc_fifo<T>& operator = ( const T& a )
		{ write( a ); return *this; }


	void trace( sc_trace_file* tf ) const {
		this->sc_fifo<T>::trace(tf);
	}


	virtual void print( ::std::ostream& os = ::std::cout ) const {
		this->sc_fifo<T>::print(os);
	}

	virtual void dump( ::std::ostream& os = ::std::cout ) const {
		this->sc_fifo<T>::dump(os);
	}

	virtual const char* kind() const
		{ return "fifo_buffer"; }
	
	// method declaring size of the token (overrides the default method, which automatically detects it form T class)
	void set_message_size(unsigned int size_bits_par);
	
	// method declaring binding of tasks (for creation and association of virtual links)
	//     (to be called at elaboration time)
	// void bind(VOIDVOIDFPTR src, VOIDVOIDFPTR src); // relies on that logic address is task info pointer // not supported yet
	
	// definition not required, already supported by the inheritance of logical link
	//void bind(task_info_t* src, task_info_t* dest); // relies on that logic address is task info pointer
	                                                 // (see link.hpp)
	
	void map_to(phy_comm_res_t *phy_comm_res_par); // nethod to map to a physical communication resource
												// (only required if map() has different alternatives
												//  so this method can be used to fix it, instead letting
												//  having an automatic resolution)
	
	// measurements
		// activation of measurements
	void	measure_write_throughput();	// activation of throughput measurement infrastructure
	void	measure_read_throughput();  // (required for obtaining any valid measurement)

	// throughput report (considering message sizes)
	double get_write_throughput(const char *unit = "b");		      // overal throughput, from the beginning of the simulation
	double get_write_stationary_throughput(const char *unit = "b");   // stationary throughput, meaningful if stationary state reached, otherwise, warning is launched
	double get_read_throughput(const char *unit  = "b");			  // overal throughput, from the beginning of the simulation
	double get_read_stationary_throughput(const char *unit  = "b");   // stationary throughput, meaningful if stationary state reached, otherwise, warning is launched

	// normalized throughput report
	double get_normalized_write_throughput(const char *unit = "H");		        // overal throughput, from the beginning of the simulation
	double get_normalized_write_stationary_throughput(const char *unit = "H");   // stationary throughput, meaningful if stationary state reached, otherwise, warning is launched
	double get_normalized_read_throughput(const char *unit  = "H");			     // overal throughput, from the beginning of the simulation
	double get_normalized_read_stationary_throughput(const char *unit  = "H");   // stationary throughput, meaningful if stationary state reached, otherwise, warning is launched

	// UNITS FOR:
	//            Throughput:   'b' (bps) (DEFAULT), 'B' (bytes/s), 'K' (Kbps) , 'M' (Mbps), 'G' (Gbps)
	// Normalized Throughput:   'H' (Hertz) (DEFAULT), ´K´ or 'k' (KiloHerzt), 'Ḿ´ (MegaHerzt), 'G´ (GigaHerzt), 'T´ (TeraHerzt),
	//												 ,  'm' (miliHerzt), 'u' (microHerzt), 'n' (nanoHerzt), 'p' (picoHertz)
	
	// reports the last_lag on the last consecutive transfer events, as long as they have been regular
	// such stationarity can be considered, and so it can be considered a period ...
	// for the read events
	sc_time get_read_period();
	double get_read_period(const char *unit);
	
	// and for the write events
	sc_time get_write_period();
	double get_write_period(const char *unit);
	
	// methods for setting and get channel role
		
	void set_role(comm_synch_role_t role);
	comm_synch_role_t &get_role();

	void set_as_ENV(); // set channel as an ENV channel
	bool is_ENV();     // return true if the channel has been settled as an ENV channel
	
	void set_as_IO();  // set channel as an I/O channel
	bool is_IO();      // return true if the channel has been settled as an IO channel
	
	void set_IO_sense(io_comm_synch_sense_t sense);
	io_comm_synch_sense_t &get_IO_sense();
	
												
private:
	phy_link_t    	  assoc_plink; 	// associated physical link
	phy_comm_res_t    *assoc_comm_res_p; // associated physical communication resource
	
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
	
	io_comm_synch_sense_t determine_io_sense();
	
	void report_channel_event(std::string event_description);

	// Semaphore for supporting a state variable, that is a memory, regarding the
	// number of transactions peformed in the link
	//  ... in principle it would be desirable to encrust it in the logic_link_t class
	// so far, it is directly integrated in the sc_buffers, since it might result
	// this feature is required for the modellign of the implementation of this type of channel
	sc_semaphore *completed_transactions;
	
	// initialization hook
	VOIDFPTR init_f;
				
};

// -----------------------------------------------------------------------
// IMPLEMENTATION: Not separated in another file, because it is a template
//                 and thus it does not admite separated compilation
// -----------------------------------------------------------------------

template <class T>
fifo_buffer<T>::fifo_buffer( int _size) : 
					sc_fifo<T>(_size),
					logic_link_info_t(sc_core::sc_gen_unique_name("fifo_buffer") )
{	
	assoc_comm_res_p = NULL;
	message_size = 0;
	write_th_mon = NULL;
    read_th_mon = NULL;
    role = SYSTEM_COMMSYNCH;
    io_sense = UNSET_IO_SENSE;
    //
    completed_transactions = new sc_semaphore(sc_gen_unique_name("fifo_buffer_transac_sem"),0); 
    //
    init_f=NULL;
}

template <class T>
fifo_buffer<T>::fifo_buffer( const char* _name, int _size) :
					sc_fifo<T>(_name, _size),
					logic_link_info_t(_name)
{
	assoc_comm_res_p = NULL;
	message_size = 0;
	write_th_mon = NULL;
	read_th_mon = NULL;
    role = SYSTEM_COMMSYNCH;
    io_sense = UNSET_IO_SENSE;    
    //
    completed_transactions = new sc_semaphore(sc_gen_unique_name("fifo_buffer_transac_sem"),0);  
    //
    init_f=NULL;
}

template <class T>
fifo_buffer<T>::fifo_buffer( const char* _name) : 
					sc_fifo<T>(_name),
					logic_link_info_t(_name)
{
	assoc_comm_res_p = NULL;
	message_size = 0;
	write_th_mon = NULL;
	read_th_mon = NULL;
	role = SYSTEM_COMMSYNCH;
    io_sense = UNSET_IO_SENSE;	
    //
    completed_transactions = new sc_semaphore(sc_gen_unique_name("fifo_buffer_transac_sem"),0);  	
    //
    init_f=NULL;
}

template <class T>
fifo_buffer<T>::fifo_buffer( const char* _name, int _size, task_info_t* src_par, task_info_t* dest_par) :
					sc_fifo<T>(_name, _size),
					logic_link_info_t(_name, src_par,dest_par)
{
	assoc_comm_res_p = NULL;
	message_size = 0;
	write_th_mon = NULL;
	read_th_mon = NULL;
    role = SYSTEM_COMMSYNCH;
    io_sense = UNSET_IO_SENSE;    
    //
    completed_transactions = new sc_semaphore(sc_gen_unique_name("fifo_buffer_transac_sem"),0);  
    //
    init_f=NULL;   
}

template <class T>
fifo_buffer<T>::fifo_buffer( const char* _name, int _size, const char *src_name_par, const char *dest_name_par) :
						sc_fifo<T>(_name, _size),
						logic_link_info_t(_name, src_name_par,dest_name_par)
{
	assoc_comm_res_p = NULL;
	message_size = 0;
	write_th_mon = NULL;
	read_th_mon = NULL;	
    role = SYSTEM_COMMSYNCH;
    io_sense = UNSET_IO_SENSE;    
    //
    completed_transactions = new sc_semaphore(sc_gen_unique_name("fifo_buffer_transac_sem"),0);  
    //
    init_f=NULL;
}


template <class T>
void fifo_buffer<T>::set_role(comm_synch_role_t role_par) 
{
	role = role_par;
}

template <class T>
comm_synch_role_t &fifo_buffer<T>::get_role() 
{
	return role;
}

template <class T>
void fifo_buffer<T>::set_as_ENV() 
{
	role = ENV_COMMSYNCH;
}

template <class T>
bool fifo_buffer<T>::is_ENV() 
{
	return (role==ENV_COMMSYNCH);
}

template <class T>
void fifo_buffer<T>::set_as_IO() 
{
	role = IO_COMMSYNCH;
}

template <class T>
bool fifo_buffer<T>::is_IO() 
{
	return (role==IO_COMMSYNCH);
}

template <class T>
void fifo_buffer<T>::set_IO_sense(io_comm_synch_sense_t sense) {
	io_sense = sense;
}

template <class T>
io_comm_synch_sense_t& fifo_buffer<T>::get_IO_sense() {
	return io_sense;
}

// blocking read

template <class T>
inline
void
fifo_buffer<T>::read( T& val_ )
{

#ifdef _REPORT_READ_IO_ACCESS
	std::string rpt_msg;
#endif	

	sc_process_handle 	current_task;
	task_info_t		*current_task_info;
					
	// collect write event if write throughput monitoring has been enabled

#ifdef _REPORT_SYSTEM_COMMUNICATION_READ_ACCESSES
	if(global_verbosity) {
		if(this->role==SYSTEM_COMMSYNCH ) {
			report_channel_event("Internal READ ACCESS");
		}
	}
#endif

#ifdef _REPORT_IO_READ_ACCESS
	if(global_verbosity) {
		if( this->role==IO_COMMSYNCH ) {
			if(this->io_sense==INPUT ) {
				report_channel_event("IO INPUT READ ACCESS");
			} else if (this->io_sense==OUTPUT ) {
				report_channel_event("IO OUTPUT READ ACCESS");
			}
		} // no report for environment channels (internal to the environment)		
	}
#endif
	
	if (this->role==SYSTEM_COMMSYNCH) {

		current_task=sc_get_current_process_handle();
		current_task_info = task_info_by_phandle[current_task];
				
		current_task_info->finished_flag = false; // to avoid it to be considered completion of the task
			
		current_task_info->waiting_comm_sync_signal.write(true);

		// is an internal system level channel, thus which can consume platform communication resources
		if( assoc_plink.resolved() ) { // the physical link is Known, ...
			// if(assoc_plink.is_intracomm() ) { 	//  .. and it an intra communication		
			// } else { //  and it is not an intra communication
			// }
			// regardless if it is an intra-comm or an inter-comm,
			// and if the write is delegated to a network inteface or not,
			// the read is releases when the transfer is completed
			
			// The wait call to the completed_transactions semaphore has to be done
			// for the case the reader  arrives at this point before at least one transfer is completed
			
			// That requires that the logic link has associated not only an event,
			// but a storage variable which nows if one transfer has been completed
			// Moreover, in a buffer channel, serveral write transaction can be completed,
			// before the readear arrival
			// By using a SystemC semaphore we use an unbounded semaphore, that is,
			// which can grow indefinitely.
			// Therefore, in this implementation we are assuming an INFINITE RECEPTION BUFFER
			// (or a sufficiently big reception buffer)
			// (remind that, in KisTA, the transmission buffer can be infinite or limited, depending
			//  on how the netif is configured)
			completed_transactions->wait();
			
			// TO DO : 
			//   - support of counter to report the maximum buffer rx buffer capability required in a trace
			//   - support of finite rx buffer       
		}
	}
	if( this->role==IO_COMMSYNCH ) {
		if(this->io_sense==INPUT ) {
			current_task=sc_get_current_process_handle();
			current_task_info = task_info_by_phandle[current_task];
				
			current_task_info->finished_flag = false; // to avoid it to be considered completion of the task
			
			current_task_info->waiting_comm_sync_signal.write(true);	
		} 
		// NOTE :For the moment, no communication delay considered related to INPUT IO communication channels	
	}
	
	
	this->sc_fifo<T>::read(val_);

	// read event is annotated immediately after the read is completed (call unblocked)
	if(read_th_mon!=NULL) {
		read_th_mon->annotate_data_event();
	}
	
	if(this->role==SYSTEM_COMMSYNCH) {
		current_task_info->waiting_comm_sync_signal.write(false);

		current_task_info->finished_flag = true;

		// guard to let the user task go on executing after release only if the scheduler has granted it a processor
		if(current_task_info->active_signal.read()==false) sc_core::wait(current_task_info->active_signal.posedge_event());
		
	}
	else if( this->role==IO_COMMSYNCH ) {
		if(this->io_sense==INPUT ) {
			
			current_task_info->waiting_comm_sync_signal.write(false);

			current_task_info->finished_flag = true;

			// guard to let the user task go on executing after release only if the scheduler has granted it a processor
			if(current_task_info->active_signal.read()==false) sc_core::wait(current_task_info->active_signal.posedge_event());			
			
		}
		// for the moment, no penalty associated to read INPUT I/O transfer
	}

#ifdef _REPORT_SYSTEM_COMMUNICATION_READ_COMPLETION
	if(global_verbosity) {
		if (this->role==SYSTEM_COMMSYNCH) {
			report_channel_event("READ COMPLETION");
		}
	}
#endif	
#ifdef _REPORT_IO_READ_COMPLETION
	if(global_verbosity) {
		if( this->role==IO_COMMSYNCH ) {
			if(this->io_sense==INPUT ) {
				report_channel_event("IO INPUT READ COMPLETION");
			} else if (this->io_sense==OUTPUT ) {
				report_channel_event("IO OUTPUT READ COMPLETION");
			}
		}
	}
#endif

}

template <class T>
inline
T
fifo_buffer<T>::read()
{
    T tmp;
    read( tmp );
    return tmp;
}

// non-blocking read
/*
template <class T>
inline
bool
fifo_buffer<T>::nb_read( T& val_ )
{
    if( num_available() == 0 ) {
	return false;
    }
    m_num_read ++;
    buf_read( val_ );
    request_update();
    return true;
}
*/

// implementation of single token injection

// basically, it behaves as a bare sc_fifo write, 
// but ensuring it is called only before simulation
// starts, and that it is only done for an internal channel

template <class T>
inline
void
fifo_buffer<T>::initial_write( const T& val_ )
{
	std::string msg;
	
	void check_call_before_sim_start(std::string call_name);
	
	if(this->role==SYSTEM_COMMSYNCH) {
		this->sc_fifo<T>::write(val_);	
		if( assoc_plink.resolved() ) {
			completed_transactions->post();
		}	
#ifdef _REPORT_INIT_TOKENS_INJECTION
		if(global_verbosity) {		
			msg = "Initial write in channel ";
			msg += this->name();
			msg += " completed.";
			SC_REPORT_INFO("KisTA",msg.c_str());
		}
#endif				
	} else {
		msg = "Injecting initial tokens in channel ";
		msg += this->name();
		msg += " is allowed only for system internal channels";
		SC_REPORT_ERROR("KisTA",msg.c_str());
	}
}

template <class T>
inline
void
fifo_buffer<T>::set_init(VOIDFPTR init_f_par)
{
	init_f = init_f_par;
}

template <class T>
inline
bool
fifo_buffer<T>::has_init()
{
	if(init_f==NULL) return false;
	else return true;
}

// blocking write

template <class T>
inline
void
fifo_buffer<T>::write( const T& val_ )
{
	
	std::string			rpt_msg;
	sc_process_handle 	current_task;
	task_info_t			*current_task_info;
	
	sc_time				comm_delay;
	
	processing_element	*pe_src;
	
#ifdef _REPORT_SYSTEM_COMMUNICATION_WRITE_ACCESSES
	if(global_verbosity) {
		if(this->role==SYSTEM_COMMSYNCH ) {
			report_channel_event("WRITE ACCESS");
		}
	}
#endif

#ifdef _REPORT_IO_WRITE_ACCESS
	if(global_verbosity) {
		if( this->role==IO_COMMSYNCH ) {
			if(this->io_sense==INPUT ) {
				report_channel_event("IO INPUT WRITE ACCESS");
			} else if (this->io_sense==OUTPUT ) {
				report_channel_event("IO OUTPUT WRITE ACCESS");
			}		
		} // no report for environment channels (internal to the environment)		
	}
#endif


	// for scheduling simulation in case it is a system communication
	
		
	if(this->role==SYSTEM_COMMSYNCH) {		
		 // it is a system channel
		current_task=sc_get_current_process_handle();
		current_task_info = task_info_by_phandle[current_task];
				
		current_task_info->finished_flag = false; // to avoid it to be considered completion of the task
			
		current_task_info->waiting_comm_sync_signal.write(true);
	} 
	else if(this->role==IO_COMMSYNCH) {
		if(this->io_sense==OUTPUT ) {
			// it is a system channel
			current_task=sc_get_current_process_handle();
			current_task_info = task_info_by_phandle[current_task];
				
			current_task_info->finished_flag = false; // to avoid it to be considered completion of the task
			
			current_task_info->waiting_comm_sync_signal.write(true);			
		}
	}
	
    //this->sc_fifo<T>::write(val_);


	// collect write event if write throughput monitoring has been enabled
	if(write_th_mon!=NULL) {
		write_th_mon->annotate_data_event();
	}

	if(this->role==SYSTEM_COMMSYNCH) {

		// is an internal system level channel, thus which can consume platform communication resources

#ifdef _CONSIDER_COMMUNICATION_DELAYS

#ifdef _REPORT_COMMUNICATION_DELAYS
		if(global_verbosity) {
			// now access the communication resource (which could require schedulable)
			rpt_msg = "Channel: ";
			rpt_msg += this->name();
			rpt_msg += " WRITE access ";
			// by now uses the physical link
			rpt_msg += "(msg. size : ";
			rpt_msg += std::to_string(message_size);
			rpt_msg += " bits)\n";
		}
#endif

		// Only if there is a resolved physical link associated to the system level channel a delay will be associated to it		
		if( assoc_plink.resolved() ) {
			if( is_PE_intracomm(assoc_plink) ) {
				if(assoc_plink.src->get_type()!=PROCESSING_ELEMENT) {
					rpt_msg = "Channel: ";
					rpt_msg += this->name();
					rpt_msg += " WRITE access ";
					// by now uses the physical link
					rpt_msg += "(msg. size : ";
					rpt_msg += std::to_string(message_size);
					rpt_msg += " bits)\n";
					rpt_msg += ". Internal communication not associated to a processing element";
					SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
				}
				
				 // communicattion within the PE
				 // is_PE_intracomm function is provided by link class, for the physical link, it means an intranode comm
				 // constant delay can be configured, but this method can be configured
				 // to support more complex models of intra-node delay depending on the PE architecture
				//comm_delay = assoc_plink.src->getIntraComMaxP2Pdelay(message_size);
				pe_src = static_cast<processing_element *>(assoc_plink.src);
				comm_delay = pe_src->getIntraComMaxP2Pdelay(message_size);
				
#ifdef _REPORT_COMMUNICATION_DELAYS
				if(global_verbosity) {
					rpt_msg += "\tinvolves intracomunication in PE ";
					rpt_msg +=  assoc_plink.src->name();
					rpt_msg += ", associated delay: ";
					rpt_msg +=  comm_delay.to_string();
					rpt_msg += "\n";
				}
#endif
				wait( comm_delay );
				completed_transactions->post();
			} else if(is_PE_PE_comm(assoc_plink)) {
				// the logic fifo communication is supported by a direct PE to PE link (e.g. TDMA channel, fast link, ...)
#ifdef _REPORT_COMMUNICATION_DELAYS
				if(global_verbosity) {
					rpt_msg += "\tinvolves communication over the physical link (";
					rpt_msg += assoc_plink.src->name();
					rpt_msg += ",";
					rpt_msg +=  assoc_plink.dest->name();
					rpt_msg += ")";
				}
#endif	
				
				//if(assoc_plink.src->has_netif()) {
				if(assoc_plink.src->get_type()!=PROCESSING_ELEMENT) {
					rpt_msg = "Channel: ";
					rpt_msg += this->name();
					rpt_msg += " WRITE access ";
					// by now uses the physical link
					rpt_msg += "(msg. size : ";
					rpt_msg += std::to_string(message_size);
					rpt_msg += " bits)\n";
					rpt_msg += ". PE-PE communication. Source PE not associated to a processing element";
					SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
				}					
				pe_src=static_cast<processing_element *>(assoc_plink.src);
				if(pe_src->has_netif()) {
#ifdef _REPORT_COMMUNICATION_DELAYS
				if(global_verbosity) {
					rpt_msg += "Communication delegated to network interface.\n";
				}
#endif

					// note: this declaration and at least three of the following assignatiosn can be likely moved
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
					//assoc_plink.src->get_netif()->request_sending(send_message_request);
					pe_src->get_netif()->request_sending(send_message_request);

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
					if(global_verbosity) {
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
					}
#endif					
					wait( comm_delay );
					completed_transactions->post();
				}
			} else {
				rpt_msg = "Channel: ";
				rpt_msg += this->name();
				rpt_msg += " in WRITE access .";
				// by now uses the physical link
				rpt_msg += " The channel is associated to a physical link currently not supported for a fifo channel type.\n";
				SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
			}
		}		
		
#ifdef _REPORT_COMMUNICATION_DELAYS
		else {
			rpt_msg += "\tAssociated physical link not resolved. No delay will be associated to the transaction.";
		}
		if(global_verbosity) {		
			SC_REPORT_INFO("KisTA",rpt_msg.c_str());
		}
#endif

#endif
	}
	// Note: For the moment, no communication delay associated to the OUTPUT I/O transfers


    this->sc_fifo<T>::write(val_);
    
	if(this->role==SYSTEM_COMMSYNCH) {
		current_task_info->waiting_comm_sync_signal.write(false);
				
		current_task_info->finished_flag = true;

		// guard to let the user task go on executing after release only if the scheduler has granted it a processor
		if(current_task_info->active_signal.read()==false) sc_core::wait(current_task_info->active_signal.posedge_event());	    	
	}    
	else if(this->role==IO_COMMSYNCH) {
		if(this->io_sense==OUTPUT ) {
			current_task_info->waiting_comm_sync_signal.write(false);
				
			current_task_info->finished_flag = true;

			// guard to let the user task go on executing after release only if the scheduler has granted it a processor
			if(current_task_info->active_signal.read()==false) sc_core::wait(current_task_info->active_signal.posedge_event());	    				
		}
	}
	
#ifdef _REPORT_SYSTEM_COMMUNICATION_WRITE_COMPLETION
	if(global_verbosity) {
		if (this->role==SYSTEM_COMMSYNCH) {
			report_channel_event("WRITE COMPLETION");
		}
	}
#endif	
#ifdef _REPORT_IO_WRITE_COMPLETION
	if(global_verbosity) {
		if( this->role==IO_COMMSYNCH ) {
			if(this->io_sense==INPUT ) {
				report_channel_event("IO INPUT WRITE COMPLETION");
			} else if (this->io_sense==OUTPUT ) {
				report_channel_event("IO OUTPUT WRITE COMPLETION");
			}
		}
	}
#endif
	
}

// non-blocking write
/*
template <class T>
inline
bool
fifo_buffer<T>::nb_write( const T& val_ )
{
    if( num_free() == 0 ) {
	return false;
    }
    m_num_written ++;
    buf_write( val_ );
    request_update();
    return true;
}
*/

// not supported yet
/*
 * template <class T>
void bind(VOIDFPTR src, VOIDFPTR src) { // relies on that logic address is task info pointer

}
*/

/*
 // definition not required, already supported by the inheritance of logical link
template <class T>
void fifo_buffer<T>::bind(task_info_t* src_par, task_info_t* dest_par) { // relies on that logic address is task info pointer
	                                                 // (see link.hpp)
	src = src_par;
	dest = dest_par;
}
*/

template <class T>
void fifo_buffer<T>::before_end_of_elaboration() {	
	// to solve the binding if not done yet
	resolve_bind_by_name();		
	// to draw the system-level channel
	sketch_report.draw((logic_link_t)*this,"fifo_buffer");
}


template <class T>
void fifo_buffer<T>::end_of_elaboration() {
	
	std::string rpt_msg;
	processing_element *pe_src;
	
	
	// Calculate (if required) message size associated to tokens...
	rpt_msg = "Channel ";
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

#ifdef _REPORT_FIFO_BUFFER_TOKEN_SIZE_DETERMINATION
	// ... and report it
	SC_REPORT_INFO("KisTA",rpt_msg.c_str());
#endif
	
	if(role==SYSTEM_COMMSYNCH) {
	
		
		// Try for the very last chance Map the logic link associated to the fifo buffer to a comm_res (bus/network) link
		// if at least one has been declared in the model and if there is not association yet...
		//
		if(phy_commres_by_name.size()>0) {
			// Only if the user has declared some instance of communication resource
			// the KisTA library will try to resolve the binding of the logical link
			// associated to the system-level channel to the physical link mapped to
			// an associated communication resource, which is then tried to be extracted
			// at the end of elaboration phase as a last chance
			if(assoc_comm_res_p==NULL) {			
				auto_map();
				
				if(assoc_comm_res_p == NULL) {
					rpt_msg = "No mapping of the channel ";
					rpt_msg += this->name();
					rpt_msg += " to a communication resource was specified and automatic mapping failed.";
					SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
				}
#ifdef _REPORT_AUTOMATIC_CHANNEL_MAPPINGS
				else {
					rpt_msg = "Automatic association (-auto map- call) of channel ";
					rpt_msg += this->name();				
					rpt_msg += " to communication resource ";
					rpt_msg += assoc_comm_res_p->name();
					SC_REPORT_INFO("KisTA",rpt_msg.c_str());
				}
#endif							
			}

#ifdef _REPORT_CHANNEL_MAPPINGS		
			// ... and report the final state of the association
			// system-level channel (logic-link) - communication resource (physical link)
			rpt_msg = "Channel ";
			rpt_msg += this->name();
		//	rpt_msg += " (blocking fifo buffer) ";
			rpt_msg += ": associated to logic link (" ;
			rpt_msg += this->src->name();
			rpt_msg += " -> ";
			rpt_msg += this->dest->name();
			
			rpt_msg += " ), mapped to physical link (";
			
			rpt_msg += assoc_plink.src->name();
			rpt_msg += " -> ";
			rpt_msg += assoc_plink.dest->name();
			
			rpt_msg += ") at resource ";
			rpt_msg += assoc_comm_res_p->name();
			
			SC_REPORT_INFO("KisTA",rpt_msg.c_str());
#endif		
			// so far, it is ensured to work on the same communication resource
			
			// Now register the link in the links table of the communication resource,
			// as a link, that in effect will be used by the logical layer
			//link_info_t initial_link_info; // void
			assoc_comm_res_p->add_link(assoc_plink); // KisTA do not oblige to register an initialized set of properties
			
			// The following command makes a static initial association of properties
			// to the physical channel.
			// In the buffer_fifo channel this makes sense since it is a channel
			// which will handle a static message size in its transaction, so
			// specially if the physical channel is time invariant, this will let cache
			// this data and speed up simulation
			if( is_PE_intracomm(assoc_plink) ) { 
				if(assoc_plink.src->get_type()!=PROCESSING_ELEMENT) {
					rpt_msg += ". Internal communication not associated to a processing element (at end of elaboration)";
					SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
				}
				pe_src=static_cast<processing_element *>(assoc_plink.src);
				assoc_comm_res_p->set_MaxP2Pdelay(assoc_plink,pe_src->getIntraComMaxP2Pdelay(message_size),message_size);
				//assoc_comm_res_p->set_MaxP2Pdelay(assoc_plink,assoc_plink.src->getIntraComMaxP2Pdelay(message_size),message_size);
			} else if ( is_PE_PE_comm(assoc_plink) ) {
				//	While Debugging comment: it seems this calls the virtual method of the base comm_res and not of the 
				//  tdma bus : Why!!!			
				//assoc_comm_res_p->set_MaxP2Pdelay(assoc_plink,message_size);
				assoc_comm_res_p->set_MaxP2Pdelay(assoc_plink,message_size);
				 // This still sets not time, but only creates the info for this link
			} else {								
				// no kind of association
			}
						
			// Register the logic link association to the physical link
			//   (notice that this is fifo_buffer, which inherits logic_link_t
			assoc_comm_res_p->map(*(logic_link_t *)this, assoc_plink);
		}
#ifdef _REPORT_AUTOMATIC_CHANNEL_MAPPINGS
		else {	
			rpt_msg = "No communication resource has been declared in the KisTA model.";
			rpt_msg += "KisTA will not try to associate a resource and so a physical link to the logical link of the channel ";
			rpt_msg += this->name();
			rpt_msg += ". System-level communications have no penalty.";
			SC_REPORT_WARNING("KisTA",rpt_msg.c_str());	
		}	
#endif

		//
		if(assoc_plink.src!=NULL) { // if there is PE (a src phy. address on the associated phy link
			if(assoc_plink.src->get_type()!=PROCESSING_ELEMENT) {
				rpt_msg += ". At end of elaboration";
				SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
			}
			pe_src=static_cast<processing_element *>(assoc_plink.src);

			//if(assoc_plink.src->has_netif()) {
			if(pe_src->has_netif()) {
#ifdef _REPORT_FIFO_BUFFER_NETIF_DETECT_WRITER_PE
				rpt_msg = "The fifo_buffer system-level channel ";
				rpt_msg += this->name();
				rpt_msg += " is associated to a physical link where the source physical address is  PE ";
				rpt_msg += assoc_plink.src->name();
				rpt_msg += ", which has a network interface ";
				//if(assoc_plink.src->get_netif()->finite_tx_buffer()) {
				if(pe_src->get_netif()->finite_tx_buffer()) {
					rpt_msg += "with a TX buffer of ";
					//rpt_msg += std::to_string(assoc_plink.src->get_netif()->get_tx_buffer_size());
					rpt_msg += std::to_string(pe_src->get_netif()->get_tx_buffer_size());
					rpt_msg += " bits. This means that the channel can eventually block in the writing side,";
					rpt_msg += " as long as there are other tasks writting in the same procesor and the buffer gets full.";
				} else {
					rpt_msg += "The channel writes vs an inifinite TX buffer. It means that it can always release immediately at writing side.";	
				}
				SC_REPORT_INFO("KisTA",rpt_msg.c_str());	
#endif
			}
		}
		
		// in the case of system-level channels, some of them can have initial tokens
		// (in an SDF model this is to substitute 
		if(has_init()) {
			init_f(); // (end_of elaboration time) callback from the channel
		}
	}
	else if(role==IO_COMMSYNCH) {
		if (this->resolved()) { // then, the sense can be determined
			io_sense = determine_io_sense();					
		} else {
			// Not resolved. An error is reported with some clue to the user for solving the issue
			rpt_msg = "The logic link for the comm&synch channel ";
			rpt_msg += this->name();
			rpt_msg += " is not resolved. The channel is connected to a system task and an environment task, but";
			rpt_msg += "The source";
			if (this->src != NULL) {
				rpt_msg += " is connected to ";
				// 1st searches the task in the system
				auto it = task_info_by_name.find(src->name());
				if(it !=  task_info_by_name.end()) {
					rpt_msg += " the system task ";
					rpt_msg += src->name();
				} else {
					// otherwise it searches the task in the environment
					auto it = env_tasks_by_name.find(src->name());
					if(it !=  env_tasks_by_name.end()) {
						rpt_msg += " the environment task ";
						rpt_msg += src->name();
					} else {
						rpt_msg += " an UNKNOWN TASK. Unexpected situation.";
						SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
					}
				}
			} else {
				rpt_msg += " is not connected.";
			}
			
			rpt_msg += "The destination";
			if (this->dest != NULL) {
				rpt_msg += " is connected ";
				// 1st searches the task in the system
				auto it = task_info_by_name.find(src->name());
				if(it !=  task_info_by_name.end()) {
					rpt_msg += " the system task ";
					rpt_msg += src->name();
				} else {
					// otherwise it searches the task in the environment
					auto it = env_tasks_by_name.find(src->name());
					if(it !=  env_tasks_by_name.end()) {
						rpt_msg += " the environment task ";
						rpt_msg += src->name();
					} else {
						rpt_msg += " an UNKNOWN TASK. Unexpected situation.";
						SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
					}
				}				
			} else {
				rpt_msg += " is not connected.";
			}
	
			SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		}
	}
	// nothing done if the role is an environment channel (then pure SystemC channels are used

}

// assumes that the link is resolved
template <class T>
io_comm_synch_sense_t fifo_buffer<T>::determine_io_sense() {

	std::string rpt_msg;
	bool src_in_system; // otherwise, assumed in the environment
	bool dest_in_system; // otherwise, assumed in the environment
/*
cout << "Environment tasks: " << endl;
auto t = task_info_by_name.begin();
while(t!=task_info_by_name.end()) {
	cout << t->second->name();
	cout << ",";
	t++;
}
cout << endl;

cout << "Environment tasks: " << endl;
auto et = env_tasks_by_name.begin();
while(et!=env_tasks_by_name.end()) {
	cout << et->second->name();
	cout << ",";
	et++;
}
cout << endl;
**/	
	// 1st searches the type of task of the source
	auto it1 = task_info_by_name.find(src->name());
	if(it1 !=  task_info_by_name.end()) {
		src_in_system = true;
	} else {
		// otherwise it searches the task in the environment
		auto it2 = env_tasks_by_name.find(src->name());
		if(it2 !=  env_tasks_by_name.end()) {
			src_in_system = false;
		} else {
			rpt_msg = "Source task type of I/O channel ";
			rpt_msg += this->name();
			rpt_msg += " could not be determined.";
			SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		}
	}

	// 2nd searches the type of task of the destination
	it1 = task_info_by_name.find(dest->name());
	if(it1 !=  task_info_by_name.end()) {
		dest_in_system = true;
	} else {
		// otherwise it searches the task in the environment
		auto it2 = env_tasks_by_name.find(dest->name());
		if(it2 !=  env_tasks_by_name.end()) {
			dest_in_system = false;
		} else {
			rpt_msg = "Destination task type of IO channel ";
			rpt_msg += this->name();
			rpt_msg += " could not be determined.";
			SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		}
	}
	
	if(src_in_system && (!dest_in_system)) {
		return OUTPUT;
	} else if ((!src_in_system) && dest_in_system) {
		return INPUT;
	} else if (src_in_system && dest_in_system) {
		rpt_msg = "Channel ";
		rpt_msg += this->name();
		rpt_msg += " seems to be actually a system channel, since source and destination tasks (";
		rpt_msg += this->src->name();
		rpt_msg += "->"; 
		rpt_msg += this->dest->name();
		rpt_msg += ") both belong to the system. However, the channel has been stated as I/O."; 
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		return UNSET_IO_SENSE;
	} else if ((!src_in_system) && (!dest_in_system)) {
		rpt_msg = "Channel ";
		rpt_msg += this->name();
		rpt_msg += " seems to be actually an environment channel, since source and destination tasks (";
		rpt_msg += this->src->name();
		rpt_msg += "->"; 
		rpt_msg += this->dest->name();
		rpt_msg += ") both belong to the environment. However, the channel has been stated as I/O."; 
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());	
		return UNSET_IO_SENSE;		
	} else {
		return UNSET_IO_SENSE;
	}
}

template <class T>
void fifo_buffer<T>::set_message_size(unsigned int size_bits_par) {
	message_size = size_bits_par;
}

// MAP methods

template <class T>
void fifo_buffer<T>::auto_map() {
	
	std::string rpt_msg;
	
	processing_element *pe_src;
	
//	phy_address phy_src, phy_dest;

	// For a direct allocation of resources
	// get the associated physical link (as a pair of physical addresses)
	
	// Calculate (if required) message size associated to tokens...
	
	if (this->resolved()) { // first, it is required that the logic or system level link (in this case, a fifo_buffer link) is resolved
	// as	
	// if((src!=NULL) && (dest!=NULL) ) {
		
		assoc_plink.src = get_phy_address(src);
		assoc_plink.dest = get_phy_address(dest);

		if(assoc_plink.resolved()) {
		// same as
		// if((assoc_plink.src!=NULL) && (assoc_plink.dest==NULL)) {
			//assoc_comm_res_p = assoc_plink.src->get_connected_comm_res();
			// no instead
			if(assoc_plink.src->get_type()!=PROCESSING_ELEMENT) {
				rpt_msg = "Automap for fifo_buffer channel \"";
				rpt_msg += this->name();
				rpt_msg += "\" provocked failure, despite the associated physical link is resolved, the source HW resource \"";
				rpt_msg += assoc_plink.src->name();
				rpt_msg += "\" is not a processing element (at auto map)";
				SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
			}
			pe_src=static_cast<processing_element *>(assoc_plink.src);
			assoc_comm_res_p = pe_src->get_connected_comm_res();
			
		} else  {
#ifdef _REPORT_FIFO_BUFFER_AUTOMAP_FAILURE		
			rpt_msg = "Automap for channel ";
			rpt_msg += this->name();
			rpt_msg += " failed because ";
			if((assoc_plink.src==NULL) && (assoc_plink.dest==NULL)) {
				rpt_msg += "neither the source, nor destination physical addresses (PEs) could be retrieved.";
			} else if(assoc_plink.src==NULL) {
				rpt_msg += "the source physical addresses (PEs) could not be retrieved.";
			} else if(assoc_plink.dest==NULL) {
				rpt_msg += "the destination physical addresses (PEs) could not be retrieved.";
			} else {
				rpt_msg += " unexpected situation after retrieving the physical addresses.";
			}
			SC_REPORT_WARNING("",rpt_msg.c_str());
#endif			
		}
	} else {
#ifdef _REPORT_FIFO_BUFFER_AUTOMAP_FAILURE				
		rpt_msg = "Automap for channel ";
		rpt_msg += this->name();
		rpt_msg += " failed because ";	
		if((src==NULL) && (dest==NULL)) {
			rpt_msg += "neither the logic source and destination addresses (tasks) where provided. You can either pass them to the fifo_buffer constructor or through the bind method.";
		} else if(src==NULL) {
			rpt_msg += "the logic source address (task) was not provided. Check that the source logical address (task) is not null or has a valid name, either at the fifo_buffer constructor, or when using the bind method.";
		} else if(dest==NULL) {
			rpt_msg += "the logic destination address (task) was not provided. Check that the destination logical address (task) is not null or has a valid name, either at the fifo_buffer constructor, or when using the bind method.";
		} else {
			rpt_msg += "unexpected situation after retrieving logical addresses.";
		}
		SC_REPORT_WARNING("",rpt_msg.c_str());
#endif		
	}
	
}

// Currently, a single association is possible!!
template <class T>
void fifo_buffer<T>::map_to(phy_comm_res_t *phy_comm_res_par) {
	
	std::string rpt_msg;
		
	if ( sc_get_status()&(SC_ELABORATION|SC_BEFORE_END_OF_ELABORATION) ) {	
	
		if(	(assoc_comm_res_p != NULL) && (assoc_comm_res_p!=phy_comm_res_par) ) {
			// a previously associated communication resource already found by the automatic map
			// different from the one that we want to state
			rpt_msg = "Explicit association (via -map_to- call) of channel ";
			rpt_msg += this->name();				
			rpt_msg += " to communication resource ";
			rpt_msg += phy_comm_res_par->name();
			rpt_msg += " overrides a previous assignation to the communication resource: ";
			rpt_msg += assoc_comm_res_p->name();
			rpt_msg += "\n";			
			rpt_msg += " KisTA uses a single generic communication resource, regardless the actual modelled network";
			rpt_msg += " is composed of several subnetworks or communication resources.";
			SC_REPORT_WARNING("KisTA",rpt_msg.c_str());			
		} 
		assoc_comm_res_p = phy_comm_res_par;
	} else {
		rpt_msg = "Explicit association (via -map_to- call) of channel ";
		rpt_msg += this->name();				
		rpt_msg += " to communication resource ";
		rpt_msg += phy_comm_res_par->name();			
		rpt_msg += " can be done only at elaboration or before the end of elaboration.";
		SC_REPORT_ERROR("KisTA","map ");
	}
}

template <class T>
void fifo_buffer<T>::report_channel_event(std::string event_description) {
	std::string rpt_msg;
	if (this->role==SYSTEM_COMMSYNCH) {
		// now access the communication resource (which could require schedulable)
		rpt_msg = "INTERNAL Channel \"";
	} else if (this->role==IO_COMMSYNCH) {
		rpt_msg = "I/O Channel \"";
	}
	rpt_msg += this->name();
	rpt_msg += "\": ";
	rpt_msg += event_description;
	rpt_msg += " at ";
	rpt_msg += sc_time_stamp().to_string();
	SC_REPORT_INFO("KisTA",rpt_msg.c_str());
}

// implementation of measurement activation methods

template <class T>
void fifo_buffer<T>::measure_read_throughput() {
	std::string rpt_msg;
	if ( sc_get_status()&(SC_ELABORATION|SC_BEFORE_END_OF_ELABORATION) ) {	
		read_th_mon = new throughput_monitor;
	} else {
		rpt_msg = "Activation of read throughput measurement (via measure_read_throughput method) of channel ";
		rpt_msg += this->name();
		rpt_msg += "has to be done before the end of the elaboration.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	}	
}

template <class T>
void fifo_buffer<T>::measure_write_throughput() {
	std::string rpt_msg;
	if ( sc_get_status()&(SC_ELABORATION|SC_BEFORE_END_OF_ELABORATION) ) {	
		write_th_mon = new throughput_monitor;
	} else {
		rpt_msg = "Activation of write throughput measurement (via measure_write_throughput method) of channel ";
		rpt_msg += this->name();
		rpt_msg += "has to be done before the end of the elaboration.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	}
}

// implementation of throughput report methods

template <class T>
double fifo_buffer<T>::get_write_throughput(const char *unit) {
	std::string rpt_msg;
	
	check_call_after_sim_start("get_write_throughput");
	
	if(write_th_mon==NULL) {
		rpt_msg = "get_write_stationary_throughput, called for channel ";
		rpt_msg += this->name();
		rpt_msg += ", can be called can be called only after a previous activation of the measurement through the measure_write_throughput method, before the simulation start.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		return -1.0;
	} else {
		return write_th_mon->get_throughput(unit);
	}
}

template <class T>
double fifo_buffer<T>::get_write_stationary_throughput(const char *unit) {
	std::string rpt_msg;
	
	check_call_after_sim_start("get_write_stationary_throughput");
	
	if(write_th_mon==NULL) {
		rpt_msg = "get_write_stationary_throughput, called for channel ";
		rpt_msg += this->name();
		rpt_msg += ", can be called only after a previous activation of the measurement through the measure_write_throughput method, before the simulation start.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		return -1.0;
	} else {
		return write_th_mon->get_stationary_throughput(unit);
	}
}

template <class T>
double fifo_buffer<T>::get_read_throughput(const char *unit) {
	std::string rpt_msg;
	
	check_call_after_sim_start("get_read_throughput");
		
	if(write_th_mon==NULL) {
		rpt_msg = "get_read_stationary_throughput, called for channel ";
		rpt_msg += this->name();
		rpt_msg += ", can be called only after a previous activation of the measurement through the measure_read_throughput method, before the simulation start.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		return -1.0;
	} else {
		return read_th_mon->get_throughput(unit);
	}
}

template <class T>
double fifo_buffer<T>::get_read_stationary_throughput(const char *unit) {
	std::string rpt_msg;
	
	check_call_after_sim_start("get_read_stationary_throughput");
		
	if(write_th_mon==NULL) {
		rpt_msg = "get_read_stationary_throughput, called for channel ";
		rpt_msg += this->name();
		rpt_msg += ", can be called only after a previous activation of the measurement through the measure_read_throughput method, before the simulation start.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		return -1.0;
	} else {
		return read_th_mon->get_stationary_throughput(unit);
	}

}

// implementation of normalized throughput report methods


template <class T>
double fifo_buffer<T>::get_normalized_write_throughput(const char *unit) {
	std::string rpt_msg;
	
	check_call_after_sim_start("get_normalized_write_throughput");
	
	if(write_th_mon==NULL) {
		rpt_msg = "get_normalized_write_throughput, called for channel ";
		rpt_msg += this->name();
		rpt_msg += ", can be called only after a previous activation of the measurement through the measure_write_throughput method, before the simulation start.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		return -1.0;
	} else {
		return write_th_mon->get_normalized_throughput(unit);
	}
}

template <class T>
double fifo_buffer<T>::get_read_period(const char *unit) {
	std::string rpt_msg;
	
	check_call_after_sim_start("get_read_period");
	
	if(read_th_mon==NULL) {
		rpt_msg = "get_read_period, called for channel ";
		rpt_msg += this->name();
		rpt_msg += ", can be called only after a previous activation of the measurement through the measure_wread_throughput method, before the simulation start.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		return -1.0;
	} else {
		return read_th_mon->get_period(unit);
	}
}

template <class T>
sc_time fifo_buffer<T>::get_read_period() {
	std::string rpt_msg;
	
	check_call_after_sim_start("get_read_period");
	
	if(read_th_mon==NULL) {
		rpt_msg = "get_read_period, called for channel ";
		rpt_msg += this->name();
		rpt_msg += ", can be called only after a previous activation of the measurement through the measure_wread_throughput method, before the simulation start.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		return -1.0;
	} else {
		return read_th_mon->get_period();
	}
}


template <class T>
double fifo_buffer<T>::get_write_period(const char *unit) {
	std::string rpt_msg;
	
	check_call_after_sim_start("get_write_period");
	
	if(write_th_mon==NULL) {
		rpt_msg = "get_write_period, called for channel ";
		rpt_msg += this->name();
		rpt_msg += ", can be called only after a previous activation of the measurement through the measure_write_throughput method, before the simulation start.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		return -1.0;
	} else {
		return write_th_mon->get_period(unit);
	}
}

template <class T>
sc_time fifo_buffer<T>::get_write_period() {
	std::string rpt_msg;
	
	check_call_after_sim_start("get_write_period");
	
	if(write_th_mon==NULL) {
		rpt_msg = "get_write_period, called for channel ";
		rpt_msg += this->name();
		rpt_msg += ", can be called only after a previous activation of the measurement through the measure_write_throughput method, before the simulation start.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		return -1.0;
	} else {
		return write_th_mon->get_period();
	}
}

template <class T>
double fifo_buffer<T>::get_normalized_write_stationary_throughput(const char *unit) {
	std::string rpt_msg;
	
	check_call_after_sim_start("get_normalized_write_stationary_throughput");
	
	if(write_th_mon==NULL) {
		rpt_msg = "get_normalized_write_stationary_throughput, called for channel ";
		rpt_msg += this->name();
		rpt_msg += ", can be called only after a previous activation of the measurement through the measure_write_throughput method, before the simulation start.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		return -1.0;
	} else {
		return write_th_mon->get_normalized_stationary_throughput(unit);
	}
}

template <class T>
double fifo_buffer<T>::get_normalized_read_throughput(const char *unit) {
	std::string rpt_msg;
	
	check_call_after_sim_start("get_normalized_read_throughput");
		
	if(write_th_mon==NULL) {
		rpt_msg = "get_normalized_read_throughput, called for channel ";
		rpt_msg += this->name();
		rpt_msg += ", can be called can be called only after a previous activation of the measurement through the measure_read_throughput method, before the simulation start.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		return -1.0;
	} else {
		return read_th_mon->get_normalized_throughput(unit);
	}
}

template <class T>
double fifo_buffer<T>::get_normalized_read_stationary_throughput(const char *unit) {
	std::string rpt_msg;
	
	check_call_after_sim_start("get_normalized_read_stationary_throughput");
		
	if(write_th_mon==NULL) {
		rpt_msg = "get_normalized_read_stationary_throughput, called for channel ";
		rpt_msg += this->name();
		rpt_msg += ", can be called can be called only after a previous activation of the measurement through the measure_read_throughput method, before the simulation start.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
		return -1.0;
	} else {
		return read_th_mon->get_normalized_stationary_throughput(unit);
	}
}


} // namespace KisTA

#endif
