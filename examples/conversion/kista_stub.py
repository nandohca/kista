# coding=utf-8
#
# KisTA simulator wrapper for M3Explorer/MOST Design Space Exploration.
# It creates the correct input KisTA XML for the system parameters.
# 
# This script expects the following four parameters:
# --xml_system_description=kista_system.xml
# --xml_system_configuration=sc_path_name.xml
# --xml_system_metrics=sm_path_name.xml
# --reference_xsd=xsd_file_name.xsd

import argparse
import xml.dom.minidom as minidom
import os
import sys


# Command line parser
parser = argparse.ArgumentParser(description='KisTA simulator wrapper for M3Explorer/MOST Design Space Exploration.')
parser.add_argument('-s','--xml_system_description',   help='XML file describing the KisTA parameterized system (platform+application)',  required=True, type=str, dest='sd_file_name')
parser.add_argument('-c','--xml_system_configuration', help='XML file describing the system configuration to be passed to the simulator', required=True, type=str, dest='sc_file_name')
parser.add_argument('-m','--xml_system_metrics',       help='Output XML file used by the simulator for producing the system metrics',     required=True, type=str, dest='sm_file_name')
parser.add_argument('-x','--reference_xsd',            help='Reference M3Explorer/MOST simulator interface XSD file in the file system',  required=True, type=str, dest='xsd_file_name')

# Application model
# as in the KisTA XML template
tasks=[]
BE=[]
edges=[]
# Platform model
# as in the KisTA XML template
procs=[]
scheds=[]
# task mapping (size = #TASKs)
par_task_map=[]
BE_map = []
# task and comm schedules (size = #RTOSs)
par_task_sched=[]
par_comm_sched=[]
# TDMA slots
n_slots = 0


def parse_kista_sample(kista_sd_path):
	global n_slots
	# Load xml tree from file
	doc = minidom.parse(kista_sd_path)
	# Application model
	for app in doc.getElementsByTagName('application'):
		# Tasks (graph nodes)
		for task in app.getElementsByTagName('task'):
			for item in task.getElementsByTagName('priority'):
				if int(item.getAttribute('value')) == 1:
					tasks.append(task.getAttribute('name'))
					par_task_map.append(0)
				if int(item.getAttribute('value')) == 3:
					BE.append(task.getAttribute('name'))
					BE_map.append(0)
		# Channels (graph edges)
		
		for edge in app.getElementsByTagName('comm_synch'):
			# Do not consider I/O edges
			if edge.getAttribute('io').lower() == "yes":
				continue
			name = edge.getAttribute('name')
			if 'fifo' in name:
				continue
			src = tasks.index(edge.getAttribute('src'))
			dst = tasks.index(edge.getAttribute('dest'))
			edges.append((name,src,dst))
	# Platform mapping (RTOS to PEs)
	for map_rtos in doc.getElementsByTagName('map_rtos'):
		scheds.append(map_rtos.getAttribute('source'))
	# Platform model (HW_platform)
	# N.B.: assuming that only one HW_Architecture available
	hw_plt = doc.getElementsByTagName('HW_Architecture')
	# PEs (Processing Elements)
	for pe in hw_plt[0].getElementsByTagName('PE'):
		procs.append(pe.getAttribute('name'))
	# TDMA bus
	for comm_res in hw_plt[0].getElementsByTagName('comm_res'):
		if comm_res.getAttribute('type') == 'tdma_bus':
			for item in comm_res.childNodes:
				if item.localName=='slots_number':
					n_slots = int(item.getAttribute('value'))
					print ('nslots is: ',n_slots)
	print (tasks)
	print (BE)
def get_param(params, param_name):
	ret = None
	# Iterate over the list of parameters
	for p in params:
		if p.getAttribute("name") == param_name:
			ret = p
			break
	assert (ret != None)
	return ret

def parse_params(params):

	# intermediate vector parameters
	global_task_sched=([0] * len(tasks))
	global_comm_sched=([0] * len(edges))

	# create simulator xml input (HW architecture)
	doc=minidom.Document()
	root=doc.createElementNS("http://www.multicube.eu/", "simulator_input_interface")
	root.setAttribute("xmlns", "http://www.multicube.eu/")
	root.setAttribute("version", "1.4")
	doc.appendChild(root)

	param = get_param(params, "N_TASKS")
	par_n_tasks = int(param.getAttribute("value"))
	print (len(tasks))
	assert (par_n_tasks == len(tasks))

	param = get_param(params, "N_EDGES")
	par_n_edges = int(param.getAttribute("value"))
	assert (par_n_edges == len(edges))

	param = get_param(params, "N_PES")
	par_n_procs = int(param.getAttribute("value"))
	assert (par_n_procs == len(procs))

	param = get_param(params, "N_SLOTS")
	par_n_slots = int(param.getAttribute("value"))
	assert (par_n_slots == n_slots)

	param = get_param(params, "N_SCHEDS")
	par_n_scheds = int(param.getAttribute("value"))
	assert (par_n_scheds == len(scheds))

	# No conversion required, same XML format for PE parameters
	for i in range(0, len(procs)):
		# PE frequency
		param = get_param(params, ("%s_FREQ" % procs[i]))
		root.appendChild(param)
		# PE memory
		param = get_param(params, ("%s_MEM" % procs[i]))
		root.appendChild(param)
		# PE TDMA slots
		param = get_param(params, ("%s_SLOTS" % procs[i]))
		root.appendChild(param)

	# Task ID to task name conversion required by KisTA
	for i in range(0, len(tasks)):
		# Task map
		param = get_param(params, ("T%d_MAP" % (i+1)))
		cnt = 0
		for item in param.childNodes:
			if item.localName=="item" and item.getAttribute("value")=="1":
				j = int(item.getAttribute("index")) - 1
				cnt += 1
		# Assume one task mapped to a single PE
		assert (cnt == 1)
		par_task_map[i] = j

	for i in range(0, len(BE)):
		# Task map
		param = get_param(params, ("BE%d_MAP" % (i+1)))
		cnt = 0
		for item in param.childNodes:
			if item.localName=="item" and item.getAttribute("value")=="1":
				j = int(item.getAttribute("index")) - 1
				cnt += 1
		# Assume one task mapped to a single PE
		assert (cnt == 1)
		BE_map[i] = j



	# Global task schedule
	param = get_param(params, "TASK_SCHED")
	for item in param.childNodes:
		if item.localName=="item":
			i = int(item.getAttribute("position")) - 1
			j = int(item.getAttribute("value")) - 1
			global_task_sched[i] = j

	# Global comm schedule
	param = get_param(params, "COMM_SCHED")
	for item in param.childNodes:
		if item.localName=="item":
			i = int(item.getAttribute("position")) - 1
			j = int(item.getAttribute("value")) - 1
			global_comm_sched[i] = j

	# Local schedules for each RTOS
	for i in range(0, par_n_scheds):
		par_task_sched.append([])
		par_comm_sched.append([])

	# task schedule vs. mapping
	# WARNING: task schedule nust follow task mapping
	for i in range(0, par_n_tasks):
		j = global_task_sched[i]
		k = par_task_map[j]
		par_task_sched[k].append(j)

	# comm schedule vs. mapping
	# WARNING: comm schedule must follow task mapping
	for i in range(0, par_n_edges):
		j = global_comm_sched[i]
		(_,src,_) = edges[j]
		src_map = par_task_map[src]
		par_comm_sched[src_map].append(j)

	return doc

def print_params():
	# mapping of tasks to task schedulers
	for i in range(0, len(tasks)):
		j = par_task_map[i]
		print "[kista stub] T%d_MAP (%s) : %s" % ((i+1), tasks[i], scheds[j])
	# for each scheduler
	for i in range(0, len(scheds)):
		# static task schedule
		str_sched=""
		for j in par_task_sched[i]:
			str_sched += (" "+tasks[j])
		print "[kista stub] TASK %s :%s" % (scheds[i], str_sched)
		# static comm schedule
		str_sched=""
		for j in par_comm_sched[i]:
			(name,src,dst) = edges[j]
			str_sched += (" ("+name+", "+tasks[src]+"->"+tasks[dst]+")")
		print "[kista stub] COMM %s :%s" % (scheds[i], str_sched)

def gen_kista_sc(doc, kista_sc_path):
	# Get tree root
	root = doc.documentElement
	# Task mapping
	for i in range(0, len(tasks)):
		node=doc.createElement("parameter")
		node.setAttribute("name", tasks[i]+"_MAP")
		node.setAttribute("value", scheds[par_task_map[i]])
		root.appendChild(node)
		
	for i in range(0, len(BE)):
		node=doc.createElement("parameter")
		node.setAttribute("name", BE[i]+"_MAP")
		node.setAttribute("value", scheds[BE_map[i]])
		root.appendChild(node)
	# Static task scheduling
	for i in range(0,len(scheds)):
		node=doc.createElement("parameter")
		node.setAttribute("name", "STATIC_"+scheds[i].upper())
		for j in par_task_sched[i]:
			item=doc.createElement("item")
			item.setAttribute("value", tasks[j])
			node.appendChild(item)
		root.appendChild(node)
	# Static comm scheduling
	for i in range(0,len(scheds)):
		node=doc.createElement("parameter")
		node.setAttribute("name", "STATIC_COMM_"+scheds[i].upper())
		for j in par_comm_sched[i]:
			(name,src,dst) = edges[j]
			# do not schedule local channels
			# i.e. when src and dst are mapped to the same PE
			if par_task_map[src] != par_task_map[dst]:
				item=doc.createElement("item")
				item.setAttribute("value", name)
				node.appendChild(item)
		root.appendChild(node)
	# writing output in the same directory of input
	outputString=doc.toprettyxml()
	doc.unlink()
	f=open(kista_sc_path, "w")
	f.write(outputString)
	f.close()

def simulator_error(reason, outputFilename):
	# create simulator xml output
	doc=minidom.Document()
	root=doc.createElementNS("http://www.multicube.eu/", "simulator_output_interface")
	root.setAttribute("xmlns", "http://www.multicube.eu/")
	root.setAttribute("version", "1.4")
	doc.appendChild(root)
	# error output
	errorNode=doc.createElement("error")
	errorNode.setAttribute("reason", reason)
	errorNode.setAttribute("kind", "fatal")
	root.appendChild(errorNode)
	print "[kista stub] fatal error: %s" % reason
	# writing output in the same directory of input
	outputString=doc.toprettyxml()
	doc.unlink()
	f=open(outputFilename, "w")
	f.write(outputString)
	f.close()

def main(sd_file_name, sc_file_name, sm_file_name, xsd_file_name):
	# validation
	validation = (os.system("/bin/bash -c \"xmllint --schema %s %s >& /dev/null\"" \
		% (xsd_file_name, sc_file_name)) == 0)
	if not validation:
		# report error
		simulator_error("validation failed", sm_file_name)
	else:
		# read KisTA application and platform model
		parse_kista_sample(sd_file_name)
		# read input configuration
		doc = minidom.parse(sc_file_name)
		params = doc.getElementsByTagName("parameter")
		# parse input configuration
		kista_sc = parse_params(params)
		# debug
#		print_params()
		# generate KisTA system configuration
		gen_kista_sc(kista_sc, 'sys_params.xml')
		# run KisTA simulator with input confguration
		#		os.system("../../scripts/kista/run_kista.sh "+sm_file_name)
		
		###qua vanno inserite le modifiche. togliere run_kista che va lanciato dal global script. oppure inserire integraz.py
		curr_path_kista_wrapper = os.path.dirname(os.path.abspath(__file__))
		os.system(curr_path_kista_wrapper+"/run_kista.sh "+sm_file_name)
		

if __name__=="__main__":
	args = parser.parse_args()
	main(args.sd_file_name, args.sc_file_name, args.sm_file_name, args.xsd_file_name)
	sys.exit(0)

