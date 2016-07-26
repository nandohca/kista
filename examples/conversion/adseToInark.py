import csv
import xml.etree.ElementTree as ET
import itertools



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
					tmp = 'sched_{0}'.format(x+1)
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
	for row in data:
		#print (row[0:splitting_column])
		temp = row[0:splitting_column]
		temp.append( "dummy") #vanno messi uno per volta
		
		temp+=(row[splitting_column:])
	#	print (temp)
		new_data.append(temp)
	#	print (len(new_data))


	f2=open("esempio_out.csv", 'w')
	write = csv.writer(f2, delimiter=';')
	write.writerow(new_header)
	for line in new_data:
		write.writerow(line)





if __name__ == "__main__":
	main()