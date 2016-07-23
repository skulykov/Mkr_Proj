#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "Sk_Draw.h"

#include "glCtx.h"


//GLuint vbo;

static FT_Library ft;
static FT_Face face;

	

	 Sk_Atlas::Sk_Atlas(FT_Face face, int height,void *gc) {
		FT_Set_Pixel_Sizes(face, 0, height);
		FT_GlyphSlot g = face->glyph;

		int roww = 0;
		int rowh = 0;
		 w = 0;
		 h = 0;

		 memset(c, 0, sizeof c);

		/* Find minimum size for a texture holding all visible ASCII characters */
		for (int i = 32; i < 128; i++) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
				//fprintf(stderr, "Loading character %c failed!\n", i);
				continue;
			}
			if (roww + g->bitmap.width + 1 >= MAXWIDTH) {
				w = max(w, roww);
				h += rowh;
				roww = 0;
				rowh = 0;
			}
			roww += g->bitmap.width + 1;
			rowh = max(rowh, g->bitmap.rows);
		}

		w = max(w, roww);
		h += rowh;

		/* Create a texture that will be used to hold all ASCII glyphs */
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glUniform1i(((Sk_Ogl *)gc)->texture_location, 0);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);

		/* We require 1 byte alignment when uploading texture data */
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		/* Clamping to edges is important to prevent artifacts when scaling */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		/* Linear filtering usually looks best for text */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		/* Paste all glyph bitmaps into the texture, remembering the offset */
		int ox = 0;
		int oy = 0;

		rowh = 0;

		for (int i = 32; i < 128; i++) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
				//fprintf(stderr, "Loading character %c failed!\n", i);
				continue;
			}

			if (ox + g->bitmap.width + 1 >= MAXWIDTH) {
				oy += rowh;
				rowh = 0;
				ox = 0;
			}

			glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
			c[i].ax = g->advance.x >> 6;
			c[i].ay = g->advance.y >> 6;

			c[i].bw = g->bitmap.width;
			c[i].bh = g->bitmap.rows;

			c[i].bl = g->bitmap_left;
			c[i].bt = g->bitmap_top;

			c[i].tx = ox / (float)w;
			c[i].ty = oy / (float)h;

			rowh = max(rowh, g->bitmap.rows);
			ox += g->bitmap.width + 1;
		}

		//fprintf(stderr, "Generated a %d x %d (%d kb) texture atlas\n", w, h, w * h / 1024);
	}

	Sk_Atlas::~Sk_Atlas() {
		glDeleteTextures(1, &tex);
	}


Sk_Atlas *a48=0;
Sk_Atlas *a24=0;
Sk_Atlas *a12=0;


int init_resources(Sk_Ogl *gc,const char *fontfilename) {
	/* Initialize the FreeType2 library */
	if (FT_Init_FreeType(&ft)) {
		//fprintf(stderr, "Could not init freetype library\n");
		return 0;
	}

	/* Load a font */
	if (FT_New_Face(ft, fontfilename, 0, &face)) {
		//fprintf(stderr, "Could not open font %s\n", fontfilename);
		return 0;
	}

	
	// Create the vertex buffer object
	//glGenBuffers(1, &vbo);

	return 1;
}

static bool atlas_init=false;
void initializeAtlas(Sk_Ogl *gc)
{
	
	if(!atlas_init)
	{
#if !SK_BUILD_FOR_ANDROID			  
		const char *fontfilename="C:\\my\\m_maps\\fonts\\DroidSerif-Regular.ttf";
#else
				  const char *fontfilename="/mnt/sdcard/fcnavgl/DroidSerif-Regular.ttf";
#endif
				  init_resources(gc,fontfilename);
				  atlas_init=true;

				 /* Create texture atlasses for several font sizes */
				a48 = new Sk_Atlas(face, 48,gc);
				a24 = new Sk_Atlas(face, 16,gc);
				a12 = new Sk_Atlas(face, 14,gc);

	}
	else
	{
		if(a48!=0)
			delete a48;
		if(a24!=0)
			delete a24;
		if(a12!=0)
			delete a12;
		a48 = new Sk_Atlas(face, 48,gc);
		a24 = new Sk_Atlas(face, 24,gc);
		a12 = new Sk_Atlas(face, 14,gc);
	}
	
	gc->atlas=a12;  // should be a different place	  
		
}




//http://www.siggraph.org/education/materials/HyperGraph/modeling/mod_tran/2drota.htm

   
	DrawT::DrawT(Sk_Ogl *gc,const char *text )
	{
		this->a=(Sk_Atlas*)gc->atlas;
		
		this->text=text;
		this->gc=gc;
		/* Use the texture containing the atlas */
		glBindTexture(GL_TEXTURE_2D, a->tex);
		glUniform1i(gc->texture_location, 0);

		/* Set up the VBO for our vertex data */
		glEnableVertexAttribArray(gc->position_location);
		glEnableVertexAttribArray(gc->texture_position_location);
		
		//glBindBuffer(GL_ARRAY_BUFFER, vbo);
		//glVertexAttribPointer(gc->position_location, 2, GL_FLOAT, GL_FALSE, sizeof(_at_point_), 0);//glVertexAttribPointer(gc->position_location, 4, GL_FLOAT, GL_FALSE, 0, 0);
		//glVertexAttribPointer(gc->texture_position_location, 2, GL_FLOAT, GL_FALSE, sizeof(_at_point_), (void*)2);

		tLen=6*strlen(text);
		coords=new _at_point_[tLen];
		c = 0;
		sy = gc->getHeight()/2.0;

		setAngle(0);

		glVertexAttribPointer(gc->position_location, 3/*2*/, GL_FLOAT, GL_FALSE, sizeof(_at_point_), &coords[0].x);//glVertexAttribPointer(gc->position_location, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(gc->texture_position_location, 2, GL_FLOAT, GL_FALSE, sizeof(_at_point_), &coords[0].s);

	}
	void DrawT::finalDraw()
	{
		/* Draw all the characters on the screen in one go */
		//glBufferData(GL_ARRAY_BUFFER, sizeof(_at_point_)*tLen, coords, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, c);

		glDisableVertexAttribArray(gc->position_location);
		glDisableVertexAttribArray(gc->texture_position_location);
		
		glBindTexture(GL_TEXTURE_2D, 0);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		if(coords)
			delete[] coords;
	}

	void DrawT::setCoords(_at_point_*p,GLfloat x,GLfloat y,GLfloat s,GLfloat t)
	{
	
		//do 2D rotation here!!
		
		if(angle!=0)
		{
			
			x=x-X0;
			y=y-Y0;
			float x1=x*cosAngle-y*sinAngle;
			float y1=y*cosAngle+x*sinAngle;
			
			p->x=x1+X0;
			p->y=y1+Y0;
			p->z=zLevel;

		}
		else
		{
			p->x=x;//*w/2;//+w/2;
			p->y=y;///* *h/2*/+h/2;
			p->z=zLevel;
		}
		
		/*p->x+=xOffset;
		p->y+=yOffset;*/

		p->s=s;
		p->t=t;
	}
	void DrawT::copyCoords(_at_point_*dst,_at_point_*src)
	{
		dst->x=src->x;
		dst->y=src->y;
		dst->z=src->z;
		dst->s=src->s;
		dst->t=src->t;
	}
	void DrawT::setAngleVal(float _sin,float _cos)
	{
		sinAngle = _sin;//sinf ( angle * _PI_ / 180.0f );
		cosAngle = _cos;//cosf ( angle * _PI_ / 180.0f );
		angle=1;
	}
	float DrawT::setAngleVal(float x2,float y2,float x1,float y1)
	{
		float dx=x2-x1;
		float dy=y2-y1;
		float curlen=sqrtf(dx*dx+dy*dy);
		float q=1.0/curlen;
		sinAngle = dy*q;
        cosAngle = dx*q;
		angle=1;
		return curlen;
	}
	void DrawT::setAngleVal(Sk_Point3D *p,int index)
	{
		float dx=p[index].fX-p[index-1].fX;
		float dy=p[index].fY-p[index-1].fY;
		float q=1.0/sqrtf(dx*dx+dy*dy);
		sinAngle = dy*q;
        cosAngle = dx*q;
		angle=1;
		//return curlen;
	}
	void DrawT::setAngle(int _angle)
	{
		angle=_angle;
		sinAngle = sinf ( angle * _PI_ / 180.0f );
		cosAngle = cosf ( angle * _PI_ / 180.0f );
	}
	void DrawT::__setCoords(_at_point_*p,GLfloat x,GLfloat y,GLfloat s,GLfloat t)
	{

		p->x=x;//*w/2;//+w/2;
		p->y=y;///* *h/2*/+h/2;
		p->z=zLevel;

		p->s=s;
		p->t=t;
	}
#if 0	
	void DrawT::render_text_line(float x, float y)  // ONLY horizontally
	{
		
		float y2=0;
		/* Loop through all characters */
		for (const uint8_t *p = (const uint8_t *)text; *p; p++) {
			/* Calculate the vertex and texture coordinates */
			struct CBitmap_S *b=&a->c[*p];
			float x2 = x + b->bl ;
			if(y2==0)
			{
				y2 = y - b->bh ;
			}
		
			float w = b->bw;
			float h = b->bh ;

			/* Advance the cursor to the start of the next character */
			x += b->ax ;
			//y += b->ay ;

			/* Skip glyphs that have no pixels */
			if (!w || !h)
				continue;

			{
				float tx1=b->tx + w / a->w;
				float ty1=b->ty + h / a->h;

												
				setCoords(&coords[c],x2, y2, b->tx, b->ty); // vertex 1 of the quad
				c++;
				setCoords(&coords[c], x2+w, y2, tx1, b->ty);   //v2
				c++;
				setCoords(&coords[c],x2, y2+h, b->tx, ty1);  //v3 
				c++;
				copyCoords(&coords[c],&coords[c-2]);  //v2
				c++;
				copyCoords(&coords[c],&coords[c-2]);  //v3
				c++;
				setCoords(&coords[c], x2+w, y2+ h, tx1, ty1); //vertex 4 of the quad
				c++;
			}
		}
	}
#else
	void DrawT::render_text_line(float x, float y)  // ONLY horizontally
	{
		char *pStr = (char*)this->text;

		float z0 = 0;

		while (1)
		{

			if (*pStr == 0)
			{
				return;
			}

			struct CBitmap_S *b = &a->c[*pStr];
			float w = b->bw;
			float h = b->bh;

			float z;//=b->bt*0.5;
			if (z0 == 0)
			{
				z0 = b->bt*0.5;
				z = z0;
			}
			else
			{
				z = b->bt - z0;
			}

			//calculate origin point
			X0 = x;//+ b->bl ;

			//advance to the next character
			x += b->ax;
		
			float x2 = X0 + w;

			/* Skip glyphs that have no pixels */
			pStr++;
			if (!w || !h)
				continue;

			{
				float tx1 = b->tx + b->bw / a->w;
				float ty1 = b->ty + b->bh / a->h;

				//float _cos = z;
				//float _sin = 0;

				//float _cos0 = z0;
				//float _sin0 = 0;

				__setCoords(&coords[c], X0 , y - z, b->tx, b->ty); // vertex 1 of the quad -> top left
				c++;
				__setCoords(&coords[c], x2 , y -z, tx1, b->ty);   //v2                   -> top right
				c++;
				__setCoords(&coords[c], X0 , y + z0, b->tx, ty1);  //v3                    -> bottom left
				c++;
				copyCoords(&coords[c], &coords[c - 2]);  //v2
				c++;
				copyCoords(&coords[c], &coords[c - 2]);  //v3
				c++;
				__setCoords(&coords[c], x2 , y + z0, tx1, ty1); //vertex 4 of the quad    -> bottom right
				c++;

			}

			
		}
	}
#endif

#if 1
	void DrawT::render_text_path(/*Sk_Path *path,float openSpace*/TextDrawInfo *text)
	{
		Sk_Path *path = text->path;
		
		float openSpace = text->openSpace;
		if (gc->rp.useProjection)  // do not show the upper part  -> like in Google navigation
		{
			int h = gc->getHeight();
			h = h*(0.5 + gc->rp.ty / 2.0);
			for (int i = 0; i<path->len; i++)
			{
				if (path->arr[i].fY<h)
					return;
			}
		}


		float x, y;
		
		int k = 1;
		if (/*path->len>2*/1)
		{

			x = path->arr[0].fX;
			y = path->arr[0].fY;
			//find the start point where to draw
			openSpace *= 0.5;
			for (int i = k; i < path->len; i++)
			{

				float x1 = path->arr[i].fX;
				float y1 = path->arr[i].fY;

				float dx = x1 - x;
				float dy = y1 - y;
				float curlen = sqrtf(dx*dx + dy*dy);
				if (curlen < openSpace)
				{
					openSpace -= curlen;
					x = x1;
					y = y1;
					continue;
				}
				else
				{
					k = i;
					/*x=path->arr[k-1].fX;
					y=path->arr[k-1].fY;*/

					float q = 1.0 / curlen;
					float sin_ = dy*q;
					float cos_ = dx*q;
					x += openSpace*cos_;
					y += openSpace*sin_;

					break;
				}
			}
		}
		else
		{
			float a1 = text->pathRotate;
			float w1 = text->bounds.width() / 2;
			x = text->centerX - w1*cos(a1);
			y = text->centerY - w1*sin(a1);

		}
		char *pStr = (char*)this->text;
		for (int i = k; i<path->len; i++)
		{

			float x1 = path->arr[i].fX;
			float y1 = path->arr[i].fY;


			float curlen = setAngleVal(x1, y1, x, y);


			float z0 = 0;

			while (1)
			{

				if (*pStr == 0)
				{
					/*i=path->len;
					break;*/
					return;
				}

				struct CBitmap_S *b = &a->c[*pStr];
				float w = b->bw;
				float h = b->bh;

				float z;//=b->bt*0.5;
				if (z0 == 0)
				{
					z0 = b->bt*0.5;
					z = z0;
				}
				else
				{
					z = b->bt - z0;
				}

				int length = b->ax;//curr->w;
				if (curlen < length)
				{
					if ((i + 1) >= path->len)
					{
						// this is the last path point BUT the whole string has not been drawn yet!!!
						//Just continue with the Last angle (sinAngle/cosAngle)
					}
					else
						break;
				}
				//calculate origin point
				X0 = x;//+ b->bl ;
				Y0 = y;// - b->bt ;



				//advance to the next character
				x += length*cosAngle;
				y += length*sinAngle;

				float x2 = X0 + w*cosAngle;
				float y2 = Y0 + w*sinAngle;




				/* Skip glyphs that have no pixels */
				pStr++;
				if (!w || !h)
					continue;

				{
					float tx1 = b->tx + b->bw / a->w;
					float ty1 = b->ty + b->bh / a->h;

					float _cos = z*cosAngle;
					float _sin = z*sinAngle;

					float _cos0 = z0*cosAngle;
					float _sin0 = z0*sinAngle;

					__setCoords(&coords[c], X0 + _sin, Y0 - _cos, b->tx, b->ty); // vertex 1 of the quad -> top left
					c++;
					__setCoords(&coords[c], x2 + _sin, y2 - _cos, tx1, b->ty);   //v2                   -> top right
					c++;
					__setCoords(&coords[c], X0 - _sin0, Y0 + _cos0, b->tx, ty1);  //v3                    -> bottom left
					c++;
					copyCoords(&coords[c], &coords[c - 2]);  //v2
					c++;
					copyCoords(&coords[c], &coords[c - 2]);  //v3
					c++;
					__setCoords(&coords[c], x2 - _sin0, y2 + _cos0, tx1, ty1); //vertex 4 of the quad    -> bottom right
					c++;

				}

				curlen -= length;


			}
		}
	
	}
#else
	void DrawT::render_text_path(/*Sk_Path *path,float openSpace*/TextDrawInfo *text)
	{
		Sk_Path *path = text->path;
		float openSpace = text->openSpace;
		if(gc->rp.useProjection)  // do not show the upper part  -> like in Google navigation
		{
			int h=gc->getHeight();
			h=h*(0.5+gc->rp.ty/2.0);
			for(int i=0;i<path->len;i++)
			{
				if(path->arr[i].fY<h)
					return;
			}
		}
		
		
		float x=path->arr[0].fX;
		float y=path->arr[0].fY;
		int k=1;
		//find the start point where to draw
		openSpace*=0.5;
		for(int i=k;i<path->len;i++)
		{
		
			float x1=path->arr[i].fX;
			float y1=path->arr[i].fY;

			float dx=x1-x;
			float dy=y1-y;
			float curlen=sqrtf(dx*dx+dy*dy);
			if(curlen<openSpace)
			{
				openSpace-=curlen;
				x=x1;
				y=y1;
				continue;
			}
			else
			{
				k=i;
				/*x=path->arr[k-1].fX;
				y=path->arr[k-1].fY;*/

				float q=1.0/curlen;
				float sin_ = dy*q;
				float cos_ = dx*q;
				x+=openSpace*cos_;
				y+=openSpace*sin_;

				break;
			}
		}

	    char *pStr=(char*)this->text;
		for(int i=k;i<path->len;i++)
		{
			
			float x1=path->arr[i].fX;
			float y1=path->arr[i].fY;
			
			
			float curlen=setAngleVal(x1,y1,x,y);

			
		   float z0=0;

			while(1)
			{
				
				if(*pStr==0)
				{
					/*i=path->len;
					break;*/
					return;
				}

				struct CBitmap_S *b=&a->c[*pStr];
				float w = b->bw;
				float h = b->bh ;
				
				float z;//=b->bt*0.5;
				if(z0==0)
				{
					z0=b->bt*0.5;
					z=z0;
				}
				else
				{
					z=b->bt-z0;
				}

				int length=b->ax;//curr->w;
				if(curlen < length )
				{
					if( (i+1)>=path->len)
					{
						// this is the last path point BUT the whole string has not been drawn yet!!!
						//Just continue with the Last angle (sinAngle/cosAngle)
					}
					else
						break;
				}
				//calculate origin point
				X0 = x ;//+ b->bl ;
				Y0 = y;// - b->bt ;
								
				

				//advance to the next character
				x+=length*cosAngle;
				y+=length*sinAngle;

				float x2=X0+w*cosAngle;
				float y2=Y0+w*sinAngle;

				
				

				/* Skip glyphs that have no pixels */
				pStr++;
				if (!w || !h)
					continue;
				
				{
					float tx1=b->tx + b->bw / a->w;
					float ty1=b->ty + b->bh / a->h;

					float _cos=z*cosAngle;
					float _sin=z*sinAngle;

					float _cos0=z0*cosAngle;
					float _sin0=z0*sinAngle;

					__setCoords(&coords[c],X0+_sin, Y0-_cos, b->tx, b->ty); // vertex 1 of the quad -> top left
					c++;
					__setCoords(&coords[c],x2+_sin , y2-_cos, tx1, b->ty);   //v2                   -> top right
					c++;
					__setCoords(&coords[c],X0-_sin0, Y0+_cos0 , b->tx, ty1);  //v3                    -> bottom left
					c++;
					copyCoords(&coords[c],&coords[c-2]);  //v2
					c++;
					copyCoords(&coords[c],&coords[c-2]);  //v3
					c++;
					__setCoords(&coords[c],x2 -_sin0, y2 + _cos0, tx1, ty1); //vertex 4 of the quad    -> bottom right
					c++;

				}

				curlen -= length;
				
				
			}
		}
	}

#endif



#if 0
void test_test_1(Sk_Ogl *gc)
{
		  
		  static bool init=false;
			  if(!init)
			  {
				  const char *fontfilename="C:\\my\\FCNavigation\\FCNav-kernel-mvs\\VCProjects\\FCNavTest\\FCNavGLTest\\fonts\\DroidSans.ttf";
				  init_resources(gc,fontfilename);
				  init=true;
			  }
		 // float sx = 2.0 / gc->width;
		  
		  
		  
		  /* Enable blending, necessary for our alpha texture */
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		  set_color(gc,1.0,0,0);



		 // render_text(gc,"The Quick Brown Fox Jumps Over The Lazy Dog", a48, -1 + 8 * sx, 1 - 50 * sy, sx, sy);
		  int y=50;
		  //render_text(gc,"The Quick Brown Fox Jumps Over The Lazy Dog", a24, 0, y);
		 //  render_text(gc,"The Quick Brown Fox Jumps Over The Lazy Dog", a48, 10,40, 1, 1);

		  int flag=0;

		  gc->atlas=a24;
		  DrawT dr(gc,"The Quick Brown Fox Jumps Over The Lazy Dog");
		  if(flag)
		  {
			   int count = 4;
			  struct point p1[4];

			  /* p1[0].x=100;
			   p1[0].y=100;

			   p1[1].x=200;
			   p1[1].y=50;

			   p1[2].x=300;
			   p1[2].y=120;

			   p1[3].x=500;
			   p1[3].y=100;*/

			   p1[0].x=400;
			   p1[0].y=100;

			   p1[1].x=360;
			   p1[1].y=150;

			   p1[2].x=400;
			   p1[2].y=200;

			   p1[3].x=320;
			   p1[3].y=340;

			   dr.render_text_path(p1,count);
		  }
		  else
		  {
			  dr.setAngle(60);
			  dr.render_text_line(20,20);
		  }
		  dr.finalDraw();

}
#endif


void DrawT::drawOnPath(Sk_Ogl *gc,TextDrawInfo *text, Sk_Paint *paint)
{
			
			//static int count=0;
			//string s="Lauderdale";  // "Corel Acres Drive"
			//if(text->text.find(s)!=string::npos)
			//{
			//	count++;
			//	_sk_log ("lauderdale count=%d",count);
			//	if(count>=2)
			//		return;
			//}
			
			//return;
			// Enable blending, necessary for our alpha texture
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	        
	       // gc->enableText(1);

			if(text->textSize>=12&&text->textSize<=14)
				gc->atlas=a12;  
			else
				gc->atlas=a24;

			gc->setColor(paint->getColor());

			//gc->atlas=a24;

		    DrawT dr(gc,text->text.c_str());
			dr.setZLevel(gc->getZLevel());


			dr.render_text_path(/*text->path,text->openSpace*/text);
		    dr.finalDraw();
			
		//	gc->enableText(0);
			glDisable(GL_BLEND);
}
void DrawT::drawText(Sk_Ogl *gc,const char *str,int len,int x,int y,Sk_Paint *paint)
{
		  //return;
			// Enable blending, necessary for our alpha texture
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			gc->setColor(paint->getColor());
			
			if(paint->getTextSize()>=12&&paint->getTextSize()<=14)
				gc->atlas=a12;  
			else
				gc->atlas=a24;

		    DrawT dr(gc,str);

			dr.setAngle(0);
			dr.setZLevel(gc->getZLevel());
			dr.render_text_line(x,y);
			
		    dr.finalDraw();
			glDisable(GL_BLEND);
}

#if 0
void ogl_clear();
void TestTextDraw1()
{
	ogl_clear();

	/*UpdateProj (pGraphicCtx,0.1);
	*/

	struct point p;
	p.x=200;
	p.y=600;
	
	//DrawT::drawOnPath(pGraphicCtx,"Test Text",&p,60);
		glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			setColor(pGraphicCtx,0x0000ffff);
			
		    DrawT dr(pGraphicCtx,"The Quick Brown Fox Jumps Over The Lazy Dog");

			dr.setAngle(30);
			dr.setZLevel(0.5);
			dr.render_text_line(p.x,p.y);
			
			UpdateProj (pGraphicCtx,0.0);
				

		    dr.finalDraw();
			glDisable(GL_BLEND);

	
	
}

#endif
