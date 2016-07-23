#ifndef GLCTX_H
#define GLCTX_H


#if SK_BUILD_FOR_ANDROID
#include <jni.h>
//#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#else
//#include <QtOpenGL>//#include <GL/gl.h>  //svk gl.h
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif
#include "../core/dbg.h"

#include "Sk_Rect_Path.h"

#include "Sk_Texture.h"

/*
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
*/



/*
#define COLOR_WHITE_ 0xffff,0xffff,0xffff,0xffff
#define COLOR_BLACK_ 0x0000,0x0000,0x0000,0xffff

#define COLOR_BACKGROUND_ 0xffff, 0xefef, 0xb7b7, 0xffff
#define COLOR_TRANSPARENT__ 0x0000,0x0000,0x0000,0xffff
#define COLOR_WHITE ((struct color) {COLOR_WHITE_})
#define COLOR_BLACK ((struct color) {COLOR_BLACK_})
#define COLOR_TRANSPARENT ((struct color) {COLOR_TRANSPARENT_})
#define COLOR_FMT "0x%x,0x%x,0x%x,0x%x"
#define COLOR_ARGS(c) (c).r,(c).g,(c).b,(c).a
#define COLOR_IS_SAME(c1,c2) ((c1).r==(c2).r && (c1).g==(c2).g && (c1).b==(c2).b && (c1).a==(c2).a)
#define COLOR_IS_WHITE(c) COLOR_IS_SAME(c, COLOR_WHITE)
#define COLOR_IS_BLACK(c) COLOR_IS_SAME(c, COLOR_BLACK)
*/
#define COLOR_IS_WHITE(c) ((c).r==0xffff && (c).g==0xffff && (c).b==0xffff && (c).a==0xffff)
#define COLOR_IS_BLACK(c) ((c).r==0x0 && (c).g==0x0 && (c).b==0x0 && (c).a==0xffff)

typedef struct
{
    GLfloat   m[4][4];
} ESMatrix;
#pragma region OpenGL ES Navit Initialization

#define SHIFT_SCENE_LEFT	-1.0
#define SHIFT_SCENE_TOP	     1.0

struct RenderParams
{
	float deltaAngle;
	int zoom_changed;
	int prev_zoom;
	float scale;
	GLfloat tx, ty,  tz;
	bool drag_changed;
	bool  useProjection;
	/*RenderParams()
	{
		tx=0.0, ty=0.0,  tz=0.0;
		scale=1.0;
		prev_zoom=16;
		zoom_changed=0;
		deltaAngle=0;
		drag_changed=false;
		useProjection=false;
	}*/
	void init() {
		tx=SHIFT_SCENE_LEFT, ty=SHIFT_SCENE_TOP,  tz=0.0;
		/*tx=0.0, ty=0.0,  tz=0.0;*/
		scale=1.0;
		prev_zoom=16;
		zoom_changed=0;
		deltaAngle=0;
		drag_changed=false;
		useProjection=false;
	}
};

typedef struct graphics_gc_priv  graphics_gc_priv_t;     // graphic context


#pragma endregion

void esMatrixLoadIdentity(ESMatrix *result);
void esMatrixMultiply(ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB);
void esScale(ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz);
void esTranslate(ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz);
void esRotate(ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void  esPerspective(ESMatrix *result, float fovy, float aspect, float nearZ, float farZ);
void esOrtho(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ);
void esFrustum(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ);

//-------------------------------------------------------------------
//-------------------------------------------------------------------
/*
void set_color( float r,float g,float b);
void draw_array( Sk_Point3D *p, int count, GLenum mode);
void draw_rectangle_do( Sk_Point3D  *p, int w, int h);

void draw_image_es(struct graphics_priv *gr, Sk_Point *p, int w, int h, unsigned char *data);

void draw_text(struct graphics_priv *gr, struct graphics_gc_priv *fg,
	  struct graphics_gc_priv *bg, struct font_freetype_font *font,
	  char *text, struct point *p, int dx, int dy);

void label_line(struct graphics_priv *gr, struct graphics_gc_priv *fg, struct graphics_gc_priv *bg, struct font_freetype_font *font, Sk_Path *path, char *pStr);

*/

class Sk_Ogl
{
private:

	/*EGLDisplay m_display;
    EGLSurface m_surface;
    EGLContext m_context;*/


	int screenWidth,screenHeight;
	
	//******************** rendering context *************************************
	float tileWX, tileHY;
	
	float leftX;  //lon left
	float topY;   // lat top
	Sk_float /*double*/ cLat,cLon;     // should be a center of the map ???  left top coner!!!!
	

	int zoom;

	Sk_Rect bounds;  // rectangle of the drawing area
	
	float density;
	int shadowRenderingMode,shadowRenderingColor;
	//***************************************************************************
	
	M_COLOR fg;//float fr, fg, fb, fa;
	//float br, bg, bb, ba;
	
	float zLevel;                   // zLevel of OGL ES drawings

	bool gl_initialized;

	GLuint  loadShader ( GLenum type, const char *shaderSrc );
	GLuint loadProgram ( const char *vertShaderSrc, const char *fragShaderSrc );
public:
	float linewidth;
	void *paint;                     // pointer to Sk_Paint
	void *atlas;                     // pointer to font atlas

	GLuint programObject;
	GLint screen_koeff,mvp_location, position_location, color_location, texture_position_location, use_texture_location, texture_location;
	GLint point_size;
	ESMatrix  mvpMatrix;

	RenderParams rp;

	Sk_Ogl();
	
	int getHeight(){return screenHeight;}
	int getWidth(){return screenWidth;}
	float getZLevel(){return zLevel;}
	int init(int screenWidth,int screenHeight  );

	void printGLString(const char *name, GLenum s);
	void checkGlError(const char* op);
	void ogl_clear();
	
	void UpdateProj ();
	void enableText(int d);

	//primitives
	void set_color(M_COLOR color);
	void setColor(Sk_Color color);
	void draw_rectangle(Sk_Point3D *p, int w, int h);
	void draw_rectangle_do( Sk_Point3D  *p, int w, int h);
	void draw_array(Sk_Point3D *p, int count, GLenum mode);
	void drawCircle(const Sk_Point3D *center, float radius, Sk_Color color,bool fill=true);
	void drawPoints(Sk_Point3D *p, int count,float pointSize,Sk_Color color);
	void draw_arrow(Sk_Point3D *p, float dx, float dy, int l);

	void drawRectLine(Sk_Point3D *p,int count,int color,int linewidth);
	void roundedRect(Sk_Point3D &p,int width,int height,int radius);
	
	void draw_polyline (Sk_Point3D *points,int count_of_points,short fCapStyle=0);

	void draw_image(Sk_Point3D *p, Sk_Texture *t);

	//*********************rendering context***************************
	float tileDivisor;
		// not expect any shadow
	int shadowLevelMin;
	int shadowLevelMax;

	int lastRenderedKey;

	float getLeft(){return bounds.fLeft;}
	float getRight(){return bounds.fRight;}
	float getTop(){return bounds.fTop;}
	float getBottom(){return bounds.fBottom;}
	int getZoom(){return zoom;}
	void setZoom(int z);
	float getLat(){return cLat;}
	float getLon(){return cLon;}
	void updateRenderingInfoFromCL(float lat,float lon,int zoom,float tileWX,float tileHY);
	void updateLocation();
	
	void updateScreenSize(int w,int h);
	void updateZoom(int z);
	void updateLocation(Sk_Point *p);
	void updateLocation(Sk_float lat,Sk_float lon,int _zoom);

	void setShadowRenderingMode(int mode){this->shadowRenderingMode = mode;	}
	int getShadowRenderingMode(){return this->shadowRenderingMode;	}
	void setShadowRenderingColor(int c){this->shadowRenderingColor = c;	}
	int getShadowRenderingColor(){return this->shadowRenderingColor;	}
	
	float getDensityValue(float v){return v*density;}

	void setXYLocation(/*double leftX, double topY*/) ;
	
	float getLeftX(){return this->leftX;}
	float getTopY(){return this->topY;}

	//---------------------------------------------
	float getXTile();
	float getYTile();
	int getMapXForPoint(Sk_float cLon);
	int getMapYForPoint(Sk_float cLat);
	//---------------------------------------------
	//*****************************************************************
};


#endif //#ifndef GLCTX_H



