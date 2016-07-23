#include <vector>
#include <set>
#include <algorithm>
#include <math.h>
#include <time.h>

#include "common.h"
#include "renderRules.h"
//#include "utf8.cpp"
#include "utf8/unchecked.h"
#include "CMapRenderer.h"
#include "glCtx.h"


bool isLetterOrDigit(char c)
{
	return c != ' ';
}


inline float sqr(float a){
	return a*a;
}

inline float absFloat(float a){
	return a > 0 ? a : -a;
}

bool intersects(Sk_Rect tRect, float tRot, TextDrawInfo* s)
{
	float sRot = s->pathRotate;
	if (absFloat(tRot) < M_PI / 15 && absFloat(sRot) < M_PI / 15) {
		return Sk_Rect::Intersects(tRect, s->bounds);
	}
	float dist = sqrt(sqr(tRect.centerX() - s->bounds.centerX()) + sqr(tRect.centerY() - s->bounds.centerY()));
	if(dist < 3) {
		return true;
	}
	Sk_Rect sRect = s->bounds;

	// difference close to 90/270 degrees
	if(absFloat(cos(tRot-sRot)) < 0.3 ){
		// rotate one rectangle to 90 degrees
		tRot += M_PI_2;
		tRect = Sk_Rect::MakeXYWH(tRect.centerX() -  tRect.height() / 2, tRect.centerY() -  tRect.width() / 2,
				tRect.height(), tRect.width());
	}

	// determine difference close to 180/0 degrees
	if(absFloat(sin(tRot-sRot)) < 0.3){
		// rotate t box
		// (calculate offset for t center suppose we rotate around s center)
		float diff = atan2(tRect.centerY() - sRect.centerY(), tRect.centerX() - sRect.centerX());
		diff -= sRot;
		float left = sRect.centerX() + dist* cos(diff) - tRect.width()/2;
		float top = sRect.centerY() - dist* sin(diff) - tRect.height()/2;
		Sk_Rect nRect = Sk_Rect::MakeXYWH(left, top, tRect.width(), tRect.height());
		return Sk_Rect::Intersects(nRect, sRect);
	}

	// TODO other cases not covered
	return Sk_Rect::Intersects(tRect, sRect);
}

bool intersects(TextDrawInfo* t, TextDrawInfo* s) {
	return intersects(t->bounds, t->pathRotate, s);
}
inline float _max_(float a, float b) {  //svk renamed to _max_
  return a > b ? a : b;
}
int nextWord(uint8_t* s, int* charRead) {
	uint8_t* init = s;
	while((*s) != 0) {
		uint32_t tp = utf8::unchecked::next(s);
		(*charRead) ++;
		if(tp == ' ' || tp == '\t') {
			return (s - init);
		}
	}
	return -1;

}

static Sk_Rect GetRect(Sk_Point3D *points)
{
	float l = 100000,t=100000,r=0,b=0;
	for (int i = 0; i < 4; i++)
	{
		Sk_Point3D p = points[i];
		if (p.fX < l) l = p.fX;
		if (p.fY < t) t = p.fY;

		if (p.fX > r) r = p.fX;
		if (p.fY > b) b = p.fY;
	}
	return Sk_Rect::MakeLTRB(l, t, r, b);
}

void CMapRenderer::drawTestBox( TextDrawInfo* i, float A,  std::string text, int color)
{
	Sk_Rect *r=&i->bounds;	  
	
	//http://www.codersource.net/MFC/OpenGL/DetectinglinerectangleintersectioninOpenGL.aspx
	
	float x=i->centerX;
	float y=i->centerY;    // we will rotate around the center
	//http://stackoverflow.com/questions/644378/drawing-a-rotated-rectangle
	float w=r->width()/2.0;
	float h=r->height()/2.0;

	float _cos = cos(A), _sin = sin(A);
	float hs = h*_sin,ws=w*_sin;
	float hc = (h)* _cos, wc = (w)* _cos;

	Sk_Point3D p[4],pp[4];
	for (int i = 0; i < 4; i++)
	{
		p[i].fZ = ogl->getZLevel();
		pp[i].fZ = ogl->getZLevel();
	}

	p[0].fX  =  x + wc - hs , p[0].fY=  y + hc  + ws;                 //UL
	p[1].fX  =  x - wc - hs , p[1].fY = y + hc  - ws;                //UR
	p[2].fX  =  x - wc + hs, p[2].fY = y - hc  - ws;                //BR
	p[3].fX  =   x + wc + hs ,p[3].fY = y - hc  + ws;                //BL
	this->ogl->drawRectLine(p,4 ,color,1);

	//ogl->drawPoints(&p[0], 1, 10, 0x00ff0000); // Red
	//ogl->drawPoints(&p[1], 1, 10, 0x0000ff00); // Green
	//ogl->drawPoints(&p[2], 1, 10, 0x000000ff); // Blue

	/*Sk_Rect rect = GetRect(p);
	r->set(rect.fLeft,rect.fTop,rect.fRight,rect.fBottom);*/

	r->set(p[0].fX, p[0].fY, p[2].fX, p[2].fY);

	//pp[0].fX = r->fLeft, pp[0].fY = r->fTop;                 //UL
	//pp[1].fX = r->fRight , pp[1].fY = r->fTop;                //UR
	//pp[2].fX = r->fRight, pp[2].fY = r->fBottom;                //BR
	//pp[3].fX = r->fLeft, pp[3].fY = r->fBottom;                //BL
	//this->ogl->drawRectLine(pp, 4, 0x00ff0000, 1);
	//

	//Sk_Point3D p2={i->path->arr[0].fX,i->path->arr[0].fY,ogl->getZLevel()};
	//ogl->drawPoints(&p2, 1,10,0x0000ff00); // Green

	//int len=i->path->len;
	//Sk_Point3D p3={i->path->arr[len-1].fX,i->path->arr[len-1].fY,ogl->getZLevel()};
	//ogl->drawPoints(&p3, 1,10,0x000000ff); //Blue
	//
	//for (int j = 1; j<i->path->len-1; j++)
	//{
	//	if (j == len / 2) continue;
	//	Sk_Point3D p1 = { i->path->arr[j].fX, i->path->arr[j].fY, ogl->getZLevel() };
	//	ogl->drawPoints(&p1, 1, 10, 0x00ffff00); //ARGB  -> Red
	//}
	//
	//Sk_Point3D p1 = { i->centerX, i->centerY, ogl->getZLevel() };
	//ogl->drawPoints(&p1, 1, 10, 0x00ff0000); //ARGB  -> Red
}
	
void CMapRenderer::drawTextOnCanvas(const char* text, uint16_t len, float centerX, float centerY, Sk_Paint *paintText,
		float textShadow) {
	if (textShadow > 0) {
		int c = paintText->getColor();
		paintText->setStyle(Sk_Paint::kStroke_Style);
		paintText->setColor(-1); // white
		paintText->setStrokeWidth(2 + textShadow);
		DrawT::drawText(ogl,text, len, centerX, centerY, paintText);
// reset
		paintText->setStrokeWidth(2);
		paintText->setStyle(Sk_Paint::kFill_Style);
		paintText->setColor(c);
	}
	DrawT::drawText(ogl,text, len, centerX, centerY, paintText);
}

void CMapRenderer::fillTextProperties(TextDrawInfo* info, float cx, float cy) {
	RenderingRuleSearchRequest* render=this->req;
	info->centerX = cx;
	info->centerY = cy + render->getIntPropertyValue(render->props()->R_TEXT_DY, 0);
	// used only for draw on path where centerY doesn't play role
	//info->vOffset = render->getIntPropertyValue(render->props()->R_TEXT_DY, 0);
	info->textColor = render->getIntPropertyValue(render->props()->R_TEXT_COLOR);
	if (info->textColor == 0) {
		info->textColor = 0xff000000;
	}
	info->textSize = render->getIntPropertyValue(render->props()->R_TEXT_SIZE);
	info->textShadow = render->getIntPropertyValue(render->props()->R_TEXT_HALO_RADIUS, 0);
	info->textWrap = render->getIntPropertyValue(render->props()->R_TEXT_WRAP_WIDTH, 0);
	info->bold = render->getIntPropertyValue(render->props()->R_TEXT_BOLD, 0) > 0;
	info->minDistance = render->getIntPropertyValue(render->props()->R_TEXT_MIN_DISTANCE, 0);
	info->shieldRes = render->getStringPropertyValue(render->props()->R_TEXT_SHIELD);
	info->textOrder = render->getIntPropertyValue(render->props()->R_TEXT_ORDER, 100);
}



//static struct graphics_font_methods *meth=0;
//static struct font_freetype_font *font = 0;
	

void CMapRenderer::drawWrappedText(TextDrawInfo* text, Sk_Paint *paint) {
	
	char ch=text->shieldRes[1];
	
	int len = text->text.length();
	const char* c_str = text->text.c_str();

	float x=text->centerX,y=text->centerY;

	Sk_Point3D p1={x-6,y-13,ogl->getZLevel()};
	ogl->draw_image(&p1, (Sk_Texture *)texture);

	
	//ogl->enableText(1);
	paint->setColor(0xffffffff);
	DrawT::drawText(ogl,c_str, len, text->centerX, text->centerY, paint);
		
}
void CMapRenderer::testTexture()
{
	Sk_Point3D p1={ogl->getWidth()+2, ogl->getHeight()+2,ogl->getZLevel()};
	ogl->draw_image(&p1, (Sk_Texture *)texture);
}

#if USE_OLD_TEXT_LOCATOR
bool CMapRenderer::calculatePathToRotate(TextDrawInfo* p) {
	if(p->path == NULL) {
		return true;
	}
	int len = p->path->countPoints();
	//Sk_Point points[len];
	//svk
	if(len>=128)
       return false;
	Sk_Point3D points[128];// = new Sk_Point[len];
	p->path->getPoints(points, len);
	if (!p->drawOnPath) {
		// simply calculate rotation of path used for shields
		float px = 0;
		float py = 0;
		for (int i = 1; i < len; i++) {
			px += points[i].fX - points[i - 1].fX;
			py += points[i].fY - points[i - 1].fY;
		}
		if (px != 0 || py != 0) {
			p->pathRotate = atan2(py, px);
		}
		//delete[] points; //svk
		return true;
	}

	bool inverse = false;
	float roadLength = 0;
	bool prevInside = false;
	float visibleRoadLength = 0;
	float textw = p->bounds.width();
	int i;
	int startVisible = 0;
	/*std::vector<float> distances;
	distances.resize(roadLength, 0);*/

	float normalTextLen = 1.5 * textw;
	for (i = 0; i < len; i++) {
		bool inside = points[i].fX >= 0 && points[i].fX <= 2*ogl->getWidth() &&
				points[i].fY >= 0 && points[i].fY <= 2*ogl->getHeight();
		if (i > 0) {
			float d = sqrt(
					(points[i].fX - points[i - 1].fX) * (points[i].fX - points[i - 1].fX)
							+ (points[i].fY - points[i - 1].fY) * (points[i].fY - points[i - 1].fY));
			//distances.push_back(d);
			roadLength += d;
			if(inside) {
				visibleRoadLength += d;
				if(!prevInside) {
					startVisible = i - 1;
				}
			} else if(prevInside) {
				if(visibleRoadLength >= normalTextLen) {
					break;
				}
				visibleRoadLength = 0;
			}

		}
		prevInside = inside;
	}
	if (textw >= roadLength) {
		//delete[] points; //svk
		return false;
	}
	p->openSpace=roadLength - textw;
	int startInd = 0;
	int endInd = len;
#if 0//NOT_IMPL
	if(textw < visibleRoadLength && i - startVisible > 1) {
		startInd = startVisible;
		endInd = i;
		// display long road name in center
		if (visibleRoadLength > 3 * textw) {
			bool ch ;
			do {
				ch = false;
				if(endInd - startInd > 2 && visibleRoadLength - distances[startInd] > normalTextLen){
					visibleRoadLength -= distances.at(startInd);
					startInd++;
					ch = true;
				}
				if(endInd - startInd > 2 && visibleRoadLength - distances[endInd - 2] > normalTextLen){
					visibleRoadLength -= distances.at(endInd - 2);
					endInd--;
					ch = true;
				}
			} while(ch);
		}
	}
	// shrink path to display more text
	if (startInd > 0 || endInd < len) {
		// find subpath
		Sk_Path* path = new Sk_Path(endInd-startInd);
		int k=0;
		for (int i = startInd; i < endInd; i++) {
			/*if (i == startInd) {
				path->moveTo(points[i].fX, points[i].fY);
			} else {
				path->lineTo(points[i].fX, points[i].fY);
			}*/
			path->set(k,points[i].fX, points[i].fY);
			k++;
		}
		if (p->path != NULL) {
			delete p->path;
		}
		p->path = path;
	}
#endif
#if 1    // need more nice algorithm !!!
	// calculate vector of the road (px, py) to proper rotate it
	float px = 0;
	float py = 0;
	/*for (i = startInd + 1; i < endInd; i++) {
		px += points[i].fX - points[i - 1].fX;
		py += points[i].fY - points[i - 1].fY;
	}*/
	px=points[endInd-1 ].fX - points[startInd].fX;
	py=points[endInd-1 ].fY - points[startInd].fY;
	
	float scale = 0.5f;
	float plen = sqrt(px * px + py * py);
	// vector ox,oy orthogonal to px,py to measure height
	float ox = -py;
	float oy = px;
	if(plen > 0) {
		float rot = atan2(py, px);
		if (rot < 0) rot += M_PI * 2;
		if (rot > M_PI_2 && rot < 3 * M_PI_2) {
			rot += M_PI;
			inverse = true;
			ox = -ox;
			oy = -oy;
		}
		p->pathRotate = rot;
		ox *= (p->bounds.height() / plen) / 2;
		oy *= (p->bounds.height() / plen) / 2;
	}


	p->centerX = points[startInd].fX + scale * px + ox;
	p->centerY = points[startInd].fY + scale * py + oy;

	/*p->vOffset += p->textSize / 2 - 1;
	p->hOffset = 0;*/

	if (inverse) {
		
		Sk_Path* path = new Sk_Path(endInd-startInd);
		int k=0;
		for (int i = endInd - 1; i >= startInd; i--) {
			/*if (i == (int)(endInd - 1)) {
				path->moveTo(points[i].fX, points[i].fY);
			} else {
				path->lineTo(points[i].fX, points[i].fY);
			}*/
			path->set(k,points[i].fX, points[i].fY);
			k++;
			
		}
		if (p->path != NULL) {
			delete p->path;
		}
		p->path = path;
	}
#endif
	//delete[] points; //svk
	return true;
}
#else
const float M_PI_8 = M_PI_4 / 4;
bool CMapRenderer::calculatePathToRotate(TextDrawInfo* p) {
	if(p->path == NULL) {
		return true;
	}
	int len = p->path->countPoints();
	//Sk_Point points[len];
	//svk
	if(len>=128)
		return false;
	Sk_Point3D points[128];// = new Sk_Point[len];
	float distances[128];
	p->path->getPoints(points, len);
	if (!p->drawOnPath) {
		// simply calculate rotation of path used for shields
		float px = 0;
		float py = 0;
		for (int i = 1; i < len; i++) {
			px += points[i].fX - points[i - 1].fX;
			py += points[i].fY - points[i - 1].fY;
		}
		if (px != 0 || py != 0) {
			p->pathRotate = atan2(py, px);
		}
		//delete[] points; //svk
		return true;
	}

	p->endIndex = -1;

	float roadLength = 0;
	
	float textw = p->bounds.width();
	float prev_rot = -1;
	int startVisible = 0;
	
	int startIndex = 0, endIndex=-1;
	float segmentLength = 0;

	float px = 0;
	float py = 0;

	
	vector<TextLocator> segments;

	//float normalTextLen = 1.0 * textw;
	int numberOfDirectionsChange = 0;
	distances[0] = 0;
	for (int i = 0; i < len; i++) {
		bool inside = points[i].fX >= 0 && points[i].fX <= 2*ogl->getWidth() &&	points[i].fY >= 0 && points[i].fY <= 2*ogl->getHeight();
		if (i > 0) {
			px = points[i].fX - points[i - 1].fX, py = points[i].fY - points[i - 1].fY;
			float d = sqrt(px*px + py*py);
			roadLength += d; 
			segmentLength += d;
			distances[i] = roadLength;
			/*if(inside) {
				visibleRoadLength += d;
				if(!prevInside) {
					startVisible = i - 1;
				}
			}
			else if (prevInside) {
				if (visibleRoadLength >= normalTextLen) {
					break;
				}
				visibleRoadLength = 0;
			}*/
			float rot = atan2(py, px);
			if (i>1)
			{
				float q = rot - prev_rot;
				if (abs(q) > M_PI_8)
				{
					
					if (textw < segmentLength)
					{
						TextLocator tl;
						tl.endIndex = i;
						//break;        // we found the ~linear segment on the curve path
						
						tl.startIndex = startIndex;
						tl.roadSegmentlen = segmentLength;
						segments.push_back(tl);

						//start finding a new segment
						startIndex = i, endIndex = -1,segmentLength = 0;
					}
					else
					{
						startIndex = i;
						endIndex = -1, segmentLength = 0;
						numberOfDirectionsChange++;
					}
				}
			}
			prev_rot = rot;
		}
		
	}
	int size=segments.size();
	if (textw >= roadLength) {
		
		return false;
	}
	bool linearSegmentFound = size > 0;
	if (linearSegmentFound)
	{

		// find longest senment to draw text
		TextLocator  tl1 = { 0 };

		for (/*auto tt : segments*/int i = 0; i<size;i++)
		{
			TextLocator tt = segments[i];
			if (tt.roadSegmentlen > tl1.roadSegmentlen)
				tl1 = tt;
		}
		p->openSpace = tl1.roadSegmentlen - textw;
		p->startIndex = tl1.startIndex, p->endIndex = tl1.endIndex;
	}
	else
	{
		
	   // we did not find any segments It might be because this is a linear ( straight) path
		p->openSpace = roadLength - textw;
		p->startIndex = 0, p->endIndex = len-1;
	}
	
	
	bool inverse = false;
	//2. Calculate rotate angle
	px = points[p->endIndex].fX - points[p->startIndex].fX;
	py = points[p->endIndex].fY - points[p->startIndex].fY;

	float scale = 0.5f;
	//float plen = sqrt(px * px + py * py);
	
	if(/*plen > 0*/px!=0||py!=0) {
		float rot = atan2(py, px);
		if (rot < 0) rot += M_PI * 2;
		if (rot > M_PI_2 && rot < 3 * M_PI_2) {
			rot += M_PI;
			inverse = true;
		
		}
		p->pathRotate = rot;
	}
	if (linearSegmentFound)
	{
		p->centerX = points[p->startIndex].fX + scale * px;
		p->centerY = points[p->startIndex].fY + scale * py;
	}
	else
	{
		if (numberOfDirectionsChange == 0)
		{
			  // this is a straight line
			p->centerX = points[p->startIndex].fX + scale * px;
			p->centerY = points[p->startIndex].fY + scale * py;
		}
		else
		{
			
			//// this is a curve path
			//// this is very approximate !!! 
			//p->centerX = points[len / 2].fX;
			//p->centerY = points[len / 2].fY;
			
			float md = roadLength / 2;
			for (int i = 0; i < len; i++)
			{
				if (distances[i]>=md)
				{
					float z0 = md - distances[i-1];
				
					px = points[i].fX - points[i-1].fX;
					py = points[i].fY - points[i-1].fY;
					float aa = atan2(py, px);
					p->centerX = points[i-1].fX+z0*cos(aa);
					p->centerY = points[i-1].fY+z0*sin(aa);

					break;
				}
			}
		}
	}
	
	if (inverse) {
		Sk_Path* path = new Sk_Path(p->endIndex - p->startIndex + 1);
		int k=0;
		for (int i = p->endIndex; i >= p->startIndex; i--) {
			path->set(k,points[i].fX, points[i].fY);
			k++;
		}
		if (p->path != NULL) {
			delete p->path;
		}
		p->path = path;
	}
	else if (linearSegmentFound)
	{
		Sk_Path* path = new Sk_Path(p->endIndex - p->startIndex + 1);
		int k = 0;
		for (int i = p->startIndex; i <= p->endIndex; i++) {
			path->set(k, points[i].fX, points[i].fY);
			k++;
		}
		if (p->path != NULL) {
			delete p->path;
		}
		p->path = path;
	}
	
	return true;
}
#endif
#if USE_OLD_TEXT_LOCATOR
bool CMapRenderer::findTextIntersection(quad_tree<TextDrawInfo*>& boundIntersections, TextDrawInfo* text,
		Sk_Paint* paintText, Sk_Paint* paintIcon/*,int test*/) {
	paintText->measureText((Sk_Atlas*)ogl->atlas,text->text.c_str(), text->text.length(), &text->bounds);
	// make wider
	text->bounds.inset(-ogl->getDensityValue( 3), -ogl->getDensityValue(10));

	

	bool display = calculatePathToRotate(text);
	if (!display) {
		return true;
	}
	
	if(text->path == NULL) {
		text->bounds.offset(text->centerX, text->centerY);
		// shift to match alignment
		text->bounds.offset(-text->bounds.width()/2, 0);
	} else {
		text->bounds.offset(text->centerX - text->bounds.width()/2, text->centerY - text->bounds.height()/2);
	}
	
	//text->processed_for_test=1;
	// for text purposes
	drawTestBox( text, text->pathRotate, text->text, NULL/*paintText*/);
	boundIntersections.query_in_box(text->bounds, searchText);
	for (uint32_t i = 0; i < searchText.size(); i++) {
		TextDrawInfo* t = searchText.at(i);
		if (intersects(text, t)) {
			return true;
		}
	}
	if(text->minDistance > 0) {
		Sk_Rect boundsSearch = text->bounds;
		boundsSearch.inset(-ogl->getDensityValue(_max_(5.0f, text->minDistance)), -ogl->getDensityValue(15));
		boundIntersections.query_in_box(boundsSearch, searchText);
//		drawTestBox(cv, &boundsSearch, text->pathRotate, paintIcon, text->text, paintText);
		for (uint32_t i = 0; i < searchText.size(); i++) {
			TextDrawInfo* t = searchText.at(i);
			if (t->minDistance > 0 && t->text == text->text && intersects(boundsSearch, text->pathRotate,  t)) {
				return true;
			}
		}
	}

	boundIntersections.insert(text, text->bounds);

	return false;
}
#else
bool CMapRenderer::findTextIntersection(quad_tree<TextDrawInfo*>& boundIntersections, TextDrawInfo* text,
	Sk_Paint* paintText, Sk_Paint* paintIcon/*,int test*/) {
	paintText->measureText((Sk_Atlas*)ogl->atlas, text->text.c_str(), text->text.length(), &text->bounds);
	// make wider
	text->bounds.inset(-ogl->getDensityValue(3), -ogl->getDensityValue(10));



	bool display = calculatePathToRotate(text);
	if (!display) {
		return true;
	}

	if (text->path == NULL) {
		text->bounds.offset(text->centerX, text->centerY);
		// shift to match alignment
		text->bounds.offset(-text->bounds.width() / 2, 0);
	}
	else {
		text->bounds.offset(text->centerX - text->bounds.width() / 2, text->centerY - text->bounds.height() / 2);
	}

	//text->processed_for_test=1;
	// for text purposes
	drawTestBox(text,text->pathRotate, text->text, NULL/*paintText*/);
	
	boundIntersections.query_in_box(text->bounds, searchText);
	for (uint32_t i = 0; i < searchText.size(); i++) {
		TextDrawInfo* t = searchText.at(i);
		
			if (intersects(text, t))
				return true;
		
	}

	if (text->minDistance > 0) {
		Sk_Rect boundsSearch = text->bounds;
		boundsSearch.inset(-ogl->getDensityValue(_max_(5.0f, text->minDistance)), -ogl->getDensityValue(15));
		boundIntersections.query_in_box(boundsSearch, searchText);
		//		drawTestBox(cv, &boundsSearch, text->pathRotate, paintIcon, text->text, paintText);
		for (uint32_t i = 0; i < searchText.size(); i++) {
			TextDrawInfo* t = searchText.at(i);
			if (t->minDistance > 0 && t->text == text->text && intersects(boundsSearch, text->pathRotate, t)) {
				return true;
			}
		}
	}
	boundIntersections.insert(text, text->bounds);

	return false;
}

#endif


bool textOrder(TextDrawInfo* text1, TextDrawInfo* text2) {
	return text1->textOrder < text2->textOrder;
}

#if 0
bool isIntersect(Sk_Path *path1,Sk_Path *path2);  // in poly.cpp
static vector<Sk_Path *> _paths_;
bool _findTextIntersection(CMapRenderer *rr, Sk_Ogl *gc,TextDrawInfo* text)
{
	
	if(text->path==NULL)
		return false;
	bool rv=false;
	if(_paths_.size()==0)
	{
		_paths_.push_back(text->path);
	}
	else
	{
		for(int i=0;i<_paths_.size();i++)
		{
			Sk_Path *path=_paths_[i];
			if(isIntersect(text->path,path))
			{
				rv=true;
				break;
			}
		}
		if(!rv)
			_paths_.push_back(text->path);
	}

	return rv; // false - NO INtersection
}
#endif
#if 0
void CMapRenderer::drawTextOverCanvas_Test() 
{
	uint32_t size = this->textToDraw.size();
	for (uint32_t i = 0; i < size; i++) 
	{
		TextDrawInfo* text = this->textToDraw.at(i);
		Sk_Paint paintIcon;
		if(text->processed_for_test)
		{
		if(text->visible==1)
		{
			drawTestBox( text, text->pathRotate, text->text, 0xffff0000);
		}
		else if(text->visible==-1)
		{
			drawTestBox( text, text->pathRotate,  text->text, 0xff0000ff);
		}
		}
	}

}
#endif
#if NEW_APPROACH_WITH_MAP
void CMapRenderer::drawTextOverCanvas() {
	
	
		//Sk_Rect r = Sk_Rect::MakeLTRB(0, 0, ogl->getWidth(), ogl->getHeight());
	Sk_Rect r = Sk_Rect::MakeLTRB(0, 0, 2*ogl->getWidth(), 2*ogl->getHeight());
		r.inset(-100, -100);
		quad_tree<TextDrawInfo*> boundsIntersect(r, 4, 0.6);

		Sk_Paint paintIcon;

		Sk_Paint paintText;
	    paintIcon.setStyle(Sk_Paint::kStroke_Style);
		paintIcon.setStrokeWidth(1);
		paintIcon.setColor(0xff000000);
		
		
	
	// 1. Sort text using text order
	//std::sort(this->textToDraw.begin(), this->textToDraw.end(), textOrder);
	
		for (auto const &ent1 : this->textToDraw) {
			string street_name = ent1.first; //is the first key
			for (auto const &ent2 : ent1.second) {
				TextDrawInfo* text = (TextDrawInfo*)(ent2);
				
				//string s = "Lauderdale Drive";// "Big Hill Crescent"; //"Lauderdale Drive";  // "Mahogany Forest Drive" " Golden Forest"
				//string s = "Grand Trunk";   // bottle like road produces a lot of attempts to draw text when not neccessary !!! WHY ?? MUST BE FIXED!!
				string s = "Muret Crescent";
				//string s = "Major Mackenzie";
				if(text->text.find(s)!=string::npos)
				{
					
 					LOGI ("Golden Forest count=%d",text->path->len);
					
				}
				//if (text->path->len > 5)
				//	continue;
				if (text->text.length() > 0) {

					int count = 0;

					if (text->visible == 0)
					{
						//if(count==2)
						{
							// calculate if there is intersection
							bool intersects = findTextIntersection(boundsIntersect, text, &paintText, &paintIcon);
							if (!intersects)
								text->visible = 1;
							else
								text->visible = -1;
						}
					}
					//if(count==2&&text->processed_for_test==1)
					if (text->visible == 1)
					{
						count++;
						paintText.setTextSize(text->textSize);
						//paintText.setFakeBoldText(text->bold);
						paintText.setColor(text->textColor);

						if (text->drawOnPath && text->path != NULL) {
							if (text->textShadow > 0) {

							}
							DrawT::drawOnPath(ogl, text, &paintText);
						}
						else {
							/*if (text->shieldRes.length() > 0) {
							}*/
							drawWrappedText(text, &paintText);
						}

						//break;
					}
					
				}
				ogl->enableText(0);  // it's changed in drawWrappedText(text,&paintText);
				
			}//for (auto const &ent2 : ent1.second) 
		}

	
	
}
#else
void CMapRenderer::drawTextOverCanvas() {


	//Sk_Rect r = Sk_Rect::MakeLTRB(0, 0, ogl->getWidth(), ogl->getHeight());
	Sk_Rect r = Sk_Rect::MakeLTRB(0, 0, 2 * ogl->getWidth(), 2 * ogl->getHeight());
	r.inset(-100, -100);
	quad_tree<TextDrawInfo*> boundsIntersect(r, 4, 0.6);

	Sk_Paint paintIcon;

	Sk_Paint paintText;
	paintIcon.setStyle(Sk_Paint::kStroke_Style);
	paintIcon.setStrokeWidth(1);
	paintIcon.setColor(0xff000000);



	// 1. Sort text using text order
	std::sort(this->textToDraw.begin(), this->textToDraw.end(), textOrder);
	uint32_t size = this->textToDraw.size();
	//LOGI(">>>>drawTextOverCanvas<<<<< NTextsize=%d paths=%d\n", size,_paths_.size());
	for (uint32_t i = 0; i < size; i++) {
		TextDrawInfo* text = this->textToDraw.at(i);
		if (text->text.length() > 0) {

			int count = 0;
			// align center y
			//paintText.getFontMetrics(&fm);
			//text->centerY += (-10.2/*fm.fAscent*/);


			//string s="Maurier Boulevard";//"Lauderdale";  // "CorelAcres Drive"
			//if(text->text.find(s)!=string::npos)
			//{
			//	count=2;//count++;
			//	_sk_log ("lauderdale count=%d",count);
			//	/*if(count>=2)
			//		continue;*/
			//}

			//string s1= "Carrier Crescent";//"Rutherford";
			//if(text->text.find(s1)!=string::npos)
			//{
			//	count=2;
			//	_sk_log ("Coral Acres Drive count=%d",count);
			//	//continue;
			//}


			if (text->visible == 0)
			{
				//if(count==2)
				{
					// calculate if there is intersection
					bool intersects = findTextIntersection(boundsIntersect, text, &paintText, &paintIcon);
					if (!intersects)
						text->visible = 1;
					else
						text->visible = -1;
				}
			}
			//if(count==2&&text->processed_for_test==1)
			if (text->visible == 1)
			{
				count++;
				paintText.setTextSize(text->textSize);
				//paintText.setFakeBoldText(text->bold);
				paintText.setColor(text->textColor);

				if (text->drawOnPath && text->path != NULL) {
					if (text->textShadow > 0) {

					}
					DrawT::drawOnPath(ogl, text, &paintText);
				}
				else {
					/*if (text->shieldRes.length() > 0) {
					}*/
					drawWrappedText(text, &paintText);
				}
			}
		}
		ogl->enableText(0);  // it's changed in drawWrappedText(text,&paintText);
	}

}
#endif

