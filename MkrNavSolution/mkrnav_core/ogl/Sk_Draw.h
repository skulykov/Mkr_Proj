#ifndef _SK_DRAW_H
#define _SK_DRAW_H

#include <math.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <set>
#include <time.h>

#include "common.h"
#include "Sk_Paint.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

class DrawT
{
	Sk_Ogl *gc;
	Sk_Atlas * a;
	const char *text;
	int tLen;
	_at_point_ *coords;
	int c;
	float sy;
	float zLevel;
	int angle;
    float sinAngle,cosAngle;
	float X0,Y0;             // origin rotation point
	//float xOffset,yOffset;  //===========================================
public:
	void finalDraw();
	void setCoords(_at_point_*p,GLfloat x,GLfloat y,GLfloat s,GLfloat t);
	void __setCoords(_at_point_*p,GLfloat x,GLfloat y,GLfloat s,GLfloat t);
	void copyCoords(_at_point_*dst,_at_point_*src);
	void setAngleVal(float _sin,float _cos);
	float setAngleVal(float x2,float y2,float x1,float y1);
	void setAngleVal(Sk_Point3D *p,int index);
	void setAngle(int _angle);
	void setZLevel(float v){zLevel=v;}
	void render_text_line(float x, float y);
	void render_text_path(/*Sk_Path *path,float openSpace*/TextDrawInfo *text);
public:
	DrawT(Sk_Ogl *gc,const char *text );
	//static void drawOnPath(Sk_Ogl *gc,Sk_Point *p,const char  *text, Sk_Path *path);
	static void drawOnPath(Sk_Ogl *gc,TextDrawInfo *text, Sk_Paint *paint);
	static void drawText(Sk_Ogl *gc,const char *text,int len,int x,int y,Sk_Paint *paint);
};


//================================Cached GLES2 Content=============================
struct CachedGLArrayTri_T
{
	//bool /*drawBoundaries,*/needTriangulate;  // used for polygon
	bool tri_done;
	Sk_Path *path;
	//Sk_Path *tri_path;   // to keep triangulated verticies
	//float linewidth;
	Sk_Color color;   // color to fill polygon
	//Sk_Color bcolor;   // color to draw plygon boundaries
#if USE_VBO_FOR_DRAWING
	int offset;   // offset 
#endif
	CachedGLArrayTri_T()
	{
		//needTriangulate=false;
		tri_done=false;
		//tri_path=NULL;
		//bcolor=-1;
	}
	~CachedGLArrayTri_T()
	{
		//since CachedGLArray_T class controls the path->ref counter
		//we need to make sure that path->ref==0  !!!!!!!!!!!!!!!!!
		if(path!=NULL&&--path->ref==0)
			delete path;
		/*if(tri_path!=NULL)
			delete tri_path;*/
	}
};
struct CachedGLArrayLines_T
{
	bool oneway;
	short fCapStyle;
	Sk_Path *path;
	float linewidth;
	Sk_Color color;
#if USE_VBO_FOR_DRAWING
	int offset;   // offset 
#endif
	CachedGLArrayLines_T()
	{
		oneway=false;
	}
	~CachedGLArrayLines_T()
	{
		//since CachedGLArray_T class controls the path->ref counter
		//we need to make sure that path->ref==0  !!!!!!!!!!!!!!!!!
		if(path!=NULL&&--path->ref==0)
			delete path;
	}
};

struct CachedObjects_T
{
	short state;         //0 - init, 1 - prepared, 2 - rendering
	bool vboInitialized;
	int totalLines,totalTri;

	GLuint    vbo_buffers[2];
	
	std::vector<CachedGLArrayLines_T*> cachedGLArray_Lines;
	std::vector<CachedGLArrayLines_T*> cachedGLArray_ShadowLines;
	std::vector<CachedGLArrayTri_T*> cachedGLArray_Tri;

	void prepare()
	{
		totalLines=0;
		totalTri=0;
		vbo_buffers[0]=0;
		vbo_buffers[1]=0;
		vboInitialized=false;
		
	};
};

class Sk_Draw
{
	enum DrawMode{
		MODE_DRAW_LINES=1,
		MODE_DRAW_TRIANGLES=2,
		MODE_DRAW_POLYGON=3
	};
private:
	/*std::vector<CachedGLArrayLines_T*> cachedGLArray_Lines;
	std::vector<CachedGLArrayLines_T*> cachedGLArray_ShadowLines;
	std::vector<CachedGLArrayTri_T*> cachedGLArray_Tri;*/
	CachedObjects_T c_objs[2];
	CachedObjects_T *drawObjs;  // point to the map objects which are currently being drawn
	CachedObjects_T *prepObjs;  // point to the map objects which are currently being prepared
	Sk_Ogl *ogl;
#if USE_VBO_FOR_DRAWING
	
	/*bool vboInitialized;
	int totalLines,totalTri;*/
#else
	//std::vector<CachedGLArray_T*> cachedGLArray;
	
#endif
	Sk_Path *triangulate(Sk_Point3D *p, int count);
	void flushCachedLines(std::vector<CachedGLArrayLines_T*> &v);
	void flushCachedPolygons();
	void flushCachedOverlayLines();
public:
#if USE_VBO_FOR_DRAWING
	
	/*GLuint    vbo_buffers[2];*///vboLine,vboTri;
	Sk_Draw(Sk_Ogl *ogl)
	{
		this->ogl=ogl;
		c_objs[0].state=0;
		c_objs[1].state=0;
	}
	/*void prepare()
	{
		totalLines=0;
		totalTri=0;
		vbo_buffers[0]=0;
		vbo_buffers[1]=0;
		vboInitialized=false;
		
	};*/
	CachedObjects_T *getObj4Prepare()
	{
	    CachedObjects_T *rv=0;	
		for (int i=0;i<2;i++)
		{
			LOGI("getObj4Prepare.. %d %d\n",i,c_objs[i].state);
			if(c_objs[i].state==0)
			{
				rv=&c_objs[i];
				break;
			}
		}
		prepObjs=rv;
		return rv;
	}
	void setObject4Rendering()
	{
		int i;
		bool found=false;
		for (i=0;i<2;i++)
		{
			if(c_objs[i].state==1)
			{
				found=true;
				break;
			}
		}
		if(found)
		{
			int j=-1;
			if(i==0)
			{
				j=1;
				
			}else if(i==1)
			{
				j=0;
			}
			drawObjs=&c_objs[i];
			drawObjs->state=2;
			prepObjs=&c_objs[j];

			LOGI("setObject4Rendering() %d %d  \n",i,j);
		}
		else
		{
			LOGI("COULD NOT FIND setObject4Rendering()   \n");
		}
	}
	void ClearCachedGraphics()
	{
		clearCachedGraphics(prepObjs);
		prepObjs->state=0;
		LOGI("ClearCachedGraphics()... \n");
	}
	bool CreateVertexBuffers(CachedObjects_T *objs);
	void insertTri(Sk_Paint *paint,Sk_Path *path,bool triangulationDone=false);
	void setColor(Sk_Color);
	
	void draw_oneway(Sk_Point3D *pnt, int count);
#endif
	void drawPath(Sk_Path *path,bool oneway,bool shadow=false);
	
	void drawPathPoly(Sk_Path *path);
	void clearCachedGraphics(CachedObjects_T *objs);
	void flushCachedGraphics();
	
	void drawRoute();
};


enum OVERLAY_TYPE{
	OVERLAY_IMAGE=1,
	OVERLAY_FAVOR=2,
	OVERLAY_CIRCLE=3,
	OVERLAY_TEXTURE=4
};
struct Sk_Overlay
{
	
	//Sk_Point3D centerPoint;
	LatLon_T ll;
	int type;
	void *ptr;

};

#endif  //_SK_DRAW_H