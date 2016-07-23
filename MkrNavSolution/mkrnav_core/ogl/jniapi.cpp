
#include "../core/dbg.h"
#include "EglRenderer.h"

#include "interface.h"
#include "../JNI/sk_msgbox.h"
#include "../main/sk_main.h"

#include "jniapi.h"

#define LOG_TAG "EglCpp"

static JavaVM* globalJVM = NULL;
static JNIEnv* gJniEnv=NULL;
static jobject gObj=NULL;

extern Sk_Interface *gInf;

static ANativeWindow *window = 0;
//static EglRenderer *renderer = 0;

//void commit_draw()
//{
//	if(renderer!=0)
//		renderer->swapBuffers();
//}

JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavMainAct_nativeOnCreate(JNIEnv* jenv, jobject obj)
{
    LOGI("nativeOnCreate");
    initialize_Inf();//renderer = new EglRenderer();
    return;
}

JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavMainAct_nativeOnResume(JNIEnv* jenv, jobject obj)
{
    LOGI("nativeOnResume");
    renderer_start();
    return;
}

JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavMainAct_nativeOnPause(JNIEnv* jenv, jobject obj)
{
    LOGI("nativeOnPause");
    renderer_stop();
    return;
}

JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavMainAct_nativeOnStop(JNIEnv* jenv, jobject obj)
{
    LOGI("nativeOnStop");
    renderer_delete();
    //renderer = 0;
    return;
}

JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavMainAct_nativeSetSurface(JNIEnv* jenv, jobject obj, jobject surface)
{
    if (surface != 0) {
        window = ANativeWindow_fromSurface(jenv, surface);
        LOGI("Got window %p", window);
        renderer_setWindow(window);
    } else {
        LOGI("Releasing window");
        ANativeWindow_release(window);
    }

    return;
}


//============================================================================================================


extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
	//flcnav_log_print(LOGI, "FCNav Native Lib_app JNI_OnLoad start"); 

	JNIEnv* globalJniEnv;
	if(vm->GetEnv((void **)&globalJniEnv, JNI_VERSION_1_6))
		return JNI_ERR; /* JNI version not supported */
	globalJVM = vm;
	/*loadJniRenderingContext(globalJniEnv);
	loadJniRenderingRules(globalJniEnv);
	jclassIntArray = findClass(globalJniEnv, "[I");
	jclassString = findClass(globalJniEnv, "java/lang/String");*/

	//jboolean rv=(android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM && (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) == ANDROID_CPU_ARM_FEATURE_NEON) ? JNI_TRUE : JNI_FALSE;
	
	/*if(rv) LOGI("!!!!!!JNI_OnLoad completed!!!!!NEON-YES");
	else */
		LOGI("!!!JNI_OnLoad completed...");
	
	return JNI_VERSION_1_6;
}
//JNIEXPORT jboolean JNICALL Java_net_flcnav_plus_render_NativeFlcnavLibrary_cpuHasNeonSupport( JNIEnv* ienv, jobject obj) {
//	return (android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM && (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) == ANDROID_CPU_ARM_FEATURE_NEON) ? JNI_TRUE : JNI_FALSE;
//}
//JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_FCNavMainActivity_onCreateN(JNIEnv * env, jobject obj)
//{
//	//flcnav_log_print(LOGI, "FCNav Native Lib_app JNI_OnLoad start"); 
//
//	
//	gJniEnv=env;
//	gObj=obj;
//	/*loadJniRenderingContext(globalJniEnv);
//	loadJniRenderingRules(globalJniEnv);
//	jclassIntArray = findClass(globalJniEnv, "[I");
//	jclassString = findClass(globalJniEnv, "java/lang/String");*/
//
//	LOGI("!!!!!!FCNavMainActivity_onCreateN");
//	
//	
//}

void invokeRender()
{
	if(gJniEnv==NULL)
		return;
	JNIEnv* env=gJniEnv;
	jstring jstr = env->NewStringUTF("Inv");
	jclass clazz = env->FindClass("com/fcnavgl/nintf/FCNavMainActivity");
    jmethodID messageMe = env->GetMethodID(clazz, "invokeRender", "(Ljava/lang/String;)V");
	// Call the method on the object
    jobject result = env->CallObjectMethod(jstr, messageMe);

	// Get a C-style string
    const char* str = env->GetStringUTFChars((jstring) result, NULL);
    LOGI("Native invokeRender %s\n", str);
        // Clean up
    env->ReleaseStringUTFChars(jstr, str);

}
//http://stackoverflow.com/questions/10847979/jni-call-fail-on-non-static-methods
void invokeRender1(JNIEnv * env, jobject obj) //JNIEnv * env, jobject obj
{
	/*if(gJniEnv==NULL || gObj==NULL)
		return;
	JNIEnv* env=gJniEnv;*/
	jclass clazz = /*env->GetObjectClass(obj);*/env->FindClass("com/fcnavgl/nintf/FCNavMainActivity");
    if (clazz == 0) {
        LOGI("FindClass error");
        return;
    }
    jmethodID javamethod = env->GetStaticMethodID(clazz, "invokeRender1", "()V");
    if (javamethod == 0) {
        LOGI("GetMethodID error");
        return;
    }
   // env->CallVoidMethod(obj, javamethod);
   env->CallStaticVoidMethod(clazz, javamethod);
}
void invokeRender2() //JNIEnv * env, jobject obj
{
	if(gJniEnv==NULL || gObj==NULL)
		return;
	JNIEnv* env=gJniEnv;
	jclass clazz = /*env->GetObjectClass(obj);*/env->FindClass("com/fcnavgl/nintf/MkrNavMainAct");
    if (clazz == 0) {
        LOGI("FindClass error");
        return;
    }
    jmethodID javamethod = env->GetStaticMethodID(clazz, "invokeRender1", "()V");
    if (javamethod == 0) {
        LOGI("GetMethodID error");
        return;
    }
   // env->CallVoidMethod(obj, javamethod);
   env->CallStaticVoidMethod(clazz, javamethod);
}


JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_inititializeOnStart(JNIEnv * env, jobject obj)
{
	if(gInf)
	{
		//gInf->inititializeRenderingOnStart();
	}
}
JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_setCurrentLocation(JNIEnv * env, jobject obj)
{
 if(gInf)
		gInf->setCurrentLocation();
}
JNIEXPORT jboolean JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_isRenderingDone(JNIEnv * env, jobject obj)
{
	/*if(gInf==NULL)
		return false;
	return gInf->isRenderingInProcess()?false:true;*/

}

JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_routeOption(JNIEnv * env, jobject obj,jfloat x,jfloat y,jint flag)
{
	if(gInf)
		gInf->setRouteOption(x,y,flag);

}
JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_setVoiceOn(JNIEnv * env, jobject obj,jboolean flag)
{
	if(gInf)
		gInf->setVoiceOn(flag);

}
JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_drag(JNIEnv * env, jobject obj,jfloat dx,jfloat dy,jint flag)
{
	
    //LOGI("MkrNavLib_drag..dx=%f dy=%f\n",dx,dy);
	if(gInf)
		gInf->dragging(dx,dy,1);

}
JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_keyUpDown(JNIEnv * env, jobject obj,jfloat x,jfloat y,jint flag)
{
	
    //LOGI("MkrNavLib_drag..dx=%f dy=%f\n",dx,dy);
	if(gInf)
		gInf->keyUpDown(x,y,flag);

}
//void test_tts();
JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_zoom(JNIEnv * env, jobject obj/*,jfloat lat,jfloat lon*/,jint zoom)
{
    LOGI("MkrNavLib_Zoom..%d\n",zoom);
	if(gInf!=NULL)
	{
		//test_tts();//gInf->setZoom(zoom>0?true:false);
	}
}
JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_locationChanged(JNIEnv * env, jobject obj,jfloat lat,jfloat lon,jint speed,jint cog)
{
    LOGI("Location Changed..%f %f\n",lat,lon);
	if(gInf!=NULL)
	{
		gInf->locationChanged(lat,lon,speed,cog);
	}
	//LOGI("Location Changed..done\n");
}

JNIEXPORT jint JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_rSearchAddress(JNIEnv * env, jobject obj,jstring jstr)
{
	if(gInf)
	{
		const char * str = env->GetStringUTFChars(jstr , NULL ) ;
		string s(str);
		env->ReleaseStringUTFChars(jstr, str);

		gInf->requestSearchAddress(s);
	}
}

JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_setProj(JNIEnv * env, jobject obj,jint flag)
{
    if(gInf!=NULL)
		gInf->getOgl()->rp.useProjection=flag==1?true:false;
}
JNIEXPORT void JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_animate(JNIEnv * env, jobject obj)
{
    LOGI("Activate animate()..\n");
	//animate();// renderFrame();
	//start_animate=1;
	//invokeRender1(env,obj);
	//invokeRender2();

	test_msg_box();
	//sk_map_download_region ("canada", 11);
}

#if 0
//http://stackoverflow.com/questions/1610045/how-to-return-an-array-from-jni-to-java
JNIEXPORT jintArray JNICALL Java_ArrayTest_initIntArray(JNIEnv *env, jclass cls, int size)
{
 jintArray result;
 result = env->NewIntArray(size);
 if (result == NULL) {
     return NULL; /* out of memory error thrown */
 }
 int i;
 // fill a temp structure to use to populate the java int array
 jint fill[256];
 for (i = 0; i < size; i++) {
     fill[i] = 0; // put whatever logic you want to populate the values here.
 }
 // move from the temp structure to the java structure
 (*env)->SetIntArrayRegion(env, result, 0, size, fill);
 return result;
}
#endif

