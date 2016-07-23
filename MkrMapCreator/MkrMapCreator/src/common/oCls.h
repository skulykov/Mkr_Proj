#ifndef OSMCLASSES_H
#define OSMCLASSES_H

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>

#include <map>
#include <vector>
#include <algorithm>
 
#include <stdint.h>
//#include <unordered_map>

#include "MapUtils.h"
#include "oSettings.h"

#include "cxxext/linked_map.hpp"

typedef int INT_T;
typedef /*long long*/ int64_t LONG_T; // __int64  -> for 64 bits

#define null NULL
using namespace std;
using namespace cxxext;


class Node;
class Way;
class Relation;

#include "Utils.h"

#if 1
#pragma region class Entity
class EntityId;
enum EntityType {
		//NONE,
		NODE,
		WAY,
		RELATION,
		WAY_BOUNDARY,
	NONE
	};
//-------------EntityId--------------------------

class Entity
{
	
	map< string, string > tags;// std::map<std::string, std::string> tags = null;
	 LONG_T id;
	 bool dataLoaded;
	
	
public:
	

	static EntityType typeOf(Entity *e){
		/*string s=typeid(*e).name();
		cout<<s<<endl;*/
		if(/*e instanceof Node*/typeid(Node)==typeid(*e)){
				return EntityType::NODE;
			} else if(typeid(Way)==typeid(*e)){
				return EntityType::WAY;
			} else if(typeid(Relation)==typeid(*e)){
				return EntityType::RELATION;
			}
			return  EntityType::NONE;
	}
	
	Entity(LONG_T id) {
		this->id = id;
	}
	Entity(Entity copy, LONG_T id) {
		this->id = id;
		/*for (std::string t : copy.getTagKeySet()) {
			putTag(t, copy.getTag(t));
		}*/
		this->dataLoaded = copy.dataLoaded;
	}
	LONG_T getId() {
		return id;
	}
	
	

	virtual  void initializeLinks(std::map<EntityId, Entity> entities){}
	
	
	/**
	 * @return middle point for entity
	 */
	virtual LatLon_S *getLatLon() {return NULL;}; //virtual LatLon_T getLatLon()=0  make the class abstract 
	

	
	bool isVirtual(){
		return id < 0;
	}

	void entityDataLoaded() {
		this->dataLoaded = true;
	}
	
	bool isDataLoaded() {
		return dataLoaded;
	}

	void putTag(string key, string value){
		
		tags.insert(pair<string,string>(key, value));
	}
		
	string getTag(string key){
		if(tags.size() == 0){
			return "";
		}
		//stdH::mapValByKey(const map<K, V>& m, K key)    will it work better???
		return tags[key];
	}

	vector<string> getIsInNames() {
		vector<string> v;
		string values = getTag(OSMTagKey_IS_IN);
		if (values == "") {
			return v;
		}
		if (values.find(';') != string::npos) {
			vector<string> splitted = stdH::split(values,';');
			
			for (int i = 0; i < splitted.size(); i++) {
				v.push_back(stdH::trim(splitted[i]));
			}
			return v;
		}
		return v;
	}
};

class EntityId {
		EntityType type;
		LONG_T id;
		
	public:		
		EntityId(EntityType type, LONG_T id){
			this->type = type;
			this->id = id;
		}
		
		static EntityId *valueOf(Entity *e){
			return new EntityId(e->typeOf(e), e->getId());
		}
		LONG_T getId(){return id;}
		EntityType getType(){return type;}
};

#pragma endregion

#pragma region class Node
class Node:public Entity
{
	//LONG_T LONG_T serialVersionUID;
	double latitude;
	double longitude;
	// currently not used
//	 boolean visible = true;
public:	
	 Node(double latitude, double longitude, LONG_T id):Entity(id)
	 {
		//super(id);
		//serialVersionUID = -2981499160640211082L;
		this->latitude = latitude;
		this->longitude = longitude;
	}
	
	Node(Node n, LONG_T newId):Entity(n,newId)
	{
		//super(n, newId);
		//serialVersionUID = -2981499160640211082L;
		this->latitude = n.latitude;
		this->longitude = n.longitude;
	}

	double getLatitude() {
		return latitude;
	}
	
	double getLongitude() {
		return longitude;
	}
	
	void setLatitude(double latitude) {
		this->latitude = latitude;
	}
	
	void setLongitude(double longitude) {
		this->longitude = longitude;
	}
	
	
	LatLon_S *getLatLon() {
		return new LatLon_S(latitude, longitude);
	}
	
	////////@Override
	void initializeLinks(std::map<EntityId, Entity> entities) {
		 //nothing to initialize
		
	}

};
#pragma endregion

#pragma region class Way
class Way:public Entity
{
	 std::vector<LONG_T> nodeIds;//TLongArrayvector nodeIds = null;
	 std::vector<Node*> nodes;// = null;
public:
	 Way(LONG_T id):Entity(id) {}
	void addNode(LONG_T id){
		nodeIds.push_back(id);
	}
	
	LONG_T getFirstNodeId(){
		if(nodeIds.size()==0)
			return -1;
		return nodeIds[0];
	}
	
	LONG_T getLastNodeId(){
		if(nodeIds.size()==0)
			return -1;
		return nodeIds[nodeIds.size() - 1];
	}
	
	void addNode(Node *n){
		
		nodeIds.push_back(n->getId());
		nodes.push_back(n);
	}
	
	void addNode(Node *n, int index){
		std::vector<LONG_T>::iterator it;
		it = nodeIds.begin();
		it+=index;

		nodeIds.insert(it, n->getId());
		
		
		//std::vector<Node*>::iterator it1;
		//it1 = nodes.begin();
		//it1+=index;
		//nodes.insert(it1, n);//nodes.add(index, n);
		stdH::insertItemAtIndex<Node*>(nodes,n,index);
	}
	
	LONG_T removeNodeByIndex(int i){
		if(nodeIds.size() == 0){
			return -1;
		}
		LONG_T toReturn = nodeIds[i];
		//if(nodes.size() > i){
		//	nodes.erase (nodes.begin()+i);  //nodes.remove(i);
		//}
		stdH::removeItemAtIndex<Node*>(nodes,i);
		return toReturn;
	}
	
	std::vector<LONG_T> &getNodeIds(){
		
		return nodeIds;
	}
	
	vector<EntityId*> getEntityIds(){
		
		vector<EntityId*> ls ;//
		for (int i = 0; i < nodeIds.size(); i++) {
			ls.push_back(new EntityId(EntityType::NODE, nodeIds[i]));
		}
		return ls;
	}
	
	std::vector<Node*> &getNodes() {
		
		return nodes;
	}
	
	//////@Override
	void initializeLinks(map<EntityId*, Entity*> entities) {
		
			nodes.clear();
			int nIsize = nodeIds.size();
			for (int i = 0; i < nIsize; i++) {
				EntityId *eId=new EntityId(EntityType::NODE,nodeIds[i]);
				Entity *n = NULL;
				bool rv=stdH::mapValByKey<EntityId*, Entity*>(entities, eId,n) ;
				if(rv)nodes.push_back((Node*)n);
			}
	}
	
	//////@Override
	LatLon_S *getLatLon() {
		if(nodes.size()==0)
			return 0;
		return __MapUtils_getWeightCenterForNodes(nodes);
	}
	
};
#pragma endregion


#pragma region Class Relation
class Relation : public Entity 
{
	/* Maps are not inherently ordered. Conceptually, they represent key-value pairs. 
	But C++ std::map maps are internally ordered. See http://www.cplusplus.com/reference/stl/map/ . 
	So entries will effectively be sorted by key values.
	*/
	//each Entity is identified by EntityId ( id,type-Node,Way,Relation) 
	/*linked_map*/linked_map<EntityId*, string> members;
	/*linked_map*/linked_map<Entity *, string> memberEntities;   // key MUST be a pointer !!!!
	
public:	
	Relation(LONG_T id): Entity(id) {}
	
	void addMember(LONG_T id, EntityType type, string role){
		
		EntityId *eId=new EntityId(type,id);
		//put//
		//	members.insert(members.end(), pair<EntityId*, string>(eId, role));  // insert at the end
		members.insert(pair<EntityId*, string>(eId, role));  // insert at the end
	}
	
		
	string getRole(Entity *e){
		LONG_T id=e->getId();
		return this->getRole(id);
	}
	
	string getRole(LONG_T id){
		for (linked_map<EntityId*,string>::iterator itr = members.begin(); itr != members.end(); ++itr)
		{
			EntityId *eId=itr->first;
			if( eId->getId()== id)
			 return itr->second;
		
		}
		return ""; // no Role found
	}
	
	vector<EntityId*> getMemberIds() {
		return getMemberIds("");
	}
	
	linked_map<EntityId*, string>& getMembersMap() {
		
		return members;//return Collections.unmodifiableMap(members);
	}
	
	vector<EntityId*> getMemberIds(string role) {
		
		vector<EntityId*> l;//= new Arrayvector<EntityId>();

		if (role == "") {
			return stdH::LMGetKeys<EntityId*,string>(members);//members.keySet();
		}
		
		vector<EntityId*> v=stdH::LMGetKeys<EntityId*,string>(members);
		for (int i=0;i<v.size(); i++) {
			EntityId *m = v[i];
			string r = "";
			stdH::LMmapValByKey<EntityId*,string>(members,m,r);
			if (role==r) {
				l.push_back(m);
			}
		}

		return l;
	}
	
	linked_map<Entity*, string>& getMemberEntities() {
		
		return memberEntities;
	}
	
	vector<Entity*>& getMembers(string role) {
		
		vector<Entity*> l;

		if (role == "") {
			return stdH::LMGetKeys<Entity*,string>(memberEntities);//return memberEntities.keySet();
		}
		
		vector<Entity*> v=stdH::LMGetKeys<Entity*,string>(memberEntities);
		for (int i=0;i<v.size(); i++) {
			Entity *m = v[i];
			string r = "";
			stdH::LMmapValByKey<Entity*,string>(memberEntities,m,r);
			if (role==r) {
				l.push_back(m);
			}
		}

		return l;
		
	}
	
	//virtual
	void initializeLinks(linked_map<EntityId*, Entity*> entities){
	
		if (entities.size() == 0 || members.size()==0)
			return;
			memberEntities.clear();
		
			/*for(EntityId l : members.keySet()){
				if(l != null && entities.get(l) != null){
					memberEntities.put(entities.get(l), members.get(l));
				}
			}*/
			vector<EntityId*> v=stdH::LMGetKeys<EntityId*,string>(members);
			for (int i=0;i<v.size(); i++) {
				EntityId *eId = v[i];
				if (eId != NULL)
				{
					Entity *e = NULL;
					bool rv = stdH::LMmapValByKey<EntityId*, Entity*>(entities, eId,e);
					if (rv)
					{	
						string str = "";
						rv = stdH::LMmapValByKey<EntityId*, string>(members, eId,str);
						//put//
							//memberEntities.insert(pair<Entity*, string>(e, str));
						memberEntities.insert(pair<Entity*, string>(e, str));
					}
				}
			}
	
	}
	

	//virtual
	LatLon_S *getLatLon() {
		return 0;
	}

};

#pragma endregion

//===========================MapObjects=========================
#pragma region Class MapObject
class MapObject
{
	EntityType type;
	LONG_T id;
	string name;
	string enName;

	//LatLon_S *location ; -> I  moved it to public
public:
	LatLon_S *location ;
	MapObject(){location=0;}
	MapObject(Entity *e){
		setEntity(e);
	}
	
	
	void setEntity(Entity *e){
		this->id = e->getId();
		this->type = Entity::typeOf(e);
		if(this->name == ""){
			this->name = e->getTag(OSMTagKey_NAME);
		}
		if (this->enName == "") {
			this->enName = e->getTag(OSMTagKey_NAME_EN);
			if(name == ""){
				this->name = this->enName;
			}
		}
		//if(this->location == null){
			this->location = __MapUtils_getCenter(e);
		//}
		//this->setNameFromOperator(e);
	}
	void setId(LONG_T id) {
		this->id = id;
	}
	
	LONG_T getId() {
		return this->id;
	}
	LatLon_S *getLocation(){
		return location;
	}
	
	void setLocation(double latitude, double longitude){
		location = new LatLon_S(latitude, longitude);
	}
	string getName(){return name;}
	virtual void setName(string _name){this->name=_name;}
	string getEnName(){return enName;}
	virtual void setEnName(string _name){this->enName=_name;}
	EntityId *getEntityId(){
		EntityType t = type;
		if(t == 0){
			t = EntityType::NODE;
		}
		return new EntityId(t, this->id);
	}
};
#pragma endregion




#pragma region class Building
typedef struct {
	string tag;
	short val;
}BuildingInterpolation_Enum_T;

class BuildingInterpolation {
public:
	static BuildingInterpolation_Enum_T values[];
	static int valueOf(string tag);
		
};
class Building : public MapObject
{
	string postcode;
	LatLon_S *latLon2;
	/*BuildingInterpolation*/short interpolationType;
	int interpolationInterval;
	string name2;
	int BuildingInterpolation_val;
	/*enum BuildingInterpolation {
		ALL(-1), EVEN(-2), ODD(-3), ALPHABETIC(-4);
		final int val;

		BuildingInterpolation(int val) {
			this->val = val;
		}
		int getValue() {
			return val;
		}
		
		static BuildingInterpolation fromValue(int i){
			for(BuildingInterpolation b : values()) {
				if(b.val == i) {
					return b;
				}
			}
			return null;
		}
	}*/
public:
	Building(Entity *e):MapObject(e){
		
		postcode = e->getTag(OSMTagKey_ADDR_POSTCODE);
	}
	
	Building(){}
	
	string getPostcode() {
		return postcode;
	}
	
	int getInterpolationInterval() {
		return interpolationInterval;
	}
	void setInterpolationInterval(int interpolationNumber) {
		this->interpolationInterval = interpolationNumber;
	}
	
	short getInterpolationType() {
		return interpolationType;
	}
	
	void setInterpolationType(int interpolationType) {
		this->interpolationType = (short)interpolationType;
	}
	
	LatLon_S *getLatLon2() {
		return latLon2;
	}
	void setLatLon2(LatLon_S *latlon2) {
		this->latLon2 = latlon2;
	}
	string getName2() {
		return name2;
	}
	
	void setName2(string name2) {
		this->name2 = name2;
	}
	
	void setPostcode(string postcode) {
		this->postcode = postcode;
	}
	
	
};
#pragma endregion

#pragma region class Street
class City;
class Street : public MapObject
{
	vector<Building*> buildings;// = new ArrayList<Building>(); 
	vector<Way> wayNodes;// = null;
	vector<Street> intersectedStreets;// = null;
	City *city;
	int indexInCity;

public:
	Street(City *city, string name);
	
	Street(City *city) ;

	Building *registerBuilding(Entity *e);
	
	Building *registerBuilding(Entity *e, string ref);
	
	void registerBuilding(Building *building);
	
	vector<Street> getIntersectedStreets() ;
	
	void addIntersectedStreet(Street s);
	
	void registerBuildingCheckById(Building *building);
	vector<Building*> getBuildings() ;
	
	void calculateCenter();
	
	bool isRegisteredInCity();
	
	//@Override
	void setName(string name);
	
	
	vector<Way> getWayNodes() ;
	
	City *getCity() ;
	
	void sortBuildings();

	//@Override
	void doDataPreparation() ;
	
	int getIndexInCity() ;
	
	void setIndexInCity(int indexInCity);

};
#pragma endregion

#endif
#pragma region Class City
 enum CityType {
		// that's tricky way to play with that numbers (to avoid including suburbs in city & vice verse)
		CityType_CITY=10000, 
		CityType_TOWN=5000, 
		CityType_VILLAGE=1300, 
		CityType_HAMLET=1000, 
		CityType_SUBURB=400
		
 };
#if 1
class City : public MapObject
{
	CityType type ;
	// Be attentive ! Working with street names ignoring case
	map<string, Street*> streets;// = new TreeMap<String, Street>(Collator.getInstance());
	string isin ;
	string postcode ;

	static LONG_T POSTCODE_INTERNAL_ID;
	City(string postcode, long id);
	
public: 
	
	City(Node *el):MapObject(el) 
	{
		//super(el);
		type = valueFromString(el->getTag(OSMTagKey_PLACE));
		isin = el->getTag(OSMTagKey_IS_IN);
		isin = isin != "" ? isin/*isin.toLowerCase()*/ : "";
		
	} 
	
	City(CityType type);

	static CityType valueFromString(string place) ;
	static string valueToString(CityType type);
	static City *createPostcode(string postcode);
	
	string getIsInValue();
	
	bool isPostcode();

	bool isEmptyWithStreets() ;

	Street *registerStreet(string street) ;

	Street *unregisterStreet(string name) ;

	void removeAllStreets() ;

	string getPostcode() ;

	void setPostcode(string postcode) ;

	Street *registerStreet(Street *street, bool en) ;

	Street *registerStreet(Street *street) ;

	Building *registerBuilding(Entity *e) ;


	CityType getType();
	vector<Street*> getStreets() ;

	Street *getStreet(string name);
};
#endif
#pragma endregion


class SimpleStreet {
	LONG_T id;
	string name;
	string cityPart;
	LatLon_S *location;

public: 
		SimpleStreet(long id, string name, string cityPart, double lat, double lon) {
		this->id = id;
		this->name = name;
		this->cityPart = cityPart;
		this->location = new LatLon_S(lat, lon);
	}
	SimpleStreet(long id, string name, string cityPart,	LatLon_S *location) {
		this->id = id;
		this->name = name;
		this->cityPart = cityPart;
		this->location = location;
	}
	string getCityPart() {
		return cityPart;
	}
	LONG_T getId() {
		return id;
	}
	string getName() {
		return name;
	}
	LatLon_S* getLocation() {
		return location;
	}
};

#endif  //#ifndef OSMCLASSES_H

