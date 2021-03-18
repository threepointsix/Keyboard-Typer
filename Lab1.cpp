// Lab1.cpp : Defines the entry point for the application.
// https://forums.codeguru.com/showthread.php?500841-Set-my-window-position-at-screen-center
// https://docs.microsoft.com/ru-ru/windows/win32/api/winuser/nf-winuser-getsystemmetrics?redirectedfrom=MSDN
// https://docs.microsoft.com/en-us/windows/win32/winmsg/using-windows

#include "framework.h"
#include "Lab1.h"
using namespace std;

#define MAX_LOADSTRING 100

typedef struct lowestChildWindow {
	HWND childWin;
	int yPos;
} lowestChildWindow;

lowestChildWindow lcw;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
TCHAR szChildClass[] = TEXT("myChildClass");
TCHAR s[26];

// Forward declarations of functions included in this code module:
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	ChildWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassEx(&wcex);
	wcex.lpfnWndProc = ChildWndProc;
	wcex.hIcon = NULL;
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszClassName = szChildClass;
	RegisterClassEx(&wcex);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	int width = GetSystemMetrics(SM_CXSCREEN), height = GetSystemMetrics(SM_CYSCREEN);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		width / 4, height / 4, width / 2, height / 2, nullptr, nullptr, hInstance, nullptr);
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hChild;
	switch (message)
	{
	case WM_CREATE:
	{
		SetTimer(hWnd, 5, 1000, NULL);

		for (int i = 0, j = 97; i < 26; i++, j++) {
			s[i] = j;
		}
		break;
	}
	case WM_TIMER: {
		if (wParam == 5) {
			unsigned int randLetter;
			rand_s(&randLetter);
			randLetter = (unsigned int)((double)randLetter / ((double)UINT_MAX + 1) * 26);
			RECT rc;
			GetWindowRect(hWnd, &rc);
			int width = rc.right - rc.left - 25;
			unsigned int xPos;
			rand_s(&xPos);
			xPos = (unsigned int)((double)xPos / ((double)UINT_MAX + 1) * width) + 1;
			CreateWindow(szChildClass, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, xPos, 0, 25, 25, hWnd, NULL, hInst, NULL);
		}
		unsigned int delay;
		rand_s(&delay);
		delay = (unsigned int)((double)delay / ((double)UINT_MAX + 1) * 1000.0) + 250;
		KillTimer(hWnd, 5);
		SetTimer(hWnd, 5, delay, NULL);
		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_SPACE:
		{
			DestroyWindow(lcw.childWin);
			lcw.yPos = 0;
			break;
		}
		}
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_NEWGAME:
			while (lcw.childWin) {
				DestroyWindow(lcw.childWin);
			}
			break;
		case IDM_PAUSE:
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
	{
		//lcw[randLetter].childWin = hWnd;
		unsigned int ySpeed;
		rand_s(&ySpeed);
		ySpeed = (unsigned int)((double)ySpeed / ((double)UINT_MAX + 1) * 35.0) + 5;
		SetTimer(hWnd, 1, ySpeed, NULL);
	}
	case WM_TIMER:
	{
		if (wParam == 1) // check timer id
		{
			RECT rc, pRC;
			GetClientRect(GetParent(hWnd), &pRC);
			GetClientRect(hWnd, &rc);
			MapWindowPoints(hWnd, GetParent(hWnd), (LPPOINT)&rc, 2);
			int yPos = rc.top + 1;
			if (yPos >= pRC.bottom) {
				DestroyWindow(hWnd);
			}
			if (yPos > lcw.yPos) {
				lcw.childWin = hWnd;
				lcw.yPos = yPos;
			}
			MoveWindow(hWnd, rc.left, yPos, 25, 25, true);
		}
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		SetBkColor(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(211, 211, 211));
		TextOut(hdc, 9, 4, &s[0], 1);
		EndPaint(hWnd, &ps);
	}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Message handler for about box.
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
