// This file can be included several times.
//  ^
//  |
// this is a hack for the lint so we can use
// pragma once instead of using a includ guard with
// path name, because the path name is chosen by the
// user of this library
#pragma once

#include <engine/server/databases/connection.h>

namespace ddnet_db_utils
{

	enum class ESqlBackend
	{
		SQLITE3,
		MYSQL
	};

	// hack to avoid editing connection.h in ddnet code
	ESqlBackend DetectBackend(IDbConnection *pSqlServer);
	bool AddIntColumn(IDbConnection *pSqlServer, const char *pTableName, const char *pColumnName, int Default, char *pError, int ErrorSize);
	bool AddColumnIntDefault0Sqlite3(IDbConnection *pSqlServer, const char *pTableName, const char *pColumnName, char *pError, int ErrorSize);
	bool AddColumnIntDefault0Mysql(IDbConnection *pSqlServer, const char *pTableName, const char *pColumnName, char *pError, int ErrorSize);

}
