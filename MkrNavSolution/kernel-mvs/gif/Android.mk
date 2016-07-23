LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(FLCNAV_GIF_LOC),)
  FLCNAV_GIF_LOC := ./gif_library
endif
ifeq ($(FLCNAV_GIF_ABS),)
  FLCNAV_GIF_ABS := $(LOCAL_PATH)/gif_library
endif

ifneq ($(FLCNAV_BUILDING_NEON_LIBRARY),true)
LOCAL_MODULE := gif
else
LOCAL_MODULE := gif_neon
endif

ifneq ($(FLCNAV_USE_PREBUILT),true)

LOCAL_SRC_FILES := \
	$(FLCNAV_GIF_LOC)/dgif_lib.c \
	$(FLCNAV_GIF_LOC)/gifalloc.c \
	$(FLCNAV_GIF_LOC)/gif_err.c
	
LOCAL_C_INCLUDES += \
	$(FLCNAV_GIF_ABS)

LOCAL_CFLAGS += -Wno-format -DHAVE_CONFIG_H

include $(BUILD_STATIC_LIBRARY)

else
LOCAL_SRC_FILES := \
	../lib/$(TARGET_ARCH_ABI)/lib$(LOCAL_MODULE).a
include $(PREBUILT_STATIC_LIBRARY)
endif
