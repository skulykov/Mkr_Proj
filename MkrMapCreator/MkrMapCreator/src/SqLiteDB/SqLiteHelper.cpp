
#include "SqLiteHelper.h"
#include <exception>
#include <typeinfo>
#include <stdexcept>

int init_sqlite() { sqlite3_initialize(); return 1; }
void close_sqlite() { sqlite3_shutdown(); }
sqlite3   * init_sqlite_db(const char *file)
{

	sqlite3         *db = NULL;



	int rc = sqlite3_open_v2(file, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	if (rc != SQLITE_OK) {
		sqlite3_close(db);
		throw std::exception("Init SqLite DB...");;
	}


	return db;

}
void close_sqlite_db(sqlite3    *db)
{
	sqlite3_close(db);

}

bool executeStatement(sqlite3    *db, const char *sql)
{
	EXEC_STMT_START(db, sql, __exit_stmt);
	EXEC_STMT_FINISH;
__exit_stmt:
	return ret;
}