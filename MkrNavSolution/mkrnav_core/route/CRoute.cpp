
#include "common.h"
#include <queue>
#include "binaryRead.h"
#include "Route.h"
#include <functional>

//#include "CMapRenderer.h"
#include "interface.h"

#if LEAK_DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//#ifndef DBG_NEW      
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )      
#define new DBG_NEW   
//#endif
//_crtBreakAlloc = 18;

#endif
static void insertPoint(RouteDataObject *o, int j,Sk_Ogl *ogl);

static vector<RouteSegmentResult> routeResult;

extern Sk_Interface *gInf; 

void addCfg(vector<ROUTE_TRIPLE> & cfg,int index,string tag,string val)
{
	ROUTE_TRIPLE val1=pair<int,tag_value>(index,tag_value(tag,val));
	cfg.push_back(val1);
}
void do_Route(float lat,float lon,float lat1,float lon1)
{
	vector<ROUTE_TRIPLE> cfg;
	// index 0 -> for highway
	addCfg(cfg,0,"motorway","100");
	addCfg(cfg,0,"motorway_link","80");

	addCfg(cfg,0,"trunk","100");
	addCfg(cfg,0,"trunk_link","75");

	addCfg(cfg,0,"primary","65");
	addCfg(cfg,0,"primary_link","50");

	addCfg(cfg,0,"secondary","60");
	addCfg(cfg,0,"secondary_link","50");

	addCfg(cfg,0,"tertiary","45");
	addCfg(cfg,0,"tertiary_link","40");

	addCfg(cfg,0,"residential","35");
	addCfg(cfg,0,"unclassified","35");
	addCfg(cfg,0,"road","35");

	addCfg(cfg,0,"service","30");
	addCfg(cfg,0,"track","15");


	//1 -> highwayPriorities
	addCfg(cfg,1,"highway$motorway","1.2");
	addCfg(cfg,1,"highway$motorway_link","1.2");
	addCfg(cfg,1,"highway$trunk","1.2");
	addCfg(cfg,1,"highway$trunk_link","1.2");

	addCfg(cfg,1,"highway$primary","1.05");
	addCfg(cfg,1,"highway$primary_link","1.05");

	addCfg(cfg,1,"highway$secondary","1.0");
	addCfg(cfg,1,"highway$secondary_link","1.0");
	addCfg(cfg,1,"highway$tertiary","0.95");
	addCfg(cfg,1,"highway$tertiary_link","0.95");
	addCfg(cfg,1,"highway$residential","0.7");
	addCfg(cfg,1,"highway$unclassified","0.7");
	addCfg(cfg,1,"highway$road","0.7");
	addCfg(cfg,1,"highway$service","0.5");
	addCfg(cfg,1,"highway$track","0.3");


	//2 -> avoid
	addCfg(cfg,2,"tracktype$grade5","0.6");
	addCfg(cfg,2,"access$private","0.3");
	addCfg(cfg,2,"motor_vehicle$private","0.3");
	addCfg(cfg,2,"toll$yes","1.0");

	//3 -> obstacles
	addCfg(cfg,3,"highway$traffic_signals","25");
	addCfg(cfg,3,"railway$crossing","25");
	addCfg(cfg,3,"railway$level_crossing","25");
	addCfg(cfg,3,"motorcar$no","-1");

	//4 -> routingObstacles ??
    addCfg(cfg,4,"highway$no","-1");

	//5 - attributes
	addCfg(cfg,5,"zoomToLoadTiles","16");
	addCfg(cfg,5,"planRoadDirection","0");
	addCfg(cfg,5,"recalculateDistanceHelp","10000");

	RoutingConfiguration config(cfg/*, initDirection*/);

	RoutingContext c(config);
	//c.progress = SHARED_PTR<RouteCalculationProgress>(new RouteCalculationProgressWrapper(ienv, progress));
	//data = ienv->GetIntArrayElements(coordinates, NULL);
	
	//    start_lat="43.84997990511598" start_lon="-79.50100243091583" target_lat="43.84325592844487" target_lon="-79.36448335647583">

	//double lon=-79.50100243091583,lat=43.84997990511598;   //start point
	//double lon1=-79.36448335647583,lat1=43.84325592844487;   //end point

	int px = MapUtils::get31TileNumberX(lon);
	int py = MapUtils::get31TileNumberY(lat);

	int px1 = MapUtils::get31TileNumberX(lon1);
	int py1 = MapUtils::get31TileNumberY(lat1);


	c.startX = px;
	c.startY = py;	
	c.endX = px1;
	c.endY = py1;
	
	routeResult.clear();
	

	routeResult = searchRouteInternal(&c, false);
	
}

static vector <Sk_Point > points;
static void insertPoint(RouteDataObject *o, int j,Sk_Ogl *ogl)
{
	if(o==0)
		return;
	int x=o->pointsX[j];
	int y=o->pointsY[j];
		
		float tx = x/ (ogl->tileDivisor);
		float ty = y / (ogl->tileDivisor);

		float dTileX = tx - ogl->getLeftX();
		float dTileY = ty - ogl->getTopY();
		Sk_Point p;
		p.fX = dTileX*TILE_SIZE; 
		p.fY = dTileY*TILE_SIZE;
		
		if (p.fX >= 0 && p.fX < 2*ogl->getWidth()&& p.fY >= 0 && p.fY < 2*ogl->getHeight())
			points.push_back(p);
}

vector<Sk_Point> *getPoints()
{
	
	int len=routeResult.size();
	if(len>0)
	{
		points.clear();

		for(int i=0;i<len;i++)
		{
			RouteSegmentResult *r=&routeResult[i];
			RouteDataObject *o=r->road.get();
			string s=o->getName();
#if !SK_BUILD_FOR_ANDROID 
			_sk_log ( "name:%s",s.c_str());
#else
			//LOGI("name:%s",s.c_str());
#endif
			if(gInf!=NULL)
			{
				Sk_Ogl *ogl=gInf->getOgl();
				int x=r->startPointIndex;
				int y=r->endPointIndex;
				if(x<=y)
					for(int j=x;j<=y;j++)
						insertPoint(o,j,ogl);
				else
					for(int j=x;j>=y;j--)
						insertPoint(o,j,ogl);
			}
		}
		return &points;
	}
	
	return 0;
}
static bool route_drawn=false;
void draw_Route(Sk_Ogl *ogl)
{
	/*if(route_drawn)
		return;*/
	int len;
	vector<Sk_Point> *p=getPoints();
	if(p==0||(len=p->size())==0)
	{
		//LOGI("draw_Route()..No Points\n");
		return;
	}
	ogl->setColor(0xff0000ff); // blue
	ogl->linewidth=8;

	//int len=p->size();
	Sk_Point3D *pnt=new Sk_Point3D[len];
	for(int i=0;i<len;i++)
	{
		pnt[i].fX=p->at(i).fX;
		pnt[i].fY=p->at(i).fY;
		pnt[i].fZ=ogl->getZLevel();
	}
	LOGI("draw_Route()..Points=%d\n",len);
	glEnableVertexAttribArray(ogl->position_location);
	ogl->draw_polyline (pnt,len);
	glDisableVertexAttribArray(ogl->position_location);
	delete pnt;
	//route_drawn=true;
}
void test_Route()
{
	double lon=-79.50100243091583,lat=43.84997990511598;   //start point
	double lon1=-79.36448335647583,lat1=43.84325592844487;   //end point
	do_Route( lat, lon, lat1, lon1);
}

#if SK_BUILD_FOR_ANDROID

//============================================================================================================
//http://stackoverflow.com/questions/15900594/retrieving-a-list-of-complex-objects-to-java-from-c-through-ndk
//http://stackoverflow.com/questions/11621449/send-c-string-to-java-via-jni
extern "C" JNIEXPORT jobjectArray JNICALL Java_com_mkr_navgl_nintf_MkrNavLib_getRoute(JNIEnv * env, jobject obj)
{
    LOGI("JNI GetRoute()..\n");
	int len=routeResult.size();

	jsize i = 0;
	jclass classString = env->FindClass("java/lang/String");

	jobjectArray jarray = env->NewObjectArray(len,  classString, NULL);

	 vector<RouteSegmentResult>::iterator it,b,e;
	 b = routeResult.begin();
	 e = routeResult.end();
	 for (it = b; it != e; ++it) {
	   RouteSegmentResult *rr=&(*it);
	   string str=rr->road->getName();
	  // string str="Hello1";
	   jstring tmp = env->NewStringUTF(str.c_str());
	   env->SetObjectArrayElement(jarray, i++, tmp);
	   env->DeleteLocalRef(tmp);
	 }
	return jarray;


}

#endif
