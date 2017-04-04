import os


def main():
	new_data = []
	f2=open("temp_data_0.csv", 'w')
	new_data.append(f2)
	f2=open("temp_data_1.csv", 'w')
	new_data.append(f2)
	f2.write("prova")
	f2.close()
	#call threads
	f2=open("esempio_out.csv", 'w')
	for partial_csv in new_data:
		f = open(partial_csv.name, 'r')
		print (partial_csv.name)
		for line in f:
			f2.write(line)
	
	
if __name__ == "__main__":
	main()