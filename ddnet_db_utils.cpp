#include "ddnet_db_utils.h"

#include <base/log.h>

namespace ddnet_db_utils
{

	// hack to avoid editing connection.h in ddnet code
	ESqlBackend DetectBackend(IDbConnection *pSqlServer)
	{
		return str_comp(pSqlServer->BinaryCollate(), "BINARY") == 0 ? ESqlBackend::SQLITE3 : ESqlBackend::MYSQL;
	}

	bool AddIntColumn(IDbConnection *pSqlServer, const char *pTableName, const char *pColumnName, int Default, char *pError, int ErrorSize)
	{
		char aBuf[4096];
		str_format(
			aBuf,
			sizeof(aBuf),
			"ALTER TABLE %s ADD COLUMN %s INTEGER DEFAULT %d;", pTableName, pColumnName, Default);

		if(!pSqlServer->PrepareStatement(aBuf, pError, ErrorSize))
		{
			return false;
		}
		pSqlServer->Print();
		int NumInserted;
		return pSqlServer->ExecuteUpdate(&NumInserted, pError, ErrorSize);
	}

	bool AddColumnIntDefault0Sqlite3(IDbConnection *pSqlServer, const char *pTableName, const char *pColumnName, char *pError, int ErrorSize)
	{
		char aBuf[4096];
		str_copy(
			aBuf,
			"SELECT COUNT() FROM pragma_table_info(?) WHERE name = ?;");
		if(!pSqlServer->PrepareStatement(aBuf, pError, ErrorSize))
		{
			dbg_msg("sql-thread", "prepare failed query: %s", aBuf);
			return false;
		}
		pSqlServer->BindString(1, pTableName);
		pSqlServer->BindString(2, pColumnName);
		pSqlServer->Print();

		bool End;
		if(!pSqlServer->Step(&End, pError, ErrorSize))
		{
			dbg_msg("sql-thread", "step failed query: %s", aBuf);
			return false;
		}

		if(End)
		{
			// we expect 0 or 1 but never nothing
			dbg_msg("sql-thread", "something went wrong failed query: %s", aBuf);
			return false;
		}
		if(pSqlServer->GetInt(1) == 0)
		{
			log_info("sql-thread", "adding missing sqlite3 column '%s' to '%s'", pColumnName, pTableName);
			return !AddIntColumn(pSqlServer, pTableName, pColumnName, 0, pError, ErrorSize);
		}
		return true;
	}

	bool AddColumnIntDefault0Mysql(IDbConnection *pSqlServer, const char *pTableName, const char *pColumnName, char *pError, int ErrorSize)
	{
		char aBuf[4096];
		str_format(
			aBuf,
			sizeof(aBuf),
			"show columns from %s where Field = ?;",
			pTableName);
		if(!pSqlServer->PrepareStatement(aBuf, pError, ErrorSize))
		{
			dbg_msg("sql-thread", "prepare failed query: %s", aBuf);
			return false;
		}
		pSqlServer->BindString(1, pColumnName);
		pSqlServer->Print();

		bool End;
		if(!pSqlServer->Step(&End, pError, ErrorSize))
		{
			dbg_msg("sql-thread", "step failed query: %s", aBuf);
			return false;
		}
		if(!End)
			return true;

		log_info("sql-thread", "adding missing mysql column '%s' to '%s'", pColumnName, pTableName);
		return !AddIntColumn(pSqlServer, pTableName, pColumnName, 0, pError, ErrorSize);
	}

}
