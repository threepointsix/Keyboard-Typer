// Lab1.cpp : Defines the entry point for the application.
// https://forums.codeguru.com/showthread.php?500841-Set-my-window-position-at-screen-center
// https://docs.microsoft.com/ru-ru/windows/win32/api/winuser/nf-winuser-getsystemmetrics?redirectedfrom=MSDN
// https://docs.microsoft.com/en-us/windows/win32/winmsg/using-windows
// https://stackoverflow.com/questions/14989062/set-a-window-to-be-topmost
// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-insertmenua
// https://docs.microsoft.com/en-us/windows/win32/dlgbox/using-common-dialog-boxes
// https://stackoverflow.com/questions/18034975/how-do-i-find-position-of-a-win32-control-window-relative-to-its-parent-window

#include "framework.h"
#include "Lab1.h"
using namespace std;

#define MAX_LOADSTRING 100

typedef struct cWin {
	HWND childWin;
	unsigned int letter;
	unsigned int ySpeed;
} cWin;

typedef struct lowestWin {
	HWND childWin;
	int yPos;
} lowestWin;

typedef struct wCaption {
	unsigned int missed;
	unsigned int wrongKeys;
} wCaption;

vector<cWin> cWindows;
vector<lowestWin> lowestCWindows(26);
wCaption wc;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
TCHAR szChildClass[] = TEXT("myChildClass");    // Children class name
TCHAR s[26];									// Letters [a, z]
WCHAR title[256];                               // Title with captions
bool pauseTimer = false;

// Forward declarations of functions included in this code module:
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	ChildWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
unsigned int		rand(double lowerBound, double upperBound);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

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
	wcex.hbrBackground = (HBRUSH)(COLOR_INACTIVECAPTION);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassEx(&wcex);
	wcex.lpfnWndProc = ChildWndProc;
	wcex.hIcon = NULL;
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszClassName = szChildClass;
	RegisterClassEx(&wcex);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

	MSG msg;

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

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance; // Store instance handle in our global variable
	int width = GetSystemMetrics(SM_CXSCREEN), height = GetSystemMetrics(SM_CYSCREEN);
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, width / 4, height / 4, width / 2, height / 2, nullptr, nullptr, hInstance, nullptr);
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	if (!hWnd) return FALSE;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE; }

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HWND hChild;
	HDC hDC, hCompatibleDC;
	PAINTSTRUCT PaintStruct;
	HANDLE hBitmap, hOldBitmap;
	RECT Rect;
	BITMAP Bitmap;
	switch (message)
	{
	case WM_CREATE:
	{
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		SetTimer(hWnd, 5, 1000, NULL);
		wc.missed = 0;
		wc.wrongKeys = 0;
		_stprintf_s(title, 256, L"Keyboard Master: WinAPI_2021, Missed: %d, Wrong keys: %d", wc.missed, wc.wrongKeys);
		SetWindowText(hWnd, title);
		for (int i = 0, j = 97; i < 26; i++, j++) {
			s[i] = j;
		}
		break;
	}
	case WM_TIMER: {
		if (wParam == 5) {
			RECT rc;
			GetWindowRect(hWnd, &rc);
			int width = rc.right - rc.left - 50;
			unsigned int randLetter = rand(0, 26);
			unsigned int ySpeed = rand(5, 25);
			unsigned int xPos = rand(1, width);
			cWin cw;
			hChild = CreateWindow(szChildClass, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, xPos, 0, 25, 25, hWnd, NULL, hInst, NULL);
			cw.childWin = hChild;
			cw.letter = randLetter;
			cw.ySpeed = ySpeed;
			cWindows.push_back(cw); }
		unsigned int delay = rand(250, 750);
		KillTimer(hWnd, 5);
		SetTimer(hWnd, 5, delay, NULL);
		break; }
	case WM_CHAR: {
		if ((int)wParam >= 97 && (int)wParam <= 122) {
			if (lowestCWindows[(int)wParam - 97].childWin) {
				DestroyWindow(lowestCWindows[(int)wParam - 97].childWin);
				lowestCWindows[(int)wParam - 97].childWin = NULL;
				lowestCWindows[(int)wParam - 97].yPos = 0; }
			else {
				wc.wrongKeys++;
				_stprintf_s(title, 256, L"Keyboard Master: WinAPI_2021, Missed: %d, Wrong keys: %d", wc.missed, wc.wrongKeys);
				SetWindowText(hWnd, title); } }
		break; }
	case WM_RBUTTONDOWN: {
		wc.wrongKeys--;
		HMENU hPopupMenu = CreatePopupMenu();
		POINT cursor;
		GetCursorPos(&cursor);
		InsertMenu(hPopupMenu, 0, MF_STRING, IDM_COLOR, L"Color...\tCtrl+C");
		InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_SEPARATOR, NULL, NULL);
		InsertMenu(hPopupMenu, 0, MF_STRING, IDM_BITMAP, L"Bitmap...\tCtrl+B");
		InsertMenu(hPopupMenu, 0, MF_STRING | MF_DISABLED, IDM_TILE, L"Tile\tCtrl+T");
		InsertMenu(hPopupMenu, 0, MF_STRING | MF_DISABLED, IDM_STRETCH, L"Stretch\tCtrl+S");
		SetForegroundWindow(hWnd);
		TrackPopupMenu(hPopupMenu, TPM_LEFTBUTTON, cursor.x, cursor.y, 0, hWnd, NULL);
		break; }
	case WM_KILLFOCUS:
		KillTimer(hWnd, 5);
		for (auto it : cWindows) {
			SendMessage(it.childWin, ID_FREEZE, NULL, NULL); }
		break;
	case WM_SETFOCUS:
		SetTimer(hWnd, 5, 1000, NULL);
		for (auto it : cWindows) {
			SendMessage(it.childWin, ID_UNFREEZE, NULL, NULL); }
		break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		switch (wmId) {
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_NEWGAME:
			for (auto it : cWindows) {
				DestroyWindow(it.childWin); }
			cWindows.clear();
			wc.missed = 0;
			wc.wrongKeys = 0;
			_stprintf_s(title, 256, L"Keyboard Master: WinAPI_2021, Missed: %d, Wrong keys: %d", wc.missed, wc.wrongKeys);
			SetWindowText(hWnd, title);
			break;
		case IDM_BITMAP:
			OPENFILENAME ofn;       // common dialog box structure
			char szFile[260];       // buffer for file name
			HANDLE hf;              // file handle
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = (LPWSTR)szFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = L"Bitmap files (*.bmp)\0*.bmp";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			if (GetOpenFileName(&ofn) == TRUE) {
				hf = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
				HBITMAP hBMP = (HBITMAP)LoadImage(NULL, ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				break; }
			break;
		case IDM_PAUSE:
			if (pauseTimer == false) {
				KillTimer(hWnd, 5);
				for (auto it : cWindows) SendMessage(it.childWin, ID_FREEZE, NULL, NULL);
				pauseTimer = true;
				break;
			}
			else {
				SetTimer(hWnd, 5, 1000, NULL);
				for (auto it : cWindows) SendMessage(it.childWin, ID_UNFREEZE, NULL, NULL);
				pauseTimer = false;
				break;
			}
		case IDM_COLOR:
		{
			CHOOSECOLOR cc;                 // common dialog box structure 
			static COLORREF acrCustClr[16]; // array of custom colors 
			HBRUSH hbrush;                  // brush handle
			static DWORD rgbCurrent;        // initial color selection
			ZeroMemory(&cc, sizeof(cc));
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hWnd;
			cc.lpCustColors = (LPDWORD)acrCustClr;
			cc.rgbResult = rgbCurrent;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;
			if (ChooseColor(&cc) == TRUE) {
				hbrush = CreateSolidBrush(cc.rgbResult);
				SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR(hbrush)));
				InvalidateRect(hWnd, NULL, TRUE); }
			break;
		}
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
	int letter = 0;
	unsigned int ySpeed = rand(5, 25);
	for (auto it : cWindows) {
		if (it.childWin == hWnd) {
			letter = it.letter;
			ySpeed = it.ySpeed;
		}
	}
	switch (message) {
	case WM_CREATE:
	{
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
			if (lowestCWindows[letter].childWin) {
				if (yPos > lowestCWindows[letter].yPos) {
					lowestCWindows[letter].childWin = hWnd;
					lowestCWindows[letter].yPos = yPos;
				}
			}
			else {
				lowestCWindows[letter].childWin = hWnd;
				lowestCWindows[letter].yPos = yPos;
			}
			if (yPos >= pRC.bottom) {
				wc.missed++;
				_stprintf_s(title, 256, L"Keyboard Master: WinAPI_2021, Missed: %d, Wrong keys: %d", wc.missed, wc.wrongKeys);
				SetWindowText(GetParent(hWnd), title);
				lowestCWindows[letter].childWin = NULL;
				lowestCWindows[letter].yPos = yPos;
				DestroyWindow(hWnd);
			}

			MoveWindow(hWnd, rc.left, yPos, 25, 25, true);
		}
		break;
	}
	case ID_UNFREEZE:
	{
		SetTimer(hWnd, 1, ySpeed, NULL);
		break;
	}
	case ID_FREEZE:
	{
		KillTimer(hWnd, 1);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		SetBkColor(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(211, 211, 211));
		TextOut(hdc, 9, 4, &s[letter], 1);
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

unsigned int rand(double lowerBound, double upperBound) {
	unsigned int r;
	rand_s(&r);
	r = (unsigned int)((double)r / ((double)UINT_MAX + 1) * upperBound) + lowerBound;
	return r;
}