#
# Assumes that each application is found in a folder of the same name
# and that the environment is placed in a folder called /env
#

EX_NAME=vad_jpeg

SD_FILE= $(EX_NAME).xml
SC_FILE= $(EX_NAME)_params.xml
SM_FILE= $(EX_NAME)_output.xml
SMD_FILE= $(EX_NAME)_smd.xml

APP1_NAME=vad
APP2_NAME=jpeg
ENV_NAME=env

PWD_PATH=$(shell pwd)

APP1_DIR= $(PWD_PATH)/$(APP1_NAME)
APP2_DIR= $(PWD_PATH)/$(APP2_NAME)
ENV_DIR= $(PWD_PATH)/$(ENV_NAME)

all: $(APP1_DIR)/(APP1_NAME).so $(APP2_DIR)/(APP2_NAME).so $(ENV_DIR)/(ENV_NAME).so run

run: $(APP1_DIR)/(APP1_NAME).so $(APP1_DIR)/(APP1_NAME).so $(ENV_DIR)/(ENV_NAME).so
	kista-xml $(SD_FILE)\
 --xml_system_configuration=./$(SC_FILE)\
 --xml_system_metrics=./$(SM_FILE)\
 --xml_system_metrics_definition=./$(SMD_FILE)

libs:lib$(LIB_APP1_FUN).so lib$(LIB_APP2_FUN).so lib$(LIB_ENV_NAME).so
	@echo Environment tasks library and the library with the application tasks functionality of the system generated in root directory:
	@ls -l lib$(LIB_APP1_FUN).so lib$(LIB_APP2_FUN).so lib$(LIB_ENV_NAME).so

$(APP1_DIR)/(APP1_NAME).so:
	@echo -------------------------------------------------
	@echo - Compiling Application: $(APP1_NAME)
	@echo -------------------------------------------------
	@make -C $(APP1_DIR)

$(APP2_DIR)/(APP2_NAME).so:
	@echo -------------------------------------------------
	@echo - Compiling Application: $(APP2_NAME)
		@echo -------------------------------------------------
	@make -C $(APP2_DIR)

$(ENV_DIR)/(ENV_NAME).so:
	@echo -------------------------------------------------
	@echo - Compiling Environment
	@echo -------------------------------------------------
	@make -C $(ENV_DIR)

sketch:
	pdflatex $(EX_NAME)_sketch.tex

trace:
	pdflatex tikz_trace.tex

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.aux
	rm -f *.log
	rm -f *.tex
	make -C $(APP1_DIR) clean
	make -C $(APP2_DIR) clean
	make -C $(ENV_DIR) clean

ultraclean:clean
	make -C $(APP1_DIR) ultraclean
	make -C $(APP2_DIR) ultraclean
	make -C $(ENV_DIR) ultraclean
	rm -f *.so
	rm -f $(EX_NAME)_output.xml
	rm -f *.pdf
	rm -f *.vcd

