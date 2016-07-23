#ifndef _MkrMap_BINARY_READ_H
#define _MkrMap_BINARY_READ_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <fstream>
#include <map>
#include <string>
#include <stdint.h>
#include "mapObjects.h"
#include "multipolygons.h"
#include "common.h"


#include "mapObjects.h"
#include "renderRules.h"



static const int MAP_VERSION = 2;
static const int BASEMAP_ZOOM = 11;


struct MapTreeBounds {
	uint32_t length;
	uint32_t filePointer;
	uint32_t mapDataBlock;
	uint32_t left ;
	uint32_t right ;
	uint32_t top ;
	uint32_t bottom;
	bool ocean;

	MapTreeBounds() {
		ocean = -1;
	}
};
struct RoutingIndex;
struct RouteSubregion {
	uint32_t length;
	uint32_t filePointer;
	uint32_t mapDataBlock;
	uint32_t left;
	uint32_t right;
	uint32_t top;
	uint32_t bottom;
	std::vector<RouteSubregion> subregions;
	RoutingIndex* routingIndex;

	RouteSubregion(RoutingIndex* ind) //: length(0), filePointer(0), mapDataBlock(0), routingIndex(ind)
	{
		length=0;
		filePointer=0;
		mapDataBlock=0;
		routingIndex=ind;   // from initMapStructure(CodedInputStream* input, BinaryMapFile* file)  new operator
	}
	~RouteSubregion()
	{
		/*if(routingIndex!=NULL)
			delete routingIndex;*/
		subregions.clear();
	}
};


struct MapRoot: MapTreeBounds {
	int minZoom ;
	int maxZoom ;
	std::vector<MapTreeBounds> bounds;
};

enum PART_INDEXES {
	MAP_INDEX = 1,
	POI_INDEX,
	ADDRESS_INDEX,
	TRANSPORT_INDEX,
	ROUTING_INDEX,
};

struct BinaryPartIndex {
	uint32_t length;
	int filePointer;
	PART_INDEXES type;
	std::string name;

	BinaryPartIndex(PART_INDEXES tp) : type(tp) {}
};

struct RoutingIndex : BinaryPartIndex {
//	unordered_map< uint32_t, tag_value > decodingRules;
	vector< tag_value > decodingRules;
	std::vector<RouteSubregion> subregions;
	std::vector<RouteSubregion> basesubregions;
	RoutingIndex() : BinaryPartIndex(ROUTING_INDEX) {
	}
	~RoutingIndex()
	{
		if(decodingRules.size()>0)
			decodingRules.clear();
		subregions.clear();
		basesubregions.clear();
	}

	void initRouteEncodingRule(uint32_t id, std::string tag, std::string val) {
		tag_value pair = tag_value(tag, val);
		// DEFINE hash
		//encodingRules[pair] = id;
		while(decodingRules.size() < id + 1){
			decodingRules.push_back(pair);
		}
		decodingRules[id] = pair;
	}
};

 

struct RouteDataObject {
	RoutingIndex* region;
	std::vector<uint32_t> types ;
	std::vector<unsigned short> bJNInd;  // added for routing
	std::vector<uint64_t> bJNWayIds;     // added for routing
	std::vector<uint32_t> pointsX ;
	std::vector<uint32_t> pointsY ;
	std::vector<uint64_t> restrictions ;
	std::vector<std::vector<uint32_t> > pointTypes;
	int64_t id;

	unordered_map<int, std::string > names;
	vector<pair<uint32_t, uint32_t> > namesIds;


	 enum Direction
     {
        OneWayForward = -1,
        TwoWay = 0,
        OneWayReverse = 1
     };

	string getName() {
		// we have a street name with key=1   -> look at a map data creation
		unordered_map<int, std::string >::const_iterator pos = names.find(1);
		if (pos == names.end()) {  // did not find key=1
			if(names.size() > 0) {
				return names.begin()->second;
			}
			return "";
		} else {
			string val = pos->second; // this MUST be a street name
			return val;
		}
		
		return names[1];

		
	}
	bool _isDigit(char ch)
	{
		if(ch>=0x30 && ch<=0x39)
			return true;
		else
			return false;
	}
	bool isLoop(){
		return pointsX[0] == pointsX[pointsX.size() - 1] && pointsY[0] == pointsY[pointsY.size() - 1] ; 
	}
	int getSize() {
		int s = sizeof(this);
		s += pointsX.capacity()*sizeof(uint32_t);
		s += pointsY.capacity()*sizeof(uint32_t);
		s += types.capacity()*sizeof(uint32_t);
		s += restrictions.capacity()*sizeof(uint64_t);
		std::vector<std::vector<uint32_t> >::iterator t = pointTypes.begin();
		for(;t!=pointTypes.end(); t++) {
			s+= (*t).capacity() * sizeof(uint32_t);
		}
		s += namesIds.capacity()*sizeof(pair<uint32_t, uint32_t>);
		s += names.size()*sizeof(pair<int, string>)*10;
		return s;
	}
	//-------------------------------------------------------------------------------------
	double directionRoute(int startPoint, bool plus){
		// look at comment JAVA
		return directionRoute(startPoint, plus, 5);
	}

	// Gives route direction of EAST degrees from NORTH ]-PI, PI]
	double directionRoute(int startPoint, bool plus, float dist) {
		int x = pointsX[startPoint];
		int y = pointsY[startPoint];
		int nx = startPoint;
		int px = x;
		int py = y;
		double total = 0;
		do {
			if (plus) {
				nx++;
				if (nx >= pointsX.size()) {
					break;
				}
			} else {
				nx--;
				if (nx < 0) {
					break;
				}
			}
			px = pointsX[nx];
			py = pointsY[nx];
			// translate into meters
			total += abs(px - x) * 0.011 + abs(py - y) * 0.01863;
		} while (total < dist);
		return -atan2( (float)x - px, (float) y - py );
	}
	//----------------------------------------------------------------------------
	//88888888888888888888888888888888888888888888888888888888888888888888888888888
	double getDirectionDelta( uint32_t originIdx, bool forward ) 
	{
		//NOTE: Victor: the problem to put more than 5 meters that BinaryRoutePlanner will treat
		// 2 consequent Turn Right as UT and here 2 points will have same turn angle
		// So it should be fix in both places
		return getDirectionDelta(originIdx, forward, 5);
	}

	double getDirectionDelta( uint32_t originIdx, bool forward, float distance )
	{
		/*
		auto itPoint = (_points.begin() + originIdx);
		const auto itOriginPoint = itPoint;
		float scannedDistance = 0.0;
		do
		{
			if(forward)
			{
				itPoint++;
				if(itPoint == _points.end())
				{
					itPoint--;
					break;
				}
			}
			else
			{
				if(itPoint == _points.begin())
					break;
				itPoint--;
			}

			// translate into meters
			scannedDistance +=
				MapUtils::x31toMeters(abs((int64_t)itPoint->x - (int64_t)itOriginPoint->x)) +
				MapUtils::y31toMeters(abs((int64_t)itPoint->y - (int64_t)itOriginPoint->y));
		} while ( scannedDistance < distance );

		return -atan2(itOriginPoint->x - itPoint->x, itOriginPoint->y - itPoint->y);
		*/
		return directionRoute((int)originIdx, forward, distance);
	}


	bool _roundabout(tag_value tv)
	{
		bool rv=false;
		if(tv.first=="junction" && tv.second=="roundabout")
			{ 
				rv = true;
			}
		return rv;
	}
	int onewayDirection(tag_value r)
	{
		int rv=0;
		if(r.first=="oneway")
		{
		   std::string v=r.second;
		   if(v=="-1"||v=="reverse")
			   rv=-1;
		   else if(v=="1"||v=="yes")
			   rv=1;
		}
		return rv;
	}
	Direction getDirection() 
	{
		/*for(auto itType = types.begin(); itType != types.end(); ++itType)
		{
			auto rule = subsection->section->_encodingRules[*itType];

			if(rule->_type == ObfRoutingSection::EncodingRule::OneWay)
				return static_cast<Direction>(rule->_parsedValue.asSignedInt);
			else if(rule->_type == ObfRoutingSection::EncodingRule::Roundabout)
				return Direction::OneWayForward;
		}*/
    
		int sz = types.size();
		for(int i=0; i<sz; i++) 
		{
			tag_value tv=region->decodingRules[types[i]];
			int rv=onewayDirection(tv);
			if(rv==1)
#if SK_BUILD_FOR_ANDROID	
				//http://stackoverflow.com/questions/5188554/my-enum-is-not-a-class-or-namespace
				return OneWayForward;
#else
				return Direction::OneWayForward;
#endif
			else if(rv==-1)
#if SK_BUILD_FOR_ANDROID
			return OneWayReverse;
#else
			return Direction::OneWayReverse;
#endif
		}
#if SK_BUILD_FOR_ANDROID
		return TwoWay;
#else
		return Direction::TwoWay;
#endif
	}
	bool isRoundabout(){
		int sz = types.size();
		for(int i=0; i<sz; i++) {
			tag_value tv=region->decodingRules[types[i]];
			if(_roundabout(tv))
			{ 
				return true;
			} else 
			if(onewayDirection(tv) != 0 && isLoop()) {
				return true;
			}
		}
		return false;
	}
	string getHighway() {
		string highway = "";
		int sz = types.size();
		for (int i = 0; i < sz; i++) {
			tag_value tv=region->decodingRules[types[i]];
			if(tv.first=="oneway" && tv.second!="")
			{
				highway = tv.second;
				break;
			}
		}
		return highway;
	}
	int getLanes() {
		int sz = types.size();
		for (int i = 0; i < sz; i++) {
			tag_value tv=region->decodingRules[types[i]];
			std::string v=tv.second;
			if(tv.first=="lanes" && v!="")
			{
				int i=0;
				while(i<v.length() && _isDigit(v[i]))
					i++;
				if (i > 0) {
					v=v.substr(0, i);
					int d=atoi( v.c_str());
					return d;
				}
			}
			
		}
		return -1;
	}
};



struct MapIndex : BinaryPartIndex {

	std::vector<MapRoot> levels;

	unordered_map<int, tag_value > decodingRules;
	// DEFINE hash
	//unordered_map<tag_value, int> encodingRules;

	int nameEncodingType;
	int refEncodingType;
	int coastlineEncodingType;
	int coastlineBrokenEncodingType;
	int landEncodingType;
	int onewayAttribute ;
	int onewayReverseAttribute ;
	unordered_set< int > positiveLayers;
	unordered_set< int > negativeLayers;

	MapIndex() : BinaryPartIndex(MAP_INDEX) {
		nameEncodingType = refEncodingType = coastlineBrokenEncodingType = coastlineEncodingType = -1;
		landEncodingType = onewayAttribute = onewayReverseAttribute = -1;
	}
	~MapIndex()
	{
		levels.clear();
		decodingRules.clear();
		positiveLayers.clear();
		negativeLayers.clear();
	}
	void finishInitializingTags() {
		int free = decodingRules.size() * 2 + 1;
		coastlineBrokenEncodingType = free++;
		initMapEncodingRule(0, coastlineBrokenEncodingType, "natural", "coastline_broken");
		if (landEncodingType == -1) {
			landEncodingType = free++;
			initMapEncodingRule(0, landEncodingType, "natural", "land");
		}
	}

	void initMapEncodingRule(uint32_t type, uint32_t id, std::string tag, std::string val) {
		tag_value pair = tag_value(tag, val);
		// DEFINE hash
		//encodingRules[pair] = id;
		decodingRules[id] = pair;

		if ("name" == tag) {
			nameEncodingType = id;
		} else if ("natural" == tag && "coastline" == val) {
			coastlineEncodingType = id;
		} else if ("natural" == tag && "land" == val) {
			landEncodingType = id;
		} else if ("oneway" == tag && "yes" == val) {
			onewayAttribute = id;
		} else if ("oneway" == tag && "-1" == val) {
			onewayReverseAttribute = id;
		} else if ("ref" == tag) {
			refEncodingType = id;
		} else if ("layer" == tag) {
			if (val != "" && val != "0") {
				if (val[0] == '-') {
					negativeLayers.insert(id);
				} else {
					positiveLayers.insert(id);
				}
			}
		}
	}
};


struct BinaryMapFile {
	std::string inputName;
	uint32_t version;
	uint64_t dateCreated;
	std::vector<MapIndex> mapIndexes;
	std::vector<RoutingIndex*> routingIndexes;
	std::vector<BinaryPartIndex*> indexes;
	int fd;
	int routefd;
	bool basemap;
	/*CodedInputStream*/ void *cis;
	bool isBasemap(){
		/*if(inputName.find("basemap")!=std::string::npos)
			return true;
		else
			return false;*/
		return basemap;
	}
	BinaryMapFile() 
	{
		basemap=false;
	}
	~BinaryMapFile() {
		mapIndexes.clear();
		for(int i=0;i<routingIndexes.size();i++)
		{
			RoutingIndex *p=routingIndexes[i];
			if(p!=NULL)
				delete p;
		}
		routingIndexes.clear();
		
		/*for(int i=0;i<indexes.size();i++)
		{
			BinaryPartIndex *p=indexes[i];
			if(p!=NULL)
				delete p;
		}*/
		indexes.clear();
		
		close(fd);
		close(routefd);
	}
};

struct ResultPublisher {
	std::vector< MapDataObject*> result;

	bool publish(MapDataObject* r) {
		result.push_back(r);
		return true;
	}
	bool publish(std::vector<MapDataObject*> r) {
		result.insert(result.begin(), r.begin(), r.end());
		return true;
	}
	bool isCancelled() {
		return false;
	}
	virtual ~ResultPublisher() {
		deleteObjects(result);
	}
};

struct SearchQuery {
	RenderingRuleSearchRequest* req;
	int left;
	int right;
	int top;
	int bottom;
	int zoom;
	ResultPublisher* publisher;

	coordinates cacheCoordinates;
	bool ocean;
	bool mixed;

	int numberOfVisitedObjects;
	int numberOfAcceptedObjects;
	int numberOfReadSubtrees;
	int numberOfAcceptedSubtrees;

	SearchQuery(int l, int r, int t, int b, RenderingRuleSearchRequest* req, ResultPublisher* publisher) :
			req(req), left(l), right(r), top(t), bottom(b),publisher(publisher) {
		numberOfAcceptedObjects = numberOfVisitedObjects = 0;
		numberOfAcceptedSubtrees = numberOfReadSubtrees = 0;
		ocean = mixed = false;
	}
	SearchQuery(int l, int r, int t, int b) :
				left(l), right(r), top(t), bottom(b) {
	}

	SearchQuery(){

	}

	bool publish(MapDataObject* obj) {
		return publisher->publish(obj);
	}
};

void searchRouteSubregions(SearchQuery* q, std::vector<RouteSubregion>& tempResult);

void searchRouteDataForSubRegion(SearchQuery* q, std::vector<RouteDataObject*>& list, RouteSubregion* sub);

ResultPublisher* searchObjectsForRendering(SearchQuery* q, bool skipDuplicates, int renderRouteDataFile, std::string msgNothingFound);

BinaryMapFile* initBinaryMapFile(std::string inputName,bool flag=false);

bool initMapFilesFromCache(std::string inputName) ;

bool closeBinaryMapFile(std::string inputName);


//==========Address search==========================
//bool initAddressStructure(CodedInputStream* input, BinaryMapFile* file) ;
//BinaryMapFile* initBinaryMapFile4Address(std::string inputName);
//---------------------Affress Search--------------------------
//https://developers.google.com/protocol-buffers/docs/encoding

typedef struct{
	string building;
	string street;
	string city;
	string province;
	string postalcode;
}FindAddress_T;
typedef struct{
	string name;
//	__int64 id;
	int x,y;
}Found_Entity_T;
typedef struct
{
	Found_Entity_T city;
	Found_Entity_T street;
	Found_Entity_T building;
}Found_Address_T;
#endif

