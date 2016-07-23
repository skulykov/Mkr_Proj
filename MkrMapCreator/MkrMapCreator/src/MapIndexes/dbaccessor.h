#ifndef DBACCESSOR_H
#define DBACCESSOR_H


#include "../common/oCls.h"
#include <sqlite3.h>

using namespace std;

class DbAccessor;
class IndAddrCreator;

#define SELECT_NODE_1	"select w.node, w.ord, w.tags, n.latitude, n.longitude, n.tags from ways w left join node n on w.node = n.id where w.id = ? order by w.ord"
#define SELECT_NODE_0   "select w.id, w.node, w.ord, w.tags, w.boundary from ways w where length(w.tags) > 0"

#define pselectNode  "select n.latitude, n.longitude, n.tags from node n where n.id = ?"
#define	pselectWay  "select w.node, w.ord, w.tags, n.latitude, n.longitude, n.tags from ways w left join node n on w.node = n.id where w.id = ? order by w.ord"
#define	pselectRelation "select r.member, r.type, r.role, r.ord, r.tags from relations r where r.id = ? order by r.ord"
		
#define			iterateNodes "select n.id, n.latitude, n.longitude, n.tags from node n where length(n.tags) > 0"
#define			iterateWays  "select w.id, w.node, w.ord, w.tags, n.latitude, n.longitude, n.tags from ways w left join node n on w.node = n.id order by w.id, w.ord"
#define			iterateWayBoundaries  "select w.id, w.node, w.ord, w.tags, n.latitude, n.longitude, n.tags from ways w left join node n on w.node = n.id  where w.boundary > 0 order by w.id, w.ord"
#define			iterateRelations "select r.id, r.tags from relations r where length(r.tags) > 0"


typedef void (*iterateEntity_T)(Entity *e, DbAccessor *ctx);
typedef void (*registerCity_T)(Entity *e,IndAddrCreator *_this) ;



#pragma region class PreparedStatement
struct PreparedStatement
{
	string sql;
	sqlite3         *db;
};
#pragma endregion

#pragma region class DbAccessor
class DbAccessor
{
	sqlite3         *mDB;
public:
	
	INT_T allNodes,allWays,allRelations,allBoundaries;

	DbAccessor(){}
	DbAccessor(sqlite3         *db){mDB=db;}
	sqlite3         *getDB(){return mDB;}
	void print_ways();
	void computeRealCounts();
	int getCounts( const char *str);
	int iterateOverEntities(EntityType type, iterateEntity_T visitor);
	void processAllCitiesFromDB(registerCity_T reg,IndAddrCreator *_this);
	static void readTags(Entity *e, unsigned char *tags,int len);
	void loadEntityRelation(Relation *e);
	void loadEntityRelation(Relation *e,int level);
	void loadEntityWay(Way *e);
};
#pragma endregion

#pragma region class EntityProducer
class EntityProducer
{
	vector<Entity*> *toProcess;
	PreparedStatement select;
	EntityType type;
	bool putEndingEntity;
public:
	EntityProducer(vector<Entity*> *_toProcess,EntityType _type,PreparedStatement _select, bool _putEndingEntity=false)
	{
		toProcess=_toProcess;
		type=_type;
		select=_select;
		putEndingEntity=_putEndingEntity;
	}
	int run()
	{
		int             rc = 0;
		const char      *data = NULL;
		sqlite3_stmt    *stmt = NULL;

		 rc = sqlite3_prepare_v2( select.db, select.sql.c_str(), -1, &stmt, NULL );
		if ( rc != SQLITE_OK) 
		{
			return -1;
		}
		LONG_T prevId = -999999999;
		Entity *prevEntity = 0;
		while( sqlite3_step( stmt ) == SQLITE_ROW ) {
        
			LONG_T curId = sqlite3_column_int64(stmt,0);
			bool newEntity = curId != prevId;
			Entity *e = prevEntity;
					if (type == NODE) {
				        double lat=sqlite3_column_double(stmt,1);
						double lon=sqlite3_column_double(stmt,2);
						e = new Node(lat, lon, curId);
						const void *bytesPtr=sqlite3_column_blob(stmt,3);
						int len=sqlite3_column_bytes(stmt, 3);
						DbAccessor::readTags(e, (unsigned char*)bytesPtr,len);
					} 
					else if (type == WAY || type == WAY_BOUNDARY) {
						if (newEntity) {
							e = new Way(curId);
						}
						int ord = sqlite3_column_int(stmt,2);//rs.getInt(3);
						if (ord == 0) {
							const void *bytesPtr=sqlite3_column_blob(stmt,3);
							int len=sqlite3_column_bytes(stmt, 3);
							DbAccessor::readTags(e, (unsigned char*)bytesPtr,len); //readTags(e, rs.getBytes(4));
							
						}
						__int64 node=sqlite3_column_int64(stmt,1);
						if (/*rs.getObject(6)*/sqlite3_column_value(stmt,5) == 0) {
							((Way*) e)->addNode(/*rs.getLong(2)*/node);
						} else {
							double lat=sqlite3_column_double(stmt,4);
							double lon=sqlite3_column_double(stmt,5);
							Node n(/*rs.getDouble(5)*/lat, /*rs.getDouble(6)*/lon, node);
							//readTags(n, rs.getBytes(7));
							const void *bytesPtr=sqlite3_column_blob(stmt,6);
							int len=sqlite3_column_bytes(stmt, 6);
							DbAccessor::readTags(e, (unsigned char*)bytesPtr,len);
							((Way *) e)->addNode(&n);
						}
					} 
					else {
						e = new Relation(curId);
						//readTags(e, rs.getBytes(2));
						const void *bytesPtr=sqlite3_column_blob(stmt,1);
						int len=sqlite3_column_bytes(stmt, 1);
						DbAccessor::readTags(e, (unsigned char*)bytesPtr,len);
					}
					if (newEntity) {
						if (prevEntity != 0) {
							toProcess->push_back(prevEntity);
						}
						prevEntity = e;
					}
					prevId = curId;
		}
		if (prevEntity != 0) {
			toProcess->push_back(prevEntity);
		}
    
		sqlite3_finalize( stmt );

		return 1;
	}
};
#pragma endregion






int init_sqlite();
void close_sqlite();
void close_sqlite_db(sqlite3    *db);
sqlite3   * init_sqlite_db(const char *file);


#endif  //#ifndef DBACCESSOR_H

