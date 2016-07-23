#ifndef IndVMapCreator_H
#define IndVMapCreator_H


#include "../common/oCls.h"
#include <sqlite3.h>
#include "DbAccessor.h"

#include "dataTileManager.h"
#include "../SqLiteDB/SqLiteHelper.h"
using namespace std;



#pragma region class IndVMapCreator
class IndVMapCreator
{
	sqlite3    *db;

	bool createMapIndexStructure(sqlite3    *db);
public:
	
	IndVMapCreator()
	{
		
	}
	
	bool createDatabaseStructure(sqlite3    *db,const char *fn) ;
	void indexMapRelationsAndMultiPolygons(Entity *e, DbAccessor *ctx);
	
	void iterateMainEntity(Entity *e, DbAccessor *ctx);
};

#pragma endregion

#endif  //#ifndef DBACCESSOR_H

