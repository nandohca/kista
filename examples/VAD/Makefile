
# Ensure that kista-xml is in $PATH
#   by setting PATH=$(PATH):$(KISTA_PATH)/bin

EX_NAME = vad

# Library with the system functionality
LIB_FUN_NAME=fun

# Library with the environment tasks functions
LIB_ENV_NAME=env_tasks_fun

# Library with the system tasks functions
LIB_TASKS_FUN = tasks_func

export LIB_ENV_NAME
export LIB_FUN_NAME

# Enable the following variable definition for enable code annotation
# In case it is not enabled, KisTA will use directly the WCET
#DEFINES = -D_USE_AUTOMATIC_CODE_ANNOTATION

# For compiling all the source files of the folder
#INSTR_SRCS = $(wildcard ./fun/*.cpp)

# For compiling only the selected source files
INSTR_SRCS = basic_op.cpp oper_32b.cpp vad.cpp
INSTR_OBJS = $(INSTR_SRCS:.cpp=.o)

ENV_TASK_SRCS = VADsource_fun.cpp VADsink_fun.cpp VADFilesource_fun.cpp VADFilesink_fun.cpp EnvCallbacks_fun.cpp
ENV_TASK_OBJS = $(ENV_TASK_SRCS:.cpp=.o)

export ENV_TASK_SRCS
export ENV_TASK_OBJS

export INSTR_SRCS
export INSTR_OBJS
export EXDIR

TASK_SRCS += ThresholdAdaptation_fun.cpp
TASK_SRCS += SpectralComparison_fun.cpp
TASK_SRCS += PredictorValues_fun.cpp
TASK_SRCS += ACFAveraging_fun.cpp
TASK_SRCS += VADhangover_fun.cpp
TASK_SRCS += EnergyComputation_fun.cpp
TASK_SRCS += VADdecision_fun.cpp
TASK_SRCS += ToneDetection_fun.cpp
TASK_SRCS += GlobalInit_fun.cpp

TASK_OBJS = $(TASK_SRCS:.cpp=.o)

#SO_FILES = $(foreach DYN_LIB,$(DYN_LIBRARIES),$(wildcard $(DYN_LIB).so))

SD_FILE= $(EX_NAME).xml
SC_FILE= $(EX_NAME)_params.xml
SM_FILE= $(EX_NAME)_output.xml
SMD_FILE= $(EX_NAME)_smd.xml

SC_FILE_TEST= $(EX_NAME)_params_test.xml
SM_FILE_TEST= $(EX_NAME)_output_test.xml

SC_FILE_TEST2= $(EX_NAME)_params_test2.xml
SM_FILE_TEST2= $(EX_NAME)_output_test2.xml

CFLAGS = -fPIC  -Wall -Wno-deprecated -Wno-return-type -Wno-char-subscripts -pthread -g -O0 -std=c++0x  -DSC_INCLUDE_DYNAMIC_PROCESSES
# NOTICE: fPIC for position independent code, required for .so

## not needed for debianized package
SYSDIR = -I$(SYSTEMC)/include

#INCDIR = -I. -I.. -I$(SFF)
## not needed for debianized package
INCDIR = -I. -I.. $(SYSDIR) -I$(KISTA)/include
##INCDIR = -I. -I.. $(SYSDIR) -I$(KISTA)/src -I$(ADD_INCLUDE_DIRS)

#LIBDIR = -L. -L..
## not needed for debianized package
LIBDIR = -L. -L.. -L$(SYSTEMC)/lib-$(TARGET_ARCH)

#all: $(INSTR_OBJS) $(TASK_OBJS) $(DYN_LIBRARY).so

LIB_FUN_NAME=tasks_func
LIB_ENV_NAME=env_tasks_func

EXDIR = $(shell pwd)
FUN_DIR = ./fun
ENV_DIR = ./env_fun

all: lib$(LIB_TASKS_FUN).so lib$(LIB_ENV_NAME).so run sketch

libs:lib$(LIB_TASKS_FUN).so lib$(LIB_ENV_NAME).so
	@echo Environment tasks and System tasks function libraries generated in root directory:
	@ls -l $(LIB_TASKS_FUN).so lib$(LIB_ENV_NAME).so
	
lib$(LIB_TASKS_FUN).so: lib$(LIB_FUN_NAME).a $(TASK_OBJS)
	@echo Packaging system tasks functions into the dynamic library: $(LIB_TASKS_FUN).so
	@g++ -shared -L. -Wl,-soname,$(LIB_TASKS_FUN).so -o $(LIB_TASKS_FUN).so -Wl,-whole-archive -l$(LIB_FUN_NAME) -Wl,-no-whole-archive *.o
# NOTICE: The use of -Wl,-whole-archive and -Wl,-no-whole-archive to delimit the
#         static libraries which want to be appended to the .so library
#
# I tried the following rule, but by some reason it does not work
#	@echo Packaging system tasks functions into the dynamic library: $@ 
#@g++ -shared -L. -Wl,-soname,$@ -o $@ -Wl,-whole-archive -l$(LIB_FUN_NAME) -Wl,-no-whole-archive *.o
#
#
%.o: %.cpp
	@echo Compiling System task function $< in root directory
	@g++ -Wall $(DEFINES) $(CFLAGS) $(INCDIR) -c -o $@ $<

lib$(LIB_FUN_NAME).a:
	@echo Compiling instrumented functionality $(INSTR_OBJS) in $(FUN_DIR)
	@make -C $(FUN_DIR)

lib$(LIB_ENV_NAME).so:
	@echo Compiling environment task functions $(OBJS) in $(ENV_DIR)
	@make -C $(ENV_DIR)

run: lib$(LIB_TASKS_FUN).so lib$(LIB_ENV_NAME).so
	kista-xml $(SD_FILE)\
 --xml_system_configuration=./$(SC_FILE)\
 --xml_system_metrics=./$(SM_FILE)\
 --xml_system_metrics_definition=./$(SMD_FILE)

test: lib$(LIB_TASKS_FUN).so lib$(LIB_ENV_NAME).so
	kista-xml $(SD_FILE)\
 --xml_system_configuration=./$(SC_FILE_TEST)\
 --xml_system_metrics=./$(SM_FILE_TEST)\
 --xml_system_metrics_definition=./$(SMD_FILE)

test2: lib$(LIB_TASKS_FUN).so lib$(LIB_ENV_NAME).so
	kista-xml $(SD_FILE)\
 --xml_system_configuration=./$(SC_FILE_TEST2)\
 --xml_system_metrics=./$(SM_FILE_TEST2)\
 --xml_system_metrics_definition=./$(SMD_FILE)

sketch:
	pdflatex $(EX_NAME)_sketch.tex

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.aux
	rm -f *.log
	rm -f *.tex

ultraclean:clean
	make -C $(FUN_DIR) ultraclean
	make -C $(ENV_DIR) ultraclean
	rm -f *.so
	rm -f $(EX_NAME)_output.xml
	rm -f *.pdf
