
# Ensure that kista-xml is in $PATH
#   by setting PATH=$(PATH):$(KISTA_PATH)/bin

EX_NAME=sample8

TASK_SRCS = t1_fun.cpp t2_fun.cpp t3_fun.cpp

TASK_OBJS = $(TASK_SRCS:.cpp=.o)

DYN_LIBRARY = task_funcs

#SO_FILES =  $(foreach DYN_LIB,$(DYN_LIBRARIES),$(wildcard $(DYN_LIB).so))

SD_FILE= $(EX_NAME).xml
SC_FILE= $(EX_NAME)_params.xml
SM_FILE= $(EX_NAME)_output.xml
SMD_FILE= $(EX_NAME)_design_space_def.xml

CFLAGS = -Wall -Wno-deprecated -Wno-return-type -Wno-char-subscripts -pthread -g -O0 -std=c++0x  -DSC_INCLUDE_DYNAMIC_PROCESSES 

## not needed for debianized package
SYSDIR = -I$(SYSTEMC)/include
          
#INCDIR = -I. -I.. -I$(SFF)
## not needed for debianized package
INCDIR = -I. -I.. $(SYSDIR) -I$(KISTA)/src
##INCDIR = -I. -I.. $(SYSDIR) -I$(KISTA)/src -I$(ADD_INCLUDE_DIRS)

#LIBDIR = -L. -L..
## not needed for debianized package
LIBDIR = -L. -L.. -L$(SYSTEMC)/lib-$(TARGET_ARCH)
	
%.o: %.cpp
	g++ -Wall -fPIC $(CFLAGS) $(INCDIR) -c -o $@ $<
	
$(DYN_LIBRARY).so: $(TASK_OBJS)
	g++ -shared -Wl,-soname,$(DYN_LIBRARY).so -o $(DYN_LIBRARY).so *.o

run: $(DYN_LIBRARY).so
	kista-xml $(SD_FILE)\
 --xml_system_configuration=./$(SC_FILE)\
 --xml_system_metrics=./$(SM_FILE)\
 --xml_system_metrics_definition=./$(SMD_FILE)

sketch:
	pdflatex $(EX_NAME)_sketch.tex

all:run sketch

clean:
	rm -f *.o
	rm -f *.so	
	rm -f $(EX_NAME)_sketch.tex
	rm -f $(EX_NAME)_sketch.aux
	rm -f $(EX_NAME)_sketch.log

ultraclean:clean
	rm -f $(EX_NAME)_output.xml
	rm -f $(EX_NAME)_sketch.pdf
		

