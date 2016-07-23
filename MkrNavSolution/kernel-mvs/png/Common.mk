FLCNAV_PNG_LOC := ./png_library

FLCNAV_PNG_ABS := $(LOCAL_PATH)/png_library
LOCAL_SRC_FILES:= \
	$(FLCNAV_PNG_LOC)/png.c \
	$(FLCNAV_PNG_LOC)/pngerror.c \
	$(FLCNAV_PNG_LOC)/pnggccrd.c \
	$(FLCNAV_PNG_LOC)/pngget.c \
	$(FLCNAV_PNG_LOC)/pngmem.c \
	$(FLCNAV_PNG_LOC)/pngpread.c \
	$(FLCNAV_PNG_LOC)/pngread.c \
	$(FLCNAV_PNG_LOC)/pngrio.c \
	$(FLCNAV_PNG_LOC)/pngrtran.c \
	$(FLCNAV_PNG_LOC)/pngrutil.c \
	$(FLCNAV_PNG_LOC)/pngset.c \
	$(FLCNAV_PNG_LOC)/pngtrans.c \
	$(FLCNAV_PNG_LOC)/pngvcrd.c \
	$(FLCNAV_PNG_LOC)/pngwio.c \
	$(FLCNAV_PNG_LOC)/pngwrite.c \
	$(FLCNAV_PNG_LOC)/pngwtran.c \
	$(FLCNAV_PNG_LOC)/pngwutil.c

LOCAL_C_INCLUDES += 


