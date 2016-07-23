
#include "Sk_Draw.h"
#include "glCtx.h"
#include "poly.h"
#define USE_ROADMAP_RENDERING  1
#if USE_ROADMAP_RENDERING

#pragma region RoadMap drawing

//#define Z_LEVEL                  -6
#define MAX_AA_SIZE              32

//#define BEZIER_LINES

//////////////////////////////////////////
// Anti Alias
// minimal implementation, based on glAArg code by arekkusu

__inline float frsqrtes_nr(float x) {
#ifdef OPTIMIZE_SQRT
   volatile float fX = x;
   volatile float *pfX = &fX;
   volatile int   *piX = NULL;

   float xhalf = 0.5f * fX;

   piX = ( volatile int* ) pfX;				// store floating-point bits in integer
   *piX = 0x5f3759d5 - ( *piX >> 1 );       // initial guess for Newton's method
   pfX = ( volatile float* ) piX;				// convert new bits into float
   fX = *pfX;
   fX = fX * ( 1.5f - xhalf*fX*fX );       // One round of Newton's method

   return fX;

#else

	return 1.0f/sqrtf(x);
#endif //OPTIMIZE_SQRT
}


// some convienence constants for the texture dimensions:
// point size, size minus one, half size squared, double size, point center, radius squared
//#define psz (phf * 2)
//#define psm (psz - 1)
//#define phs (phf * phf)
//#define pdb (psz * 2)
//#define pct (psz + phf)
//#define prs ((phf-1)*(phf-1))


#define roadmap_log

typedef struct BezierCurve_tag {
   Sk_Point3D p1;
   Sk_Point3D p2;
} BezierCurve;
/* Multiple lines buffer */


#ifdef BEZIER_LINES
#define OGL_SMOOTH_FACTOR     1.0f  // higher is smoother
#define OGL_LENGTH_FACTOR     0.2f  //was 0.4f
#define OGL_BEZIER_RES        4     // additional points per line (between two existing points)
#define OGL_BEZIER_POS_START  0
#define OGL_BEZIER_POS_MID    1
#define OGL_BEZIER_POS_END    2
#endif //BEZIER_LINES


#ifdef BEZIER_LINES
static void calc_bezier_points (int count, Sk_Point3D *points, BezierCurve *curve) {
   int i;

#define _MIL(_a, _b) ((_a+_b)/2)
#define _SYM(_a, _b, _f) (_a-((_b-_a)*_f))
   
   curve[0].p1.fX = points[0].fX + (points[1].fX - points[0].fX)*0.33;
   curve[0].p1.fY = points[0].fY + (points[1].fY - points[0].fY)*0.33;
   
   for (i = 1; i < count -1; i++) {
      //if (roadmap_math_screen_distance(&points[i], &points[i+1], 0) < 10)
      //   continue;
      
      //control points
      curve[i-1].p2.fX = _MIL(_MIL(points[i].fX, points[i-1].fX), _MIL(_SYM(points[i].fX, points[i+1].fX, OGL_SMOOTH_FACTOR), points[i].fX));
      curve[i-1].p2.fY = _MIL(_MIL(points[i].fY, points[i-1].fY), _MIL(_SYM(points[i].fY, points[i+1].fY, OGL_SMOOTH_FACTOR), points[i].fY));
      if ((points[i].fX - points[i-1].fX) && (float)(points[i].fX - curve[i-1].p2.fX)/(points[i].fX - points[i-1].fX) > 0.2 ||
          (points[i].fY - points[i-1].fY) && (float)(points[i].fY - curve[i-1].p2.fY)/(points[i].fY - points[i-1].fY) > 0.2) {
         curve[i-1].p2.fX = points[i].fX - (points[i].fX - curve[i-1].p2.fX) / 2;
         curve[i-1].p2.fY = points[i].fY - (points[i].fY - curve[i-1].p2.fY) / 2;
      }
      curve[i].p1.fX = _MIL(_MIL(points[i].fX, points[i+1].fX), _MIL(_SYM(points[i].fX, points[i-1].fX, OGL_SMOOTH_FACTOR), points[i].fX));
      curve[i].p1.fY = _MIL(_MIL(points[i].fY, points[i+1].fY), _MIL(_SYM(points[i].fY, points[i-1].fY, OGL_SMOOTH_FACTOR), points[i].fY));
      if ((points[i].fX - points[i+1].fX) && (float)(points[i].fX - curve[i].p1.fX)/(points[i].fX - points[i+1].fX) > 0.2 ||
          (points[i].fY - points[i+1].fY) && (float)(points[i].fY - curve[i].p1.fY)/(points[i].fY - points[i+1].fY) > 0.2) {
         curve[i].p1.fX = points[i].fX - (points[i].fX - curve[i].p1.fX) / 2;
         curve[i].p1.fY = points[i].fY - (points[i].fY - curve[i].p1.fY) / 2;
      }
   }
   
   curve[i-1].p2.fX = points[i].fX + (points[i-1].fX - points[i].fX)*0.33;
   curve[i-1].p2.fY = points[i].fY + (points[i-1].fY - points[i].fY)*0.33;
   
   printf("===\n");
}

static void calc_bezier_ctrl (Sk_Point3D *p, BezierCurve *curve, int pos) {
#define _MIL(_a, _b) ((_a+_b)/2)
#define _SYM(_a, _b, _f) (_a-((_b-_a)*_f))
   int i;
   Sk_Point3D *points;
   
   if (pos == OGL_BEZIER_POS_START) {
      i = 0;
      points = p;
   } else {
      i = 1;
      points = p -1;
   }

   
   if (pos == OGL_BEZIER_POS_START) {
      curve->p1.fX = points[0].fX + (points[1].fX - points[0].fX)*0.33;
      curve->p1.fY = points[0].fY + (points[1].fY - points[0].fY)*0.33;
   } else {
      float factor = OGL_LENGTH_FACTOR;
      float l;
      
      curve->p1.fX = _MIL(_MIL(points[i].fX, points[i+1].fX), _MIL(_SYM(points[i].fX, points[i-1].fX, OGL_SMOOTH_FACTOR), points[i].fX));
      curve->p1.fY = _MIL(_MIL(points[i].fY, points[i+1].fY), _MIL(_SYM(points[i].fY, points[i-1].fY, OGL_SMOOTH_FACTOR), points[i].fY));

      l = (points[i].fX - curve->p1.fX)*(points[i].fX - curve->p1.fX) + (points[i].fY - curve->p1.fY)*(points[i].fY - curve->p1.fY);
      factor = l / ((points[i].fX - points[i+1].fX)*(points[i].fX - points[i+1].fX) + (points[i].fY - points[i+1].fY)*(points[i].fY - points[i+1].fY));
      
      if (factor > OGL_LENGTH_FACTOR) {
         curve->p1.fX = points[i].fX - (points[i].fX - curve->p1.fX) * OGL_LENGTH_FACTOR / factor;
         curve->p1.fY = points[i].fY - (points[i].fY - curve->p1.fY) * OGL_LENGTH_FACTOR / factor;
      }
   }
   
   if (pos == OGL_BEZIER_POS_END) {
      curve->p2.fX = points[2].fX + (points[1].fX - points[2].fX)*0.33;
      curve->p2.fY = points[2].fY + (points[1].fY - points[2].fY)*0.33;
   } else {
      float factor = OGL_LENGTH_FACTOR;
      float l;
      
      i++;
      curve->p2.fX = _MIL(_MIL(points[i].fX, points[i-1].fX), _MIL(_SYM(points[i].fX, points[i+1].fX, OGL_SMOOTH_FACTOR), points[i].fX));
      curve->p2.fY = _MIL(_MIL(points[i].fY, points[i-1].fY), _MIL(_SYM(points[i].fY, points[i+1].fY, OGL_SMOOTH_FACTOR), points[i].fY));

      l = (points[i].fX - curve->p2.fX)*(points[i].fX - curve->p2.fX) + (points[i].fY - curve->p2.fY)*(points[i].fY - curve->p2.fY);
      factor = l / ((points[i].fX - points[i-1].fX)*(points[i].fX - points[i-1].fX) + (points[i].fY - points[i-1].fY)*(points[i].fY - points[i-1].fY));
      
      if (factor > OGL_LENGTH_FACTOR) {
         curve->p2.fX = points[i].fX - (points[i].fX - curve->p2.fX) * OGL_LENGTH_FACTOR / factor;
         curve->p2.fY = points[i].fY - (points[i].fY - curve->p2.fY) * OGL_LENGTH_FACTOR / factor;
      }
   }
}
#endif //BEZIER_LINES


#if 0
void Sk_Draw::sk_draw_multiple_lines (int count, int *lines,Sk_Point3D *points) {


   Sk_Point3D glpoints[10];
   Sk_Point3D *gl_array = Glpoints;
   int phf = MAX_AA_SIZE;
   float r = ogl->linewidth;
   float bord = phf / r;
   int count_of_points;
   Sk_Point3D *prev;
  #ifdef BEZIER_LINES
   BezierCurve bezier[1024];
#endif
   int i;
   float x, y, x1, y1;
   float perp_y, perp_x, perpd, factor;
   float width;         // = (maxr+1.0f)*0.5f;
  // float parl_x, parl_y;

   int pts_count = 0;
   
   int *first_line = lines;
   Sk_Point3D *first_point = points;

   
   glpoints[0].fZ = glpoints[1].fZ =
   glpoints[2].fZ = glpoints[3].fZ =
   glpoints[4].fZ = glpoints[5].fZ =
   glpoints[6].fZ = glpoints[7].fZ =
   glpoints[8].fZ = glpoints[9].fZ = ogl->getZLevel();//Z_LEVEL;
   
   for (i = 0; i < count; ++i) {
      bool bezier_available = false;
      count_of_points = *lines;
#ifdef BEZIER_LINES
      bezier_available  = smooth && (count_of_points > 2); //TODO: maybe also test if points are too close
#endif
      prev = points;
      points++;
      count_of_points--;

      while (count_of_points) {
#ifdef BEZIER_LINES
         float t = 0.0f;
         BezierCurve bezier_control;
         
         if (bezier_available) {
            int bezier_pos = OGL_BEZIER_POS_MID;
            if (count_of_points + 1 == *lines)
               bezier_pos = OGL_BEZIER_POS_START;
            else if (count_of_points == 1)
               bezier_pos = OGL_BEZIER_POS_END;
            calc_bezier_ctrl(prev, &bezier_control, bezier_pos);            
         }
#endif //BEZIER_LINES
         x = (float)prev->fX;
         y = (float)prev->fY;
         
         while (x != points->fX ||
                y != points->fY) {
#ifdef BEZIER_LINES
            if (bezier_available)
            {
               double tm1,tm13,tm3;
               
               x1 = x;
               y1 = y;
               
               while (x == x1 && y == y1 && t < 1) {
                  t += 1.0f/(OGL_BEZIER_RES + 2);
                  
                  if (t > 1.0f) {
                     //roadmap_log(ROADMAP_FATAL, "t too large !!!");
                     t = 1.0f;
                  }
                  
                  tm1 = 1 - t;
                  tm13 = tm1 * tm1 * tm1;
                  tm3 = t * t * t;
                  
                  x = floorf(tm13*prev->fX + 3*t*tm1*tm1*bezier_control.p1.fX + 3*t*t*tm1*bezier_control.p2.fX + tm3*points->fX);
                  y = floorf(tm13*prev->fY + 3*t*tm1*tm1*bezier_control.p1.fY + 3*t*t*tm1*bezier_control.p2.fY + tm3*points->fY);
               }
            }
            else
#endif //BEZIER_LINES
            {
               x1 = (float)prev->fX;
               y1 = (float)prev->fY;
               x = (float)points->fX;
               y = (float)points->fY;
            }
            
            perp_y = x1-x;
            perp_x = y-y1;
            factor = perp_y*perp_y+perp_x*perp_x;
            if (factor) {
               perpd = frsqrtes_nr(factor);
               perp_y *= perpd;           // normalize to 1px
               perp_x *= perpd;
               x1 -= perp_y*0.5f;
               y1 += perp_x*0.5f;
            } else {
               perp_y = 0.0f;
               perp_x = 1.0f;
            }


			width = (r+1.0f)*0.5f;
            perp_y *= width;
            perp_x *= width;
           /* parl_x = -perp_y;
            parl_y =  perp_x;*/
            glpoints[0].fX = x1+perp_x;      glpoints[0].fY = y1+perp_y;  //top left
			glpoints[1].fX = x1-perp_x;		glpoints[1].fY = y1-perp_y;  //bottom left
			glpoints[2].fX = x+perp_x;           glpoints[2].fY = y+perp_y; //top right
			glpoints[3].fX = x-perp_x;           glpoints[3].fY = y-perp_y; //bottom right

			
            memcpy(gl_array, glpoints, sizeof(*glpoints) * 4);
            pts_count += 4;
            gl_array += 4;

            if ((pts_count + 10) >= GL_POINTS_SIZE) {
               glVertexAttribPointer (ogl->position_location, 3, GL_FLOAT, false, sizeof(Sk_Point3D), &Glpoints[0].fX );//glVertexPointer(3, GL_FLOAT, sizeof(Sk_Point3D), &Glpoints[0].x);
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
      
      lines++;
   } //for count

   if (pts_count) {
      glVertexAttribPointer (ogl->position_location, 3, GL_FLOAT, false, sizeof(Sk_Point3D), &Glpoints[0].fX );//glVertexPointer(3, GL_FLOAT, sizeof(Sk_Point3D), &Glpoints[0].x);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, pts_count);
     
   }
}
#endif


#if 0//RM_RENDERING_TEST
void ogl_clear();
void draw_Line();
extern graphics_gc_priv_t *pGraphicCtx;
void DrawTestDepth()
{
	ogl_clear();
	
	UpdateProj (pGraphicCtx,0.0);
	
	Sk_Point3D  p;
	p.fX=200;
	p.fY=200;
	p.fZ=-6.0;

	setColor(pGraphicCtx,0x00ff0000);
	draw_rectangle_do(&p, 200, 200);

	p.fX+=100;
	p.fY+=100;
	p.fZ=-6.01;

	setColor(pGraphicCtx,0x0000ffff);
	draw_rectangle_do(&p, 200, 200);
	
}
void Draw1()
{
   
		ogl_clear();
#if 1
	Sk_Point3D        dynamicVerts[48];

	int lines=24;

	float x=-1.0,dx=2.0/lines;
	for (int i = 0 ; i < lines ; i++) {

		dynamicVerts[i].fX=x*500+500;
		dynamicVerts[i].fY=sin(4*x)*500+500;
		dynamicVerts[i].fZ=0;
		x+=dx;
	}
	
	UpdateProj (pGraphicCtx,0.0);
		
	glLineWidth(1);
	roadmap_canvas_draw_multiple_lines_smooth (1, &lines, dynamicVerts, 1,pGraphicCtx);
#else
		draw_Line();
#endif
}
void test_draw()
{
	Sk_Point3D        dynamicVerts[48];

	int lines=24;

	float x=-1.0,dx=2.0/lines;
	for (int i = 0 ; i < lines ; i++) {

		dynamicVerts[i].fX=x*500+500;
		dynamicVerts[i].fY=sin(4*x)*500+500;
		dynamicVerts[i].fZ=0;
		x+=dx;
	}
	
	UpdateProj (pGraphicCtx,0.0);
		
	roadmap_canvas_draw_multiple_lines_smooth (1, &lines, dynamicVerts, 1,pGraphicCtx);
}
#endif
#pragma endregion

#endif

#pragma region Sk_Draw class

void Sk_Draw::setColor(Sk_Color color)
{
	if(!color) color=((Sk_Paint*)ogl->paint)->getColor();
	ogl->setColor(color);
}
bool Sk_Draw::CreateVertexBuffers(CachedObjects_T *objs)
{
	glGenBuffers(2, objs->vbo_buffers); 
	//checkGlError("glGenBuffers");
	if(objs->vbo_buffers[0]==0||objs->vbo_buffers[1]==0)
	{
		//vbo_buffers[0]=1;
		return false;
	}
	//1 fill lines buffer
    glBindBuffer(GL_ARRAY_BUFFER, objs->vbo_buffers[0]); 
    glBufferData(GL_ARRAY_BUFFER, objs->totalLines, NULL,/*GL_STREAM_DRAW*/ GL_STATIC_DRAW); 
    
	//now it's time to fill data
	
	std::vector<CachedGLArrayLines_T*>::iterator it;
	for(it = objs->cachedGLArray_Lines.begin(); it != objs->cachedGLArray_Lines.end(); ++it)
	{
		
		CachedGLArrayLines_T *arr=(CachedGLArrayLines_T *)(*it);
		//if(arr->mode==MODE_DRAW_LINES)
		{
			glBufferSubData(GL_ARRAY_BUFFER, arr->offset, arr->path->len*sizeof(Sk_Point3D), arr->path->arr);
			
		}
	
	}
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	//2 fill polygons buffer
    glBindBuffer(GL_ARRAY_BUFFER, objs->vbo_buffers[1]); 
    glBufferData(GL_ARRAY_BUFFER, objs->totalTri, NULL,/*GL_STREAM_DRAW*/ GL_STATIC_DRAW); 
    
	//now it's time to fill data
	
	std::vector<CachedGLArrayTri_T*>::iterator it1;
	for(it1 = objs->cachedGLArray_Tri.begin(); it1 != objs->cachedGLArray_Tri.end(); ++it1)
	{
		
		CachedGLArrayTri_T *arr=(CachedGLArrayTri_T *)(*it1);
		//if(arr->mode==MODE_DRAW_LINES)
		{
			glBufferSubData(GL_ARRAY_BUFFER, arr->offset, arr->path->len*sizeof(Sk_Point3D), arr->path->arr);
			
		}
	
	}

	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
    
	return true;
}

Sk_Path *Sk_Draw::triangulate(Sk_Point3D *p, int count)
{
		Sk_Path *path=0;
		TPPLPartition pp;
		std::list<TPPLPoly> result;
		TPPLPoly poly;
		poly.Init(count);
		for (int j=0; j < count; j++)
		{
			poly[j].x=p[j].fX;
			poly[j].y=p[j].fY;
		}
		
		//Sk_Point3D m_poly[3];
		poly.SetOrientation(TPPL_CCW);
		//testpolys.push_back(poly);
		//int rv=pp.Triangulate_EC(&poly,&result);
		int rv=pp.my_Triangulate_EC(&poly,&result);
		int n=result.size();
		//LOGI("pp.Triangulate_EC count=%d rv=%d size=%d\n",count,rv,n);
		if(rv&&n>0)
		{
			path=new Sk_Path(n*3);
			//LOGI("Size of polys %d \n",n);
			list<TPPLPoly>::iterator it;
			 it = result.begin();
			 for(int i=0;i<n;i++)
			 {
				 TPPLPoly ppl=*it;
				 int k=3;//ppl.GetNumPoints();
				 if(k>0)
				 {
					 for(int j=0;j<k;j++)
					 {
						/*m_poly[j].fX=ppl[j].x;
						m_poly[j].fY=ppl[j].y;
						m_poly[j].fZ=0;*/

						 path->arr[i*3+j].fX=ppl[j].x;
						 path->arr[i*3+j].fY=ppl[j].y;
						 path->arr[i*3+j].fZ=0;
					 }
					 
					 //draw_array( &path->arr[i*3]/*m_poly*/, 3, GL_TRIANGLE_FAN);
				 }

				 it++;
			 }
		}
		//LOGI("RETURN pp.Triangulate_EC \n");
		return path;
}

void Sk_Draw::drawPath(Sk_Path *path,bool oneway,bool shadow)
{
	Sk_Paint *paint=(Sk_Paint*)this->ogl->paint;
	CachedGLArrayLines_T *arr = new CachedGLArrayLines_T();
	path->ref++;  // increment Reference to this object
	arr->path=path;
	arr->oneway=oneway;
	arr->fCapStyle=paint->getStrokeCap();
	arr->linewidth=paint->getStrokeWidth();
	arr->color=paint->getColor();
	arr->offset=prepObjs->totalLines;
	prepObjs->totalLines+=path->len*sizeof(Sk_Point3D);      // 3D object
	if(!shadow)
		prepObjs->cachedGLArray_Lines.push_back(arr);
	else
		prepObjs->cachedGLArray_ShadowLines.push_back(arr);
}

void Sk_Draw::insertTri(Sk_Paint *paint,Sk_Path *path,bool triangulationDone)
{
	CachedGLArrayTri_T *arr = new CachedGLArrayTri_T;
	arr->color=paint->getColor();
	path->ref++;  // increment Reference to this object
	arr->path=path;
	arr->tri_done=triangulationDone;
	arr->offset=prepObjs->totalTri;
	prepObjs->totalTri+=arr->path->len*sizeof(Sk_Point3D);
	prepObjs->cachedGLArray_Tri.push_back(arr);
}
void Sk_Draw::drawPathPoly(Sk_Path *path)
{
	
	Sk_Paint *paint=(Sk_Paint*)this->ogl->paint;
	
	if(path->len>3 && path->len < 100)
	{
			Sk_Path *new_path=this->triangulate(path->arr,path->len);
			if(new_path!=NULL)
			{
				insertTri(paint,new_path,true);
				
			}
			else
			{
				// we could not triangulate this polygon
				return ;
			}
	}
	else
	{		
			insertTri(paint,path);
	}

}


void Sk_Draw::clearCachedGraphics(CachedObjects_T *objs)
{
	int len=objs->cachedGLArray_Lines.size();
	if(len>0)
	{
		for(int i=0;i<len;i++)
		{
			CachedGLArrayLines_T *arr=objs->cachedGLArray_Lines[i];
			if(arr!=NULL)
			{	
				delete arr;
			}
		}
		objs->cachedGLArray_Lines.clear();  
	}
	len=objs->cachedGLArray_ShadowLines.size();
	if(len>0)
	{
		for(int i=0;i<len;i++)
		{
			CachedGLArrayLines_T *arr=objs->cachedGLArray_ShadowLines[i];
			if(arr!=NULL)
			{	
				delete arr;
			}
		}
		objs->cachedGLArray_ShadowLines.clear();  
	}
	len=objs->cachedGLArray_Tri.size();
	if(len>0)
	{
		for(int i=0;i<len;i++)
		{
			CachedGLArrayTri_T *arr=objs->cachedGLArray_Tri[i];
			if(arr!=NULL)
			{	
				delete arr;
			}
		}
		objs->cachedGLArray_Tri.clear();  
	}
	//clean up VBOs
	glDeleteBuffers(2, objs->vbo_buffers);
	
}

void Sk_Draw::flushCachedPolygons()
{
	if(!(drawObjs->cachedGLArray_Tri.size()>0))
	{
		//LOGI("====Nothing in flushCachedGraphics====\n");
		return;
	}
	glBindBuffer(GL_ARRAY_BUFFER, drawObjs->vbo_buffers[1]);
	//glEnableVertexAttribArray(gc->position_location); 

	std::vector<CachedGLArrayTri_T*>::iterator it;
	for(it = drawObjs->cachedGLArray_Tri.begin(); it != drawObjs->cachedGLArray_Tri.end(); ++it)
	{
		
		CachedGLArrayTri_T *arr=(CachedGLArrayTri_T *)(*it);
		setColor(arr->color);
		if(arr->tri_done)
		{
			int n=arr->path->len/3;
			int off=arr->offset;
			for(int i=0;i<n;i++)
			{
				glVertexAttribPointer(this->ogl->position_location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(off));
				glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
				off+=3*sizeof(Sk_Point3D);
			}
		}
		else
		{
			glVertexAttribPointer(this->ogl->position_location, 3, GL_FLOAT, GL_FALSE, 0/*sizeof(Sk_Point3D)*/, /*(void*)arr->offset*/BUFFER_OFFSET(arr->offset));
			glDrawArrays(GL_TRIANGLE_FAN, 0, arr->path->len);		
		}
	}	
	
	//glDisableVertexAttribArray(gc->position_location); 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}
void Sk_Draw::flushCachedGraphics()
{
	
	if(!drawObjs->vboInitialized)
	{	
		drawObjs->vboInitialized=CreateVertexBuffers(drawObjs);
	}
	glEnableVertexAttribArray(this->ogl->position_location); 
	
	flushCachedPolygons();
	
	flushCachedLines(drawObjs->cachedGLArray_ShadowLines);  //Shadow Lines First !!!
	flushCachedLines(drawObjs->cachedGLArray_Lines);

	//flushCachedOverlayLines(gc);
	glDisableVertexAttribArray(this->ogl->position_location); 
	
}
void draw_Route(Sk_Ogl *ogl);
void Sk_Draw::drawRoute()
{
	
	draw_Route(this->ogl);
	 
}
#if 0 //USE_VBO_FOR_DRAWING
void Sk_Draw::flushCachedLines(Sk_Ogl *gc)
{
	if(!(cachedGLArray_Lines.size()>0))
	{
		//LOGI("====Nothing in flushCachedGraphics====\n");
		return;
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo_buffers[0]);
	

	std::vector<CachedGLArrayLines_T*>::iterator it;
	for(it = cachedGLArray_Lines.begin(); it != cachedGLArray_Lines.end(); ++it)
	{
		
		CachedGLArrayLines_T *arr=(CachedGLArrayLines_T *)(*it);
		
			setColor(arr->color);
			glLineWidth(arr->linewidth);

			//glBindBuffer(GL_ARRAY_BUFFER, vbo_buffers[0]); 
			glVertexAttribPointer(gc->position_location, 3, GL_FLOAT, GL_FALSE, 0/*sizeof(Sk_Point3D)*/, /*(void*)arr->offset*/BUFFER_OFFSET(arr->offset));
			glDrawArrays(GL_LINE_STRIP, 0, arr->path->len);		
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
#else
void Sk_Draw::flushCachedLines(std::vector<CachedGLArrayLines_T*> &v)
{
	if(!(v.size()>0))
	{
		//LOGI("====Nothing in flushCachedGraphics====\n");
		return;
	}
	
	std::vector<CachedGLArrayLines_T*>::iterator it;
	for(it = v.begin(); it != v.end(); ++it)
	{
		
		CachedGLArrayLines_T *arr=(CachedGLArrayLines_T *)(*it);
		setColor(arr->color);
		//glLineWidth(arr->linewidth);
		//draw_array(arr->path->arr , arr->path->len, GL_LINE_STRIP);
		this->ogl->linewidth=arr->linewidth;
		this->ogl->draw_polyline (arr->path->arr,arr->path->len,arr->fCapStyle);

		
		//{ // Draw line in the middle  //just for test   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//	setColor(0xffff0000);
		//	ogl->draw_array(arr->path->arr , arr->path->len, GL_LINE_STRIP);
		//}
		
	}
	
}
#endif
void Sk_Draw::flushCachedOverlayLines()
{
	if(!(drawObjs->cachedGLArray_Lines.size()>0))
	{
		//LOGI("====Nothing in flushCachedGraphics====\n");
		return;
	}
	setColor(0xff335599);
	std::vector<CachedGLArrayLines_T*>::iterator it;
	for(it = drawObjs->cachedGLArray_Lines.begin(); it != drawObjs->cachedGLArray_Lines.end(); ++it)
	{
		
		CachedGLArrayLines_T *arr=(CachedGLArrayLines_T *)(*it);
		if(!arr->oneway)
			continue;
		 draw_oneway(arr->path->arr, arr->path->len);
	}
	
}

void Sk_Draw::draw_oneway(Sk_Point3D *pnt, int count)
{
	int i=0;
	float dx,dy,l;
	if(count<2)
		return;
	Sk_Point3D p;p.fX=0;p.fY=0;p.fZ=0;
	if(count==2)
		i=0;
	else
		if(count>2)
			i=1;
		dx=pnt[i+1].fX-pnt[i].fX;
		dy=pnt[i+1].fY-pnt[i].fY;
		l=sqrt(dx*dx+dy*dy);
		if (l>0) {
			dx=dx/l;   // cosine
			dy=dy/l;   //sine
			p=pnt[i+1];
			p.fX-=dx*15;
			p.fY-=dy*15;
			ogl->draw_arrow(&p, dx, dy, 8);
			/*p=pnt[i+1];
			p.fX-=dx*15;
			p.fY-=dy*15;
			ogl->draw_arrow(&p, dx, dy, 10);*/
		}
	
}

#pragma endregion
