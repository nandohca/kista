#!/usr/bin/python3

import os, subprocess, sys
import xml.etree.ElementTree as ET

def main():
	if len(sys.argv) != 2:
		print('Usage: launcher.py <params file>')
		sys.exit()
	ET.register_namespace('', "http://www.multicube.eu/")
	paramsTree = ET.parse(sys.argv[1])
	root = paramsTree.getroot()
	NS = '{http://www.multicube.eu/}'
	
	print (root)
	print (root.tag)
	print (root.attrib)
	schedulers = {}
	with open('Makefile.original', 'r') as original: data = original.read()
	with open('Makefile', 'w') as modified: 
		modified.write ("DEFINES =\n")
		num_map_done = 0
		for child in root:
			print (child.tag, child.attrib)
		matrice = "DEFINES += -D_{0}_{1}={2:.0f}\n"
		print ('*****************************')
		for elem in root.findall(NS + 'parameter'):
			if('STATIC_SCHED' in elem.attrib.get('name')):
				var = (elem.attrib.get('name').split('STATIC_SCHED')[1])
				
	#			elem.get('name') == 'STATIC_SCHED1':
				for child in elem:
					print (child.tag, child.attrib.get('value'))
					if var in schedulers:
						print('trovata')
						print(schedulers[var])
						ET.SubElement(root,"parameter", name='T'+str(num_map_done)+'_M', value=schedulers[var])
						num_map_done+=1
						ET.SubElement(root,"parameter", name='T'+str(num_map_done)+'_M', value=child.attrib.get('value'))
						num_map_done+=1
						
						modified.write(matrice.format(schedulers[var],'OUT',num_map_done/2))
						modified.write(matrice.format(child.attrib.get('value'),'IN',num_map_done/2))
						#modified.write("DEFINES += -D_"+schedulers[var]+"_OUT = "+str(num_map_done/2)+"\n")
						#modified.write("DEFINES += -D_"+child.attrib.get('value')+"_IN = "+str(num_map_done/2)+"\n")
						schedulers[var] = child.attrib.get('value')
					else:
						print('not found')
						ET.SubElement(root,"parameter", name='T'+str(num_map_done)+'_M', value='D'+var)
						num_map_done+=1
						ET.SubElement(root,"parameter", name='T'+str(num_map_done)+'_M', value=child.attrib.get('value'))
						num_map_done+=1
						schedulers[var] = child.attrib.get('value')
						modified.write("DEFINES += -D_D{0}_OUT={1:.0f}\n".format(var,num_map_done/2))
						modified.write(matrice.format(child.attrib.get('value'),'IN',num_map_done/2))
						
						#modified.write("DEFINES += -D_"+child.attrib.get('value')+"_IN = "+str(num_map_done/2)+"\n")

				root.remove(elem)
			else:
				print ('ciao')
		paramsTree.write('BE_tasks_params.xml',encoding="UTF-8", xml_declaration=True)
		modified.write( data)
	
	
if __name__ == "__main__":
	main()