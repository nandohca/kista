
EXTRACFLAGS = 
EXTRA_LIBS =

SRCS = $(wildcard *.cpp)

OBJS = $(SRCS:.cpp=.o)

ADD_INCLUDE_DIRS= 

include ../Makefile.defs


.PHONY: clean ultraclean

all: $(OBJS)
	cp -f *.o $(BUILDIR)

%.o : %.cpp
	$(CC) $(CFLAGS) $(INCDIR) -c -o $@ $<

install:


clean:
	rm -f $(OBJS)

ultraclean: clean
	
