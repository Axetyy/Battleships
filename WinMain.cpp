#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include "GameLogic.h"

#define ID_PLAY_BUTTON  1
#define ID_EXIT_BUTTON  2
#define ID_START_BUTTON 3
#define ID_AI_EDIT 4
#define ID_FAST_BUTTON 5
#define IDI_MAINICON 3002
// Global game object instance.
GameLogic game;

// Forward declaration of the Window Procedure.
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance,_In_ LPWSTR lpCmdLine,_In_ int nShowCmd){
	// Initialize your game logic.
	game.initialize();

	// Define and register the window class.
	const TCHAR CLASS_NAME[] = _T("BattleshipWindowClass");
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINICON));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, _T("Window Registration Failed!"), _T("Error"), MB_ICONERROR);
		return 0;
	}

	// Create the main window with WS_CLIPCHILDREN to protect child controls.
	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		_T("Battleships Game"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,       // Parent window.
		NULL,       // Menu.
		hInstance,  // Instance handle.
		NULL        // Additional application data.
	);

	if (hwnd == NULL)
	{
		MessageBox(NULL, _T("Window Creation Failed!"), _T("Error"), MB_ICONERROR);
		return 0;
	}
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ERASEBKGND:
		return 0;
	case WM_CREATE:
	{
		CreateWindowEx(
			0,
			_T("BUTTON"),
			_T("Play"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			50, 50, 300, 100,      // x, y, width, height
			hwnd,
			(HMENU)ID_PLAY_BUTTON,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		CreateWindowEx(
			0,
			_T("BUTTON"),
			_T("Exit"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			50, 90, 300, 100,      // x, y, width, height
			hwnd,
			(HMENU)ID_EXIT_BUTTON,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		CreateWindowEx(
			0,
			_T("BUTTON"),
			_T("Start Game"),
			WS_CHILD | BS_PUSHBUTTON,
			50, 50, 300, 100,      // x, y, width, height
			hwnd,
			(HMENU)ID_START_BUTTON,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);

		CreateWindowEx(
			0,
			_T("STATIC"),
			_T("Opponent Level ( 1 - 20 ):"),
			WS_CHILD,
			1000, 50, 200, 50,
			hwnd,
			(HMENU)1001,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);

		CreateWindowEx(
			WS_EX_CLIENTEDGE,
			_T("EDIT"),
			_T(""),
			WS_CHILD | WS_BORDER | ES_NUMBER,
			1000, 70, 180, 30,
			hwnd,
			(HMENU)ID_AI_EDIT,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		CreateWindowEx(
			0,
			_T("BUTTON"),
			_T("Fast Mode"),
			WS_CHILD | BS_AUTOCHECKBOX,
			1000, 140, 100, 30,
			hwnd,
			(HMENU)ID_FAST_BUTTON,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		CreateWindowEx(
			0,
			_T("STATIC"),
			_T("Carrier"),
			WS_CHILD,
			270, 210, 100, 20,
			hwnd,
			(HMENU)1002,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		CreateWindowEx(
			0,
			_T("STATIC"),
			_T("Battleship"),
			WS_CHILD,
			240, 280, 100, 20,
			hwnd,
			(HMENU)1003,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		CreateWindowEx(
			0,
			_T("STATIC"),
			_T("Cruiser"),
			WS_CHILD,
			200, 350, 100, 20,
			hwnd,
			(HMENU)1004,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		CreateWindowEx(
			0,
			_T("STATIC"),
			_T("Submarine"),
			WS_CHILD,
			200, 420, 100, 20,
			hwnd,
			(HMENU)1005,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		CreateWindowEx(
			0,
			_T("STATIC"),
			_T("Destroyer"),
			WS_CHILD,
			160, 490, 100, 20,
			hwnd,
			(HMENU)1006,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case ID_PLAY_BUTTON:
		{
			game.activeState = 1;
			game.gameStarted = false;
			HWND hPlayButton = GetDlgItem(hwnd, ID_PLAY_BUTTON);
			HWND hAIText = GetDlgItem(hwnd, ID_AI_EDIT);
			HWND hCheckBox = GetDlgItem(hwnd, ID_FAST_BUTTON);
			for (int id = 1002;id <= 1006;id++) ShowWindow(GetDlgItem(hwnd, id), SW_SHOW);
			ShowWindow(GetDlgItem(hwnd, ID_AI_EDIT), SW_SHOW);
			ShowWindow(GetDlgItem(hwnd, ID_FAST_BUTTON), SW_SHOW);
			ShowWindow(GetDlgItem(hwnd, 1001), SW_SHOW);
			ShowWindow(hPlayButton, SW_HIDE);
			ShowWindow(GetDlgItem(hwnd, ID_START_BUTTON), SW_SHOW);
			HWND hExitButton = GetDlgItem(hwnd, ID_EXIT_BUTTON);
			RECT rc;
			GetClientRect(hwnd, &rc);

			SetWindowPos(hExitButton, NULL, rc.right - 110, 20, 100, 30, SWP_NOZORDER);
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		}

		case ID_START_BUTTON:
		{
			if (game.allShipsPlaced())
			{
				game.startGame();

				ShowWindow(GetDlgItem(hwnd, ID_START_BUTTON), SW_HIDE);
				ShowWindow(GetDlgItem(hwnd, ID_AI_EDIT), SW_HIDE);
				ShowWindow(GetDlgItem(hwnd, ID_FAST_BUTTON), SW_HIDE);
				ShowWindow(GetDlgItem(hwnd, 1001), SW_HIDE);
				for (int id = 1002;id <= 1006;id++) ShowWindow(GetDlgItem(hwnd, id), SW_HIDE);
				InvalidateRect(hwnd, NULL, TRUE);
			}
			break;
		}

		case ID_EXIT_BUTTON:
			PostQuitMessage(0);
			break;

		case ID_AI_EDIT:
		{
			if (game.allShipsPlaced())
			{
				TCHAR aiLevelText[10];
				GetWindowText(GetDlgItem(hwnd, ID_AI_EDIT), aiLevelText, 10);
				int aiLevel = _ttoi(aiLevelText);
				game.AILevel = aiLevel;
			}
			break;
		}
		case ID_FAST_BUTTON:
		{
			HWND hFastButton = GetDlgItem(hwnd, ID_FAST_BUTTON);
			LRESULT state = SendMessageW(hFastButton, BM_GETCHECK, 0, 0);
			game.fastMode = (state == BST_CHECKED);
		}
		break;
		}
		break;
	}
	case WM_SIZE:
	{
		HWND hPlayButton = GetDlgItem(hwnd, ID_PLAY_BUTTON);
		HWND hExitButton = GetDlgItem(hwnd, ID_EXIT_BUTTON);

		UINT width = LOWORD(lParam);
		UINT height = HIWORD(lParam);
		RECT rc = {};
		GetClientRect(hPlayButton, &rc);
		int x = (int)(width - (rc.right - rc.left)) / 2;
		int y = (int)(height - (rc.bottom - rc.top)) / 2;
		MoveWindow(hPlayButton, x, y - 100, rc.right - rc.left, rc.bottom - rc.top, 1);
		MoveWindow(hExitButton, x, y, rc.right - rc.left, rc.bottom - rc.top, 1);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// Create a memory device context for double buffering.
		HDC hdcMem = CreateCompatibleDC(hdc);
		RECT rc;
		GetClientRect(hwnd, &rc);
		HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

		// Fill the background with the window color.
		HBRUSH hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		FillRect(hdcMem, &rc, hbrBkGnd);
		DeleteObject(hbrBkGnd);

		// Render the game to the memory device context.
		if (game.activeState == 1)
		{
			game.render(hdcMem);
		}

		// Copy the memory device context to the screen.
		BitBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);

		// Clean up.
		SelectObject(hdcMem, hbmOld);
		DeleteObject(hbmMem);
		DeleteDC(hdcMem);

		EndPaint(hwnd, &ps);
		break;
	}

	case WM_LBUTTONDOWN:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		InvalidateRect(hwnd, NULL, TRUE);
		game.handleMouseDown(x, y, false);
		break;
	}

	case WM_MOUSEMOVE:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		game.handleMouseMove(x, y);
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	}

	case WM_LBUTTONUP:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		game.handleMouseUp(x, y);
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	}

	case WM_RBUTTONDOWN:
	{
		if (game.activeState == 1)
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			game.handleMouseDown(x, y, true);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		if (game.gameStarted)
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			game.handleAttack(x, y);
		}
		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}