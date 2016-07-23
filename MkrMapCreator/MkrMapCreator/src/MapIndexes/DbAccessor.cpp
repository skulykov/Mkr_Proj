
#include "../SqLiteDB/SqLiteHelper.h"
#include "IndAddrCreator.h"



static const char* get_db_file( )
{
   static char *full_path="C:\\my\\M_Maps\\mOut\\nodes.tmp.odb";

   return full_path;
}

#pragma region DBAccessor methods
void DbAccessor::readTags(Entity *e, unsigned char *tags,int len){
	
	if (tags != 0) {
			
				int prev = 0;
				vector<string> vs;
				for (int i = 0; i < len; i++) {
					if (tags[i] == 0) {
						unsigned char* uc=tags+prev;
						int slen=i - prev;
						//UTF -- TODO
						string s( (char*)uc, slen ) ;//vs.add(new String(tags, prev, i - prev, "UTF-8"));
						vs.push_back(s);
						prev = i + 1;
					}
				}
				for(int i=0; i<vs.size(); i+=2) {
					e->putTag(vs[i], vs[i+1]);
				}
			 
		}
}

void DbAccessor::print_ways()
{
	int             rc = 0;
	const char      *data = NULL;
	sqlite3_stmt    *stmt = NULL;
	sqlite3    *db=getDB();
	 rc = sqlite3_prepare_v2( db, "SELECT w.id, w.node, w.ord, w.tags, w.boundary FROM ways w", -1, &stmt, NULL );
	 if (rc != SQLITE_OK) throw std::exception("print_ways()...");

    while( sqlite3_step( stmt ) == SQLITE_ROW ) {
        
		for(int i=0;i<5;i++)
		{
			data = (const char*)sqlite3_column_text( stmt, i );
			printf( "%s ", data ? data : "[NULL]" );
		}
		printf("\r\n");
    }
    
    sqlite3_finalize( stmt );
}
int DbAccessor::getCounts( const char *str)
{
	int             rc = 0;
	int res = 0;
	sqlite3_stmt    *stmt = NULL;
	sqlite3    *db=getDB();
	 rc = sqlite3_prepare_v2( db, str, -1, &stmt, NULL );
	 if (rc != SQLITE_OK) throw std::exception("getCounts()...");;
	/*
	//http://stackoverflow.com/questions/962032/how-do-i-count-the-number-of-rows-returned-in-my-sqlite-reader-in-c
	SQLite produces records one by one, on request, every time you call sqlite3_step. 
	It simply doesn't know how many there are going to be, until on some sqlite3_step call it discovers there are no more.
	*/
    while( sqlite3_step( stmt ) == SQLITE_ROW ) {
        
		
			res = sqlite3_column_int( stmt, 0 );
			printf( "%d \r\n", res);
		
    }
    
    sqlite3_finalize( stmt );

	return res;
}
#if 0
void DbAccessor::computeRealCounts()
{
	static bool realCounts=false;
	if (!realCounts) {
		
			realCounts = true;
			printf( "Nodes=");
			allNodes=/*getCounts(db, "select max(n.id) from node n ");*/getCounts("select count(distinct n.id) from node n where length(n.tags) > 0");
   			printf( "Ways=");
			allWays=getCounts( "select count(*) from ways w where w.ord = 0");
			printf( "allRelations=");
			int allRelations=getCounts( "select count(distinct r.id) from relations r");
   			printf( "allBoundaries=");
			int allBoundaries=getCounts( "select count(*) from ways w where w.ord = 0 and w.boundary > 0");
	}
}
#endif
#if 0
int DbAccessor::iterateOverEntities(EntityType type, iterateEntity_T visitor_iter)
{
		PreparedStatement select;
		int count = 0;
		select.db=getDB();
		//computeRealCounts();
				
		vector<Entity*> toProcess;
		
		if (type == NODE) {
			// filter out all nodes without tags
			select.sql = iterateNodes;
			count = allNodes;
		} else if (type == WAY) {
			select.sql = iterateWays;
			count = allWays;
		} else if (type == WAY_BOUNDARY) {
			select.sql = iterateWayBoundaries;
			count = allBoundaries;
		} else {
			select.sql = iterateRelations;
			count = allRelations;
		}
		EntityProducer entityProducer(&toProcess, type, select);
		//produce
		int rv=entityProducer.run();
		
		INT_T len=toProcess.size();
		for(int i=0;i<len;i++)
		{
			Entity *entityToProcess=toProcess[i];
			if(visitor_iter!=NULL)
				visitor_iter(entityToProcess, this);
		}
		
		return count;
}
#else
vector<Entity*> e_nodes, e_ways, e_relations, e_boundary;
int DbAccessor::iterateOverEntities(EntityType type, iterateEntity_T visitor_iter)
{
	PreparedStatement select;
	select.db = getDB();
	//computeRealCounts();
	vector<Entity*> *toProcess=null;
	
	if (type == NODE) {
		// filter out all nodes without tags
		select.sql = iterateNodes;
		toProcess = &e_nodes;

	}
	else if (type == WAY) {
		select.sql = iterateWays;
		toProcess = &e_ways;

	}
	else if (type == WAY_BOUNDARY) {
		select.sql = iterateWayBoundaries;
		toProcess = &e_boundary;
	}
	else if (type == RELATION) {
		select.sql = iterateRelations;
		toProcess = &e_relations;
	}
	if (toProcess == null)
		return -1;
	INT_T len = toProcess->size();
	if (len == 0)
	{
		EntityProducer entityProducer(toProcess, type, select);
		int rv = entityProducer.run();
	}
	len = toProcess->size();
	for (int i = 0; i<len; i++)
	{
		Entity *entityToProcess = toProcess->at(i);
		if (visitor_iter != NULL)
			visitor_iter(entityToProcess, this);
	}

	return 1;
}
#endif
//void DbAccessor::processAllCitiesFromDB(registerCity_T reg,IndAddrCreator *_this)
//{
//	PreparedStatement select;
//		int count = 0;
//		select.db=getDB();
//		computeRealCounts();
//				
//		vector<Entity*> toProcess;
//		
//		//if (type == NODE) {
//			// filter out all nodes without tags
//			select.sql = iterateNodes;
//			count = allNodes;
//		//} 
//		EntityProducer entityProducer(&toProcess, EntityType::NODE, select);
//		
//		
//		//produce
//		int rv=entityProducer.run();
//		
//		INT_T len=toProcess.size();
//		for(int i=0;i<len;i++)
//		{
//			Entity *entityToProcess=toProcess[i];
//			if(reg!=NULL)
//				reg(entityToProcess,_this);
//		}
//}
void DbAccessor::loadEntityWay(Way *e)
{
		if (e->getEntityIds().empty()) {
			
			EXEC_STMT_START(this->getDB(),pselectWay,__exit_stmt__);
				sqlite3_bind_int64(stmt, 1, e->getId());
				while( sqlite3_step( stmt ) == SQLITE_ROW ) {		//EXEC_STMT_FINISH;
					LONG_T id=sqlite3_column_int64(stmt,0); 
					int ord = sqlite3_column_int(stmt,1);  // 1 
					if(ord==0)
					{
						READ_TAGS(e,stmt,2);
					}
					double lon=sqlite3_column_double(stmt,4);
					if(lon!=0)//if (rs.getObject(5) != null)
					{
						double lat=sqlite3_column_double(stmt,3);
						//double lon=sqlite3_column_double(stmt,4);
						Node *n = new Node(lat,lon,id);
						e->addNode(n);
						READ_TAGS(n,stmt,5);
					}
					else{
						e->addNode(id);
					}
					
				}
    
				sqlite3_finalize( stmt );
__exit_stmt__:
				if(!ret)
				{
					//printf("DbAccessor::loadEntityWay failed \r\n ");
				}

		}
	}
void DbAccessor::loadEntityRelation(Relation *e) {
		loadEntityRelation(e, 1);
}
void DbAccessor::loadEntityRelation(Relation *e,int level)
{
	if (e->isDataLoaded()) { //data was already loaded, nothing to do
			return;
		}
		linked_map<EntityId*, Entity*> map ;//= new LinkedHashMap<EntityId, Entity>();
		if (e->getMemberIds().empty()) {
			
				EXEC_STMT_START(this->getDB(),pselectRelation,__exit_stmt__);
				sqlite3_bind_int64(stmt, 1, e->getId());
				while( sqlite3_step( stmt ) == SQLITE_ROW ) {		//EXEC_STMT_FINISH;
					LONG_T id=sqlite3_column_int64(stmt,0); 
					int ord = sqlite3_column_int(stmt,3);  // 3 
					int _type=sqlite3_column_int(stmt,1);  // Entity type
					const unsigned char *p=sqlite3_column_text(stmt,2); 
					string role((char*)p);
					if(ord==0)
					{
						const void *bytesPtr=sqlite3_column_blob(stmt,4);
						int len=sqlite3_column_bytes(stmt, 4);
						DbAccessor::readTags(e,(unsigned char*)bytesPtr,len);
					}
					e->addMember(id,(EntityType)_type,role);
				}
    
				sqlite3_finalize( stmt );
__exit_stmt__:
				if(!ret)
				{
					//printf("DbAccessor::loadEntityRelation\r\n ");
				}
		}

		vector<EntityId*> ids = e->getMemberIds() ;
		if (level > 0) 
		{
			for (int k=0;k<ids.size();k++/*EntityId i : ids*/) {
				EntityId *i=ids[k];
				LONG_T iId = i->getId();
				/*if (iId == 313096628)
				{
					iId = iId;
				}*/
				/*if (iId == 313088776)
				{
					iId = iId;
				}*/
				if (i->getType() == EntityType::NODE) {
					EXEC_STMT_START(this->getDB(),pselectNode,__exit_stmt__1);
					sqlite3_bind_int64(stmt, 1, iId);
					Node *n = null;
					while( sqlite3_step( stmt ) == SQLITE_ROW ) {		//EXEC_STMT_FINISH;
						if(n==NULL)
						{
							double lat=sqlite3_column_double(stmt,0);
							double lon=sqlite3_column_double(stmt,1);
							n = new Node(lat, lon, iId);
							const void *bytesPtr=sqlite3_column_blob(stmt,2);
							int len=sqlite3_column_bytes(stmt, 2);
							DbAccessor::readTags(n, (unsigned char*)bytesPtr,len);
						}
					}
					if(n!=NULL) map.insert(pair<EntityId*, Entity*>(i, n));
					sqlite3_finalize( stmt );
	__exit_stmt__1:
					if(!ret)
					{
						//printf("DbAccessor::loadEntityRelation_1\r\n ");
					}

				} 
				else 
				if (i->getType() == EntityType::WAY) {
					Way *way = new Way(iId);
					loadEntityWay(way);
					map.insert(pair<EntityId*, Entity*>(i, way));//map.put(i, way);
				} 
				else if (i->getType() == EntityType::RELATION) {
					Relation *rel = new Relation(iId);
					loadEntityRelation(rel, level - 1);
					map.insert(pair<EntityId*, Entity*>(i, rel));//map.put(i, rel);
				}
			}

			e->initializeLinks(map);
			e->entityDataLoaded();
	}
}
#pragma endregion

#pragma region Init SQLite
sqlite3         *gDB = NULL;

#pragma endregion

//template <typename K, typename V>
//static vector<K> LMGetKeys1(linked_map<K, V>& m) {
//	vector<K> result;
//
//	//for (typename linked_map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
//	//	result.push_back(itr->first);
//	for (auto pair : m) {
//	result.push_back(pair.first);
//	}
//	return result;
//}
//void linked_map_test()
//{
//	linked_map<EntityId*, Entity*> map;//= new LinkedHashMap<EntityId, Entity>();
//
//	Relation *e = new Relation(112233);
//	LONG_T id = 12345;
//	int _type = 1;
//	string role = "outer";
//	e->addMember(id, (EntityType)_type, role);
//
//	id++;
//	e->addMember(id, (EntityType)_type, role);
//
//	id -= 3;
//	e->addMember(id, (EntityType)_type, role);
//
//	id = 1;
//
//	linked_map<EntityId*, string>& map1 = e->getMembersMap();
//	for (auto pair : map1) {
//		//cout << pair.first << " = " << pair.second << endl;
//		EntityId* eid = pair.first;
//		if (eid != 0)
//		{
//			printf("eid=%d\r\n", eid->getId());
//		}
//	}
//
//	vector<EntityId*> vv = stdH::LMGetKeys(map1);
//	for (EntityId* eid : vv)
//	{
//		if (eid != 0)
//		{
//			printf("VV eid=%d\r\n", eid->getId());
//		}
//	}
//
//	/*linked_map<string, string> map;
//	string name, value;
//
//	map.insert("b", "2");
//	map.insert("a", "1");
//	map.insert("q", "44");
//
//	for (auto pair : map) {
//		cout << pair.first << " = " << pair.second << endl;
//	}*/
//
//}

void test_1()
{
	/*vector<Node*> n1,n2;
	for (int i = 0; i < 5; i++)
	{
		Node *n = new Node(0,0,i);
		n1.push_back(n);
	}

	for (int i = 10; i < 15; i++)
	{
		Node *n = new Node(0, 0, i);
		n2.push_back(n);
	}

	stdH::addAll<Node*>(0, n1, n2);

	
		for (Node* eid : n1)
		{
			if (eid != 0)
			{
				printf("VV eid=%d\r\n", eid->getId());
			}
		}
*/


		vector<Boundary*> freeBoundaries, allBoundaries;
		for (int i = 0; i < 8; i++)
		{
			Boundary *n = new Boundary(false);
			n->setBoundaryId(i);
			allBoundaries.push_back(n);
		}


		stdH::addAllV<Boundary*>(&freeBoundaries, &allBoundaries);

		vector<Boundary*> testB;
		testB.push_back(allBoundaries[1]);
		testB.push_back(allBoundaries[3]);
		testB.push_back(allBoundaries[5]);

		stdH::removeAll(freeBoundaries, testB/*stdH::GetValues<City *, Boundary*>(cityBoundaries)*/);

		
		for (Boundary* b : freeBoundaries)
		{
			printf("BB eid=%d\r\n", b->getBoundaryId());
		}
}