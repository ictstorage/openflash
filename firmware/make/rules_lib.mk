LIB_FILE_NAME = $(LIB_NAME).a

PUBLIC_LIB = $(PUBLIC_LIB_PATH)/$(LIB_FILE_NAME)

#######################
### 
#######################
all : $(PUBLIC_LIB)
ifneq ($(SUB_DIRS),)
	@for x in $(SUB_DIRS); do cd $$x; make $@; cd $(CURDIR); done
endif

$(PUBLIC_LIB) : $(OBJ_DIR) $(OBJS)
	@mkdir -p $(PUBLIC_LIB_PATH)
	@if [ -f $(LINKER_LIBS) ]; then sed -i -e "/$(LIB_FILE_NAME)/d" $(LINKER_LIBS); fi
	$(AR) $(AR_FLAGS) $@ $(OBJS)
	@echo $@ >> $(LINKER_LIBS)
