
#include "oCls.h"

using namespace std;

//===========================MapObjects=========================
#pragma region class Building
BuildingInterpolation_Enum_T BuildingInterpolation::values[] = {
	"ALL",-1,
	"EVEN",-2,
	"ODD",-3,
	"ALPHABETIC",-4
};
int BuildingInterpolation::valueOf(string tag)
{
	toUpperCase(tag);
	int rv = 0;
	for (int i = 0; i < 4; i++)
	{
		if (BuildingInterpolation::values[i].tag == tag)
		{
			rv = BuildingInterpolation::values[i].val;
			break;
		}
	}
	return rv;
}
#pragma endregion
#if 1
#pragma region class City
static struct{
				 const char *name;
				 CityType type;
			 }cityType[5]={
				 "CITY",CityType_CITY,
				 "TOWN",CityType_TOWN,
				 "VILLAGE",CityType_VILLAGE,
				 "HAMLET",CityType_HAMLET,
				 "SUBURB",CityType_SUBURB
			 };
LONG_T City::POSTCODE_INTERNAL_ID = -1000;
	City::City(string postcode, long id) {
		this->type = (CityType)CityType_CITY;
		this->setName( postcode);
		this->setEnName ( postcode);
		this->setId(id);
	}
	City::City(CityType type) {
		this->type = type;
	}
	
	CityType City::valueFromString(string place) {
			
			if (place == "") {
				return (CityType)0;
			}
			for (int i=0;i<5;i++) 
			{
				string s=cityType[i].name;
				toLowerCase(s);//std::transform(s.begin(), s.end(), s.begin(), ::tolower);  // toLowerCase !!!!!!!!!
				if (s==place) {
					return cityType[i].type;
				}
			}
			return (CityType)0;
		}

	string City::valueToString(CityType type)
	{
		int i=0;
		switch(type)
		{
		case CityType_SUBURB:
			i=4;
			break;
		case CityType_HAMLET:
			i=3;
			break;
		case CityType_VILLAGE:
			i=2;
			break;
		case CityType_TOWN:
			i=1;
			break;
		default:
			break;

		}
		return cityType[i].name;
	}
	
	
	City *City::createPostcode(string postcode){
		return new City(postcode, POSTCODE_INTERNAL_ID--);
	}
	
	string City::getIsInValue() {
		return isin;
	}
	
	bool City::isPostcode(){
		return type == (CityType)0;
	}

	bool City::isEmptyWithStreets() {
		return streets.size()==0;
	}

	Street *City::registerStreet(string street) {
		toLowerCase(street);
		if (!stdH::containsKey<string,Street*>(streets,getName())) {
			streets.insert(pair<string,Street*>(street, new Street(this, street)));
		}
		return streets[street];
	}

	Street *City::unregisterStreet(string name) {
		return 0;//streets.remove(name.toLowerCase());
	}

	void City::removeAllStreets() {
		streets.clear();
	}

	string City::getPostcode() {
		return postcode;
	}

	void City::setPostcode(string postcode) {
		this->postcode = postcode;
	}

	Street *City::registerStreet(Street *street, bool en) {
		string name = en ? street->getEnName() : street->getName();
		toLowerCase(name);
		
		if (name!="") {
			if (!stdH::containsKey<string,Street*>(streets,name)) {
				streets.insert(pair<string,Street*>(name, street));
				return street;
			} else {
				// try to merge streets
				Street *prev = streets[name];
				if (!street->getWayNodes().empty()) {
					stdH::addAll<Way>(prev->getWayNodes(),street->getWayNodes());//prev.getWayNodes().addAll(street.getWayNodes());
				}
				stdH::addAll<Building*>(prev->getBuildings(),street->getBuildings());//prev.getBuildings().addAll(street.getBuildings());
				return prev;
			}
		}
		return null;
	}

	Street *City::registerStreet(Street *street) {
		return registerStreet(street, false);
	}

	Building *City::registerBuilding(Entity *e) {
		string number = e->getTag(OSMTagKey_ADDR_HOUSE_NUMBER);
		string street = e->getTag(OSMTagKey_ADDR_STREET);
		if (street != "" && number != "") {
			return registerStreet(street)->registerBuilding(e);
		}
		return null;
	}


	CityType City::getType(){return type;}
	vector<Street*> City::getStreets() {
		return stdH::GetValues<string,Street*>(streets);//.values();
	}

	
	Street *City::getStreet(string name) {
		toLowerCase(name);
		return streets[name];
	}

#pragma endregion
#endif

#if 1

#pragma region class Street

	Street::Street(City *city, string name){
		indexInCity = -1;
		this->city = city;
		this->setName(name);
	}
	
	Street::Street(City *city) {
		indexInCity = -1;
		this->city = city;
	}

	Building *Street::registerBuilding(Entity *e){
		return registerBuilding(e, e->getTag(OSMTagKey_ADDR_HOUSE_NUMBER));
	}
	
	Building *Street::registerBuilding(Entity *e, string ref){
		
		Building *building = new Building(e);
		building->setName(ref);
		buildings.push_back(building);
		return building;
	}
	
	void Street::registerBuilding(Building *building){
		buildings.push_back(building);
	}
	
	vector<Street> Street::getIntersectedStreets() {
		return intersectedStreets;
	}
	
	void Street::addIntersectedStreet(Street s){
		intersectedStreets.push_back(s);
	}
	
	void Street::registerBuildingCheckById(Building *building){
		/*for(Building b : buildings) {
			if(b.getId() == building.getId()){
				return;
			}
		}*/
		LONG_T _id=building->getId();
		for(int i=0;i<buildings.size();i++)
		{
			Building *b=buildings[i];
			if(b->getId() == _id){
				return;
			}
		}
		buildings.push_back(building);
	}
	
	vector<Building*> Street::getBuildings() {
		return buildings;
	}
	
	void Street::calculateCenter(){
		vector<Node*> nodes;// = new Arrayvector<Node>();
		int len=wayNodes.size();
		if (len > 0) {
			for (int i=0;i<len;i++/*Way w : wayNodes*/) {
				Way w=wayNodes[i];
				stdH::addAll<Node*>(nodes,w.getNodes());//nodes.addAll(w.getNodes());
			}
		}
		if(nodes.size()==0)
			return;
		LatLon_S *c = __MapUtils_getWeightCenterForNodes(nodes);
		double dist  = 1.0e+38;//Double.POSITIVE_INFINITY;
		for(int i=0;i<nodes.size();i++/*Node n : nodes*/){
			Node *n=nodes[i];
			
			double nd = MapUtils::getDistance(n->getLatitude(),n->getLongitude(), c->getLatitude(),c->getLongitude());
				if (nd < dist) {
					dist = nd;
					location = n->getLatLon();
				}
			
		}
	}
	
	bool Street::isRegisteredInCity(){
		return city != null && city->getStreet(getName()) == this;
	}
	
	//@Override
	void Street::setName(string name) {
		if (name==getName()/*name.equals(getName())*/) {
			return;
		}
		if (city != null && city->getStreet(getName()) == this) {
			city->unregisterStreet(getName());
			MapObject::setName(name);//super.setName(name);
			Street *s = city->registerStreet(this);
			if(s != this){
				// that case means that street unregistered
//				city = null;
			}
		} else {
			MapObject::setName(name);//super.setName(name);
		}
	}
	
	
	vector<Way> Street::getWayNodes() {
		return wayNodes;
	}
	
	City *Street::getCity() {
		return city;
	}
	
	void Street::sortBuildings(){
		//Collections.sort(buildings, new Comparator<Building>(){
		//	//@Override
		//	int compare(Building o1, Building o2) {
		//		int i1 = Algoritms.extractFirstIntegerNumber(o1.getName());
		//		int i2 = Algoritms.extractFirstIntegerNumber(o2.getName());
		//		return i1 - i2;
		//	}
		//});
	}

	//@Override
	void Street::doDataPreparation() {
		sortBuildings();
		calculateCenter();
		if(getLocation() == null){
			vector<LatLon_S*> nodes;// = new Arrayvector<LatLon>();
			for(int i=0;i<buildings.size();i++){
				Building *b = buildings[i];
				nodes.push_back(b->getLocation());
			}
			location = __MapUtils_getWeightCenter(nodes);
		}
		if(location == null || ((wayNodes.size()==0) && buildings.size()==0)){
			city->unregisterStreet(getName());
		}
		if ( wayNodes.size() > 0) {
			this->setId( wayNodes[0].getId());
		} else if(buildings.size() > 0){
			this->setId(buildings[0]->getId());
		}
		
	}
	
	int Street::getIndexInCity() {
		return indexInCity;
	}
	
	void Street::setIndexInCity(int indexInCity) {
		this->indexInCity = indexInCity;
	}


#pragma endregion

#endif


