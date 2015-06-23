
LIBNAME = kista

# For svn in KTH svn server
# KISTA_REVISION = $(shell svnversion)

# For GitHub 
KISTA_REVISION = $(shell git rev-list HEAD | wc -l)

export KISTA_REVISION

# The following for a release...
#KISTA_VERSION=v1.0

# ... or the following for a repository version
KISTA_VERSION=$(KISTA_REVISION)

export KISTA_VERSION

STATIC_LIB_NAME = lib$(LIBNAME).a
DYN_LIB_NAME = lib$(LIBNAME).so

# This does not work since it is executed before any tule
#LIBOBJS = $(wildcard *.o)
#LIBOBJS=$(shell ls *.o)


ifeq (,$(SRCDIR))
	SRCDIR=$(shell pwd)
	export SRCDIR
endif

ifeq (,$(INSTALLDIR))
	INSTALLDIR=$(shell pwd)
	export INSTALLDIR
endif

# rather more a BUILDIR, this is an objdir where all the .o will be dumped to create the kista library
ifeq (,$(BUILDIR))
	BUILDIR=$(shell pwd)/build
	export BUILDIR
endif

EX=$(SRCDIR)/examples
BRT_EX=$(EX)/basic_real_time/brt_ex
SYN_EX=$(EX)/synch_comm/synch_ex
COMRES_EX=$(EX)/comm_res/comres_ex
SRC=$(SRCDIR)/src


INC_INSTALLDIR = $(INSTALLDIR)/include
LIB_INSTALLDIR = $(INSTALLDIR)/lib
TOOL_INSTALLDIR = $(INSTALLDIR)/bin

export INC_INSTALLDIR
export LIB_INSTALLDIR
export TOOL_INSTALLDIR

TARGET_SYS=ubuntu_12.04L_64b

DISTRO_DIR = $(INSTALLDIR)/kista_R$(KISTA_REVISION)_$(TARGET_SYS)

#if no INSTALLDIR variable is settled, the current path 

#if no INSTALLDIR variable is settled, the current path (root of sources)
# adopted as installation direcotry
# In the rule, notice that changing to a directory, make a command and return back
# is done in a single line (each rule is executed in a new shell)

all: kista install

kista:
	@echo Compiling KisTA in $(BUILDIR) and installing in $(INSTALLDIR)
	mkdir -p $(BUILDIR)
	make -C ./src
	cd $(BUILDIR);\
	ar rcs -o $(STATIC_LIB_NAME) *.o;\
	g++ -shared -Wl,-soname,$(DYN_LIB_NAME) -o $(DYN_LIB_NAME) *.o;\
	cd ..
	
install:
	mkdir -p $(LIB_INSTALLDIR)
	cp -rf $(BUILDIR)/*.a $(LIB_INSTALLDIR)
	cp -rf $(BUILDIR)/*.so $(LIB_INSTALLDIR)
	cp -rf $(SRCDIR)/src/system_level_bypass/libsyslevel_bypass.a $(LIB_INSTALLDIR)
	mkdir -p $(INC_INSTALLDIR)	
	cp -rf $(SRCDIR)/src/*.hpp $(INC_INSTALLDIR)
	mkdir -p $(INC_INSTALLDIR)/synch_comm
	cp -rf $(SRCDIR)/src/synch_comm/*.hpp $(INC_INSTALLDIR)/synch_comm
	mkdir -p $(INC_INSTALLDIR)/comm_res
	cp -rf $(SRCDIR)/src/comm_res/*.hpp $(INC_INSTALLDIR)/comm_res
	mkdir -p $(INC_INSTALLDIR)/comm_res/buses
	cp -rf $(SRCDIR)/src/comm_res/buses/*.hpp $(INC_INSTALLDIR)/comm_res/buses
	mkdir -p $(INC_INSTALLDIR)/system_level_bypass
	cp -rf $(SRCDIR)/src/system_level_bypass/*.hpp $(INC_INSTALLDIR)/system_level_bypass
	mkdir -p $(INC_INSTALLDIR)/annotation_plugin
	cp -rf $(SRCDIR)/src/annotation_plugin/*.hpp $(INC_INSTALLDIR)/annotation_plugin
	mkdir -p $(INC_INSTALLDIR)/load_analysis
	cp -rf $(SRCDIR)/src/load_analysis/*.hpp $(INC_INSTALLDIR)/load_analysis
	
clean:
	make -C ./src clean
	rm -rf $(BUILDIR)

ultraclean: clean
	make -C ./src ultraclean
	rm -rf $(LIB_INSTALLDIR)
	rm -rf $(INC_INSTALLDIR)
	rm -rf $(TOOL_INSTALLDIR)
	
tools:$(LIB_INSTALLDIR)
	make -C ./src/xml_if

tools_install:
	mkdir -p $(TOOL_INSTALLDIR)
	make -C ./src/xml_if install

tools_clean:
	make -C ./src/xml_if clean

tools_ultraclean:
	rm -rf $(TOOL_INSTALLDIR)
	make -C ./src/xml_if ultraclean

distro: ultraclean tools_ultraclean all install tools tools_install
	mkdir -p $(DISTRO_DIR)
	cp -r $(INC_INSTALLDIR) $(DISTRO_DIR)
	cp -r $(LIB_INSTALLDIR) $(DISTRO_DIR)
	cp -r $(TOOL_INSTALLDIR) $(DISTRO_DIR)	
	cp -r $(EX) $(DISTRO_DIR)
	rsync -av --exclude=".*" $(EX) $(DISTRO_DIR)
	tar czf $(DISTRO_DIR).tar.gz $(DISTRO_DIR)
#	cp -r $(TOOL_INSTALLDIR) $(DISTRO_DIR)
# for distro, cp tries to copy everything, including hidden files

src_lines:
	wc $(SRC)/*.cpp $(SRC)/*.hpp \
	   $(SRC)/synch_comm/*.hpp $(SRC)/synch_comm/*.cpp\
	   $(SRC)/comm_res/*.hpp $(SRC)/comm_res/*.cpp\
	   $(SRC)/comm_res/buses/*.hpp $(SRC)/comm_res/buses/*.cpp\
	   $(SRC)/system_level_bypass/*.hpp $(SRC)/system_level_bypass/*.cpp\
	   $(SRC)/annotation_plugin/*.hpp $(SRC)/annotation_plugin/*.cpp\
       $(SRC)/xml_if/*.cpp\
       $(SRC)/load_analysis/*.cpp       
       
#	   $(SRC)/comm_res/networks/*.hpp $(SRC)/comm_res/networks/*.hpp\       
#	   $(SRC)/xml_if/*.hpp

ex_lines:
	wc $(BRT_EX)1/*.cpp $(BRT_EX)2/*.cpp $(BRT_EX)3/*.cpp $(BRT_EX)4/*.cpp $(BRT_EX)5/*.cpp $(BRT_EX)6/*.cpp\
	 $(SYN_EX)1/*.cpp $(SYN_EX)2/*.cpp $(SYN_EX)3/*.cpp $(SYN_EX)4/*.cpp $(SYN_EX)5/*.cpp $(SYN_EX)6/*.cpp\
	 $(COMRES_EX)1/*.cpp\
	 $(EX)/fdl2013_ex/*.cpp $(EX)/fdl2013_ex/*.h\
	 $(EX)/VAD/*.cpp $(EX)/VAD/*.h $(EX)/VAD/*.hpp $(EX)/VAD/env_fun/*.cpp\
	 $(EX)/vad_jpeg/env/*.cpp $(EX)/vad_jpeg/vad/*.cpp $(EX)/vad_jpeg/vad/*.hpp $(EX)/vad_jpeg/jpeg/*.cpp
#	 $(EX)/VAD/*.cpp $(EX)/VAD/*.h $(EX)/VAD/*.hpp $(EX)/VAD/fun/*.h $(EX)/VAD/fun/*.cpp $(EX)/VAD/env_fun/*.cpp
