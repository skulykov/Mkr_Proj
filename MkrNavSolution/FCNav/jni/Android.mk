# Relative definition of kernel makefile
FLCNAV_MAKEFILES := $(call my-dir)/../../kernel/Android.mk

# Protect from previous builds
ifneq ($(TARGET_ARCH_ABI),armeabi-v7a)
FLCNAV_BUILDING_NEON_LIBRARY := false
endif

# FLCNAV_FORCE_NEON_SUPPORT is used to force only NEON support on ARMv7a
ifdef FLCNAV_FORCE_NEON_SUPPORT
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
FLCNAV_BUILDING_NEON_LIBRARY := true
endif
endif

# By default, include makefiles only once
include $(FLCNAV_MAKEFILES)

# If we're not asked not to support NEON and not asked to support only NEON ARMv7a, then
# make additional build
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
ifndef FLCNAV_SKIP_NEON_SUPPORT
ifndef FLCNAV_FORCE_NEON_SUPPORT

FLCNAV_BUILDING_NEON_LIBRARY := true
include $(FLCNAV_MAKEFILES)

endif
endif
endif