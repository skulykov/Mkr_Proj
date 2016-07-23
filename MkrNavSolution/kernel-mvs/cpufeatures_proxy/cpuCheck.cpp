#include <jni.h>
#include <cpu-features.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL Java_net_flcnav_plus_render_NativeFlcnavLibrary_getCpuCount( JNIEnv* ienv, jobject obj) {
	return android_getCpuCount();
}

JNIEXPORT jboolean JNICALL Java_net_flcnav_plus_render_NativeFlcnavLibrary_cpuHasNeonSupport( JNIEnv* ienv, jobject obj) {
	return (android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM && (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) == ANDROID_CPU_ARM_FEATURE_NEON) ? JNI_TRUE : JNI_FALSE;
}

#ifdef __cplusplus
}
#endif
