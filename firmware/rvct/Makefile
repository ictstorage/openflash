TARGET = make_image

LIB_NAME =

SUB_DIRS =

PUBLIC_INC_PATH +=

EXTRA_C_DEFINES =

IMAGE_LAYOUT_FILE = rvctScatter

IMAGE_ENTRY_POINT = 0

ASM_SRC =           \
    rvctHeap.s      \
    rvctStack.s     \
    rvctVectors.s   \
    rvctRstHdlr.s   \
    sysCPU_a.s

C_SRC = rvctISR.c   \
    rvctException.c \
    rvctRetarget.c  \
    rvctSubMain.c

include $(MAKE_RULES)
