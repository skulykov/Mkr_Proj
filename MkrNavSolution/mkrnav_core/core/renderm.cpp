
#include <math.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <set>
#include <time.h>

#include "common.h"
#include "renderRules.h"
#include "binaryRead.h"
#include "mapObjects.h"

#include "CMapRenderer.h"
#include "glCtx.h"

extern CMapRenderer *gRenderer;
extern graphics_gc_priv_t *pGraphicCtx;
extern bool ready_to_draw;
/*
#include <unistd.h>   // need for usleep
#include <pthread.h>
*/
#if LEAK_DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//#ifndef DBG_NEW      
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )      
#define new DBG_NEW   
//#endif
#endif



#pragma region GLES rendering
int CMapRenderer::calcPoint(std::pair<int, int>  c, int flag)
{
    
	int x=c.first;
	int y=c.second;
	/*if(flag)
		if(x < rc->bounds.fLeft || x > rc->bounds.fRight || y < rc->bounds.fTop || y > rc->bounds.fBottom)
			return 0;
*/
	
		

	float tx = x/ (ogl->tileDivisor);
	float ty = y / (ogl->tileDivisor);

    float dTileX = tx - ogl->getLeftX();
    float dTileY = ty - ogl->getTopY();
    this->calcX = dTileX*TILE_SIZE; //rc->cosRotateTileSize * dTileX - rc->sinRotateTileSize * dTileY;
    this->calcY = dTileY*TILE_SIZE;//rc->sinRotateTileSize * dTileX + rc->cosRotateTileSize * dTileY;

   // if (rc->calcX >= 0 && rc->calcX < rc->getWidth()&& rc->calcY >= 0 && rc->calcY < rc->getHeight())
   //     rc->pointInsideCount++;
	return 1;
}

#if 0
unordered_map<std::string, SkPathEffect*> pathEffects;
SkPathEffect* getDashEffect(std::string input)
{
    if(pathEffects.find(input) != pathEffects.end())
        return pathEffects[input];

    const char* chars = input.c_str();
    int i = 0;
    char fval[10];
    int flength = 0;
    float primFloats[20];
    int floatLen = 0;
    for(;;i++)
    {
        if(chars[i] == 0)
        {
            if(flength > 0)	{ fval[flength] = 0;
            primFloats[floatLen++] = atof(fval); flength = 0;}
            break;
        }
        else
        {
            if(chars[i] != '_')
            {
                // suppose it is a character
                fval[flength++] = chars[i];
            }
            else
            {
                if(flength > 0)
                {
                    fval[flength] = 0;
                    primFloats[floatLen++] = atof(fval); flength = 0;
                }
            }
        }
    }
    /*SkPathEffect* r = new SkDashPathEffect(primFloats, floatLen, 0);
    pathEffects[input] = r;*/
    return 0;//r;
}
#endif

int CMapRenderer::updatePaint( Sk_Paint* paint, int ind, int area)
{
    RenderingRuleProperty* rColor;
    RenderingRuleProperty* rStrokeW;
    RenderingRuleProperty* rCap;
    RenderingRuleProperty* rPathEff;
    if (ind == 0)
    {
        rColor = req->props()->R_COLOR;
        rStrokeW = req->props()->R_STROKE_WIDTH;
        rCap = req->props()->R_CAP;
        rPathEff = req->props()->R_PATH_EFFECT;
    }
    else if (ind == 1)
    {
        rColor = req->props()->R_COLOR_2;
        rStrokeW = req->props()->R_STROKE_WIDTH_2;
        rCap = req->props()->R_CAP_2;
        rPathEff = req->props()->R_PATH_EFFECT_2;
    }
    else if (ind == -1)
    {
        rColor = req->props()->R_COLOR_0;
        rStrokeW = req->props()->R_STROKE_WIDTH_0;
        rCap = req->props()->R_CAP_0;
        rPathEff = req->props()->R_PATH_EFFECT_0;
    }
    else if (ind == -2)
    {
        rColor = req->props()->R_COLOR__1;
        rStrokeW = req->props()->R_STROKE_WIDTH__1;
        rCap = req->props()->R_CAP__1;
        rPathEff = req->props()->R_PATH_EFFECT__1;
    }
    else
    {
        rColor = req->props()->R_COLOR_3;
        rStrokeW = req->props()->R_STROKE_WIDTH_3;
        rCap = req->props()->R_CAP_3;
        rPathEff = req->props()->R_PATH_EFFECT_3;
    }

    if (area)
    {
    	/*paint->setColorFilter(NULL);
    	paint->setShader(NULL);
    	paint->setLooper(NULL);
        paint->setStyle(SkPaint::kStrokeAndFill_Style);
        paint->setStrokeWidth(0);*/
		paint->setStyle(Sk_Paint::kStrokeAndFill_Style);
        paint->setStrokeWidth(1);
		
		//gc->linewidth=1;
    }
    else
    {
        float stroke = req->getFloatPropertyValue(rStrokeW);
        if (!(stroke > 0))
            return 0;
		
		//gc->linewidth=stroke;
		paint->setStyle(Sk_Paint::kStroke_Style);
        paint->setStrokeWidth(stroke);

		std::string cap = req->getStringPropertyValue(rCap);
        std::string pathEff = req->getStringPropertyValue(rPathEff);

        if (cap == "BUTT" || cap == "")
            paint->setStrokeCap(Sk_Paint::kButt_Cap);
        else if (cap == "ROUND")
            paint->setStrokeCap(Sk_Paint::kRound_Cap);
        else if (cap == "SQUARE")
            paint->setStrokeCap(Sk_Paint::kSquare_Cap);
        else
            paint->setStrokeCap(Sk_Paint::kButt_Cap);

        if (pathEff.size() > 0)
        {
           /* SkPathEffect* p = getDashEffect(pathEff);
            paint->setPathEffect(p);*/
        }
        else
        {
            /*paint->setPathEffect(NULL);*/
        }



      
    }

    int color = req->getIntPropertyValue(rColor);
    paint->setColor(color);
	

	/*gc->fg.a=1.0;;
	gc->fg.r=0.0;
	gc->fg.g=0.0;
	gc->fg.b=1.0;*/

    if (ind == 0)
    {
        std::string shader = req->getStringPropertyValue(req->props()->R_SHADER);
        if (shader.size() > 0)
        {
            /*SkBitmap* bmp = getCachedBitmap(rc, shader);
            if (bmp != NULL)
                paint->setShader(new SkBitmapProcShader(*bmp, SkShader::kRepeat_TileMode, SkShader::kRepeat_TileMode))->unref();*/
        }
    }

    // do not check shadow color here
    if (ogl->getShadowRenderingMode() == 1 && ind == 0)
    {
        int shadowColor = req->getIntPropertyValue(req->props()->R_SHADOW_COLOR);
        int shadowLayer = req->getIntPropertyValue(req->props()->R_SHADOW_RADIUS);
        if (shadowColor == 0) {
			shadowColor = ogl->getShadowRenderingColor();
		}
        if (shadowColor == 0)
            shadowLayer = 0;

       /* if (shadowLayer > 0)
            paint->setLooper(new SkBlurDrawLooper(shadowLayer, 0, 0, shadowColor))->unref();*/
    }
    return 1;
}

void CMapRenderer::renderText(MapDataObject* obj, std::string tag,std::string value,Sk_Path* path,float xc,float yc) {
	
	unordered_map<std::string, std::string>::iterator it = obj->objectNames.begin();
	while (it != obj->objectNames.end()) {
		bool insert = true;
		if (it->second.length() > 0) {
			std::string name = it->second;
			
			req->setInitialTagValueZoom(tag, value, ogl->getZoom(), obj);
			req->setIntFilter(req->props()->R_TEXT_LENGTH, name.length());
			std::string tagName = it->first == "name" ? "" : it->first;
			req->setStringFilter(req->props()->R_NAME_TAG, tagName);
			
			if (req->searchRule(RenderingRulesStorage::TEXT_RULES)	&& req->getIntPropertyValue(req->props()->R_TEXT_SIZE) > 0) 
			{
			//	LOGI("renderText; %s \n",name.c_str());
				TextDrawInfo* info = new TextDrawInfo(name);
				if (path != NULL)
				{   
					info->drawOnPath = (req->getIntPropertyValue(req->props()->R_TEXT_ON_PATH, 0) > 0);
					if (info->drawOnPath)
					{
						Sk_Path *newpath = new Sk_Path(path->len);
						memcpy(newpath->arr, path->arr, path->len*sizeof(Sk_Point3D));
						info->path = newpath;//NULL;//new SkPath(*path);
						//LOGI(">>>>Path inserted len=%d path=%x arr=%x\n", path->len,path,path->arr);
					}
					else
					{
						// problem with the highway badge when on streen intersection ( e.g. major Mackenzie and Durrefin)
						// so many street badges assigned to this place ( this is a probably map creation issue) !!!
						//try workaround 
						if (path->len > 6)
						{
							insert = false;   // ignore
						}
						else
						{
							Sk_Path *newpath = new Sk_Path(path->len);
							memcpy(newpath->arr, path->arr, path->len*sizeof(Sk_Point3D));
							info->path = newpath;//NULL;//new SkPath(*path);
						}
					}
				}
				else   // path==nNULL -> from polygon ( or pointer )
				{
					info->drawOnPath = false;
					info->centerX = xc;
					info->centerY = yc;

				}

				if (insert)
				{
					fillTextProperties(info);


#if NEW_APPROACH_WITH_MAP	
					if(this->textToDraw.count(info->text)==1)
					{
						vector<TextDrawInfo*> v=this->textToDraw[info->text];
						v.push_back(info);
						this->textToDraw[info->text]=v;
					}
					else
					{
						vector<TextDrawInfo*> v;
						v.push_back(info);
						this->textToDraw[info->text]=v;
					}
#else
					this->textToDraw.push_back(info);
#endif
				}
			}
		}

		it++;
	}

}


#if 0
void drawPolylineShadow(SkCanvas* cv, SkPaint* paint, RenderingContext* rc, SkPath* path, int shadowColor, int shadowRadius)
{
    // blurred shadows
    if (rc->getShadowRenderingMode() == 2 && shadowRadius > 0) {
        // simply draw shadow? difference from option 3 ?
        // paint->setColor(0xffffffff);
        paint->setLooper(new SkBlurDrawLooper(shadowRadius, 0, 0, shadowColor))->unref();
        PROFILE_NATIVE_OPERATION(rc, cv->drawPath(*path, *paint));
    }

    // option shadow = 3 with solid border
    if (rc->getShadowRenderingMode() == 3 && shadowRadius > 0) {
        paint->setLooper(NULL);
        paint->setStrokeWidth(paint->getStrokeWidth() + shadowRadius * 2);
        //		paint->setColor(0xffbababa);
        paint->setColorFilter(SkColorFilter::CreateModeFilter(shadowColor, SkXfermode::kSrcIn_Mode))->unref();
        //		paint->setColor(shadowColor);
        PROFILE_NATIVE_OPERATION(rc, cv->drawPath(*path, *paint));
    }
}


//std::vector<SkPaint> oneWayPaints;
//std::vector<SkPaint> reverseWayPaints;
//SkPaint* oneWayPaint(){
//    SkPaint* oneWay = new SkPaint;
//    oneWay->setStyle(SkPaint::kStroke_Style);
//    oneWay->setColor(0xff6c70d5);
//    oneWay->setAntiAlias(true);
//    return oneWay;
//}
void drawOneWayPaints(RenderingContext* rc, SkCanvas* cv, SkPath* p, int oneway) {
	/*if (oneWayPaints.size() == 0) {
        const float intervals_oneway[4][4] = {
            {0, 12, 10, 152},
            {0, 12, 9, 153},
            {0, 18, 2, 154},
            {0, 18, 1, 155}
        };
		SkPathEffect* arrowDashEffect1 = new SkDashPathEffect(intervals_oneway[0], 4, 0);
		SkPathEffect* arrowDashEffect2 = new SkDashPathEffect(intervals_oneway[1], 4, 1);
		SkPathEffect* arrowDashEffect3 = new SkDashPathEffect(intervals_oneway[2], 4, 1);
		SkPathEffect* arrowDashEffect4 = new SkDashPathEffect(intervals_oneway[3], 4, 1);

		SkPaint* p = oneWayPaint();
		p->setStrokeWidth(1);
		p->setPathEffect(arrowDashEffect1)->unref();
		oneWayPaints.push_back(*p);
		delete p;

		p = oneWayPaint();
		p->setStrokeWidth(2);
		p->setPathEffect(arrowDashEffect2)->unref();
		oneWayPaints.push_back(*p);
		delete p;

		p = oneWayPaint();
		p->setStrokeWidth(3);
		p->setPathEffect(arrowDashEffect3)->unref();
		oneWayPaints.push_back(*p);
		delete p;

		p = oneWayPaint();
		p->setStrokeWidth(4);
		p->setPathEffect(arrowDashEffect4)->unref();
		oneWayPaints.push_back(*p);
		delete p;
	}
	if (reverseWayPaints.size() == 0) {
            const float intervals_reverse[4][4] = {
                {0, 12, 10, 152},
                {0, 13, 9, 152},
                {0, 14, 2, 158},
                {0, 15, 1, 158}
            };
		SkPathEffect* arrowDashEffect1 = new SkDashPathEffect(intervals_reverse[0], 4, 0);
		SkPathEffect* arrowDashEffect2 = new SkDashPathEffect(intervals_reverse[1], 4, 1);
		SkPathEffect* arrowDashEffect3 = new SkDashPathEffect(intervals_reverse[2], 4, 1);
		SkPathEffect* arrowDashEffect4 = new SkDashPathEffect(intervals_reverse[3], 4, 1);
		SkPaint* p = oneWayPaint();
		p->setStrokeWidth(1);
		p->setPathEffect(arrowDashEffect1)->unref();
		reverseWayPaints.push_back(*p);
		delete p;

		p = oneWayPaint();
		p->setStrokeWidth(2);
		p->setPathEffect(arrowDashEffect2)->unref();
		reverseWayPaints.push_back(*p);
		delete p;

		p = oneWayPaint();
		p->setStrokeWidth(3);
		p->setPathEffect(arrowDashEffect3)->unref();
		reverseWayPaints.push_back(*p);
		delete p;

		p = oneWayPaint();
		p->setStrokeWidth(4);
		p->setPathEffect(arrowDashEffect4)->unref();
		reverseWayPaints.push_back(*p);
		delete p;
	}
	if (oneway > 0) {
		for (size_t i = 0; i < oneWayPaints.size(); i++) {
			PROFILE_NATIVE_OPERATION(rc, cv->drawPath(*p, oneWayPaints.at(i)));
		}
	} else {
		for (size_t i = 0; i < reverseWayPaints.size(); i++) {
			PROFILE_NATIVE_OPERATION(rc, cv->drawPath(*p, reverseWayPaints.at(i)));
		}
	}*/
}
#endif


void CMapRenderer::drawPolyline(MapDataObject* mObj,  tag_value pair, int layer, int drawOnlyShadow) {
	size_t length = mObj->points.size();
	if (length < 2) {
		return;
	}
	std::string tag = pair.first;
	std::string value = pair.second;

	
	req->setInitialTagValueZoom(tag, value, ogl->getZoom(), mObj);
	req->setIntFilter(req->props()->R_LAYER, layer);
	bool rendered = req->searchRule(2);
	
	if (!rendered || !updatePaint((Sk_Paint*)this->ogl->paint, 0, 0)) {
		return;
	}
	
	int oneway = 0;
	if (ogl->getZoom() >= 16 && pair.first == "highway") {
		if (mObj->containsAdditional("oneway", "yes")) {
			oneway = 1;
		} else if (mObj->containsAdditional("oneway", "-1")) {
			oneway = -1;
		}
	}


	//Sk_Point *p=new Sk_Point[length];  // maybe it's faster not to use new allocation
	Sk_Path *path=new Sk_Path(length);
		
	int i=0;
	
	//int len=0;
	Sk_Point3D *p=path->arr;
	for (; i < length; i++) {
		/*if(!calcPoint(mObj->points.at(i), rc))
			continue;*/
		calcPoint(mObj->points.at(i));
		p[i].fX=this->calcX;
		p[i].fY= this->calcY;
		p[i].fZ=ogl->getZLevel();
		//len++;
		
	}
	//======================
	/*
	bool found = false;
	unordered_map< std::string, std::string >::const_iterator itr;
	for (itr = mObj->objectNames.begin(); itr != mObj->objectNames.end(); ++itr){
		//cout << "Key: " << (*itr).first << " Value: " << (*itr).second;
		string name = (*itr).second;
		string s = "Golden Forest";//"Keele";//
		if (name.find(s) != string::npos)
		{
			found = true;
		}
	}*/
	//======================

#if 1
	if (i > 0) {
		if (drawOnlyShadow) {
			int shadowColor = req->getIntPropertyValue(req->props()->R_SHADOW_COLOR);
			int shadowRadius = req->getIntPropertyValue(req->props()->R_SHADOW_RADIUS);
			if(shadowColor == 0) {
				shadowColor = ogl->getShadowRenderingColor();
			}
			((Sk_Paint*)ogl->paint)->setStrokeWidth(((Sk_Paint*)ogl->paint)->getStrokeWidth()+shadowRadius * 2);
			((Sk_Paint*)ogl->paint)->setColor(shadowColor);
			
			this->draw->drawPath( path,false,true);   // 1. draw shadow lines

		} 
		else 
		{
			//if (updatePaint(req, (Sk_Paint*)gc->paint, -2, 0, rc)) {
			//	rr->draw.drawPath(gc,path);
			//}
			//if (updatePaint(req, (Sk_Paint*)gc->paint, -1, 0, rc)) {
			//	rr->draw.drawPath(gc,path);
			//}
			//if (updatePaint(req, (Sk_Paint*)gc->paint, 0, 0, rc)) {
			//	//gc->linewidth=20;
			//	rr->draw.drawPath(gc,path);
			//}
			// do we need this???//draw_lines_cached( gc, p, len);//PROFILE_NATIVE_OPERATION(rc, cv->drawPath(path, *paint));
			this->draw->drawPath(path,oneway==1?true:false);
			/*if (updatePaint(req, (Sk_Paint*)gc->paint, 1, 0, rc)) {
				rr->draw.drawPath(gc,path);
			}
			if (updatePaint(req, (Sk_Paint*)gc->paint, 2, 0, rc)) {
				rr->draw.drawPath(gc,path);
			}*/
			//if (oneway && !drawOnlyShadow) {
			//	//drawOneWayPaints(rc, cv, &path, oneway);
			//}
		
				this->renderText(mObj, pair.first, pair.second, path);
				//IMPORTANT!!! The points cleanup will be done after text rendering
				// Double check with this
			
			
		}
	}
#endif

	//delete[] p;
	//delete path;
}


void CMapRenderer::drawPolygon(MapDataObject* mObj, tag_value pair) {
	
	
	size_t length = mObj->points.size();
	if (length <= 2) {
		return;
	}
	
	std::string tag = pair.first;
	std::string value = pair.second;
	
	//int flag=0;
	//if(length==15)
	//{
	//	if(value.find("park")!=std::string::npos)
	//	{
	//		flag=1;//return;
	//	}
	//}


	req->setInitialTagValueZoom(tag, value, ogl->getZoom(), mObj);
	bool rendered = req->searchRule(3);

	float xText = 0;
	float yText = 0;
	
	if (!rendered || !updatePaint( (Sk_Paint*)ogl->paint, 0, 1)) {
		return;
	}

	Sk_Path *path=new Sk_Path(length); 
	int len=0;
	for (int i=0; i < length; i++) {
		if(!calcPoint(mObj->points.at(i), 0))
			continue;
		//calcPoint(mObj->points.at(i), rc);
		path->arr[len].fX=this->calcX;
		path->arr[len].fY= this->calcY;
		path->arr[len].fZ=ogl->getZLevel();   // for GL Depth test
		xText += this->calcX;
		yText += this->calcY;

		len++;
	}
	
    if(len==5)
	{
		//this is a special case when poly is the whole area
		if(path->arr[1].fX==ogl->getWidth() && path->arr[2].fY==ogl->getHeight())
		{
			delete  path;
			return;
		}
	}
	path->len=len;
//	if(mObj->area)
	int color=((Sk_Paint*)ogl->paint)->getColor()&0x00ffffff;
	
	//if(color==0x100000)
	//{
	//	//wetland, mud, etc
	//	return;
	//}
	if(color==0xffffff)  // // TODO ???? find out================================
	{
		if(value.find("park")!=std::string::npos)
		{
			int x=color;
			color=0x9DB487;//x&0xff;
			((Sk_Paint*)ogl->paint)->setColor(color);
		}
		
	}
	 this->draw->drawPathPoly(path);						// 1. we fill polygon
	
	if(updatePaint((Sk_Paint*)ogl->paint, 1, 0))
		this->draw->drawPath(path,false);				//2. then we draw polygon boundaries

	if(path->ref==0)
		delete path;

	//TODO need to improve this 
	renderText(mObj, pair.first, pair.second, NULL, xText / len, yText / len);
}

void CMapRenderer::drawObject(MapDataObject* mObj, int l, int renderText, int drawOnlyShadow, int t) {
	
	
	tag_value pair = mObj->types.at(l);
	if (t == 1 && !drawOnlyShadow) {
		// point
		//drawPoint(mObj, req, gc/*cv, paint,*/ ,rc, pair, renderText);
	} else if (t == 2) {
		drawPolyline(mObj, pair, mObj->getSimpleLayer(), drawOnlyShadow);
	} else if (t == 3 && !drawOnlyShadow) {
		// polygon
		drawPolygon(mObj, pair);
	}
}

#if 0
void drawIconsOverCanvas(RenderingContext* rc, /* SkCanvas* canvas*/ Sk_Ogl *gc)
{
	int skewConstant = (int) rc->getDensityValue(16);
	int iconsW = rc -> getWidth() / skewConstant;
	int iconsH = rc -> getHeight() / skewConstant;
	int len = (iconsW * iconsH) / 32;
	//svk int alreadyDrawnIcons[len];
	int *alreadyDrawnIcons = new int[len];
	memset(alreadyDrawnIcons, 0, sizeof(int)*len);
	size_t ji = 0;
	SkPaint p;
	p.setStyle(SkPaint::kStroke_Style);
	p.setFilterBitmap(true);
	for(;ji< rc->iconsToDraw.size(); ji++)
	{
		IconDrawInfo icon = rc->iconsToDraw.at(ji);
		if (icon.y >= 0 && icon.y < rc->getHeight() && icon.x >= 0 && icon.x < rc->getWidth() && icon.bmp != NULL) {
			int z = (((int) icon.x / skewConstant) + ((int) icon.y / skewConstant) * iconsW);
			int i = z / 32;
			if (i >= len) {
				continue;
			}
			int ind = alreadyDrawnIcons[i];
			int b = z % 32;
			// check bit b if it is set
			if (((ind >> b) & 1) == 0) {
				alreadyDrawnIcons[i] = ind | (1 << b);
				SkBitmap* ico = icon.bmp;
				float left = icon.x - rc->getDensityValue(ico->width() / 2);
				float top = icon.y - rc->getDensityValue(ico->height() / 2);
				SkRect r = SkRect::MakeXYWH(left, top, rc->getDensityValue(ico->width()),
						rc->getDensityValue(ico->height()));
				//PROFILE_NATIVE_OPERATION(rc, canvas->drawBitmapRect(*ico, (SkIRect*) NULL, r, &p));
			}
		}
		if (rc->interrupted()) {
			delete [] alreadyDrawnIcons; //svk
			return;
		}
	}
	delete [] alreadyDrawnIcons; //svk
}
#endif

unordered_map<int, std::vector<int> > CMapRenderer::sortObjectsByProperOrder(std::vector <MapDataObject* > mapDataObjects)
{
	unordered_map<int, std::vector<int> > orderMap;
	
	if (req != NULL) {
		req->clearState();
		const int size = mapDataObjects.size();
		int i = 0;
		for (; i < size; i++) {
			uint32_t sh = i << 8;
			MapDataObject* mobj = mapDataObjects[i];
			size_t sizeTypes = mobj->types.size();
			size_t j = 0;
			for (; j < sizeTypes; j++) {
				int layer = mobj->getSimpleLayer();
				tag_value pair = mobj->types[j];
				req->setTagValueZoomLayer(pair.first, pair.second, ogl->getZoom(), layer, mobj);
				req->setIntFilter(req->props()->R_AREA, mobj->area);
				req->setIntFilter(req->props()->R_POINT, mobj->points.size() == 1);
				req->setIntFilter(req->props()->R_CYCLE, mobj->cycle());
				if (req->searchRule(RenderingRulesStorage::ORDER_RULES)) {
					int objectType = req->getIntPropertyValue(req->props()->R_OBJECT_TYPE);
					int order = req->getIntPropertyValue(req->props()->R_ORDER);
					orderMap[(order << 2)|objectType].push_back(sh + j);
					// polygon
					if(objectType == 3) {
						// add icon point all the time
						orderMap[(128 << 2)|1].push_back(sh + j);
					}
					if (req->getIntPropertyValue(req->props()->R_SHADOW_LEVEL) > 0) {
						//rc->shadowLevelMin = std::min(rc->shadowLevelMin, order);
						//rc->shadowLevelMax = std::max(rc->shadowLevelMax, order);
						//svk  std::max problem
						ogl->shadowLevelMin = min(ogl->shadowLevelMin, order);
						ogl->shadowLevelMax = max(ogl->shadowLevelMax, order);
						req->clearIntvalue(req->props()->R_SHADOW_LEVEL);
					}
				}

			}
		}
	}
	return orderMap;
}

void CMapRenderer::doPrepareObjects4Rendering(std::vector <MapDataObject* > mapDataObjects)
{
	 
	 ogl->paint=new Sk_Paint();
	LOGI("doRendering..objSize=%d\n",mapDataObjects.size());

	CachedObjects_T *obj=this->draw->getObj4Prepare();
	if(obj==0)
	{
		LOGI("doRendering..COULD NOT find Obj\n");
		return;
	}
	obj->prepare();
	this->draw->clearCachedGraphics(obj);


	// put in order map
	unordered_map<int, std::vector<int> > orderMap = sortObjectsByProperOrder(mapDataObjects);
	std::set<int> keys;
	unordered_map<int, std::vector<int> >::iterator it = orderMap.begin();

	while (it != orderMap.end()) {
		keys.insert(it->first);
		it++;
	}
	bool shadowDrawn = false;
	for (std::set<int>::iterator ks = keys.begin(); ks != keys.end(); ks++) {
		if (!shadowDrawn && ((*ks)>>2) >= ogl->shadowLevelMin && ((*ks)>>2) <= ogl->shadowLevelMax && ogl->getShadowRenderingMode() > 1) 
		{
			for (std::set<int>::iterator ki = ks; ki != keys.end(); ki++) {
				if (((*ki)>>2) > ogl->shadowLevelMax ) {
					break;
				}
				std::vector<int> list = orderMap[*ki];
				for (std::vector<int>::iterator ls = list.begin(); ls != list.end(); ls++) {
					int i = *ls;
					int ind = i >> 8;
					int l = i & 0xff;
					MapDataObject* mapObject = mapDataObjects.at(ind);

					// show text only for main type

					drawObject( mapObject,  l, l == 0, true, (*ki)&3);
				}
			}
			shadowDrawn = true;
		}

		std::vector<int> list = orderMap[*ks];
		for (std::vector<int>::iterator ls = list.begin(); ls != list.end(); ls++) {
			int i = *ls;
			int ind = i >> 8;
			int l = i & 0xff;

			MapDataObject* mapObject = mapDataObjects.at(ind);
			// show text only for main type
			drawObject(mapObject, l, l == 0, false, (*ks)&3);
		}
		ogl->lastRenderedKey = (*ks) >>2;
		

	}

	delete ogl->paint;
	obj->state=1;      // Map Objects for rendering have been prepared
}
#pragma endregion


#if USE_NATIVE_TH_FOR_RENDERING
#pragma region Rendering Threading 
//http://stackoverflow.com/questions/9733331/thread-synchronization-native-android
//http://stackoverflow.com/questions/6144496/android-ndk-mutex
pthread_mutex_t mutexlock;


static bool thread_running=false;
void invokeRender2();
void* threadProc(void* _arg)
{
     RenderingThreadRequest_T * arg = (RenderingThreadRequest_T*) _arg;
	/*while(1)
	{
	
		LOGE("Inside threadProc... \n");

		usleep(800*1000);
	}
    */
	 thread_running=true;

	 while(ready_to_draw){usleep(10*1000);};  // wait until we finish drawing in Draw() -> MUST be mutex here

	 MapTileView* pmap=arg->rr->getMapView();
	if(arg->lat!=0 && arg->lon!=0 && arg->zoom!=0)
		pmap->updateZoom(arg->zoom);//pmap->updateLocation(lat,lon,zoom);
	LOGI("Zoom to  lat=%f lon=%f zoom=%d\n",arg->lat,arg->lon,arg->zoom); 
	//if(first==0)
	
	arg->rr->runSimpleRendering(arg->gc);
	ready_to_draw=true;
	delete arg;
	thread_running=false;
	LOGE("Finish threadProc... \n");
	//invokeRender2();
    return 0;
}

void createRenderingThread(RenderingThreadRequest_T *arg)
{    
	
	
	pthread_t thread = 0;
	

    if(pthread_create(&thread, NULL, threadProc, arg) != 0)
	{
		LOGE("ERROR threadProc... \n");
		delete arg;
	}
  
}
bool isRenderingThreadRunning()
{
		return thread_running;
}

//int first=0;
void requestRendering (float lat,float lon,int zoom )
{
 
  //if(first==0)
	
	
  //if(prev_zoom<zoom)
  //{
	 // scale+=0.2;
  //}else if(prev_zoom>zoom)
  //{
	 // scale-=0.2;
	 // if(scale<1.0)
	 // {
		//  //tx-=0.2;
		//  //ty-=0.2;
	 // }
  //}
  //prev_zoom=zoom;

 
	
	if(!isRenderingThreadRunning())
	{
		RenderingThreadRequest_T *arg=new RenderingThreadRequest_T;
		arg->rr=gRenderer;
		arg->lat=lat;
		arg->lon=lon;
		arg->zoom=zoom;
		arg->gc=pGraphicCtx;
		createRenderingThread(arg);

		//LOGI("=====Go and Run Thread=====\n");
	}
  //first=1;
  //glFinish();//glFlush();
}
#pragma endregion
#endif  //USE_NATIVE_TH_FOR_RENDERING

