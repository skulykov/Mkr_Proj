#include "../SqLiteDB/SqLiteHelper.h"
#include "DBStreetDAO.h"



#define addressStreetStat "insert into street (id, latitude, longitude, name, name_en, city, citypart) values (?, ?, ?, ?, ?, ?, ?)"
#define addressStreetNodeStat "insert into street_node (id, latitude, longitude, street, way) values (?, ?, ?, ?, ?)"
#define addressBuildingStat "insert into building (id, latitude, longitude, name, name_en, street, postcode, name2, name_en2, lat2, lon2, interval, interpolateType) values (?, ?, ?, ?, ?, ?, ?, ?, ? ,? ,? ,? ,?)"
#define addressSearchStreetStat "SELECT id,latitude,longitude FROM street WHERE ? = city AND ? = citypart AND ? = name"
#define addressSearchStreetStatWithoutCityPart "SELECT id,name,citypart,latitude,longitude FROM street WHERE ? = city AND ? = name"
#define addressStreetUpdateCityPart "UPDATE street SET citypart = ? WHERE id = ?"
#define addressSearchBuildingStat "SELECT id FROM building where ? = id"
#define addressRemoveBuildingStat "DELETE FROM building where ? = id"
#define addressSearchStreetNodeStat "SELECT way FROM street_node WHERE ? = way"


bool DBStreetDAO::createMapIndexStructure(sqlite3    *db)
{
	    bool rv;
		db_executeStatement(db,"create table street (id bigint primary key, latitude double, longitude double, name varchar(1024), name_en varchar(1024), city bigint, citypart varchar(1024))");
		db_executeStatement(db,"create index street_cnp on street (city,citypart,name,id)");
		db_executeStatement(db,"create index street_city on street (city)");
		db_executeStatement(db,"create index street_id on street (id)");
		// create index on name ?
		db_executeStatement(db,"create table building (id bigint, latitude double, longitude double, name2 varchar(1024), name_en2 varchar(1024), lat2 double, lon2 double, interval int, interpolateType varchar(50), name varchar(1024), name_en varchar(1024), street bigint, postcode varchar(1024), primary key(street, id))");
		db_executeStatement(db,"create index building_postcode on building (postcode)");
		db_executeStatement(db,"create index building_street on building (street)");
		db_executeStatement(db,"create index building_id on building (id)");

		db_executeStatement(db,"create table street_node (id bigint, latitude double, longitude double, street bigint, way bigint)");
		db_executeStatement(db,"create index street_node_street on street_node (street)");
		db_executeStatement(db,"create index street_node_way on street_node (way)");
__exit_xx:
		return rv;
}
bool DBStreetDAO::createDatabaseStructure(sqlite3    *_db,const char *fn) 
{
	this->db=_db;
	bool rv=createMapIndexStructure(_db);
	if(!rv)
	{
		printf("DBStreetDAO::createDatabaseStructure : Failed!!\r\n");
	}
	return rv;
}

void DBStreetDAO::writeStreetWayNodes(vector<LONG_T>* streetIds, Way *way)
{
	std::vector<Node*> nodes=way->getNodes();
	int lenIds=streetIds->size();
	int len=nodes.size();
	for (int i=0;i,lenIds;i++/*Long streetId : streetIds*/) {
		LONG_T streetId=streetIds->at(i);
			for (int j=0;j,len;j++/*Node n : way.getNodes()*/) {
				Node *n=nodes[j];
				
				EXEC_STMT_START(this->db,addressStreetNodeStat,__exit_stmt__);
				sqlite3_bind_int64(stmt, 1, n->getId());
				sqlite3_bind_double(stmt, 2, n->getLatitude());
				sqlite3_bind_double(stmt, 3, n->getLongitude());
				sqlite3_bind_int64(stmt, 4, way->getId());
				sqlite3_bind_int64(stmt, 5, streetId);
				EXEC_STMT_FINISH;
				__exit_stmt__:
				if(!ret)
				{
					printf("Error in DBStreetDAO::writeStreetWayNodes\r\n ");
				}
				
			}
		}
}
void DBStreetDAO::writeBuilding(vector<LONG_T> *streetIds, Building* b)
{
	int len = streetIds==0?0:streetIds->size();
	string str = "";
	for (int i = 0; i < len;i++)
	{
		LONG_T streetId = streetIds->at(i);
		EXEC_STMT_START(this->db, addressBuildingStat, __exit_stmt__);
		sqlite3_bind_int64(stmt, 1, b->getId());
		sqlite3_bind_double(stmt, 2, b->getLocation()->getLatitude());
		sqlite3_bind_double(stmt, 3, b->getLocation()->getLongitude());

		str = b->getName();
		sqlite3_bind_text(stmt, 4, str.c_str(), str.length(), SQLITE_STATIC);
		str = b->getEnName();
		sqlite3_bind_text(stmt, 5, str.c_str(), str.length(), SQLITE_STATIC);
		sqlite3_bind_int64(stmt, 6, streetId);
		str = b->getPostcode();
		sqlite3_bind_text(stmt, 7, str.c_str(), str.length(), SQLITE_STATIC);

		str = b->getName2();
		sqlite3_bind_text(stmt, 8, str.c_str(), str.length(), SQLITE_STATIC);
		sqlite3_bind_text(stmt, 9, str.c_str(), str.length(), SQLITE_STATIC);

		LatLon_S *l = b->getLatLon2();
		double val = l == null ? 0 : l->getLatitude();
		sqlite3_bind_double(stmt, 10, val);
		val = l == null ? 0 : l->getLongitude();
		sqlite3_bind_double(stmt, 11, val);
		sqlite3_bind_int(stmt, 12, b->getInterpolationInterval());
		sqlite3_bind_int(stmt, 13, b->getInterpolationType());

		EXEC_STMT_FINISH;
	__exit_stmt__:
		if (!ret)
		{
			printf("Error in DBStreetDAO::writeBuilding\r\n ");
		}
	}
}
SimpleStreet* DBStreetDAO::findStreet(string name, City* city)
{
	SimpleStreet* street=null;
	EXEC_STMT_START(this->db, addressSearchStreetStatWithoutCityPart, __exit_stmt__);
	sqlite3_bind_int64(stmt, 1, city->getId());
	sqlite3_bind_text(stmt, 2, name.c_str(),name.length(), SQLITE_STATIC);
	while (sqlite3_step(stmt) == SQLITE_ROW) {		//EXEC_STMT_FINISH;
		//new SimpleStreet(rs.getLong(1), rs.getString(2), rs.getString(3), rs.getDouble(4), rs.getDouble(5));
		LONG_T id = sqlite3_column_int64(stmt, 0);
		const unsigned char *p = sqlite3_column_text(stmt, 1);
		string name((char*)p);
		p = sqlite3_column_text(stmt, 2);
		string cityPart((char*)p);
		double lat= sqlite3_column_double(stmt, 3);
		double lon = sqlite3_column_double(stmt, 4);
		street = new SimpleStreet(id, name, cityPart, lat, lon);
	}
	sqlite3_finalize(stmt);
__exit_stmt__:
	if (!ret)
	{
		//printf("DbAccessor::loadEntityRelation\r\n ");
	}
	
	return street;
}

SimpleStreet* DBStreetDAO::findStreet(string name, City* city, string citypart)
{
	SimpleStreet* street = null;
	EXEC_STMT_START(this->db, addressSearchStreetStat, __exit_stmt__);
	sqlite3_bind_int64(stmt, 1, city->getId());
	sqlite3_bind_text(stmt, 2, citypart.c_str(), citypart.length(), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, name.c_str(), name.length(), SQLITE_STATIC);
	while (sqlite3_step(stmt) == SQLITE_ROW) {		//EXEC_STMT_FINISH;
													//new SimpleStreet(rs.getLong(1), rs.getString(2), rs.getString(3), rs.getDouble(4), rs.getDouble(5));
		LONG_T id = sqlite3_column_int64(stmt, 0);
		double lat = sqlite3_column_double(stmt, 1);
		double lon = sqlite3_column_double(stmt, 2);
		street = new SimpleStreet(id, name, citypart, lat, lon);
	}
	sqlite3_finalize(stmt);
__exit_stmt__:
	if (!ret)
	{
		//printf("DbAccessor::loadEntityRelation\r\n ");
	}

	return street;
}

SimpleStreet* DBStreetDAO::updateStreetCityPart(SimpleStreet* street, City* city, string citypart)
{
	EXEC_STMT_START(this->db, addressStreetUpdateCityPart, __exit_stmt__);
	sqlite3_bind_text(stmt, 1, citypart.c_str(), citypart.length(), SQLITE_STATIC);
	sqlite3_bind_int64(stmt, 2, street->getId());
	
	EXEC_STMT_FINISH;
__exit_stmt__:
	if (!ret)
	{
		printf("Error DBStreetDAO::updateStreetCityPart\r\n ");
	}
	return new SimpleStreet(street->getId(),street->getName(),citypart,street->getLocation());
}
LONG_T DBStreetDAO::insertStreet(string name, string nameEn, LatLon_S *location, City* city, string cityPart)
{
	LONG_T streetId = streetIdSequence++;
	EXEC_STMT_START(this->db, addressStreetStat, __exit_stmt__);
	sqlite3_bind_int64(stmt, 1, streetId);
	sqlite3_bind_text(stmt, 4, name.c_str(), name.length(), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 5, nameEn.c_str(), nameEn.length(), SQLITE_STATIC);
	sqlite3_bind_double(stmt, 2, location->getLatitude());
	sqlite3_bind_double(stmt, 3, location->getLongitude());
	sqlite3_bind_int64(stmt, 6, city->getId());
	sqlite3_bind_text(stmt, 7, cityPart.c_str(), cityPart.length(), SQLITE_STATIC);

	EXEC_STMT_FINISH;
__exit_stmt__:
	if (!ret)
	{
		printf("Error DBStreetDAO::insertStreet\r\n ");
	}
	return streetId;
}
bool DBStreetDAO::findBuilding(Entity *e)
{
	bool exist = false;
	EXEC_STMT_START(this->db, addressSearchBuildingStat, __exit_stmt__);
	sqlite3_bind_int64(stmt, 1, e->getId());
	while (sqlite3_step(stmt) == SQLITE_ROW) {		//EXEC_STMT_FINISH;
		LONG_T id = sqlite3_column_int64(stmt, 0);
		exist = true;
	}
	sqlite3_finalize(stmt);
__exit_stmt__:
	if (!ret)
	{
		//printf("DbAccessor::loadEntityRelation\r\n ");
	}
	return exist;
}
bool DBStreetDAO::removeBuilding(Entity *e)
{
	EXEC_STMT_START(this->db, addressRemoveBuildingStat, __exit_stmt__);
	sqlite3_bind_int64(stmt, 1, e->getId());
	EXEC_STMT_FINISH;
__exit_stmt__:
	if (!ret)
	{
		printf("DBStreetDAO::removeBuilding\r\n ");
	}
	return ret;
}

bool DBStreetDAO::findStreetNode(Entity *e) {
	bool exist=false;
	EXEC_STMT_START(this->db, addressSearchStreetNodeStat, __exit_stmt__);
	sqlite3_bind_int64(stmt, 1, e->getId());
	while (sqlite3_step(stmt) == SQLITE_ROW) {		//EXEC_STMT_FINISH;
		LONG_T wid = sqlite3_column_int64(stmt, 0);
		exist = true;
	}
	sqlite3_finalize(stmt);
__exit_stmt__:
	if (!ret)
	{
		//printf("DbAccessor::loadEntityRelation\r\n ");
	}
	return exist;
}