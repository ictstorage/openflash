#######################
###
#######################
IMAGE_AXF = $(PUBLIC_BIN_PATH)/$(IMAGE_PREFIX).axf
IMAGE_BIN = $(PUBLIC_BIN_PATH)/$(IMAGE_PREFIX).bin
IMAGE_VHX = $(PUBLIC_BIN_PATH)/$(IMAGE_PREFIX).vhx
IMAGE_LST = $(PUBLIC_BIN_PATH)/$(IMAGE_PREFIX).list
IMAGE_SYM = $(PUBLIC_BIN_PATH)/$(IMAGE_PREFIX).symbol

ifeq ($(MAKECMDGOALS),all)
	LIBS = $(shell cat $(LINKER_LIBS))
endif


#######################
### all
#######################
all: $(IMAGE_BIN)

$(IMAGE_BIN) : $(IMAGE_AXF)
	fromelf --bin --output $@ $<
	fromelf --vhx --output $(IMAGE_VHX) $<
#	fromelf --i32 --base 0x1000 --output bin/SYS_S.i32 $<
#	fromelf --m32 --output bin/SYS_S.m32 $<

#	$(PUBLIC_BIN_PATH)/SUM.exe $(IMAGE_BIN)
	@echo
#	@$(PUBLIC_BIN_PATH)/compose_bin.sh $(IMAGE_BIN) $(PUBLIC_BIN_PATH)/$(IMAGE_PREFIX)2.bin $(PUBLIC_BIN_PATH)/$(IMAGE_PREFIX).bin

$(IMAGE_AXF) : $(OBJ_DIR) $(OBJS) $(LIBS)
	@mkdir -p $(PUBLIC_BIN_PATH)
	$(LINK) $(LINK_FLAGS) --entry $(IMAGE_ENTRY_POINT) --scatter $(IMAGE_LAYOUT_FILE) --list $(IMAGE_LST) --symdefs $(IMAGE_SYM) --output $@ $(LIBS) $(OBJS)
	@echo

$(LIBS) :
	@echo examine $(LIBS)


#######################
###
#######################

ifeq ($(MAKECMDGOALS),all)
ifneq ($(wildcard $(OBJ_DIR)/depend.mk),)
	include $(OBJ_DIR)/depend.mk
endif
endif
