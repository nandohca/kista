
# Ensure that kista-xml is in $PATH
#   by setting PATH=$(PATH):$(KISTA_PATH)/bin

EX_NAME=sample6

SD_FILE= $(EX_NAME).xml
SC_FILE= $(EX_NAME)_params.xml
SM_FILE= $(EX_NAME)_output.xml
SMD_FILE= $(EX_NAME)_design_space_def.xml

run:
	kista-xml $(SD_FILE)\
 --xml_system_configuration=./$(SC_FILE)\
 --xml_system_metrics=./$(SM_FILE)\
 --xml_system_metrics_definition=./$(SMD_FILE)

sketch:
	pdflatex $(EX_NAME)_sketch.tex

all:run sketch

clean:
	rm -f $(EX_NAME)_sketch.tex
	rm -f $(EX_NAME)_sketch.aux
	rm -f $(EX_NAME)_sketch.log

ultraclean:clean
	rm -f $(EX_NAME)_output.xml
	rm -f $(EX_NAME)_sketch.pdf
		

