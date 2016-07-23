# Include tools definitions
include ../Makefile.vars

# Include project files
LOCAL_PATH = .
FLCNAV_ZLIB_LOC := ./zlib_library

FLCNAV_ZLIB_ABS := $(LOCAL_PATH)/zlib_library

LOCAL_SRC_FILES:= \
	$(FLCNAV_ZLIB_LOC)/adler32.c \
	$(FLCNAV_ZLIB_LOC)/compress.c \
	$(FLCNAV_ZLIB_LOC)/crc32.c \
	$(FLCNAV_ZLIB_LOC)/deflate.c \
	$(FLCNAV_ZLIB_LOC)/inffast.c \
	$(FLCNAV_ZLIB_LOC)/inflate.c \
	$(FLCNAV_ZLIB_LOC)/infback.c \
	$(FLCNAV_ZLIB_LOC)/inftrees.c \
	$(FLCNAV_ZLIB_LOC)/trees.c \
	$(FLCNAV_ZLIB_LOC)/uncompr.c \
	$(FLCNAV_ZLIB_LOC)/zutil.c

# Set library name
LIBNAME = $(LIBRARY_PREFIX)z
LIBTYPE = $(STATICLIB_EXT)

# Finally, include generic rules
ifdef ZLIB_BUILD
include ../Makefile.rules
endif