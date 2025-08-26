OBJ_DIR = obj

C_OBJS  = $(addprefix $(OBJ_DIR)/,$(C_SRC:.c=.c.o))
ASM_OBJS= $(addprefix $(OBJ_DIR)/,$(ASM_SRC:.s=.s.o))
OBJS    = $(C_OBJS) $(ASM_OBJS)


########################
### Compiler Parameters
########################

#--------------------------------------------------
# Include directories
#--------------------------------------------------
C_INCLUDE_DIRS   = -I$(CURDIR) $(foreach dir, $(PUBLIC_INC_PATH), -I$(dir))
ASM_INCLUDE_DIRS = -I$(CURDIR) $(foreach dir, $(PUBLIC_INC_PATH), -I$(dir))

#--------------------------------------------------
# compiler flags
#--------------------------------------------------
#C_FLAGS = --diag_error=39,63,47,223,14,3017,940,186 --bss_threshold=0 --littleend --cpu=ARM968E-S --apcs interwork --signed_chars --no_multifile --feedback=$(ROOT_PATH)\bin\sys\$(IMAGE_PREFIX).fb
#C_FLAGS = --diag_error=39,63,47,223,14,3017,940,186 --bss_threshold=0 --littleend --cpu=ARM968E-S --apcs interwork --signed_chars --no_multifile
#C_FLAGS = --diag_error=39,63,47,223,14,3017,940,186 --bss_threshold=0 --littleend --cpu=ARM968E-S --apcs interwork --signed_chars --no_multifile --inlinemax=0
C_FLAGS = --diag_error=39,63,47,223,14,3017,940,186 --bss_threshold=0 --littleend --cpu=ARM9E-S --apcs interwork --signed_chars --no_multifile --inlinemax=0

#--------------------------------------------------
# Enabled for making loader
#--------------------------------------------------
ifeq ($(MAKE_LOADER_ONLY), 1)
C_DEFINES += -DMAKE_LOADER_ONLY
endif

#--------------------------------------------------
# MAKE_DEBUG=1: optimization off
#            0: optimization on
#--------------------------------------------------
ifeq ($(MAKE_DEBUG), 1)
C_FLAGS += -g -O0
else
C_FLAGS += -Ospace -O2
endif

#--------------------------------------------------
# MAKE_TARGET = 0: make firmware for ASIC
#               1: make firmware for FPGA
#               2: make firmware for HW simulation
#--------------------------------------------------
ifeq ($(MAKE_TARGET), 1)
C_DEFINES += -DFPGA_MODE
endif

ifeq ($(MAKE_TARGET), 2)
C_DEFINES += -DRTL_SIM
endif

#--------------------------------------------------
# Place user specific compile options after this line
# Reserved for new compile option.
#   1. Duplicate this section
#   2. Add compile option after symbol '+=' for 'C_FLAGS' or 'C_DEFINES'
#   3. Uncomment new option(s).
#   4. Modify comment for new option(s) especially
#      its(their) default state be enabled or disabled
#  *5. Make sure new compile option state be correct (enabled or disable)
#      before checking-in your code.
#--------------------------------------------------
#C_FLAGS +=
#C_DEFINES +=


########################
### Assembler Parameters
########################
#ASM_FLAGS = --apcs interwork -g --cpu=ARM968E-S
ASM_FLAGS = --apcs interwork -g --cpu=ARM9E-S

########################
### Archive Parameters
########################
AR_FLAGS = --create

########################
### Linker Parameters
########################
LINK_FLAGS = --map --info totals --debug --userlibpath $(PUBLIC_LIB_PATH) --list_mapping_symbols --info libraries --strict

########################
### LINKER_LIBS
########################
LINKER_LIBS = $(PUBLIC_LIB_PATH)/link.d



########################
### Common Rules 1
########################

ifeq ($(TARGET),)

all :
ifneq ($(SUB_DIRS),)
	@for x in $(SUB_DIRS); do cd $$x; make $@; cd $(CURDIR); done
endif

endif


clean :
	@rm -rf $(OBJ_DIR)
ifneq ($(SUB_DIRS),)
	@for x in $(SUB_DIRS); do cd $$x; make $@; cd $(CURDIR); done
endif

########################
### Common Rules 2
########################

$(OBJ_DIR)/%.c.o : %.c
	$(CC) $(C_INCLUDE_DIRS) $(C_FLAGS) $(C_DEFINES) $(EXTRA_C_DEFINES) -c $< -o $@

$(OBJ_DIR)/%.s.o : %.s
	$(ASM) $(ASM_INCLUDE_DIRS) $(ASM_FLAGS) $< -o $@

########################
### Common Rules 3
########################

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

########################
### Include Extra Rules
########################

ifeq ($(TARGET), make_lib)
include $(ROOT_PATH)/make/rules_lib.mk
endif

ifeq ($(TARGET), make_image)
include $(ROOT_PATH)/make/rules_image.mk
endif
