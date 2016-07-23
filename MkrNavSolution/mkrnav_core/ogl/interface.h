#ifndef GLINTERFACE_H
#define GLGLINTERFACE_H

#include <stdint.h>
#if SK_BUILD_FOR_ANDROID
#include <jni.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer
#include <android\asset_manager_jni.h>
#endif
//#include <unistd.h>   // need for usleep
#include <pthread.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "CMapRenderer.h"

#include "glCtx.h"
#include "Sk_Draw.h"
#include "Sk_Texture.h"

#include "EglRenderer.h"

#include "MapUtils.h"

int main_fsinit();
void initializeAtlas(Sk_Ogl *gc);

#define		INIT_ZOOM	16
#define		INIT_LAT	43.854954695691774
#define		INIT_LON	-79.51239109039306

enum DRAGGING_STATE{
	DRAGGING_START=1,
	DRAGGING_INPROGRESS=2,
	DRAGGING_END=3
};
class Sk_Interface
{
private:
	bool fs_initialized;
	bool gl_initialized;
	bool lock_redraw;
	bool ready_to_draw;
	bool request4MapObjectsPreparationInProcess;
	bool routingInProcess;
	bool searchAddressInProcess;

	bool voiceOn;

	CMapRenderer *mapRenderer;
	Sk_Ogl *ogl;
	EglRenderer *renderer;

	float curLat,curLon;
	int speed,cog;
	bool curLocReceived;
	
	


	int prev_zoom;
	/*int _zoom,prev_zoom;
    float _latitude,_longitude;*/

	//dragging
	float X0,Y0,sumX,sumY;
	int draggingState;

	
	//RequestRendering Callbacks
	static void cbRequest4MapObjectsPreparation();
	

	static void cbRouting();

	vector<Sk_Overlay *> overlays;
public:
	
//	int draw_state; 
	Sk_Mutex *rr_mutex;  // request rendering mutex
	
	Sk_Interface()
	{
		fs_initialized=false;
		gl_initialized=false;
		ready_to_draw=false;
		lock_redraw=false;    // atomic_t  ??? it's used for a simple lock in multithreading     
		request4MapObjectsPreparationInProcess=false;
		routingInProcess=false;
		searchAddressInProcess=false;


		curLocReceived=false;
		rr_mutex=new Sk_Mutex();
		renderer=new EglRenderer(this);
		ogl=new Sk_Ogl();
		mapRenderer=new CMapRenderer(ogl);
		
		

		X0=(float) SHIFT_SCENE_LEFT,Y0=(float) SHIFT_SCENE_TOP;
		sumX=(float) SHIFT_SCENE_LEFT,sumY=(float) SHIFT_SCENE_TOP;  // drag calculation

		prev_zoom=0;
		draggingState=0;
		voiceOn=true;
	}
	~Sk_Interface()
	{
		if(rr_mutex)
			delete rr_mutex;
		if(ogl)
			delete ogl;
		if(mapRenderer)
			delete mapRenderer;
		
	}
	EglRenderer *getEglRenderer(){return renderer;}
	CMapRenderer *getMapRenderer(){return mapRenderer;}
	
	/*int getZoom(){return _zoom;}
	void setZoom(int z){_zoom=z;}
	void setLatLon(float y, float x){_latitude=y;_longitude=x;}*/
	//MapTile View
	int getTileSize(){return 256;}
	float calcDiffTileY(float dx, float dy) {return dy / getTileSize();}
	float calcDiffTileX(float dx, float dy) {return dx / getTileSize();}
	float getXTile() {return (float) MapUtils::GetTileNumberX(ogl->getZoom(), ogl->getLon());}
	float getYTile() {return (float) MapUtils::GetTileNumberY(ogl->getZoom(), ogl->getLat());}

	void initialize_ogl(int w,int h);
	Sk_Ogl *getOgl(){return ogl;}
	void inititializeRenderingOnStart();
	
	int request4MapObjectsPreparation (float lat,float lon,int zoom , cbThreadFunc_T _cb);
	static void* threadRRProc(void* _arg);
	int doRequestRendering (float lat,float lon,int zoom );

	bool drawFrame ();
	void drag (float dx,float dy,int flag);
	void dragging(float x,float y,int flag);  // the screen is being dragged
	void keyUpDown(float x,float y,int key);  // the screen has been touched/released
	void initDragSumXY();
	
	void setZoom(bool plus);

	int requestSearchAddress(string s);
	static void*  threadSearchAddressProc(void* _arg);
	void setSearchAddressInProcess(bool f){searchAddressInProcess=f;}
	bool isSearchAddressInProcess(){return searchAddressInProcess;}

	void locationChanged (float lat,float lon, int speed,int cog);
	void drawCurLoc();
	void setCurrentLocation();
	void drawOverlays();
	int checkBounds(int w,int h);
	LatLon_T getLatLonFromScreenPoint(float x, float y);
	bool isReadyToDraw(){return ready_to_draw;}
	void setRequest4MapObjectsPreparationInProcess(bool f){request4MapObjectsPreparationInProcess=f;}
	bool isRequest4MapObjectsPreparationInProcess(){return request4MapObjectsPreparationInProcess;}

	bool getVoiceOn(){return voiceOn;}
	void setVoiceOn(bool v){voiceOn=v;}
	void setRouteOption(float x,float y,int v);
	int requestRouting (float lat1,float lon1,float lat2,float lon2 , cbThreadFunc_T _cb);
	static void* threadRoutingProc(void* _arg);
	void setRoutingInProcess(bool f){routingInProcess=f;}
	bool isRoutingInProcess(){return routingInProcess;}
	
	
	void signalReDraw();

	void rendererStart();
	void rendererStop();
	void rendererSetWindow(ANativeWindow_T *window);
	void rendererDelete();
};

void initialize_Inf();
void renderer_start();
void renderer_stop();
void renderer_delete();
void renderer_setWindow(ANativeWindow_T *window);
#if SK_BUILD_FOR_ANDROID
AAssetManager* getAssetMng();  
#endif
#endif  //GLINTERFACE_H