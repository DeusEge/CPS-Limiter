#define SDL_MAIN_HANDLED // windows defender may not thinks program is a virus while this exist idk why its so idiotic
#include <windows.h> // for win32 gui, mouse hook and timer
#include <shellapi.h> // for opening tabs
#include <string> // for std::to_wstring and .c_str()

HWND hwnd;
HHOOK mouseHook;

HFONT hFont1;
HFONT hFont2;

BOOL sound = 1;

namespace left {
	UINT cps = 0; // this value is will be cps while procress
	UINT limit = 15; // maximum cps limit
	UINT_PTR timerID = 0;
	void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
		cps--;
		KillTimer(hwnd, idEvent);
	}
}

namespace right {
	UINT cps = 0; // this value is will be cps while procress
	UINT limit = 18; // maximum cps limit
	UINT_PTR timerID = 0;
	void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
		cps--;
		KillTimer(hwnd, idEvent);
	}
}

void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	SetDlgItemTextW(hwnd, 9, L"CPS LimiterV5 By DeusEge");
	KillTimer(hwnd, idEvent);
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {
	SendMessage(hwnd, WM_SETFONT, (WPARAM)lParam, TRUE);
	return TRUE;
}

LRESULT CALLBACK MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		switch (wParam) {
		case WM_LBUTTONDOWN:
			if (left::cps == left::limit) return -1;
			left::cps++; // incresing left click cps by one
			left::timerID++;
			SetTimer(hwnd, left::timerID, 1000, left::TimerProc); // starting a timer that runs left::TimerProc after 1000 milliseconds and it will degrase right cps by one
			return CallNextHookEx(mouseHook, nCode, wParam, lParam);
			break;
		case WM_RBUTTONDOWN:
			if (right::cps == right::limit) return -1;
			right::cps++; // incresing right click cps by one
			right::timerID++;
			SetTimer(hwnd, right::timerID, 1000, right::TimerProc); // starting a timer that runs left::TimerProc after 1000 milliseconds and it will degrase right cps by one
			return CallNextHookEx(mouseHook, nCode, wParam, lParam);
		}
	}
	return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_DESTROY:
		if (mouseHook != NULL) UnhookWindowsHookEx(mouseHook);
		DeleteObject(hFont1);
		DeleteObject(hFont2);
		PostQuitMessage(0);
		break;

	case WM_CREATE: {
		HMENU hMenu = CreateMenu();
		HMENU hCommunication = CreateMenu();
		HMENU hSettings = CreateMenu();

		AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSettings, TEXT("Settings"));
		AppendMenu(hSettings, MF_STRING, 101, TEXT("Sound"));

		AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hCommunication, TEXT("Communication"));
		AppendMenu(hCommunication, MF_STRING, 111, TEXT("Discord"));
		AppendMenu(hCommunication, MF_STRING, 112, TEXT("Youtube"));
		AppendMenu(hCommunication, MF_STRING, 113, TEXT("Github"));

		SetMenu(hwnd, hMenu);
		if (sound) CheckMenuItem(GetMenu(hwnd), 101, MF_CHECKED);



		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 5, 10, 285, 65, hwnd, NULL, NULL, NULL);

		CreateWindowW(L"STATIC", L"Set Left CPS Limit:", WS_CHILD | WS_VISIBLE, 10, 20, 130, 20, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 140, 20, 51, 20, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | ES_NUMBER | SS_CENTER, 141, 21, 50, 20, hwnd, (HMENU)5, NULL, NULL);
		CreateWindowW(L"BUTTON", L"Set", WS_CHILD | WS_VISIBLE, 200, 20, 30, 20, hwnd, (HMENU)1, NULL, NULL);
		CreateWindowW(L"STATIC", L"Current Left CPS Limit:", WS_CHILD | WS_VISIBLE, 10, 50, 160, 20, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"STATIC", std::to_wstring(left::limit).c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 170, 50, 60, 20, hwnd, (HMENU)7, NULL, NULL);




		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 264, 68, 12, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 261, 67, 3, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 276, 67, 3, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 259, 66, 2, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 279, 66, 2, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 258, 65, 2, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 280, 65, 2, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 257, 62, 2, 3, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 281, 62, 2, 3, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 256, 58, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 282, 58, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 255, 35, 2, 23, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 283, 35, 2, 23, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 256, 30, 2, 5, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 282, 30, 2, 5, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 257, 26, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 281, 26, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 258, 23, 2, 3, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 280, 23, 2, 3, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 260, 22, 2, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 278, 22, 2, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 262, 21, 3, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 275, 21, 3, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 265, 20, 10, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 269, 25, 2, 8, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 257, 39, 26, 2, hwnd, NULL, NULL, NULL); // mouse art

		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 256, 35, 2, 6, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 258, 35, 2, 6, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 260, 35, 2, 6, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 262, 35, 2, 6, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 264, 35, 2, 6, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 266, 35, 2, 6, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 268, 35, 2, 6, hwnd, NULL, NULL, NULL); // mouse art

		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 258, 30, 2, 5, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 260, 30, 2, 5, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 262, 30, 2, 5, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 264, 30, 2, 5, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 266, 30, 2, 5, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 268, 30, 2, 5, hwnd, NULL, NULL, NULL); // mouse art

		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 258, 26, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 260, 26, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 262, 26, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 264, 26, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 266, 26, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 268, 26, 2, 4, hwnd, NULL, NULL, NULL); // mouse art

		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 258, 24, 12, 2, hwnd, NULL, NULL, NULL); // mouse art

		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 260, 22, 10, 2, hwnd, NULL, NULL, NULL); // mouse art




		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 5, 95, 285, 65, hwnd, NULL, NULL, NULL);

		CreateWindowW(L"STATIC", L"Set Right CPS Limit:", WS_CHILD | WS_VISIBLE, 10, 105, 130, 20, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 140, 105, 51, 20, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | ES_NUMBER | SS_CENTER, 141, 106, 50, 20, hwnd, (HMENU)6, NULL, NULL);
		CreateWindowW(L"BUTTON", L"Set", WS_CHILD | WS_VISIBLE, 200, 105, 30, 20, hwnd, (HMENU)2, NULL, NULL);
		CreateWindowW(L"STATIC", L"Current Right CPS Limit:", WS_CHILD | WS_VISIBLE, 10, 135, 160, 20, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"STATIC", std::to_wstring(right::limit).c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 170, 135, 60, 20, hwnd, (HMENU)8, NULL, NULL);




		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 264, 153, 12, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 261, 152, 3, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 276, 152, 3, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 259, 151, 2, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 279, 151, 2, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 258, 150, 2, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 280, 150, 2, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 257, 147, 2, 3, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 281, 147, 2, 3, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 256, 143, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 282, 143, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 255, 120, 2, 23, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 283, 120, 2, 23, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 256, 115, 2, 5, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 282, 115, 2, 5, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 257, 111, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 281, 111, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 258, 108, 2, 3, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 280, 108, 2, 3, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 260, 107, 2, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 278, 107, 2, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 262, 106, 3, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 275, 106, 3, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 265, 105, 10, 2, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 269, 110, 2, 8, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 257, 124, 26, 2, hwnd, NULL, NULL, NULL); // mouse art

		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 270, 120, 2, 6, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 272, 120, 2, 6, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 274, 120, 2, 6, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 276, 120, 2, 6, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 278, 120, 2, 6, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 280, 120, 2, 6, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 282, 120, 2, 6, hwnd, NULL, NULL, NULL); // mouse art

		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 270, 115, 2, 5, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 272, 115, 2, 5, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 274, 115, 2, 5, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 276, 115, 2, 5, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 278, 115, 2, 5, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 280, 115, 2, 5, hwnd, NULL, NULL, NULL); // mouse art

		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 270, 111, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 272, 111, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 274, 111, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 276, 111, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 278, 111, 2, 4, hwnd, NULL, NULL, NULL); // mouse art
		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 280, 111, 2, 4, hwnd, NULL, NULL, NULL); // mouse art

		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 270, 109, 10, 2, hwnd, NULL, NULL, NULL); // mouse art

		CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 270, 107, 8, 2, hwnd, NULL, NULL, NULL); // mouse art




		CreateWindowW(L"STATIC", L"CPS LimiterV5 By DeusEge", WS_CHILD | WS_VISIBLE | SS_CENTER | ES_READONLY, 5, 170, 285, 20, hwnd, (HMENU)9, NULL, NULL); // output box
		CreateWindowW(L"BUTTON", L"STOP", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_DISABLED, 5, 195, 140, 50, hwnd, (HMENU)3, NULL, NULL); // STOP button
		CreateWindowW(L"BUTTON", L"RUN", WS_CHILD | WS_VISIBLE | WS_BORDER, 150, 195, 140, 50, hwnd, (HMENU)4, NULL, NULL); // RUN button

		EnumChildWindows(hwnd, EnumChildProc, (LPARAM)hFont1); // changing font
		SendMessage(GetDlgItem(hwnd, 3), WM_SETFONT, (WPARAM)hFont2, TRUE); // changing font
		SendMessage(GetDlgItem(hwnd, 4), WM_SETFONT, (WPARAM)hFont2, TRUE); // changing font
		break;
	}
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case 1: { // left SET button
			BOOL success;
			UINT num = GetDlgItemInt(hwnd, 5, &success, TRUE); // getting input from user
			if (success) {
				left::limit = num;
				SetDlgItemInt(hwnd, 7, left::limit, TRUE);
				SetDlgItemTextW(hwnd, 9, L"Left Input Has Set!"); // sending message to inform user
			}
			else { // this means there isn't an integer at input
				SetDlgItemTextW(hwnd, 9, L"Just Integers!"); // sending message to inform user
				if (sound) MessageBeep(MB_ICONWARNING); // playing error sound
			}
			SetTimer(hwnd, -1, 2000, TimerProc); // starting a timer that runs TimerProc function after 2000 milliseconds
			SetFocus(hwnd); // remove focus lines
			break;
		}
		case 2: { // right SET button
			BOOL success;
			UINT num = GetDlgItemInt(hwnd, 6, &success, TRUE); // getting input from user
			if (success) {
				right::limit = num;
				SetDlgItemInt(hwnd, 8, right::limit, TRUE);
				SetDlgItemTextW(hwnd, 9, L"Right Input Has Set!"); // sending message to inform user
			}
			else { // this means there isn't an integer at input
				SetDlgItemTextW(hwnd, 9, L"Just Integers!"); // sending message to inform user
				if (sound) MessageBeep(MB_ICONWARNING); // playing error sound
			}
			SetTimer(hwnd, -1, 2000, TimerProc); // starting a timer that runs TimerProc function after 2000 milliseconds
			SetFocus(hwnd); // remove focus lines
			break;
		}
		case 3: // STOP button
			EnableWindow(GetDlgItem(hwnd, 4), TRUE); // activateing RUN button
			EnableWindow(GetDlgItem(hwnd, 3), FALSE); // deactivateing STOP button
			UnhookWindowsHookEx(mouseHook); // removeing mousehook
			SetDlgItemTextW(hwnd, 9, L"Program Has Stopped!"); // sending message to inform user
			SetFocus(hwnd); // removing focus lines,
			SetTimer(hwnd, -1, 2000, TimerProc); // starting a timer that runs TimerProc function after 2000 milliseconds
			if (sound) MessageBeep(MB_ICONERROR); // playing error sound
			break;
		case 4: // RUN button
			EnableWindow(GetDlgItem(hwnd, 3), TRUE); // activateing STOP button
			EnableWindow(GetDlgItem(hwnd, 4), FALSE); // deactivateing RUN button
			mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookCallback, GetModuleHandle(NULL), 0); // identifying mousehook
			if (mouseHook == NULL) { // failed to install mouse hook
				MessageBox(NULL, L"Failed to install mouse hook!", (L"Error Code: " + std::to_wstring(GetLastError())).c_str(), MB_ICONEXCLAMATION | MB_OK); // creating a messagebox to inform user
				PostQuitMessage(-1); // closing the program
			}
			SetDlgItemTextW(hwnd, 9, L"Program Is Running!"); // sending message to inform user
			SetFocus(hwnd); // removing focus lines
			SetTimer(hwnd, -1, 2000, TimerProc); // starting a timer that runs TimerProc function after 2000 milliseconds
			if (sound) MessageBeep(MB_OK); // playing ok sound
			break;
		case 101: // Settings / Sound
			sound = !sound;
			if (sound) CheckMenuItem(GetMenu(hwnd), 101, MF_CHECKED);
			else CheckMenuItem(GetMenu(hwnd), 101, MF_UNCHECKED);
			break;
		case 111: // Communication / Discord
			ShellExecuteW(hwnd, L"open", L"https://discord.gg/zf25y8y3ug", NULL, NULL, 0); // open it on default browser
			break;
		case 112: // Communication / Youtube
			ShellExecuteW(hwnd, L"open", L"https://youtube.com/watch?v=dQw4w9WgXcQ", NULL, NULL, 0); // open it on default browser
			break;
		case 113: // Communication / Github
			ShellExecuteW(NULL, L"open", L"https://github.com/DeusEge/CPS-Limiter", NULL, NULL, 0); // open it on default browser
			break;
		}
	}
	return DefWindowProcW(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
	hFont1 = CreateFontW(0, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, TEXT("Calibri"));
	hFont2 = CreateFontW(21, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, TEXT("Arial"));

	WNDCLASSEXW wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = (HICON)LoadImageW(wc.hInstance, MAKEINTRESOURCE(101), IMAGE_ICON, 48, 48, LR_DEFAULTCOLOR); // setting icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"CPS LimiterV5";
	wc.hIconSm = (HICON)LoadImageW(wc.hInstance, MAKEINTRESOURCE(101), IMAGE_ICON, 38, 38, LR_DEFAULTCOLOR); // setting icon that shows on title bar

	if (!RegisterClassExW(&wc)) { // failed window registration
		MessageBox(NULL, L"Window Registration Failed!", (L"Error Code: " + std::to_wstring(GetLastError())).c_str(), MB_ICONEXCLAMATION | MB_OK); // creating a messagebox to inform user
		return -1; // closing the program
	}

	hwnd = CreateWindowExW(NULL, L"CPS LimiterV5", L"CPS LimiterV5", WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, GetSystemMetrics(SM_CXSCREEN) / 2 - 155, GetSystemMetrics(SM_CYSCREEN) / 2 - 155, 310, 310, NULL, NULL, wc.hInstance, NULL);
	if (hwnd == NULL) { // window creation failed
		MessageBox(NULL, L"Window Creation Failed!", (L"Error Code: " + std::to_wstring(GetLastError())).c_str(), MB_ICONEXCLAMATION | MB_OK); // creating a messagebox to inform user
		return -1; // closing the program
	}

	MSG msg = { 0 }; // creating a message loop
	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}