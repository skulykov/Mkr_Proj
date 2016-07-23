#ifndef DBStreetDAO_H
#define DBStreetDAO_H


#include "../common/oCls.h"
#include <sqlite3.h>
#include "DbAccessor.h"

#include "dataTileManager.h"

using namespace std;



#pragma region class DBStreetDAO
class DBStreetDAO
{
	sqlite3    *db;
	bool createMapIndexStructure(sqlite3    *db);
	LONG_T streetIdSequence;
public:
	
	DBStreetDAO()
	{
		streetIdSequence = 0;
	}
	
	bool createDatabaseStructure(sqlite3    *db,const char *fn) ;
	void writeStreetWayNodes(vector<LONG_T> *streetIds, Way *way);
	void writeBuilding(vector<LONG_T> *streetIds,Building* building);

	SimpleStreet *findStreet(string name,City* city);
	SimpleStreet *findStreet(string name, City* city,string citypart);

	SimpleStreet *updateStreetCityPart(SimpleStreet* street, City* city, string citypart);

	LONG_T insertStreet(string name,string nameEn,LatLon_S *location,City* city,string cityPart);

	bool findBuilding(Entity *first);
	bool removeBuilding(Entity *first);
	bool findStreetNode(Entity *e);
};

#pragma endregion

#endif  //#ifndef DBACCESSOR_H

