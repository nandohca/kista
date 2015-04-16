/*****************************************************************************

  tikz_activity_trace.cpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 June

 *****************************************************************************/

#ifndef TIKZ_ACTIVITY_TRACE_CPP
#define TIKZ_ACTIVITY_TRACE_CPP

#include "defaults.hpp"
//#include "types.hpp"
#include "global_elements.hpp"

#include "task_info_t.hpp"
#include "scheduler.hpp"

#include "tikz_activity_trace.hpp"

namespace kista {

// -----------------------------------
// Implementation of the C-like API
// -----------------------------------

// dumps a file with a TIKZ image tracing tasks activity, scheduler activity and communication activity
tikz_activity_trace_handler create_tikz_activity_trace(std::string ttikz_file_name_par) {
	tikz_activity_trace *tat_p;
	tat_p = new tikz_activity_trace(sc_module_name(ttikz_file_name_par.c_str()));
	tikz_activity_trace_handler_table.push_back(tat_p); // update a global table with the activity traces
	return tat_p;
}

tikz_activity_trace_handler create_tikz_activity_trace() {
	tikz_activity_trace *tat_p;
	tat_p = new tikz_activity_trace;
	tikz_activity_trace_handler_table.push_back(tat_p); // update a global table with the activity traces
	return tat_p;	
}

		// by default, previous calls enable a tikz tracing with the default events limit and start time
void set_max_number_of_time_stamps(tikz_activity_trace_handler handler, unsigned int max_time_stamps) {
	check_call_at_elaboration("set_max_number_of_time_stamps");
	if(max_time_stamps<2) {
		SC_REPORT_ERROR("KisTA","The number of collected time stamps collected for TiKZ trace should be 2 or more. Please, check any call to set_max_number_of_time_stamps");
	}
	handler->set_max_number_of_time_stamps(max_time_stamps);
}

void set_start_time(tikz_activity_trace_handler handler, sc_time start_time) {
	check_call_at_elaboration("set_start_time");
	handler->set_start_time(start_time);
}

void cluster(tikz_activity_trace_handler handler) {
	check_call_at_elaboration("cluster");
	handler->cluster();
}

void compact(tikz_activity_trace_handler handler) {
	check_call_at_elaboration("compress");
	handler->compact();
}
	
void do_not_show_schedulers(tikz_activity_trace_handler handler) {
	check_call_at_elaboration("do_not_show_schedulers");
	handler->set_landscape();
}

void set_base_lines_separation(tikz_activity_trace_handler handler, unsigned int separation) {
	check_call_at_elaboration("set_base_line_separation");
	handler->set_base_lines_separation(separation);
}

void set_time_stamps_max_separation(tikz_activity_trace_handler handler, unsigned int max_separation) {
	check_call_at_elaboration("set_time_stamps_max_separation");
	handler->set_time_stamps_max_separation(max_separation);
}

void set_scale(tikz_activity_trace_handler handler, float scale) {
	check_call_at_elaboration("set_scale");
	handler->set_scale(scale);
}

void set_landscape(tikz_activity_trace_handler handler) {
	check_call_at_elaboration("set_landscape");
	handler->set_landscape();
}

void no_text_in_traces(tikz_activity_trace_handler handler) {
	check_call_at_elaboration("no_labels");
	handler->no_text_in_traces();
}

void do_not_show_unactive_boxes(tikz_activity_trace_handler handler) {
	check_call_at_elaboration("do_not_show_unactive_boxes");
	handler->do_not_show_unactive_boxes();
}

void only_image(tikz_activity_trace_handler handler) {
	check_call_at_elaboration("only_image");
	handler->only_image();
}

// ---------------------------------------------------
// Implementation of the tikz_activity_trace class
// ---------------------------------------------------

tikz_activity_trace::tikz_activity_trace(sc_module_name name) : sc_module(name) {
		// tikz tracing
	    ttikz_file_name = this->name();
		ttikz_file_name += ".tex";
		ttikz_start_trace_time = sc_time(DEFAULT_TIKZ_TRACE_START_TIME_NS,SC_NS);
	    ttikz_max_n_time_stamps = DEFAULT_TIKZ_TRACE_EVENTS_LIMIT;		
		base_lines_separation = DEFAULT_TIKZ_TRACE_BASE_LINES_SEPARATION;
		time_stamps_max_separation = DEFAULT_TIKZ_TIME_STAMPS_MAX_SEPARATION;
		ttikz_trace_scale = DEFAULT_TIKZ_TRACE_SCALE;
		ttikz_trace_landscape = false;
		show_schedulers_activity = true;
		show_text_in_traces = true;
		show_unactive_boxes = true;
		clustered_style = false;
		compact_style = false;
		only_image_flag = false;
}

void tikz_activity_trace::cluster() {
	clustered_style = true;
}

void tikz_activity_trace::compact() {
	compact_style = true;
}

void tikz_activity_trace::set_max_number_of_time_stamps(unsigned int max_time_stamps) {
	ttikz_max_n_time_stamps = max_time_stamps;
}
	
void tikz_activity_trace::set_start_time(sc_time start_time) {
	ttikz_start_trace_time = start_time;
}

void tikz_activity_trace::do_not_show_schedulers() {
	show_schedulers_activity = false;
}

void tikz_activity_trace::set_base_lines_separation(unsigned int separation) {
	base_lines_separation = separation;
}

void tikz_activity_trace::set_time_stamps_max_separation(unsigned int max_separation) {
	time_stamps_max_separation = max_separation;
}

void tikz_activity_trace::set_scale(float scale) {
	ttikz_trace_scale = scale;
}

void tikz_activity_trace::set_landscape() {
	ttikz_trace_landscape = true;
}

void tikz_activity_trace::no_text_in_traces() {
	show_text_in_traces = false;
}

void tikz_activity_trace::do_not_show_unactive_boxes() {
	show_unactive_boxes = false;
}

void tikz_activity_trace::only_image() {
	only_image_flag = true;
}

void tikz_activity_trace::before_end_of_elaboration() {
	
	// creates the tikz trace process
	sc_spawn_options spawn_options_tikz_trace_proc;
	
	task_states_table_t *task_state_table_p;
	sched_states_table_t *sched_state_table_p;
	compact_states_table_t	*compact_state_table_p;

	// static sensitivity: on default all tasks state changes, scheduler change
	// TO DO: add communication events (read and writes)
	tasks_info_by_name_t::iterator it;
	sched_set_by_name_t::iterator it_sched;
	
	for(it = task_info_by_name.begin(); it != task_info_by_name.end(); ++it ) {
		spawn_options_tikz_trace_proc.set_sensitivity(&it->second->state_signal.value_changed_event());
	}
	
	for(it_sched = scheds_by_name.begin(); it_sched != scheds_by_name.end(); ++it_sched ) {
		spawn_options_tikz_trace_proc.set_sensitivity(&it_sched->second->state_signal.value_changed_event());
	}
	
	// Initializes the activity data structure with void activity vectors for each task
	for(it = task_info_by_name.begin(); it != task_info_by_name.end(); ++it ) {
		task_state_table_p = new task_states_table_t;
		task_activity[it->second] = task_state_table_p;
	}
	
	// Initializes the activity data structure with void activity vectors for each scheduler
	//if(show_schedulers_activity) {
		for(it_sched = scheds_by_name.begin(); it_sched != scheds_by_name.end(); ++it_sched ) {
			sched_state_table_p = new sched_states_table_t;
			sched_activity[it_sched->second] = sched_state_table_p;
		}
	//}
	
	// Initializes the compact activity data structure with void compact activity vectors for each scheduler
	for(it_sched = scheds_by_name.begin(); it_sched != scheds_by_name.end(); ++it_sched ) {
		compact_state_table_p = new compact_states_table_t;
		compact_activity[it_sched->second] = compact_state_table_p;
	}
		
	sc_spawn(
		sc_bind(&tikz_activity_trace::tikz_trace_activity_process,this),
		"tikz_trace_activity_process",
		&spawn_options_tikz_trace_proc
	);
}				

void tikz_activity_trace::end_of_simulation() {
	
	// calculate the spans given the time stamps
	// first calculate the time gap corresponding
	// for it, calculate the max time gap and divide it by the max separation
	sc_time max_gap, cur_gap;
	double normalized_distance;
	unsigned int span_value;
	std::string	msg;
	
	if(ttikz_start_trace_time>sc_time_stamp()) {
		msg = "Start time of activity trace (";
		msg += ttikz_start_trace_time.to_string();
		msg += ") bigger than the last simulation time (";
		msg += sc_time_stamp().to_string();
		msg += "). Either, extend simulation time or start the sample before.";
		SC_REPORT_ERROR("KisTA",msg.c_str());		
	}
	cout << "Last time stamp: " << sc_time_stamp() << endl; 
	if(time_stamps.size()<2) {
		msg = "Unexpected error in the TiKZ activity trace. ";
		msg += std::to_string(time_stamps.size());
		msg += " time stamps stored, while at least 2 time stamps should be recorded.";
		SC_REPORT_ERROR("KisTA",msg.c_str());
	}
	
	// look for the max time span and fills the time span vector
	max_gap = time_stamps[1]-time_stamps[0];
	time_spans.push_back(max_gap);  // first gap loaded into the vector
	for(unsigned int i=2; i < time_stamps.size(); i++) {
		cur_gap = time_stamps[i]-time_stamps[i-1];
		time_spans.push_back(cur_gap); // fills the time spans vector
		if(cur_gap>max_gap) max_gap = cur_gap;
	}
	span_time_gap = max_gap / time_stamps_max_separation;
//cout << "max gap: " << max_gap << endl; 		
//cout << "span_time gap: " << span_time_gap << endl; 	
	// now calculates the spans with the recently calculated threshold
	
	// calculate the spans vector and the x_location vectors for representation
	// ---------------------------------------------------------------------
	x_location.push_back(0);
	for(unsigned int i=1; i < time_stamps.size(); i++) {
		cur_gap = time_stamps[i]-time_stamps[i-1];
		normalized_distance = cur_gap/span_time_gap;
//cout << i-1 << ":" << normalized_distance << endl;
		span_value = normalized_distance+1; // directly truncates ...
		if(span_value>time_stamps_max_separation) span_value = time_stamps_max_separation;
		span_vec.push_back(span_value);
		x_location.push_back(x_location.back() + span_value);
	}
	
	cout << "SPAN VECTOR:" << endl;
	for(unsigned int i=0; i < span_vec.size(); i++) {
		cout << i << ":" << span_vec[i] << endl;
	}

	cout << "X_LOCATION VECTOR:" << endl;
	for(unsigned int i=0; i < x_location.size(); i++) {
		cout << i << ":" << x_location[i] << endl;
	}
	
	// Now, it does the actual export to the TiKZ file
	this->draw_start();
	if(clustered_style) {		
		if(compact_style) {			
			this->draw_clustered_compact();
		} else {		
			this->draw_clustered_unfolded();
		}
	} else {		
		this->draw_flat_all();
	}
	this->draw_end();
}
		
void tikz_activity_trace::draw_start() {
	// creating a trace file (associated to the scheduler)	
				
	tikz_trace_file = new ofstream(ttikz_file_name);
	
	if(ttikz_trace_landscape) {
		*tikz_trace_file << "\\documentclass[landscape,a4paper,10pt]{article}" << endl;
	} else {
		*tikz_trace_file << "\\documentclass[a4paper,10pt]{article}" << endl;
	}
	
	*tikz_trace_file << "\\usepackage{tikz}" << endl;
	*tikz_trace_file << "\\usepackage{verbatim}" << endl;
	// Set geometry package. 
	// If the user has set the landscape format option, then the setting is performed in the geometry
	// package load
	if(ttikz_trace_landscape) {
		*tikz_trace_file << "\\usepackage[landscape,margin=15mm]{geometry}" << endl;
	} else {
		*tikz_trace_file << "\\usepackage[margin=15mm]{geometry}" << endl;
	}
	//
	*tikz_trace_file << "\\usetikzlibrary{shapes,arrows,fit,calc,positioning}" << endl;
	
	*tikz_trace_file << "\\begin{document}" << endl;
	
	// Print header (unless only image option is enabled)
	if(!only_image_flag) {
		*tikz_trace_file << "File created by KisTA library" << endl << endl;
		*tikz_trace_file << "Author: F.Herrera" << endl << endl;
		*tikz_trace_file << "Institution: KTH (2012-2014, Jul), Univ. of Cantabria (from 2014, Sept.)" << endl << endl;
		*tikz_trace_file << "Rights reserved by the authors in the terms defined by the KisTA license." << endl << endl;
		*tikz_trace_file << "KisTA library compilation date: " << __DATE__ << " at " << __TIME__ << "." << endl << endl;
		*tikz_trace_file << "Tracing Options used:" << endl;
		
		*tikz_trace_file <<  "Style=";
		if(clustered_style) *tikz_trace_file <<  "Clustered";
		else *tikz_trace_file <<  "Flat";
		*tikz_trace_file <<  ", ";
		
		if(compact_style) *tikz_trace_file <<  "Compact";
		else *tikz_trace_file <<  "Unfolded";
		*tikz_trace_file <<  "; ";
		
		*tikz_trace_file <<  "Baselines sep.=";
		*tikz_trace_file << std::to_string(base_lines_separation);
		*tikz_trace_file <<  "; ";

		*tikz_trace_file <<  "Max. t span=";
		*tikz_trace_file << std::to_string(time_stamps_max_separation);
		*tikz_trace_file <<  "; ";
		
		*tikz_trace_file <<  "Scale:=";
		*tikz_trace_file << std::to_string(ttikz_trace_scale);
		*tikz_trace_file <<  "; ";

		if(ttikz_trace_landscape) *tikz_trace_file <<  "Landscape";
		else *tikz_trace_file <<  "Portrait";
		*tikz_trace_file <<  "; ";

		if(show_schedulers_activity) *tikz_trace_file <<  "Show sched. activity.";
		else *tikz_trace_file <<  "Do not show sched. activity.";
		
		*tikz_trace_file  << endl << "\\\\";
		*tikz_trace_file  << endl;
	}
	
	// Start to declare the TikZ picture
	// ------------------------------------------------------------
	
	//*tikz_trace_file << "\\begin{tikzpicture}[xscale=1,transform shape]" << endl;
	*tikz_trace_file << "\\begin{tikzpicture}[scale=";
	*tikz_trace_file << std::to_string(ttikz_trace_scale);
	*tikz_trace_file << "]" << endl;
	
	draw_time_axis();

}

void tikz_activity_trace::draw_time_axis() {

	std::string msg;
	unsigned int x_loc;
	// -----------------------------------------------------------------------
	// draw time (x) axis
	// ------------------------------------------------------------
    *tikz_trace_file << "\% Draw time axis" << endl;
    *tikz_trace_file << "\% --------------" << endl;	
	*tikz_trace_file << "\\draw [-latex](-0.5,0) coordinate(LEFTX)-- (0,0) coordinate (ORIG) -- (";
	// Check where it would be located the last time stamps
	x_loc = x_location.back();
	if(x_loc>DEFAULT_MAX_X_LOCATION) {
		msg = "Current TiKZ export configuration involves a highest X location=";
		msg += std::to_string(x_loc);
		msg += ", which is higher than the current limit of X dimentsion of the TiKZ export (";
		msg += std::to_string(DEFAULT_MAX_X_LOCATION);
		msg += "). Reducing the number of events of the trace and/or ensuring that ";
		msg +=  "the max. separation of events in the time axis is 1 (by default) should work.";
		msg +=  " If there is a need for many events and/or many resolution in the span distinction ";
		msg +=  " (high number for set_time_stamps_max_separation), the re-compilation of KisTA is ";
		msg +=  " required with a higher value of DEFAULT_MAX_X_LOCATION. ";
		msg +=  " Notice that it will likely require higher sheet dimensions. ";
		SC_REPORT_ERROR("KisTA",msg.c_str());
	}
	*tikz_trace_file << std::to_string(x_loc+1); // notice: add 1 to extend beyond the last time stamp division
	*tikz_trace_file << ",0) coordinate(RIGHTX) node[above]{$t$};" << endl;	
	*tikz_trace_file << endl;

	// -------------------------------------------------------------------------------------------------
	// draw time axis division
	// -------------------------------------------------------------------------------------------------
	*tikz_trace_file << "\% Draw time axis divisions" << endl;
    *tikz_trace_file << "\% ------------------------" << endl;

	for(unsigned int i=0;i<time_stamps.size();i++) {
		*tikz_trace_file << "\\draw[] (";
		*tikz_trace_file << std::to_string(x_location[i]);
		*tikz_trace_file << ",0.2) -- (";
		*tikz_trace_file << std::to_string(x_location[i]);
		*tikz_trace_file << ",-0.2)  node[below]{$t_{";
		*tikz_trace_file << std::to_string(i);
		*tikz_trace_file << "}$};" << endl;
	}
	*tikz_trace_file << endl;

/*
 	*tikz_trace_file << "\\foreach \\tx in{0,...,";
	*tikz_trace_file << time_stamps.size()-1;
	*tikz_trace_file << "} {" << endl;
	*tikz_trace_file << "\\draw[] (\\tx,0.2) -- (\\tx,-0.2)  node[below]{$t_{\\tx}$};" << endl;
	*tikz_trace_file << "}" << endl;		
	*tikz_trace_file << endl;
*/	
}

void tikz_activity_trace::draw_y_axis_flat() {
	if(show_schedulers_activity) {
		// In case scheduler activity is drawn, it is placed at the bottom of the sketch
		for(auto it_sched = sched_activity.begin(); it_sched != sched_activity.end(); ++it_sched ) {
			*tikz_trace_file << "-- ++(0,";
			*tikz_trace_file << std::to_string(base_lines_separation);
			*tikz_trace_file << ") coordinate(";
			*tikz_trace_file << it_sched->first->name(); //scheduler name
			*tikz_trace_file << "e0) ";
		}
	}

	// y references for tasks
	for(auto it = task_activity.begin(); it != task_activity.end(); ++it ) {
		*tikz_trace_file << "-- ++(0,";
		*tikz_trace_file << std::to_string(base_lines_separation);
		*tikz_trace_file << ") coordinate(";
		*tikz_trace_file << it->first->name();
		*tikz_trace_file << "e0) ";
	}
	*tikz_trace_file << " -- ++(0,1) coordinate(YTOP);" << endl;
	*tikz_trace_file <<  endl;

}

void tikz_activity_trace::draw_y_axis_clustered_unfolded(scheduler *sched, unsigned int cluster_number) {
	
	taskset_by_name_t *assigned_tasks;
	
	*tikz_trace_file << "\\draw [dashed,thick] ";
	
	// In case scheduler activity is drawn, it is placed at the bottom of the group/cluster
	// so "before" (lower Y axis) than the tasks
	if(show_schedulers_activity) {
		if(cluster_number==0) {
			*tikz_trace_file << " (ORIG) -- (0,1)";
		} else {
			*tikz_trace_file << " (CLUSTER_";
			*tikz_trace_file << std::to_string(cluster_number-1);
			*tikz_trace_file << "_YTOP) ";
			*tikz_trace_file << "-- ++(0,";
			*tikz_trace_file << std::to_string(base_lines_separation);
			*tikz_trace_file << ") " << endl;
		}
		*tikz_trace_file << "coordinate(";
		*tikz_trace_file << sched->name(); //scheduler name
		*tikz_trace_file << "e0) ";
	}
		
	assigned_tasks = sched->gets_tasks_assigned();

	taskset_by_name_t::iterator it;

	for(it = assigned_tasks->begin(); it != assigned_tasks->end(); ++it ) {	
		if( it == assigned_tasks->begin() && !show_schedulers_activity )  {
			if( (cluster_number==0) ) {
				// first cluster, and scheduler not shown -> first baseline
				*tikz_trace_file << " (ORIG) -- (0,1)";
			} else {
				// no first cluster, and scheduler not shown -> start n-th cluster
				*tikz_trace_file << " (CLUSTER_";
				*tikz_trace_file << std::to_string(cluster_number-1);
				*tikz_trace_file << "_YTOP) -- ++(0,";
				*tikz_trace_file << std::to_string(base_lines_separation);
				*tikz_trace_file << ") " << endl;
			}
		} else {
			// no first cluster, "normal" separation stated by base_lines_separation
			*tikz_trace_file << " -- ++(0,";
			*tikz_trace_file << std::to_string(base_lines_separation);
			*tikz_trace_file << ")" << endl;
		}
		// assign coordinate name
		*tikz_trace_file << " coordinate(";
		*tikz_trace_file << it->first;
		*tikz_trace_file << "e0) ";
	}
	// adds cluster separation and assigns a name to the coordinate of the end of cluster
	*tikz_trace_file << " -- ++(0,1) coordinate(CLUSTER_";
	*tikz_trace_file << std::to_string(cluster_number);
	*tikz_trace_file << "_YTOP);" << endl;
	*tikz_trace_file <<  endl;
}


void tikz_activity_trace::draw_y_axis_clustered_compact(scheduler *sched, unsigned int cluster_number) {

	*tikz_trace_file << "\\draw [dashed,thick] ";
	
	// Starts from the bottom of the group/cluster
	// Grouped from scheduler assignation (regardless if scheduler activity is shown)
	if(cluster_number==0) {
		*tikz_trace_file << " (ORIG) -- (0,1)";
	} else {
		*tikz_trace_file << " (CLUSTER_";
		*tikz_trace_file << std::to_string(cluster_number-1);
		*tikz_trace_file << "_YTOP) ";
		*tikz_trace_file << "-- ++(0,1) ";
	}
	*tikz_trace_file << "coordinate(";
	*tikz_trace_file << sched->name(); //scheduler name
	*tikz_trace_file << "e0) ";
	
	// adds cluster separation and assigns a name to the coordinate of the end of cluster
	//*tikz_trace_file << " -- ++(0,1) coordinate(CLUSTER_";
	// lesser cluster separation because using compact format
	*tikz_trace_file << " -- ++(0,0.5) coordinate(CLUSTER_";
	*tikz_trace_file << std::to_string(cluster_number);
	*tikz_trace_file << "_YTOP);" << endl;
	*tikz_trace_file <<  endl;
}

void tikz_activity_trace::draw_y_axis_clustered() {
	unsigned int cluster_idx;   

	cluster_idx = 0;	
	for(auto it_sched = sched_activity.begin(); it_sched != sched_activity.end(); ++it_sched ) {
		if(compact_style) {						
			draw_y_axis_clustered_compact(it_sched->first, cluster_idx);
		} else {		
			draw_y_axis_clustered_unfolded(it_sched->first, cluster_idx);
		}
		cluster_idx++;
	}

	// adds final YTOP coordinate and assigns it a name
	*tikz_trace_file << "\\draw [dashed,thick] (CLUSTER_";
	*tikz_trace_file << std::to_string(cluster_idx-1);
	*tikz_trace_file << "_YTOP) -- ++(0,1) coordinate(YTOP);" << endl;
	*tikz_trace_file <<  endl;
}

void tikz_activity_trace::draw_baselines() {
	if(show_schedulers_activity) {
		for(auto it_sched = sched_activity.begin(); it_sched != sched_activity.end(); ++it_sched ) {
			*tikz_trace_file << "\\draw [thick] (LEFTX|-";
			*tikz_trace_file << it_sched->first->name();
			*tikz_trace_file << "e0)";
			*tikz_trace_file << " node[above left]{";
			*tikz_trace_file << it_sched->first->name();
	//	Here, a short alias, or a shortened name by cuting the sched name can be
	//  put, e.g. for automatic task alias:
	//		*tikz_trace_file << "T";
	//		*tikz_trace_file << std::to_string(task_position[it_sched->first]);
			*tikz_trace_file << "} -- (";
			*tikz_trace_file << it_sched->first->name();
			*tikz_trace_file << "e0";
			*tikz_trace_file << "-|RIGHTX);" << endl;
			*tikz_trace_file << endl;			
		}
	}
	
	if(!compact_style) { 
		for(auto it = task_activity.begin(); it != task_activity.end(); ++it ) {	
			*tikz_trace_file << "\\draw [thick] (LEFTX|-";
			*tikz_trace_file << it->first->name();
			*tikz_trace_file << "e0)";
			*tikz_trace_file << " node[above left]{";
			*tikz_trace_file << it->first->name();
	//	Here, a short alias, or a shortened name by cuting the taske name can be
	//  put, e.g. for automatic task alias:
	//		*tikz_trace_file << "T";
	//		*tikz_trace_file << std::to_string(task_position[it->first]);
			*tikz_trace_file << "} -- (";
			*tikz_trace_file << it->first->name();
			*tikz_trace_file << "e0";
			*tikz_trace_file << "-|RIGHTX);" << endl;
			*tikz_trace_file << endl;
		}
	}
}

/*
 * 	if(show_schedulers_activity) {
		for(auto sched_it = scheds_by_name.begin(); sched_it != scheds_by_name.end(); ++sched_it ) {
			*tikz_trace_file << "\\draw [thick] (LEFTX|-";
			*tikz_trace_file << sched_it->first;
			*tikz_trace_file << "e0)";
			*tikz_trace_file << " node[above left]{";
			*tikz_trace_file << sched_it->first;
		//	Here, a short alias, or a shortened name by cuting the sched name can be
		//  put, e.g. for automatic task alias:
		//		*tikz_trace_file << "T";
		//		*tikz_trace_file << std::to_string(task_position[it_sched->first]);
			*tikz_trace_file << "} -- (";
			*tikz_trace_file << sched_it->first;
			*tikz_trace_file << "e0";
			*tikz_trace_file << "-|RIGHTX);" << endl;
			*tikz_trace_file << endl;
			
			for(auto it = sched_it->second->gets_tasks_assigned()->begin(); it != sched_it->second->gets_tasks_assigned()->end(); ++it ) {	
				*tikz_trace_file << "\\draw [thick] (LEFTX|-";
				*tikz_trace_file << it->first;
				*tikz_trace_file << "e0)";
				*tikz_trace_file << " node[above left]{";
				*tikz_trace_file << it->first;
		//	Here, a short alias, or a shortened name by cuting the taske name can be
		//  put, e.g. for automatic task alias:
		//		*tikz_trace_file << "T";
		//		*tikz_trace_file << std::to_string(task_position[it->first]);
				*tikz_trace_file << "} -- (";
				*tikz_trace_file << it->first;
				*tikz_trace_file << "e0";
				*tikz_trace_file << "-|RIGHTX);" << endl;
				*tikz_trace_file << endl;
			}	
					
		}

	}
 * */

unsigned int tikz_activity_trace::get_span(unsigned int cur_index, unsigned int next_index) {
	unsigned int span;
	/*span = 0;
	for(unsigned int i=cur_index; i<next_index;i++) {
		span += span_vec[i];
	}*/
	// mor efficient
	span = x_location[next_index] - x_location[cur_index];
	return span;
}	
	 
void tikz_activity_trace::write_sched_activity_path(scheduler *sched, sched_states_table_t *sched_states_table_p) {

	std::string 		msg;
	sc_time				ev_time, next_ev_time;	
	unsigned int		ev_index, prev_index;
	unsigned int		x_span;
	
	*tikz_trace_file << "\\path ";
			
	auto ev_it = sched_states_table_p->begin();

	*tikz_trace_file << "(";
	*tikz_trace_file << sched->name();
	*tikz_trace_file << "e"; 
	// ev_it-> first = time stamp of the state change event
	ev_time = ev_it->first;
	ev_index = time_stamps_index[ev_time];
			
	// In this call, this index should be 0 and the time stamp the configured 
	// start time, so it is actually checked
	if(ev_time!=ttikz_start_trace_time) {
		msg = "Unexpected situation. First events in the activity record of scheduler ";
		msg += sched->name();
		msg += " for TiKZ tracing have time stamp (";
		msg += ev_time.to_string();
		msg += "), different from the configured time for recording start (";
		msg += ttikz_start_trace_time.to_string();
		msg += ").";
		SC_REPORT_ERROR("KisTA", msg.c_str());
	}

	if(ev_index!=0) {
		msg = "Unexpected situation. First events in the activity record of scheduler ";
		msg += sched->name();
		msg += " for TiKZ tracing have an index (";
		msg += std::to_string(ev_index);
		msg += ") different from 0.";
		SC_REPORT_ERROR("KisTA", msg.c_str());
	}
	// everything seems fine, so we can continue stating the first coordinate...
	*tikz_trace_file << std::to_string( ev_index );
	*tikz_trace_file << ")";
			
	// and then we can start to state the actual remaining event coordinates
	ev_it++;
								
	// generates the coordinate name: The convention is to add e[i]
	// where [i] is the index of the time stamp in the activity record
	// The first one will be always e0
	while(ev_it != sched_states_table_p->end()) {
		prev_index = ev_index;
		ev_time = ev_it->first;
		ev_index = time_stamps_index[ev_time];
		*tikz_trace_file << " ++ (";
		x_span = get_span(prev_index,ev_index);
		*tikz_trace_file << std::to_string(x_span);
		*tikz_trace_file << ",0) coordinate(";
		*tikz_trace_file << sched->name();
		*tikz_trace_file << "e"; 
		*tikz_trace_file << std::to_string( ev_index );
		*tikz_trace_file << ")";		
		ev_it++;
	}
			
	// if the last change event is not the last time stamp,
	// then a coordinate with such last event time is generated
	if(ev_time != time_stamps.back()) { // notice that here, time stamps
									  // has been already filled
		prev_index = ev_index; 
		ev_index = time_stamps_index.rbegin()->second; // rbegin = iterator to the end
		*tikz_trace_file << " ++ (";
		x_span = get_span(prev_index,ev_index);
		*tikz_trace_file << std::to_string(x_span);
		*tikz_trace_file << ",0) coordinate(";
		*tikz_trace_file << sched->name();
		*tikz_trace_file << "e"; 
		*tikz_trace_file << std::to_string( ev_index );
		*tikz_trace_file << ")";
	} 
	*tikz_trace_file << ";" << endl;
}


void tikz_activity_trace::write_task_activity_path(task_info_t *task_info, task_states_table_t	*task_states_table_p) {
	
	std::string 		msg;
	sc_time				ev_time, next_ev_time;	
	unsigned int		ev_index, prev_index;
	unsigned int		x_span;
		
	*tikz_trace_file << "\\path ";
		
	auto ev_it = task_states_table_p->begin();

	*tikz_trace_file << "(";
	*tikz_trace_file << task_info->name();
	*tikz_trace_file << "e"; 
	// ev_it-> first = time stamp of the state change event
	ev_time = ev_it->first;
	ev_index = time_stamps_index[ev_time];
		
	// In this call, this index should be 0 and the time stamp the configured 
	// start time, so it is actually checked
	if(ev_time!=ttikz_start_trace_time) {
		msg = "Unexpected situation. First events in the activity record of task ";
		msg += task_info->name();
		msg += " for TiKZ tracing have time stamp (";
		msg += ev_time.to_string();
		msg += "), different from the configured time for recording start (";
		msg += ttikz_start_trace_time.to_string();
		msg += ").";
		SC_REPORT_ERROR("KisTA", msg.c_str());
	}

	if(ev_index!=0) {
		msg = "Unexpected situation. First events in the activity record of task ";
		msg += task_info->name();
		msg += " for TiKZ tracing have index (";
		msg += std::to_string(ev_index);
		msg += ") different from 0.";
		SC_REPORT_ERROR("KisTA", msg.c_str());
	}
	// everything seems fine, so we can continue stating the first coordinate...
	*tikz_trace_file << std::to_string( ev_index );
	*tikz_trace_file << ")";
		
	// and then we can start to state the actual remaining event coordinates
	ev_it++;
							
	// generates the coordinate name: The convention is to add e[i]
	// where [i] is the index of the time stamp in the activity record
	// The first one will be always e0
	while(ev_it != task_states_table_p->end()) {
		prev_index = ev_index;
		ev_time = ev_it->first;
		ev_index = time_stamps_index[ev_time];
		*tikz_trace_file << " ++ (";
		x_span = get_span(prev_index,ev_index);
		*tikz_trace_file << std::to_string(x_span);
		*tikz_trace_file << ",0) coordinate(";
		*tikz_trace_file << task_info->name();
		*tikz_trace_file << "e"; 
		*tikz_trace_file << std::to_string( ev_index );
		*tikz_trace_file << ")";
		ev_it++;
	}
		
	// if the last change event is not the last time stamp,
	// then a coordinate with such last event time is generated
	if(ev_time != time_stamps.back()) { // notice that here, time stamps
		                                  // has been already filled
	    prev_index = ev_index; 
	    ev_index = time_stamps_index.rbegin()->second; // iterator to the end
	    *tikz_trace_file << " ++ (";
	    x_span = get_span(prev_index,ev_index);
		*tikz_trace_file << std::to_string(x_span);
		*tikz_trace_file << ",0) coordinate(";
		*tikz_trace_file << task_info->name();
		*tikz_trace_file << "e"; 
		*tikz_trace_file << std::to_string( ev_index );
		*tikz_trace_file << ")";
	} 
	*tikz_trace_file << ";" << endl;
}

void tikz_activity_trace::write_compact_cluster_activity_path(scheduler *sched, compact_states_table_t *compact_states_table_p) {
	std::string 		msg;
	sc_time				ev_time, next_ev_time;	
	unsigned int		ev_index, prev_index;
	unsigned int		x_span;

	*tikz_trace_file << "\\path ";
			
	auto ev_it = compact_states_table_p->begin();

	*tikz_trace_file << "(";
	*tikz_trace_file << sched->name();
	*tikz_trace_file << "e";
	// ev_it-> first = time stamp of the state change event
	ev_time = ev_it->first;
	ev_index = time_stamps_index[ev_time];
			
	// In this call, this index should be 0 and the time stamp the configured 
	// start time, so it is actually checked
	if(ev_time!=ttikz_start_trace_time) {
		msg = "Unexpected situation. First events in the compact activity record of scheduler ";
		msg += sched->name();
		msg += " for TiKZ tracing have time stamp (";
		msg += ev_time.to_string();
		msg += "), different from the configured time for recording start (";
		msg += ttikz_start_trace_time.to_string();
		msg += ").";
		SC_REPORT_ERROR("KisTA", msg.c_str());
	}

	if(ev_index!=0) {
		msg = "Unexpected situation. First events in the compact activity record of scheduler ";
		msg += sched->name();
		msg += " for TiKZ tracing have an index (";
		msg += std::to_string(ev_index);
		msg += ") different from 0.";
		SC_REPORT_ERROR("KisTA", msg.c_str());
	}
	// everything seems fine, so we can continue stating the first coordinate...
	*tikz_trace_file << std::to_string( ev_index );
	*tikz_trace_file << ")";
			
	// and then we can start to state the actual remaining event coordinates
	ev_it++;
								
	// generates the coordinate name: The convention is to add e[i]
	// where [i] is the index of the time stamp in the activity record
	// The first one will be always e0
	while(ev_it != compact_states_table_p->end()) {
		prev_index = ev_index;
		ev_time = ev_it->first;
		ev_index = time_stamps_index[ev_time];
		*tikz_trace_file << " ++ (";
		x_span = get_span(prev_index, ev_index);
		*tikz_trace_file << std::to_string(x_span);
		*tikz_trace_file << ",0) coordinate(";
		*tikz_trace_file << sched->name();
		*tikz_trace_file << "e"; 
		*tikz_trace_file << std::to_string( ev_index );
		*tikz_trace_file << ")";
		ev_it++;
	}
			
	// if the last change event is not the last time stamp,
	// then a coordinate with such last event time is generated
	if(ev_time != time_stamps.back()) { // notice that here, time stamps
									  // has been already filled
		prev_index = ev_index; 
		ev_index = time_stamps_index.rbegin()->second; // rbegin = iterator to the end
		*tikz_trace_file << " ++ (";
		x_span = get_span(prev_index, ev_index);
		*tikz_trace_file << std::to_string(x_span);
		*tikz_trace_file << ",0) coordinate(";
		*tikz_trace_file << sched->name();
		*tikz_trace_file << "e"; 
		*tikz_trace_file << std::to_string( ev_index );
		*tikz_trace_file << ")";
	} 
	*tikz_trace_file << ";" << endl;
}


void tikz_activity_trace::draw_task_activity(task_info_t *task_info, task_states_table_t	 *task_states_table_p) {

	std::string 		msg;
	sc_time				ev_time, next_ev_time;	
	unsigned int		ev_index, next_index;
	unsigned int		x_span;
	task_state_t		task_state;
	
	auto ev_it = task_states_table_p->begin();
					
	while(ev_it != task_states_table_p->end()) {
					//prev_index = ev_index;

		ev_time = ev_it->first;
		ev_index = time_stamps_index[ev_time];
		task_state = ev_it->second;	
						
		auto next_ev_it = ev_it;
		next_ev_it++;
		next_ev_time = next_ev_it->first;
		next_index = time_stamps_index[next_ev_time];

		if( next_ev_it == task_states_table_p->end() ) {
			// there is no next event, so...
			if(ev_time == time_stamps.back()) {
				// the last event is the same stamp in the time axis
				// depicting another box is not necessary
				break;
			} else if(ev_time < time_stamps.back()) {
				// calculate x_span to extend the rectangle till the last index
				x_span = get_span(ev_index, time_stamps.size()-1);
			} else {
				SC_REPORT_ERROR("KisTA","Unexpected situation in TiKZ task activity.");
			}
		} else {
			// rectangle has to be drawn till the next coordinate
			x_span =  get_span(ev_index, next_index);
		}
			
		// Now, we have to draw a rectangle per event pair (by default)
		// or at least for those cases where the state passes to be executing
		*tikz_trace_file << "\\begin{scope}[shift={";
		// reference coordinate
		*tikz_trace_file << "(";
		*tikz_trace_file << task_info->name();
		*tikz_trace_file << "e";
		*tikz_trace_file << std::to_string( ev_index );
		*tikz_trace_file << ")";	
		*tikz_trace_file << "}]" << endl;	

		// draw rectangle
		if(show_unactive_boxes) {
			// draw always boxes, using the filling color to encode tasks states
			*tikz_trace_file << "\\draw[";
			switch(task_state) {
				case EXECUTING:
					// no filling at all
					break;
				case READY:			
					*tikz_trace_file << "fill=black!10"; // lighter grey for ready state
					break;
				default: // remaining states refer to blocking states of the task
						  // and are shown in darker grey
					*tikz_trace_file << "fill=black!30"; // lighter grey for ready state				
			}
			*tikz_trace_file << "] (0,0) rectangle (";
			*tikz_trace_file << std::to_string(x_span);
			*tikz_trace_file << ",0.7);" << endl;
		} else {
			// draw boxes only for the running state
			if(task_state==EXECUTING) {
				*tikz_trace_file << "\\draw[] (0,0) rectangle (";
				*tikz_trace_file << std::to_string(x_span);
				*tikz_trace_file << ",0.7);" << endl;
			}
		}

		// draw state(if they have not been disabled)
		// note: has to be done after drawing state to avoid the rectangle fill
		//       hides it
		if(show_text_in_traces) {
			*tikz_trace_file << "\\draw[] (0,0) node[rectangle, above right] {";
			*tikz_trace_file << task_state;
			*tikz_trace_file << "}";
			*tikz_trace_file << ";" << endl;
		}
			
		*tikz_trace_file << "\\end{scope}" << endl;
			
		ev_it++;
	}		
}

void tikz_activity_trace::draw_sched_activity(scheduler *sched, sched_states_table_t *sched_states_table_p) {

	std::string 		msg;
	sc_time				ev_time, next_ev_time;	
	unsigned int		ev_index, next_index;
	unsigned int		x_span;
	sched_state_t		sched_state;
		
	auto ev_it = sched_states_table_p->begin();
						
	while(ev_it != sched_states_table_p->end()) {
					//prev_index = ev_index;

		ev_time = ev_it->first;
		ev_index = time_stamps_index[ev_time];
		sched_state = ev_it->second;	
							
		auto next_ev_it = ev_it;
		next_ev_it++;
		next_ev_time = next_ev_it->first;
		next_index = time_stamps_index[next_ev_time];

		if( next_ev_it == sched_states_table_p->end() ) {
			// there is no next event, so...
			if(ev_time == time_stamps.back()) {
				// the last event is the same stamp in the time axis
				// depicting another box is not necessary
				break;
			} else if(ev_time < time_stamps.back()) {
				// calculate x_span to extend the rectangle till the last index
				x_span = get_span(ev_index,time_stamps.size()-1);
			} else {
				SC_REPORT_ERROR("KisTA","Unexpected situation while drawing TiKZ scheduler activity.");
			}
		} else {
			// rectangle has to be drawn till the next coordinate
			x_span =  get_span(ev_index, next_index);
		}
		
		// Now, we have to draw a rectangle per event pair (by default)
		// or at least for those cases where the state passes to be executing
		*tikz_trace_file << "\\begin{scope}[shift={";
		// reference coordinate
		*tikz_trace_file << "(";
		*tikz_trace_file << sched->name();
		*tikz_trace_file << "e";
		*tikz_trace_file << std::to_string( ev_index );
		*tikz_trace_file << ")";	
		*tikz_trace_file << "}]" << endl;	

		if(show_unactive_boxes) {
			// draw always boxes, using the filling color to encode schedulers states
			*tikz_trace_file << "\\draw[";
			switch(sched_state) {
				case SCHEDULING:
					// no filling at all
					break;
				default: // INACTIVE: dark grey 
					*tikz_trace_file << "fill=black!30"; // lighter grey for ready state				
			}
			*tikz_trace_file << "] (0,0) rectangle (";
			*tikz_trace_file << std::to_string(x_span);
			*tikz_trace_file << ",0.7);" << endl;
		} else {	
			// draw boxes only for the running state
			if(sched_state==SCHEDULING) {
				*tikz_trace_file << "\\draw[] (0,0) rectangle (";
				*tikz_trace_file << std::to_string(x_span);
				*tikz_trace_file << ",0.7);" << endl;
			}
		}
			
		if(show_text_in_traces) {
			// draw scheduler state (in case it is not disabled)
			// note: has to be done after drawing state to avoid the rectangle fill
			//       hides it
			*tikz_trace_file << "\\draw[] (0,0) node[rectangle, above right] {";
			*tikz_trace_file << sched_state;
			*tikz_trace_file << "}";
			*tikz_trace_file << ";" << endl;
		}				
		
		*tikz_trace_file << "\\end{scope}" << endl;
		
		ev_it++;
	}	
}

void tikz_activity_trace::draw_compact_cluster_activity(scheduler *sched,  compact_states_table_t *compact_states_table_p) {
	
	std::string 		msg;
	sc_time				ev_time, next_ev_time;	
	unsigned int		ev_index, next_index;
	unsigned int		x_span;
	compact_state_t		compact_state;
		
	auto ev_it = compact_states_table_p->begin();
						
	while(ev_it != compact_states_table_p->end()) {
					//prev_index = ev_index;

		ev_time = ev_it->first;
		ev_index = time_stamps_index[ev_time];
		compact_state = ev_it->second;	
							
		auto next_ev_it = ev_it;
		next_ev_it++;
		next_ev_time = next_ev_it->first;
		next_index = time_stamps_index[next_ev_time];

		if( next_ev_it == compact_states_table_p->end() ) {
			// there is no next event, so...
			if(ev_time == time_stamps.back()) {
				// the last event is the same stamp in the time axis
				// depicting another box is not necessary
				break;
			} else if(ev_time < time_stamps.back()) {
				// calculate x_span to extend the rectangle till the last index
				x_span = get_span(ev_index, time_stamps.size()-1);
			} else {
				SC_REPORT_ERROR("KisTA","Unexpected situation while drawing TiKZ  compact scheduler activity.");
			}
		} else {
			// rectangle has to be drawn till the next coordinate
			x_span = get_span(ev_index, next_index);
		}
		
		// Now, we have to draw a rectangle per event pair (by default)
		// or at least for those cases where the state passes to be executing
		*tikz_trace_file << "\\begin{scope}[shift={";
		// reference coordinate
		*tikz_trace_file << "(";
		*tikz_trace_file << sched->name();
		*tikz_trace_file << "e";
		*tikz_trace_file << std::to_string( ev_index );
		*tikz_trace_file << ")";	
		*tikz_trace_file << "}]" << endl;	

		if(show_unactive_boxes) {
			// draw always boxes, using the filling color to encode schedulers states
			*tikz_trace_file << "\\draw[";
			if(compact_state==SCHEDULING ) {
				*tikz_trace_file << "fill=black!10"; // light grey for scheduling
			}
			if(compact_state=='X') {
				*tikz_trace_file << "fill=black!30"; // dark grey for no activity
			}			
			// otherwise INACTIVE: no filling at all
			*tikz_trace_file << "] (0,0) rectangle (";
			*tikz_trace_file << std::to_string(x_span);
			*tikz_trace_file << ",0.7);" << endl;
		} else {	
			// draw boxes only for the running states
			if(compact_state!='X') {
				*tikz_trace_file << "\\draw[] (0,0) rectangle (";
				*tikz_trace_file << std::to_string(x_span);
				*tikz_trace_file << ",0.7);" << endl;
			}
		}
			
		if(show_text_in_traces) {
			// draw scheduler state (in case it is not disabled)
			// note: has to be done after drawing state to avoid the rectangle fill
			//       hides it
			*tikz_trace_file << "\\draw[] (0,0) node[rectangle, above right] {";
			switch(compact_state) {
				case SCHEDULING:
					break;
				case 'X':
					break;
				default:
					*tikz_trace_file << "T";
			}
			*tikz_trace_file << compact_state;
			*tikz_trace_file << "}";
			*tikz_trace_file << ";" << endl;
		}				
		
		*tikz_trace_file << "\\end{scope}" << endl;
		
		ev_it++;
	}	
}

void tikz_activity_trace::draw_flat_all() {
	
	std::string msg;
	
	// -----------------------------------------------------------------------
	// draw y axis
	// ------------------------------------------------------------
    *tikz_trace_file << "\% Draw y axis" << endl;
    *tikz_trace_file << "\% --------------" << endl;	
	//*tikz_trace_file << "\\draw [dashed,thick] (O1) -- (0,1) coordinate(T1) -- (0,3) coordinate(S2) -- (0,5) coordinate(e3) -- ++(0,1)coordinate(ff2);" << endl;
	*tikz_trace_file << "\\draw [dashed,thick] (ORIG) ";
	draw_y_axis_flat();

	// -----------------------------------------------------------------------
    // draw baselines
    // -----------------------------------------------------------------------
    *tikz_trace_file << "\% Draw baselines" << endl;
    *tikz_trace_file << "\% --------------" << endl;
	draw_baselines();

	// -------------------------------------------------------------------------------------------------
	// define coordinates
	// -------------------------------------------------------------------------------------------------
	*tikz_trace_file << "\% Set event coordinates   " << endl;
    *tikz_trace_file << "\% ------------------------" << endl;

    // for schedulers activity    
    if(show_schedulers_activity) {
		 for(auto it_sched = sched_activity.begin(); it_sched != sched_activity.end(); ++it_sched ) {
			// retrieves the pointer to the task states table
			write_sched_activity_path(it_sched->first,it_sched->second);
		}
	}
	
    // for tasks activity
    for(auto it_task = task_activity.begin(); it_task != task_activity.end(); ++it_task ) {
		// retrieves the pointer to the task states table
		write_task_activity_path(it_task->first, it_task->second);		
	}
	*tikz_trace_file << endl;
	
	// -----------------------------------------------------------------------------------------------------
	// draw activity
	// -----------------------------------------------------------------------------------------------------

	// draw scheduler activity
	// ------------------------
	if(show_schedulers_activity) {
		*tikz_trace_file << "\% Draw scheduler activity (relying on event coordinates) " << endl;
		*tikz_trace_file << "\% -------------------------------------------------------" << endl;

		auto it_sched = sched_activity.begin();
		
		while(it_sched!= sched_activity.end()) {
			draw_sched_activity(it_sched->first,it_sched->second);
			it_sched++;
		}
	}

	// draw task activity
	// ----------------------
	*tikz_trace_file << "\% Draw task activity (relying on event coordinates) " << endl;
    *tikz_trace_file << "\% --------------------------------------------------" << endl;

	auto it_task = task_activity.begin();
	
    while(it_task!= task_activity.end()) {
		draw_task_activity(it_task->first, it_task->second);
		it_task++;
	}
}



void tikz_activity_trace::draw_clustered_unfolded() {

	// -----------------------------------------------------------------------
	// draw y axis
	// ------------------------------------------------------------
    *tikz_trace_file << "\% Draw y axis" << endl;
    *tikz_trace_file << "\% --------------" << endl;	
	   // iterate on the task set to define the coordinates of the tasks
	draw_y_axis_clustered();
		
	// -----------------------------------------------------------------------
    // draw baselines
    // -----------------------------------------------------------------------
    *tikz_trace_file << "\% Draw baselines" << endl;
    *tikz_trace_file << "\% --------------" << endl;
	draw_baselines();

	// -------------------------------------------------------------------------------------------------
	// define coordinates
	// -------------------------------------------------------------------------------------------------
	*tikz_trace_file << "\% Set event coordinates   " << endl;
    *tikz_trace_file << "\% ------------------------" << endl;

    // for schedulers activity    
	 for(auto it_sched = sched_activity.begin(); it_sched != sched_activity.end(); ++it_sched ) {
		// retrieves the pointer to the task states table
		write_sched_activity_path(it_sched->first,it_sched->second);
	}
	
	*tikz_trace_file << endl;
	
	    // for tasks activity
    for(auto it_task = task_activity.begin(); it_task != task_activity.end(); ++it_task ) {
		// retrieves the pointer to the task states table
		write_task_activity_path(it_task->first, it_task->second);		
	}
	*tikz_trace_file << endl;
	
	// -----------------------------------------------------------------------------------------------------
	// draw activity
	// -----------------------------------------------------------------------------------------------------

	// draw scheduler activity
	// ------------------------
	if(show_schedulers_activity) {
		*tikz_trace_file << "\% Draw scheduler activity (relying on event coordinates) " << endl;
		*tikz_trace_file << "\% -------------------------------------------------------" << endl;

		auto it_sched = sched_activity.begin();
		
		while(it_sched!= sched_activity.end()) {
			draw_sched_activity(it_sched->first,it_sched->second);
			it_sched++;
		}
	}

	// draw task activity
	// ----------------------
	*tikz_trace_file << "\% Draw task activity (relying on event coordinates) " << endl;
    *tikz_trace_file << "\% --------------------------------------------------" << endl;

	auto it_task = task_activity.begin();
	
    while(it_task!= task_activity.end()) {
		draw_task_activity(it_task->first, it_task->second);
		it_task++;
	}
	
}


void tikz_activity_trace::draw_clustered_compact() {

	// -----------------------------------------------------------------------
	// draw y axis
	// ------------------------------------------------------------
    *tikz_trace_file << "\% Draw y axis" << endl;
    *tikz_trace_file << "\% --------------" << endl;	
	   // iterate on the task set to define the coordinates of the tasks
	draw_y_axis_clustered();
		
	// -----------------------------------------------------------------------
    // draw baselines
    // -----------------------------------------------------------------------
    *tikz_trace_file << "\% Draw baselines" << endl;
    *tikz_trace_file << "\% --------------" << endl;
	draw_baselines();

	// -------------------------------------------------------------------------------------------------
	// define coordinates
	// -------------------------------------------------------------------------------------------------
	*tikz_trace_file << "\% Set event coordinates   " << endl;
    *tikz_trace_file << "\% ------------------------" << endl;

    // for schedulers activity    
	 for(auto it_sched = compact_activity.begin(); it_sched != compact_activity.end(); ++it_sched ) {
		// retrieves the pointer to the task states table
		write_compact_cluster_activity_path(it_sched->first,it_sched->second);
	}
	
	*tikz_trace_file << endl;
	
	// -----------------------------------------------------------------------------------------------------
	// draw activity
	// -----------------------------------------------------------------------------------------------------

	// draw scheduler activity
	// ------------------------
	*tikz_trace_file << "\% Draw scheduler activity (relying on event coordinates) " << endl;
	*tikz_trace_file << "\% -------------------------------------------------------" << endl;

	auto it_sched = compact_activity.begin();
			
	while(it_sched != compact_activity.end()) {
		draw_compact_cluster_activity(it_sched->first,it_sched->second);
		it_sched++;
	}
	
}

void tikz_activity_trace::draw_end() {
	// End of the picture
    *tikz_trace_file << "\\end{tikzpicture}" << endl;

    *tikz_trace_file << endl;
    
    // Print tail (unless only image option is enabled)
	if(!only_image_flag) {
			
		// to show all the aliases for the Tasks
		// -------------------------------------------------------
		if(compact_style) {
			*tikz_trace_file << "\% Show Automatic Task Alias names " << endl;
			*tikz_trace_file << "\% --------------------------------" << endl;
			*tikz_trace_file << "\\textbf{Task alias list (alias=task name):}\\\\" << endl;
			auto it = task_activity.begin();
			while(it!=task_activity.end()) {
				*tikz_trace_file << "T";
				*tikz_trace_file << std::to_string(it->first->kista_id);
				*tikz_trace_file << "=" << it->first->name();
				it++;
				if(it == task_activity.end()) {
					*tikz_trace_file << "\\\\" << endl;
				} else {
					*tikz_trace_file << " , " << endl;
				}
			}
			*tikz_trace_file << endl;
		}
			
		// to show all the time stamps
		// -------------------------------------------------------
		*tikz_trace_file << "\% Show Time stamp values" << endl;
		*tikz_trace_file << "\% ---------------------" << endl;
		*tikz_trace_file << "\\textbf{Time Stamp values:}\\\\" << endl;	
		*tikz_trace_file << "$";
		for(unsigned int i=0;i<time_stamps.size();i++) {
			*tikz_trace_file << "t_{";
			*tikz_trace_file << std::to_string(i);
			*tikz_trace_file << "} = ";
			*tikz_trace_file << time_stamps[i].to_string();
			*tikz_trace_file << "; ";
		}
		*tikz_trace_file << " \\\\" << endl;
		*tikz_trace_file << "$" << endl;
		
		*tikz_trace_file << endl;
		
		// to show all the time spans
		// -------------------------------------------------------
		*tikz_trace_file << "\% Show Time span values" << endl;
		*tikz_trace_file << "\% ---------------------" << endl;
		*tikz_trace_file << "\\textbf{Time Span values:}\\\\" << endl;	
		*tikz_trace_file << "$";
		for(unsigned int i=0;i<time_spans.size();i++) {
			*tikz_trace_file << "t_{";
			*tikz_trace_file << std::to_string(i+1);
			*tikz_trace_file << "} - t_{";
			*tikz_trace_file << std::to_string(i);
			*tikz_trace_file << "} = ";
			*tikz_trace_file << time_spans[i].to_string();
			*tikz_trace_file << "; ";
		}
		*tikz_trace_file << " \\\\" << endl;
		*tikz_trace_file << "$" << endl;	
		*tikz_trace_file << endl;
	}
	*tikz_trace_file << "\\end{document}" << endl;
	*tikz_trace_file << endl;
}

void tikz_activity_trace::tikz_trace_activity_process() {
	if(clustered_style && compact_style) {
		tikz_trace_activity_process_clustered_compact();
	} else {
		tikz_trace_activity_process_unfolded();
	}
}

void tikz_activity_trace::tikz_trace_activity_process_unfolded() {
	std::string msg;
	unsigned int i;
	sc_time current_time;
	task_info_t			*cur_task_info;
	task_state_t			cur_task_state;
	task_states_table_t	*task_state_table_p;
	
	scheduler*				cur_sched;
	sched_state_t  		cur_sched_state;
	task_states_table_t	*sched_state_table_p;
	
	//  the data collection start after the start configured ime
	wait(ttikz_start_trace_time);
	
	current_time = sc_time_stamp();

	// an alternative is to start the capture right after the first event
	// in the sensitivity list equal or after the threshold time
/*
 * 	while(current_time < ttikz_start_trace_time) {
		 wait();
		 current_time = sc_time_stamp();
	}
*/

	msg = "Data collection for TiKZ activity report start at ";
	msg += current_time.to_string();
	SC_REPORT_INFO("KisTA",msg.c_str());

	// stores the first time stamp and its corresponding index
	time_stamps.push_back(current_time);
	i = 0;
	time_stamps_index[current_time] = i;
	i++;
	
	// dump the initial state of the tasks amd pf tje schedulers (if they are shown)
	// in the corresponding tables even if there was no events on them!.
	// Notice that this is required because the trace
	// starts at a given time stamp, regardless if there was any event
	// at any of the tasks
	for(auto it_task = task_activity.begin(); it_task != task_activity.end(); ++it_task ) {
		cur_task_info = it_task->first;
		cur_task_state = cur_task_info->state_signal.read();
		task_state_table_p = it_task->second;
		(*task_state_table_p)[current_time] = cur_task_state;		
	}

	for(auto it_sched = sched_activity.begin(); it_sched != sched_activity.end(); ++it_sched ) {
		cur_sched = it_sched->first;
		cur_sched_state = cur_sched->state_signal.read();
		sched_state_table_p = it_sched->second;
		(*sched_state_table_p)[current_time] = cur_sched_state;
	}
	
	while(true) {
		wait();
//		cout << "TiKZ activity report event at " << sc_time_stamp() << endl;
		
		// adds a new time stamp only if the time stamp is bigger
		// and the max. amount of time stamps is respected
		current_time = sc_time_stamp();

		if(current_time>=time_stamps.back()) {
			if(current_time>time_stamps.back()) { // if there is time advance
			                                       // either the time stamps vector has
			                                       // to be grown, or the collection finished
				if(time_stamps.size()<ttikz_max_n_time_stamps) {
					// this update takes into account that several events
					// can appear at the same time stamp
					// still more time stamps can be added...
					time_stamps.push_back(current_time);
					// ... and its corresponding index
					time_stamps_index[current_time] = i;
					i++;
				} else { // the limit of time stamps has been reached
						 // and the next event exceedes the limit
						 // so we end the collection of data
					break;
				}
			}
			
			// Collect task activity:
			//  Notice that it is done also if the current time is equal to the last
			// collected time stamp (this way, events in different deltas, but the same
			// time stamp are considered).
			// For each task, it is checked if the event triggering this process
			// corresponds to an event in such task, and, in such a case, it is
			// reported to the task activity vector
			for(auto it = task_activity.begin(); it != task_activity.end(); ++it ) {
				cur_task_info = it->first;
				if(cur_task_info->state_signal.event()) { // this if could be removed, only for efficiency
					cur_task_state = cur_task_info->state_signal.read();
					// adds the state change for the current time in the task activity vector
					task_state_table_p = it->second;
					(*task_state_table_p)[current_time] = cur_task_state;
					// Note: a map structure used, so in case of state change in the same time stamp
					//       the last state value steps over the previous one
				}
			}
			
			// Collect scheduler activity, in case it has not been disabled
			// by the user
//			if(show_schedulers_activity) {
			for(auto it = sched_activity.begin(); it != sched_activity.end(); ++it ) {
				cur_sched = it->first;
				if(cur_sched->state_signal.event()) { // this if could be removed, only for efficiency
					cur_sched_state = cur_sched->state_signal.read();
					// adds the state change for the current time in the task activity vector
					sched_state_table_p = it->second;
					(*sched_state_table_p)[current_time] = cur_sched_state;
					// Note: a map structure used, so in case of state change in the same time stamp
					//       the last state value steps over the previous one
				}
			}		

		}
		
	}
}	


void tikz_activity_trace::tikz_trace_activity_process_clustered_compact() {
	std::string msg;
	unsigned int i;
	sc_time current_time;
	task_info_t			*cur_task_info;
	task_state_t		cur_task_state;
	
	scheduler*			cur_sched;
	sched_state_t  		cur_sched_state;
	
	compact_states_table_t	*compact_states_table_p;
	
	taskset_by_name_t::iterator task_by_name_it;
			
	bool sched_executing_found;
	unsigned int tasks_executing;
	bool cluster_event; // flag to record if there was an event associated to the tasks-sched cluster
	
	//  the data collection start after the start configured ime
	wait(ttikz_start_trace_time);
	
	current_time = sc_time_stamp();

	// an alternative is to start the capture right after the first event
	// in the sensitivity list equal or after the threshold time
/*
 * 	while(current_time < ttikz_start_trace_time) {
		 wait();
		 current_time = sc_time_stamp();
	}
*/

	msg = "Data collection for TiKZ activity report start at ";
	msg += current_time.to_string();
	SC_REPORT_INFO("KisTA",msg.c_str());

	// stores the first time stamp and its corresponding index
	time_stamps.push_back(current_time);
	i = 0;
	time_stamps_index[current_time] = i;
	i++;
	
	// initial state for the compact activity format
	for(auto it_sched = compact_activity.begin(); it_sched != compact_activity.end(); ++it_sched ) {
		sched_executing_found = false;
		tasks_executing = 0;
		cur_sched = it_sched->first;
		
		cur_sched_state = cur_sched->state_signal.read();
		compact_states_table_p=it_sched->second;
		if(cur_sched_state==SCHEDULING) {
			(*compact_states_table_p)[current_time] = cur_sched_state;	// assign "S"	
			sched_executing_found = true;
		}
			
		for(task_by_name_it = cur_sched->gets_tasks_assigned()->begin(); task_by_name_it != cur_sched->gets_tasks_assigned()->end(); ++task_by_name_it ) {
			cur_task_info = task_by_name_it->second;			
			cur_task_state = cur_task_info->state_signal.read();
			if(cur_task_state==EXECUTING) {			
				(*compact_states_table_p)[current_time] = std::to_string(cur_task_info->kista_id).c_str()[0];	// assign task id string
				tasks_executing++;
			}
		}
/*		
 // For the moment not to apply this checks because they are globally applied, not for each scheduler.
 // What has to be added here is that for each PE/scheduler not more than one task (included the scheduler is applied)
		if(sched_executing_found && (tasks_executing>0)) {
			SC_REPORT_ERROR("KisTA","When start to record data for TiKZ trace. The scheduler and at least one task are active at the same time (not possible in local schedulers).");
		}

		if(tasks_executing>1) {
			SC_REPORT_ERROR("KisTA","When start to record data for TiKZ trace. More than one task are active at the same time (not possible in local schedulers).");
		}
*/					
		if(!sched_executing_found && (tasks_executing==0)) {
			(*compact_states_table_p)[current_time] = 'X';	// assign 'X' indicating neither the scheduler nor a task executing
		}
				
	}
	
	while(true) {
		wait();
//		cout << "TiKZ activity report event at " << sc_time_stamp() << endl;
		
		// adds a new time stamp only if the time stamp is bigger
		// and the max. amount of time stamps is respected
		current_time = sc_time_stamp();

		if(current_time>=time_stamps.back()) {
			if(current_time>time_stamps.back()) { // if there is time advance
			                                       // either the time stamps vector has
			                                       // to be grown, or the collection finished
				if(time_stamps.size()<ttikz_max_n_time_stamps) {
					// this update takes into account that several events
					// can appear at the same time stamp
					// still more time stamps can be added...
					time_stamps.push_back(current_time);
					// ... and its corresponding index
					time_stamps_index[current_time] = i;
					i++;
				} else { // the limit of time stamps has been reached
						 // and the next event exceedes the limit
						 // so we end the collection of data
					break;
				}
			}
			
			// collect activity in compact format
			for(auto it_sched = compact_activity.begin(); it_sched != compact_activity.end(); ++it_sched ) {
				sched_executing_found = false;
				tasks_executing = 0;
				cluster_event = false;
			
				cur_sched = it_sched->first;
				
				compact_states_table_p=it_sched->second;
				
				if(cur_sched->state_signal.event()) {
					cluster_event = true;
					cur_sched_state = cur_sched->state_signal.read();
					if(cur_sched_state==SCHEDULING) {
						(*compact_states_table_p)[current_time] = cur_sched_state;	// assign "S"	
						sched_executing_found = true;
					}
				}
												
				for(task_by_name_it = cur_sched->gets_tasks_assigned()->begin(); task_by_name_it != cur_sched->gets_tasks_assigned()->end(); ++task_by_name_it ) {
					cur_task_info = task_by_name_it->second;
					if(cur_task_info->state_signal.event()) {
						cluster_event = true;
						cur_task_state = cur_task_info->state_signal.read();
						if(cur_task_state==EXECUTING) {
							(*compact_states_table_p)[current_time] = std::to_string(cur_task_info->kista_id).c_str()[0];	// assign task id string
							tasks_executing++;					
						}
					}
				}
				
/*
 * 				if(sched_executing_found && (tasks_executing>0)) {
						SC_REPORT_ERROR("KisTA","While recording data for TiKZ trace. The scheduler and at least one task are active at the same time (not possible in local schedulers).");
				}

				if(tasks_executing>1) {
						SC_REPORT_ERROR("KisTA","While recording data for TiKZ trace. More than one task are active at the same time (not possible in local schedulers).");
				}
	*/			

				if(cluster_event && (!sched_executing_found) && (tasks_executing==0)) {
					(*compact_states_table_p)[current_time] = 'X';	// assign 'X' indicating neither the scheduler nor a task executing
				}
				
			}

		}
		
	}
}	
	
} // end kista namespace

#endif
