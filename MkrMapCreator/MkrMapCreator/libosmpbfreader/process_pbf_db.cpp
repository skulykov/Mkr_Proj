

#include "osmpbfreader.h"

#include "common/process_pbf_db.h"

#include "common/MapUtils.h"
#include "common/Utils.h"


vector<NodeDB*> nodes_db;
vector<WayDB*> ways_db;
vector<RelationDB*> relations_db;


// do not store these tags in the database, just ignore them
const string tagsToIgnore[6] = { "created_by","source","converted_by","geobase","geological","statscan" };


SQLiteDatabase *pDatabase = 0;
SQLiteStatement *pStmt = 0;

#define nodeStat "insert into node values (?, ?, ?, ?)"
#define wayStat "insert into ways values (?, ?, ?, ?, ?)"
#define relStat "insert into relations values (?, ?, ?, ?, ?, ?)"
bool createNodesDB(const char *db_name)
{
	pDatabase = new SQLiteDatabase(db_name, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
	pStmt = new SQLiteStatement(pDatabase);

	bool rv=true;
	pStmt->SqlStatement( "create table node(id bigint primary key, latitude double, longitude double, tags blob)");
	pStmt->SqlStatement( "create index IdIndex ON node (id)");

	pStmt->SqlStatement( "create table ways (id bigint, node bigint, ord smallint, tags blob, boundary smallint, primary key (id, ord))");
	pStmt->SqlStatement( "create index IdWIndex ON ways (id)");

	pStmt->SqlStatement( "create table relations (id bigint, member bigint, type smallint, role varchar(1024), ord smallint, tags blob, primary key (id, ord))");
	pStmt->SqlStatement( "create index IdRIndex ON relations (id)");

	return rv;

}
bool tagsToRemoveContains(string str)
{
	for (int i = 0; i < 6; i++) {
		string s = tagsToIgnore[i];
		if (stdH::contains(str,s))
		{
			return true;
		}
	}
	return false;
}
int processTags(unsigned char *_tags, const Tags &tags,int *boundary)
{
	//unsigned char _tags[1024];  // size should be enough to keep all tags  -> No size check here !!!!
	int index = 0;
	for (const auto& pair : tags)
	{
		string key = pair.first;
		if (tagsToRemoveContains(key))
			continue;
		int len = key.length();
		memcpy((_tags + index), key.c_str(), len);
		index += len;
		_tags[index++] = 0;

		string value = pair.second;
		len = value.length();
		memcpy((_tags + index), value.c_str(), len);
		index += len;
		_tags[index++] = 0;
		if(boundary!=0)
		{
			if (key == "boundary" && !value.empty())
			{
				*boundary= 1;
			}
		}
	}
	return index;
}

/*Try using SQLITE_STATIC instead of SQLITE_TRANSIENT for those inserts. 

SQLITE_TRANSIENT will cause SQLite to copy the string data before returning. 

SQLITE_STATIC tells it that the memory address you gave it will be valid until the query has been performed (which in this loop is always the case). This will save you several allocate, copy and deallocate operations per loop. Possibly a large improvement.

*/
#if 0
static bool write2DB()
{
	//1.
	pStmt->Sql(nodeStat);
	for (NodeDB *v : nodes_db)
	{
		// bind all three values
		pStmt->BindInt64(1, v->id);					
		pStmt->BindDouble(2, v->lat);			
		pStmt->BindDouble(3, v->lon);				
		pStmt->BindBlob(4, v->pBuf,v->len);
												// execute the statement and reset the bindings
		pStmt->Execute();
		pStmt->Reset();
	}
	// we don't need the prepared statement anymore so we clean-up everything
	pStmt->FreeQuery();

	//2.
	pStmt->Sql(wayStat);
	for (WayDB *v : ways_db)
	{
		uint64_t id = v->id;
		int boundary = v->boundary;
		int ord = 0;
		for (uint64_t x : v->node_ids)
		{
			// bind all three values
			pStmt->BindInt64(1,id);
			pStmt->BindInt64(2, x);
			pStmt->BindInt64(3, ord);
			if(ord==0)
				pStmt->BindBlob(4, v->pBuf, v->len);
			pStmt->BindInt(3, boundary);
			ord++;
			// execute the statement and reset the bindings
			pStmt->Execute();
			pStmt->Reset();
		}
	}
	// we don't need the prepared statement anymore so we clean-up everything
	pStmt->FreeQuery();


	//3.
	pStmt->Sql(relStat);
	for (RelationDB *v : relations_db)
	{
		uint64_t id = v->id;
		int ord = 0;
		for (Reference r : v->refs)
		{
			// bind all three values
			pStmt->BindInt64(1, id);
			pStmt->BindInt64(2, r.member_id);
			pStmt->BindInt64(3, r.member_type);
			pStmt->BindString(4, r.role);
			pStmt->BindInt64(5, ord);
			if (ord == 0)
				pStmt->BindBlob(6, v->pBuf, v->len);
			
			ord++;
			// execute the statement and reset the bindings
			pStmt->Execute();
			pStmt->Reset();
		}
	}
	// we don't need the prepared statement anymore so we clean-up everything
	pStmt->FreeQuery();


	return true;
}
#else
//http://stackoverflow.com/questions/1711631/improve-insert-per-second-performance-of-sqlite
static bool write2DB()
{
	pStmt->Sql(nodeStat);
	pStmt->StartTransactionCmd();
	//1.
	
	for (NodeDB *v : nodes_db)
	{
		uint64_t id = v->id;
		//if (/*id == 1578643674*/id<0)
		//{
		//	id = id;
		//}
		// bind all three values
		pStmt->BindInt64(1, id);
		pStmt->BindDouble(2, v->lat);
		pStmt->BindDouble(3, v->lon);
		pStmt->BindBlob(4, v->pBuf, v->len);
		// execute the statement and reset the bindings
		pStmt->Execute();
		pStmt->Reset();
	}
	
	pStmt->EndTransactionCmd();
	// we don't need the prepared statement anymore so we clean-up everything
	pStmt->FreeQuery();

	//2.
	pStmt->Sql(wayStat);
	pStmt->StartTransactionCmd();
	
	for (WayDB *v : ways_db)
	{
		uint64_t id = v->id;
		/*if (id == 313088776)
		{
			id = id;
		}*/
		int boundary = v->boundary;
		int ord = 0;
		for (uint64_t x : v->node_ids)
		{
			// bind all three values
			pStmt->BindInt64(1, id);
			pStmt->BindInt64(2, x);
			pStmt->BindInt64(3, ord);
			if (ord == 0)
				pStmt->BindBlob(4, v->pBuf, v->len);
			pStmt->BindInt(5, boundary);
			ord++;
			// execute the statement and reset the bindings
			pStmt->Execute();
			pStmt->Reset();
		}
	}
	pStmt->EndTransactionCmd();
	// we don't need the prepared statement anymore so we clean-up everything
	pStmt->FreeQuery();


	//3.
	pStmt->Sql(relStat);
	pStmt->StartTransactionCmd();
	for (RelationDB *v : relations_db)
	{
		uint64_t id = v->id;
		/*if (id == 324211)
		{
			id = -1;
		}*/
		int ord = 0;
		for (Reference r : v->refs)
		{
			// bind all three values
			pStmt->BindInt64(1, id);
			pStmt->BindInt64(2, r.member_id);
			pStmt->BindInt(3, r.member_type);
			pStmt->BindString(4, r.role);
			pStmt->BindInt(5, ord);
			if (ord == 0)
				pStmt->BindBlob(6, v->pBuf, v->len);

			ord++;
			// execute the statement and reset the bindings
			pStmt->Execute();
			pStmt->Reset();
		}
	}
	pStmt->EndTransactionCmd();
	// we don't need the prepared statement anymore so we clean-up everything
	pStmt->FreeQuery();


	return true;
}

#endif
void cleanUp()
{
	for (int i = 0; i < nodes_db.size(); i++)
	{
		NodeDB *n = nodes_db[i];
		delete n;
	}
	nodes_db.clear();

	for (int i = 0; i < ways_db.size(); i++)
	{
		WayDB *n = ways_db[i];
		delete n;
	}
	ways_db.clear();

	for (int i = 0; i < relations_db.size(); i++)
	{
		RelationDB *n = relations_db[i];
		delete n;
	}
	relations_db.clear();
}

// We need to define a visitor with three methods that will be called while the file is read
struct Counter {
    // Three integers count how many times each object type occurs
    int nodes;
    int ways;
    int relations;

    Counter() : nodes(0), ways(0), relations(0) {}

    // This method is called every time a Node is read
    void node_callback(uint64_t osmid, double lon, double lat, const Tags &tags){
		
        ++nodes;
		NodeDB *node=new NodeDB();
		node->id = osmid;
		node->lat = lat;
		node->lon = lon;
		
		if (tags.size() > 0)
		{
			unsigned char _tags[1024] = { 0 };  // size should be enough to keep all tags  -> No size check here !!!!
			int index = processTags(_tags, tags, 0);

			node->pBuf = new unsigned char[index];
			memcpy(node->pBuf, _tags, index);
			node->len = index;
		}
		else
		{
			node->pBuf = 0;
			node->len = 0;
		}
		nodes_db.push_back(node);
		
    }

    // This method is called every time a Way is read
    // refs is a vector that contains the reference to the nodes that compose the way
    void way_callback(uint64_t osmid, const Tags &tags, const std::vector<uint64_t> &refs){
        ++ways;
		int boundary = 0;
		short ord = 0;
		

		WayDB *way = new WayDB();
		way->id = osmid;
		way->node_ids = refs; // will it copy ???
		if (tags.size() > 0)
		{
			unsigned char _tags[1024] = { 0 };  // size should be enough to keep all tags  -> No size check here !!!!
			int index = processTags(_tags, tags, &boundary);

			way->pBuf = new unsigned char[index];
			memcpy(way->pBuf, _tags, index);
			way->len = index;
		}
		else
		{
			way->pBuf = NULL;
			way->len = 0;
		}
		ways_db.push_back(way);
		
    }

    // This method is called every time a Relation is read
    // refs is a vector of pair corresponding of the relation type (Node, Way, Relation) and the reference to the object
    void relation_callback(uint64_t osmid, const Tags &tags, const References & refs){
        ++relations;
		short ord = 0;

		
		RelationDB *rel=new RelationDB();
		rel->id = osmid;
		rel->refs = refs; // will it copy ???
		if (tags.size() > 0)
		{
			unsigned char _tags[1024] = { 0 };  // size should be enough to keep all tags  -> No size check here !!!!
			int index = processTags(_tags, tags, 0);

			rel->pBuf = new unsigned char[index];
			memcpy(rel->pBuf, _tags, index);
			rel->len = index;
		}
		else
		{
			rel->pBuf = NULL;
			rel->len = 0;
		}
		relations_db.push_back(rel);
		/*
		for (Reference v : refs)
		{
			EXEC_STMT_START(nodesDB, nodeStat, __exit_stmt__);
			sqlite3_bind_int64(stmt, 1, osmid);
			sqlite3_bind_int64(stmt, 2, v.member_id);
			sqlite3_bind_int64(stmt, 3, v.member_type);
			sqlite3_bind_text(stmt, 4, v.role.c_str(), v.role.length(), SQLITE_STATIC);
			sqlite3_bind_int64(stmt, 5, ord++);
			if (ord == 0) {
				sqlite3_bind_blob(stmt, 6, _tags, index, SQLITE_STATIC);
			}
			

			EXEC_STMT_FINISH;
		__exit_stmt__:
			if (!ret)
			{
				throw std::exception("relation_callback exc");
			}
		}
		*/
    }
};

int main_process_pbf(int argc, char** argv) {
     if(argc < 2) {
         std::cout << "Usage: " << argv[0] << " file_to_read.osm.pbf" << std::endl;
         return 1;
     }

	 string fname = string(argv[2]);
	 string fn = "C:\\my\\M_Maps\\mOut\\" + fname + ".nodes.tmp.odb";
	 
	 createNodesDB(fn.c_str());

	// Let's read that file !
     Counter counter;
     read_osm_pbf(argv[1], counter);
     std::cout << "We read " << counter.nodes << " nodes, " << counter.ways << " ways and " << counter.relations << " relations" << std::endl;
    
	 if (!write2DB())
	 {
		 throw std::exception("write2DB() return false");
	 }

	 // clean-up
	 if(pStmt!=0) delete pStmt;
	 if(pDatabase!=0) delete pDatabase;

	 cleanUp();
	 return 0;
}
