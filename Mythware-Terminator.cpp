#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
using namespace std;

DWORD main_pid = 0;
DWORD hook_pid = 0;
HWND hBoardWindow;
HANDLE hBoardHookThread;
HHOOK kbdHook;
HHOOK mseHook;

BOOL FindProcessPid(LPCSTR ProcessName, DWORD& dwPid) {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == NULL)
    {
        return(FALSE);
    }
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);
        return(FALSE);
    }
    BOOL bRet = FALSE;
    do {
        if (!strcmp(ProcessName, pe32.szExeFile)) {
            dwPid = pe32.th32ProcessID;
            bRet = TRUE;
            break;
        }

    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return bRet;
}

void SuspendProcess(DWORD process_id) { 
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);    
    THREADENTRY32 threadEntry; 
    threadEntry.dwSize = sizeof(THREADENTRY32);       
    Thread32First(hThreadSnapshot, &threadEntry);
    do {
        if (threadEntry.th32OwnerProcessID == process_id) {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE,threadEntry.th32ThreadID);
            SuspendThread(hThread);
            CloseHandle(hThread); 
        }
    } while (Thread32Next(hThreadSnapshot, &threadEntry));   
    CloseHandle(hThreadSnapshot); 
}

void ResumeProcess(DWORD process_id) { 
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);    
    THREADENTRY32 threadEntry; 
    threadEntry.dwSize = sizeof(THREADENTRY32);
    Thread32First(hThreadSnapshot, &threadEntry);
    do {
        if (threadEntry.th32OwnerProcessID == process_id) {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE,threadEntry.th32ThreadID);
            ResumeThread(hThread);
            CloseHandle(hThread); 
        }
    } while (Thread32Next(hThreadSnapshot, &threadEntry));   
    CloseHandle(hThreadSnapshot); 
}

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    return FALSE;
}

DWORD WINAPI HideThreadProc(LPVOID lpParameter) {
    while (true) {
        ShowWindow(hBoardWindow, SW_MINIMIZE);
        Sleep(50);
        if (!IsWindow(hBoardWindow)) {
			break;
		}
    }
    return 0;
}

void WindowHookProc() {
	if (!IsWindow(hBoardWindow)) {
		hBoardWindow = FindWindow(NULL, "ÆÁÄ»¹ã²¥");
		if (hBoardWindow == NULL) {
			hBoardWindow = FindWindow(NULL, "¹²ÏíÆÁÄ»");
		}
		if (hBoardWindow == NULL) {
			hBoardWindow = FindWindow(NULL, "BlackScreen Window");
		}
	}
	if (!IsIconic(hBoardWindow)) {
		hBoardHookThread = CreateThread(NULL, 0, HideThreadProc, NULL, 0, NULL);
	}
	else {
		TerminateThread(hBoardHookThread, 0);
		ShowWindow(hBoardWindow, SW_RESTORE);
	}
	return;
}

DWORD WINAPI KeyHookThreadProc(LPVOID lpParameter) {
    while (true) {
        kbdHook = (HHOOK)SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)HookProc, GetModuleHandle(NULL), 0);
        Sleep(50);
        if (kbdHook != NULL) {
            UnhookWindowsHookEx(kbdHook);
        }
    }
    return 0;
}

DWORD WINAPI MouseHookThreadProc(LPVOID lpParameter) {
    while (true) {
        mseHook = (HHOOK)SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)HookProc, GetModuleHandle(NULL), 0);
        ClipCursor(0);
        Sleep(50);
        if (mseHook != NULL) {
            UnhookWindowsHookEx(mseHook);
        }
    }
    return 0;
}

DWORD WINAPI WindowHookThreadProc(LPVOID lpParameter) {
	RegisterHotKey(NULL, 1, NULL, VK_F1);
    MSG msg = {0};
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (msg.message == WM_HOTKEY) {
			WindowHookProc();
		}
	}
}


void init();

void Suspend_service() {
    SuspendProcess(main_pid);
    Sleep(1500);
    system("cls");
    init();
}

void Resume_service() {
    ResumeProcess(main_pid);
    Sleep(1500);
    system("cls");
    init();
}

void init() {
    int n;
    cout << "1. Suspend service"<< endl;
    cout << "2. Resume service" << endl;
    cout << "3. Hide/Show window (Press F1)" << endl;
    cout << "4. Exit" << endl;
    cin >> n;
    switch(n) {
        case 1: Suspend_service(); break;
        case 2: Resume_service(); break;
        case 3: WindowHookProc(); break;
        case 4: exit(0); break;
        default: system("cls"); init();
    }
}

int main() {
    SetConsoleTitle("Mythware Terminator");
    HWND hWnd = ::GetForegroundWindow();
    SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
    ShowWindow(hWnd, SW_MINIMIZE);
	::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 100, 100, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREPOSITION);
    FindProcessPid("StudentMain.exe", main_pid);
    if (main_pid == NULL) {
    	ShowWindow(hWnd, SW_HIDE);
        MessageBox(hWnd, "¼«ÓòÎ´ÔËÐÐ£¡", "Mythware Terminator", MB_OK);
        return 0;
    }
    CreateThread(NULL, 0, KeyHookThreadProc, NULL, 0, NULL);
    CreateThread(NULL, 0, MouseHookThreadProc, NULL, 0, NULL);
    CreateThread(NULL, 0, WindowHookThreadProc, NULL, 0, NULL);
	init();
}
