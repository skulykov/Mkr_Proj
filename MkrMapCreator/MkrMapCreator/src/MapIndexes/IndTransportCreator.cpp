
#include "IndTransportCreator.h"

bool executeStatement(sqlite3    *db,const char *sql);
#define db_executeStatement(a,b)	rv=executeStatement(a,b); \
	                                if(!rv) goto __exit_xx;

bool IndTransportCreator::createMapIndexStructure(sqlite3    *db)
{
	    bool rv;
		db_executeStatement(db,"create table transport_route (id bigint primary key, type varchar(1024), operator varchar(1024), ref varchar(1024), name varchar(1024), name_en varchar(1024), dist int)");
		db_executeStatement(db,"create index transport_route_id on transport_route (id)");

		db_executeStatement(db,"create table transport_route_stop (stop bigint, route bigint, ord int, direction smallint, primary key (route, ord, direction))");
		db_executeStatement(db,"create index transport_route_stop_stop on transport_route_stop (stop)");
		db_executeStatement(db,"create index transport_route_stop_route on transport_route_stop (route)");
		
		db_executeStatement(db,"create table transport_stop (id bigint primary key, latitude double, longitude double, name varchar(1024), name_en varchar(1024))");
		db_executeStatement(db,"create index transport_stop_id on transport_stop (id)");
		db_executeStatement(db,"create index transport_stop_location on transport_stop (latitude, longitude)");
__exit_xx:
		return rv;
}
bool IndTransportCreator::createDatabaseStructure(sqlite3    *db,const char *fn) 
{
	bool rv=createMapIndexStructure(db);
	if(!rv)
	{
		printf("IndTransportCreator::createDatabaseStructure : Failed!!\r\n");
	}
	return rv;
}

