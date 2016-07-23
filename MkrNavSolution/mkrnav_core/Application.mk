# Build both ARMv5TE and ARMv7-A machine code.
NDK_TOOLCHAIN_VERSION=4.8

#APP_ABI := armeabi armeabi-v7a
APP_ABI := armeabi-v7a 
APP_STL := stlport_shared
#APP_STL := gnustl_static
#APP_CPPFLAGS := -frtti 
FLCNAV_BUILDING_NEON_LIBRARY:= true




