
# Ensure that kista-xml is in $PATH
#   by setting PATH=$(PATH):$(KISTA_PATH)/bin

# LIB_TASKS_NAME : name of the library containing both functional code and tasks code
#                  (add lib prefix for static version)

# LIB_FUN_NAME : name of the library containing only functional code
#                  (add lib prefix for static version)
# it is used and linked by LIB_TASKS_NAME library

LIB_TASKS_NAME=vad

# Enable the following variable definition for enable code annotation
# In case it is not enabled, KisTA will use directly the WCET
#DEFINES = -D_USE_AUTOMATIC_CODE_ANNOTATION

# For compiling all the source files of the folder
#INSTR_SRCS = $(wildcard ./fun/*.cpp)

# For compiling only the selected source files
INSTR_SRCS = basic_op.cpp oper_32b.cpp vad.cpp
INSTR_OBJS = $(INSTR_SRCS:.cpp=.o)

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
TASK_SRCS += VADInit_fun.cpp

TASK_OBJS = $(TASK_SRCS:.cpp=.o)

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

LIB_FUN_NAME=$(LIB_TASKS_NAME)_func

APPDIR = $(shell pwd)
FUN_DIR = $(APPDIR)/fun


all: $(LIB_TASKS_NAME).so

#$(LIB_TASKS_NAME).so: lib$(LIB_FUN_NAME).a $(TASK_OBJS)
#	@echo Packaging application system tasks functions into the dynamic library: $(LIB_TASKS_NAME).so
#	@g++ -shared -L. -Wl,-soname,$(LIB_TASKS_NAME).so -o $(LIB_TASKS_NAME).so -Wl,-whole-archive -l$(LIB_FUN_NAME) -Wl,-no-whole-archive *.o

$(LIB_TASKS_NAME).so: $(TASK_OBJS)
	@if [ -d $(FUN_DIR) ]; then \
		echo Packaging application system tasks functions into the dynamic library: $(LIB_TASKS_NAME).so;\
		g++ -shared -L. -Wl,-soname,$(LIB_TASKS_NAME).so -o $(LIB_TASKS_NAME).so -Wl,-whole-archive -l$(LIB_FUN_NAME) -Wl,-no-whole-archive *.o;\
	else \
		echo Packaging application system tasks functions into the dynamic library: $(LIB_TASKS_NAME).so;\
		g++ -shared -L. -Wl,-soname,$(LIB_TASKS_NAME).so -o $(LIB_TASKS_NAME).so -Wl,-whole-archive -Wl,-no-whole-archive *.o;\
	fi	
	
# NOTICE: The use of -Wl,-whole-archive and -Wl,-no-whole-archive to delimit the
#         static libraries which want to be appended to the .so library
#
# I tried the following rule, but by some reason it does not work
#	@echo Packaging system tasks functions into the dynamic library: $@ 
#@g++ -shared -L. -Wl,-soname,$@ -o $@ -Wl,-whole-archive -l$(LIB_FUN_NAME) -Wl,-no-whole-archive *.o
#
#
%.o: %.cpp
	@echo - Compiling task function $< in $(APPDIR)
	@g++ -Wall $(DEFINES) $(CFLAGS) $(INCDIR) -c -o $@ $<

lib$(LIB_FUN_NAME).a:
	@echo - Compiling instrumented functionality $(INSTR_OBJS) in $(FUN_DIR)
	@make -C $(FUN_DIR) LIB_FUN_NAME=$(LIB_FUN_NAME) CPDIR=$(APPDIR)

clean:
	@rm -f *.o
	@if [ -d $(FUN_DIR) ]; then\
		make -C $(FUN_DIR) clean;\
	fi;		
	
ultraclean:clean
	@rm -f lib$(LIB_TASKS_NAME).a
	@rm -f lib$(LIB_FUN_NAME).a
	@rm -f $(LIB_TASKS_NAME).so
	@if [ -d $(FUN_DIR) ]; then\
		make -C $(FUN_DIR) ultraclean LIB_FUN_NAME=$(LIB_FUN_NAME);\
	fi;

