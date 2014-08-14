/*****************************************************************************

  draw.hpp
  
    Facilities for automatically exporting a graphical sketch of the system
  
   This file belongs to the KisTA library
   All rights reserved by the authors (until further License definition)

  Author: F. Herrera
  Institution: KTH
  Deparment:   Electronic Systems
  Date: 2013 February
  
  Note: Currently, this sketch report enables is written taking into account
        that KisTA model is not enclosed in a top module.
        This this class assumes that a global object will be built (sketch report, of sketch_report_t type).
        Then KisTA user can enable or not the report (the call has to be done only once)
        and the header of the sketch file is created at construction time.
        Then, advantage of the SystemC callbacks are used for a correct insertion order in the
        file.
        Before end of ellaboration phase is used for the different elements in the top hierarchy to add
        to the sketch.
        End of elaboration phase is used to add the tail of the sketch file.
  
		(this is considered a preliminar solution. A more elegant solution could considered if
		KisTA enclosed the model within a top module. However, it is not clear if this should be
		like that for other reasons. In the meanwhile, this gives a solution which you can notice
		that does not require the simulation to start). Therefore, it is possible to be simply used to extract
		a Tikz sketch form the KisTA model, 

 *****************************************************************************/

#ifndef DRAW_HPP
#define DRAW_HPP

#include "systemc.h"

#include "task_info_t.hpp"
#include "logic_link.hpp"
#include "comm_res/phy_link.hpp"

namespace kista {

class sketch_report_t : sc_module {
	friend sketch_report_t& operator<<(sketch_report_t& sk_rpt, std::string content);
public:
		// create sketch report class, by default disabled
		// sketch file with the default name "system_sketch"
	sketch_report_t();
	
	bool &is_enabled(); // true if sketch reported enabled
	
		// set name of sketch report file
	void set_file_name(std::string	name_par); 
	
	void enable(); // enable sketch report and creates report header
	                // (has to be called at construction time and after set_name, otherwise, settled name is overriden and the default one taken)
	
	// configuration of the draw
	void draw_sys_level_conn();
	
	void highlight_environment();
	void highlight_system();      // overrides the hilighting of application and platform
	void highlight_application();
	void highlight_platform();
	
	void only_image();
	void set_scale(float scale_par);
	
	// auxiliar methods for other kista classes
	
	// add content to the sketch report file
	void add_content(std::string content);
	
	// specific draw methods (content filled at before_end_of ellaboration time)
	// draw elements for system-level/application elements
	void draw(task_info_t *task);
	void draw(logic_link_t ll, std::string channel_type);
	// draw method for platform elements
	void draw(scheduler *sched);
	// draw method for processing elements
	void draw(processing_element *pe);
	// draw method for generic and specific resources
	void draw(phy_comm_res_t *phy_commres_par);
	void draw(tdma_bus *bus_par);
				
private:

	void init(); // init actions
	
	void end_of_elaboration(); // closes the report file

	// helper functions
	void dot_by_slash(std::string &str_par);
	void insert_TO_tikz_macro();
	unsigned int calculate_out_angle();
	unsigned int calculate_in_angle();
		
	// method for actual dumping of the tikz content
	void actual_draw(); 

	// auxiliar methods
    void write_header();
    void write_tail();

	void draw_system_level_connections();
	void calculate_angles(std::string src_task_id, std::string dest_task_id);
 
	bool with_sys_level_conn_names;
 
 	void draw_system_level_connections_random(); // a simpler version
   
	ofstream		*sketch_file;
	std::string		sketch_file_name;
	
	bool sketch_enabled;
	bool environment_box, system_box, application_box, platform_box;
	bool only_image_flag;
	
	float scale;
	
	// structure to store task positions (index from left to right)
	// (used for improving system-level connection draw)
	std::map<std::string, unsigned int> task_position;
	std::map<std::string, unsigned int> env_task_position;
		
	// vectors storing number of inputs and outputs
	std::vector<unsigned int> outs;
	std::vector<unsigned int> inps;
	std::vector<unsigned int> env_outs;
	std::vector<unsigned int> env_inps;
};

}  // end kista namespace
	
#endif

