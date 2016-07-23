LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

ifeq ($(FLCNAV_JPEG_LOC),)
  FLCNAV_JPEG_LOC := ./jpeg_library
endif
ifeq ($(FLCNAV_JPEG_ABS),)
  FLCNAV_JPEG_ABS := $(LOCAL_PATH)/jpeg_library
endif

ifneq ($(FLCNAV_BUILDING_NEON_LIBRARY),true)
LOCAL_MODULE := jpeg
else
LOCAL_MODULE := jpeg_neon
endif

ifneq ($(FLCNAV_USE_PREBUILT),true)

LOCAL_SRC_FILES := \
	$(FLCNAV_JPEG_LOC)/jcapimin.c \
	$(FLCNAV_JPEG_LOC)/jcapistd.c \
	$(FLCNAV_JPEG_LOC)/jccoefct.c \
	$(FLCNAV_JPEG_LOC)/jccolor.c \
	$(FLCNAV_JPEG_LOC)/jcdctmgr.c \
	$(FLCNAV_JPEG_LOC)/jchuff.c \
	$(FLCNAV_JPEG_LOC)/jcinit.c \
	$(FLCNAV_JPEG_LOC)/jcmainct.c \
	$(FLCNAV_JPEG_LOC)/jcmarker.c \
	$(FLCNAV_JPEG_LOC)/jcmaster.c \
	$(FLCNAV_JPEG_LOC)/jcomapi.c \
	$(FLCNAV_JPEG_LOC)/jcparam.c \
	$(FLCNAV_JPEG_LOC)/jcphuff.c \
	$(FLCNAV_JPEG_LOC)/jcprepct.c \
	$(FLCNAV_JPEG_LOC)/jcsample.c \
	$(FLCNAV_JPEG_LOC)/jctrans.c \
	$(FLCNAV_JPEG_LOC)/jdapimin.c \
	$(FLCNAV_JPEG_LOC)/jdapistd.c \
	$(FLCNAV_JPEG_LOC)/jdatadst.c \
	$(FLCNAV_JPEG_LOC)/jdatasrc.c \
	$(FLCNAV_JPEG_LOC)/jdcoefct.c \
	$(FLCNAV_JPEG_LOC)/jdcolor.c \
	$(FLCNAV_JPEG_LOC)/jddctmgr.c \
	$(FLCNAV_JPEG_LOC)/jdhuff.c \
	$(FLCNAV_JPEG_LOC)/jdinput.c \
	$(FLCNAV_JPEG_LOC)/jdmainct.c \
	$(FLCNAV_JPEG_LOC)/jdmarker.c \
	$(FLCNAV_JPEG_LOC)/jdmaster.c \
	$(FLCNAV_JPEG_LOC)/jdmerge.c \
	$(FLCNAV_JPEG_LOC)/jdphuff.c \
	$(FLCNAV_JPEG_LOC)/jdpostct.c \
	$(FLCNAV_JPEG_LOC)/jdsample.c \
	$(FLCNAV_JPEG_LOC)/jdtrans.c \
	$(FLCNAV_JPEG_LOC)/jerror.c \
	$(FLCNAV_JPEG_LOC)/jfdctflt.c \
	$(FLCNAV_JPEG_LOC)/jfdctfst.c \
	$(FLCNAV_JPEG_LOC)/jfdctint.c \
	$(FLCNAV_JPEG_LOC)/jidctflt.c \
	$(FLCNAV_JPEG_LOC)/jidctfst.c \
	$(FLCNAV_JPEG_LOC)/jidctint.c \
	$(FLCNAV_JPEG_LOC)/jidctred.c \
	$(FLCNAV_JPEG_LOC)/jquant1.c \
	$(FLCNAV_JPEG_LOC)/jquant2.c \
	$(FLCNAV_JPEG_LOC)/jutils.c \
	$(FLCNAV_JPEG_LOC)/jmemmgr.c \
	$(FLCNAV_JPEG_LOC)/armv6_idct.S

# the original android memory manager.
# use sdcard as libjpeg decoder's backing store
LOCAL_SRC_FILES += \
	$(FLCNAV_JPEG_LOC)/jmem-android.c

LOCAL_CFLAGS += -DAVOID_TABLES 
LOCAL_CFLAGS += -O3 -fstrict-aliasing -fprefetch-loop-arrays

# enable tile based decode
LOCAL_CFLAGS += -DANDROID_TILE_BASED_DECODE

# enable armv6 idct assembly
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS += -DANDROID_ARMV6_IDCT
endif

include $(BUILD_STATIC_LIBRARY)

else
LOCAL_SRC_FILES := \
	../lib/$(TARGET_ARCH_ABI)/lib$(LOCAL_MODULE).a
include $(PREBUILT_STATIC_LIBRARY)
endif
