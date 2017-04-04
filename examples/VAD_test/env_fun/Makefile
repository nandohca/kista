#
# Makefile for annotated pure functionality
#

# For compiling all the source files of the folder
#INSTR_SRCS = $(wildcard ./fun/*.cpp)

# For compiling only the selected source files
#INSTR_SRCS = gen_data.cpp process_data.cpp
#INSTR_OBJS = $(INSTR_SRCS:.cpp=.o)

#exported from root Makefile
#LIB_ENV_NAME=env_task_funs

CC = g++ $(SCO)

OPT = -O2
#CFLAGS = $(COMPILATION_TYPE_FLAG) $(OPT)

# -srd=C++0x os for using 2011 standard. For instance, to use to_string functions
CFLAGS = -fPIC $(COMPILATION_TYPE_FLAGS) -g -O0 -std=c++0x -Wall

# This is related to the type of host installation platform
# For linux 32 bits
#LIB = lib-linux
# For linux 64 bits 
LIB = lib-linux64

DEFINES = $(COMPILATION_TYPE_FLAG)

ADD_INCDIR = -I$(SYSTEMC_PATH)/include -I$(KISTA)/include
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

all: $(ENV_TASK_OBJS) $(ENV_TASK_SRCS)
	@echo Generating Environment Task Function Library lib$(LIB_ENV_NAME).so
	@g++ -shared -Wl,-soname,lib$(LIB_ENV_NAME).so -o lib$(LIB_ENV_NAME).so *.o
	@ar rcs -o lib$(LIB_ENV_NAME).a *.o
	@cp -f lib$(LIB_ENV_NAME).so $(EXDIR)
#	cp -f lib$(LIB_ENV_NAME).a $(EXDIR)	

%.o: %.cpp
	@echo Compiling Environment Task Function: $<
	@$(CC) $(CFLAGS) $(INCDIR) -c $<

clean:
	rm -rf *.o

ultraclean: clean
	rm -f lib$(LIB_ENV_NAME).a
	rm -f lib$(LIB_ENV_NAME).so
	rm -rf *~

CFLAGS +=  -DSC_INCLUDE_DYNAMIC_PROCESSES
