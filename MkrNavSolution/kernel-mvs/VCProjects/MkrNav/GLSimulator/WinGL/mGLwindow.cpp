
//
#include <Windows.h>

#include <stdlib.h>

#include "esUtil.h"
#include "interface.h"

extern Sk_Interface *gInf; 

void ShutDown ( ESContext *esContext );




//static bool drawn=false;



void updatePixels(ESContext *esContext)
{
	
}

void commit_draw(ESContext *esContext)
{	
 //  eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );
}




void Draw ( ESContext *esContext )
{

__exit:
   Sleep(1000);

}
void intfRenderLoop();
void _Draw ( ESContext *esContext )
{

//	if(drawn) 
//		goto __exit;
//	
//	if(step()==0) 
//		goto __exit;
//	if(step()==0) 
//		goto __exit;
//		
//	commit_draw(esContext);
//
//  // drawn=true;
//
//__exit:
//   Sleep(100);

	intfRenderLoop();

}

static int _x00=0,_y00=0;
void dragging(int x,int y,int flag)
{
		  /* int w=esContext->width;
		   int h=esContext->height;*/

		   float dx=(1.0*(x-_x00));
		   float dy=(1.0*(y-_y00));
		   
		  if(gInf!=NULL)
			  gInf->dragging(dx,dy,flag);

		   _x00=x;_y00=y;

}
void mouseEventHandler ( ESContext *esContext,int x,int y,int flag )
{
   static int state=0;
   
   if(flag==kMouseDblClick_State)  // left button double click
   {
		if(gInf!=NULL)
			gInf->setZoom(true);
	   return;
   }
   else if(flag==kMouseRightDblClick_State)  // right button double click
   {
	   if(gInf!=NULL)
			gInf->requestSearchAddress("180 lauderdale drive,vaughan,on" );//gInf->setZoom(false);	
	   return;
   }
   else if(flag==kMouseWheel_State)
   {
   //    int d=y;
	  //
	  // if(d>0)
	  // {
		 //   
			//zoom++; 
			//if(zoom>19) zoom=19;
			//
	  // }
	  // else
	  // {
		 //  zoom--; 
		 //  if(zoom<5) zoom=5;
	  // }
	  //
	  // //pos.zoomChanged=true;
	  // if(zoom!=pos.zoom)
	  // {
		 //  pos.zoom=zoom;
		 //  rRender(latitude,longitude,zoom);
	  // }
   }

   switch(state)
   {
   case 0:
	   if(flag==kMouseDown_State)
	   {
		   _x00=x;_y00=y;
		   /*pos.updated=false;
		   pos.moved=false;*/
		   state=1;
	   }
	   break;
   case 1:
	   if(flag==kMouseMoved_State)
	   {
		   /*pos.moved=true;*/
		   state=2;
	   }
	   break;
	case 2:
		 if(flag==kMouseMoved_State)
	   {
		   dragging( x, y,1);
	   }
	   else
	   if(flag==kMouseUp_State)
	   {
		  /* if(pos.moved)
			pos.updated=true;*/
		   state=0;
		  dragging( x, y,2);
	   }

	   break;
   }
}



void periodicUpdate(ESContext *esContext,float x)
{
	
	_Draw ( esContext );	
	Sleep(10);

}

void kbHandler(ESContext *esContext,unsigned char key,int x,int y)
{
	
	if(key=='i')
	{
		//rRender(43.854954695691774,-79.51239109039306,15);
		dragging(x, y, 2);
	}
	else
	if(key=='p')
	{
		if (gInf != NULL)
			gInf->setZoom(true);
		
	}
	if(key=='m')
	{
		if (gInf != NULL)
			gInf->setZoom(false);
	}
	
}



extern const char *test__argv__[];
void test_Route();

void Init_Intf(EGLDisplay display, EGLSurface surface, EGLContext context,int width,int height);
void main_init ( )
{
   ESContext esContext;
   
   int h=480,w=640;//1097;  //2048,1024
  // int w=1600,h=1000;

 /* gRenderer = new CMapRenderer ();
  gRenderer->initFromCL(9,(char **)test__argv__);

  gRenderer->getInfo()->updateSize(w,h);*/

 

   esInitContext ( &esContext );
   esCreateWindow ( &esContext, TEXT("MkrNav"), w,h, ES_WINDOW_RGB );
  /* esContext.graphicCtx =Init_gl_program (w,h); 
   if ( esContext.graphicCtx==0)
      return ;*/
   Init_Intf(esContext.eglDisplay,esContext.eglSurface,esContext.eglContext,w,h);
  
 // rRender(43.854954695691774,-79.51239109039306,16);

 //  esRegisterDrawFunc ( &esContext, (drawFunc_T) Draw );
   esRegisterMouseFunc ( &esContext, (mouseFunc_T) mouseEventHandler);
   esRegisterUpdateFunc( &esContext,(updateFunc_T) periodicUpdate);
   esRegisterKeyFunc ( &esContext,(keyFunc_T)kbHandler );

//   test_Route();

   esMainLoop ( &esContext );

   ShutDown ( &esContext );
   
 //  delete gRenderer;

   exit(0);
   return ;
}


///
// Cleanup
//
void ShutDown ( ESContext *esContext )
{
   //UserData *userData = (UserData *)esContext->userData;

   //// Delete texture object
   //glDeleteTextures ( 1, &userData->textureId );

   //// Delete program object
   //glDeleteProgram ( userData->programObject );
}