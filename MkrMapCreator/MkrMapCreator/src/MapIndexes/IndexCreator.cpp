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
void iterateEntity_address_relations_1(Entity *e, DbAccessor *ctx)
{
	indexAddrCreator.indexBoundariesRelation(e, ctx);
}
void iterateEntity_address_relations_2(Entity *e, DbAccessor *ctx)
{
	indexAddrCreator.indexBoundariesRelation((Relation*)e, ctx);
}
void iterateEntity_main(Entity *e, DbAccessor *ctx)
{
	/*if (indexPOI) {
		indexPoiCreator.iterateEntity(e, ctx);
	}
	if (indexTransport) {
		indexTransportCreator.visitEntityMainStep(e, ctx);
	}*/
	//if (indexMap) {
		indexMapCreator.iterateMainEntity(e, ctx);
	//}
	//if (indexAddress) {
		indexAddrCreator.iterateMainEntity(e, ctx);
	//}
	//if (indexRouting) {
		indexRouteCreator.iterateMainEntity(e, ctx);
	//}
}
#pragma endregion
void registerCityIfNeeded(Entity *e, DbAccessor *ctx)
{
	indexAddrCreator.registerCityIfNeeded(e, ctx);
}
void generateIndexes(const char *fileName  )
{
	init_sqlite();
	// 1. creating nodes db to fast access for all nodes and simply import all relations, ways, nodes to it
	string fname=fileName;
	string fn="C:\\my\\M_Maps\\mOut\\"+fname+".nodes.tmp.odb";
	sqlite3    *nodesDB=init_sqlite_db(fn.c_str());
	DbAccessor accessor(nodesDB);
	
	// 2. Create index connections and index structure
	sqlite3    *tmpDB =createDatabaseIndexesStructure(fileName);

	
	//******** 3. Processing all entries********
	printf(" 3.1 write all cities \r\n");
	accessor.iterateOverEntities(EntityType::NODE, registerCityIfNeeded);//accessor.processAllCitiesFromDB((registerCity_T)&IndAddrCreator::registerCityIfNeeded,&indexAddrCreator);
	indexAddrCreator.writeCitiesIntoDb();
	// 3.2 index address relations
	printf("[30 / 100] \r\n");
	accessor.iterateOverEntities(EntityType::RELATION,iterateEntity_address_relations );

	printf("[40 / 100] \r\n");
	accessor.iterateOverEntities(EntityType::NODE, iterateEntity_address_relations_1);

	printf("[42 / 100] \r\n");
	indexAddrCreator.bindCitiesWithBoundaries();

	//"[45 / 100]"
	//accessor.iterateOverEntities(EntityType::RELATION, iterateEntity_address_relations_2);


	// 3.3 MAIN iterate over all entities
	printf("[50 / 100] \r\n");
	accessor.iterateOverEntities(EntityType::NODE, iterateEntity_main);

	printf("[60 / 100] \r\n");
	accessor.iterateOverEntities(EntityType::WAY, iterateEntity_main);   //indexRouteCreator.ReadJunctions(e, ctx); ???

	printf("[85 / 100] \r\n");
	accessor.iterateOverEntities(EntityType::RELATION, iterateEntity_main);  




	printf("Finish...\r\n");
	indexPoiCreator.close_db();
	close_sqlite_db(tmpDB);
	close_sqlite_db(nodesDB);
	close_sqlite();

	
}
