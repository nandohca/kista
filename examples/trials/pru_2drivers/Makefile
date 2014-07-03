EXTRACFLAGS = 
EXTRA_LIBS =

MODULE = run
SRCS = $(wildcard *.cpp)

OBJS = $(SRCS:.cpp=.o)

EXAMPLE_DIR=$(CURDIR)

ADD_INCLUDE_DIRS= -I$(EXAMPLE_DIR)/vocoder -I$(EXAMPLE_DIR)/full_duplex_scenario

include ../../Makefile.defs

#CFLAGS += 
CFLAGS +=  -DSC_INCLUDE_DYNAMIC_PROCESSES
