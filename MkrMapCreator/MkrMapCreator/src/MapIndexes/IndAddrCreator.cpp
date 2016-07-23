#include "../SqLiteDB/SqLiteHelper.h"
#include "IndAddrCreator.h"


#define addressCityStat "insert into city (id, latitude, longitude, name, name_en, city_type) values (?, ?, ?, ?, ?, ?)"


//void initSettings(bool normalizeStreets, string[] normalizeDefaultSuffixes, string[] normalizeSuffixes,	string cityAdminLevel) {
//	cities.clear();
//	cityManager.clear();
//	postalCodeRelations.clear();
//	cityBoundaries.clear();
//	this.normalizeStreets = normalizeStreets;
//	this.normalizeDefaultSuffixes = normalizeDefaultSuffixes;
//	this.normalizeSuffixes = normalizeSuffixes;
//}


linked_map<EntityId *, City*> IndAddrCreator::cities;
void IndAddrCreator::registerCityIfNeeded(Entity *e, DbAccessor *ctx)
{
	//I'm loosing typeid here when doing (*e) e - type NODE becomes (*e) - type Entity ??????
		if (Entity::typeOf(e)== NODE && e->getTag(OSMTagKey_PLACE) != "") {
			City *city=new City((Node*) e);
			if (city->getType() != 0 && city->getName()!="") {
				if (city->getType() == CityType_CITY || city->getType() == CityType_TOWN) {
					this->cityManager.registerObject(((Node*) e)->getLatitude(), ((Node*) e)->getLongitude(), city);
				} else {
					this->cityVillageManager.registerObject(((Node*) e)->getLatitude(), ((Node*) e)->getLongitude(), city);
				}
				cities.insert(pair<EntityId *,City*>(city->getEntityId(), city));
			}
		}
}

bool IndAddrCreator::createMapIndexStructure(sqlite3    *db)
{
	    bool rv;
		db_executeStatement(db,"create table city (id bigint primary key, latitude double, longitude double, name varchar(1024), name_en varchar(1024), city_type varchar(32))");
		db_executeStatement(db,"create index city_ind on city (id, city_type)");

		__exit_xx:
		return rv;
}
bool IndAddrCreator::createDatabaseStructure(sqlite3    *_db,const char *fn) 
{
	this->db=_db;
	streetDAO.createDatabaseStructure(db,fn);

	bool rv=createMapIndexStructure(db);
	if(!rv)
	{
		printf("IndAddrCreator::createDatabaseStructure : Failed!!\r\n");
	}
	return rv;
}

 
	void IndAddrCreator::writeCity(City *city) {
		char *b1 = 0, *b2 = 0, *b3 = 0; // we need 3 buffers to bind text fields
				EXEC_STMT_START(this->db,addressCityStat,__exit_stmt__);
				sqlite3_bind_int64(stmt, 1, city->getId());
				sqlite3_bind_double(stmt, 2, city->getLocation()->getLatitude());
				sqlite3_bind_double(stmt, 3, city->getLocation()->getLongitude());
				
				db_bind_text(stmt,4,city->getName(),b1,__exit_stmt__);
				db_bind_text(stmt,5,city->getEnName(),b2,__exit_stmt__);
				{
				string txt=City::valueToString(city->getType());
				toLowerCase(txt);
				db_bind_text(stmt,6,txt,b3,__exit_stmt__);
				}
				EXEC_STMT_FINISH;
__exit_stmt__:
				if(!ret)
				{
					printf("Error in IndAddrCreator::writeCity\r\n ");
				}
		        if(b1!=0)
					delete[] b1;
				if(b2!=0)
					delete[] b2;
				if(b3!=0)
					delete[] b3;
	}
	
	
	void IndAddrCreator::writeCitiesIntoDb() {
		vector<City*> v=stdH::LMGetValues<EntityId *,City*>(cities);
		int len=v.size();
		if(len==0)
			return;

		for (int i=0;i<len;i++) {
			City *c=v[i];
			writeCity(c);
		}
		
	}
int IndAddrCreator::extractBoundaryAdminLevel(Entity *e) 
{
		int adminLevel = -1;
		string tag="";
		try
		{
			tag = e->getTag(OSMTagKey_ADMIN_LEVEL);
			if (tag == "") {
				return adminLevel;
			}
			int value = atoi(tag.c_str());
			return value;
		}
		catch(exception &ex)
		{
			cout << "Exception IndAddrCreator::extractBoundaryAdminLevel : " << ex.what() << endl;
		}
		catch(...)
		{
			string str="UNEXPECTED...";
			cout << "Exception raised: " << str << '\n';

		}
}
 Boundary *IndAddrCreator::putCityBoundary(Boundary *boundary, City *cityFound) {
	// printf("putCityBoundary bId=%d cname=%s\r\n ", boundary->getBoundaryId(), cityFound->getName());
	 Boundary *oldBoundary = null;// cityBoundaries[cityFound];//cityBoundaries.get(cityFound);  !!!!!!!!!!!!!!!
	 bool rv = stdH::mapValByKey<City*, Boundary*>(cityBoundaries, cityFound, oldBoundary);
		if (rv&&oldBoundary != null) {
			// try to found the biggest area (not small center district)
			if (oldBoundary->getAdminLevel() >  boundary->getAdminLevel() && !stdH::equalsIgnoreCase(oldBoundary->getName(),cityFound->getName())) {
				cityBoundaries.insert(pair<City*,Boundary*>(cityFound, boundary));
				string s = "City: " + cityFound->getName() + " boundary: " +  boundary->getName();
				
				//	log.info(s);
				
			} 
			else if( stdH::equalsIgnoreCase(boundary->getName(),cityFound->getName()) && 
					!stdH::equalsIgnoreCase(oldBoundary->getName(),cityFound->getName())){
				cityBoundaries.insert(pair<City*,Boundary*>(cityFound, boundary));
				string s = "City: " + cityFound->getName() + " boundary: " +  boundary->getName();
				
				//	log.info(s);
				
			} 
			else if(oldBoundary->getAdminLevel() ==  boundary->getAdminLevel() && 
					oldBoundary != boundary &&  stdH::equalsIgnoreCase(boundary->getName(),oldBoundary->getName())){
				if(!oldBoundary->isClosedWay() && ! boundary->isClosedWay()){
					stdH::addAll(oldBoundary->getInnerWays(), boundary->getInnerWays());
					stdH::addAll(oldBoundary->getOuterWays(), boundary->getOuterWays());
				}
			}
		} else {
			cityBoundaries.insert(pair<City*,Boundary*>(cityFound, boundary));
			
			//	log.info("City: " + cityFound.getName() + " boundary: " +  boundary->getName());
			
		}
		return oldBoundary;
	}
bool IndAddrCreator::isBoundary(Entity *e) {
	return ("administrative"==e->getTag(OSMTagKey_BOUNDARY)) && (Entity::typeOf(e)== RELATION || Entity::typeOf(e)== WAY);	//return "administrative".equals(e.getTag(OSMTagKey.BOUNDARY)) && (e instanceof Relation || e instanceof Way);
}
Boundary *IndAddrCreator::extractBoundary(Entity *e, DbAccessor *ctx)
{
		if (isBoundary(e)) {
			Boundary *boundary = null;
			if (Entity::typeOf(e)== RELATION) {
				Relation *aRelation = (Relation*) e;
				ctx->loadEntityRelation(aRelation);
				boundary = new Boundary(true); //is computed later
				boundary->setName(aRelation->getTag(OSMTagKey_NAME));
				boundary->setBoundaryId(aRelation->getId());
				boundary->setAdminLevel(extractBoundaryAdminLevel(aRelation));
				linked_map<Entity*, string>& entities = aRelation->getMemberEntities();
				vector<Entity*> v= stdH::LMGetKeys<Entity*,string>(entities);
				for ( int i=0;i<v.size();i++) {
					Entity *es=v[i];
					if (Entity::typeOf(es)== WAY) {
						string val = "";
						stdH::LMmapValByKey<Entity*,string>(entities,es,val);
						bool inner = "inner"==val; 
						if (inner) {
							//boundary->getInnerWays().push_back((Way*) es);
							boundary->setInnerWays((Way*)es);
						} else {
							string wName = es->getTag(OSMTagKey_NAME);
							// if name are not equal keep the way for further check (it could be different suburb)
							if (wName== boundary->getName()) {
								visitedBoundaryWays.push_back(es->getId());
							}
							/*Way* w=(Way*)es;
							printf("wayId=%d \r\n ",w->getId());*/
							boundary->setOuterWays((Way*)es);
						}
					}
				}
				boundary->computeIsClosedWay();
			} 
			else if (Entity::typeOf(e)== WAY) {
				if (!stdH::containsItem<LONG_T>(visitedBoundaryWays,e->getId()))
				{
					bool closed = false;
					if(((Way*) e)->getNodeIds().size() > 1){
						closed = ((Way*) e)->getFirstNodeId() == ((Way*) e)->getLastNodeId();
					}
					boundary = new /*WayBoundary*/Boundary(closed);
					boundary->setName(e->getTag(OSMTagKey_NAME));
					boundary->setBoundaryId(e->getId());
					boundary->setAdminLevel(extractBoundaryAdminLevel(e));
					boundary->getOuterWays().push_back((Way*) e);
				}
			}
			return boundary;
		} else {
			return null;
		}
}
void IndAddrCreator::indexBoundariesRelation(Entity *e, DbAccessor *ctx) {
	LONG_T __id=e->getId();
	if (__id == 324211)
	{
		__id = -1;
	}
		Boundary *boundary = extractBoundary(e, ctx);
		if (boundary != null) {
			LatLon_S *boundaryCenter =  boundary->getCenterPoint();
			bool isCW=boundary->isClosedWay();
			int alv = boundary->getAdminLevel();
			string boundaryName = boundary->getName();
			if (isCW&&alv>=4&& boundaryCenter!=NULL&&!stdH::isEmpty(boundaryName))
			{
				vector<City*> citiesToSearch;//List<City> citiesToSearch = new ArrayList<City>();
				stdH::addAllV<City*>(&citiesToSearch, cityManager.getClosestObjects(boundaryCenter->getLatitude(), boundaryCenter->getLongitude(), 3));
				stdH::addAllV<City*>(&citiesToSearch, cityVillageManager.getClosestObjects(boundaryCenter->getLatitude(), boundaryCenter->getLongitude(), 3));

				City *cityFound = null;

				toLowerCase(boundaryName);
				for (int i = 0; i < citiesToSearch.size(); i++) {
					City *c = citiesToSearch[i];
					if (boundary->containsPoint(c->getLocation())) {
						string cname = c->getName();
						toLowerCase(cname);
						if (boundaryName == cname/*boundaryName.equalsIgnoreCase(c.getName())*/) {
							cityFound = c;
							break;
						}
					}
				}
				//We should not look for similarities, this can be 'very' wrong....
				if (cityFound == null) {
					// try to find same name in the middle
					for (int i = 0; i < citiesToSearch.size(); i++) {
						City *c = citiesToSearch[i];
						if (boundary->containsPoint(c->getLocation())) {
							string lower = c->getName();
							toLowerCase(lower);

							if (stdH::startsWith(boundaryName, (lower + " ")) || stdH::endsWith(boundaryName, (lower + " ")) || stdH::contains(boundaryName, (" " + lower + " "))) {
								cityFound = c;
								break;
							}
						}
					}
				}
				if (cityFound != null) {
					putCityBoundary(boundary, cityFound);
				}
				allBoundaries.push_back(boundary);
			}
			else if ( !isCW) {
				//log.info("Not using opened boundary: " + boundary);
			}
		}
	}

void IndAddrCreator::bindCitiesWithBoundaries()
{
	vector<Boundary*> freeBoundaries;// = new HashSet<Boundary>(allBoundaries);
	stdH::addAllV<Boundary*>(&freeBoundaries, &allBoundaries);
	vector<Boundary*> testB= stdH::GetValues<City *, Boundary*>(cityBoundaries);
	stdH::removeAll(freeBoundaries, testB);

	vector<City*> v_cities = stdH::LMGetValues<EntityId *, City*>(cities);
	for (City* c : v_cities)
	{
		Boundary *cityB = null;//cityBoundaries.get(c);
		bool rv = stdH::mapValByKey<City*, Boundary*>(cityBoundaries, c, cityB);
		int smallestAdminLevel = 8; //TODO start at level 8 for now...
		if (cityB == null) {
			LatLon_S *location = c->getLocation();
			Boundary *smallestBoundary = null;
			//try to found boundary
			for (Boundary *b : freeBoundaries) {
				if (b->getAdminLevel() > smallestAdminLevel) {
					if (b->containsPoint(location->getLatitude(), location->getLongitude())) {
						//the bigger the admin level, the smaller the boundary :-)
						smallestAdminLevel = b->getAdminLevel();
						smallestBoundary = b;
					}
				}
			}
			if (smallestBoundary != null) {
				Boundary *oldBoundary = putCityBoundary(smallestBoundary, c);
				stdH::removeItem(freeBoundaries,smallestBoundary);
				if (oldBoundary != null) {
					freeBoundaries.push_back(oldBoundary);
				}
			}
		}
	}
	//now for each city, try to put it in all boundaries it belongs to
	for (City *c : v_cities) {
		
		for (Boundary *b : allBoundaries) {
			if (b->containsPoint(c->getLocation())) {
				list<City*> *lst = 0;
				bool rv = stdH::mapValByKey<Boundary*, list<City*>*>(boundariesToCities, b, lst);	//boundariesToCities.get(b);
				if (lst == null) {
					lst = new list<City*>();
					boundariesToCities.insert(pair<Boundary*, list<City*>*>(b, lst));
				}
				lst->push_back(c);
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
void IndAddrCreator::indexAddressRelation(Relation *e, DbAccessor *ctx)  // did not hit "address" is not a relation type !!!
{
	bool rv = Entity::typeOf(e) == RELATION && e->getTag(OSMTagKey_TYPE) == "address";
	if (!rv) return;
#if 0
	string type = e->getTag(OSMTagKey_ADDRESS_TYPE);
	bool house = "house"==type; //$NON-NLS-1$
	bool street = "a6"==type; //$NON-NLS-1$
	if (house || street) {
		// try to find appropriate city/street
		City *c = null;
		// load with member ways with their nodes and tags !
		ctx->loadEntityRelation(e);

		vector<Entity*> &members = e->getMembers("is_in"); //$NON-NLS-1$
		Relation *a3 = null;
		Relation *a6 = null;
		if (!members.empty()) {
			if (street) {
				a6 = e;
			}
			Entity *in = members[0];  //members.iterator().next();
			if (Entity::typeOf(in) == RELATION) {
				ctx->loadEntityRelation((Relation*)in);
				// go one level up for house
				if (house) {
					a6 = (Relation*)in;
					members = ((Relation*)in)->getMembers("is_in"); //$NON-NLS-1$
					if (!members.empty()) {
						in = members[0];// members.iterator().next();
						if (Entity::typeOf(in) == RELATION) {
							ctx->loadEntityRelation((Relation*)in);
							a3 = (Relation*)in;
						}
					}

				}
				else {
					a3 = (Relation*)in;
				}
			}
		}

		if (a3 != null) {
			vector<EntityId*> &memberIds = a3->getMemberIds("label"); //$NON-NLS-1$
			if (!memberIds.empty()) {
				//c = cities.get(memberIds.iterator().next());
				EntityId* eid = memberIds[0];
				bool rv = stdH::LMmapValByKey<EntityId*, City*>(cities, eid, c);
			}
		}
		if (c != null && a6 != null) {
			string name = a6->getTag(OSMTagKey_NAME);

			if (!name.empty()) {
				LatLon_S *location = c->getLocation();
				for (Entity *en : e->getMembers(null)) {
					if (Entity::typeOf(en) == WAY) {
						LatLon_S *l = ((Way*)en)->getLatLon();
						if (l != null) {
							location = l;
							break;
						}
					}
				}

				static list<City*> singletonRes;
				vector<LONG_T> *streetIds = registerStreetInCities(name, "", location,/* Collections.singletonList(c)*/singletonRes);
				if (streetIds == null) {
					return;
				}
				if (street) {
					linked_map<Entity*, string>& entities = e->getMemberEntities();
					
					for (pair<Entity*, string> r : entities) {
						string val = r.second;
						Entity *en = r.first;
						if ("street"== val) { //$NON-NLS-1$
							if (Entity::typeOf(en) == WAY) {
								streetDAO.writeStreetWayNodes(*streetIds, (Way*)en);
							}
						}
						else if ("house"==val) { //$NON-NLS-1$
																 // will be registered further in other case
							if (!(Entity::typeOf(en) == RELATION)) {
								string hno = en->getTag(OSMTagKey_ADDR_HOUSE_NUMBER);
								if (hno != "") {
									Building *building = new Building(en);
									building->setName(hno);
									streetDAO.writeBuilding(*streetIds, building);
								}
							}
						}
					}
				}
				else {
					string hno = e->getTag(OSMTagKey_ADDRESS_HOUSE);
					if (hno == "") {
						hno = e->getTag(OSMTagKey_ADDR_HOUSE_NUMBER);
					}
					if (hno == "") {
						hno = e->getTag(OSMTagKey_NAME);
					}
					members = e->getMembers("border"); //$NON-NLS-1$
					if (!members.empty()) {
						Entity *border = members[0];//members.iterator().next();
						if (border != null) {
							EntityId *id = EntityId::valueOf(border);
							// special check that address do not contain twice in a3 - border and separate a6
							if (!a6.getMemberIds().contains(id)) {
								Building *building = new Building(border);
								if (building->getLocation() != null) {
									building->setName(hno);
									streetDAO.writeBuilding(*streetIds, building);
								}
								else {
									//log.error("Strange border " + id + " location couldn't be found");
								}
							}
						}
					}
					else {
						//log.info("For relation " + i.getId() + " border not found"); //$NON-NLS-1$ //$NON-NLS-2$
					}

				}
			}
		}
	}
#endif
}

vector<LONG_T>* IndAddrCreator::registerStreetInCities(string name, string nameEn, LatLon_S *location, /*Collection<City> result*/ vector<City*> &result)
{
	if (result.empty()) {
		return null;//Collections.emptySet();
	}
	/*if (Algoritms.isEmpty(nameEn)) {
		nameEn = Junidecode.unidecode(name);
	}*/

	vector<LONG_T> *values = new vector<LONG_T>();//TreeSet<Long>();
	for (City *city : result) {
		string cityPart = "";
		SimpleStreet *foundStreet = streetDAO.findStreet(name, city);
		if (foundStreet != null) {
			//matching the nodes is done somewhere else. This is just a simple check if the streets are really close to each other
			if (MapUtils::getDistance(location, foundStreet->getLocation()) > 900) {
				//oops, same street name within one city!
				if (foundStreet->getCityPart() == "") {
					//we need to update the city part first 
					string aCityPart = findCityPart(foundStreet->getLocation(), city);
					foundStreet = streetDAO.updateStreetCityPart(foundStreet, city, aCityPart != "" ? aCityPart : city->getName());
				}
				//now, try to find our cityPart again
				cityPart = findCityPart(location, city);
				foundStreet = streetDAO.findStreet(name, city, cityPart);
			}
		}
		if (foundStreet == null) {
			//by default write city with cityPart of the city
			LONG_T streetId = streetDAO.insertStreet(name, nameEn, location, city, cityPart);
			values->push_back(streetId);
		}
		else {
			values->push_back(foundStreet->getId());
		}
	}
	return values;
}
string IndAddrCreator::findCityPart(LatLon_S* location, City* city)
{
	string cityPart = city->getName();
	bool found = false;
	Boundary *cityBoundary = null;// cityBoundaries.get(city);
	bool rv = stdH::mapValByKey<City*,Boundary*>(cityBoundaries, city, cityBoundary);
	if (rv&&cityBoundary != null) {
		list<City*> *lst = null;
		rv = stdH::mapValByKey<Boundary*, list<City*>*>(boundariesToCities, cityBoundary, lst);
		if (rv&&lst != null)
		{
			for (City *subpart : (*lst)) {
				if (subpart != city) {
					Boundary *subBoundary = null;// cityBoundaries.get(subpart);
					rv = stdH::mapValByKey<City*, Boundary*>(cityBoundaries, subpart, subBoundary);
					if (cityBoundary != null && subBoundary != null &&  subBoundary->getAdminLevel() > cityBoundary->getAdminLevel()) {
						cityPart = findNearestCityOrSuburb(subBoundary, location); //subpart.getName();
						found = true;
						break;
					}
				}
			}
		}
	}
	if (!found) {
		Boundary *b = null;// cityBoundaries.get(city);
		rv = stdH::mapValByKey<City*, Boundary*>(cityBoundaries, city, b);
		if(rv&&b!=null)
			cityPart = findNearestCityOrSuburb(b, location);
	}
	return cityPart;
}
string IndAddrCreator::findNearestCityOrSuburb(Boundary* greatestBoundary,LatLon_S* location)
{
	string result = "";
	double dist = 100000000.0;// Double.MAX_VALUE;
	list<City*> *lst = null;// new list<City*>();
	if (greatestBoundary != null) {
		result = greatestBoundary->getName();
		//lst = boundariesToCities.get(greatestBoundary);
		
		stdH::mapValByKey<Boundary*, list<City*>*>(boundariesToCities, greatestBoundary, lst);
	}
	else {
		lst=new list<City*>();
		stdH::addAll2LV(lst,cityManager.getClosestObjects(location->getLatitude(), location->getLongitude()));
		stdH::addAll2LV(lst,cityVillageManager.getClosestObjects(location->getLatitude(), location->getLongitude()));
	}
	for (City *c : *lst) {
		double actualDistance = MapUtils::getDistance(location, c->getLocation());
		if (actualDistance < dist) {
			result = c->getName();
			dist = actualDistance;
		}
	}
	return result;
}

//---------------------------------------------------------------------------------------
void IndAddrCreator::iterateMainEntity(Entity *e, DbAccessor *ctx)
{
	string aitag = e->getTag(OSMTagKey_ADDR_INTERPOLATION);
	if (Entity::typeOf(e) == WAY &&  aitag!= "") 
	{
		int BuildingInterpolationType = 0;
		int interpolationInterval = 0;
		if (aitag != "") {
			
			BuildingInterpolationType = BuildingInterpolation::valueOf(aitag);
		}
		if (BuildingInterpolationType != 0 && interpolationInterval > 0) {
			vector<Node*> nodesWithHno;// = new ArrayList<Node>();
			for (Node *n : ((Way*)e)->getNodes()) {
				if (n->getTag(OSMTagKey_ADDR_HOUSE_NUMBER) != "" && n->getTag(OSMTagKey_ADDR_STREET) != "") {
					nodesWithHno.push_back(n);
				}
			}
			if (nodesWithHno.size() > 1) {
				for (int i = 1; i < nodesWithHno.size(); i++) {
					Node *first = nodesWithHno[(i - 1)];
					Node *second = nodesWithHno[i];
					bool exist = streetDAO.findBuilding(first);
					if (exist) {
						streetDAO.removeBuilding(first);
					}
					LatLon_S *l = e->getLatLon();
					vector<LONG_T> *idsOfStreet = getStreetInCity(first->getIsInNames(), first->getTag(OSMTagKey_ADDR_STREET), null, l);
					if (!idsOfStreet->empty()) {
						Building *building = new Building(first);
						building->setInterpolationInterval(interpolationInterval);
						building->setInterpolationType(BuildingInterpolationType);
						building->setName(first->getTag(OSMTagKey_ADDR_HOUSE_NUMBER));
						building->setName2(second->getTag(OSMTagKey_ADDR_HOUSE_NUMBER));
						building->setLatLon2(second->getLatLon());
						streetDAO.writeBuilding(idsOfStreet, building);
					}
				}
			}
		}
	}//if (Entity::typeOf(e) == WAY &&  tag!= "") 
	if (e->getTag(OSMTagKey_ADDR_HOUSE_NUMBER) != "" && e->getTag(OSMTagKey_ADDR_STREET) != "") {
		bool exist = streetDAO.findBuilding(e);
		if (!exist) {
			LatLon_S* l = e->getLatLon();
			vector<LONG_T> *idsOfStreet = getStreetInCity(e->getIsInNames(), e->getTag(OSMTagKey_ADDR_STREET), "", l);
			if (!idsOfStreet->empty()) {
				Building *building = new Building(e);
				string hno = e->getTag(OSMTagKey_ADDR_HOUSE_NUMBER);
				int i = hno.find('-');
				if (i != -1) {
					building->setInterpolationInterval(1);
					if (aitag != "") {
						int type = BuildingInterpolation::valueOf(aitag);
						building->setInterpolationType(type);
					}
					building->setName(hno.substr(0, i));
					building->setName2(hno.substr(i + 1));
				}
				else {
					building->setName(hno);
				}

				streetDAO.writeBuilding(idsOfStreet, building);
			}
		}
	}
	else if (Entity::typeOf(e) == WAY
		&& e->getTag(OSMTagKey_HIGHWAY) != "" && e->getTag(OSMTagKey_NAME) != "") {
		// suppose that streets with names are ways for car
		// Ignore all ways that have house numbers and highway type

		// if we saved address ways we could checked that we registered before
		bool exist = streetDAO.findStreetNode(e);

		// check that street way is not registered already
		if (!exist) {
			LatLon_S* l = e->getLatLon();
			vector<LONG_T> *idsOfStreet = getStreetInCity(e->getIsInNames(), e->getTag(OSMTagKey_NAME), e->getTag(OSMTagKey_NAME_EN), l);
			if (!idsOfStreet->empty()) {
				streetDAO.writeStreetWayNodes(idsOfStreet, (Way*)e);
			}
		}
	}
	if (Entity::typeOf(e)==RELATION) {
		if (e->getTag(OSMTagKey_POSTAL_CODE) != "") {
			ctx->loadEntityRelation((Relation*)e);
			postalCodeRelations.push_back((Relation*)e);
		}
	}

}

string IndAddrCreator::normalizeStreetName(string name) {
	name = stdH::trim(name);
	if (normalizeStreets) {
		string newName = name;
		bool processed = newName.length() != name.length();
		for (string ch : normalizeDefaultSuffixes) {
			int ind = checkSuffix(newName, ch);
			if (ind != -1) {
				newName = cutSuffix(newName, ind, ch.length());
				processed = true;
				break;
			}
		}
		
		if (!processed) {
			for (string ch : normalizeSuffixes) {
				int ind = checkSuffix(newName, ch);
				if (ind != -1) {
					newName = putSuffixToEnd(newName, ind, ch.length());
					processed = true;
					break;
				}
			}
		}
		if (processed) {
			return newName;
		}
	}
	return name;
}

int IndAddrCreator::checkSuffix(string name, string suffix) {
	int i = -1;
	bool searchAgain = false;
	do {
		i = name.find(suffix, i);
		searchAgain = false;
		if (i > 0) {
			if (stdH::isLetterOrDigit(name.at(i - 1))) {
				i++;
				searchAgain = true;
			}
		}
	} while (searchAgain);
	return i;
}

string IndAddrCreator::cutSuffix(string name, int ind, int suffixLength) {
	string newName = name.substr(0, ind);
	if (name.length() > ind + suffixLength + 1) {
		newName += name.substr(ind + suffixLength + 1);
	}
	return stdH::trim(newName);
}

string IndAddrCreator::putSuffixToEnd(string name, int ind, int suffixLength) {
	if (name.length() <= ind + suffixLength) {
		return name;

	}
	string newName;
	if (ind > 0) {
		newName = name.substr(0, ind);
		newName += name.substr(ind + suffixLength);
		newName += name.substr(ind - 1, ind + suffixLength);
	}
	else {
		newName = name.substr(suffixLength + 1) + name.at(suffixLength) + name.substr(0, suffixLength);
	}

	return stdH::trim(newName);
}

vector<LONG_T>* IndAddrCreator::getStreetInCity(vector<string> &isInNames, string name, string nameEn, LatLon_S *location)
{
	if (name == "" || location == null) {
		
		return null;

	}
	name = normalizeStreetName(name);
	vector<City*> result;// = new HashSet<City>();
	vector<City*> nearestObjects;// = new ArrayList<City>();
	stdH::addAllV(&nearestObjects,cityManager.getClosestObjects(location->getLatitude(), location->getLongitude()));
	stdH::addAllV(&nearestObjects,cityVillageManager.getClosestObjects(location->getLatitude(), location->getLongitude()));
	//either we found a city boundary the street is in
	for (City *c : nearestObjects) {
		Boundary *boundary = null;
		stdH::mapValByKey(cityBoundaries, c, boundary);
			
		string c_name = c->getName();
		if (stdH::containsV( isInNames , c_name) || (boundary != null && boundary->containsPoint(location))) {
			result.push_back(c);
		}
	}
	//or we need to find closes city
	if (result.empty()) {
		City *city = getClosestCity(location, isInNames, nearestObjects);
		if (city != null) {
			result.push_back(city);
		}
	}
	return registerStreetInCities(name, nameEn, location, result);
}
City* IndAddrCreator::getClosestCity(LatLon_S* point, vector<string> isInNames, vector<City*> &nearCitiesAndVillages)
{
	if (point == null) {
		return null;
	}
	// search by distance considering is_in names 
	City *closest = null;
	double relDist = 100000000.0;// Double.POSITIVE_INFINITY;
	for (City *c : nearCitiesAndVillages) {
		if (stdH::containsV(isInNames, c->getName())) {
			return c;
		}
		double radius = c->getType();//c->getType().getRadius();
		double rel = MapUtils::getDistance(c->getLocation(), point) / radius;
		if (rel < relDist) {
			closest = c;
			relDist = rel;
			if (relDist < 0.2 && isInNames.empty()) {
				return closest;
			}
		}
	}
	return closest;
}