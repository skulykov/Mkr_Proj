


#include "DbAccessor.h"
#include "IndAddrCreator.h"
#include "IndVMapCreator.h"
#include "IndPoiCreator.h"
#include "IndRouteCreator.h"
#include "IndTransportCreator.h"

//global vars
IndVMapCreator indexMapCreator;
IndAddrCreator indexAddrCreator;
IndPoiCreator  indexPoiCreator;
IndRouteCreator indexRouteCreator;
IndTransportCreator indexTransportCreator;

using namespace std;

sqlite3    * createDatabaseIndexesStructure(const char *fileName)
{
	// 2.1 create temporary sqlite database to put temporary results to it
	string fname=fileName;
	string fn="C:\\my\\M_Maps\\mOut\\"+fname+".obf.tmp";

	//int fd = open(fn.c_str(), _O_WRONLY | _O_CREAT, _S_IREAD | _S_IWRITE );
	//close(fd); // just create file
	fstream file;
	file.open(fn.c_str(), ios_base::out | ios_base::in);  // will not create file
	if (file.is_open())
	{
		file.close();
		remove(fn.c_str());
	}
	

	sqlite3    *db=init_sqlite_db(fn.c_str());

	// 2.2 create rtree map -> Make sure It creates RTrees as well
	indexMapCreator.createDatabaseStructure(db,fileName);
	indexRouteCreator.createDatabaseStructure(db,fileName);
	indexAddrCreator.createDatabaseStructure(db,fileName);
	indexPoiCreator.createDatabaseStructure(fileName);
	indexTransportCreator.createDatabaseStructure(db,fileName);
		
	return db;
}

#pragma region iterateOverEntity Callback functions

void iterateEntity_address_relations(Entity *e, DbAccessor *ctx)
{
	indexAddrCreator.indexBoundariesRelation((Relation*) e, ctx);
	indexMapCreator.indexMapRelationsAndMultiPolygons(e, ctx);
//	indexRouteCreator.indexRelations(e, ctx);
}
#pragma endregion
void registerCityIfNeeded(Entity *e) ;
void generateIndexes(const char *fileName  )
{
	init_sqlite();
	string fname=fileName;
	string fn="C:\\my\\M_Maps\\mOut\\"+fname+".nodes.tmp.odb";
	sqlite3    *nodesDB=init_sqlite_db(fn.c_str());
	DbAccessor accessor(nodesDB);
	
	
	// 1. creating nodes db to fast access for all nodes and simply import all relations, ways, nodes to it
	//TODO		boolean loadFromExistingFile = createPlainOsmDb(progress, readFile, addFilter, false);
	accessor.processAllCitiesFromDB((registerCity_T)&IndAddrCreator::registerCityIfNeeded,&indexAddrCreator);

	// 2. Create index connections and index structure
	sqlite3    *tmpDB =createDatabaseIndexesStructure(fileName);

	
	//******** 3. Processing all entries********
	// 3.1 write all cities
	indexAddrCreator.writeCitiesIntoDb();
	// 3.2 index address relations
	accessor.iterateOverEntities(EntityType::RELATION,iterateEntity_address_relations );
	

	//	accessor.iterateOverEntities(NODE, NULL);


	printf("Finish...\r\n");
	indexPoiCreator.close_db();
	close_sqlite_db(tmpDB);
	close_sqlite_db(nodesDB);
	close_sqlite();

	
}
