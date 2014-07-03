#
# Makefile for annotated pure functionality
#

# For compiling all the source files of the folder
#INSTR_SRCS =$(wildcard ./fun/*.cpp)

# For compiling only the selected source files
#INSTR_SRCS = gen_data.cpp process_data.cpp
#INSTR_OBJS = $(INSTR_SRCS:.cpp=.o)

SCOPE_CC = scope-g++ --scope-method=op-cost --scope-noicache
ANNOTATION_FLAGS=-D_USE_AUTOMATIC_CODE_ANNOTATION -fPIC

CC = g++ $(SCO)

OPT    = -O2
#CFLAGS = $(COMPILATION_TYPE_FLAG) $(OPT)

# -srd=C++0x os for using 2011 standard. For instance, to use to_string functions
CFLAGS = $(COMPILATION_TYPE_FLAGS) -g -std=c++0x -Wall

# This is related to the type of host installation platform
# For linux 32 bits
#LIB = lib-linux
# For linux 64 bits 
LIB =lib-linux64

DEFINES = $(COMPILATION_TYPE_FLAG)

ADD_INCDIR = -I$(SYSTEMC_PATH)/include -I$(KISTA)/src
ADD_LIBDIR = -L$(SYSTEMC_PATH)/$(LIB) -L$(KISTA)/lib
ADD_LIBS = -lsystemc -lkista

#ADD_INCDIR = -I$(SYSTEMC_PATH)/include -I/home/nando/soft/ucsoft/hetsc_v1.3/include
#ADD_LIBDIR = -L$(SYSTEMC_PATH)/$(LIB) -I/home/nando/soft/ucsoft/hetsc_v1.3/lib
#ADD_LIBS = -lsystemc -lhetsc

# Target prefix: none = native compilation
TARGET_PREFIX = 


INCDIR = $(BASIC_INCDIR) $(ADD_INCDIR)
LIBDIR = $(BASIC_LIBDIR) $(ADD_LIBDIR)

LIBS = $(ADD_LIBS)

	
#.SUFFIXES: .cc .cpp .o .x

all:$(INSTR_OBJS)
	g++ -shared -Wl,-soname,libfun.so -o libfun.so *.o
	ar rcs -o libfun.a *.o
	cp -f libfun.a $(EXDIR)
	
%.o: %.cpp
	$(SCOPE_CC) $(ANNOTATION_FLAGS) $(CFLAGS) $(INCDIR) -c $<

clean:
	rm -rf *.o
	
ultraclean: clean
	rm -f libfun.a
	rm -f libfun.so
	rm -rf *~
	

#CFLAGS += 
CFLAGS +=  -DSC_INCLUDE_DYNAMIC_PROCESSES
