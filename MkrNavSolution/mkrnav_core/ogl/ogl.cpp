#include <stdlib.h>
#include <math.h>

#include "glCtx.h"
//#if !SK_BUILD_FOR_ANDROID
//#include "../common/esUtil.h"
//#endif

#include "MapUtils.h"
#include <vector>



#define USE_FLOAT	1
#define USE_OPENGLES	1
#define USE_OPENGLES2	1
#define REQUIRES_POWER_OF_2 0

#if USE_FLOAT
#define glF(x)  x
#define glD(x)  x
#define GL_F    GL_FLOAT
typedef GLfloat GLf;
#endif

#if !SK_BUILD_FOR_ANDROID
inline float round2(float x) { return (x-floor(x))>0.5 ? ceil(x) : floor(x); }
#define _roundf_	round2
#else
#define _roundf_	roundf
#endif

//global vars


#pragma region class Sk_Ogl

Sk_Ogl::Sk_Ogl()
	{
		linewidth=2;
		zLevel=-0.01;
		
		//resetCtx();

		gl_initialized=false;
			// not expect any shadow
		shadowLevelMin=0;
		shadowLevelMax=0;
		density=1.0;
		shadowRenderingColor=0xff969696;
		rp.init();
		LOGI("Sk_Ogl::Sk_Ogl() .. tx=%f ty=%f\n",rp.tx,rp.ty);
	}

//void Sk_Ogl::setCtx(EGLDisplay display, EGLSurface surface, EGLContext context,int w,int h)
//{
//	m_display = display;
//    m_surface = surface;
//    m_context = context;
//	
//	screenWidth=w;
//	screenHeight=h;
//	LOG_INFO("Sk_Ogl::setCtx  d:%x c:%x ",m_display,m_surface);
//}
//void Sk_Ogl::destroyCtx()
//{
//	LOGE("Sk_Ogl::destroyCtx  d:%d c:%d ",m_display,m_surface);
//	eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
//    eglDestroyContext(m_display, m_context);
//    eglDestroySurface(m_display, m_surface);
//    eglTerminate(m_display);
// 
//	resetCtx();
//}
//void Sk_Ogl::_swapBuffers()
//{
//	LOG_INFO("swapBuffers()  d:%x c:%x ",m_display,m_surface);
//	eglSwapBuffers(m_display,m_surface);
//}
GLuint  Sk_Ogl::loadShader ( GLenum type, const char *shaderSrc )
{
   GLuint shader;
   GLint compiled;
   
   // Create the shader object
   shader = glCreateShader ( type );

   if ( shader == 0 )
   	return 0;

   // Load the shader source
   glShaderSource ( shader, 1, &shaderSrc, NULL );
   
   // Compile the shader
   glCompileShader ( shader );

   // Check the compile status
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

   if ( !compiled ) 
   {
      GLint infoLen = 0;

      glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );
      
      if ( infoLen > 1 )
      {
         char* infoLog =(char*) malloc (sizeof(char) * infoLen );

         glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
         LOGE ( "Error compiling shader:\n%s\n", infoLog );            
         
         free ( infoLog );
      }

      glDeleteShader ( shader );
      return 0;
   }

   return shader;

}

GLuint Sk_Ogl::loadProgram ( const char *vertShaderSrc, const char *fragShaderSrc )
{
   GLuint vertexShader;
   GLuint fragmentShader;
   GLuint programObject;
   GLint linked;

   // Load the vertex/fragment shaders
   vertexShader = loadShader ( GL_VERTEX_SHADER, vertShaderSrc );
   if ( vertexShader == 0 )
      return 0;

   fragmentShader = loadShader ( GL_FRAGMENT_SHADER, fragShaderSrc );
   if ( fragmentShader == 0 )
   {
      glDeleteShader( vertexShader );
      return 0;
   }

   // Create the program object
   programObject = glCreateProgram ( );
   
   if ( programObject == 0 )
      return 0;

   glAttachShader ( programObject, vertexShader );
   glAttachShader ( programObject, fragmentShader );

   // Link the program
   glLinkProgram ( programObject );

   // Check the link status
   glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

   if ( !linked ) 
   {
      GLint infoLen = 0;

      glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );
      
      if ( infoLen > 1 )
      {
         char* infoLog = (char*)malloc (sizeof(char) * infoLen );

         glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
         LOGE ( "Error linking program:\n%s\n", infoLog );            
         
         free ( infoLog );
      }

      glDeleteProgram ( programObject );
      return 0;
   }

   // Free up no longer needed shader resources
   glDeleteShader ( vertexShader );
   glDeleteShader ( fragmentShader );

   return programObject;
}

///
// Initialize the shader and program object
//http://zach.in.tu-clausthal.de/teaching/cg_literatur/glsl_tutorial/

//u_koeff is used to normalize vertex data to screen coordiates
//const char vertex_Shader [] =
//"                                        \
//   attribute vec2        a_position;       \
//   attribute vec2        a_texture_position;       \
//   uniform mat4        u_mvpMatrix;             \
//   uniform vec2        u_koeff;             \
//   varying vec2 v_texture_position; \
//   void main()                           \
//   {                                     \
//      v_texture_position=a_texture_position; \
//	  vec2 pos=a_position;    \
//	  pos.x=pos.x*u_koeff.x-1.0;  \
//	  pos.y=-pos.y*u_koeff.y+1.0;  \
//	  gl_Position = u_mvpMatrix * vec4(pos,0.0,1.0);   \
//   }                                     \
//";
//GLbyte fragment_Shader[] =                                       //Fragment Shader 
//      "precision mediump float;                            \n"
//	  "uniform vec4   avcolor;                             \n"
//      "varying vec2 v_texture_position;                            \n"
//      "uniform sampler2D s_texture;                        \n"
//	  "uniform int use_texture;                        \n"
//      "void main()                                         \n"
//      "{                                                   \n"
//      " if (use_texture==1) {  \n"
//	  "  gl_FragColor = texture2D( s_texture, v_texture_position );\n"
//	  " } else {  \n"
//	  "  gl_FragColor =  avcolor; \n"
//	  " } \n"
//	  "}                                                   \n";

const char vertex_Shader [] =
"                                        \
   attribute vec3        a_position;       \
   attribute vec2        a_texture_position;       \
   uniform mat4        u_mvpMatrix;             \
   uniform vec2        u_koeff;             \
   uniform float uPointSize;                \
   varying vec2 v_texture_position; \
   void main()                           \
   {                                     \
      v_texture_position=a_texture_position; \
	  gl_PointSize = uPointSize;             \
	  vec3 pos=a_position;    \
	  pos.x=pos.x*u_koeff.x-1.0;  \
	  pos.y=-pos.y*u_koeff.y+1.0;  \
	  gl_Position = u_mvpMatrix * vec4(pos,1.0);   \
   }                                     \
";

/*
GLbyte fragment_Shader[] =                                       //Fragment Shader 
      "precision mediump float;                            \n"
	  "uniform vec4   avcolor;                             \n"
      "varying vec2 v_texture_position;                            \n"
      "uniform sampler2D s_texture;                        \n"
	  "uniform int use_texture;                        \n"
      "void main()                                         \n"
      "{                                                   \n"
        "  gl_FragColor = vec4(1, 1, 1, texture2D(s_texture, v_texture_position).a) * avcolor;\n"
	  "}                                                   \n";
*/

GLbyte fragment_Shader[] =                                       //Fragment Shader 
      "precision mediump float;                            \n"
	  "uniform vec4   avcolor;                             \n"
      "varying vec2 v_texture_position;                            \n"
      "uniform sampler2D s_texture;                        \n"
	  "uniform int use_texture;                        \n"
      "void main()                                         \n"
      "{                                                   \n"
		" if (use_texture==1) {  \n"
	  "  gl_FragColor = texture2D( s_texture, v_texture_position );\n"
	  " } else {  \n"
	  "  gl_FragColor = vec4(1, 1, 1, texture2D(s_texture, v_texture_position).a) * avcolor;\n"
	  " } \n"
	  "}                                                   \n";

int Sk_Ogl::init (int _screenWidth,int _screenHeight  )
{
	

   // Load the shaders and get a linked program object
   programObject = loadProgram ((char*) vertex_Shader,(char*) fragment_Shader );
   glUseProgram ( programObject );
   // Get the attribute locations
   position_location = glGetAttribLocation ( programObject, "a_position" );
   glEnableVertexAttribArray(position_location);
   texture_position_location = glGetAttribLocation ( programObject, "a_texture_position" );
   screen_koeff=glGetUniformLocation(programObject, "u_koeff");
   color_location=glGetUniformLocation(programObject, "avcolor");
   point_size=glGetUniformLocation(programObject, "uPointSize");
   mvp_location=glGetUniformLocation(programObject, "u_mvpMatrix");
  
   
   // Get the sampler location
   texture_location = glGetUniformLocation ( programObject, "s_texture" );
   use_texture_location=glGetUniformLocation(programObject, "use_texture");

   glUniform1i(use_texture_location, 0);
   glUniform1i(texture_location, 0);

   screenWidth=_screenWidth;
   screenHeight=_screenHeight;
   /*kW=2.0/screenWidth;
   kH=2.0/screenHeight;*/
   float vec2[2]={(float)(2.0/screenWidth),(float)(2.0/screenHeight)};
   glUniform2fv( screen_koeff, 1, (GLfloat*) vec2 );

   esMatrixLoadIdentity( &mvpMatrix ); // make mat4[1]
   
   //http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/#The_Model__View_and_Projection_matrices
   /*
   OpenGL uses 4 x 4 matrix for transformations. 
   Notice that 16 elements in the matrix are stored as 1D array in column-major order. 
   IMPORTANT!!!!!!!!!!
   You need to transpose this matrix if you want to convert it to the standard convention, row-major format. 
   */
   //mvpMatrix.m[0][0]=kW;
   //mvpMatrix.m[1][1]=-kH;
   ///*mvpMatrix.m[0][3]=-1.0;
   //mvpMatrix.m[1][3]=1.0;*/
   //mvpMatrix.m[3][0]=-1;
   //mvpMatrix.m[3][1]=1;
   //
     
  	/*   
	GLuint textures;
		glGenTextures(1, &textures);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   	*/	

	
   
   /* glHint( GL_GENERATE_MIPMAP_HINT, GL_NICEST );
	glHint( GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES, GL_NICEST );*/
	
	/*glEnable( GL_DEPTH_TEST );
	glDepthMask( GL_TRUE );*/

	//glEnable( GL_CULL_FACE  );
	////glDisable( GL_DITHER );
	
	//glDepthFunc( GL_LESS );
	//glDepthRangef( 0.0f, 1.0f );
	//glClearDepthf( 1.0f );
	////glCullFace ( GL_BACK );
	////glFrontFace( GL_CCW  );
	////glClearStencil( 0 );
	////glStencilMask( 0xFFFFFFFF );
	//
	glClearColor( 0xf1/255.0, 0xea/255.0, 0xe4/255.0, 1.0f );
	
	glClear( GL_DEPTH_BUFFER_BIT | /*GL_STENCIL_BUFFER_BIT |*/ GL_COLOR_BUFFER_BIT );


    //glClearColor ( 0.0f, 0.0f, 1.0f, 0.0f );
	
	
	
	glViewport(0,0,_screenWidth,_screenHeight); // do it once
	

	//http://www.khronos.org/registry/egl/sdk/docs/man/xhtml/eglMakeCurrent.html
	

	return 1;
}
void Sk_Ogl::enableText(int d)
{
	glUniform1i(use_texture_location, d);
}
void Sk_Ogl::printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}


void Sk_Ogl::checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}


void Sk_Ogl::ogl_clear()
{
	glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
	glClearColor( 0xf1/255.0, 0xea/255.0, 0xe4/255.0, 1.0f );//glClearColor (0.3 , 0.3f, 0.3f, 1.0f );
}

//http://stackoverflow.com/questions/7785354/opengl-es-2-0-why-does-this-perspective-projection-matrix-not-give-the-right-re?rq=1
void Sk_Ogl::UpdateProj ()
{	
   if(this->rp.useProjection)
   {
	   ESMatrix perspective;
	   ESMatrix modelview;
	   //ESMatrix cameraview;
	   float    aspect;
	   static float angle = 40.0;
	   if(this->rp.deltaAngle!=0.0)
		angle += ( rp.deltaAngle * 40.0f );
   

	   // Compute the window aspect ratio
	   aspect = (GLfloat) this->screenWidth / (GLfloat) this->screenHeight;
   
	   // Generate a perspective matrix with a 60 degree FOV
	   esMatrixLoadIdentity( &perspective );
	   esPerspective( &perspective, 60.0f, aspect, 0.1, 500.0 );

	   esMatrixLoadIdentity( &modelview );

	   // Translate away from the viewer
	   esTranslate( &modelview, 0.0,0.4, -2.0 );

		esRotate( &modelview, angle, 1.0, 0.0, 0.0 );  // around x 
   
	//	esRotate( &modelview, 90, 0.0, 0.0, 1.0 );     // around x
   
	   // Compute the final MVP by multiplying the 
	   // modevleiw and perspective matrices together
	   esMatrixMultiply( &mvpMatrix, &modelview, &perspective );
   }
   else
   {
	    esMatrixLoadIdentity( &mvpMatrix );
   }

   //if(rp.scale!=0.0)
	   esScale(&mvpMatrix,this->rp.scale,this->rp.scale,1.0);
  // if(rp.drag_changed)
	esTranslate( &mvpMatrix, this->rp.tx,this->rp.ty, this->rp.tz );
	//LOGI("Sk_Ogl::UpdateProj .. tx=%f ty=%f\n",this->rp.tx,this->rp.ty);
   glUniformMatrix4fv( mvp_location, 1, GL_FALSE, (GLfloat*) &mvpMatrix.m[0][0] );
}


#pragma endregion

#pragma region OpenGL Common Primitives


void Sk_Ogl::drawPoints(Sk_Point3D *p, int count,float pointSize,Sk_Color color)
{
	glUniform1f(point_size, pointSize);
	setColor(color);
	glEnableVertexAttribArray(position_location);
	draw_array(p,count,GL_POINTS);
}
void Sk_Ogl::set_color(M_COLOR color)
{
	glUniform4fv(color_location, 1, color.color/*col*/);

}
void Sk_Ogl::setColor( Sk_Color color)
{
		
	GLfloat col[4];
	col[0]=((color>>16)&0xff)*Sk_Clr_Norm;
	col[1]=((color>>8)&0xff)*Sk_Clr_Norm;
	col[2]=((color>>0)&0xff)*Sk_Clr_Norm;
	col[3]=1.0;
	glUniform4fv(color_location, 1, col);

}
void Sk_Ogl::draw_array(Sk_Point3D *p, int count, GLenum mode)
{
	glVertexAttribPointer (position_location, 3/*2*/, GL_FLOAT, false, 0, p );
    glDrawArrays(mode, 0, count);
}

void Sk_Ogl::draw_rectangle_do( Sk_Point3D  *p, int w, int h)
{
	Sk_Point3D pa[4];
	pa[0]=pa[1]=pa[2]=pa[3]=*p; // top left
	pa[1].fX+=w;                // top right
	pa[2].fY+=h;                // bottom left
	pa[3].fX+=w;
	pa[3].fY+=h;                // bottom right
	draw_array( pa, 4, GL_TRIANGLE_STRIP);
}


void Sk_Ogl::draw_rectangle(Sk_Point3D *p, int w, int h)
{
	glEnableVertexAttribArray(position_location);
	draw_rectangle_do( p, w, h);
}
void Sk_Ogl::drawRectLine(Sk_Point3D *p,int count ,int color,int linewidth)
{
	
	setColor(color);
	glEnableVertexAttribArray(position_location);

	draw_array( p, count, GL_LINE_LOOP);
}
void Sk_Ogl::roundedRect(Sk_Point3D &p,int width,int height,int radius)
{
	int resolution=48;
	float step = ( 2.0f * M_PI ) / resolution,
      angle = 0.0f,
      x_offset,
      y_offset;

int i = 0;

unsigned int index = 0,
             segment_count = ( int )( resolution / 4 );

Sk_Point3D *top_left             = ( Sk_Point3D * ) malloc( segment_count * sizeof( Sk_Point3D ) ), 
         *bottom_left         = ( Sk_Point3D * ) malloc( segment_count * sizeof( Sk_Point3D ) ),
         *top_right             = ( Sk_Point3D * ) malloc( segment_count * sizeof( Sk_Point3D ) ),
         *bottom_right         = ( Sk_Point3D * ) malloc( segment_count * sizeof( Sk_Point3D ) ),
          bottom_left_corner = { p.fX + radius,
                                 p.fY + height - radius }; 

while( i != segment_count )
{
    x_offset = cosf( angle );
    y_offset = sinf( angle );


    top_left[ index ].fX = bottom_left_corner.fX - 
                          ( x_offset * radius );
    top_left[ index ].fY = ( height - ( radius * 2.0f ) ) + 
                            bottom_left_corner.fY - 
                          ( y_offset * radius );


    top_right[ index ].fX = ( width - ( radius * 2.0f ) ) + 
                             bottom_left_corner.fX + 
                           ( x_offset * radius );
    top_right[ index ].fY = ( height - ( radius * 2.0f ) ) + 
                             bottom_left_corner.fY -
                           ( y_offset * radius );


    bottom_right[ index ].fX = ( width - ( radius * 2.0f ) ) +
                                bottom_left_corner.fX + 
                              ( x_offset * radius );
    bottom_right[ index ].fY = bottom_left_corner.fY + 
                              ( y_offset * radius );


    bottom_left[ index ].fX = bottom_left_corner.fX - 
                             ( x_offset * radius );
    bottom_left[ index ].fY = bottom_left_corner.fY +
                             ( y_offset * radius );


    top_left[ index ].fX = _roundf_( top_left[ index ].fX );
    top_left[ index ].fY = _roundf_( top_left[ index ].fY );


    top_right[ index ].fX = _roundf_( top_right[ index ].fX );
    top_right[ index ].fY = _roundf_( top_right[ index ].fY );


    bottom_right[ index ].fX = _roundf_( bottom_right[ index ].fX );
    bottom_right[ index ].fY = _roundf_( bottom_right[ index ].fY );


    bottom_left[ index ].fX = _roundf_( bottom_left[ index ].fX );
    bottom_left[ index ].fY = _roundf_( bottom_left[ index ].fY );

    angle -= step;

    ++index;

    ++i;
}

glEnableVertexAttribArray(position_location);

setColor(0xff0000ff);

//segment_count*2 + 2 +4+segment_count*2
int length=segment_count*2 + 2 +4+segment_count*2;
Sk_Point3D *v=new Sk_Point3D[length];
index=0;
//memset(v,0,sizeof(Sk_Point3D)*length);
for(int k=0;k<length;k++)
{
	v[k].fZ=getZLevel();
}
//glBegin( GL_TRIANGLE_STRIP );
{
    // Top
    {
        i = 0;
        while( i != segment_count )
        {
           
            v[index].fX= top_left[ i ].fX,v[index].fY=top_left[ i ].fY;
			index++;
			v[index].fX= top_right[ i ].fX,v[index].fY=top_right[ i ].fY;
            index++;
           
            ++i;
        }
    }


    // In order to stop and restart the strip.
    v[index].fX= top_right[ 0 ].fX,v[index].fY=top_right[ 0 ].fY;
    index++;
    v[index].fX= top_right[ 0 ].fX,v[index].fY=top_right[ 0 ].fY;
    index++;
    

    // Center
    {
        
       v[index].fX= top_right[ 0 ].fX,v[index].fY=top_right[ 0 ].fY;
		index++;

       
        v[index].fX= top_left[ 0 ].fX,v[index].fY=top_left[ 0 ].fY;
			index++;


       
         v[index].fX= bottom_right[ 0 ].fX,v[index].fY=bottom_right[ 0 ].fY;
		index++;


       
         v[index].fX= bottom_left[ 0 ].fX,v[index].fY=bottom_left[ 0 ].fY;
		index++;
    }


    // Bottom
    i = 0;
    while( i != segment_count )
    {
        
        v[index].fX= bottom_right[ i ].fX,v[index].fY=bottom_right[ i ].fY;
		index++;  

       
         v[index].fX= bottom_left[ i ].fX,v[index].fY=bottom_left[ i ].fY;
		index++;                                  

        ++i;
    }    
}
//glEnd();
draw_array(v, index, GL_TRIANGLE_STRIP);


//glBegin( GL_LINE_STRIP );

setColor(0xffff0000);  //red
index=0;
// Border
{
    i = ( segment_count - 1 );
    while( i > -1 )
    {    
         v[index].fX= top_left[ i ].fX,v[index].fY=top_left[ i ].fY;
			index++;

        --i;
    }


    i = 0;
    while( i != segment_count )
    {    
         v[index].fX= bottom_left[ i ].fX,v[index].fY=bottom_left[ i ].fY;
			index++;

        ++i;
    }


    i = ( segment_count - 1 );
    while( i > -1 )
    {    
        v[index].fX= bottom_right[ i ].fX,v[index].fY=bottom_right[ i ].fY;
			index++;

        --i;
    }


    i = 0;
    while( i != segment_count )
    {    
        v[index].fX= top_right[ i ].fX,v[index].fY=top_right[ i ].fY;
			index++;

        ++i;
    }


    // Close the border.
	v[index].fX= top_left[ ( segment_count - 1 ) ].fX,v[index].fY=top_left[ ( segment_count - 1 ) ].fY;
			index++;
   
}
//glEnd();
#if USE_POLYLINE
this->linewidth=4.0;
draw_polyline(v,index);
#else
glLineWidth(1.0);
draw_array(v, index, GL_LINE_STRIP);
#endif


//glBegin( GL_LINES );
glLineWidth(1.0);
index=0;
setColor(0x00ff00ff);

// Separator
{
    // Top bar
     v[index].fX= top_right[ 0 ].fX,v[index].fY=top_right[ 0 ].fY;
		index++;

     v[index].fX= top_left[ 0 ].fX,v[index].fY=top_left[ 0 ].fY;
		index++;   


    // Bottom bar
     v[index].fX= bottom_left[ 0 ].fX,v[index].fY=bottom_left[ 0 ].fY;
			index++;
       
    v[index].fX= bottom_right[ 0 ].fX,v[index].fY=bottom_right[ 0 ].fY;
	index++;   
}
//glEnd();
draw_array(v, index, GL_LINES);


free( top_left );
free( bottom_left );
free( top_right );
free( bottom_right );
delete[] v;
}

#define GL_POINTS_SIZE     12288
static Sk_Point3D Glpoints[GL_POINTS_SIZE];
//void Sk_Ogl::draw_multiple_lines (int count, int *lines,Sk_Point3D *points) {
void Sk_Ogl::draw_polyline (Sk_Point3D *points,int count_of_points,short fCapStyle) {


   Sk_Point3D glpoints[4];
   Sk_Point3D *gl_array = Glpoints;
   // int count_of_points;
   Sk_Point3D *prev;
 
  
  Sk_Point3D *p=points;
   
   float w=this->linewidth*0.5;        
 
   int pts_count = 0;
   
  // int *first_line = lines;
  // Sk_Point3D *first_point = points;

   
   glpoints[0].fZ = glpoints[1].fZ =
   glpoints[2].fZ = glpoints[3].fZ =this->getZLevel();//Z_LEVEL;
   
      
  /* for (int i = 0; i < count; ++i) {
      bool bezier_available = false;
      count_of_points = *lines;*/

      prev = points;
      points++;
      count_of_points--;

      while (count_of_points) {

        float x = (float)prev->fX;
        float y = (float)prev->fY;
         
         while (x != points->fX ||
                y != points->fY) {

            
            float x1 = (float)prev->fX;
            float y1 = (float)prev->fY;
            x = (float)points->fX;
            y = (float)points->fY;
            
            
  
			float dx=x1-x; // x coord
			float dy=y-y1; // y coord
			float d=1.0/sqrtf(dx*dx+dy*dy);
			dx*=d; // cos
			dy*=d; //sin
			dy*=w;
			dx*=w;
          
            glpoints[0].fX = x1+dy;     glpoints[0].fY = y1+dx;  //top left
			glpoints[1].fX = x1-dy;		glpoints[1].fY = y1-dx;  //bottom left
			glpoints[2].fX = x+dy;      glpoints[2].fY = y+dx; //top right
			glpoints[3].fX = x-dy;      glpoints[3].fY = y-dx; //bottom right

			
            memcpy(gl_array, glpoints, sizeof(*glpoints) * 4);
            pts_count += 4;
            gl_array += 4;

            if ((pts_count + 4) >= GL_POINTS_SIZE) {
               glVertexAttribPointer (this->position_location, 3, GL_FLOAT, false, sizeof(Sk_Point3D), &Glpoints[0].fX );//glVertexPointer(3, GL_FLOAT, sizeof(Sk_Point3D), &Glpoints[0].x);
               glDrawArrays(GL_TRIANGLE_STRIP, 0, pts_count);
               //check_gl_error();
               gl_array = Glpoints;
               pts_count = 0;
            }
         } //while last_drawn
         
         prev = points;
         points++;
         count_of_points--;
      } //while count_of_points
      
   //   lines++;
   //} //for count

   if (pts_count) {
      glVertexAttribPointer (this->position_location, 3, GL_FLOAT, false, sizeof(Sk_Point3D), &Glpoints[0].fX );//glVertexPointer(3, GL_FLOAT, sizeof(Sk_Point3D), &Glpoints[0].x);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, pts_count);
     
	 // 	setColor(0xffff0000);
		////draw_array(arr->path->arr , arr->path->len, GL_LINE_STRIP);
		// glVertexAttribPointer (this->position_location, 3, GL_FLOAT, false, 2*sizeof(Sk_Point3D), &Glpoints[1].fX );//glVertexPointer(3, GL_FLOAT, sizeof(Sk_Point3D), &Glpoints[0].x);
		//glDrawArrays(GL_LINE_STRIP, 0, pts_count/2);

	   if(fCapStyle==2)  // ROUND
	   {
		   /* glUniform1f(point_size, 12);
			setColor(0xffff0000);
			draw_array(prev,1,GL_POINTS);*/
		   //draw_array(p,1,GL_POINTS);
		   drawCircle(p, w, 0,true);
		   drawCircle(prev, w, 0,true);
	   }

   }
}


void Sk_Ogl::drawCircle(const Sk_Point3D *center, float radius, Sk_Color color,bool fill)
{
	const float k_segments = 16.0f;
	const int vertexCount=16;
	const float k_increment = 2.0f * M_PI / k_segments;
	float theta = 0.0f;
	
	float scaleFactor=1.0;//(screenHeight*1.0/screenWidth);//(width*1.0/height);
	Sk_Point3D				glVertices[vertexCount+1];
	for (int i = 0; i < vertexCount; ++i)
	{
		//M_POINT2D v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		glVertices[i].fX=center->fX+radius*cosf(theta);
		glVertices[i].fY=center->fY+radius*sinf(theta)*scaleFactor;
		glVertices[i].fZ=0;
		theta += k_increment;
	}
	glVertices[vertexCount].fX=glVertices[0].fX;
	glVertices[vertexCount].fY=glVertices[0].fY;
	glVertices[vertexCount].fZ=0;
	
	if(color!=0)setColor(color);
	glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, glVertices);
	glEnableVertexAttribArray(position_location);
	if(!fill)
		glDrawArrays(GL_LINE_STRIP, 0, vertexCount+1);
	else glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount+1);
}


//void Sk_Ogl::draw_arrow(Sk_Point3D *p, float dx, float dy, int l)
//{
//	Sk_Point3D pnt[4];
//	
//	pnt[0]=pnt[1]=pnt[2]=*p;
//	float x=p->fX-dx*l;
//	float y=p->fY-dy*l;
//		
//	int w=l>>1;
//	
//	
//	//if(dy<0&&dx>0)
//	{	
//		pnt[0].fX=x+w*dy;   // do rotation here  dy-sin dx-cos  (?? or vice versa)
//		pnt[0].fY=y-w*dx;
//		pnt[2].fX=x-w*dy;
//		pnt[2].fY=y+w*dx;
//	}
//	
//	glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, pnt);
//	glDrawArrays(GL_TRIANGLES, 0, 3);
//
//	l=l<<1;
//
//	// do triangle strip to draw polygon
//	w=w>>1;
//
//	pnt[2].fX=x+w*dy;   //v3
//	pnt[2].fY=y-w*dx;
//	pnt[3].fX=x-w*dy;  //v3
//	pnt[3].fY=y+w*dx;
//	pnt[3].fZ=0;
//
//	x=p->fX-dx*l;
//	y=p->fY-dy*l;
//	
//	pnt[0].fX=x+w*dy;   // v1
//	pnt[0].fY=y-w*dx;
//	pnt[1].fX=x-w*dy;  // v2
//	pnt[1].fY=y+w*dx;
//
//	glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, pnt);
//	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//
//}

void Sk_Ogl::draw_arrow(Sk_Point3D *p, float dx, float dy, int l)
{
	Sk_Point3D v[7];
	memset(v,0,7*sizeof(Sk_Point3D));
	v[0]=*p;
	
	float x=p->fX-dx*l;
	float y=p->fY-dy*l;
		
	int w=l>>1;
	
	
	//if(dy<0&&dx>0)
	{	
		v[1].fX=x+w*dy;   // do rotation here  dy-sin dx-cos  (?? or vice versa)
		v[1].fY=y-w*dx;
		v[6].fX=x-w*dy;
		v[6].fY=y+w*dx;
	}
	
	
	l=l<<1;

	// do triangle strip to draw polygon
	w=w>>1;

	v[2].fX=x+w*dy;  
	v[2].fY=y-w*dx;
	v[5].fX=x-w*dy;  
	v[5].fY=y+w*dx;
	

	x=p->fX-dx*l;
	y=p->fY-dy*l;
	
	v[3].fX=x+w*dy;   // v1
	v[3].fY=y-w*dx;
	v[4].fX=x-w*dy;  // v2
	v[4].fY=y+w*dx;

	glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, v);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 7);

}

#pragma endregion



#pragma region OGL Image
void Sk_Ogl::draw_image(Sk_Point3D *p, Sk_Texture *t)
{
	Sk_Point  x[4]; //texture coord

	x[0].fX=0;x[0].fY=1.0;  // top left
	x[1].fX=1.0;x[1].fY=1.0;  // top right
	x[2].fX=0.0;x[2].fY=0.0; // bottom left
	x[3].fX=1.0;x[3].fY=0.0; // bottom right
	

	glUniform1i(use_texture_location, 1);
	
	
	t->Bind();
	
	glUniform1i(texture_location, 0);

	glEnableVertexAttribArray(position_location);
	glEnableVertexAttribArray(texture_position_location);
	glVertexAttribPointer (texture_position_location, 2, GL_FLOAT, 0, 0, x );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	TextureInfo *info=t->getTextureInfo();
	draw_rectangle_do( p, info->width, info->height);
	
	
	t->Unbind();
	glDisableVertexAttribArray(texture_position_location);

	glDisable(GL_BLEND);

	glUniform1i(use_texture_location, 0);
}

//********************************rendering info***********************************
void Sk_Ogl::updateRenderingInfoFromCL(float lat,float lon,int zoom,float tileWX,float tileHY)
{
	this->cLat=lat;
	this->cLon=lon;
	setZoom(zoom);
	this->tileWX=tileWX;
	this->tileHY=tileHY;

	updateLocation();
}
void Sk_Ogl::setZoom(int z) {
		this->zoom = z;
		this->tileDivisor = (1 << (31 - z));
	}
void Sk_Ogl::updateLocation()
	{
		if(screenWidth==0||screenHeight==0)
			return;
		float tileX=(float) getTileNumberX(zoom, cLon);
		float tileY=(float) getTileNumberY(zoom, cLat);
		
		float x0=tileX-(1.0*screenWidth)/TILE_SIZE;
		float y0=tileY-(1.0*screenHeight)/TILE_SIZE;
		
		//float x1=tileX+width/TILE_SIZE;
		//float y1=tileY+height/TILE_SIZE;
		Sk_float lattop, lonleft;
		lonleft=getLongitudeFromTile(zoom, x0);
		lattop=	getLatitudeFromTile(zoom, y0);

		bounds.fLeft = get31TileNumberX(lonleft);
		bounds.fTop = get31TileNumberY(lattop);
		
				
		tileWX=/*ceilf*/(2.0*screenWidth/TILE_SIZE) ;
		tileHY=/*ceilf*/(2.0*screenHeight/TILE_SIZE);

		

		bounds.fRight = bounds.fLeft + tileWX * (1 << (31 - zoom));
		bounds.fBottom = bounds.fTop + tileHY * (1 << (31 - zoom));
		
		
		rp.tx=SHIFT_SCENE_LEFT;//shift map scene to the left for a half a screen area (screenWidth/2.0)
		rp.ty=SHIFT_SCENE_TOP; //shift map scene to the top for a half a screen area (screenHeight/2.0)
		//rp.tx=0.0;//(screenWidth/2.0)
		//rp.ty=0.0;


		

		//LOGI("Sk_Ogl::updateLocation .. tx=%f ty=%f\n",rp.tx,rp.ty);
	}

void Sk_Ogl::updateScreenSize(int w,int h)
	{
		screenWidth=w;
		screenHeight=h;
		updateLocation();
	}
	void Sk_Ogl::updateZoom(int z)
	{
		setZoom(z);
		updateLocation();
	}
	void Sk_Ogl::updateLocation(Sk_Point *p)
	{
		cLon=p->fX;
		cLat=p->fY;
		updateLocation();
	}
	void Sk_Ogl::updateLocation(Sk_float lat,Sk_float lon,int z)
	{
		cLon=lon;
		cLat=lat;
		updateZoom(z);
	}
		
	void Sk_Ogl::setXYLocation(/*double leftX, double topY*/) 
	{
		/*this->leftX = leftX;
		this->topY = topY;*/
		this->leftX =getLeft()/getPowZoom(31-zoom);
		this->topY =getTop()/getPowZoom(31-zoom);
	}
	//-----------------------------------------------------------------
	float Sk_Ogl::getXTile() {
		return (float) getTileNumberX(zoom, cLon);
	}

	 float Sk_Ogl::getYTile() {
		return (float) getTileNumberY(zoom, cLat);
	}
	int Sk_Ogl::getMapXForPoint(Sk_float cLon) {
		int cx = screenWidth;//getCenterPointX()
		Sk_float tileX = getTileNumberX(zoom, cLon);
		return (int) ((tileX - getXTile()) * TILE_SIZE + cx);
	}

	int Sk_Ogl::getMapYForPoint(Sk_float cLat) {
		int cy = screenHeight;//getCenterPointY()
		Sk_float tileY = getTileNumberY(zoom, cLat);
		return (int) ((tileY - getYTile()) * TILE_SIZE + cy);
	}
	//-----------------------------------------------------------------
//********************************************************************************
#pragma endregion




