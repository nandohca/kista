#
# Simple Makefile for SystemC compilation
#

CC = g++ $(SCO)
#CFLAGS = $(COMPILATION_TYPE_FLAG) -O2
CFLAGS = $(COMPILATION_TYPE_FLAGS) -g -Wall

# This is related to the type of host installation platform
# For linux 32 bits
#LIB = lib-linux
# For linux 64 bits 
LIB =lib-linux64

DEFINES = $(COMPILATION_TYPE_FLAG)

ADD_INCDIR = -I$(SYSTEMC_PATH)/include
ADD_LIBDIR = -L$(SYSTEMC_PATH)/$(LIB)
ADD_LIBS = -lsystemc

#ADD_INCDIR = -I$(SYSTEMC_PATH)/include -I/home/nando/soft/ucsoft/hetsc_v1.3/include
#ADD_LIBDIR = -L$(SYSTEMC_PATH)/$(LIB) -I/home/nando/soft/ucsoft/hetsc_v1.3/lib
#ADD_LIBS = -lsystemc -lhetsc

# Target prefix: none = native compilation
TARGET_PREFIX = 

EXEDIR = sys

EXAMPLE = ed
SRCS = f1.cpp f2.cpp f3.cpp sc_f1.cpp sc_f2.cpp sc_f3.cpp testbench.cpp basic_scope.cpp
OBJS = $(SRCS:.cpp=.o)
EXE_SUFIX = .x
EXE = $(EXAMPLE).x

INCDIR = $(BASIC_INCDIR) $(ADD_INCDIR)
LIBDIR = $(BASIC_LIBDIR) $(ADD_LIBDIR)

LIBS = $(ADD_LIBS)

.SUFFIXES: .cc .cpp .o .x

./$(EXEDIR)/$(EXE):$(OBJS)
	mkdir -p ./$(EXEDIR)
	$(CC) $(CFLAGS) $(INCDIR) $(LIBDIR) -o $@ $(OBJS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCDIR) -c $<

.cc.o:
	$(CC) $(CFLAGS) $(INCDIR) -c $<

clean:
	rm -rf $(OBJS)

ultraclean: clean
	rm -rf $(EXEDIR)
	rm -rf *~
	rm -rf sys

run:./$(EXEDIR)/$(EXE)
	./$(EXEDIR)/$(EXE)
	
update:
	cp *.gif ../../figs_orig



