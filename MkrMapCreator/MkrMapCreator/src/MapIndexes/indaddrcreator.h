#ifndef IndAddrCreator_H
#define IndAddrCreator_H

#include <sqlite3.h>
#include "DbAccessor.h"
#include "dataTileManager.h"
#include "DBStreetDAO.h"
#include "Boundary.h"

#include "../common/oCls.h"
#include <list>
#include <set>

using namespace std;



#pragma region class IndAddrCreator
class IndAddrCreator
{
	DataTileManager<City *> cityVillageManager ;
	DataTileManager<City *> cityManager;
	DBStreetDAO streetDAO;
	vector<LONG_T> visitedBoundaryWays;
	vector<Boundary*> allBoundaries;
	map<City*, Boundary*> cityBoundaries;
	map<Boundary*, list<City*>* > boundariesToCities ;
	sqlite3    *db;

	vector<Relation*> postalCodeRelations;

	bool normalizeStreets;
	vector<string> normalizeSuffixes, normalizeDefaultSuffixes;

	bool createMapIndexStructure(sqlite3    *db);
	void writeCity(City *city);
	bool isBoundary(Entity *e);
	Boundary *extractBoundary(Entity *e, DbAccessor *ctx);
	int extractBoundaryAdminLevel(Entity *e) ;
	Boundary *putCityBoundary(Boundary *boundary, City *cityFound);
	vector<LONG_T>* registerStreetInCities(string name, string nameEn, LatLon_S *location, /*Collection<City> result*/ vector<City*> &result);
	string findCityPart(LatLon_S* location, City* city);
	string findNearestCityOrSuburb(Boundary* greatestBoundary, LatLon_S* location);
	vector<LONG_T> *getStreetInCity(vector<string> &isInNames, string name, string nameEn, LatLon_S *location);

	string normalizeStreetName(string name);
	int checkSuffix(string name, string suffix);
	string cutSuffix(string name, int ind, int suffixLength);
	string putSuffixToEnd(string name, int ind, int suffixLength);
	City *getClosestCity(LatLon_S* point, vector<string> isInNames, vector<City*> &nearCitiesAndVillages);

public:
	static linked_map<EntityId *,City *> cities;
	IndAddrCreator()
	{
		cityVillageManager.setZoom(13);// = new DataTileManager<City>(13);
		cityManager.setZoom(10);// = new DataTileManager<City>(10);

		normalizeStreets=false;
	}
	
	void registerCityIfNeeded(Entity *e, DbAccessor *ctx);//static void registerCityIfNeeded(Entity *e,IndAddrCreator *_this);

	bool createDatabaseStructure(sqlite3    *db,const char *fn) ;
	void writeCitiesIntoDb();
	void indexBoundariesRelation(Entity *e, DbAccessor *ctx);

	void bindCitiesWithBoundaries();
	void indexAddressRelation(Relation*e, DbAccessor *ctx);

	void iterateMainEntity(Entity *e, DbAccessor *ctx);
};

#pragma endregion

#endif  //#ifndef DBACCESSOR_H

