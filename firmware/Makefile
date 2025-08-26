#################################################################
### MAKE_LOADER_ONLY = 1 : only loader is make and all the
###                        programs are "optimized" for loader
### MAKE_LOADER_ONLY = 0 : except loader, all the programa are
###                        created for non-loader applications
#################################################################
MAKE_LOADER_ONLY = 0


#################################################################
### MAKE_DEBUG = 1 : make debug version program (with least
###                  optimization)
### MAKE_DEBUG = 0 : make release version program with most
###                  optimization)
#################################################################
MAKE_DEBUG = 1


#################################################################
### MAKE_TARGET = 0 : make the program for release (default)
### MAKE_TARGET = 1 : make the program for FPGA debug
### MAKE_TARGET = 2 : make the program for RTL code simulation
#################################################################
MAKE_TARGET = 0


#################################################################
### DEFAULT_IMAGE_PREFIX
#################################################################
DEFAULT_IMAGE_PREFIX = SYS_S


#################################################################
###          !!!  don't modify anything below  !!!
#################################################################


#################################################################
### ROOT_PATH
#################################################################
ROOT_PATH = $(CURDIR)


#################################################################
### PUBLIC_SYS_PATH
#################################################################
PUBLIC_SYS_PATH = $(ROOT_PATH)/sys


#################################################################
### PUBLIC_INC_PATH
#################################################################
PUBLIC_INC_PATH = $(ROOT_PATH)/inc


#################################################################
### PUBLIC_LIB_PATH
#################################################################
PUBLIC_LIB_PATH = $(ROOT_PATH)/lib


#################################################################
### PUBLIC_BIN_PATH
#################################################################
PUBLIC_BIN_PATH = $(ROOT_PATH)/bin


#################################################################
### MAKE_RULES
#################################################################
MAKE_RULES = $(ROOT_PATH)/make/rules.mk


#################################################################
### compiler commands and options
#################################################################
CC = armcc
ASM = armasm
AR = armar
LINK = armlink

SUB_DIRS = drv sys rvct
CONFIGURE = $(DEFAULT_IMAGE_PREFIX)


#################################################################
### export
#################################################################
export MAKE_LOADER_ONLY
export MAKE_DEBUG
export MAKE_TARGET
export ROOT_PATH
export PUBLIC_SYS_PATH
export PUBLIC_INC_PATH
export PUBLIC_LIB_PATH
export PUBLIC_BIN_PATH
export MAKE_RULES
export CC ASM AR LINK


#################################################################
### make
#################################################################
all   : clean
	@echo Compile codes ...
	@for x in $(SUB_DIRS); do cd $$x; make -s $@ IMAGE_PREFIX=$(CONFIGURE); cd "$(ROOT_PATH)"; done
	@echo done.

clean :
	#@- > include/autocfg.h
	@rm -rf $(PUBLIC_LIB_PATH) $(PUBLIC_BIN_PATH)
	@for x in $(SUB_DIRS); do cd $$x; make $@; cd $(ROOT_PATH); done

debug : clean
	@echo Compile codes ...
	@for x in $(SUB_DIRS); do cd $$x; make all IMAGE_PREFIX=$(CONFIGURE) MAKE_DEBUG=1; cd $(ROOT_PATH); done
	@echo done.

fpga  : clean
	@echo Compile codes ...
	@for x in $(SUB_DIRS); do cd $$x; make all IMAGE_PREFIX=$(CONFIGURE) MAKE_TARGET=1; cd $(ROOT_PATH); done
	@echo done.

sim   : clean
	@echo Compile codes ...
	@for x in $(SUB_DIRS); do cd $$x; make all IMAGE_PREFIX=$(CONFIGURE) MAKE_TARGET=2; cd $(ROOT_PATH); done
	@echo done.

help  :
	@echo
	@echo "make clean       - Remove build objects and dependency files"
	@echo "make all         - Make objects and applications"
	@echo "make debug       - Make debug version objects and applications"
	@echo "make fpga        - Make FPGA version objects and applications"
	@echo "make sim         - Make HW simulation version objects and applications"

% :
	@echo
	@echo "Not support \"$@\" target."
	@echo "Please type \"make help\" for help information."
