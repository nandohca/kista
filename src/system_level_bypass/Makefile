
EXTRACFLAGS = 
EXTRA_LIBS =

SRCS = $(wildcard *.cpp)

OBJS = $(SRCS:.cpp=.o)

ADD_INCLUDE_DIRS= 

include ../Makefile.defs

.PHONY: clean ultraclean

all: $(OBJS)
	ar rcs -o libsyslevel_bypass.a *.o


%.o : %.cpp
	$(CC) $(CFLAGS) $(INCDIR) -c -o $@ $<

install:
	cp -f libsyslevel_bypass.a $(LIB_INSTALL_DIR)

clean:
	rm -f $(OBJS)

ultraclean: clean
	rm -f libsyslevel_bypass.a
