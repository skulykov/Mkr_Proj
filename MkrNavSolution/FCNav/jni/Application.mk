APP_STL := stlport_shared
APP_ABI := all
#APP_ABI := armeabi-v7a
APP_CPPFLAGS := -fno-rtti -fno-exceptions

ifdef FLCNAV_X86_ONLY
APP_ABI := x86
else
ifdef FLCNAV_ARM_ONLY
APP_ABI := armeabi armeabi-v7a
endif
ifdef FLCNAV_ARMv5_ONLY
APP_ABI := armeabi
endif
ifdef FLCNAV_ARMv7a_ONLY
APP_ABI := armeabi-v7a
endif
endif

ifndef FLCNAV_DEBUG_NATIVE
# Force release compilation in release optimizations, even if application is debuggable by manifest
APP_OPTIM := release
endif