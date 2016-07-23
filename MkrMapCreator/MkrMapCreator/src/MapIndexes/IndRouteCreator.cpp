
#include "IndRouteCreator.h"


#define ROUTE_CREATETABLE  "create table route_objects (id bigint primary key, types binary, pointTypes binary, pointIds binary, pointCoordinates binary, name varchar(4096))"
#define ROUTE_CREATE_IND  "create index route_objects_ind on route_objects (id)"
#define ROUTE_SELECT_STAT  "SELECT types, pointTypes, pointIds, pointCoordinates, name FROM route_objects WHERE id = ?"
#define ROUTE_INSERT_STAT  "insert into route_objects(id, types, pointTypes, pointIds, pointCoordinates, name) values(?, ?, ?, ?, ?, ?)"


bool IndRouteCreator::createMapIndexStructure(sqlite3    *db)
{
	    bool rv;
		db_executeStatement(db,ROUTE_CREATETABLE);
		db_executeStatement(db,ROUTE_CREATE_IND);

		__exit_xx:
		return rv;
}
bool IndRouteCreator::createDatabaseStructure(sqlite3    *db,const char *fn) 
{
	bool rv=createMapIndexStructure(db);
	if(!rv)
	{
		printf("IndRouteCreator::createDatabaseStructure : Failed!!\r\n");
	}
	return rv;
}

void IndRouteCreator::iterateMainEntity(Entity *e, DbAccessor *ctx)
{
}
