#include "BaseDB.h"

BaseDB::BaseDB(char *fname) {
	int len = strlen(fname) + 1;
	this->fileName = new char[len];
	strcpy(this->fileName, fname);
	this->fileName[len - 1] = 0;

	if (pthread_mutex_init(&lockMutex, NULL) != 0)
	{
		//printf("\n mutex init failed\n");
		//return 1;
	}

}
 BaseDB::~BaseDB()
{
	delete[] this->fileName;
}

 bool BaseDB::IsTableExist(SQLiteStatement *pStmt, std::string tableName)
 {

	 pStmt->Sql("select * from sqlite_master where type='table' and name=?");
	 pStmt->BindString(1, tableName);		// bind tableName
	 int rv = 0;
	 while (pStmt->FetchRow())
	 {
		 rv = pStmt->GetDataCount();
	 }
	 // pStmt->GetColumnInt(0);//pStmt->GetColumnCount();
	 //const char *res = pStmt->GetColumnName(0);
	 pStmt->FreeQuery();
	 return rv > 0;
 }

#if defined(ANDROID)
 std::string BaseDB::createPath()
 {
	 char dir[128 * 2];
	 memset(dir, 0, sizeof(dir));
	 strcpy(dir, "/mnt/sdcard/unidriver/");
	 int rv = -1;
	 struct stat st = { 0 };
	 if (stat(dir, &st) == -1) {
		 rv = mkdir(dir, 0777);
	 }

	 strcat(dir, "db/");
	 st = { 0 };
	 if (stat(dir, &st) == -1) {
		 rv = mkdir(dir, 0777);
	 }

	 //strcat(dir, "/");

	 strcat(dir, this->fileName);
	 std::string res = std::string(dir);

	 //int x=	mkpath(rv, 0777);
	 return res;
 }

#elif defined(WIN32)
 std::string BaseDB::createPath()
 {
	 char dir[128 * 2];
	 memset(dir, 0, sizeof(dir));
	 strcpy(dir, "c:\\my\\mkrproj\\newunidriver\\sqlite_db\\");
	 strcat(dir, this->fileName);
	 std::string res = std::string(dir);

	 //int x=	mkpath(rv, 0777);
	 return res;
 }
#endif
 SQLiteStatement* BaseDB::CreateConnection()
 {
	 pthread_mutex_lock(&lockMutex);
	 std::string fullPath = createPath();
	 if (fullPath.length() > 0)
	 {
		 pDatabase = new SQLiteDatabase(fullPath.c_str(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
		 pStmt = new SQLiteStatement(pDatabase);
	 }
	 return pStmt;
 }

// SQLiteStatement *BaseDB::GetStmt() { return pStmt; }
 SQLiteDatabase *BaseDB::GetDB() { return pDatabase; }
 int BaseDB::GetNuberOfRowsUpdated()
 {
	 //get the number of rows updated
	 int tmp = pDatabase->GetDatabaseChanges();
	 return tmp;
 }
 void BaseDB::CloseConn()
 {
	 if (pStmt != 0)delete pStmt;
	 if (pDatabase != 0)delete pDatabase;
	 pthread_mutex_unlock(&lockMutex);
 }

 void BaseDB::CreateTableA(SQLiteStatement *pStmt, std::string sql)
 {
	 pStmt->SqlStatement(sql);

 }

