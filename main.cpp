#include <windows.h> // for win32 gui, mouse hook and timer
#include <chrono> // for time
#include <string> // for std::to_wstring and .c_str()
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

#define appName L"CPS LimiterV9"
#define keyPath "Software\\CPS Limiter"
#define defaultOutputMessage L"CPS LimiterV9 By Deus"
#define WM_TRAYICON (WM_USER + 1)

HWND hwnd;
HWND hwndTooltip = NULL;
HMENU hSettings;
NOTIFYICONDATA nid = { 0 };
HMENU hTrayMenu;
HHOOK mouseHook;

HFONT hFont1; // general font
HFONT hFont2; // RUN & STOP buttons font

bool storeData = 0;

namespace left {
	UINT16 cooldown = 50; // if time between last click and previous click smaller than cooldown last click will be blocked
	UINT16 limit = 15; // maximum cps limit
	std::chrono::steady_clock::time_point arr[65535]; // the time when you click will be stored in this array
	std::chrono::steady_clock::time_point now; // its declared so it will not declared while algorithm working
	UINT16 start = 0; // index of first value larger than now - 1 ms in arr
	UINT16 end = 0; // index of last value in arr
	bool isPreviousBlocked = 0; // for block click up message if previous click down blocked
}

namespace right {
	UINT16 cooldown = 0; // if time between last click and previous click smaller than cooldown last click will be blocked
	UINT16 limit = 18; // maximum cps limit
	std::chrono::steady_clock::time_point arr[65535]; // the time when you click will be stored in this array
	std::chrono::steady_clock::time_point now; // its declared so it will not declared while algorithm working
	UINT16 start = 0; // index of first value larger than now - 1 ms in arr
	UINT16 end = 0; // index of last value in arr
	bool isPreviousBlocked = 0; // for block click up message if previous click down blocked
}

struct PixelRect {
	int x, y, w, h;
};
PixelRect leftMousePixels[] = {
	{264, 68, 12, 2}, {261, 67, 3, 2}, {276, 67, 3, 2}, {259, 66, 2, 2},
	{279, 66, 2, 2}, {258, 65, 2, 2}, {280, 65, 2, 2}, {257, 62, 2, 3},
	{281, 62, 2, 3}, {256, 58, 2, 4}, {282, 58, 2, 4}, {255, 35, 2, 23},
	{283, 35, 2, 23}, {256, 30, 2, 5}, {282, 30, 2, 5}, {257, 26, 2, 4},
	{281, 26, 2, 4}, {258, 23, 2, 3}, {280, 23, 2, 3}, {260, 22, 2, 2},
	{278, 22, 2, 2}, {262, 21, 3, 2}, {275, 21, 3, 2}, {265, 20, 10, 2},
	{269, 25, 2, 8}, {257, 39, 26, 2},
};
PixelRect leftMouseButtonPixels[] = {
	{257, 35, 13, 4}, {258, 30, 12, 5}, {259, 25, 11, 5},
	{260, 24, 10, 1}, {261, 23, 9, 1}, {263, 22, 7, 1}
};
PixelRect rightMousePixels[] = {
	{264, 153, 12, 2}, {261, 152, 3, 2}, {276, 152, 3, 2}, {259, 151, 2, 2},
	{279, 151, 2, 2}, {258, 150, 2, 2}, {280, 150, 2, 2}, {257, 147, 2, 3},
	{281, 147, 2, 3}, {256, 143, 2, 4}, {282, 143, 2, 4}, {255, 120, 2, 23},
	{283, 120, 2, 23}, {256, 115, 2, 5}, {282, 115, 2, 5}, {257, 111, 2, 4},
	{281, 111, 2, 4}, {258, 108, 2, 3}, {280, 108, 2, 3}, {260, 107, 2, 2},
	{278, 107, 2, 2}, {262, 106, 3, 2}, {275, 106, 3, 2}, {265, 105, 10, 2},
	{269, 110, 2, 8}, {257, 124, 26, 2}
};
PixelRect rightMouseButtonPixels[] = {
	{270, 120, 13, 4}, {270, 115, 12, 5}, {270, 110, 11, 5},
	{270, 109, 10, 1}, {270, 108, 9, 1}, {270, 107, 7, 1}
};


void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	SetDlgItemTextW(hwnd, 9, defaultOutputMessage);
	KillTimer(hwnd, idEvent);
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {
	SendMessage(hwnd, WM_SETFONT, (WPARAM)lParam, TRUE);
	return TRUE;
}

LRESULT CALLBACK EditSubclassProc(HWND hwndEdit, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	switch (uMsg) {
	case WM_PASTE:
		return 0;
		break;
	case WM_CHAR:
		if (wParam == VK_BACK) {
			DWORD start, end;
			SendMessage(hwndEdit, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
			DWORD selectedTextLength = end - start;

			if (selectedTextLength == 0 and GetWindowTextLength(hwndEdit) == 1) {
				SetWindowText(hwndEdit, L"0");
				return 0;
			}
			else if (selectedTextLength == GetWindowTextLength(hwndEdit)) {
				SetWindowText(hwndEdit, L"0");
				return 0;
			}
			return DefSubclassProc(hwndEdit, uMsg, wParam, lParam);
		}
		else if (wParam == 0x30) {
			DWORD start;
			SendMessage(hwndEdit, EM_GETSEL, (WPARAM)&start, NULL);
			if (start == 0) return 0;
		}
		if (iswdigit((wchar_t)wParam)) {
			wchar_t buffer[11];
			GetWindowText(hwndEdit, buffer, sizeof(buffer));
			if ((std::wstring)buffer == L"0") {
				SetWindowText(hwndEdit, &buffer[1]);
			}
		}
		else if ((!iswdigit((wchar_t)wParam) and (wParam != VK_BACK))) {
			return 0;
		}
		DWORD start, end;
		SendMessage(hwndEdit, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
		if (GetWindowTextLength(hwndEdit) == 4 and end - start == 0) return 0;
		break;
	}
	return DefSubclassProc(hwndEdit, uMsg, wParam, lParam);
}

LRESULT CALLBACK MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {
		switch (wParam) {
		case WM_LBUTTONDOWN:
			left::now = std::chrono::steady_clock::now();
			left::start = std::upper_bound(left::arr + left::start, left::arr + left::end + 1, left::now - std::chrono::seconds(1)) - left::arr; // find the index of value that larger than now-1
			if ((left::end - left::start + 1) == left::limit or std::chrono::duration_cast<std::chrono::milliseconds>(left::now - left::arr[left::end]) < std::chrono::milliseconds(left::cooldown)) {
				left::isPreviousBlocked = 1;
				return -1; // block the click
			}
			left::isPreviousBlocked = 0;
			left::end++;
			left::arr[left::end] = left::now;
			// SetDlgItemTextW(hwnd, 9, (L"LEFT     cps: " + std::to_wstring(left::end - left::start + 1) + L"   start: " + std::to_wstring(left::start) + L"   end: " + std::to_wstring(left::end)).c_str()); // show info about algorithm
			// SetDlgItemTextW(hwnd, 9, std::to_wstring((std::chrono::steady_clock::now() - now).count()).c_str()); // show delay between real click and processed click in nanoseconds
			break;

		case WM_LBUTTONUP:
			if (left::isPreviousBlocked) return -1; // block click if mouse left click down blocked
			break;

		case WM_RBUTTONDOWN:
			right::now = std::chrono::steady_clock::now();
			right::start = std::upper_bound(right::arr + right::start, right::arr + right::end + 1, right::now - std::chrono::seconds(1)) - right::arr;
			if ((right::end - right::start + 1) == right::limit or std::chrono::duration_cast<std::chrono::milliseconds>(right::now - right::arr[right::end]) < std::chrono::milliseconds(right::cooldown)) {
				right::isPreviousBlocked = 1;
				return -1; // block the click
			}
			right::isPreviousBlocked = 0;
			right::end++;
			right::arr[right::end] = right::now;
			// SetDlgItemTextW(hwnd, 9, (L"RIGHT     cps: " + std::to_wstring(right::end - right::start + 1) + L"   start: " + std::to_wstring(right::start) + L"   end: " + std::to_wstring(right::end)).c_str()); // show info about algorithm
			// SetDlgItemTextW(hwnd, 9, std::to_wstring((std::chrono::steady_clock::now() - now).count()).c_str()); // show delay in nanoseconds
			break;
		case WM_RBUTTONUP:
			if (right::isPreviousBlocked) return -1; // block click if mouse right click down blocked
			break;
		}
	}
	return CallNextHookEx(mouseHook, nCode, wParam, lParam); // allow mouse event
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	case WM_CREATE: {
		// menu
		HMENU hMenu = CreateMenu();
		HMENU hCommunication = CreateMenu();
		hSettings = CreateMenu();

		AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSettings, TEXT("Settings"));
		AppendMenu(hSettings, MF_STRING, 1001, TEXT("Store Data"));
		AppendMenu(hSettings, MF_STRING, 1002, TEXT("Hide To Tray"));


		AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hCommunication, TEXT("Communication"));
		AppendMenu(hCommunication, MF_STRING, 1101, TEXT("Discord"));
		AppendMenu(hCommunication, MF_STRING, 1102, TEXT("Youtube"));
		AppendMenu(hCommunication, MF_STRING, 1103, TEXT("Github"));

		SetMenu(hwnd, hMenu);
		if (storeData) CheckMenuItem(GetMenu(hwnd), 1001, MF_CHECKED);

		// left
		CreateWindowW(L"BUTTON", L"Left", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 5, 0, 285, 75, hwnd, NULL, NULL, NULL);

		CreateWindowW(L"STATIC", L"Set Left CPS Limit:", WS_CHILD | WS_VISIBLE, 10, 20, 180, 20, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"EDIT", L"0", WS_CHILD | WS_VISIBLE | SS_CENTER, 140, 20, 35, 20, hwnd, (HMENU)5, NULL, NULL);
		CreateWindowW(L"BUTTON", L"Set", WS_CHILD | WS_VISIBLE, 180, 20, 30, 20, hwnd, (HMENU)1, NULL, NULL);
		CreateWindowW(L"STATIC", std::to_wstring(left::limit).c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 215, 20, 35, 20, hwnd, (HMENU)7, NULL, NULL);
		CreateWindowW(L"STATIC", L"Click Cooldown ms:", WS_CHILD | WS_VISIBLE, 10, 45, 160, 20, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"EDIT", L"0", WS_CHILD | WS_VISIBLE | SS_CENTER, 140, 45, 35, 20, hwnd, (HMENU)105, NULL, NULL);
		CreateWindowW(L"BUTTON", L"Set", WS_CHILD | WS_VISIBLE, 180, 45, 30, 20, hwnd, (HMENU)101, NULL, NULL);
		CreateWindowW(L"STATIC", std::to_wstring(left::cooldown).c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 215, 45, 35, 20, hwnd, (HMENU)107, NULL, NULL);
		SetWindowSubclass(GetDlgItem(hwnd, 5), EditSubclassProc, 1, 0);
		SetWindowSubclass(GetDlgItem(hwnd, 105), EditSubclassProc, 1, 0);


		// right
		CreateWindowW(L"BUTTON", L"Right", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 5, 85, 285, 75, hwnd, NULL, NULL, NULL);

		CreateWindowW(L"STATIC", L"Set Right CPS Limit:", WS_CHILD | WS_VISIBLE, 10, 105, 180, 20, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"EDIT", L"0", WS_CHILD | WS_VISIBLE | SS_CENTER, 140, 105, 35, 20, hwnd, (HMENU)6, NULL, NULL);
		CreateWindowW(L"BUTTON", L"Set", WS_CHILD | WS_VISIBLE, 180, 105, 30, 20, hwnd, (HMENU)2, NULL, NULL);
		CreateWindowW(L"STATIC", std::to_wstring(right::limit).c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 215, 105, 35, 20, hwnd, (HMENU)8, NULL, NULL);
		CreateWindowW(L"STATIC", L"Click Cooldown ms:", WS_CHILD | WS_VISIBLE, 10, 135, 160, 20, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"EDIT", L"0", WS_CHILD | WS_VISIBLE | SS_CENTER, 140, 135, 35, 20, hwnd, (HMENU)106, NULL, NULL);
		CreateWindowW(L"BUTTON", L"Set", WS_CHILD | WS_VISIBLE, 180, 135, 30, 20, hwnd, (HMENU)102, NULL, NULL);
		CreateWindowW(L"STATIC", std::to_wstring(right::cooldown).c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 215, 135, 35, 20, hwnd, (HMENU)108, NULL, NULL);
		SetWindowSubclass(GetDlgItem(hwnd, 6), EditSubclassProc, 1, 0);
		SetWindowSubclass(GetDlgItem(hwnd, 106), EditSubclassProc, 1, 0);


		// run & stop
		CreateWindowW(L"STATIC", defaultOutputMessage, WS_CHILD | WS_VISIBLE | SS_CENTER | ES_READONLY, 5, 170, 285, 20, hwnd, (HMENU)9, NULL, NULL); // output box
		CreateWindowW(L"BUTTON", L"STOP", WS_CHILD | WS_VISIBLE | WS_DISABLED, 5, 195, 140, 50, hwnd, (HMENU)3, NULL, NULL); // STOP button
		CreateWindowW(L"BUTTON", L"RUN", WS_CHILD | WS_VISIBLE, 150, 195, 140, 50, hwnd, (HMENU)4, NULL, NULL); // RUN button


		// font
		EnumChildWindows(hwnd, EnumChildProc, (LPARAM)hFont1);
		SendMessage(GetDlgItem(hwnd, 3), WM_SETFONT, (WPARAM)hFont2, TRUE);
		SendMessage(GetDlgItem(hwnd, 4), WM_SETFONT, (WPARAM)hFont2, TRUE);
		
		break;
	}
	case WM_COMMAND: {
		switch (LOWORD(wp)) {
		case 1: { // left cps SET button
			BOOL success;
			UINT num = GetDlgItemInt(hwnd, 5, &success, TRUE); // getting input from user
			if (success) {
				left::limit = num;
				SetDlgItemInt(hwnd, 7, left::limit, TRUE);
				SetDlgItemTextW(hwnd, 9, L"Input Has Set!"); // sending message to inform user
			}
			else SetDlgItemTextW(hwnd, 9, L"Just Integers!"); // this means there isn't an integer in input and send message to inform user
			SetTimer(hwnd, -1, 2000, TimerProc); // starting a timer that runs TimerProc function after 2000 milliseconds
			break;
		}
		case 2: { // right cps SET button
			BOOL success;
			UINT num = GetDlgItemInt(hwnd, 6, &success, TRUE); // getting input from user
			if (success) {
				right::limit = num;
				SetDlgItemInt(hwnd, 8, right::limit, TRUE);
				SetDlgItemTextW(hwnd, 9, L"Input Has Set!"); // sending message to inform user
			}
			else SetDlgItemTextW(hwnd, 9, L"Just Integers!"); // this means there isn't an integer in input and send message to inform user
			SetTimer(hwnd, -1, 2000, TimerProc); // starting a timer that runs TimerProc function after 2000 milliseconds
			break;
		}
		case 101: { // left cooldown set button
			BOOL success;
			UINT num = GetDlgItemInt(hwnd, 105, &success, TRUE); // getting input from user
			if (success) {
				left::cooldown = num;
				SetDlgItemInt(hwnd, 107, left::cooldown, TRUE);
				SetDlgItemTextW(hwnd, 9, L"Input Has Set!"); // sending message to inform user
			}
			else SetDlgItemTextW(hwnd, 9, L"Just Integers!"); // this means there isn't an integer in input and send message to inform user
			SetTimer(hwnd, -1, 2000, TimerProc); // starting a timer that runs TimerProc function after 2000 milliseconds
			break;
		}
		case 102: { // right cooldown set button
			BOOL success;
			UINT num = GetDlgItemInt(hwnd, 106, &success, TRUE); // getting input from user
			if (success) {
				right::cooldown = num;
				SetDlgItemInt(hwnd, 108, right::cooldown, TRUE);
				SetDlgItemTextW(hwnd, 9, L"Input Has Set!"); // sending message to inform user
			}
			else SetDlgItemTextW(hwnd, 9, L"Just Integers!"); // this means there isn't an integer in input and send message to inform user
			SetTimer(hwnd, -1, 2000, TimerProc); // starting a timer that runs TimerProc function after 2000 milliseconds
			break;
		}
		case 3: // STOP button
			EnableWindow(GetDlgItem(hwnd, 4), TRUE); // activateing RUN button
			EnableWindow(GetDlgItem(hwnd, 3), FALSE); // deactivateing STOP button
			UnhookWindowsHookEx(mouseHook); // removeing mousehook
			mouseHook = NULL;
			SetDlgItemTextW(hwnd, 9, L"Program Has Stopped!"); // sending message to inform user
			SetTimer(hwnd, -1, 2000, TimerProc); // starting a timer that runs TimerProc function after 2000 milliseconds
			break;
		case 4: // RUN button
			EnableWindow(GetDlgItem(hwnd, 3), TRUE); // activateing STOP button
			EnableWindow(GetDlgItem(hwnd, 4), FALSE); // deactivateing RUN button
			mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookCallback, GetModuleHandle(NULL), 0); // identifying mousehook
			if (mouseHook == NULL) { // failed to install mouse hook
				MessageBox(hwnd, L"Failed to install mouse hook!", (L"Error Code: " + std::to_wstring(GetLastError())).c_str(), MB_ICONEXCLAMATION | MB_OK); // creating a messagebox to inform user
				PostQuitMessage(-1); // closing the program
			}
			SetDlgItemTextW(hwnd, 9, L"Program Is Running!"); // sending message to inform user
			SetTimer(hwnd, -1, 2000, TimerProc); // starting a timer that runs TimerProc function after 2000 milliseconds
			break;
		case 1001: // Settings / StoreData
			storeData = !storeData;
			if (storeData) CheckMenuItem(GetMenu(hwnd), 1001, MF_CHECKED);
			else CheckMenuItem(GetMenu(hwnd), 1001, MF_UNCHECKED);
			break;
		case 1002: // Settings / Make Invisable
			Shell_NotifyIcon(NIM_ADD, &nid);
			ShowWindow(hwnd, SW_HIDE);
			break;
		case 1101: // Communication / Discord
			ShellExecuteW(hwnd, L"open", L"https://discord.gg/zf25y8y3ug", NULL, NULL, 0); // open it on default browser
			break;
		case 1102: // Communication / Youtube
			ShellExecuteW(hwnd, L"open", L"https://youtube.com/watch?v=dQw4w9WgXcQ", NULL, NULL, 0); // open it on default browser
			break;
		case 1103: // Communication / Github
			ShellExecuteW(NULL, L"open", L"https://github.com/DeusEge/CPS-Limiter", NULL, NULL, 0); // open it on default browser
			break;
		case 1201: // tray_icon / Quit
			PostQuitMessage(0);
			break;
		}
		break;
	}
	case WM_TRAYICON: { // tray_icon event
		switch (LOWORD(lp)) {
		case WM_LBUTTONUP: // left clicked tray_icon
			Shell_NotifyIcon(NIM_DELETE, &nid);
			ShowWindow(hwnd, SW_SHOW);
			break;
		case WM_RBUTTONUP: // right clicked tray_icon
			POINT pt;
			GetCursorPos(&pt);
			SetForegroundWindow(hwnd);
			TrackPopupMenu(hTrayMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
			break;
		}
		break;
	}
	case WM_PAINT: { // generating mouse arts
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		HBRUSH brush = CreateSolidBrush(RGB(100, 100, 100));
		RECT rect;
		for (PixelRect p : leftMousePixels) {
			rect = { p.x, p.y, p.x + p.w, p.y + p.h };
			FillRect(hdc, &rect, brush);
		}
		for (PixelRect p : leftMouseButtonPixels) {
			rect = { p.x, p.y, p.x + p.w, p.y + p.h };
			FillRect(hdc, &rect, brush);
		}
		for (PixelRect p : rightMousePixels) {
			rect = { p.x, p.y, p.x + p.w, p.y + p.h };
			FillRect(hdc, &rect, brush);
		}
		for (PixelRect p : rightMouseButtonPixels) {
			rect = { p.x, p.y, p.x + p.w, p.y + p.h };
			FillRect(hdc, &rect, brush);
		}
		DeleteObject(brush);
		EndPaint(hwnd, &ps);
		break;
	}
	}
	return DefWindowProcW(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
	HKEY hKey;
	if (RegOpenKeyExA(HKEY_CURRENT_USER, keyPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) { // if key is exist
		storeData = 1;

		LONG readResult;

		DWORD dataSize = sizeof(DWORD);
		DWORD dwType = REG_DWORD;

		readResult = RegQueryValueExA(hKey, "l", NULL, &dwType, reinterpret_cast<LPBYTE>(&left::limit), &dataSize);
		if (!(readResult == ERROR_SUCCESS)) MessageBox(hwnd, (L"Failed to read data. Error Code: " + std::to_wstring(readResult)).c_str(), L"Error", MB_ICONERROR);

		readResult = RegQueryValueExA(hKey, "r", NULL, &dwType, reinterpret_cast<LPBYTE>(&right::limit), &dataSize);
		if (!(readResult == ERROR_SUCCESS)) MessageBox(hwnd, (L"Failed to read data. Error Code: " + std::to_wstring(readResult)).c_str(), L"Error", MB_ICONERROR);

		readResult = RegQueryValueExA(hKey, "c", NULL, &dwType, reinterpret_cast<LPBYTE>(&left::cooldown), &dataSize);
		if (!(readResult == ERROR_SUCCESS)) MessageBox(hwnd, (L"Failed to read data. Error Code: " + std::to_wstring(readResult)).c_str(), L"Error", MB_ICONERROR);

		readResult = RegQueryValueExA(hKey, "v", NULL, &dwType, reinterpret_cast<LPBYTE>(&right::cooldown), &dataSize);
		if (!(readResult == ERROR_SUCCESS)) MessageBox(hwnd, (L"Failed to read data. Error Code: " + std::to_wstring(readResult)).c_str(), L"Error", MB_ICONERROR);
	}
	RegCloseKey(hKey);

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	hFont1 = CreateFontW(0, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, TEXT("Calibri"));
	hFont2 = CreateFontW(21, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, TEXT("Arial"));

	WNDCLASSEXW wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = (HICON)LoadImageW(hInst, MAKEINTRESOURCE(101), IMAGE_ICON, 48, 48, LR_DEFAULTCOLOR); // setting icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = appName;
	wc.hIconSm = (HICON)LoadImageW(hInst, MAKEINTRESOURCE(101), IMAGE_ICON, 38, 38, LR_DEFAULTCOLOR); // setting icon that shows on title bar

	if (!RegisterClassExW(&wc)) { // failed window registration
		MessageBox(NULL, L"Window Registration Failed!", (L"Error Code: " + std::to_wstring(GetLastError())).c_str(), MB_ICONEXCLAMATION | MB_OK); // creating a messagebox to inform user
		return -1; // closing the program
	}

	hwnd = CreateWindowExW(NULL, appName, appName, WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, GetSystemMetrics(SM_CXSCREEN) / 2 - 155, GetSystemMetrics(SM_CYSCREEN) / 2 - 155, 310, 310, NULL, NULL, wc.hInstance, NULL);
	if (hwnd == NULL) { // window creation failed
		MessageBox(NULL, L"Window Creation Failed!", (L"Error Code: " + std::to_wstring(GetLastError())).c_str(), MB_ICONEXCLAMATION | MB_OK); // creating a messagebox to inform user
		return -1; // closing the program
	}

	hTrayMenu = CreatePopupMenu();
	AppendMenu(hTrayMenu, MF_STRING, 1201, L"Quit");
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hwnd;
	nid.uID = WM_TRAYICON;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_TRAYICON;
	nid.hIcon = (HICON)LoadImageW(hInst, MAKEINTRESOURCE(101), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	wcscpy_s(nid.szTip, appName);

	MSG msg = { 0 }; // creating a message loop
	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (storeData) {
		HKEY hKey;
		if (RegCreateKeyExA(HKEY_CURRENT_USER, keyPath, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) { // saves data at windows registery book
			LONG setValueResult;

			setValueResult = RegSetValueExA(hKey, "l", 0, REG_BINARY, reinterpret_cast<const BYTE*>(&left::limit), sizeof(left::limit));
			if (!(setValueResult == ERROR_SUCCESS)) MessageBox(hwnd, (L"Failed to set registry value " + std::to_wstring(setValueResult)).c_str(), L"Error", MB_ICONERROR);

			setValueResult = RegSetValueExA(hKey, "r", 0, REG_BINARY, reinterpret_cast<const BYTE*>(&right::limit), sizeof(right::limit));
			if (!(setValueResult == ERROR_SUCCESS)) MessageBox(hwnd, (L"Failed to set registry value " + std::to_wstring(setValueResult)).c_str(), L"Error", MB_ICONERROR);

			setValueResult = RegSetValueExA(hKey, "c", 0, REG_BINARY, reinterpret_cast<const BYTE*>(&left::cooldown), sizeof(left::cooldown));
			if (!(setValueResult == ERROR_SUCCESS)) MessageBox(hwnd, (L"Failed to set registry value " + std::to_wstring(setValueResult)).c_str(), L"Error", MB_ICONERROR);

			setValueResult = RegSetValueExA(hKey, "v", 0, REG_BINARY, reinterpret_cast<const BYTE*>(&right::cooldown), sizeof(right::cooldown));
			if (!(setValueResult == ERROR_SUCCESS)) MessageBox(hwnd, (L"Failed to set registry value " + std::to_wstring(setValueResult)).c_str(), L"Error", MB_ICONERROR);
			RegCloseKey(hKey);
		}
		else {
			MessageBox(hwnd, (L"Failed to create or open registry key " + std::to_wstring(GetLastError())).c_str(), L"Error", MB_ICONERROR);
		}
	}
	else {
		HKEY hKey; // Settings / Delete Data
		if (RegOpenKeyExA(HKEY_CURRENT_USER, keyPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) { // if key is exist
			LONG deleteResult = RegDeleteKeyA(HKEY_CURRENT_USER, keyPath); // delete key
			if (!(deleteResult == ERROR_SUCCESS)) {
				MessageBox(hwnd, (L"Failed To Delete Data " + std::to_wstring(GetLastError())).c_str(), L"Error", MB_ICONERROR);
			}
		}
		RegCloseKey(hKey);
	}
	if (mouseHook != NULL) UnhookWindowsHookEx(mouseHook);
	Shell_NotifyIcon(NIM_DELETE, &nid);
	DeleteObject(hFont1);
	DeleteObject(hFont2);

	return 0;
}
