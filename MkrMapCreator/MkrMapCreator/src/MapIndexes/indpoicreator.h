#ifndef IndPoiCreator_H
#define IndPoiCreator_H


#include "../common/oCls.h"
#include <sqlite3.h>
#include "DbAccessor.h"

#include "dataTileManager.h"
#include "../SqLiteDB/SqLiteHelper.h"
using namespace std;



#pragma region class IndPoiCreator
class IndPoiCreator
{
	sqlite3    *poiDB;
	bool createMapIndexStructure();
public:
	
	IndPoiCreator()
	{
		
	}
	
	bool createDatabaseStructure(const char *fn) ;
	void close_db(){close_sqlite_db(poiDB);	}

};

#pragma endregion

#endif  //#ifndef DBACCESSOR_H

