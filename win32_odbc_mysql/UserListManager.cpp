#include "stdafx.h"
#include "UserListManager.h"
#include "resource.h"

UserListManager::UserListManager()
{
	mysql = Odbc::GetInstance();
}

UserListManager::~UserListManager()
{
}

bool CALLBACK UserListManager::UserListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_INITDIALOG:
	{
		hUserList = GetDlgItem(hWnd, IDC_SEARCH_LIST_USERLIST);

		hCreateUserID = GetDlgItem(hWnd, IDC_CREATE_EDIT_USERID);
		hCreateUserPW = GetDlgItem(hWnd, IDC_CREATE_EDIT_USERPW);

		hSearchUserID = GetDlgItem(hWnd, IDC_SEARCH_EDIT_USERID);
		hSearchUserPW = GetDlgItem(hWnd, IDC_SEARCH_EDIT_USERPW);

		hSearchUserID = GetDlgItem(hWnd, IDC_DELETE_EDIT_USERID);
		hSearchUserPW = GetDlgItem(hWnd, IDC_DELETE_EDIT_USERPW);
	}
	break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_CREATE_BUTTON_CREATE:
			CreateUser(hWnd);
			break;

		case IDC_SEARCH_BUTTON_SEARCH_CONDITION:
		{
			wchar_t userID[1024];
			wchar_t userPW[1024];

			GetWindowText(hSearchUserID, userID, sizeof(userID));
			GetWindowText(hSearchUserPW, userPW, sizeof(userPW));

			if (wstring(userID) == L"" && wstring(userPW) == L"")
			{
				GetUserListFull(hWnd);
			}
			else
			{
				GetUserByCondition(hWnd);
				GetUserListFull(hWnd);
			}
		}
		break;

		case IDC_DELETE_BUTTON_DELETE_CONDITION:
			DeleteUserByCondition(hWnd);
			break;

		default:
			return false;
		}
	}
	break;

	case WM_CLOSE:
	{
		OutputDebugString(L"close");
		EndDialog(hWnd, 0);
		return true;
	}
	break;

	default:
		return false;
	}
	return true;
}

void UserListManager::GetUserListFull(HWND hWnd)
{
	SendDlgItemMessageA(hWnd, IDC_SEARCH_LIST_USERLIST, LB_RESETCONTENT, 0, 0);

	vector<vector<string>> result = mysql->SelectFrom("gameserver", "users");
	int index = 0;
	for (auto& tuple : result)
	{
		string value = "";
		for (auto& attr = tuple.rbegin(); attr < tuple.rend(); ++attr)
		{
			value += (*attr);
			value += "\t";
		}
		SendDlgItemMessageA(hWnd, IDC_SEARCH_LIST_USERLIST, LB_ADDSTRING, 0, (LPARAM)value.c_str());
	}
}


void UserListManager::CreateUser(HWND hWnd)
{
	wchar_t userID[1024];
	wchar_t userPW[1024];

	GetWindowText(hCreateUserID, userID, sizeof(userID));
	GetWindowText(hCreateUserPW, userPW, sizeof(userPW));

	auto result = mysql->CallCreateUser(wstring(userID), wstring(userPW));
	if (result == true)
		MessageBox(NULL, L"Success!", NULL, NULL);
	else
		MessageBox(NULL, L"Fail...", NULL, NULL);
}

void UserListManager::GetUserByCondition(HWND hWnd)
{
	wchar_t userID[1024];
	wchar_t userPW[1024];

	GetWindowText(hSearchUserID, userID, sizeof(userID));
	GetWindowText(hSearchUserPW, userPW, sizeof(userPW));

	auto result = mysql->CallSearchUser(wstring(userID), wstring(userPW));

	string value = "";
	for (auto& attr = result.rbegin(); attr < result.rend(); ++attr)
	{
		value += (*attr);
		value += "\t";
	}

	SendDlgItemMessageA(hWnd, IDC_SEARCH_LIST_USERLIST, LB_RESETCONTENT, 0, 0);
	SendDlgItemMessageA(hWnd, IDC_SEARCH_LIST_USERLIST, LB_ADDSTRING, 0, (LPARAM)value.c_str());
}

void UserListManager::DeleteUserByCondition(HWND hWnd)
{
	wchar_t userID[1024];
	wchar_t userPW[1024];

	GetWindowText(hSearchUserID, userID, sizeof(userID));
	GetWindowText(hSearchUserPW, userPW, sizeof(userPW));

	std::wstring deleteID = std::wstring(userID);
	std::wstring deletePW = std::wstring(userPW);

	if (deleteID == L"" && deletePW == L"")
		return;

	auto result = mysql->CallDeleteUser(wstring(deleteID), wstring(deletePW));
	if (result == true)
		MessageBox(NULL, L"Delete!", NULL, NULL);
	else
		MessageBox(NULL, L"Fail...", NULL, NULL);
}