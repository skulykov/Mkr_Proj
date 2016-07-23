FLCNAV_FREETYPE_LOC := ./freetype_library

FLCNAV_FREETYPE_ABS := $(LOCAL_PATH)/freetype_library

LOCAL_SRC_FILES:= \
	$(FLCNAV_FREETYPE_LOC)/src/base/ftbbox.c \
	$(FLCNAV_FREETYPE_LOC)/src/base/ftbitmap.c \
	$(FLCNAV_FREETYPE_LOC)/src/base/ftfstype.c \
	$(FLCNAV_FREETYPE_LOC)/src/base/ftglyph.c \
	$(FLCNAV_FREETYPE_LOC)/src/base/ftlcdfil.c \
	$(FLCNAV_FREETYPE_LOC)/src/base/ftstroke.c \
	$(FLCNAV_FREETYPE_LOC)/src/base/fttype1.c \
	$(FLCNAV_FREETYPE_LOC)/src/base/ftxf86.c \
	$(FLCNAV_FREETYPE_LOC)/src/base/ftbase.c \
	$(FLCNAV_FREETYPE_LOC)/src/base/ftsystem.c \
	$(FLCNAV_FREETYPE_LOC)/src/base/ftinit.c \
	$(FLCNAV_FREETYPE_LOC)/src/base/ftgasp.c \
	$(FLCNAV_FREETYPE_LOC)/src/raster/raster.c \
	$(FLCNAV_FREETYPE_LOC)/src/sfnt/sfnt.c \
	$(FLCNAV_FREETYPE_LOC)/src/smooth/smooth.c \
	$(FLCNAV_FREETYPE_LOC)/src/autofit/autofit.c \
	$(FLCNAV_FREETYPE_LOC)/src/truetype/truetype.c \
	$(FLCNAV_FREETYPE_LOC)/src/cff/cff.c \
	$(FLCNAV_FREETYPE_LOC)/src/psnames/psnames.c \
	$(FLCNAV_FREETYPE_LOC)/src/pshinter/pshinter.c

LOCAL_C_INCLUDES += \
	$(FLCNAV_FREETYPE_ABS)/builds \
	$(FLCNAV_FREETYPE_ABS)/include

LOCAL_CFLAGS += -W -Wall
LOCAL_CFLAGS += "-DDARWIN_NO_CARBON"
LOCAL_CFLAGS += "-DFT2_BUILD_LIBRARY"

# the following is for testing only, and should not be used in final builds
# of the product
#LOCAL_CFLAGS += "-DTT_CONFIG_OPTION_BYTECODE_INTERPRETER"

LOCAL_CFLAGS += -O2

