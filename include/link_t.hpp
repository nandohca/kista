/*****************************************************************************

  link_t.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 August

 *****************************************************************************/

#ifndef LINK_T_HPP
#define LINK_T_HPP

#include <systemc.h>

#include "defaults.hpp"

#define NO_ADDRESS NULL

namespace kista {

// advanced declaration of classes and methods
class processing_element;
class scheduler;
class task_info_t;


// Functions to get addresses by elem name
template<class address_type>
address_type get_address_by_elem_name(std::string elem_name) {
	std::string rpt_msg;
	rpt_msg = "The generic get_address_by_elem_name function template was called.";
	rpt_msg += "This is because link_t bind by name has been performed for an address type not currently supported.";
	rpt_msg += "Such support requires an specialization of the get_address_by_elem_name function for the address type ";
	rpt_msg += typeid(address_type).name();
	rpt_msg += ".";
	SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	return NULL;
}


template<class address_type>
class link_t  {
//class link_t : virtual public sc_prim_channel {  // see (**)
public:
	
	link_t();
	link_t(const char *name_par);
	link_t(const char *name_par, address_type src_par, address_type dest_par);
	link_t(const char *name_par, const char  *src_name_par, const char *dest_name_par);

	address_type src;
	address_type dest;
	std::string get_link_name(); // Note: for the moment, name is not used
	                          // it would mean class when buffer channel inherits sc_fifo and logic link
	                          // A solutions is using and ad-hoc fifo , instead sc_fifo, but this is avoided so far
	                          // Then, so far a kista buffer channel has a "link name" and a "name" associated
	
	void bind(address_type src_par, address_type dest_par);
	void bind(const char *src_name_par, const char *dest_name_par);
		
	bool less_than(const link_t<address_type> &llink) const;
	bool operator<(const link_t<address_type> &llink) const;
	bool operator==(const link_t<address_type> &llink) const;
	// to enable the use in maps
//	bool operator()( MyType const& lhs, MyType const& rhs ) const

	bool is_intracomm() const; // return true whenever the sender and destination address are the same
	
	void print(ostream &os) const;
	
	bool resolved();
	void resolve_bind_by_name(); // to solve the bind from the name parameters
	
	// see (**)
	//void before_end_of_elaboration(); // to try to solve the bind by name before the end of the simulation

private:
	void init();

	// names of the source and destination linked resources
	// (this serves to support naming based binding and their resolution at either construction of before end of elaboration time
	std::string link_name;
	std::string src_elem_name;
	std::string dest_elem_name;
	

	void check_binding_status();
	void print_binding_info();
	
};


    //bool operator()( MyType const& lhs, MyType const& rhs ) const
    //{
        ////  ...
    //}

template<class address_type>
void link_t<address_type>::init()
{
	src = NO_ADDRESS;
	dest = NO_ADDRESS;
}

template<class address_type>
link_t<address_type>::link_t()
{
	init();
}

template<class address_type>
link_t<address_type>::link_t(const char *name_par)
{
	link_name = name_par;
	init();
}

template<class address_type>
link_t<address_type>::link_t(const char *name_par, address_type src_par, address_type dest_par)
{
	link_name = name_par;
	init();	
	src = src_par;
	dest = dest_par;
}

template<class address_type>
link_t<address_type>::link_t(const char *name_par, const char  *src_name_par, const char *dest_name_par)
{
	link_name = name_par;
		
	init();	

	src_elem_name = src_name_par;
	dest_elem_name = dest_name_par;
}

template<class address_type>
std::string link_t<address_type>::get_link_name()
{
	return link_name;
}

template<class address_type>
bool link_t<address_type>::resolved() {
	return (src!=NULL) && (dest!=NULL);
}

template<class address_type>
void link_t<address_type>::check_binding_status() {
	std::string rpt_msg;	
	if(!resolved()) {
		rpt_msg = "Link ";
		rpt_msg += link_name;
		rpt_msg += " not resolved.";
		print_binding_info();
//		SC_REPORT_ERROR("KisTA:",rpt_msg.c_str());		
		SC_REPORT_WARNING("KisTA:",rpt_msg.c_str());
				// This reports if the binding in the link cannot be resolved
				// It seems that it is better to report as a warning (instead of as an error) the unpossibility to solve a binding 
				// (specifically, for a logical link), since some KisTA high-level models not using communication resources
				// will not require to define logical links (so to later bind then to communication resources)
	}
}

template<class address_type>
void link_t<address_type>::print_binding_info() {
#ifdef _PRINT_BINDING_INFO
	std::string rpt_msg;
	rpt_msg += "Binding (src=";
	if(src!=NO_ADDRESS) rpt_msg +=  src->name();
	else rpt_msg +=  "??";
	rpt_msg += ", dest=";
	if(dest!=NO_ADDRESS) rpt_msg +=  dest->name();
	else rpt_msg +=  "??";
	rpt_msg += ")";
#endif
}

template<class address_type>
void link_t<address_type>::bind(address_type src_par, address_type dest_par)
{
	src = src_par;
	dest = dest_par;
}

template<class address_type>
void link_t<address_type>::bind(const char *src_name_par, const char *dest_name_par) {
	
	src_elem_name = src_name_par;
	dest_elem_name = dest_name_par;

	src = get_address_by_elem_name<address_type>(src_elem_name);
	dest = get_address_by_elem_name<address_type>(dest_elem_name);

#ifdef _PRINT_BINDING_INFO	
	std::string rpt_msg;
	if(resolved()) {
		rpt_msg = "";
		print_binding_info();
		rpt_msg + = " resolved at construction time.";
		SC_REPORT_INFO("KisTA",rpt_msg.c_str());
	} else {
		rpt_msg = "Name based binding (";
		rpt_msg += src_name_par;
		rpt_msg += ", ";
		rpt_msg += dest_name_par;
		rpt_msg += ") inclompletely resolved at construction time,";
		print_binding_info();
		rpt_msg += ".\nKisTA will try to solve the link before the end of elaboration.";
		SC_REPORT_WARNING("KisTA",rpt_msg.c_str());
	}
#endif
}

template<class address_type>
void link_t<address_type>:: resolve_bind_by_name() {	
	std::string rpt_msg;
	
	// link_t class tried to solve here the unsolved bindings and report an error if the link remains unsolved
	if(!resolved()) {	

#ifdef _PRINT_BINDING_INFO	
		rpt_msg = "Trying to resolve ";
		print_binding_info();
		rpt_msg + = " before the end of elaboration.";
		SC_REPORT_INFO("KisTA",rpt_msg.c_str());
#endif

		if(src==NO_ADDRESS) { // try to resolve src address
			if(src_elem_name.size()!=0) {
				src = get_address_by_elem_name<address_type>(src_elem_name);
			} else {
				rpt_msg = "Source address cannot be resolved by the end of elaboration because no source (task) name was provided to the channel";
				SC_REPORT_WARNING("KisTA",rpt_msg.c_str());
			}
		}
		if(dest==NO_ADDRESS) { // try to resolve src address
			if(dest_elem_name.size()!=0) {
				dest = get_address_by_elem_name<address_type>(dest_elem_name);
			} else {
				rpt_msg = "Source address cannot be resolved by the end of elaboration because no source (task) name was provided to the channel";
				SC_REPORT_WARNING("KisTA",rpt_msg.c_str());
			}

		}
	}

	check_binding_status();
	
#ifdef _PRINT_BINDING_INFO	
	rpt_msg = "";
	print_binding_info();
	rpt_msg + = " resolved before the end of elaboration.";
	SC_REPORT_INFO("KisTA",rpt_msg.c_str());
#endif	
}

/*
 * 
 * The end_of_elaboration check is not solved here because SystemC primitive channels
 * currently do not inherit sc_prim_channel as virtual. Because of that, inheriting
 * sc_prim_channel by link and, for instance, sc_prim_channel by sc_fifo produces a clash 
 * which produces two objects of sc_prim_channel type and ambiguities. 
 * The standard technique requires inheriting sc_prim_channel as virtual by both, link_t and 
 * by sc_fifo
 * There are two possible workarounds:
 *  - a) that fifo_buffer do not inherits sc_fifo, but use an instance of it internally, and
 *  - b) that link does not inherit sc_prim_channel and delegate to the extended standard channel, in this
 *    case, fifo_buffer, to solve the binding at end_of_elaboration time in case it is not resolved.
 *   
 *    The b) alternative has been selected in order to avoid double call to the write and read recurrent methods
 *    and to provide maximum compatibility with the standard interfaces.
 *
 * 
 
template<class address_type>
void link_t<address_type>:: resolve_bind_by_name() {
	resolve_bind_by_name();
}
*/ 

template<class address_type>
bool link_t<address_type>::less_than(const link_t<address_type> &llink) const
{
	if(this->src < llink.src) {
		return true;
	} else {
		if(this->dest < llink.dest) {
			return true;
		} else {
			return false;
		}
	}
}
	
template<class address_type>
bool link_t<address_type>::operator<(const link_t<address_type> &link) const
{
	// unfold the code instead of calling less_than for speed
	if(this->src < link.src) {
		return true;
	} else {
		if(this->dest < link.dest) {
			return true;
		} else {
			return false;
		}
	}
}

template<class address_type>
bool link_t<address_type>::operator==(const link_t<address_type> &link) const
{
/*
cout << "COMPARING EQ ADDRESSES:" << endl;
cout << "SRC : ";
cout << this->src << " , " << link.src << endl;
cout << "DEST : ";
cout << this->dest << " , " << link.dest << endl;
*/
	return (this->src==link.src && this->dest==link.dest);
}

template<class address_type>
bool link_t<address_type>::is_intracomm() const
{
	return (this->src==this->dest);
}


#define _PRINT_LINK_ADDRESSES

template<class address_type>
void link_t<address_type>::print(ostream& os) const {
	os << "(";
	if(this->src==NULL) os << "NULL";
	else os << this->src->name();
#ifdef _PRINT_LINK_ADDRESSES	
	os << "(";
	if(this->src==NULL) os << "@0";
	else os << this->src;
	os << ")";
#endif
	os << ",";
	os << this->dest->name();
#ifdef _PRINT_LINK_ADDRESSES	
	os << "(";
	if(this->dest==NULL) os << "@0";
	else os << this->dest;
	os << ")";
#endif
	os << ")";
}

// operator << for easy reporting of links
// when passing a reference...
template<class address_type>
ostream& operator<<(ostream& os, const link_t<address_type> &link) {
	link.print(os);
	return os;
}

} // end if kista namespace

// Definition of hass function for the link_t 
// (required for defining unordered_map structures taking link_t as key)

namespace std
{

using namespace kista;
	
template<class address_type>
struct hash<link_t<address_type> > : public __hash_base<size_t, link_t<address_type> >
{
	size_t operator()(const link_t<address_type>& link) const
	{
		std::string hash_str;
		hash<string> hasher;
		// concatenates link src and dest name
		hash_str = link.src->name();
		hash_str += link.dest->name();
		return hasher.operator ()( hash_str);
	}
};


//
// Definition of a hash function for enabling matching through the (link, message size)
// This way, a communication resource can cache attributes for a (link, message size) key
//


template<class address_type>
struct hash< pair<link_t<address_type>, unsigned int> > : public __hash_base<size_t, pair<link_t<address_type>, unsigned int> >
{
	size_t operator()(const pair<link_t<address_type>, unsigned int> &pair) const
	{
		std::string hash_str;
		hash<string> hasher;
		
		// concatenates link src and dest name
		hash_str = pair.first.src->name();
		hash_str += pair.first.dest->name();
		// concatenates message size
		hash_str += to_string(pair.second);
		
		return hasher.operator ()( hash_str);
	}
};


}

#endif
