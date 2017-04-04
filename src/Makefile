
EXTRACFLAGS =
EXTRA_LIBS =

SRCS = $(wildcard *.cpp)

OBJS = $(SRCS:.cpp=.o)

ADD_INCLUDE_DIRS= 

include ./Makefile.defs

.PHONY: clean ultraclean

all: $(OBJS)
	make -C ./synch_comm
	make -C ./comm_res
	make -C ./system_level_bypass
	make -C ./annotation_plugin
	make -C ./load_analysis
	make -C ./energy_power
	make -C ./xml_if
	cp -f *.o $(BUILDIR)
	
%.o : %.cpp
	$(CC) $(CFLAGS) $(INCDIR) -c -o $@ $<

install:


clean:
	rm -f $(OBJS)
	make -C ./synch_comm clean
	make -C ./comm_res clean
	make -C ./system_level_bypass clean
	make -C ./annotation_plugin clean
	make -C ./load_analysis clean
	make -C ./energy_power clean
	
ultraclean: clean
	make -C ./synch_comm ultraclean
	make -C ./comm_res ultraclean
	make -C ./system_level_bypass ultraclean
	make -C ./annotation_plugin ultraclean
	make -C ./load_analysis ultraclean
	make -C ./energy_power ultraclean
