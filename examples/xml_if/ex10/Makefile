
# Ensure that kista-xml is in $PATH
#   by setting PATH=$(PATH):$(KISTA_PATH)/bin

EX_NAME=sample10

# Enable the following variable definition for enable code annotation
# In case it is not enabled, KisTA will use directly the WCET
DEFINES=-D_USE_AUTOMATIC_CODE_ANNOTATION

# For compiling all the source files of the folder
#INSTR_SRCS =$(wildcard ./fun/*.cpp)

# For compiling only the selected source files
INSTR_SRCS = gen_data.cpp process_data.cpp
INSTR_OBJS = $(INSTR_SRCS:.cpp=.o)

EXDIR = $(shell pwd)

export INSTR_SRCS
export INSTR_OBJS
export EXDIR

TASK_SRCS = t1_fun.cpp t2_fun.cpp t3_fun.cpp

TASK_OBJS = $(TASK_SRCS:.cpp=.o)

DYN_LIBRARY = task_funcs

#SO_FILES =  $(foreach DYN_LIB,$(DYN_LIBRARIES),$(wildcard $(DYN_LIB).so))

SD_FILE= $(EX_NAME).xml
SC_FILE= $(EX_NAME)_params.xml
SM_FILE= $(EX_NAME)_output.xml
SMD_FILE= $(EX_NAME)_design_space_def.xml

CFLAGS = -fPIC  -Wall -Wno-deprecated -Wno-return-type -Wno-char-subscripts -pthread -g -O0 -std=c++0x  -DSC_INCLUDE_DYNAMIC_PROCESSES 
# NOTICE: fPIC for position independent code, required for .so

## not needed for debianized package
SYSDIR = -I$(SYSTEMC)/include
          
#INCDIR = -I. -I.. -I$(SFF)
## not needed for debianized package
INCDIR = -I. -I.. $(SYSDIR) -I$(KISTA)/src
##INCDIR = -I. -I.. $(SYSDIR) -I$(KISTA)/src -I$(ADD_INCLUDE_DIRS)

#LIBDIR = -L. -L..
## not needed for debianized package
LIBDIR = -L. -L.. -L$(SYSTEMC)/lib-$(TARGET_ARCH)

#all: $(INSTR_OBJS) $(TASK_OBJS) $(DYN_LIBRARY).so

all: $(DYN_LIBRARY).so run $(EX_NAME)_sketch.pdf
	
$(DYN_LIBRARY).so: libfun.a $(TASK_OBJS)
	@echo Packaging everything into the dynamic library: $(DYN_LIBRARY).so
	g++ -shared -L. -Wl,-soname,$(DYN_LIBRARY).so -o $(DYN_LIBRARY).so -Wl,-whole-archive -lfun -Wl,-no-whole-archive *.o 

# NOTICE: The use of -Wl,-whole-archive and -Wl,-no-whole-archive to delimit the
#         static libraries which want to be appended to the .so library

%.o: %.cpp
	@echo Compiling task function $<
	g++ -Wall $(DEFINES) $(CFLAGS) $(INCDIR) -c -o $@ $<

libfun.a:
	@echo Compiling instrumented functionality $(INSTR_OBJS) into libfun.a
	make -C ./fun

run: $(DYN_LIBRARY).so
	kista-xml $(SD_FILE)\
 --xml_system_configuration=./$(SC_FILE)\
 --xml_system_metrics=./$(SM_FILE)\
 --xml_system_metrics_definition=./$(SMD_FILE)

$(EX_NAME)_sketch.pdf:$(EX_NAME)_sketch.tex
	pdflatex $(EX_NAME)_sketch.tex

clean:
	rm -f *.o
	rm -f *.a
	
ultraclean:clean
	make -C ./fun ultraclean
	rm -f *.so
	rm -f $(EX_NAME)_output.xml
	rm -f $(EX_NAME)_sketch.tex
	rm -f $(EX_NAME)_sketch.pdf
	rm -f $(EX_NAME)_sketch.aux
	rm -f $(EX_NAME)_sketch.log
		

