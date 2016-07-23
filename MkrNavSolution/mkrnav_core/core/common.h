#ifndef _FCNAV_COMMON_H
#define _FCNAV_COMMON_H

#include <string>
#include <vector>
#include <sstream>
//#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#if defined(ANDROID)
#	include <unordered_map>
#	include <unordered_set>
#undef _WIN32
#elif defined(__APPLE__)
#	include <tr1/unordered_map>
#	include <tr1/unordered_set>
#else
#	include <unordered_map>
#	include <unordered_set>
#endif
#include <stdint.h>

#if defined(__APPLE__)
#	include <mach/mach_time.h>|
#endif

#include "glCtx.h"
#include "mySharedPtr.h"

#define NEW_APPROACH_WITH_MAP 0
#define USE_OLD_TEXT_LOCATOR 0

// M_PI is no longer part of math.h/cmath by standart, but some GCC's define them
#define _USE_MATH_DEFINES
#include <math.h>
#if !defined(M_PI)
	const double M_PI = 3.14159265358979323846;
#endif
#if !defined(M_PI_2)
	const double M_PI_2 = M_PI / 2.0;
#endif

#define toLowerCase(s) std::transform(s.begin(), s.end(), s.begin(), ::tolower)

// Wrapper for unordered classes
#if defined(ANDROID)
//#	define UNORDERED_NAMESPACE std::tr1
#	define UNORDERED_map unordered_map
#	define UNORDERED_set unordered_set
#elif defined(__APPLE__)
#	define UNORDERED_NAMESPACE std::tr1
#	define UNORDERED_map unordered_map
#	define UNORDERED_set unordered_set
#else
#	define UNORDERED_NAMESPACE std
#	define UNORDERED_map unordered_map
#	define UNORDERED_set unordered_set
#endif
//#define UNORDERED(cls) UNORDERED_NAMESPACE::UNORDERED_##cls
#if defined(ANDROID)
//#include <memory>

//#include "shared_ptr.h"
//#define SHARED_PTR my_shared_ptr
//#   include <tr1/shared_ptr.h>
#define SHARED_PTR MySharedPtr

#else
//	#   define SHARED_PTR
//#define SHARED_PTR std::tr1::shared_ptr
#define SHARED_PTR MySharedPtr
#endif
// Better don't do this
using namespace std;


#pragma region COMMON

#ifdef PROFILE_NATIVE_OPERATIONS
	#define PROFILE_NATIVE_OPERATION(rc, op) rc->nativeOperations.pause(); op; rc->nativeOperations.start()
#else
	#define PROFILE_NATIVE_OPERATION(rc, op) op;
#endif

struct RenderingContext;

inline Sk_float toRadians(Sk_float angdeg) {
	return angdeg / 180 * M_PI;
}
#if 1
class ElapsedTimer
{
private:
	long elapsedTime;
	bool enableFlag;
	bool run;

#if defined(_WIN32)
	DWORD startInit;
	DWORD endInit;
#elif defined(__APPLE__)
	mach_timebase_info_data_t machTimeInfo;
	uint64_t startInit;
	uint64_t endInit;
#else
	timespec startInit;
	timespec endInit;
#endif
	
public:
	ElapsedTimer();

	void enable();
	void disable();

	void start();
	void pause();

	int getElapsedTime();
};
#endif
struct TextLocator
{
	int startIndex, endIndex;
	float roadSegmentlen;
};
struct TextDrawInfo {
	TextDrawInfo(std::string);
	~TextDrawInfo();

	std::string text;

	Sk_Rect bounds;
	Sk_Point3D bbox[4];  // bounding box to where the text must be placed
	float centerX;
	float centerY;
	int startIndex , endIndex ;
	float textSize;
	float minDistance;
	int textColor;
	int textShadow;
	uint32_t textWrap;
	bool bold;
	//std::string shieldRes;
	int textOrder;

	bool drawOnPath;
	Sk_Path *path;
	float pathRotate;
	//float vOffset;
	//float hOffset;
	float openSpace;
	short processed_for_test;
	short visible;
	
};

struct IconDrawInfo
{
	IconDrawInfo();

	void *bmp;//SkBitmap* bmp;
	float x;
	float y;
};

static const float TILE_SIZE = 256.0;

/*
if (RectA.X1 < RectB.X2 && RectA.X2 > RectB.X1 &&
    RectA.Y1 < RectB.Y2 && RectA.Y2 > RectB.Y1) 

	bool Rectangle::intersects(Rectangle r) {
  return
    (x1 < r.x2
  && x2 > r.x1
  && y1 < r.y2
  && y2 > r.y1);
}
*/
#if 0
struct RenderingContext
{
private :
	// parameters
	bool useEnglishNames;
	float density;

	float leftX;
	float topY;
	int width;
	int height;
	int defaultColor;

	int zoom;
	float rotate;
	// int shadowRenderingMode = 0; // no shadow (minumum CPU)
	// int shadowRenderingMode = 1; // classic shadow (the implementaton in master)
	// int shadowRenderingMode = 2; // blur shadow (most CPU, but still reasonable)
	// int shadowRenderingMode = 3; solid border (CPU use like classic version or even smaller)
	int shadowRenderingMode;
	int shadowRenderingColor;
	string defaultIconsDir;

public:
	// debug purpose
	int pointCount;
	int pointInsideCount;
	int visible;
	int allObjects;
	int lastRenderedKey;
//	class ElapsedTimer textRendering;
//	class ElapsedTimer nativeOperations;

// because they used in 3rd party functions
public :

	Sk_Rect bounds;
	// calculated
	float tileDivisor;
	float cosRotateTileSize;
	float sinRotateTileSize;

//	std::vector<TextDrawInfo*> textToDraw;
//	std::vector<IconDrawInfo> iconsToDraw;
	// use to calculate points
	float calcX;
	float calcY;

	// not expect any shadow
	int shadowLevelMin;
	int shadowLevelMax;

public:
	RenderingContext()
	{
		setRotate(0);
		setZoom(15);
		setDefaultColor(0xfff1eee8);
	}
	~RenderingContext(){};
	/*RenderingContext() : shadowLevelMax(0), shadowLevelMin(256), density(true), useEnglishNames(false), pointCount(0),
		pointInsideCount(0), visible(0), allObjects(0), shadowRenderingColor(0xff969696) {
		setRotate(0);
		setZoom(15);
		setDefaultColor(0xfff1eee8);
	}
	virtual ~RenderingContext();*/

	
	void setDefaultIconsDir(string path) {
		defaultIconsDir = path;
	}

	void setZoom(int z) {
		this->zoom = z;
		this->tileDivisor = (1 << (31 - z));
	}

	void setDefaultColor(int z) {
		this->defaultColor = z;
	}

	void setRotate(float rot) {
		this->rotate = rot;
		this->cosRotateTileSize = cos(toRadians(rot)) * TILE_SIZE;
		this->sinRotateTileSize = sin(toRadians(rot)) * TILE_SIZE;
	}

	void setLocation(double leftX, double topY) {
		this->leftX = leftX;
		this->topY = topY;
	}

	void setDimension(int width, int height) {
		this->width = width;
		this->height = height;
	}

	inline int getShadowRenderingMode(){
		return shadowRenderingMode;
	}

	int getShadowRenderingColor(){
		return shadowRenderingColor;
	}

	void setShadowRenderingColor(int color) {
		shadowRenderingColor = color;
	}

	inline int getWidth(){
		return width;
	}

	inline int getDefaultColor(){
		return defaultColor;
	}

	inline int getHeight(){
		return height;
	}

	inline int getZoom() {
		return zoom;
	}

	inline float getLeft() {
		return leftX;
	}

	inline float getTop() {
		return topY;
	}

	void setShadowRenderingMode(int mode){
		this->shadowRenderingMode = mode;
	}

	void setDensityScale(float val) {
		density = val;
	}

	float getDensityValue(float val) {
		return val * density;
	}

	void setUseEnglishNames(bool b){
		this->useEnglishNames = b;
	}

	bool isUsingEnglishNames(){
		return this->useEnglishNames;
	}

	std::string getTranslatedString(const std::string& src) {
	return src;
	}

	

};
#endif

//SkBitmap* getCachedBitmap(RenderingContext* rc, const std::string& bitmapResource);
void purgeCachedBitmaps();

#ifdef SK_SCALAR_IS_FLOAT
int get31TileNumberX(Sk_float longitude);
int get31TileNumberY( Sk_float latitude);

Sk_float getLongitudeFromTile(float zoom, Sk_float x) ;
Sk_float getLatitudeFromTile(float zoom, Sk_float y);

Sk_float get31LongitudeX(int tileX);
Sk_float get31LatitudeY(int tileY);
Sk_float getTileNumberX(float zoom, Sk_float longitude);
Sk_float getTileNumberY(float zoom, Sk_float latitude);

Sk_float getPowZoom(float zoom);
Sk_float calculateProjection31TileMetric(int xA, int yA, int xB, int yB, int xC, int yC);
Sk_float squareDist31TileMetric(int x1, int y1, int x2, int y2);
Sk_float convert31YToMeters(int y1, int y2) ;
Sk_float convert31XToMeters(int x1, int x2);

#else
int get31TileNumberX(double longitude);
int get31TileNumberY( double latitude);

double getLongitudeFromTile(float zoom, double x) ;
double getLatitudeFromTile(float zoom, double y);

double get31LongitudeX(int tileX);
double get31LatitudeY(int tileY);
double getTileNumberX(float zoom, double longitude);
double getTileNumberY(float zoom, double latitude);

double getPowZoom(float zoom);
#endif

#pragma endregion



#pragma region Useful Vectop/Map functions

//http://stackoverflow.com/questions/3450860/check-if-a-stdvector-contains-a-certain-object

class stdH
{
public:
#if 0
	template <typename K, typename V>
	static std::vector<K> GetKeys(const std::map<K, V>& m) {
		std::vector<K> result;
		result.reserve(m.size()); // For efficiency

		for (typename std::map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			 result.push_back(itr->first);

		return result;
	}

	template <typename K, typename V>
	static std::vector<V> GetValues(const std::map<K, V>& m) {
		std::vector<V> result;
		result.reserve(m.size()); // For efficiency

		for (typename std::map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			 result.push_back(itr->second);

		return result;
	}
	template <typename K, typename V>
	static  bool containsKey(const map<K, V>& m, K key) //http://stackoverflow.com/questions/4599792/c-accessing-stdmap-keys-and-values
	{
		
		for (typename map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
         if(itr->first == key)
			 return true;
		return false;

	}
	template <typename K, typename V>
	static  V *mapValByKey(const map<K, V>& m, K key) 
	{
		
		for (typename map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
         if(itr->first == key)
		 		 return (V*)&itr->second;
		return NULL;

	}
	template <typename V>
	static void addAllV(vector<V> *dest,vector<V> *src)
	{
				int len1=src->size();
				if(len1>0)
				{
					for(int j=0;j<len1;j++)
					{
						V el=(*src)[j];
						dest->push_back(el);
					}
				}
	}
	template <typename V>
	static void addAll(vector <V>&dest,const vector <V>&src)
	{
				int len1=src.size();
				if(len1>0)
				{
					for(int j=0;j<len1;j++)
						dest.push_back(src[j]);
				}
	}
	template <typename V>
	static void addAll(int index,vector <V>&dest,const vector <V>&src)
	{
				int len1=src.size();
				if(len1>0)
				{
					for(int j=0;j<len1;j++)
						insertItemAtIndex(dest,src[j],index );//dest.push_back(src[j]);
				}
	}
	template <typename V>
	static bool containsItem(vector<V> &v,V x)
	{
		bool rv=false;
		if(v.size()==0)
			return false;
		if(std::find(v.begin(), v.end(), x) != v.end()) {
			rv=true;// v contains x 
		} 
		return rv;
	}
#endif
	template <typename V>
	static bool removeItemAtIndex(vector<V> &v,int index )
	{
		if(v.size() > index){
			v.erase (v.begin()+index);  //nodes.remove(i);
		}
		return true;
	}
    /* svk I have a compiler error here Not sure WHY???
	template <typename V>
	static bool insertItemAtIndex(std::vector<V> &v,V item,int index )
	{
		std::vector<V>::iterator it = v.begin();
		it+=index;
		v.insert(it, item);
		return true;
	}
	*/
	template <typename V>
	static bool reverseV(vector<V> &v)
	{
		reverse(v.begin(),v.end());
		return true;
	}

//====================string manipulations========================
	//http://stackoverflow.com/questions/1878001/how-do-i-check-if-a-c-string-starts-with-a-certain-string-and-convert-a-sub
	static bool contains(string &str, string &substr)
	{
		return str.find(substr)!=string::npos;
	}
	static bool startsWith(string &str, string &substr)
	{
		bool rv=false;
		//strncmp(str, substr, strlen(substr));
		int len=substr.length();
		return strncmp(str.c_str(), substr.c_str(), len)==0;
	}
	//http://stackoverflow.com/questions/874134/find-if-string-endswith-another-string-in-c
	static inline bool endsWith(string str, string substr)   // it was (string &str, string &substr)  I removed & and it compiles now for android
	{
		 if (str.length() >= substr.length()) {
			return (0 == str.compare (str.length() - substr.length(), substr.length(), substr));
		} else {
			return false;
		}

	}
	static bool equalsIgnoreCase(string s,string s1)
	{
		toLowerCase(s);
		toLowerCase(s1);
		return s.compare(s1)==0;
	}
	static bool isEmpty(string s)
	{
		return s=="";
	}
	

	static int split(const string &s, char delim, vector<string> &elems) {
		int rv=0;
		stringstream ss(s);
		string item;
		while (getline(ss, item, delim)) {
			elems.push_back(item);
			rv++;
		}
		return rv;
	}

	// trim from start
	static inline std::string &ltrim(std::string &s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			return s;
	}

	// trim from end
	static inline std::string &rtrim(std::string &s) {
			s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
			return s;
	}

	// trim from both ends
	static inline std::string &trim(std::string &s) {
			return ltrim(rtrim(s));
	}

	//http://stackoverflow.com/questions/4668760/converting-an-int-to-stdstring
	template<typename T>
	static string toString(const T& value)
	{
		std::ostringstream oss;
		oss << value;
		return oss.str();
	}

};

#pragma endregion

#if !SK_BUILD_FOR_ANDROID  
void _sk_log ( const char *format, ...);
#else
#define _sk_log 
#endif


typedef struct 
{
	double latitude,longitude; 
}LatLon_T;

#endif /*_FCNAV_COMMON_H*/
