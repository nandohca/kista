import csv
import xml.etree.ElementTree as ET
import itertools
import re
import copy
import argparse
import datetime
import os
import subprocess
import threading



parser = argparse.ArgumentParser(description='iNARK wrapper for the CONTREX chain')
parser.add_argument('-s','--xml_system_description',   help='XML file describing the design space on the system (platform+application)',  required=True, type=str, dest='sd_file_name')
parser.add_argument('-c','--csv_system_options', help='csv file describing the options validated by the ADSE tool', required=True, type=str, dest='data_file_name')
parser.add_argument('-d', '--debug', help='save temp files during the process', dest='debug_flag', action='store_true')
parser.set_defaults(debug_flag=False)
parser.add_argument('-t','--thread_number', help='number of threads to spawn', required=False, type=int, dest='thread_number')
parser.set_defaults(thread_number=1)

iNARKExe = "/home/emanuele/iNARK/build/src/iNARK"

########### this script uses design space xml AND the csv file. 
def get_BE_tasks():
  
	ET.register_namespace('', "http://www.multicube.eu/")
	paramsTree = ET.parse(args.sd_file_name)
	root = paramsTree.getroot()
	NS = '{http://www.multicube.eu/}'
	BE_vect = []
	for child in root:
		#print (child.tag, child.attrib)
		for elem in child.findall(NS + 'parameter'):
			#print (elem)
			if('BE' in elem.attrib.get('name')):
				var = (elem.attrib.get('name').split('_')[0])
				print (var)
				BE_vect.append(var)

	return BE_vect


def get_tasks():
  
	ET.register_namespace('', "http://www.multicube.eu/")
	paramsTree = ET.parse(args.sd_file_name)
	root = paramsTree.getroot()
	NS = '{http://www.multicube.eu/}'
	BE_vect = []
	pattern = re.compile('T[0-9]+_MAP')
	print (pattern)
	for child in root:
		#print (child.tag, child.attrib)
		for elem in child.findall(NS + 'parameter'):
			print (elem.attrib.get('name'))
			if(pattern.match(elem.attrib.get('name'))):
				var = (elem.attrib.get('name').split('_')[0])
				print (var)
				BE_vect.append(var)

	return BE_vect

def get_scheds():
  
	ET.register_namespace('', "http://www.multicube.eu/")
	paramsTree = ET.parse(args.sd_file_name)
	root = paramsTree.getroot()
	NS = '{http://www.multicube.eu/}'
	BE_vect = []
	for child in root:
		#print (child.tag, child.attrib)
		for elem in child.findall(NS + 'parameter'):
			#print (elem)
			if('N_SCHEDS' in elem.attrib.get('name')):
				var = (elem.attrib.get('max'))
				print (var)
				for x in range(0, int(var)):
					tmp = (x+1)
					print (tmp)
					BE_vect.append(tmp)
					

	return BE_vect


def explore(parameters, plan, conf):
	"""
	Recursevely build the plan
	"""
	# if we are the last then bail out
	if not parameters:
		plan.append(conf)
		return
	# take first key of the parameters
	param_key = [ x for x in parameters.keys()][0]
	# create a duplicate of the params
	new_params = dict(parameters)
	# delete the current configuration
	del new_params[param_key]
	# loop over the values
	for value in parameters[param_key]:
		# duplicate and add the value to the configuration
		this_conf = list(conf)
		# append the current value
		this_conf.extend([param_key, str(value)])
		# go deeper
		explore(new_params, plan, this_conf)

def main():
	BE_tasks = get_BE_tasks()
	print(BE_tasks)
	schedulers = get_scheds()
	print (schedulers)
	tasks = get_tasks()
	print (tasks)
	



	tasks_sched_pairs = {}
	for task in BE_tasks:
		tasks_sched_pairs[task] = []
	for element in itertools.product(BE_tasks,schedulers):
		tasks_sched_pairs[element[0]].append(element[1])
		
	print ('taskpairs is:', tasks_sched_pairs)

	critical_BE_pairs = list(itertools.product(BE_tasks, tasks))
#	print ('critical be pairs is:', critical_BE_pairs)
#	print (critical_BE_pairs[0][0])

	plan = []
	explore(tasks_sched_pairs, plan, [])
	print ('\n')
	print ('plan is: ',plan)

	final_configurations_map = {}
	for index, elem in enumerate(plan):
		print (elem)
		final_configurations_map[index] = {}
		x = 0
		while x < len(elem):
			final_configurations_map[index][elem[x]] = elem[x+1]
			x = x+2
			
	print (final_configurations_map)


	f=open(args.data_file_name,'r')

	reader=csv.reader(f,delimiter=';')
	
	data = []
	for row in reader:
		data.append(row)
	
	header = data[0]
	data.pop(0)
	
	
	
	associazione = {}
	for index,value in enumerate(header):
		if value:
			associazione[value] = index
	
	
	splitting_column = associazione['TASK_SCHED']
	new_header = []
	new_header = header[0:splitting_column]
	for task in BE_tasks:
		new_header.append(task+'_MAP')
		
	new_header+= header[splitting_column:]
	
	"""this loop is needed to call the inark with the desired configuration and check if feasible.
	then if it is ok, the csv has to be modified adding and editing some columns.
	the points to be modified are: adding BE mapping, N tasks, TASK_SCHED column (?) 
	note that 1 row has to be attempted with all the final configuration map so the final line num of the csv is inizial line num * final_configurations_map - invalid configurations"""
	if (args.debug_flag!=0):
		print ('debug_flag is true',args.debug_flag)
	else:
		print ('debug_flag is false')

	paramsTree = ET.parse('inark_plat.xml')
	root = paramsTree.getroot()
	components = root.find('components')
	if element is None:
		print ("element not found")
		return
	      
	
	for task in tasks:
		a = ET.SubElement(components, 'component')
		b = ET.SubElement(a, 'layer')
		b.text = '2'
		b = ET.SubElement(a, 'name')
		b.text = task
	for task in BE_tasks:
		a = ET.SubElement(components, 'component')
		b = ET.SubElement(a, 'layer')
		b.text = '2'
		b = ET.SubElement(a, 'name')
		b.text = task
	dt = datetime.datetime.now().replace(second=0, microsecond=0)
	d = os.path.dirname('debug'+str(dt)+'/')
	if not os.path.exists(d):
		os.makedirs(d)
	d1 = os.path.dirname('debug_fails'+str(dt)+'/')
	if not os.path.exists(d1):
		os.makedirs(d1)
	
	
	
	
			
	#introduce parallelism here.
	#dividi in #thread le righe. scrivi su tmp o su file mentre in thread?
	data_splitted = split_list(data,args.thread_number)
	
	threads = []
	new_data = []
# Create new threads
	for i in range (0, args.thread_number):
		f2=open("temp_data_"+str(i)+".csv", 'w')
		new_data.append(f2)
		tmp_thr = myThread(i, "Thread-"+str(i), schedulers, associazione,paramsTree,splitting_column,final_configurations_map,data_splitted[i],f2,tasks,critical_BE_pairs,d,d1)
		threads.append(tmp_thr)

	for t in threads:
		t.start()

# Wait for all threads to complete
	for t in threads:
		t.join()
	
	#call threads
	f2=open("esempio_out.csv", 'w')
	f2.write(new_header)
	for partial_csv in new_data:
		f = open(partial_csv.name, 'r')
		for line in f:
			f2.write(line)
	
	
	#returns:join files
	
	
	#finalize

	#write = csv.writer(f2, delimiter=';',quoting=csv.QUOTE_ALL)
	#write.writerow(new_header)
	#for line in new_data:
	#	write.writerow(line)
	
	
	
class myThread (threading.Thread):
	def __init__(self, threadID, name, schedulers, associazione,paramsTree,splitting_column,final_configurations_map,data,new_data,tasks,critical_BE_pairs,d,d1):	#se passo newdata a tt i thread dovrebbe scrivere non in ordine ma chi se l'incula l'ordine.
		threading.Thread.__init__(self)
		self.threadID = threadID
		self.name = name
		self.schedulers = schedulers
		self.associazione= associazione
		self.paramsTree= paramsTree
		self.splitting_column= splitting_column
		self.final_configurations_map=final_configurations_map
		self.data=data
		self.new_data=new_data
		self.tasks= tasks
		self.critical_BE_pairs=critical_BE_pairs
		self.d=d
		self.d1=d1
	def run(self):
		
		temp_sched = {}
		counter = 0
		write = csv.writer(self.new_data, delimiter=';',quoting=csv.QUOTE_ALL)
		
		for row in self.data:
			for i in range (0, len(self.schedulers)): #reset every loop
				temp_sched[i]=[]
			for task in self.tasks: #build scheduler for the single configuration, gets where every task is mapped
			  
				value = row[self.associazione[task+'_MAP']]
				#print (task, value)
				st = filter(None, re.split("[\-()]", value))
				for index, i in enumerate(st):
					if i == str(1):
						temp_sched[index].append(task)

			#print (temp_sched)
			
			
			for value in self.final_configurations_map:  #for every possible BE mapping, creates the inark file and tests it, given the critical mapping recovered by the csv
				temptree = copy.deepcopy(self.paramsTree)
				root = temptree.getroot()
				edges = root.find('edges')
				be_temp_sched = copy.deepcopy(temp_sched)
				#t1:schedy
				
				#print(len(row))
				temp = row[0:self.splitting_column]
				for aaaa in self.final_configurations_map[value]:
					x = []
					for z in range (0, len (self.schedulers)):
						x.append(0)
					#print (aaaa,'value is: ', final_configurations_map[value][aaaa])
					be_temp_sched[int (self.final_configurations_map[value][aaaa])-1].append(aaaa)
				#print (row[0:splitting_column])
					x[int(self.final_configurations_map[value][aaaa])-1] = 1
					temp.append('({0})'.format('-'.join(str(y) for y in x))) #vanno messi uno per volta
					#print ('({0})'.format('-'.join(str(y) for y in x)))
				temp+=(row[self.splitting_column:])
				#	print (temp)
				
				#print(be_temp_sched)
				#print('*****')
				### manca ancora la cd e il nome del file. ma siamo sulla buona strada
				##controllo porta ha senso? basta fare che ogni sched ha abbastanza porte per tutti i task e non dovrebbe servire. parlane con gp
				
				for sched in be_temp_sched:
					for index, task in enumerate(be_temp_sched[sched]):
						a = ET.SubElement(edges, 'edge')
						b = ET.SubElement(a, 'from')
						c = ET.SubElement(b, 'name')
						c.text = task
						b = ET.SubElement(a, 'to')
						c = ET.SubElement(b, 'name')
						c.text ='sched_'+str(sched+1)
						c = ET.SubElement(b, 'port')
						c.text = str(index+1)
				filename_src = self.name+str(counter)+'_inark.xml'
				logname_src =  self.name+str(counter)+'.log'
				counter+=1
				##with this trick it is possible to avoid to change directory. the files will be saved only if debug is on, and the configuration is accepted. otherwise it will be removed.
				##the directory is removed if empty (shoud happen only if debug is false)
				filename = self.d+'/'+filename_src
				temptree.write(filename)
				###lancia test
				##funzione che prende le due liste in input, e ritorna true se va tutto bene o false se la configurazione non è valida.
				##ciclo da spostare fuori: creata once use always.
				
				
				logname = self.d1+'/'+logname_src
				log=open(logname,'w')
				
				for pair in self.critical_BE_pairs:
					testFailed=False
					result = runINARK(filename, pair,log)
					if (result != True):
						print("fail")
						testFailed = True
						break
				#remove the failed tree
				
				log.close()
				
				if (testFailed == True or args.debug_flag == False):
					os.remove(filename)
					temptree.write(self.d1+'/'+filename_src)
					
				else:
					os.remove(logname)
				##delete tmp files unless debuggin.
				
				
					write.writerow(temp)
				#print (len(temp), '###à####')
		self.new_data.close()
		print ("end of thread"+str(self.threadID))
	      

	
	#paramsTree.write("inark_out.xml")

# run iNARK with the given parameters, return true if iNARK exits succesfully
def runINARK(inputFile, tasks,log):
	return subprocess.run([
		iNARKExe,
		"-i", inputFile,
		"--source", tasks[0],
		"--target", tasks[1],
		"--depth", "2",
		"--type", "1"
	], stdout = log, stderr = log).returncode == 0


def split_list(alist, wanted_parts=1):
	length = len(alist)
	return [ alist[i*length // wanted_parts: (i+1)*length // wanted_parts] 
		for i in range(wanted_parts) ]


if __name__ == "__main__":
	args = parser.parse_args()
	main()