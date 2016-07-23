#ifndef IndTransportCreator_H
#define IndTransportCreator_H


#include "../common/oCls.h"
#include <sqlite3.h>
#include "DbAccessor.h"

#include "dataTileManager.h"

using namespace std;



#pragma region class IndTransportCreator
class IndTransportCreator
{
	bool createMapIndexStructure(sqlite3    *db);
public:
	
	IndTransportCreator()
	{
		
	}
	
	bool createDatabaseStructure(sqlite3    *db,const char *fn) ;
	

};

#pragma endregion

#endif  //#ifndef DBACCESSOR_H

