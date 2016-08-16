#include <iostream>
#include <fstream>
#include <string>
#include <clocale>
#include <codecvt>

#include "format.h"
#include "OdbcLib.h"

using namespace std;

Odbc* Odbc::_Inst = nullptr;

Odbc::Odbc()
	:_hEnv(nullptr), _hDbc(nullptr), _hStmt(nullptr)
{}

Odbc::~Odbc()
{
	_Inst = nullptr;
}

Odbc* Odbc::GetInstance()
{
	if (_Inst == nullptr)
		_Inst = new Odbc();
	else
		return _Inst;
}

bool Odbc::Connect(wchar_t* odbcName, wchar_t* mysqlId, wchar_t* password)
{
	int ret;

	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_hEnv);
	ret = SQLSetEnvAttr(_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	ret = SQLAllocHandle(SQL_HANDLE_DBC, _hEnv, &_hDbc);

	ret = SQLConnect(
		_hDbc,
		(SQLWCHAR*)odbcName, SQL_NTS,
		(SQLWCHAR*)mysqlId, SQL_NTS,
		(SQLWCHAR*)password, SQL_NTS
	);

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		_IsConnect = true;
		ret = SQLAllocHandle(SQL_HANDLE_STMT, _hDbc, &_hStmt);
		return true;
	}
	else
		return false;
}

void Odbc::DisConnect()
{
	if (_hStmt != nullptr)
		SQLFreeHandle(SQL_HANDLE_STMT, _hStmt);
	if (_hDbc != nullptr)
		SQLDisconnect(_hDbc);
	if (_hDbc != nullptr)
		SQLFreeHandle(SQL_HANDLE_DBC, _hDbc);
	if (_hEnv != nullptr)
		SQLFreeHandle(SQL_HANDLE_ENV, _hEnv);
	_IsConnect = false;
}

//bool Odbc::PushQuery(std::wstring query)
//{
//	if (!_IsConnect || _Inst == nullptr)
//		return false;
//
//	SQLWCHAR* sql = (SQLWCHAR*)query.c_str();
//	int ret = SQLExecDirect(_hStmt, sql, SQL_NTS);
//
//	if (ret == SQL_SUCCESS)
//		return true;
//	else
//		return false;
//}

// http://stackoverflow.com/questions/17156998/c11-variadic-templates-return-tuple-from-variable-list-of-vectors
// http://stackoverflow.com/questions/20407753/can-c11-parameter-packs-be-used-outside-templates
//template<typename... Ts>

vector<vector<string>> Odbc::SelectFrom(std::string dbName, std::string tableName)
{
	//auto recordType = std::vector<std::string>();
	std::vector<std::vector<std::string>> retVector = std::vector<std::vector<std::string>>();

	if (!_IsConnect || _Inst == nullptr)
		return retVector;

	std::string query = fmt::format("SELECT * FROM {0}.{1}", dbName, tableName);
	
	SQLCHAR* sql = (SQLCHAR*)query.c_str();

	int ret = SQLAllocHandle(SQL_HANDLE_STMT, _hDbc, &_hStmt);
	ret = SQLExecDirectA(_hStmt, sql, SQL_NTS);

	if (ret == SQL_SUCCESS)
	{
		SQLLEN resultLen;
		SQLSMALLINT columns;

		while (TRUE)
		{
			ret = SQLFetch(_hStmt);
			SQLNumResultCols(_hStmt, &columns);

			if (ret == SQL_NO_DATA || ret == SQL_ERROR || ret == SQL_SUCCESS_WITH_INFO)
				return retVector;

			//			SELECT
			if (ret == SQL_SUCCESS)
			{
				auto record = std::vector<std::string>();
				for (int i = 1; i <= columns; ++i)
				{
					char strResult[200];
					SQLGetData(_hStmt, i, SQL_C_CHAR, strResult, 200, &resultLen);
					record.push_back(strResult);
				}
				retVector.push_back(record);
			}
		}
		return retVector;
	}
}
//
//bool Odbc::CallStoredProcedure(std::wstring spName)
//{
//	if (!_IsConnect || _Inst == nullptr)
//		return false;
//
//	std::wstring query = L"call " + spName;
//
//	SQLWCHAR* sql = (SQLWCHAR*)query.c_str();
//	int ret = SQLAllocHandle(SQL_HANDLE_STMT, _hDbc, &_hStmt);
//	ret = SQLExecDirect(_hStmt, sql, SQL_NTS);
//
//	if (ret == SQL_SUCCESS)
//		return true;
//	else
//		return false;
//}

bool Odbc::CallCreateUser(std::wstring userID, std::wstring userPW)
{
	if (!_IsConnect || _Inst == nullptr)
		return false;

	std::wstring query = fmt::format(L"call CreateUser('{0}', '{1}')", userID, userPW);

	SQLWCHAR* sql = (SQLWCHAR*)query.c_str();
	int ret = SQLAllocHandle(SQL_HANDLE_STMT, _hDbc, &_hStmt);
	ret = SQLExecDirect(_hStmt, sql, SQL_NTS);

	if (ret == SQL_SUCCESS)
		return true;
	else
		return false;
}

bool Odbc::CallDeleteUser(std::wstring userID, std::wstring userPW)
{
	if (!_IsConnect || _Inst == nullptr)
		return false;

	std::wstring query = fmt::format(L"call DeleteUser('{0}', '{1}')", userID, userPW);

	SQLWCHAR* sql = (SQLWCHAR*)query.c_str();
	int ret = SQLAllocHandle(SQL_HANDLE_STMT, _hDbc, &_hStmt);
	ret = SQLExecDirect(_hStmt, sql, SQL_NTS);

	if (ret == SQL_SUCCESS)
		return true;
	else
		return false;
}

vector<string> Odbc::CallSearchUser(std::wstring userID, std::wstring userPW)
{
	std::vector<std::string> tuple = std::vector<std::string>();

	if (!_IsConnect || _Inst == nullptr)
		return tuple;

	std::wstring query = fmt::format(L"call SearchUser('{0}', '{1}')", userID, userPW);

	SQLWCHAR* sql = (SQLWCHAR*)query.c_str();
	int ret = SQLAllocHandle(SQL_HANDLE_STMT, _hDbc, &_hStmt);
	ret = SQLExecDirect(_hStmt, sql, SQL_NTS);

	if (ret == SQL_SUCCESS)
	{
		SQLLEN resultLen;
		SQLSMALLINT columns;

		while (TRUE)
		{
			ret = SQLFetch(_hStmt);
			SQLNumResultCols(_hStmt, &columns);

			if (ret == SQL_NO_DATA || ret == SQL_ERROR || ret == SQL_SUCCESS_WITH_INFO)
				return tuple;

			//			SELECT
			if (ret == SQL_SUCCESS)
			{
				for (int i = 1; i <= columns; ++i)
				{
					char strResult[200];
					SQLGetData(_hStmt, i, SQL_C_CHAR, strResult, 200, &resultLen);
					tuple.push_back(strResult);
				}
			}
		}
		return tuple;
	}
	else
		return tuple;
}