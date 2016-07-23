
#include "CMapRenderer.h"
#include "glCtx.h"

//#include <sstream>
//#include <string>  -> defined in common.h

#if LEAK_DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//#ifndef DBG_NEW      
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )      
#define new DBG_NEW   
//#endif
#endif

void loadRoutingData(BinaryMapFile* mf);
vector<string> initBinaryMapFile4Address(std::string inputName,FindAddress_T *addr);
/*
void testRenderingRuleStorage(const char* basePath, const char* name) {
	string filePath = string(basePath) + string(name);
	RenderingRulesStorage* st = new RenderingRulesStorage(filePath.c_str());
	st->parseRulesFromXmlInputStream(filePath.c_str(),
			new BasePathRenderingRulesStorageResolver(string(basePath)));
	st->printDebug(RenderingRulesStorage::TEXT_RULES);
	RenderingRuleSearchRequest* searchRequest = new RenderingRuleSearchRequest(st);
	searchRequest->setStringFilter(st->PROPS.R_TAG, "highway");
	searchRequest->setStringFilter(st->PROPS.R_VALUE, "motorway");
	searchRequest->setIntFilter(st->PROPS.R_LAYER, 1);
	searchRequest->setIntFilter(st->PROPS.R_MINZOOM, 15);
	searchRequest->setIntFilter(st->PROPS.R_MAXZOOM, 15);
	//	searchRequest.setBooleanFilter(storage.PROPS.R_NIGHT_MODE, true);
	// searchRequest.setBooleanFilter(storage.PROPS.get("hmRendered"), true);

	bool res = searchRequest->search(RenderingRulesStorage::LINE_RULES, true);
	printf("Result %d\n", res);
	searchRequest->printDebugResult();
}
*/

//-----------------------------------CMapRenderer-----------------------------

void CMapRenderer::getRenderingInfoFromCL(int argc, char **params) {
		Sk_float l1, l2;
		char s[100];
		int z, z1, z2 ;
		float cLat = 85;
		float tileHY = 2;
		float cLon = -180;
		float tileWX = 2;
		int zoom = 16;
		
		for (int i = 1; i != argc; ++i) {
			/*
			if (sscanf(params[i], "-renderingOutputFile=%s", s)) {
				tileFileName = s;
			} 
			else*/ 
			if (sscanf(params[i], "-imagesBasePathFile=%s", s)) {
				this->resourcesFileName = s;
			} 
			else if (sscanf(params[i], "-renderingStyleFile=%s", s)) {
				this->renderingFileName = s;
			} else if (sscanf(params[i], "-zoom=%d", &z)) {
				zoom = z;
			} else if (sscanf(params[i], "-lbox=%f,%f", &l1, &l2)) {//else if (sscanf(params[i], "-lbox=%lg,%lg", &l1, &l2)) {
				cLon/*lonleft*/ = l1;
				cLat/*lattop*/ = l2;
			} else if (sscanf(params[i], "-lt=%d,%d", &z1, &z2)) {
				tileWX = z1;
				tileHY = z2;
			}
		}

		ogl->updateRenderingInfoFromCL(cLat,cLon,zoom,tileWX,tileHY);
		
	}
void CMapRenderer::initFromCL(int _argc, char **_argv) {
	
	this->argc=_argc;
	this->argv =_argv;
	const char* f = argv[1];
	if (f[0] == '-') {
		// command
		
				this->getRenderingInfoFromCL(argc, argv);
				
				char s[100];
				for (int i = 1; i != argc; ++i) {
					if (sscanf(argv[i], "-renderingInputFile=%s", s)) {
						BinaryMapFile* mf = initBinaryMapFile(s);
						if(mf->routingIndexes.size()>0)
							this->bmf=mf;
												
					}
				}
				//runSimpleRendering(info->renderingFileName, info->imagesFileName, info);
				
//				
	} 
	initSearchRequest();

	
}
void CMapRenderer::initSearchRequest()
{
	this->rlst = new RenderingRulesStorage(this->renderingFileName.c_str());
	this->rlst->parseRulesFromXmlInputStream(this->renderingFileName.c_str(), NULL);
	this->req = new RenderingRuleSearchRequest(rlst);

}

void CMapRenderer::cleanUpCachedText()
{

	
#if NEW_APPROACH_WITH_MAP
	for (std::map<string, vector<TextDrawInfo*> >::const_iterator itr = textToDraw.begin(); itr != textToDraw.end(); ++itr)
	{
			 //result.push_back(itr->first);
		     //string v=itr->first;
		vector<TextDrawInfo*> v=itr->second;
		if(v.size()>0)
		{
			vector<TextDrawInfo*>::iterator it;
			for(it = v.begin(); it != v.end(); ++it)
				delete (*it);

			v.clear();
		}
	}
	textToDraw.clear();
#else
	std::vector<TextDrawInfo*>::iterator itTextToDraw;
	for(itTextToDraw = textToDraw.begin(); itTextToDraw != textToDraw.end(); ++itTextToDraw)
		delete (*itTextToDraw);

	textToDraw.clear();
#endif

	//we also need to clean up searchText;  // used to find text intersection
	searchText.clear();
	
}
void CMapRenderer::runSearchAddress(string str)
{
	string a1="",city="",province="";
	{
		vector<string> x;
		int rv = stdH::split(str, ',',x);
		if(rv==0)
			return;
		a1=x[0];
		if(rv>1)
			city=x[1];
		if(rv>2)
			province=x[2];
	}

	//======Init address to find
	FindAddress_T *addr = new FindAddress_T();
	/*addr->city="Vaughan";
	addr->street="Lauderdale";
	addr->building="180";*/

	toLowerCase(city);
	addr->city=stdH::trim(city);
	addr->building="";
	addr->street="";
	{
		vector<string> x;
		int rv = stdH::split(a1, ' ',x);
		
		a1=x[0];
		// make sure this is a building number
		bool flag=true;
		
		for(int i=0;i<a1.length();i++)
			if(a1[i]<0x30||a1[i]>0x39)
			{
				flag=false;
				break;
			}
        if(flag)
			addr->building=a1;

		if(rv>2)
			a1=x[1];
		if(rv>3)
			a1=a1+" "+x[2];

		toLowerCase(a1);
		addr->street=a1;
	}

	
#if SK_BUILD_FOR_ANDROID
	province="/mnt/sdcard/fcnavgl/maps/ontario.obf";
#else
	province="C:\\my\\M_Maps\\obf\\ontario.obf";
#endif
	vector<string> v = initBinaryMapFile4Address(province,addr);
	
	if(v.size()>0)
	{
		 // -> for test ONLY
		string s=v[0];
		v.push_back(s);
		v.push_back(s);
		
		std::stringstream ss;

		for(int i=0;i<v.size();i++)
		{
			string s1=v[i];
			ss<<s;
		}
		s=ss.str();

		LOGI("addr found_1 : msg =%s",s.c_str());
	}
	else
		LOGI("addr NOT found_1 : msg ");
}
void CMapRenderer::runPrepareMapObjects() {
	
	//runSearchAddress("180 Lauderdale Drive, Vaughan,On");
	
	ResultPublisher* publisher = new ResultPublisher();
	SearchQuery q(floorf(ogl->getLeft()), floorf(ogl->getRight()), ceilf(ogl->getTop()), ceilf(ogl->getBottom()), req, publisher);
	q.zoom = ogl->getZoom();

	ResultPublisher* res = searchObjectsForRendering(&q, true,2, "Nothing found");
		
	//rc->setDefaultIconsDir(pView->resourcesFileName);
	req->clearState();
	req->setIntFilter(rlst->PROPS.R_MINZOOM, ogl->getZoom());
	if (req->searchRenderingAttribute(A_DEFAULT_COLOR)) {
		//defaultMapColor = searchRequest->getIntPropertyValue(searchRequest->props()->R_ATTR_COLOR_VALUE);
	}
	req->clearState();
	req->setIntFilter(rlst->PROPS.R_MINZOOM, ogl->getZoom());
	if (req->searchRenderingAttribute(A_SHADOW_RENDERING)) {
		ogl->setShadowRenderingMode(req->getIntPropertyValue(req->props()->R_ATTR_INT_VALUE));
		//rc->setShadowRenderingColor(searchRequest->getIntPropertyValue(searchRequest->props()->R_SHADOW_COLOR));
	}
	/*rc->setLocation(
			((double)pView->left)/getPowZoom(31-pView->zoom),
			((double)pView->top)/getPowZoom(31-pView->zoom)
			);
	rc->setDimension(pView->width, pView->height);
	rc->setZoom(pView->zoom);
	rc->setRotate(0);
	rc->setDensityScale(1);
	
	
	rc->bounds.fBottom=pView->bottom;
	rc->bounds.fLeft=pView->left;
	rc->bounds.fRight=pView->right;
	rc->bounds.fTop=pView->top;*/

	ogl->setXYLocation();

	cleanUpCachedText(); // clean Up all previous text caches !!!!!!!!!!!!!!!!!!!

	if(req==NULL)
	{
		//fatal
		return;
	}
	doPrepareObjects4Rendering(res->result);
	
	delete publisher;
	//delete searchRequest;
	//delete st;

}
CMapRenderer::~CMapRenderer()
{

	/*if(this->bitmapData!=0)
		free(this->bitmapData);*/
	
	char s[100];
	for (int i = 1; i != argc; ++i) {
					if (sscanf(argv[i], "-renderingInputFile=%s", s)) {
						closeBinaryMapFile(s);
					}
				}
	
	if(texture)
			delete texture;
#if SK_BUILD_FOR_ANDROID
	if(aFile)
		aFile->Release();
#endif
	if(draw)
		delete draw;
	/*if(rc)
		delete rc;*/
	if(this->rlst)
		delete this->rlst;
	if(this->req)
		delete this->req;

}


#if SK_BUILD_FOR_ANDROID
void CMapRenderer::initTextures(AAssetManager* am)
{
	if(aFile==0)
	{
		aFile=new AndroidFileIO(am);
		aFile->Initialize();
		//LOGI("AndroidFileIO Init DONE!!! ");
	}
#else
void CMapRenderer::initTextures()
{
#endif
	if(texture)
	{
#if SK_BUILD_FOR_ANDROID
		///mnt/sdcard/fcnavgl/maps/ontario.obf"
		bool rv=texture->Load(":11.png");
		if(!rv)
		{
			sk_log(SK_ERROR,"COULD NOT INITIALIZE TEXTURE!!!\n");
		}
		//texture->Load(":marker.png");   // load file from assets folder
		//LOGI("Android texture->Load(DONE!!! ");
#else
		//texture->Load("C:\\my\\FCNavigation\\icons\\back_location.png");
		texture->Load("C:\\my\\FCNavigation\\icons\\22.png");

#endif
	}

}