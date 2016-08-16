// win32_odbc_mysql.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "win32_odbc_mysql.h"
#include <iostream>
#include <OdbcLib.h>

#include <string>
#include <vector>

//#include "UserListManager.h"

using namespace std;

#define MAX_LOADSTRING 100

Odbc* mysql = nullptr;

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.

HWND g_hConnectWnd = NULL;
HWND g_hUserListWnd = NULL;
HWND g_hAboutWnd = NULL;

WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

bool CALLBACK	MenuConnectProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool CALLBACK   UserListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	mysql = Odbc::GetInstance();

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32_ODBC_MYSQL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
		// return 0;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32_ODBC_MYSQL));

    MSG msg;

    // �⺻ �޽��� �����Դϴ�.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


//  �Լ�: MyRegisterClass()
//  ����: ������ Ŭ������ ����մϴ�.
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32_ODBC_MYSQL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32_ODBC_MYSQL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//   �Լ�: InitInstance(HINSTANCE, int)
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//   ����:
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC	hdc;

    switch (message)
    {
	case WM_CREATE:
	{
		g_hConnectWnd = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_CONNECT), hWnd, (DLGPROC)(WNDPROC)MenuConnectProc);
		g_hUserListWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_USER_LIST), hWnd, (DLGPROC)(WNDPROC)UserListProc);
		g_hAboutWnd = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);

		EnableWindow(hWnd, TRUE);
		EnableWindow(g_hConnectWnd, TRUE);
		EnableWindow(g_hUserListWnd, TRUE);
		EnableWindow(g_hAboutWnd, TRUE);
	}
	break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
        case IDM_ABOUT:
			ShowWindow(g_hConnectWnd, SW_SHOW);
            break;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;

		case ID_CONNECT:
			ShowWindow(g_hConnectWnd, SW_SHOW);
			break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
	{
		PostQuitMessage(0);
		DestroyWindow(g_hConnectWnd);
		DestroyWindow(g_hUserListWnd);
	}
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

bool CALLBACK MenuConnectProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hButtonConnect = nullptr;
	static HWND hButtonCancel = nullptr;

	static HWND hOdbcName = nullptr;
	static HWND hDBUserID = nullptr;
	static HWND hDBUserPW = nullptr;

	switch (message)
	{
	case WM_INITDIALOG:
	{
		hButtonConnect = GetDlgItem(hWnd, IDC_CONNECT_BUTTON_CONNECT);
		hButtonCancel = GetDlgItem(hWnd, IDC_CONNECT_BUTTON_CANCEL);
		
		hOdbcName = GetDlgItem(hWnd, IDC_CONNECT_EDIT_ODBC);
		hDBUserID = GetDlgItem(hWnd, IDC_CONNECT_EDIT_DBID);
		hDBUserPW = GetDlgItem(hWnd, IDC_CONNECT_EDIT_DBPW);
	}
	break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_CONNECT_BUTTON_CONNECT:
			{
			wchar_t odbc[1024];
			wchar_t userID[1024];
			wchar_t userPW[1024];

			GetWindowText(hOdbcName, odbc, sizeof(odbc));
			GetWindowText(hDBUserID, userID, sizeof(userID));
			GetWindowText(hDBUserPW, userPW, sizeof(userPW));
			
			mysql->Connect(odbc, userID, userPW);
			std::wstring result = L"false";
			if (mysql->IsConnect() == true)
			{
				result = L"true";
				EndDialog(hWnd, 0);
			}

			std::wstring tmp = L"Connection : " + result;
			
			MessageBox(NULL, tmp.c_str(), NULL, NULL);
			}
		break;
		case IDC_CONNECT_BUTTON_CANCEL:
		{
			EndDialog(hWnd, 0);
		}
		break;
		default:
			return false;
		}
	}
	break;

	case WM_CLOSE:
	{
		EndDialog(hWnd, 0);
		return true;
	}
	break;
	
	default:
		return false;
	
	}
	return true;
}

bool CALLBACK UserListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hUserList = nullptr;
	static HWND hCreateUserID = nullptr;
	static HWND hCreateUserPW = nullptr;

	static HWND hSearchUserID = nullptr;
	static HWND hSearchUserPW = nullptr;

	static HWND hDeleteUserID = nullptr;
	static HWND hDeleteUserPW = nullptr;

	switch (message)
	{
	case WM_INITDIALOG:
	{
		hUserList = GetDlgItem(hWnd, IDC_SEARCH_LIST_USERLIST);

		hCreateUserID = GetDlgItem(hWnd, IDC_CREATE_EDIT_USERID);
		hCreateUserPW = GetDlgItem(hWnd, IDC_CREATE_EDIT_USERPW);

		hSearchUserID = GetDlgItem(hWnd, IDC_SEARCH_EDIT_USERID);
		hSearchUserPW = GetDlgItem(hWnd, IDC_SEARCH_EDIT_USERPW);

		hDeleteUserID = GetDlgItem(hWnd, IDC_DELETE_EDIT_USERID);
		hDeleteUserPW = GetDlgItem(hWnd, IDC_DELETE_EDIT_USERPW);
	}
	break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_CREATE_BUTTON_CREATE:
		{
			wchar_t userID[1024];
			wchar_t userPW[1024];

			GetWindowText(hCreateUserID, userID, sizeof(userID));
			GetWindowText(hCreateUserPW, userPW, sizeof(userPW));


			auto result = mysql->CallCreateUser(wstring(userID), wstring(userPW));
			if (result == true)
			{
				SetWindowText(hCreateUserID, L"");
				SetWindowText(hCreateUserPW, L"");
				MessageBox(NULL, L"Success!", NULL, NULL);
			}
			else
				MessageBox(NULL, L"Fail...", NULL, NULL);
		}
		break;
	
		case IDC_SEARCH_BUTTON_SEARCH_CONDITION:
		{
			wchar_t userID[1024];
			wchar_t userPW[1024];

			GetWindowText(hSearchUserID, userID, sizeof(userID));
			GetWindowText(hSearchUserPW, userPW, sizeof(userPW));

			std::wstring searchID = std::wstring(userID);
			std::wstring searchPW = std::wstring(userPW);

			if (searchID == L"" && searchPW == L"")
			{
				SendDlgItemMessageA(hWnd, IDC_SEARCH_LIST_USERLIST, LB_RESETCONTENT, 0, 0);

				vector<vector<string>> result = mysql->SelectFrom("gameserver","users");
				int index = 0;
				for (auto& tuple : result)
				{
					string value = "";
					for (auto& attr = tuple.begin(); attr < tuple.end(); ++attr)
					{
						value += (*attr);
						value += "  |  ";
					}
					LPARAM result = (LPARAM)(value.c_str());
					SendDlgItemMessageA(hWnd, IDC_SEARCH_LIST_USERLIST, LB_ADDSTRING, 0, result);
				}
			}
			else
			{
				auto result = mysql->CallSearchUser(wstring(userID), wstring(userPW));

				string value = "";
				for (auto& attr = result.rbegin(); attr < result.rend(); ++attr)
				{
					value += (*attr);
					value += " \t ";
				}

				SendDlgItemMessageA(hWnd, IDC_SEARCH_LIST_USERLIST, LB_RESETCONTENT, 0, 0);
				SendDlgItemMessageA(hWnd, IDC_SEARCH_LIST_USERLIST, LB_ADDSTRING, 0, (LPARAM)value.c_str());
			}

			SetWindowText(hSearchUserID, L"");
			SetWindowText(hSearchUserPW, L"");
		}
		break;

		case IDC_DELETE_BUTTON_DELETE_CONDITION:
		{
			wchar_t userID[1024];
			wchar_t userPW[1024];

			GetWindowText(hDeleteUserID, userID, sizeof(userID));
			GetWindowText(hDeleteUserPW, userPW, sizeof(userPW));

			std::wstring deleteID = std::wstring(userID);
			std::wstring deletePW = std::wstring(userPW);

			if (deleteID == L"" && deletePW == L"")
				break;

			auto result = mysql->CallDeleteUser(deleteID, deletePW);
			if (result == true)
			{
				MessageBox(NULL, L"Delete!", NULL, NULL);
				SetWindowText(hDeleteUserID, L"");
				SetWindowText(hDeleteUserPW, L"");
			}
			else
				MessageBox(NULL, L"Fail...", NULL, NULL);

		}
		break;

		default:
			return false;
		}
	}
	break;

	case WM_CLOSE:
	{
		EndDialog(hWnd, 0);		
		return true;
	}
	break;
	
	default:
		return false;
	}
	return true;
}