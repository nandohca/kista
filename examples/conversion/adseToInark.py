import csv
import xml.etree.ElementTree as ET
import itertools
import re
import copy

########### this script uses design space xml AND the csv file. 
def get_BE_tasks():
  
	ET.register_namespace('', "http://www.multicube.eu/")
	paramsTree = ET.parse('design_space.xml.in')
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
	paramsTree = ET.parse('design_space.xml.in')
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
	paramsTree = ET.parse('design_space.xml.in')
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
		
	print (tasks_sched_pairs)



	plan = []
	explore(tasks_sched_pairs, plan, [])
	print ('\n')
	print (plan)

	final_configurations_map = {}
	for index, elem in enumerate(plan):
		print (elem)
		final_configurations_map[index] = {}
		x = 0
		while x < len(elem):
			final_configurations_map[index][elem[x]] = elem[x+1]
			x = x+2
			
	print (final_configurations_map)


	datafilename = 'esempio.csv'
	f=open(datafilename,'r')

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
	
	new_data = []
	"""this loop is needed to call the inark with the desired configuration and check if feasible.
	then if it is ok, the csv has to be modified adding and editing some columns.
	the points to be modified are: adding BE mapping, N tasks, TASK_SCHED column (?) 
	note that 1 row has to be attempted with all the final configuration map so the final line num of the csv is inizial line num * final_configurations_map - invalid configurations"""
	print ('inizio test')
	stringa = '(0-0-1-0-0-0-1)'
	
	st = filter(None, re.split("[\-()]", stringa))
	
	for index, i in enumerate(st):
		if i == str(1):
			print (index+1)
	print ('fine test')


	paramsTree = ET.parse('inark_plat.xml')
	root = paramsTree.getroot()
	components = root.find('components')
	if element is None:
		print ("element not found")
		return
	      
	temp_sched = {}
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

	for row in data:
		for i in range (0, len(schedulers)):
			 temp_sched[i]=[]
		for task in tasks:
		  
			value = row[associazione[task+'_MAP']]
			#print (task, value)
			st = filter(None, re.split("[\-()]", value))
			for index, i in enumerate(st):
				if i == str(1):
					temp_sched[index].append(task)

		#print (temp_sched)
		
		
		for value in final_configurations_map:
			temptree = copy.deepcopy(paramsTree)
			root = temptree.getroot()
			edges = root.find('edges')
			be_temp_sched = copy.deepcopy(temp_sched)
			#t1:schedy
			
			#print(len(row))
			temp = row[0:splitting_column]
			for aaaa in final_configurations_map[value]:
				x = []
				for z in range (0, len (schedulers)):
					x.append(0)
				#print (aaaa,'value is: ', final_configurations_map[value][aaaa])
				be_temp_sched[int (final_configurations_map[value][aaaa])-1].append(aaaa)
			#print (row[0:splitting_column])
				x[int(final_configurations_map[value][aaaa])-1] = 1
				
				
				
				
				
				temp.append('({0})'.format('-'.join(str(y) for y in x))) #vanno messi uno per volta
				#print ('({0})'.format('-'.join(str(y) for y in x)))
			temp+=(row[splitting_column:])
			#	print (temp)
			### test da farsi qua ###
			print(be_temp_sched)
			print('*****')
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
			filename = row[splitting_column]+'_prova.xml'
			temptree.write(filename)
			
			
			
			
			
			new_data.append(temp)
			#print (len(temp), '###à####')
	    

	f2=open("esempio_out.csv", 'w')
	write = csv.writer(f2, delimiter=';',quoting=csv.QUOTE_ALL)
	write.writerow(new_header)
	for line in new_data:
		write.writerow(line)
	#paramsTree.write("inark_out.xml")




if __name__ == "__main__":
	main()