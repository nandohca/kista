/*****************************************************************************

  tikz_activity_trace.hpp
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2014 June

 *****************************************************************************/

#ifndef TIKZ_ACTIVITY_TRACE_HPP
#define TIKZ_ACTIVITY_TRACE_HPP

#include <systemc.h>

#include "sched_types.hpp"

namespace kista {

// ---------------------------------------------
// ACTIVITY TRACING IN TIKZ FORMAT (user API)
// ---------------------------------------------

// dumps a file with a TIKZ image tracing tasks activity, scheduler activity and communication activity
tikz_activity_trace_handler create_tikz_activity_trace(std::string ttikz_file_name_par);
tikz_activity_trace_handler create_tikz_activity_trace(); // uses default name (relying on scheduler name)

// Previous calls enable a tikz tracing with the default options
// Following calls enable the configuration of the Tikz trace

// Control of activity record
// --------------------------------		
void set_max_number_of_time_stamps(tikz_activity_trace_handler handler, unsigned int max_time_stamps);
	// default number of events, so of time stamps reported: 15

void set_start_time(tikz_activity_trace_handler handler, sc_time start_time);
	// default start time where the activity trace start: 0s
	
// Control of representation
// --------------------------------		
void cluster(tikz_activity_trace_handler handler);
	// cluster traces on schedulers

void compact(tikz_activity_trace_handler handler);
	// uses the compact style (one trace for each scheduler and task)
	// By default, an unfolded style, using a trace for each task and scheduler
	// ONLY APPLICABLE TO THE CLUSTERED STYLE

void set_base_lines_separation(tikz_activity_trace_handler handler, unsigned int separation);
    // default baselines separation: 2

void set_time_stamps_max_separation(tikz_activity_trace_handler handler, unsigned int max_separation);
    // default max separation: 1 which involves that the obtained TiKZ trace will
    // equally space time stamps. 
    // However, in order to provide graphically an idea the user can establisn a max_separation
    // value (values 2 or 3 recommended. For instance, lets suposse we assign value 3.
    // Then, KisTA will assign a separation of 1, 2 or 3, etc depending of the actual
    // separation (in SystemC time) of consecutive time tags.

// CONTROL OF PICTURE FORMAT
void set_scale(tikz_activity_trace_handler handler, float scale);  // default scale: 1
    // default scale: 1 
    
void set_landscape(tikz_activity_trace_handler handler);
	// set the trace report in landscape format (default portrait)

// CONTROL OF INFORMATION IN PICTURE
void do_not_show_schedulers(tikz_activity_trace_handler handler);
	// to not to show schedulers (shown by default) in the default unfolded style

void no_text_in_traces(tikz_activity_trace_handler handler);
	// do not show labels within the activity boxes (state in the unfolded style,
	// tasks short names in the compact style)

void do_not_show_unactive_boxes(tikz_activity_trace_handler handler);
	// do not show labels within the activity boxes (state in the unfolded style,
	// tasks short names in the compact style)

// ---------------------------------------------
// Internal classes (not intended for user)
// ---------------------------------------------
	
class tikz_activity_trace : public sc_module {
	typedef char compact_state_t;
	typedef std::map<sc_time,compact_state_t> compact_states_table_t;
	typedef std::unordered_map<scheduler*, compact_states_table_t*>	compact_activity_t;
public:
	tikz_activity_trace(sc_module_name name = sc_gen_unique_name("tikz_activity_trace"));
	
	// control of activity record
	void set_max_number_of_time_stamps(unsigned int max_time_stamps);
	void set_start_time(sc_time start_time); // default: 0s
	
	// control of representation
	void cluster(); // by default, all selected tasks are hown in a flat style
	                 // If cluster is called, the representations shows it in groups of 
	                 // tasks + associated schedulers
	                 
	void compact(); // instead the default style, which uses a trace for each scheduler state and
	                  // task state, it uses a trace for each scheduler-assigned task set (cluster)
	                  // ONLY APPLICABLE FOR THE CLUSTERED STYLE
	
	void do_not_show_schedulers(); // shown by default
	void set_base_lines_separation(unsigned int separation); // default: 2
	void set_time_stamps_max_separation(unsigned int max_separation);
	void set_scale(float scale); // default: 1
	void set_landscape(); // (portrait by default)
	void no_text_in_traces(); // do not informative text within the activity boxes
					   // By default, the trace shows the state in the unfolded style,
	                   // and the short task alias in the compact style)
	void do_not_show_unactive_boxes(); // do not informative text within the activity boxes

	//
	// To Be done:
	// In order to select tasks and schedulers to be represented
	// 	void select_tasks(set<std::string> tasks);
	// 	void select_schedulers(set<std::string> tasks);

private:
	ofstream		*tikz_trace_file;
	std::string		ttikz_file_name;
	sc_time			ttikz_start_trace_time;
	unsigned int	ttikz_max_n_time_stamps;

	bool			clustered_style;
	bool			compact_style;

	unsigned int	base_lines_separation;
	unsigned int	time_stamps_max_separation;
	float			ttikz_trace_scale;
	bool			ttikz_trace_landscape;
	bool			show_schedulers_activity;
	bool			show_text_in_traces;
	bool			show_unactive_boxes;

	// data structures to store:
	// - time stamps of time axis
	std::vector<sc_time>				time_stamps;
	// - structure for fast extraction of the index of a time stamp
	std::map<sc_time,unsigned int>		time_stamps_index;
	std::vector<unsigned int>			span_vec; // index spans, give the span for the representation
	                                          // to the next time index, which can be up to time_stamps_max_separation
	std::vector<sc_time>				time_spans;
	std::vector<unsigned int>			x_location;                    
	
	sc_time span_time_gap; // Span time gap
		
	// - tasks activity
	task_activity_t					task_activity;
	// - position index for tasks
//	std::unordered_map<task_info_t*,unsigned int>	task_position;
	
	// - schedulers activity
	sched_activity_t					sched_activity;
	// - position index for schedulers
//	std::unordered_map<scheduler*,unsigned int>	sched_position;
	
	// - time stamps of time axis
	compact_activity_t				compact_activity;

	void before_end_of_elaboration();
	void end_of_simulation();
		
	void tikz_trace_activity_process();
	void tikz_trace_activity_process_unfolded();
	void tikz_trace_activity_process_clustered_compact();

	// auxiliar methods for exporting the tikz file
	void draw_start();
	void draw_end();
		
	void draw_flat_all();
	void draw_clustered_unfolded();
	void draw_clustered_compact();
	//void draw_selection();
	
	// helpers for unfolded style
	void draw_time_axis();
			
	// helpers for unfolded style
	void draw_y_axis_flat();
	
	void draw_y_axis_clustered_unfolded(scheduler *sched, unsigned int cluster_number);
	void draw_y_axis_clustered_compact(scheduler *sched, unsigned int cluster_number);
	void draw_y_axis_clustered();
	
	void draw_baselines();
	
	void write_task_activity_path(task_info_t *task_info, task_states_table_t	 *task_states_table_p);
	void write_sched_activity_path(scheduler *sched, sched_states_table_t *sched_states_table_p);

	void draw_task_activity(task_info_t *task_info, task_states_table_t	 *task_states_table_p);
	void draw_sched_activity(scheduler *sched, sched_states_table_t *sched_states_table_p);

	// helpers for compact style
	void write_compact_cluster_activity_path(scheduler *sched, compact_states_table_t *compact_states_table_p);
	void draw_compact_cluster_activity(scheduler *sched, compact_states_table_t *compact_states_table_p);
	
	unsigned int get_span(unsigned int cur_index, unsigned int next_index); // provide the span in x location given two event indexes
	
};

} // end kista namespace

#endif
