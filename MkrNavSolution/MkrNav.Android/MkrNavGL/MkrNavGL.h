#if SK_BUILD_FOR_ANDROID
#include <jni.h>
#include <android/log.h>
#endif

class MkrNavGL
{
public:
	const char * getPlatformABI();
	MkrNavGL();
	~MkrNavGL();
};

