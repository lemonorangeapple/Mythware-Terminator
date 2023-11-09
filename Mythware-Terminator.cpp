#include <bits/stdc++.h>
#include <Windows.h>
#include <TlHelp32.h>
using namespace std;
#define LONG_FULL_SCREEN -1778384896
#define LONG_WINDOW -1764818944

DWORD main_pid = 0;
DWORD hook_pid = 0;
HWND hBoardWindow;
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
DWORD WINAPI KeyHookThreadProc(LPVOID lpParameter) {
    kbdHook = (HHOOK)SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)HookProc, GetModuleHandle(NULL), 0);
    if (kbdHook != NULL) {
        UnhookWindowsHookEx(kbdHook);
    }
    return 0;
}

DWORD WINAPI MouseHookThreadProc(LPVOID lpParameter) {
    mseHook = (HHOOK)SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)HookProc, GetModuleHandle(NULL), 0);
    if (mseHook != NULL) {
        UnhookWindowsHookEx(mseHook);
    }
    return 0;
}

void init();

void Suspend_service() {
    SuspendProcess(main_pid);
    Sleep(3000);
    system("cls");
    init();
}

void Resume_service() {
    ResumeProcess(main_pid);
    Sleep(3000);
    system("cls");
    init();
}

void Unlock_keyboard() {
	CreateThread(NULL, 0, KeyHookThreadProc, NULL, 0, NULL);
	Sleep(3000);
    system("cls");
    init();
}

void Unlock_mouse() {
	CreateThread(NULL, 0, MouseHookThreadProc, NULL, 0, NULL);
	Sleep(3000);
    system("cls");
    init();
}

void init() {
    int n;
    cout << "1. Suspend service"<< endl;
    cout << "2. Resume service" << endl;
    cout << "3. Unlock keyboard" << endl;
    cout << "4. Unlock mouse" << endl;
    cout << "5. Exit" << endl;
    cin >> n;
    switch(n) {
        case 1: Suspend_service(); break;
        case 2: Resume_service(); break;
        case 3: Unlock_keyboard(); break;
        case 4: Unlock_mouse(); break;
        case 5: exit(0); break;
        default: system("cls"); init();
    }
}

int main() {
    SetConsoleTitle("Mythware Terminator");
    HWND hWnd = ::GetForegroundWindow();
    SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
	::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 100, 100, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREPOSITION);
    FindProcessPid("StudentMain.exe", main_pid);
    if (main_pid == NULL) { 
    	ShowWindow(hWnd, SW_HIDE);
        MessageBox(hWnd, "¼«ÓòÎ´ÔËÐÐ£¡", "Mythware Terminator", MB_OK);
        return 0;
    }
	init();
}
