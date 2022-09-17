// MagicWheelAndKeys.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "MagicWheelAndKeys.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
static BOOL ABOUT_DLG_SHOWN = FALSE;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MAGICWHEELANDKEYS, szWindowClass, MAX_LOADSTRING);

	//  Detect previous instance, and bail if there is one.
	if (NULL != FindWindow(szWindowClass, szTitle))
		return 0;

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance)) {
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAGICWHEELANDKEYS));

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


ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAGICWHEELANDKEYS));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MAGICWHEELANDKEYS);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance) {
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(
		szWindowClass,
		szTitle,
		//WS_OVERLAPPEDWINDOW,
		//WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		//WS_CAPTION,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT,
		0,
		480,
		480,
		nullptr,
		nullptr,
		hInstance,
		nullptr);
	if (!hWnd)
	{
		return FALSE;
	}

	//ShowWindow(hWnd, nCmdShow);
	//UpdateWindow(hWnd);

	ListenHIDMessages(hWnd);

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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE: {
		AddTrayIcon(hWnd);
		break;
	}

	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId) {
		case IDM_ABOUT:
			if (!ABOUT_DLG_SHOWN) {
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			}
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			RemoveTrayIcon(hWnd);
			PostQuitMessage(0);
			break;
		case IDM_SHOW:
			ShowWindow(hWnd, SW_SHOWNORMAL);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		BeginPaint(hWnd, &ps);
		PaintIt(hdc);
		EndPaint(hWnd, &ps);
		break;
	}

	case WM_CLOSE: {
		ShowWindow(hWnd, SW_HIDE);
		break;
	}

	case WM_APP: {
		switch (lParam) {
		case WM_RBUTTONUP:
			SetForegroundWindow(hWnd);
			ShowTrayIconPopupMenu(hWnd);
			PostMessage(hWnd, WM_APP + 1, 0, 0);
			break;
		}
		break;
	}

	case WM_INPUT: {
		HandleInput(hWnd, message, wParam, lParam);
		break;
	}

	default: {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		ABOUT_DLG_SHOWN = TRUE;
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			ABOUT_DLG_SHOWN = FALSE;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void AddTrayIcon(HWND hWnd) {
	NOTIFYICONDATA  nid{};
	nid.hWnd = hWnd;
	nid.uID = TRAY_ICON;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_APP;
	nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));
	lstrcpyW(nid.szTip, szTitle);
	Shell_NotifyIcon(NIM_ADD, &nid);
}

void RemoveTrayIcon(HWND hWnd) {
	NOTIFYICONDATA  nid{};
	nid.hWnd = hWnd;
	nid.uID = TRAY_ICON;
	Shell_NotifyIcon(NIM_DELETE, &nid);
}

void ShowTrayIconPopupMenu(HWND hWnd) {
	//if (TRAY_POPUP_MENU_SHOWN)
	//    return;

	HMENU hPop = LoadMenu(hInst, MAKEINTRESOURCE(IDR_TRAY_POPUP_MENU));
	HMENU hmenuTrackPopup = GetSubMenu(hPop, 0);

	SetMenuDefaultItem(hmenuTrackPopup, IDM_SHOW, FALSE);
	SendMessage(hWnd, WM_INITMENUPOPUP, (WPARAM)hmenuTrackPopup, 0);

	POINT pt;
	GetCursorPos(&pt);

	BOOL cmd = TrackPopupMenuEx(hmenuTrackPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, hWnd, NULL);
	SendMessage(hWnd, WM_COMMAND, cmd, 0);

	DestroyMenu(hPop);
}

void ListenHIDMessages(HWND hWnd) {
	RAWINPUTDEVICE rid[1]{};
	rid[0].usUsagePage = 0x0C; // The keyboard is using this UsagePage
	rid[0].usUsage = 0X01;     // with this Usage
	rid[0].dwFlags = RIDEV_INPUTSINK;
	rid[0].hwndTarget = hWnd;

	BOOL ret = RegisterRawInputDevices(rid, 1, sizeof(rid[0]));
	if (!ret) std::cout << "Unable to register raw device input: " << GetLastError() << std::endl;
}

void HandleInput(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	// Gets the size
	UINT dwSize = 0;
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	LPBYTE lpb = new BYTE[dwSize];
	if (!lpb)
		return;
	
	// Gets the content
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
	
	// Handler
	{
		RAWINPUT* raw = (RAWINPUT*)lpb;
		if (raw->header.dwType == RIM_TYPEHID) {
			HANDLE device = raw->header.hDevice;

			// Gets source device
			UINT cbSize = sizeof(RID_DEVICE_INFO);
			RID_DEVICE_INFO deviceInfo{};
			deviceInfo.cbSize = cbSize;
			GetRawInputDeviceInfo(device, RIDI_DEVICEINFO, &deviceInfo, &cbSize);

			if (VENDORID == deviceInfo.hid.dwVendorId && PRODUCTID == deviceInfo.hid.dwProductId) {
				DWORD64 data = 0;
				memcpy(&data, raw->data.hid.bRawData, sizeof(DWORD64));
				HandleScancode(data);
			}

			//std::wcout << "Vendor 0x" << std::hex << deviceInfo.hid.dwVendorId << std::endl; // Expected: 0x0000045E
			//std::wcout << "Product 0x" << std::hex << deviceInfo.hid.dwProductId << std::endl; // Expected: 0x00000048

			//for (DWORD itemIdx = 0; itemIdx < raw->data.hid.dwCount; itemIdx++) {
			//	for (DWORD data = 0; data < raw->data.hid.dwSizeHid; data++) {
			//		DWORD idx = (itemIdx * raw->data.hid.dwSizeHid) + data;
			//		std::wcout << "0x" << std::hex << raw->data.hid.bRawData[idx] << " ";
			//	}
			//}
			//std::wcout << std::endl;
		}
	}

	delete[] lpb;

	// https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-input
	// RIM_INPUT: Input occurred while the application was in the foreground
	//		The application must call DefWindowProc so the system can perform cleanup.
	if (GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT) {
		DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void HandleScancode(DWORD64 scancode) {
	UINT nbInputs = NULL;
	INPUT* inputs = NULL;

	switch (scancode) {
	case KEY_PREVIOUS:
	case KEY_NEXT: {
		break; // Keep current behavior
	}
	case KEY_SCROLLUP_SLOWER: {
		WheelInput(1, &inputs, nbInputs); // 1 wheel clicks up
		break;
	}
	case KEY_SCROLLUP_SLOW: {
		WheelInput(3, &inputs, nbInputs); // 3 wheel clicks up
		break;
	}
	case KEY_SCROLLUP_FAST: {
		WheelInput(5, &inputs, nbInputs); // 5 wheel clicks up
		break;
	}
	case KEY_SCROLLUP_FASTER: {
		WheelInput(7, &inputs, nbInputs); // 7 wheel clicks up
		break;
	}
	case KEY_SCROLLDOWN_SLOWER: {
		WheelInput(-1, &inputs, nbInputs); // 1 wheel clicks down
		break;
	}
	case KEY_SCROLLDOWN_SLOW: {
		WheelInput(-3, &inputs, nbInputs); // 3 wheel clicks down
		break;
	}
	case KEY_SCROLLDOWN_FAST: {
		WheelInput(-5, &inputs, nbInputs); // 5 wheel clicks down
		break;
	}
	case KEY_SCROLLDOWN_FASTER: {
		WheelInput(-7, &inputs, nbInputs); // 7 wheel clicks down
		break;
	}
	case KEY_CUT: {
		const UINT nbkeys = 2;
		WORD* keys = new WORD[nbkeys]{ VK_CONTROL, 'X' }; // Ctrl + X : Cut
		KeyboardInput(nbkeys, keys, &inputs, nbInputs);
		delete[] keys;
		break;
	}
	case KEY_COPY: {
		const UINT nbkeys = 2;
		WORD* keys = new WORD[nbkeys]{ VK_CONTROL, 'C' }; // Ctrl + C : Copy
		KeyboardInput(nbkeys, keys, &inputs, nbInputs);
		delete[] keys;
		break;
	}
	case KEY_PASTE: {
		const UINT nbkeys = 2;
		WORD* keys = new WORD[nbkeys]{ VK_CONTROL, 'V' }; // Ctrl + V : Paste
		KeyboardInput(nbkeys, keys, &inputs, nbInputs);
		delete[] keys;
		break;
	}
	case KEY_APP_BACK: {
		const UINT nbkeys = 2;
		WORD* keys = new WORD[nbkeys]{ VK_LWIN, 'D' }; // Win + D : show desktop
		KeyboardInput(nbkeys, keys, &inputs, nbInputs);
		delete[] keys;
		break;
	}
	case KEY_APP_FWD: {
		const UINT nbkeys = 2;
		WORD* keys = new WORD[nbkeys]{ VK_MENU, VK_TAB }; // Alt + Tab
		KeyboardInput(nbkeys, keys, &inputs, nbInputs);
		delete[] keys;
		break;
	}
	case KEY_UNDO: {
		const UINT nbkeys = 2;
		WORD* keys = new WORD[nbkeys]{ VK_CONTROL, 'Z' }; // Ctrl + Z : UNDO
		KeyboardInput(nbkeys, keys, &inputs, nbInputs);
		delete[] keys;
		break;
	}
	case KEY_REDO: {
		const UINT nbkeys = 2;
		WORD* keys = new WORD[nbkeys]{ VK_CONTROL, 'Y' }; // Ctrl + Y : REDO
		KeyboardInput(nbkeys, keys, &inputs, nbInputs);
		delete[] keys;
		break;
	}
	case KEY_WORD:
	case KEY_EXCEL:
	case KEY_WEB:
	case KEY_EMAIL:
	case KEY_CALENDAR:
	case KEY_FILE:
	case KEY_CALC:
	case KEY_LOGOUT:
	case KEY_VOL_PLUS:
	case KEY_VOL_MINUS:
	case KEY_VOL_MUTE:
	case KEY_HELP:
	case KEY_OFFICE_HOME:
	case KEY_TASK_PANEL:
	case KEY_NEW:
	case KEY_OPEN:
	case KEY_CLOSE:
	case KEY_REPLY:
	case KEY_FWD:
	case KEY_SEND:
	case KEY_SPELL:
	case KEY_SAVE:
	case KEY_PRINT:
	case KEY_EQUAL:
	case KEY_LEFT_PARENTHESIS:
	case KEY_RIGHT_PARENTHESIS:
	//case KEY_SLEEP:
		break;
	default:
		break;

	}
	SendInput(nbInputs, inputs, sizeof(INPUT));
	delete[] inputs;
}

void WheelInput(INT nbWheelClick, INPUT** lpinput, UINT& nbinputs) {
	UINT _nbinputs = 1;
	INPUT* _input = new INPUT[1];

	_input[0].type = INPUT_MOUSE;
	_input[0].mi.dx = 0;
	_input[0].mi.dy = 0;
	_input[0].mi.mouseData = nbWheelClick * WHEEL_DELTA;
	_input[0].mi.dwFlags = MOUSEEVENTF_WHEEL;
	_input[0].mi.time = 0;
	_input[0].mi.dwExtraInfo = NULL;

	*lpinput = _input;
	nbinputs = _nbinputs;
}

void KeyboardInput(UINT nbkeys, WORD* keys, INPUT** lpinputs, UINT& nbinputs) {
	UINT _nbinputs = nbkeys * 2;
	INPUT* _inputs = new INPUT[nbkeys * 2];

	for (UINT idx = 0; idx < nbkeys; idx++) {
		_inputs[idx].type = INPUT_KEYBOARD;
		_inputs[idx].ki.wVk = keys[idx];
		_inputs[idx].ki.wScan = 0;
		_inputs[idx].ki.dwFlags = 0;
		_inputs[idx].ki.time = 0;
		_inputs[idx].ki.dwExtraInfo = 0;

		_inputs[idx + nbkeys].type = INPUT_KEYBOARD;
		_inputs[idx + nbkeys].ki.wVk = keys[idx];
		_inputs[idx + nbkeys].ki.wScan = 0;
		_inputs[idx + nbkeys].ki.dwFlags = KEYEVENTF_KEYUP;
		_inputs[idx + nbkeys].ki.time = 0;
		_inputs[idx + nbkeys].ki.dwExtraInfo = 0;
	}
	
	*lpinputs = _inputs;
	nbinputs = _nbinputs;
}

void PaintIt(HDC hdc) {
	RECT rect{};
	rect.left = 0;
	rect.top = 0;
	rect.bottom = 300;
	rect.right = 300;
	LPCWSTR  text = _T("\
- Closing this window is not ending the application, File / Exit to do so\r\n\
- The wheel works with 4 speeds up and down\r\n\
- Cut / Copy / Paste : work as intended (Ctrl + X / C / V)\r\n\
- Application left : Show desktop\r\n\
- Application right : Previsou window (Alt + Tab)\r\n\
- Undo / Redo : work as intended (Ctrl + Z / Y)\r\n\
- Others buttons : not yet allocated\r\n");
//- 8\r\n\
//- 9\r\n\
//- 10\r\n\
//- 11\r\n\
//- 12\r\n\
//- 13\r\n\
//- 14\r\n\
//- 15\r\n\
//- 16\r\n\
//- 17\r\n\
//- 18\r\n\
//- 19\r\n\
//- 20\r\n\
//- 21\r\n\
//- 22\r\n\
//- 23\r\n\
//- 24\r\n\
//- 25\r\n\
//- 26\r\n\
//---------

	DrawText(hdc, text, (int)_tcslen(text),&rect, DT_NOCLIP);
}