#ifndef _SK_ATLAS_H
#define _SK_ATLAS_H

#if SK_BUILD_FOR_ANDROID
#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#else
//#include <QtOpenGL>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif


#include <ft2build.h>
#include <freetype/ftglyph.h>

#include "Sk_Types.h"
#define uint8_t unsigned char

struct _at_point_ {
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat s;
	GLfloat t;
	
};

// Maximum texture width
#define MAXWIDTH 1024

struct CBitmap_S{
		float ax;	// advance.x
		float ay;	// advance.y

		float bw;	// bitmap.width;
		float bh;	// bitmap.height;

		float bl;	// bitmap_left;
		float bt;	// bitmap_top;

		float tx;	// x offset of glyph in texture coordinates
		float ty;	// y offset of glyph in texture coordinates
	};



/**
 * The atlas class holds a texture that contains the visible US-ASCII characters
 * of a certain font rendered with a certain character height.
 * It also contains an array that contains all the information necessary to
 * generate the appropriate vertex and texture coordinates for each character.
 *
 * After the constructor is run, you don't need to use any FreeType functions anymore.
 */
struct Sk_Atlas
{


	GLuint tex;		// texture object

	int w;			// width of texture in pixels
	int h;			// height of texture in pixels

	struct CBitmap_S c[128];		// character information
	
	Sk_Atlas(FT_Face face, int height,void *gc);
    ~Sk_Atlas();
};

#endif  //_SK_ATLAS_H