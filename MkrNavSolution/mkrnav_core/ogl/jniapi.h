
#ifndef JNIAPI_H
#define JNIAPI_H

extern "C" {

	JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_inititializeOnStart(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_setCurrentLocation(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_animate(JNIEnv * env, jobject obj);
	//JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_FCNavMainActivity_onCreateN(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_drag(JNIEnv * env, jobject obj,jfloat dx,jfloat dy,jint flag);
	JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_keyUpDown(JNIEnv * env, jobject obj,jfloat x,jfloat y,jint flag);
	JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_routeOption(JNIEnv * env, jobject obj,jfloat x,jfloat y,jint flag);
	JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_zoom(JNIEnv * env, jobject obj,/*jfloat lat,jfloat lon,*/jint zoom);
	JNIEXPORT jint JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_rSearchAddress(JNIEnv * env, jobject obj,jstring jstr);
	JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_locationChanged(JNIEnv * env, jobject obj,jfloat lat,jfloat lon,jint speed,jint cog);
	JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_setProj(JNIEnv * env, jobject obj,jint flag);
	//JNIEXPORT jboolean JNICALL Java_net_flcnav_plus_render_NativeFlcnavLibrary_cpuHasNeonSupport( JNIEnv* ienv, jobject obj);

	JNIEXPORT jboolean JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_isRenderingDone(JNIEnv * env, jobject obj);

//=========================================================================================================================


    JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavMainAct_nativeOnCreate(JNIEnv* jenv, jobject obj);
    JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavMainAct_nativeOnResume(JNIEnv* jenv, jobject obj);
    JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavMainAct_nativeOnPause(JNIEnv* jenv, jobject obj);
    JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavMainAct_nativeOnStop(JNIEnv* jenv, jobject obj);
    JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavMainAct_nativeSetSurface(JNIEnv* jenv, jobject obj, jobject surface);
};

#endif // JNIAPI_H
