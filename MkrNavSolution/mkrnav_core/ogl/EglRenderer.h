//
// Copyright 2011 Tero Saarni
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef RENDERER_H
#define RENDERER_H

#include <pthread.h>
#include <EGL/egl.h> 
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "glCtx.h"

#if SK_BUILD_FOR_ANDROID
    typedef ANativeWindow ANativeWindow_T;
#else
	typedef void ANativeWindow_T;
#endif

class Sk_Semaphore;
class Sk_Interface;

class EglRenderer {

public:
    EglRenderer(Sk_Interface *owner);
    virtual ~EglRenderer();

    // Following methods can be called from any thread.
    // They send message to render thread which executes required actions.
    void start();
    void stop();
	//void swapBuffers();
	void resetCtx();
	void destroyCtx();
	void setCtx(EGLDisplay display, EGLSurface surface, EGLContext context,int w,int h);
	void postSemaphore();
    void setWindow(ANativeWindow_T* window);
#if !SK_BUILD_FOR_ANDROID	
	void intfRenderLoop(){this->doDrawing();}
#endif
private:

    enum RenderThreadMessage {
        MSG_NONE = 0,
        MSG_WINDOW_SET,
        MSG_RENDER_LOOP_EXIT
    };
	Sk_Interface *intf;
    pthread_t _threadId;
    pthread_mutex_t _mutex;
    enum RenderThreadMessage _msg;
    
	Sk_Semaphore *rSemaphore;
    // android window, supported by NDK r5 and newer
    ANativeWindow_T* _window;

	EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;
    int _width;
    int _height;

	short renderingEnabled;
    //GLfloat _angle;
    
    // RenderLoop is called in a rendering thread started in start() method
    // It creates rendering context and renders scene until stop() is called
    void renderLoop();
	void doDrawing();
	short processMessage();

 #if SK_BUILD_FOR_ANDROID 
    bool initialize();
#else
#endif
    void destroy();

    
	
    // Helper method for starting the thread 
    static void* threadStartCallback(void *myself);

};

#endif // RENDERER_H
