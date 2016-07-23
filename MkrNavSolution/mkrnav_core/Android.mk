LOCAL_PATH:= $(call my-dir)

#LOCAL_ARM_MODE := arm
#LOCAL_CFLAGS := -O3

# first lib, which will be built statically
#
include $(CLEAR_VARS)

STATIC_LIB_LOC := ./../static_libs

ifneq ($(FLCNAV_BUILDING_NEON_LIBRARY),true)
LOCAL_MODULE    := expat_lib
LOCAL_SRC_FILES := $(STATIC_LIB_LOC)/libexpat_static.a
else
LOCAL_MODULE    := expat_lib_neon
LOCAL_SRC_FILES := $(STATIC_LIB_LOC)/libexpat_static_neon.a

endif


include $(PREBUILT_STATIC_LIBRARY)

#===================================================================================
include $(CLEAR_VARS)

STATIC_LIB_LOC := ./../static_libs

ifneq ($(FLCNAV_BUILDING_NEON_LIBRARY),true)
LOCAL_MODULE    := png_lib
LOCAL_SRC_FILES := $(STATIC_LIB_LOC)/libpng.a
else
LOCAL_MODULE    := png_lib_neon
LOCAL_SRC_FILES := $(STATIC_LIB_LOC)/libpng_neon.a

endif

include $(PREBUILT_STATIC_LIBRARY)

#===================================================================================

include $(CLEAR_VARS)


STATIC_LIB_LOC := ./../static_libs
#ifneq ($(FLCNAV_BUILDING_NEON_LIBRARY),true)
#LOCAL_MODULE    := proto_lib
#LOCAL_SRC_FILES := $(STATIC_LIB_LOC)/libproto.a
#else
#LOCAL_MODULE    := proto_lib_neon
#LOCAL_SRC_FILES := $(STATIC_LIB_LOC)/libproto_neon.a
#endif

NET_LIB_LOC := ./../FCNavGL/proto_lib/obj/local/armeabi-v7a
ifneq ($(FLCNAV_BUILDING_NEON_LIBRARY),true)
LOCAL_MODULE    := proto_lib
LOCAL_SRC_FILES := $(NET_LIB_LOC)/lib_proto.a
else
LOCAL_MODULE    := proto_lib_neon
LOCAL_SRC_FILES := $(NET_LIB_LOC)/lib_proto.a
endif

include $(PREBUILT_STATIC_LIBRARY)
#===================================================================================
include $(CLEAR_VARS)




STATIC_LIB_LOC := ./../static_libs
ifneq ($(FLCNAV_BUILDING_NEON_LIBRARY),true)
LOCAL_MODULE    := ft2_lib
LOCAL_SRC_FILES := $(STATIC_LIB_LOC)/libft2_static.a
else
LOCAL_MODULE    := ft2_lib_neon
LOCAL_SRC_FILES := $(STATIC_LIB_LOC)/libft2_static_neon.a
endif
include $(PREBUILT_STATIC_LIBRARY)
#===================================================================================
include $(CLEAR_VARS)


#STATIC_LIB_LOC := ./../static_libs
#NET_LIB_LOC :=  C:\my\FCNavigation\FCNavGL\FCNavGL\network_lib\obj\local\armeabi-v7a
NET_LIB_LOC := ./../FCNavGL/network_lib/obj/local/armeabi-v7a
ifneq ($(FLCNAV_BUILDING_NEON_LIBRARY),true)
LOCAL_MODULE    := lib_network
LOCAL_SRC_FILES := $(NET_LIB_LOC)/lib_net.a
else
LOCAL_MODULE    := lib_network_neon
LOCAL_SRC_FILES := $(NET_LIB_LOC)/lib_net.a
endif
include $(PREBUILT_STATIC_LIBRARY)
#===================================================================================
include $(CLEAR_VARS)


NET_LIB_LOC := ./../FCNavGL/tts_lib/obj/local/armeabi-v7a
ifneq ($(FLCNAV_BUILDING_NEON_LIBRARY),true)
LOCAL_MODULE    := lib_mtts
LOCAL_SRC_FILES := $(NET_LIB_LOC)/lib_tts.a
else
LOCAL_MODULE    := lib_mtts_neon
LOCAL_SRC_FILES := $(NET_LIB_LOC)/lib_tts.a
endif
include $(PREBUILT_STATIC_LIBRARY)
#===================================================================================
include $(CLEAR_VARS)





ifneq ($(FLCNAV_BUILDING_NEON_LIBRARY),true)
LOCAL_MODULE    := fcnavgl_lib
else
LOCAL_MODULE    := fcnavgl_lib_neon
LOCAL_ARM_NEON:= true
endif


LOCAL_CFLAGS := \
	-DGOOGLE_PROTOBUF_NO_RTTI \
	-DANDROID_BUILD \
	-DANDROID=1 \
	-DSK_BUILD_FOR_ANDROID=1 \
	-DSK_BUILD_FOR_ANDROID_NDK=1 \
	-DSK_ALLOW_STATIC_GLOBAL_INITIALIZERS=0 \
	-DSK_RELEASE \
	-DGR_RELEASE=1 \
    -DUSE_VBO_FOR_DRAWING=1  \
    -D__GXX_EXPERIMENTAL_CXX0X__


LOCAL_CFLAGS += -mfpu=vfp3

LOCAL_CFLAGS += -fPIC

#VisualGDBAndroid: AutoUpdateSourcesInNextLine

M_LIB_PATH := C:\my\FCNavigation\FCNavGl\jni
PROTOBUF := C:\my\FCNavigation\kernel4GL\protobuf
#FLCNAV_SKIA_ABS := C:\my\FCNavigation\kernel4GL\skia\skia_library

FLNAV_CORE:= C:\my\FCNavigation\kernel4GL
#source file produce error when use full path -> Error : *** target pattern contains no `%'.

#FLNAV_PATH:= ../../kernel4GL/flcnav/src 
#FLNAV_PATH := $(abspath $(call my-dir)/../../kernel4GL/flcnav/src)

ANDROID_INC:=C:\Android\android-ndk\sources\cxx-stl\gnu-libstdc++\include

# Include paths
LOCAL_C_INCLUDES := $(M_LIB_PATH)/core \
    $(M_LIB_PATH)/ogl \
	$(M_LIB_PATH)/net \
	$(M_LIB_PATH)/tts \
	$(M_LIB_PATH)/jni \
	$(M_LIB_PATH)/external/png/png_library \
    $(PROTOBUF) \
	$(FLNAV_CORE)/expat/expat_library/lib \
	$(FLNAV_CORE)/freetype/freetype_library/include 
   # $(ANDROID_INC)/bits \
   #	$(ANDROID_INC)
	


#LOCAL_CFLAGS += -frtti
#LOCAL_CFLAGS  += -std=c++0x
#LOCAL_CFLAGS  += -std=c++11

LOCAL_SRC_FILES := ogl/interface.cpp ogl/matrix.cpp ogl/ogl.cpp ogl/Sk_Draw.cpp ogl/Sk_DrawT.cpp ogl/jniapi.cpp ogl/EglRenderer.cpp JNI/MkrJNI.cpp JNI/MkrMsgBox.cpp JNI/MkrNVMngr.cpp JNI/MkrSoundMngr.cpp JNI/MKrTimerMngr.cpp JNI/MkrTtsMngr.cpp JNI/sk_msgbox.cpp JNI/tts_a_prov.cpp main/AndroidFileIO.cpp main/download.cpp main/sk_io.cpp main/sk_main.cpp main/sound.cpp core/CMapRenderer.cpp core/common.cpp core/filemng.cpp core/mapObjects.cpp core/map_binary.cpp core/multipolygons.cpp core/poly.cpp core/renderm.cpp core/renderRules.cpp core/rendert.cpp core/proto/flcnav_index.pb.cpp route/Route.cpp route/CRoute.cpp route/RouteP.cpp route/TurnInfo.cpp core/searchAddr.cpp


ifneq ($(LOCAL_ARM_NEON),true)
LOCAL_STATIC_LIBRARIES := \
	proto_lib \
	png_lib \
	ft2_lib \
	expat_lib \
	lib_network \
    lib_mtts

else
LOCAL_STATIC_LIBRARIES := \
	proto_lib_neon \
	png_lib_neon \
	ft2_lib_neon \
	expat_lib_neon \
    lib_network_neon \
    lib_mtts_neon

endif


LOCAL_LDLIBS    := -lz -ldl -llog -landroid -lEGL -lGLESv2 -lOpenSLES
LOCAL_LDLIBS    += -L$(M_LIB_PATH)/external -lssl -lcrypto -lsqlite

# to generate map file
#LOCAL_LDFLAGS := -Wl,-Map,app.map 

#without this line I had an error: dlopen("/data/app-lib/com.fcnavgl.nintf-4/libfcnavgl_lib_neon.so") failed: dlopen failed: cannot locate symbol "__exidx_end" referenced by "libfcnavgl_lib_neon.so"...
# r8c, r8d: bug with __exidx_end & __exidx_start? 
LOCAL_LDLIBS += -fuse-ld=bfd

include $(BUILD_SHARED_LIBRARY)

# $(call import-module,android/cpufeatures)
