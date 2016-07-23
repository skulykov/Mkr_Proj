#ifndef SQLITEHELPER_H
#define SQLITEHELPER_H

#include <sqlite3.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>


#include <iostream>
#include <fstream>
#include <string>



#define EXEC_STMT_START(db,sql,_label_)	\
		bool ret=false;	\
		int             rc = 0;	\
		sqlite3_stmt    *stmt = NULL;	\
		rc = sqlite3_prepare_v2( db, sql, -1, &stmt, NULL );	\
		if ( rc != SQLITE_OK) goto _label_;	

#define EXEC_STMT_FINISH	\
		 rc = sqlite3_step(stmt); \
		ret = (rc == SQLITE_DONE); \
		sqlite3_finalize( stmt );


bool executeStatement(sqlite3    *db, const char *sql);
#define db_executeStatement(a,b)	rv=executeStatement(a,b); \
	                                if(!rv) goto __exit_xx;

//#define db_bind_text(stmt,ind,s,lbl) \
//	{int lbl##len=s.length();         \
//	sqlite3_bind_text(stmt, ind, s.c_str(),lbl##len,SQLITE_STATIC);}

#define db_bind_text(stmt,ind,s,b,_label_) \
	{							\
	int len=s.length();         \
	b=new char[len];      \
	memcpy(b,s.c_str(),len);  \
	rc = sqlite3_bind_text(stmt, ind, b,len,SQLITE_STATIC); \
	if ( rc != SQLITE_OK) goto _label_;  \
	}


#define READ_TAGS(e,stmt,ind) \
	{ \
	const void *bytesPtr=sqlite3_column_blob(stmt,ind); \
	int len=sqlite3_column_bytes(stmt, ind); \
	DbAccessor::readTags(e,(unsigned char*)bytesPtr,len); \
	}




int init_sqlite();
void close_sqlite();
sqlite3   * init_sqlite_db(const char *file);
void close_sqlite_db(sqlite3    *db);

#endif
