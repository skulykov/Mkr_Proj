#include "interface.h"
#include "../JNI/sk_msgbox.h"
#include "../main/sk_main.h"
//#include <cpu-features.h>

Sk_Interface *gInf=NULL;  // global interface var

//double lon=-79.50100243091583,lat=43.84997990511598;   //start point -> lauderdale
//----------------------------------------------------------------------------------------------
#if SK_BUILD_FOR_ANDROID
const char *test__argv__[]={
	"-fcnav",
	//"-renderingOutputFile=1.png", 
	"-zoom=17",
	//"-lt=8,4",  //8*256=2132  4*256 = 1128
	"-lt=4,8",  //8*256=2132  4*256 = 1128
    "-lbox=-79.50100,43.84997",
   // "-lbox=-79.51039109039306,43.854954695691774",
	"-renderingInputFile=/mnt/sdcard/fcnavgl/maps/ontario.obf",
	"-renderingInputFile=/mnt/sdcard/fcnavgl/maps/basemap.obf",
  //"-renderingStyleFile=C:\\my\\FCNavigation\\DataExtractionOSM\\src\\net\\flcnav\\render\\default.render.xml",
	"-renderingStyleFile=/mnt/sdcard/fcnavgl/maps/render.xml",
    "-imagesBasePathFile=drawable-mdpi"
};
#else
const char *test__argv__[]={
	"-fcnav",
	"-renderingOutputFile=C:\\my\\M_Maps\\obf\\1.png", 
	"-zoom=17",
	"-lt=8,4",  //8*256=2132  4*256 = 1128
   // "-lbox=-79.49917316436767,43.85117915386791",
    "-lbox=-79.49617,43.857985",                              //Major Mackenzie
   //"-lbox=-79.50100,43.84997",                              // lauderdale Dr
	"-renderingInputFile=C:\\my\\M_Maps\\obf\\ontario.obf",
	"-renderingInputFile=C:\\my\\M_Maps\\obf\\basemap.obf",
  //"-renderingStyleFile=C:\\my\\FCNavigation\\DataExtractionOSM\\src\\net\\flcnav\\render\\default.render.xml",
	"-renderingStyleFile=C:\\my\\M_Maps\\obf\\render.xml",
    "-imagesBasePathFile=C:\\my\\FCNavigation\\FCNav\\res\\drawable-mdpi"
};
#endif
#pragma region Sk_Interface
//------------------------------------------------------
void initialize_Inf()
{
	 if(gInf==NULL)
	 {
		gInf=new Sk_Interface();
#if SK_BUILD_FOR_ANDROID
		//sk_main_start_init();    // not sure if we need it !!!
#endif
	 }
	 
}

void renderer_start()
{
	if(gInf!=NULL)
	{
		gInf->rendererStart();
	}
}
void renderer_stop()
{
	if(gInf!=NULL)
	{
		gInf->rendererStop();
	}
}
void renderer_delete()
{
	if(gInf!=NULL)
	{
		gInf->rendererDelete();
	}
}
void renderer_setWindow(ANativeWindow_T *window)
{
	gInf->rendererSetWindow(window);
}
//-------------------------------------------------------

	void Sk_Interface::rendererStart()
	{
		if(renderer==0)
			renderer=new EglRenderer(this);
		renderer->start();
	}
	void Sk_Interface::rendererStop()
	{
		renderer->stop();
	}
	void Sk_Interface::rendererSetWindow(ANativeWindow_T *window)
	{
		renderer->setWindow(window);
	}
	void Sk_Interface::rendererDelete()
	{
		if(renderer)
			delete renderer;
		renderer=0;
	}



void Sk_Interface::initialize_ogl(int w,int h)
{
    if(!fs_initialized)
	{
		//main_fsinit();
    	mapRenderer->initFromCL(9-1,(char **)test__argv__);
		fs_initialized=true;
	}
	LOGI("Init with width=%d height=%d\n",w,h);
	int rv =ogl->init (/*1024,768*/w,h); 
	if ( rv==0)
     		LOGE("COULD NOT INIT\n");
	//every time the orientation chages we need to reinitialize texture in atlas!!!!!!!!!!!!!
	initializeAtlas(ogl);
	ogl->updateScreenSize(w,h);
	
	gl_initialized=true;

	inititializeRenderingOnStart();
}
void Sk_Interface::inititializeRenderingOnStart()
{
	//from java code inititializeOnStart
#if SK_BUILD_FOR_ANDROID	
	AAssetManager* am=getAssetMng();
	if(am==NULL)
	{
		LOGE("Asset Manager is not initialized!!!!\n");
		return;
	}
	mapRenderer->initTextures(am);
#else
	mapRenderer->initTextures();
#endif
	request4MapObjectsPreparation (0,0,0 ,(cbThreadFunc_T)this->cbRequest4MapObjectsPreparation);  // initial rendering request !!!!!!
}

//bool Sk_Interface::drawFrame()
//{
//    if(ready_to_draw)
//	{
//		if(draw_state==0)
//		{
//			//LOGI("Enter Graph Render %d\n",draw_state);
//
//			ogl->ogl_clear();
//			ogl->UpdateProj ();
//			mapRenderer->draw->flushCachedGraphics();
//
//			//LOGI("End Graph Render %d\n",draw_state);
//			draw_state=1;
//			
//			
//			
//		}
//		else if(draw_state==1)
//		{
//			
//			LOGI("Enter TEXT Render %d\n",draw_state);
//			if(draggingState==DRAGGING_INPROGRESS)
//			{
//				//if I comment out the following line it does not work on Pantech -> it works on other devices with GPU
//				//mapRenderer->draw->flushCachedGraphics();
//				mapRenderer->drawTextOverCanvas();
//			
//				this->drawCurLoc();
//			
//				//drawOverlays();
//			}
//			ready_to_draw=false;
//			//LOGI("End Text Render=== %d \n",draw_state);
//			draw_state=0;
//			
//			
//		}
//		return true;
//	}
//	return false;
//}
bool Sk_Interface::drawFrame()
{
    if(ready_to_draw)
	{
		
			//LOGI("Enter Graph Render %d\n",draw_state);
		
			ogl->ogl_clear();
			ogl->UpdateProj ();
			mapRenderer->draw->flushCachedGraphics();

		
			if(draggingState!=DRAGGING_INPROGRESS)
			{
				//if I comment out the following line it does not work on Pantech -> it works on other devices with GPU
				//mapRenderer->draw->flushCachedGraphics();
				mapRenderer->draw->drawRoute();
				mapRenderer->drawTextOverCanvas();

				//mapRenderer->drawTextOverCanvas_Test();
			
				this->drawCurLoc();
			   // mapRenderer->testTexture();//drawOverlays();
				
			}
		
			ready_to_draw=false;
			
		
		return true;
	}
	return false;
}
LatLon_T Sk_Interface::getLatLonFromScreenPoint(float x, float y) 
{
	LatLon_T ll;
	
	float _dy=sumY-Y0;
	float _dx=sumX-X0;
	
	float dx = x-_dx - ogl->getWidth()/2;
		float dy = y+_dy - ogl->getHeight()/2;
		float fy = calcDiffTileY(dx, dy);
		float fx = calcDiffTileX(dx, dy);
		ll.latitude =(float) MapUtils::getLatitudeFromTile(ogl->getZoom(), getYTile() + fy);
		ll.longitude =(float) MapUtils::getLongitudeFromTile(ogl->getZoom(), getXTile() + fx);
		return ll;
}
void Sk_Interface::drawOverlays()
{
	//-----------------Test Rect
	/*       
	float tx=ogl->rp.tx, 
			ty=ogl->rp.ty;
			Sk_Point3D p={0-tx*ogl->getWidth()/2,0+ty*ogl->getHeight()/2,ogl->getZLevel()};
			ogl->roundedRect(p,400,200,40);
			//ogl->setColor(0xffff0000);
			//ogl->draw_rectangle(&p, 200, 200);
			//ogl->drawPoints(&p,1,10,0xffff0000);

			Sk_Point3D p1={400,600,ogl->getZLevel()};
			ogl->drawCircle(p, 40.0, 0xffff0000);


			Sk_Point3D p2={600,600,ogl->getZLevel()};
			ogl->draw_image(&p2, texture);
			*/
	int len=overlays.size();
	for(int i=0;i<len;i++)
	{
		Sk_Overlay *o=overlays[i];
		switch(o->type)
		{
		case OVERLAY_CIRCLE:
			{
				LOGI("Draw OVERLAY_CIRCLE.. \n");
				Sk_Point3D p;
				p.fX=ogl->getMapXForPoint(o->ll.longitude);
				p.fY=ogl->getMapYForPoint(o->ll.latitude);
				p.fZ=ogl->getZLevel();				
				ogl->drawCircle(&p, 40.0, 0xffff0000);
			}
			break;
		case OVERLAY_TEXTURE:
			{
				/*LOGI("Draw OVERLAY_TEXTURE.. \n");
				float tx=ogl->rp.tx, 
				ty=ogl->rp.ty;
				Sk_Point3D p={o->centerPoint.fX-tx*ogl->getWidth()/2,o->centerPoint.fY+ty*ogl->getHeight()/2,ogl->getZLevel()};
				ogl->draw_image(&p, (Sk_Texture *)o->ptr);*/
			}
			break;
		}
	}
}

void Sk_Interface::initDragSumXY()
{
	sumX=X0;sumY=Y0;
}

int Sk_Interface::checkBounds(int w,int h)
{
	float _dy=sumY-Y0;
		float _dx=sumX-X0;
		if(abs(_dx)>1.0||abs(_dy)>1.0)
		{
			float xp=(float) (_dx*w/2.0);
			float yp=(float) (_dy*h/2.0);
						
			float fy = calcDiffTileY(xp,yp);
			float fx = calcDiffTileX(xp, yp);
			int z=ogl->getZoom();
			float lat =(float) MapUtils::getLatitudeFromTile(z, getYTile() + fy);
			float lon =(float) MapUtils::getLongitudeFromTile(z, getXTile() - fx);
			ogl->updateLocation(lat,lon,z);
			request4MapObjectsPreparation(lat, lon, z,this->cbRequest4MapObjectsPreparation);
			initDragSumXY();//sumX=X0;sumY=Y0;
			return 1;
		}
		return 0;
}
#if 0
void Sk_Interface::dragging(float dx,float dy,int flag)
{
		   int w=ogl->getWidth();
		   int h=ogl->getHeight();
		   
#if SK_BUILD_FOR_ANDROID
		   sumX+=-(2.0*dx/w);sumY+=(2.0*dy/h);
#else
		   sumX+=(2.0*dx/w);sumY+=-(2.0*dy/h);
#endif
		  if(!isRequest4MapObjectsPreparationInProcess())
		  {
			   if(!checkBounds( w, h))
			   {
					drag(sumX,sumY,1);
			   }
		  }
		  else
		  {
			  //the rendering is in process at this point -> keep moving
			  drag(sumX,sumY,1);
		  }
		 
		
			   
		 

		  if(flag==1)draggingState=DRAGGING_INPROGRESS;
		  else draggingState=DRAGGING_END;
}
#else
void Sk_Interface::dragging(float dx,float dy,int flag)
{
		   int w=ogl->getWidth();
		   int h=ogl->getHeight();
		   
#if SK_BUILD_FOR_ANDROID
		   sumX+=-(2.0*dx/w);sumY+=(2.0*dy/h);
#else
		   sumX+=(2.0*dx/w);sumY+=-(2.0*dy/h);
#endif
		 
		  if(flag==1)
		  {
			  draggingState=DRAGGING_INPROGRESS;
			  drag(sumX,sumY,1);
		  }
		  else
		  { 
			  checkBounds( w, h);
			  draggingState=DRAGGING_END;
		  }

		   
}

#endif
void Sk_Interface::keyUpDown(float x,float y,int key)
{
	if(key==1)// down
	{
	}
	else if(key==2) // Up
	{
		if(draggingState==DRAGGING_INPROGRESS)
		{
			dragging(0,0,0);
		}
	}
}
void Sk_Interface::drag (float dx,float dy,int flag)
{
	
	ogl->rp.tx=dx;
	ogl->rp.ty=dy;
	ogl->rp.drag_changed=true;
		
	signalReDraw();
}

void Sk_Interface::signalReDraw()
{
	//LOGI("Sk_Interface::signalReDraw()... \n");
	if(lock_redraw)
		return;
	ready_to_draw=true;
	renderer->postSemaphore();
}

void Sk_Interface::setRouteOption(float x,float y,int v)
{
		
	switch(v)
	{
	case 1:
		{
			Sk_Overlay *ovl=new Sk_Overlay();
			ovl->ll=getLatLonFromScreenPoint( x,  y);
			ovl->type=OVERLAY_CIRCLE;  // set as destination
			overlays.push_back(ovl);
			signalReDraw();
		}
		break;
	case 2:
		{
			//double lon1=-79.36448335647583,lat1=43.84325592844487;   //end point
			LatLon_T ll=getLatLonFromScreenPoint( x,  y);
			LOGI("Route..to  %f  %f \n",ll.latitude,ll.longitude);
			requestRouting (curLat,curLon,ll.latitude,ll.longitude, (cbThreadFunc_T)this->cbRouting);
		}
		break;
	case 3:
		{
			//Sk_Overlay *ovl=new Sk_Overlay();
			//ovl->ll=getLatLonFromScreenPoint( x,  y);
			//ovl->type=OVERLAY_TEXTURE;  
			//ovl->ptr= texture;          // set as favorite
			//overlays.push_back(ovl);
			//signalReDraw();
		}
		
		
		break;
	}

	
}

//-------Routing Callbacks---------
	void Sk_Interface::cbRouting()
	{
		if(gInf)
		{
			//gInf->signalReDraw();
			LOGI("Sk_Interface::CB Routing... \n");
			gInf->setRoutingInProcess(false);
			gInf->signalReDraw();
		}
	}
//-------RequestRendering Callbacks--------- this is called from the thread
	void Sk_Interface::cbRequest4MapObjectsPreparation()
	{
		if(gInf)
		{
		    LOGI("cbRequest4MapObjectsPreparation...%d \n",gInf->ready_to_draw?1:0);

			// at this point we have map objects ready to be drawn
			// switch the Sk_Draw::drawObjs in Sk_Draw.cpp
			// MAKE SURE to synchronize it with the rendering process
			//1. wait until another thread finishes the rendering
			while(gInf->ready_to_draw) {}
			gInf->lock_redraw=true; // lock redraw
		
			gInf->mapRenderer->draw->setObject4Rendering();

			gInf->lock_redraw=false;   // unlock redraw
			gInf->signalReDraw();
			
			// now we can clean up the map objects which are not needed !!!
			gInf->mapRenderer->draw->ClearCachedGraphics();
		}
	}
	
//---------------------------------------

void Sk_Interface::drawCurLoc()
{
	/*if(!this->curLocReceived)
		return;
	this->curLocReceived=false;*/
	Sk_Point3D p;
	p.fX=ogl->getMapXForPoint(curLon);
	p.fY=ogl->getMapYForPoint(curLat);
	p.fZ=0;
	ogl->drawPoints(&p,1,10,0xffff0000);//ogl->drawCircle(p,40,0xffff0000);
}

void Sk_Interface::locationChanged (float lat,float lon, int _speed,int _cog)
{
	
	this->curLat=lat;
	this->curLon=lon;
	this->speed=_speed;
	this->cog=_cog;
	this->curLocReceived=true;

	//this->drawCurLoc();
	
	//signalReDraw();
}
void Sk_Interface::setCurrentLocation()
{
	// check if we are in the close proximity to this place -> in this case we do not need requestRendering
	request4MapObjectsPreparation(this->curLat, this->curLon, ogl->getZoom(),this->cbRequest4MapObjectsPreparation);
}
void Sk_Interface::setZoom(bool plus)
{
	int _zoom=ogl->getZoom();
	if(plus)
	{
		_zoom++; 
		if(_zoom>19) _zoom=19;
		if(_zoom!=prev_zoom)
	   {
		   prev_zoom=_zoom;
		   ogl->setZoom(_zoom);
		   request4MapObjectsPreparation(ogl->getLat(), ogl->getLon(), _zoom,this->cbRequest4MapObjectsPreparation);
	   }
	}
	else
	{
		_zoom--; 
		if(_zoom<1) _zoom=1;
		if(_zoom!=prev_zoom)
	   {
		   prev_zoom=_zoom;
		   ogl->setZoom(_zoom);
		   _sk_log("Zoom z=%d \r\n", _zoom);
		   request4MapObjectsPreparation(ogl->getLat(), ogl->getLon(), _zoom,this->cbRequest4MapObjectsPreparation);
	   }
	}
}
//=================Search Address==============================================
int Sk_Interface::requestSearchAddress(string s)
{
	
	if(!gl_initialized || isSearchAddressInProcess())
		return -1;
	SearchAddressThreadRequest_T *arg=new SearchAddressThreadRequest_T;
		
		arg->address_request=s;
		
		
		{
			pthread_t thread = {0};
			if(pthread_create(&thread, NULL, threadSearchAddressProc, arg) != 0)
			{
				LOGE("ERROR SearchAddressThreadproc... \n");
				delete arg;
				return -1;
			}
		}
		LOGE("in SearchAddressThread... \n");
		setSearchAddressInProcess(true);  // flag must be cleared in Callback function !!!!!!!!!!!!!!!!!
	return 1;
	
	
	
	
}


void* Sk_Interface::threadSearchAddressProc(void* _arg)
{
     SearchAddressThreadRequest_T * arg = (SearchAddressThreadRequest_T*) _arg;
	 if(arg==0)
		 return 0;
	 
	
	gInf->mapRenderer->runSearchAddress(arg->address_request);

	 LOGE("Finish threadsetSearchAddressProc... \n");
	
	delete arg;
	gInf->setSearchAddressInProcess(false);
    return 0;
}
//=================== Rendering Request Threading==============================
int Sk_Interface::doRequestRendering (float lat,float lon,int zoom )
{

	LOGI("requestRendering .. lat=%f lon=%f zoom=%d\n",lat,lon,zoom); 
	rr_mutex->Lock();
	if(lat!=0 && lon!=0 && zoom!=0)
		ogl->updateLocation(lat,lon,zoom);
	
	mapRenderer->runPrepareMapObjects();
	rr_mutex->Unlock();
	//ready_to_draw=true;
	LOGE("requestRendering .. Done\n"); 

	return 1;
}

int Sk_Interface::request4MapObjectsPreparation (float lat,float lon,int zoom , cbThreadFunc_T _cb )
{
	if(!gl_initialized || isRequest4MapObjectsPreparationInProcess())
		return -1;
	RenderingThreadRequest_T *arg=new RenderingThreadRequest_T;
		
		arg->lat=lat;
		arg->lon=lon;
		arg->zoom=zoom;
		arg->cb=_cb;
		
		//createRenderingThread
		{
			pthread_t thread = {0};
			if(pthread_create(&thread, NULL, threadRRProc, arg) != 0)
			{
				LOGE("ERROR threadProc... \n");
				delete arg;
				return -1;
			}
		}

		setRequest4MapObjectsPreparationInProcess(true);  // flag must be cleared in Callback function !!!!!!!!!!!!!!!!!
	return 1;
}

void* Sk_Interface::threadRRProc(void* _arg)
{
     RenderingThreadRequest_T * arg = (RenderingThreadRequest_T*) _arg;
	 if(arg==0)
		 return 0;
	 
	 gInf->doRequestRendering(arg->lat,arg->lon,arg->zoom);
	
	 LOGE("Finish threadRRProc... \n");
	 if(arg->cb!=NULL)
		 arg->cb();
	delete arg;
	gInf->setRequest4MapObjectsPreparationInProcess(false);
    return 0;
}


//=================== Routing Request Threading==============================


int Sk_Interface::requestRouting (float lat1,float lon1,float lat2,float lon2 , cbThreadFunc_T _cb)
{
	if(!gl_initialized || isRoutingInProcess())
		return -1;
	RoutingThreadRequest_T *arg=new RoutingThreadRequest_T;
		
		arg->lat1=lat1;
		arg->lon1=lon1;
		arg->lat2=lat2;
		arg->lon2=lon2;
		
		arg->cb=_cb;
		
		//createRenderingThread
		{
			pthread_t thread = {0};
			if(pthread_create(&thread, NULL, threadRoutingProc, arg) != 0)
			{
				LOGE("ERROR threadRoutingProc... \n");
				delete arg;
				return -1;
			}
		}
		LOGE("in requestRouting... \n");
		setRoutingInProcess(true);  // flag must be cleared in Callback function !!!!!!!!!!!!!!!!!
	return 1;
}

void do_Route(float lat,float lon,float lat1,float lon1);
void* Sk_Interface::threadRoutingProc(void* _arg)
{
     RoutingThreadRequest_T * arg = (RoutingThreadRequest_T*) _arg;
	 if(arg==0)
		 return 0;
	 
	 do_Route(arg->lat1,arg->lon1,arg->lat2,arg->lon2);
	
	 LOGE("Finish threadRoutingProc... \n");
	 if(arg->cb!=NULL)
		 arg->cb();
	delete arg;

    return 0;
}

#pragma endregion


#pragma region java Interface



#if !SK_BUILD_FOR_ANDROID

void setLatLon(float y, float x);
void Init_Intf(EGLDisplay display, EGLSurface surface, EGLContext context,int width,int height)
{
	initialize_Inf();

	EglRenderer *r = gInf->getEglRenderer();
	r->setCtx(display,surface,context,width,height);
	if(gInf!=NULL)
	{
		gInf->initialize_ogl(width,height);
	
		/*gInf->setZoom(INIT_ZOOM);
		gInf->setLatLon(INIT_LAT,INIT_LON);*/
	}

//	r->start();
}

void intfRenderLoop()
{
	EglRenderer *r = gInf->getEglRenderer();
	r->intfRenderLoop();
}



#endif //SK_BUILD_FOR_ANDROID



