#ifndef DRAW_CPP
#define DRAW_CPP

#include <iostream>
#include <ctime>

#include "time.h"

#include "defaults.hpp"

#include "global_elements.hpp"

#include "draw.hpp"
#include "taskset_by_name_t.hpp"
#include "application_t.hpp"
#include "scheduler.hpp"
#include "processing_element.hpp"
#include "comm_res/buses/tdma_bus.hpp"

namespace kista {

sketch_report_t::sketch_report_t() : sc_module (sc_module_name("system_sketch")) {
	sketch_file_name = name();
	sketch_file_name += ".tex";
	init();
}

void sketch_report_t::init() {
	// creation of the file
	sketch_enabled =  false;
	
	// to allow different routings for the task connection whenever the KisTA executable is executed
	srand (time(NULL));
	
	with_sys_level_conn_names = false;
	
	environment_box = false;
	system_box = false;
	application_box = false;
	platform_box = false;
	only_image_flag = false;
	
	scale = 1.0;
}

void sketch_report_t::set_file_name(std::string name_par) {
	sketch_file_name = name_par;
	sketch_file_name += ".tex";
}

void sketch_report_t::enable() {
	sketch_file = new ofstream(sketch_file_name);
	sketch_enabled = true;
//cout << "Sketch report file name: " << sketch_file_name << endl;
}

void sketch_report_t::only_image() {
	check_call_before_sim_start("only_image");
	only_image_flag = true;
}

void sketch_report_t::set_scale(float scale_par) {
	check_call_before_sim_start("set_scale");	
	scale = scale_par;
}

bool& sketch_report_t::is_enabled() {
	return sketch_enabled;
}

void sketch_report_t::end_of_elaboration() {
	if(is_enabled()) {
		// init to 0 inputs and outputs in inps and outs vector
		for(unsigned int i=0; i < task_info_by_name.size();i++) {
			inps.push_back(0);
			outs.push_back(0);
		}
		for(unsigned int i=0; i < env_tasks_by_name.size();i++) {
			env_inps.push_back(0);
			env_outs.push_back(0);
		}
		// draw
		write_header();
		actual_draw();
		write_tail();
	}
}

void sketch_report_t::write_header() {
   	char* daytime;
	time_t now;
	tm *gmtime_p;
	
   // current date/time based on current system
   now = time(0);
   
   // convert now to string form
   daytime = ctime(&now);

   // convert now to tm struct for UTC
   //gmtime_p = gmtime(&now);
   gmtime_p = localtime(&now);
   daytime = asctime(gmtime_p);	
   if(only_image_flag) {
		*sketch_file << "\\documentclass[]{article}" << endl;	   
   } else {
		*sketch_file << "\\documentclass[a4paper,10pt]{article}" << endl;
   }
   *sketch_file << "\\usepackage{tikz}" << endl;
   *sketch_file << "\\usepackage{verbatim}" << endl;
   *sketch_file << "\\usepackage[margin=15mm]{geometry}" << endl;
   *sketch_file << "\\usetikzlibrary{shapes,arrows,fit,calc,positioning}" << endl;
   
   if(with_sys_level_conn_names) {
		*sketch_file << "\\usetikzlibrary{decorations.text}" << endl;
		*sketch_file << "\\usetikzlibrary{decorations.pathmorphing}" << endl;
   }

   if(environment_box || system_box || application_box || platform_box) {
	   *sketch_file << "\\usetikzlibrary{backgrounds}" << endl;
   }
   
   insert_TO_tikz_macro();
   
   *sketch_file << "\\begin{document}" << endl;

   if(!only_image_flag) {
	   *sketch_file << "File created by KisTA library" << endl << endl;
	   *sketch_file << "Author: F.Herrera" << endl << endl;
	   *sketch_file << "Institution: KTH (2012-2014, Jul), Univ. of Cantabria (from 2014, Sept.)" << endl << endl;
	   *sketch_file << "Rights reserved by the authors in the terms defined by the KisTA license." << endl << endl;
	   *sketch_file << "KisTA library compilation date: " << __DATE__ << " at " << __TIME__ << "." << endl << endl;
	   *sketch_file << endl << endl;
	   *sketch_file << "Sketch Creation: " << daytime << endl << endl;
			
	   *sketch_file << "Sketch options enabled:" << endl;	
	   if(with_sys_level_conn_names) {
			*sketch_file << "draw system level connections, ";
	   }

	   if(environment_box) {
			*sketch_file << "draw environment box, ";
	   }

	   if(system_box) {
			*sketch_file << "draw system box, ";	
	   }  	
	 
	   if(application_box) {
			*sketch_file << "draw application box(es), ";
	   }  
	   
	   if(platform_box) {
			*sketch_file << "draw platform box, ";
	   } 
	   *sketch_file << "scale = " << scale << endl;
			
	   *sketch_file << "\\hfill" << endl << endl;
	}
   // defining styles for KisTA sketch export
   // *****************************************
   *sketch_file << "" << endl;
   
   // system / application / PIM styles
   // ----------------------------------
     // task style
   *sketch_file << "\\tikzstyle{task_style}=[circle, thick, draw=orange!80, fill=orange!25]" << endl;     
   //*sketch_file << "\\tikzstyle{task_style}=[circle, thick, minimum size=1.2cm, draw=orange!80, fill=orange!25]" << endl;
     // system-level channels and logic link arrow styles
   *sketch_file << "\\tikzstyle{logic_link_style} = [->, >=latex', shorten >=1pt]" << endl;
   *sketch_file << "\\tikzstyle{fifo_buffer_style} = [->, >=latex', shorten >=1pt, thick]" << endl;
   
     // task to scheduler mapping arrow style
   *sketch_file << "\\tikzstyle{task_map_arrow} = [dashed, ->, draw=orange!80, fill=orange!80, >=latex', shorten >=1pt, thick]" << endl;
     
	// SW platform level styles
	// ----------------------------------
     // scheduler style
   *sketch_file << "\\tikzstyle{sched_style} = [rectangle, draw=red!80, fill=red!20, rounded corners, thick]" << endl;     

     // scheduler to PE mapping arrow style
   *sketch_file << "\\tikzstyle{sched_map_arrow} = [dashed, ->, draw=red!80, fill=red!80, >=latex', shorten >=1pt, thick]" << endl;
   
   // HW platform level styles
   // ----------------------------------
     // processing element style
   *sketch_file << "\\tikzstyle{pe_style} = [rectangle, draw=blue!50, fill=blue!20, thick]" << endl;

     // generic communication resource
   *sketch_file << "\\tikzstyle{phy_commres_style} = [cloud, draw=blue!50, fill=blue!10, thick]" << endl;
     // specific communication resources
   *sketch_file << "\\tikzstyle{tdma_bus_style} = [rectangle, draw=blue!50, fill=blue!10, thick]" << endl;
   
     // generic communication resource
   *sketch_file << "\\tikzstyle{platform_conn_style} = [blue!50, >=latex', shorten >=1pt]" << endl;

   // Styles for highlighting boxes
   // ----------------------------------
   if(environment_box){
	   *sketch_file << "\\tikzstyle{env_box_st} = [draw=green, fill=green!10, very thick, rectangle, rounded corners, inner sep=10pt, inner ysep=10pt]" << endl;
   }
   
   if(system_box) {
      *sketch_file << "\\tikzstyle{sys_box_st} = [draw=black, fill=black!10, very thick, rectangle, rounded corners, inner sep=10pt, inner ysep=10pt]" << endl;
   } else {
      if(application_box) {
         *sketch_file << "\\tikzstyle{app_box_st} = [draw=red, fill=red!10, very thick, rectangle, rounded corners, inner sep=10pt, inner ysep=10pt]" << endl;	
      }
      if(platform_box) {
         *sketch_file << "\\tikzstyle{plat_box_st} = [draw=blue, fill=blue!10, very thick, rectangle, rounded corners, inner sep=10pt, inner ysep=10pt]" << endl;		
      }		
   }

   // begin tikz picture environment
   *sketch_file << "\% Use the scale factor for changing the size of the figure and better export to png" << endl;
   *sketch_file << "\% You can set if from KisTA with set_scale method, or by editing this .tex file" << endl;
   *sketch_file << "\\begin{tikzpicture}[scale=";
   *sketch_file << scale;
   *sketch_file << ", transform shape]" << endl;

   //*sketch_file << "\\begin{tikzpicture}" << endl;

}

void sketch_report_t::write_tail() {
   *sketch_file << " " << endl;
		
   *sketch_file << "\\end{tikzpicture}" << endl;
   *sketch_file << "\\end{document}" << endl;
   *sketch_file << endl;
}

void sketch_report_t::draw_sys_level_conn() {
	check_call_before_sim_start("draw_sys_level_conn");
	with_sys_level_conn_names = true;
}


void sketch_report_t::highlight_environment() {
	check_call_before_sim_start("hilight_environment");
	environment_box = true;
}

void sketch_report_t::highlight_system() {      // overrides the hilighting of application and platform
	check_call_before_sim_start("hilight_system");
	system_box = true;
}

void sketch_report_t::highlight_application() {
	check_call_before_sim_start("hilight_application");
	application_box = true;
}

void sketch_report_t::highlight_platform() {
	check_call_before_sim_start("hilight_platform");
	platform_box = true;
}
	
		
void sketch_report_t::add_content(std::string content) {
   *sketch_file << content;
}

void sketch_report_t::draw(task_info_t *task) {
	tasks.push_back(task);
}

void sketch_report_t::draw(scheduler *sched) {
	scheds.push_back(sched);	
}

void sketch_report_t::draw(processing_element *pe) {
	pes.push_back(pe);
}

void sketch_report_t::draw(logic_link_t ll, std::string channel_type) {
	sys_conn_types[ll]=channel_type;
}

void sketch_report_t::draw(phy_comm_res_t *phy_commres_par) {
	phy_commres_set.push_back(phy_commres_par);
}

void sketch_report_t::draw(tdma_bus *bus_par) {
	tdma_buses.push_back(bus_par);
}

void sketch_report_t::dot_by_slash(std::string &str_par) {
	size_t char_idx; // note: using unsigned int here does not work!
	char_idx = 0; 
	do {
		char_idx=str_par.find(".");
		if(char_idx==std::string::npos) break;
		str_par.replace(char_idx,1,"/");
	} while(true);
}

//
// notes:
//   (1) Includes $ $ for labels, since the names can include "_" for subscrits
//       So underscores in the KisTA code are represented as subscripts in the output representation
//   (2) for node identifieres, "." are substituted by "/", to avoid drawing issues for references (like .north or .south)
//       in the tikz output
//       SystemC includes "." in the names automatically to produce unique names in an hierarchy of modules
//       KisTA also involves "." in names when generating, for instance a default PE associated to an scheduler
//
//
void sketch_report_t::actual_draw() {
	unsigned int i;
	std::string rpt_msg;
	std::string task_id_name, task_prev_id_name,
				env_task_id_name, env_task_prev_id_name;
	std::string	last_task_id_name;
	std::string sched_id_name, sched_prev_id_name;
	std::string pe_id_name, pe_prev_id_name;
	std::string commres_id_name, commres_prev_id_name;
	
	// to iterate assigned taskets
	applications_by_name_t::iterator app_it;
	taskset_by_name_t::iterator taskset_it; // iterator for taskset
	taskset_by_name_t *current_tasks_assigned;
	
	tasks_info_by_name_t::iterator task_it;
	tasks_info_by_name_t::iterator task_env_it;
		
	// to iterate processing elements
	//PEset_t::iterator PEset_it; // iterator for PEset 
	//PEset_t *current_PEs_assigned;
	processing_element *current_PEs_assigned; // for the moment, a single PE assigned
	
	std::string last_app_name;
	
	// draw tasks
	// ---------------------------------
/*	if(tasks.size()>0) {
		// (2) for node identifieres, "." are substituted by "/", to avoid drawing issues
		task_id_name = tasks[0]->name();
		task_position[task_id_name] = 0;
		dot_by_slash(task_id_name);
		
		*sketch_file << "\\node (" << task_id_name << ") [task_style] {$" << tasks[0]->name() << "$};" << endl;	
		for(i=1; i<tasks.size(); i++) {
			task_id_name = tasks[i]->name();
			task_position[task_id_name] = i;
			dot_by_slash(task_id_name);
			task_prev_id_name = tasks[i-1]->name();
			dot_by_slash(task_prev_id_name);
			*sketch_file << "\\node (" << task_id_name  << ") [task_style, right=of ";
			*sketch_file <<  task_prev_id_name;
			*sketch_file << "] {$" << tasks[i]->name() << "$};" << endl;	
		}
	}
*/

	// draw environment tasks
	// ---------------------------------
	if(env_tasks_by_name.size()>0) {
		
		task_env_it = env_tasks_by_name.begin();
		env_task_id_name = task_env_it->first;

		env_task_position[env_task_id_name] = 0;
		dot_by_slash(env_task_id_name); // (2) for node identifiers, "." are substituted by "/", to avoid drawing issues in tikz	
		
		*sketch_file << "\\node (" << env_task_id_name << ") [task_style] {$" << env_task_id_name << "$};" << endl;	
		
		env_task_prev_id_name = env_task_id_name;
		dot_by_slash(env_task_prev_id_name);
			
		task_env_it++;
		i=1;
		
		while(task_env_it != env_tasks_by_name.end()) {	
			env_task_id_name = task_env_it->first;
			env_task_position[env_task_id_name] = i;
			dot_by_slash(env_task_id_name);
			
			*sketch_file << "\\node (" << env_task_id_name  << ") [task_style, right=of ";
			*sketch_file <<  env_task_prev_id_name;
			*sketch_file << "] {$" << env_task_id_name << "$};" << endl;

			env_task_prev_id_name = env_task_id_name;
			dot_by_slash(env_task_prev_id_name);
							
			task_env_it++;
			i++;
		}

		env_task_id_name = env_tasks_by_name.begin()->first; // reference for lower layer
	}
	
	// draw system tasks
	// ----------------------------------------------------------------------
	if(task_info_by_name.size()>0) {	
		task_it = task_info_by_name.begin();
		task_id_name = task_it->first;

		task_position[task_id_name] = 0;
		dot_by_slash(task_id_name); // (2) for node identifiers, "." are substituted by "/", to avoid drawing issues in tikz	
		
		if(env_tasks_by_name.size()>0) {
			*sketch_file << "\\node (" << task_id_name << ") [task_style, below=of " << env_task_id_name<< "] {$" << task_id_name << "$};" << endl;		
		} else {
			*sketch_file << "\\node (" << task_id_name << ") [task_style] {$" << task_id_name << "$};" << endl;	
		}
		
		task_prev_id_name = task_id_name;
		dot_by_slash(task_prev_id_name);
			
		task_it++;
		i=1;
		
		while(task_it != task_info_by_name.end()) {	
			task_id_name = task_it->first;
			task_position[task_id_name] = i;
			dot_by_slash(task_id_name);
			
			*sketch_file << "\\node (" << task_id_name  << ") [task_style, right=of ";
			*sketch_file <<  task_prev_id_name;
			*sketch_file << "] {$" << task_id_name << "$};" << endl;

			task_prev_id_name = task_id_name;
			dot_by_slash(task_prev_id_name);
							
			task_it++;
			i++;
		}
		
		last_task_id_name = task_id_name; // to be used later with application boxes
		
		task_id_name = task_info_by_name.begin()->first; // reference for lower layer

	} else {
		rpt_msg = "While generating sketch. There are no system tasks.";
		SC_REPORT_ERROR("KisTA",rpt_msg.c_str());
	}
	
	// draw schedulers
	// ---------------------------------
	if(scheds.size()>0) {
		sched_id_name = tasks[0]->name();
		dot_by_slash(sched_id_name);
		sched_id_name = scheds[0]->name() ;
		// location of first sched (always below tasks)
		if(tasks.size()>0) {
			*sketch_file << "\\node (" << sched_id_name << ") [sched_style, below=of " << task_id_name<< "] {$" << scheds[0]->name() << "$};" << endl;	
		} else {
			*sketch_file << "\\node (" << sched_id_name << ") [sched_style] {$" << scheds[0]->name() << "$};" << endl;	
		}
		for(i=1; i<scheds.size(); i++) {
			sched_id_name = scheds[i]->name();
			dot_by_slash(sched_id_name);
			sched_prev_id_name = scheds[i-1]->name();
			dot_by_slash(sched_prev_id_name);
			*sketch_file << "\\node (" << sched_id_name << ") [sched_style, right=of ";
			*sketch_file <<  sched_prev_id_name;
			*sketch_file << "] {$" << scheds[i]->name() << "$};" << endl;	
		}
	}
	
	*sketch_file << endl;
	
	sched_id_name = scheds[0]->name(); // reference for lower layer
	
	// draw processing elements
	// ---------------------------------
	if(pes.size()>0) {
		pe_id_name = pes[0]->name();
		dot_by_slash(pe_id_name);	
		// location of first pe (always below sched, task)
		if(scheds.size()>0) {
			// first, it tries to place it below first scheduler
			*sketch_file << "\\node (" << pe_id_name << ") [pe_style, below=of " << sched_id_name << "] {$" << pes[0]->name() << "$};" << endl;	
		} else if (tasks.size()>0) {
			// if there are no scheds depicted it tries to place it below tasks
			*sketch_file << "\\node (" << pe_id_name << ") [pe_style, below=of " << task_id_name << "] {$ " << pes[0]->name() << "$};" << endl;	
		} else {
			*sketch_file << "\\node (" << pe_id_name << ") [pe_style] {$" << pes[0]->name() << "$};" << endl;	
		}
		for(i=1; i<pes.size(); i++) {
			pe_id_name = pes[i]->name();
			dot_by_slash(pe_id_name);
			pe_prev_id_name = pes[i-1]->name();
			*sketch_file << "\\node (" << pe_id_name << ") [pe_style, right=of ";
			*sketch_file << pe_prev_id_name;
			*sketch_file << "] {$" << pes[i]->name() << "$};" << endl;	
		}
	}
	
	*sketch_file << endl;
	
	pe_id_name = pes[0]->name(); // reference for lower layer
	
	// draw communication resources
	// ---------------------------------
	if(phy_commres_set.size()>0) {
		// (2) for node identifieres, "." are substituted by "/", to avoid drawing issues
		commres_id_name = phy_commres_set[0]->name();
		dot_by_slash(commres_id_name);
		
		// location of first commres (always below pe, sched, task)
		if(pes.size()>0) {
			*sketch_file << "\\node (" << commres_id_name << ") [phy_commres_style, below=of " << pe_id_name << "] {$" << phy_commres_set[0]->name() << "$};" << endl;	
		} else if(scheds.size()>0) {
			// first, it tries to place it below first scheduler
			*sketch_file << "\\node (" << commres_id_name << ") [phy_commres_style, below=of " << sched_id_name << "] {$" << phy_commres_set[0]->name() << "$};" << endl;	
		} else if (tasks.size()>0) {
			// if there are no scheds depicted it tries to place it below tasks
			*sketch_file << "\\node (" << commres_id_name << ") [phy_commres_style, below=of " << task_id_name << "] {$ " << phy_commres_set[0]->name() << "$};" << endl;	
		} else {
			*sketch_file << "\\node (" << commres_id_name << ") [phy_commres_style] {$" << phy_commres_set[0]->name() << "$};" << endl;
		}
		
		for(i=1; i<phy_commres_set.size(); i++) {
			commres_id_name = phy_commres_set[i]->name();
			dot_by_slash(commres_id_name);
			commres_prev_id_name = phy_commres_set[i-1]->name();
			dot_by_slash(commres_prev_id_name);
			*sketch_file << "\\node (" << commres_id_name  << ") [phy_commres_style, right=of ";
			*sketch_file <<  commres_prev_id_name;
			*sketch_file << "] {$" << phy_commres_set[i]->name() << "$};" << endl;	
		}
	}
	
	// draw tdma buses
	// ---------------------------------
	if(tdma_buses.size()>0) {
		// (2) for node identifieres, "." are substituted by "/", to avoid drawing issues
		commres_id_name = tdma_buses[0]->name();
		dot_by_slash(commres_id_name);
		
		// location of first commres (always below pe, sched, task)
		if(pes.size()>0) {
			*sketch_file << "\\node (" << commres_id_name << ") [tdma_bus_style, below=of " << pe_id_name << "] {$" << tdma_buses[0]->name() << "$};" << endl;	
		} else if(scheds.size()>0) {
			// first, it tries to place it below first scheduler
			*sketch_file << "\\node (" << commres_id_name << ") [tdma_bus_style, below=of " << sched_id_name << "] {$" << tdma_buses[0]->name() << "$};" << endl;	
		} else if (tasks.size()>0) {
			// if there are no scheds depicted it tries to place it below tasks
			*sketch_file << "\\node (" << commres_id_name << ") [tdma_bus_style, below=of " << task_id_name << "] {$ " << tdma_buses[0]->name() << "$};" << endl;	
		} else {
			*sketch_file << "\\node (" << commres_id_name << ") [tdma_bus_style] {$" << tdma_buses[0]->name() << "$};" << endl;
		}
		
		for(i=1; i<phy_commres_set.size(); i++) {
			commres_id_name = tdma_buses[i]->name();
			dot_by_slash(commres_id_name);
			commres_prev_id_name = tdma_buses[i-1]->name();
			dot_by_slash(commres_prev_id_name);
			*sketch_file << "\\node (" << commres_id_name  << ") [commres_style, right=of ";
			*sketch_file <<  commres_prev_id_name;
			*sketch_file << "] {$" << tdma_buses[i]->name() << "$};" << endl;	
		}
	}
	
	// draw system-level (PIM/application) connections
	// ------------------------------------------------	
	draw_system_level_connections();

	// draw_system_level_connections_random();

	// draw task to scheduler mappings
	// ---------------------------------
	for(i=0; i < scheds.size();i++) {
		sched_id_name = scheds[i]->name();
		dot_by_slash(sched_id_name);
		current_tasks_assigned = scheds[i]->tasks_assigned;
		for(taskset_it=current_tasks_assigned->begin();taskset_it!=current_tasks_assigned->end();taskset_it++) {
			*sketch_file << "\\draw (";
			task_id_name = taskset_it->second->name();
			dot_by_slash(task_id_name);
			*sketch_file << task_id_name;
			*sketch_file << ".south) [task_map_arrow] -- (";
			*sketch_file << sched_id_name;
			*sketch_file << ".north);" << endl;
		}
	}
	
	*sketch_file << endl;

	// draw scheduler to processing elements mappings
	// ---------------------------------
	for(i=0; i < scheds.size();i++) {
		sched_id_name = scheds[i]->name();
		dot_by_slash(sched_id_name);
		
		// note; for the moment, a single one, since scheduler class maps to a single one
		
		// so, the following code (ready for a multiple PE assignation) is disabled by now...
		/*
		current_PEs_assigned = (*scheds)[i]->PEs_assigned; 
		for(PEset_it=current_tasks_assigned->begin();PEset_it!=current_tasks_assigned->end();PEset_it++) {
		   *sketch_file << "\\draw (";
			*sketch_file << sched_id_name;
			*sketch_file << ".south) [map_arrow] -- (";
			pe_id_name = PEset_it->second->name();
			dot_by_slash(pe_id_name);
			*sketch_file << pe_id_name;		
			*sketch_file << ".north);" << endl;
		}
		*/
		// ... and substituted by the following one
		current_PEs_assigned = scheds[i]->PE; // note; for the moment, a single one, since scheduler class maps to a single one
		*sketch_file << "\\draw (";
		*sketch_file << sched_id_name;
		*sketch_file << ".south) [sched_map_arrow] -- (";
		pe_id_name = current_PEs_assigned->name();
		dot_by_slash(pe_id_name);
		*sketch_file << pe_id_name;		
		*sketch_file << ".north);" << endl;
	}
	
	*sketch_file << endl;

	// draw connections between processing elements and communication resources
	// --------------------------------------------------------------------------
	if((phy_commres_set.size()>0) || (tdma_buses.size()>0)) { 
		// KisTA library can model abstract models where no communication resources are involved.
		// (in such a case a 0 penalty is assumed for PE-PE communications)
		// Then, the sketch can be exported, since no communication resources are sketched.
		// In coherence with it, under such situation, no platform connection between PEs and communication resoruces
		// need to be done)
		for(i=0; i < pes.size();i++) {
			pe_id_name = pes[i]->name();
			commres_id_name = pes[i]->get_connected_comm_res()->name();
			dot_by_slash(pe_id_name);
			dot_by_slash(commres_id_name);
			*sketch_file << "\\draw (";
			*sketch_file << pe_id_name;
			*sketch_file << ".south) [platform_conn_style] -- (";
			*sketch_file << commres_id_name;		
			*sketch_file << ".north);" << endl;
		}
	}
	
	// draw hilighting boxes
	// ---------------------------------
	if(environment_box) {
		*sketch_file << "\\begin{scope}[on background layer]" << endl;
		*sketch_file << "\\node[env_box_st,fit = ";
		for(task_env_it=env_tasks_by_name.begin(); task_env_it != env_tasks_by_name.end(); task_env_it++) {
			*sketch_file << "(";
			*sketch_file << task_env_it->first;
			*sketch_file << ")";
		}
		*sketch_file << "] (env_box) {};" << endl;
		*sketch_file << "\\end{scope}" << endl;
		*sketch_file << "\\node [right=of env_box] {$Environment$};" << endl;
	}
	
	// either a system box or application(s) and platform boxes are drawn
	if(system_box) {
		*sketch_file << "\\begin{scope}[on background layer]" << endl;
		*sketch_file << "\\node[sys_box_st,fit = ";
		for(task_it=task_info_by_name.begin(); task_it != task_info_by_name.end(); task_it++) {
			*sketch_file << "(";
			*sketch_file << task_it->first;
			*sketch_file << ")";
		}
		for(i=0; i<scheds.size(); i++) {
			sched_id_name = scheds[i]->name();
			*sketch_file << "(";
			*sketch_file << sched_id_name;
			*sketch_file << ")";				
		}
		for(i=0; i<pes.size(); i++) {
			pe_id_name = pes[i]->name();
			*sketch_file << "(";
			*sketch_file << pe_id_name;
			*sketch_file << ")";					
		}
		for(i=0;i<phy_commres_set.size();i++) {
			commres_id_name = phy_commres_set[i]->name();
			*sketch_file << "(";
			*sketch_file << commres_id_name;
			*sketch_file << ")";
		}
		for(i=0;i<tdma_buses.size();i++) {
			commres_id_name = tdma_buses[i]->name();
			*sketch_file << "(";
			*sketch_file << commres_id_name;
			*sketch_file << ")";				
		}		
		*sketch_file << "] (sys_box) {};" << endl;
		*sketch_file << "\\end{scope}" << endl;
		*sketch_file << "\\node [right=of sys_box] {$System$};" << endl;
	} else {
		if(application_box) {
			switch(app_visibility) {
				case UNSET_APP_VISIBILITY:
					SC_REPORT_WARNING("KisTA","Application visibility unset. This is an unexpected situation while drawing application boxes. They will not be drawn. Please, contact the authors.");
					break;
				case SET_IMPLICIT_APPS:
					// draw for a single application (default application)
					// A single box (default application) without label wraps all the tasks
					*sketch_file << "\\begin{scope}[on background layer]" << endl;
					*sketch_file << "\\node[app_box_st,fit = ";
					for(task_it=task_info_by_name.begin(); task_it != task_info_by_name.end(); task_it++) {
						*sketch_file << "(";
						*sketch_file << task_it->first;
						*sketch_file << ")";
					}
					*sketch_file << "] (app_box) {};" << endl;
					*sketch_file << "\\end{scope}" << endl;
					*sketch_file << "\\node [right=of app_box] {$Application$};" << endl;
					break;
				case SET_EXPLICIT_APPS:
					// draw for a multiple applications
					// A box per application wraps its tasks. Each application box has a corresponding label
					*sketch_file << "\\begin{scope}[on background layer]" << endl;
					app_it=applications_by_name.begin();
					while(app_it!=applications_by_name.end()) {
						*sketch_file << "\\node[app_box_st,fit = ";
						task_it = app_it->second->tasks_by_name.begin(); // now task_it points to the first element in the application task list
						while(task_it!=app_it->second->tasks_by_name.end()) {
							*sketch_file << "(";
							*sketch_file << task_it->first;
							  // now check if this app is the one which contains the last task node (in the right part of the draw)
							if(task_it->first == last_task_id_name) { // string comparison
								last_app_name=app_it->first;
							}
							*sketch_file << ")";
							task_it++;
						}
						*sketch_file << ",label=\\color{red}" << app_it->first; // prints the app name as box label in red
						*sketch_file << "] (" << app_it->first << "_app_box) {};" << endl;
						app_it++;
					}
					*sketch_file << "\\end{scope}" << endl;
					*sketch_file << "\\node [right=of " << last_app_name << "_app_box] {$Applications$};" << endl;
					break;				
				default:
					SC_REPORT_WARNING("KisTA","Unexpected situation while drawing application boxes. They will not be drawn. Please, contact the authors.");
			}
		}
		
		if(platform_box) {
			*sketch_file << "\\begin{scope}[on background layer]" << endl;
			*sketch_file << "\\node[plat_box_st,fit = ";
			for(i=0; i<scheds.size(); i++) {
				sched_id_name = scheds[i]->name();
				*sketch_file << "(";
				*sketch_file << sched_id_name;
				*sketch_file << ")";				
			}
			for(i=0; i<pes.size(); i++) {
				pe_id_name = pes[i]->name();
				*sketch_file << "(";
				*sketch_file << pe_id_name;
				*sketch_file << ")";					
			}
			for(i=0;i<phy_commres_set.size();i++) {
				commres_id_name = phy_commres_set[i]->name();
				*sketch_file << "(";
				*sketch_file << commres_id_name;
				*sketch_file << ")";
			}
			for(i=0;i<tdma_buses.size();i++) {
				commres_id_name = tdma_buses[i]->name();
				*sketch_file << "(";
				*sketch_file << commres_id_name;
				*sketch_file << ")";				
			}			
			*sketch_file << "] (plat_box) {};" << endl;
			*sketch_file << "\\end{scope}" << endl;
			*sketch_file << "\\node [right=of plat_box] {$Platform$};" << endl;			
		}		
	}
	
}

void sketch_report_t::draw_system_level_connections() {
	
	std::string rpt_msg;
	std::string src_task_id_name, dest_task_id_name;
	// to iterate system-level channels
	std::map<logic_link_t, std::string>::iterator sys_conn_types_it;
	
	for(sys_conn_types_it=sys_conn_types.begin(); sys_conn_types_it != sys_conn_types.end();sys_conn_types_it++) {
		
		// get src task name
		src_task_id_name = sys_conn_types_it->first.src->name();
		dot_by_slash(src_task_id_name);

		// get dest task name
		dest_task_id_name = sys_conn_types_it->first.dest->name();
		dot_by_slash(dest_task_id_name);
		
		*sketch_file << "\\draw (";		
		*sketch_file << src_task_id_name;
		*sketch_file << ") [";
		
		// style of arrow for representing the system-level connection
		if(sys_conn_types_it->second=="fifo_buffer") {
		    *sketch_file << "fifo_buffer_style";
		} else {
			*sketch_file << "logic_link_style";
			rpt_msg = "Drawing of system-level channel of type";
			rpt_msg += sys_conn_types_it->second; 
			rpt_msg += "not supported. Default logic link representation will be used for them.";
			SC_REPORT_WARNING("KisTA",rpt_msg.c_str());
		}
		if(with_sys_level_conn_names) {
			*sketch_file << ", ";
			// template
			// postaction={decorate,decoration={text along path,text align=center,text={$USER_TEXT{\;}$}}}
			*sketch_file << "postaction={decorate,decoration={text along path,text align=center,text={$";
			logic_link_t cur_llink = sys_conn_types_it->first;
			*sketch_file << cur_llink.get_link_name();
			//*sketch_file << sys_conn_types_it->first.get_link_name();
			*sketch_file << "{\\;}$}}}";
		}
		*sketch_file << "] ";
				
		calculate_angles(src_task_id_name,dest_task_id_name);
		
		*sketch_file << " (";
		*sketch_file << dest_task_id_name;
		*sketch_file << ");";
		*sketch_file << endl;
	}

}


void sketch_report_t::draw_system_level_connections_random() {
	
	std::string rpt_msg;
	std::string task_id_name;
	// to iterate system-level channels
	std::map<logic_link_t, std::string>::iterator sys_conn_types_it;
	
	for(sys_conn_types_it=sys_conn_types.begin(); sys_conn_types_it != sys_conn_types.end();sys_conn_types_it++) {
		
		*sketch_file << "\\draw (";
		
		// get src task name
		task_id_name = sys_conn_types_it->first.src->name();
		dot_by_slash(task_id_name);
		
		*sketch_file << task_id_name;
		*sketch_file << ") [";
		
		// style of arrow for representing the system-level connection
		if(sys_conn_types_it->second=="fifo_buffer") {
		    *sketch_file << "fifo_buffer_style";
		} else {
			*sketch_file << "logic_link_style";
			rpt_msg = "Drawing of system-level channel of type";
			rpt_msg += sys_conn_types_it->second; 
			rpt_msg += "not supported. Default logic link representation will be used for them.";
			SC_REPORT_WARNING("KisTA",rpt_msg.c_str());
		}
		
		//*sketch_file << "] -- (";
		*sketch_file << "] ";
		*sketch_file << "to[out=";
		*sketch_file << calculate_out_angle();
		*sketch_file << ",in=";
		*sketch_file << calculate_in_angle();
		*sketch_file << "] (";
		
		// get dest task name
		task_id_name = sys_conn_types_it->first.dest->name();
		dot_by_slash(task_id_name);
		
		*sketch_file << task_id_name;
		*sketch_file << ");";
		*sketch_file << endl;
	}

}

// In order to facilitate the drawing of task links without overlapping,
// the following marcro is used: http://tex.stackexchange.com/questions/27899/automatically-connect-nodes-without-overlapping-other-nodes-or-connections
//

/*
\usetikzlibrary{calc}

\makeatletter
\tikzset{
  through point/.style={
    to path={%
      \pgfextra{%
        \tikz@scan@one@point\pgfutil@firstofone(\tikztostart)\relax
        \pgfmathsetmacro{\pt@sx}{\pgf@x * 0.03514598035}%
        \pgfmathsetmacro{\pt@sy}{\pgf@y * 0.03514598035}%
        \tikz@scan@one@point\pgfutil@firstofone#1\relax
        \pgfmathsetmacro{\pt@ax}{\pgf@x * 0.03514598035 - \pt@sx}%
        \pgfmathsetmacro{\pt@ay}{\pgf@y * 0.03514598035 - \pt@sy}%
        \tikz@scan@one@point\pgfutil@firstofone(\tikztotarget)\relax
        \pgfmathsetmacro{\pt@ex}{\pgf@x * 0.03514598035 - \pt@sx}%
        \pgfmathsetmacro{\pt@ey}{\pgf@y * 0.03514598035 - \pt@sy}%
        \pgfmathsetmacro{\pt@len}{\pt@ex * \pt@ex + \pt@ey * \pt@ey}%
        \pgfmathsetmacro{\pt@t}{(\pt@ax * \pt@ex + \pt@ay *           \pt@ey)/\pt@len}%
        \pgfmathsetmacro{\pt@t}{(\pt@t > .5 ? 1 - \pt@t : \pt@t)}%
        \pgfmathsetmacro{\pt@h}{(\pt@ax * \pt@ey - \pt@ay *           \pt@ex)/\pt@len}%
        \pgfmathsetmacro{\pt@y}{\pt@h/(3 * \pt@t * (1 - \pt@t))}%
      }
      (\tikztostart) .. controls +(\pt@t * \pt@ex + \pt@y * \pt@ey, \pt@t * \pt@ey - \pt@y * \pt@ex) and +(-\pt@t * \pt@ex + \pt@y * \pt@ey, -\pt@t * \pt@ey - \pt@y * \pt@ex) .. (\tikztotarget)
    }
  }
}

\makeatother
*/

void sketch_report_t::insert_TO_tikz_macro() {

	*sketch_file << "\\makeatletter" << endl;
	*sketch_file << "\\tikzset{" << endl;
	*sketch_file << "  through point/.style={" << endl;
	*sketch_file << "    to path={%" << endl;
	*sketch_file << "      \\pgfextra{%" << endl;
	*sketch_file << "        \\tikz@scan@one@point\\pgfutil@firstofone(\\tikztostart)\\relax" << endl;
	*sketch_file << "        \\pgfmathsetmacro{\\pt@sx}{\\pgf@x * 0.03514598035}\%" << endl;
	*sketch_file << "        \\pgfmathsetmacro{\\pt@sy}{\\pgf@y * 0.03514598035}\%" << endl;
	*sketch_file << "        \\tikz@scan@one@point\\pgfutil@firstofone#1\\relax" << endl;
	*sketch_file << "        \\pgfmathsetmacro{\\pt@ax}{\\pgf@x * 0.03514598035 - \\pt@sx}\%" << endl;
	*sketch_file << "        \\pgfmathsetmacro{\\pt@ay}{\\pgf@y * 0.03514598035 - \\pt@sy}%" << endl;
	*sketch_file << "        \\tikz@scan@one@point\\pgfutil@firstofone(\\tikztotarget)\\relax" << endl;
	*sketch_file << "        \\pgfmathsetmacro{\\pt@ex}{\\pgf@x * 0.03514598035 - \\pt@sx}%" << endl;
	*sketch_file << "        \\pgfmathsetmacro{\\pt@ey}{\\pgf@y * 0.03514598035 - \\pt@sy}%" << endl;
	*sketch_file << "        \\pgfmathsetmacro{\\pt@len}{\\pt@ex * \\pt@ex + \\pt@ey * \\pt@ey}%" << endl;
	*sketch_file << "        \\pgfmathsetmacro{\\pt@t}{(\\pt@ax * \\pt@ex + \\pt@ay *           \\pt@ey)/\\pt@len}%" << endl;
	*sketch_file << "        \\pgfmathsetmacro{\\pt@t}{(\\pt@t > .5 ? 1 - \\pt@t : \\pt@t)}%" << endl;
	*sketch_file << "        \\pgfmathsetmacro{\\pt@h}{(\\pt@ax * \\pt@ey - \\pt@ay *           \\pt@ex)/\\pt@len}%" << endl;
	*sketch_file << "        \\pgfmathsetmacro{\\pt@y}{\\pt@h/(3 * \\pt@t * (1 - \\pt@t))}%" << endl;
	*sketch_file << "      }" << endl;
	*sketch_file << "      (\\tikztostart) .. controls +(\\pt@t * \\pt@ex + \\pt@y * \\pt@ey, \\pt@t * \\pt@ey - \\pt@y * \\pt@ex) and +(-\\pt@t * \\pt@ex + \\pt@y * \\pt@ey, -\\pt@t * \\pt@ey - \\pt@y * \\pt@ex) .. (\\tikztotarget)" << endl;
	*sketch_file << "    }" << endl;
	*sketch_file << "  }" << endl;
	*sketch_file << "}" << endl;

	*sketch_file << "\\makeatother" << endl;

}



unsigned int sketch_report_t::calculate_in_angle() {
	return rand()%360;
}

unsigned int sketch_report_t::calculate_out_angle() {
	return rand()%360;
}


void sketch_report_t::calculate_angles(std::string src_task_id, std::string dest_task_id) {
		bool src_in_sys, src_in_env;
		bool dest_in_sys, dest_in_env;
		unsigned int src_pos, dest_pos;
		unsigned int src_angle,dest_angle;
		//unsigned int distance;
	
		src_in_env = env_tasks_by_name.find(src_task_id)!=env_tasks_by_name.end();
		src_in_sys = task_info_by_name.find(src_task_id)!=task_info_by_name.end();
		
		dest_in_env = env_tasks_by_name.find(dest_task_id)!=env_tasks_by_name.end();
		dest_in_sys = task_info_by_name.find(dest_task_id)!=task_info_by_name.end();
		
		if(src_in_sys && dest_in_sys) {
			
			// retrieve tasks indexes
			src_pos = task_position[src_task_id];
			dest_pos = task_position[dest_task_id];
			
			if(src_pos<dest_pos) { // from left to right
				//distance = dest_pos - src_pos;
				src_angle = 30 + outs[src_pos]*DEGREES_OUTPUTS_SEPARATION; 			// adds 10 degrees per output
				dest_angle = 150 - inps[dest_pos]*DEGREES_INPUTS_SEPARATION;	// subst 10 degrees per input
			} else if(src_pos>dest_pos) {  // from right to left
				//distance = src_pos - dest_pos;
				src_angle = 210 + outs[src_pos]*DEGREES_OUTPUTS_SEPARATION; 		// adds 10 degrees per output
				dest_angle = 330 - inps[dest_pos]*DEGREES_INPUTS_SEPARATION;	// subst 10 degrees per input			
			} else { // (src_pos==dest_pos) right auto arrow
				src_angle = 95; 	// from right to left comming to the same buble
				dest_angle = 85;	//
			}
			
			// updates inps and outs vector for system
			outs[src_pos]++;
			inps[dest_pos]++;
			
		} else if(src_in_sys && dest_in_env) {
			
			// retrieve tasks indexes
			src_pos = task_position[src_task_id];
			dest_pos = env_task_position[dest_task_id];
						
			// form sys to env: SYSTEM OUTPUS
			src_angle = 80 - env_inps[src_pos]*DEGREES_OUTPUTS_SEPARATION; 			// adds 10 degrees per output
			dest_angle = 280 + outs[dest_pos]*DEGREES_INPUTS_SEPARATION;	// subst 10 degrees per input
			
			// updates inps and outs vector for system and environment
			outs[src_pos]++;
			env_inps[dest_pos]++;
			
		} else if(src_in_env && dest_in_sys) {
			
			// retrieve tasks indexes
			src_pos = env_task_position[src_task_id];
			dest_pos = task_position[dest_task_id];
						
			// form env to sys: SYSTEM INPUTS
			src_angle = 240 + env_outs[src_pos]*DEGREES_OUTPUTS_SEPARATION; 			// adds 10 degrees per output
			dest_angle = 120 - inps[dest_pos]*DEGREES_INPUTS_SEPARATION;	// subst 10 degrees per input
			
			// updates inps and outs vector for system and environment
			env_outs[src_pos]++;
			inps[dest_pos]++;
							
		} else { // env to env connection
			// retrieve tasks indexes
			src_pos = env_task_position[src_task_id];
			dest_pos = env_task_position[dest_task_id];
					
			if(src_pos<dest_pos) { // from left to right
				//distance = dest_pos - src_pos;
				src_angle = 30 + outs[src_pos]*DEGREES_OUTPUTS_SEPARATION; 			// adds 10 degrees per output
				dest_angle = 150 - inps[dest_pos]*DEGREES_INPUTS_SEPARATION;	// subst 10 degrees per input
			} else if(src_pos>dest_pos) {  // from right to left
				//distance = src_pos - dest_pos;
				src_angle = 210 - outs[src_pos]*DEGREES_OUTPUTS_SEPARATION; 		// adds 10 degrees per output
				dest_angle = 330 + inps[dest_pos]*DEGREES_INPUTS_SEPARATION;	// subst 10 degrees per input			
			} else { // (src_pos==dest_pos) right auto arrow
				src_angle = 95; 	// from right to left comming to the same buble
				dest_angle = 85;	//
			}
			
			// updates inps and outs vector for environment
			env_outs[src_pos]++;
			env_inps[dest_pos]++;
		}
		
		//*sketch_file << "] -- (";
		*sketch_file << "to[out=";
		*sketch_file << src_angle;
		*sketch_file << ",in=";
		*sketch_file << dest_angle;
		*sketch_file << "]";
}

} // end kista namespace

#endif
