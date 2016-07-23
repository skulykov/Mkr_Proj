
#include "IndPoiCreator.h"

#define POI_CREATETABLE "create table poi  (id bigint, x int, y int, name_en varchar(1024), name varchar(1024), type varchar(1024), subtype varchar(1024), opening_hours varchar(1024), phone varchar(1024), site varchar(1024), description varchar(4096), primary key(id, type, subtype))"
#define POI_CREATE_IND  "create index poi_loc on poi (x, y, type, subtype)"
#define POI_CREATE_IND_1  "create index poi_id on poi (id, type, subtype)"
#define POI_TABLE_VERSION "PRAGMA user_version = 1"   // version 1

bool IndPoiCreator::createMapIndexStructure()
{
	sqlite3    *db=this->poiDB;    
	bool rv;
		db_executeStatement(db,POI_CREATETABLE);
		db_executeStatement(db,POI_CREATE_IND);
		db_executeStatement(db,POI_CREATE_IND_1);
		db_executeStatement(db,POI_TABLE_VERSION);
		__exit_xx:
		return rv;
}
bool IndPoiCreator::createDatabaseStructure(const char *fileName) 
{
	string fname=fileName;
	string fn="C:\\my\\M_Maps\\mOut\\"+fname+".poi.odb";

	//int fd = open(fn.c_str(), _O_WRONLY | _O_CREAT, _S_IREAD | _S_IWRITE );
	//close(fd); // just create file
	fstream file;
	file.open(fn.c_str(), ios_base::out | ios_base::in);  // will not create file
	if (file.is_open())
	{
		file.close();
		remove(fn.c_str());
	}
	

	poiDB=init_sqlite_db(fn.c_str());
	
	
	bool rv=createMapIndexStructure();
	if(!rv)
	{
		printf("IndPoiCreator::createDatabaseStructure : Failed!!\r\n");
	}
	return rv;
}

