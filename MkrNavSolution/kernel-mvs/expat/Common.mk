FLCNAV_EXPAT_LOC := ./expat_library
FLCNAV_EXPAT_ABS := $(LOCAL_PATH)/expat_library

# Include paths
LOCAL_C_INCLUDES += \
	$(FLCNAV_EXPAT_ABS) \
	$(FLCNAV_EXPAT_ABS)/lib
	
LOCAL_CFLAGS += -Wall -Wmissing-prototypes -Wstrict-prototypes -fexceptions -DHAVE_EXPAT_CONFIG_H

LOCAL_SRC_FILES := \
	$(FLCNAV_EXPAT_LOC)/lib/xmlparse.c \
	$(FLCNAV_EXPAT_LOC)/lib/xmlrole.c \
	$(FLCNAV_EXPAT_LOC)/lib/xmltok.c	
