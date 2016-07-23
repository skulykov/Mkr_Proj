#ifndef IndRouteCreator_H
#define IndRouteCreator_H


#include "../common/oCls.h"
#include <sqlite3.h>
#include "DbAccessor.h"

#include "dataTileManager.h"
#include "../SqLiteDB/SqLiteHelper.h"
using namespace std;



#pragma region class IndRouteCreator
class IndRouteCreator
{
	bool createMapIndexStructure(sqlite3    *db);
public:
	
	IndRouteCreator()
	{
		
	}
	
	bool createDatabaseStructure(sqlite3    *db,const char *fn) ;
	
	void iterateMainEntity(Entity *e, DbAccessor *ctx);
};

#pragma endregion

#endif  //#ifndef DBACCESSOR_H

