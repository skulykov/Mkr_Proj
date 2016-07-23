
#include "IndVMapCreator.h"


bool IndVMapCreator::createMapIndexStructure(sqlite3    *db)
{
	    bool rv;
		db_executeStatement(db,"create table binary_map_objects (id bigint primary key, name varchar(4096), area smallint, types binary, additionalTypes binary, coordinates binary, innerPolygons binary)");
		db_executeStatement(db,"create index binary_map_objects_ind on binary_map_objects (id)");

		db_executeStatement(db,"create table low_level_map_objects (id bigint primary key, start_node bigint, end_node bigint, name varchar(1024), nodes binary, type binary, addType binary, level smallint)");
		db_executeStatement(db,"create index low_level_map_objects_ind on low_level_map_objects (id)");
		db_executeStatement(db,"create index low_level_map_objects_ind_st on low_level_map_objects (start_node, type)");
		db_executeStatement(db,"create index low_level_map_objects_ind_end on low_level_map_objects (end_node, type)");
__exit_xx:
		return rv;
}
bool IndVMapCreator::createDatabaseStructure(sqlite3    *db,const char *fn) 
{
	this->db=db;
	bool rv=createMapIndexStructure(db);
	if(!rv)
	{
		printf("IndVMapCreator::createDatabaseStructure : Failed!!\r\n");
	}
	return rv;
}
void IndVMapCreator::indexMapRelationsAndMultiPolygons(Entity *e, DbAccessor *ctx)
{
}

void IndVMapCreator::iterateMainEntity(Entity *e, DbAccessor *ctx)
{
}
