#pragma once
#include <Windows.h>
#include <OdbcLib.h>

class UserListManager
{
public:
	UserListManager();
	~UserListManager();

	bool CALLBACK UserListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void GetUserListFull(HWND hWnd);
	void CreateUser(HWND hWnd);
	void GetUserByCondition(HWND hWnd);
	void DeleteUserByCondition(HWND hWnd);

	void SetHWND(HWND hWnd)	{ hUserListWnd = hWnd; }
	HWND GetHWND(){	return hUserListWnd; }

private:
	Odbc* mysql;
	HWND hUserListWnd = nullptr;

	HWND hUserList = nullptr;
	HWND hCreateUserID = nullptr;
	HWND hCreateUserPW = nullptr;

	HWND hSearchUserID = nullptr;
	HWND hSearchUserPW = nullptr;
};

