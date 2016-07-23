#ifndef PROCESS_PBF_DB_H
#define PROCESS_PBF_DB_H
#include "KompexSQLitePrerequisites.h"
#include "KompexSQLiteDatabase.h"
#include "KompexSQLiteStatement.h"
#include "KompexSQLiteException.h"
#include "KompexSQLiteStreamRedirection.h"
#include "KompexSQLiteBlob.h"

using namespace std;
using namespace CanalTP;
using namespace Kompex;

struct NodeDB
{
	uint64_t id;
	double lat, lon;
	unsigned char *pBuf;
	int len;

	NodeDB() {
		pBuf = 0;
	}
	~NodeDB() {
		if (pBuf != 0)
			delete[] pBuf;
	}
};
struct WayDB
{
	uint64_t id;
	vector<uint64_t> node_ids;
	int boundary;
	unsigned char *pBuf;
	int len;

	WayDB() {
		pBuf = 0;
	}
	~WayDB() {
		if (pBuf != 0)
			delete[] pBuf;
		node_ids.clear();
	}
};
struct RelationDB
{
	uint64_t id;
	References refs;
	unsigned char *pBuf;
	int len;

	RelationDB() {
		pBuf = 0;
	}
	~RelationDB() {
		if (pBuf != 0)
			delete[] pBuf;
		refs.clear();
			
	}
};
#endif
