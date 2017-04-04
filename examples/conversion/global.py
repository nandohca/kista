import os
import argparse



parser = argparse.ArgumentParser(description='global wrapper for the CONTREX chain')
parser.add_argument('-s','--xml_system_description',   help='XML file describing the design space on the system (platform+application)',  required=True, type=str, dest='sd_file_name')
parser.add_argument('-a','--adse_input_params', help='input parameters needed fot the launch of the ADSE tool', required=True, type=str, dest='adse_params')
parser.add_argument('-d', '--debug', help='save temp files during the process', dest='debug_flag', action='store_true')
parser.set_defaults(debug_flag=False)
parser.add_argument('-i', '--inark_platform_description', help='xml with the description of the platform in iNARK format', dest='inark_platform', required=True, type=str)
parser.add_argument('-m', '--MOST_search_params', help='MOST search informations', dest='most_params', required=True, type=str)
parser.add_argument('-whatever', '--inark_platform_description', help='xml with the description of the platform in iNARK format', dest='inark_platform', required=True, type=str)
## xsd è lo stesso che i search params? o meglio, xsd è il design space?
## smd -> metriche da misurare

##sys.xml è lo stesso di most? 



parser.set_defaults(thread_number=1)



def main():
	#call adse
	
	#adsetoinark
	
	#most
	
	#kista stub
	
		#integraz
	
		#run kista
	
	#il problema è che most controlla quello che viene dopo (se non ho capito male) quindi stub deve gestire gli altri 2 script.
	#dovrei recuperare i vari pezzi e controllare. 
	#problema 2: subdirectory temporanee in cui copiare e lanciare make.
	
	
if __name__ == "__main__":
	argparse
	main()