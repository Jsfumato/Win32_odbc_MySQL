#pragma once
#include <Windows.h>
#include <vector>
#include "sql.h"
#include "sqlext.h"

using namespace std;

class Odbc
{
public:
	static Odbc* _Inst;
	static Odbc* Odbc::GetInstance();

	bool	IsConnect() { return _IsConnect; }

	bool	Connect(wchar_t* odbcName, wchar_t* mysqlId, wchar_t* password);
	void	DisConnect();

	std::vector<std::vector<std::string>> SelectFrom(std::string dbName, std::string tableName);

	//bool	CallStoredProcedure(std::wstring spName);

	bool CallCreateUser(std::wstring userID, std::wstring userPW);

	//bool CallCreateUser(std::string userID, std::string userPW);

	bool CallDeleteUser(std::wstring userID, std::wstring userPW);

	vector<string> CallSearchUser(std::wstring userID, std::wstring userPW);

private:
	Odbc();
	~Odbc();

	bool		_IsConnect = false;
	SQLHENV		_hEnv;
	SQLHDBC		_hDbc;
	SQLHSTMT	_hStmt;
};