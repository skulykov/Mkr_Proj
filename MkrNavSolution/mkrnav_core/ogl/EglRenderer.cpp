
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#if SK_BUILD_FOR_ANDROID
#include <android/native_window.h> // requires ndk r5 or newer
#endif
//#include "EglRenderer.h"
#include "interface.h"
#include "../core/dbg.h"


#define LOG_TAG "Egl_Cpp"


EglRenderer::EglRenderer(Sk_Interface *owner)
{
    LOGI("EglRenderer instance created");
    pthread_mutex_init(&_mutex, 0); 
	rSemaphore=new Sk_Semaphore(0);
    resetCtx();
    this->intf=owner;
	renderingEnabled=0;
    return;
}
void EglRenderer::resetCtx()
{
	_display = EGL_NO_DISPLAY;
    _surface = EGL_NO_SURFACE;
    _context = EGL_NO_CONTEXT;
	
	_width=0;
	_height=0;
}
EglRenderer::~EglRenderer()
{
   
    pthread_mutex_destroy(&_mutex);
	if(rSemaphore)
		delete rSemaphore;
	 LOGI("EglRenderer instance destroyed");
    return;
}

void EglRenderer::start()
{
    LOGI("Creating renderer thread");
    pthread_create(&_threadId, 0, threadStartCallback, this);
    return;
}

void EglRenderer::stop()
{
    LOGI("Stopping renderer thread");

    // send message to render thread to stop rendering
    pthread_mutex_lock(&_mutex);
    _msg = MSG_RENDER_LOOP_EXIT;
    pthread_mutex_unlock(&_mutex);    
	rSemaphore->post();            // ---------Notify renderer
    pthread_join(_threadId, 0);
	for(;;)
	{
		if(renderingEnabled==2)
			break;
	}
    LOGI("EglRenderer thread stopped");

    return;
}
void EglRenderer::setWindow(ANativeWindow_T *window)
{
    // notify render thread that window has changed
    pthread_mutex_lock(&_mutex);
    _msg = MSG_WINDOW_SET;
    _window = window;
    pthread_mutex_unlock(&_mutex);
	rSemaphore->post();            // ---------Notify renderer
    return;
}
//void EglRenderer::swapBuffers()
//{
//	//LOGI("swapBuffers()  d:%x c:%x ",_display,_surface);
//	eglSwapBuffers(_display,_surface);
//}
void EglRenderer::postSemaphore()
{
	rSemaphore->post();
}
void EglRenderer::setCtx(EGLDisplay display, EGLSurface surface, EGLContext context,int w,int h)
{
	_display = display;
    _surface = surface;
    _context = context;
	
	_width=w;
	_height=h;
	//LOGI("Sk_Ogl::setCtx  d:%x c:%x ",_display,_surface);
}

#if SK_BUILD_FOR_ANDROID

bool EglRenderer::initialize()
{
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 5,
            EGL_GREEN_SIZE, 6,
            EGL_RED_SIZE, 5,
            EGL_DEPTH_SIZE, 1,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_SAMPLE_BUFFERS, 1,
			EGL_SAMPLES, 4,
            EGL_NONE
    };

    EGLDisplay display;
    EGLConfig config;    
    EGLint numConfigs;
    EGLint format;
    EGLSurface surface;
    EGLContext context;
    EGLint width;
    EGLint height;
    GLfloat ratio;
    
    LOGI("Initializing context");
    
    if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay() returned error %d", eglGetError());
        return false;
    }
    if (!eglInitialize(display, 0, 0)) {
        LOGE("eglInitialize() returned error %d", eglGetError());
        return false;
    }

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
        LOGE("eglChooseConfig() returned error %d", eglGetError());
        destroy();
        return false;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        LOGE("eglGetConfigAttrib() returned error %d", eglGetError());
        destroy();
        return false;
    }

    ANativeWindow_setBuffersGeometry(_window, 0, 0, format);

    if (!(surface = eglCreateWindowSurface(display, config, _window, 0))) {
        LOGE("eglCreateWindowSurface() returned error %d", eglGetError());
        destroy();
        return false;
    }
    
	int attrib_list[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    if (!(context = eglCreateContext(display, config, 0, attrib_list))) {
        LOGE("eglCreateContext() returned error %d", eglGetError());
        destroy();
        return false;
    }
    
    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOGE("eglMakeCurrent() returned error %d", eglGetError());
        destroy();
        return false;
    }

    if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
        !eglQuerySurface(display, surface, EGL_HEIGHT, &height)) {
        LOGE("eglQuerySurface() returned error %d", eglGetError());
        destroy();
        return false;
    }

	/*_display=display;
	_surface=surface;*/
	setCtx(display,surface,context,width,height);
    //LOGI("setCtx...DONE  d:%x s:%x c:%x %d %d",display,surface,context,width,height);
	 if(intf!=NULL)
	 {
		 intf->initialize_ogl(width,  height);
	 }
	 
	 LOGI("Initializing context...DONE  d:%x s:%x c:%x %d %d",display,surface,context,width,height);
    return true;
}


#endif

void EglRenderer::doDrawing()
{
	
	if (_display) {
            if(intf!=NULL)
			{
				if(intf->drawFrame())
				{
					if (!eglSwapBuffers(_display, _surface)) {
						LOGE("eglSwapBuffers() returned error %d", eglGetError());
					}
					//usleep(10);
				}
				//if(intf->drawFrame())
				//{
				//	if (!eglSwapBuffers(_display, _surface)) {
				//		LOGE("eglSwapBuffers() returned error %d", eglGetError());
				//	}
				//	//usleep(10);
				//}
			}
        }
}
short EglRenderer::processMessage()
{
	short rv=1;	
	pthread_mutex_lock(&_mutex);

        // process incoming messages
        switch (_msg) {

            case MSG_WINDOW_SET:
#if SK_BUILD_FOR_ANDROID                
				initialize();
#else
#endif
                break;

            case MSG_RENDER_LOOP_EXIT:
                rv = 0;
                destroy();
                break;

            default:
                break;
        }
        _msg = MSG_NONE;
        
              
        pthread_mutex_unlock(&_mutex);

		return rv;
}

void EglRenderer::renderLoop()
{
    renderingEnabled = 1;
    
    LOGI("GOTO >> renderLoop()");

    while (renderingEnabled) {

       rSemaphore->wait();  // must be a blocked call
       
	   if(_msg != MSG_NONE)
		  renderingEnabled = processMessage();
	   else	
		doDrawing();
    }
    
    LOGI("Render loop exits");
    
    return;
}
void EglRenderer::destroyCtx()
{
	//LOGI("Sk_Ogl::destroyCtx  d:%d c:%d ",_display,_surface);
	eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(_display, _context);
    eglDestroySurface(_display, _surface);
    eglTerminate(_display);
 
	resetCtx();
}
void EglRenderer::destroy() {
    LOGI("Destroying context");

   destroyCtx();

    return;
}


void* EglRenderer::threadStartCallback(void *myself)
{
    EglRenderer *renderer = (EglRenderer*)myself;

    renderer->renderLoop();
  //  pthread_exit(0);
    LOGI("threadStartCallback... EXIT!!!");
	renderer->renderingEnabled = 2;
    return 0;
}



