#ifndef CMAPRENDERER_MM
#define CMAPRENDERER_MM

#if !SK_BUILD_FOR_ANDROID
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#endif
#if SK_BUILD_FOR_ANDROID
#include "../main/AndroidFileIO.h"
#endif
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string>

#include "binaryRead.h"
#include "renderRules.h"

//#include <SkImageEncoder.h>
//#include <SkGraphics.h>
#include <stdio.h>
#include <time.h>
//#include "myrouting.h"

//#include "MapTileView.h"

#include "Sk_Paint.h"
#include "Sk_Draw.h"


class VerboseInfo {
public:
	bool vaddress;
	bool vtransport;
	bool vpoi;
	bool vmap;
	double lattop, latbottom, lonleft, lonright;
	int zoom;

	VerboseInfo(int argc, char **params) {
		lattop = 85;
		latbottom = -85;
		lonleft = -180;
		lonright = 180;
		zoom = 15;
		int z;
		double l1, l2, l3, l4;
		for (int i = 1; i != argc; ++i) {
			if (strcmp(params[i], "-vaddress") == 0) {
				vaddress = true;
			} else if (strcmp(params[i], "-vmap") == 0) {
				vmap = true;
			} else if (strcmp(params[i], "-vpoi") == 0) {
				vpoi = true;
			} else if (strcmp(params[i], "-vtransport") == 0) {
				vtransport = true;
			} else if (sscanf(params[i], "-zoom=%d", &z)) {
				zoom = z;
			} else if (sscanf(params[i], "-bbox=%le,%le,%le,%le", &l1, &l2, &l3, &l4)) {
				lonleft = l1;
				lattop = l2;
				lonright = l3;
				latbottom = l4;

			}
		}
	}
};

template <typename T> class quad_tree {
private :
	struct node {
        typedef std::vector<T> cont_t;
        cont_t data;
		node* children[4];
		Sk_Rect bounds;

		node(Sk_Rect& b) : bounds(b) {
            memset(children,0,4*sizeof(node*));
		}

		~node() {
			for (int i = 0; i < 4; i++) {
				if (children[i] != NULL) {
					delete children[i];
				}
			}
		}
	};
	typedef typename node::cont_t cont_t;
	typedef typename cont_t::iterator node_data_iterator;
	double ratio;
	unsigned int max_depth;
	node root;
public:
	quad_tree(Sk_Rect& r, int depth=8, double ratio = 0.55) : ratio(ratio), max_depth(depth), root(r) {
	}

    void insert(T data, Sk_Rect& box)
    {
        unsigned int depth=0;
        do_insert_data(data, box, &root, depth);
    }

    void query_in_box(Sk_Rect& box, std::vector<T>& result)
    {
        result.clear();
        query_node(box, result, &root);
    }

private:

    void query_node(Sk_Rect& box, std::vector<T> & result, node* node) const {
		if (node) {
			if (Sk_Rect::Intersects(box, node->bounds)) {
				node_data_iterator i = node->data.begin();
				node_data_iterator end = node->data.end();
				while (i != end) {
					result.push_back(*i);
					++i;
				}
				for (int k = 0; k < 4; ++k) {
					query_node(box, result, node->children[k]);
				}
			}
		}
	}


    void do_insert_data(T data, Sk_Rect& box, node * n, unsigned int& depth)
    {
        if (++depth >= max_depth) {
			n->data.push_back(data);
		} else {
			Sk_Rect& node_extent = n->bounds;
			Sk_Rect ext[4];
			split_box(node_extent, ext);
			for (int i = 0; i < 4; ++i) {
				if (ext[i].contains(box)) {
					if (!n->children[i]) {
						n->children[i] = new node(ext[i]);
					}
					do_insert_data(data, box, n->children[i], depth);
					return;
				}
			}
			n->data.push_back(data);
		}
    }
    void split_box(Sk_Rect& node_extent,Sk_Rect * ext)
    {
        //coord2d c=node_extent.center();

    	float width=node_extent.width();
    	float height=node_extent.height();

        float lox=node_extent.fLeft;
        float loy=node_extent.fTop;
        float hix=node_extent.fRight;
        float hiy=node_extent.fBottom;

        ext[0]=Sk_Rect::MakeLTRB(lox,loy,lox + width * ratio,loy + height * ratio);
        ext[1]=Sk_Rect::MakeLTRB(hix - width * ratio,loy,hix,loy + height * ratio);
        ext[2]=Sk_Rect::MakeLTRB(lox,hiy - height*ratio,lox + width * ratio,hiy);
        ext[3]=Sk_Rect::MakeLTRB(hix - width * ratio,hiy - height*ratio,hix,hiy);
    }
};

class Sk_Mutex
{
public:
	Sk_Mutex(){ pthread_mutex_init(&mutex, NULL);}
	~Sk_Mutex(){pthread_mutex_destroy(&mutex);}
    //! Locks mutex
	void Lock(){pthread_mutex_lock(&mutex);}
    //! Unlock mutex
	void Unlock(){pthread_mutex_unlock(&mutex);}
    //! Not used
    void UnlockQuasiFIFO(int usec=10)
	{
		pthread_mutex_unlock(&mutex);
#if SK_BUILD_FOR_ANDROID
		usleep(usec);
#else
		Sleep(usec);
#endif
	}

private:
    pthread_mutex_t mutex;
};

class Sk_Semaphore
{
public:	
	Sk_Semaphore(unsigned int v)
	{
		if(::sem_init(&_semaphore, 0,v))
		{
		}
	}
	~Sk_Semaphore()
	{
		 int error = ::sem_destroy (&_semaphore);
		 //assert (error == 0);
	}
	void wait ()
	{
		::sem_wait (&_semaphore);
	}


	bool tryWait ()
	{
		return sem_trywait (&_semaphore) == 0;
	}


	void post ()
	{
		if (::sem_post (&_semaphore))
		{
			//Iex::throwErrnoExc ("Post operation on semaphore failed (%T).");
		}
	}


	int value () const
	{
		int value;

		if (::sem_getvalue (&_semaphore, &value))
		{
			//Iex::throwErrnoExc ("Cannot read semaphore value (%T).");
		}
		return value;
	}

private:
	mutable sem_t _semaphore;
};

class CMapRenderer
{
	int argc;
	char **argv;
	
	std::string renderingFileName;
	std::string resourcesFileName;

	BinaryMapFile* bmf;
	RenderingRuleSearchRequest* req;
	RenderingRulesStorage* rlst;

	vector<TextDrawInfo*> searchText;  // used to find text intersection
	
	Sk_Ogl *ogl;

	// use to calculate points
	float calcX;
	float calcY;

	Sk_Texture *texture;
#if SK_BUILD_FOR_ANDROID
	AndroidFileIO *aFile;
#endif
private:
	
	
	void createBBox(TextDrawInfo* ti, int color);//	void drawTestBox(TextDrawInfo* i, float rot, std::string text, int color);
	void initSearchRequest();
	void getRenderingInfoFromCL(int argc, char **params);

	int calcPoint(std::pair<int, int>  c, int flag=0);
	int updatePaint( Sk_Paint* paint, int ind, int area);
	void renderText(MapDataObject* obj, std::string tag,std::string value,  Sk_Path* path,float xc=-1,float yc=-1);
	void drawPolyline(MapDataObject* mObj,  tag_value pair, int layer, int drawOnlyShadow);
	void drawPolygon(MapDataObject* mObj,  tag_value pair);
	void drawObject(MapDataObject* mObj, int l, int renderText, int drawOnlyShadow, int t);
	unordered_map<int, std::vector<int> > sortObjectsByProperOrder(std::vector <MapDataObject* > mapDataObjects);
	void doPrepareObjects4Rendering(std::vector <MapDataObject* > mapDataObjects);

	bool findTextIntersection(quad_tree<TextDrawInfo*>& boundIntersections, TextDrawInfo* text,
		Sk_Paint* paintText, Sk_Paint* paintIcon/*,int test=0*/);
	bool m_findTextIntersection( TextDrawInfo* text);
	bool calculatePathToRotate(TextDrawInfo* p) ;

	void fillTextProperties(TextDrawInfo* info);
	void drawWrappedText(TextDrawInfo* text, Sk_Paint *paintText,bool draw_on_texture=true );
	void drawTextOnCanvas(const char* text, uint16_t len, float centerX, float centerY, Sk_Paint *paintText,float textShadow);
public:
	
	//RenderingContext *rc;
	Sk_Draw *draw;

	//these I moved from Rendering context
#if NEW_APPROACH_WITH_MAP	
	std::map<string,vector<TextDrawInfo*> > textToDraw;
#else
	std::vector<TextDrawInfo*> textToDraw;
#endif
	std::vector<IconDrawInfo> iconsToDraw;

	CMapRenderer(Sk_Ogl *ogl)
	{	req=NULL;
		rlst=NULL;
		this->ogl=ogl;
		//rc=new RenderingContext();
		draw=new Sk_Draw(ogl);
#if SK_BUILD_FOR_ANDROID
	aFile=0;
#endif
		texture=new Sk_Texture();
	}
#if SK_BUILD_FOR_ANDROID	
	void initTextures(AAssetManager* am);
#else
	void initTextures();
#endif
	void initFromCL(int argc, char **argv); 
	void cleanUpCachedText();
	void runPrepareMapObjects();
	void drawTextOverCanvas() ;
	void drawTextOverCanvas_Test() ;
	void runSearchAddress(string s);

	void testTexture();

	~CMapRenderer();
};

typedef void (*cbThreadFunc_T)(void);

typedef struct  {
	
	int zoom;
   	float lon,lat;
	cbThreadFunc_T cb;
}RenderingThreadRequest_T;

typedef struct  {
	
	float lon1,lat1;
	float lon2,lat2;
	cbThreadFunc_T cb;
}RoutingThreadRequest_T;

typedef struct  {
	
	string address_request;
}SearchAddressThreadRequest_T;


void createRenderingThread(RenderingThreadRequest_T *arg);

#endif