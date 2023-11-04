#include <bits/stdc++.h>
#include <Windows.h>
#include <TlHelp32.h>
using namespace std;

DWORD main_pid = 0;
DWORD hook_pid = 0;
HHOOK kbdHook;
HHOOK mseHook;

BOOL FindProcessPid(LPCSTR ProcessName, DWORD& dwPid) {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
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
    while (true) {
        kbdHook = (HHOOK)SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)HookProc, GetModuleHandle(NULL), 0);
        Sleep(50);
        if (kbdHook != INVALID_HANDLE_VALUE) {
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
        if (mseHook != INVALID_HANDLE_VALUE) {
            UnhookWindowsHookEx(mseHook);
        }
    }
    return 0;
}

DWORD WINAPI MenuBarThreadProc(LPVOID lpParameter) {
    while (true) {
        HWND hBdCst = FindWindow(NULL, "屏幕广播");
        if (hBdCst == INVALID_HANDLE_VALUE) {
            hBdCst = FindWindow(NULL, " 正在共享屏幕");
        }
        Sleep(50);
        if (hBdCst == INVALID_HANDLE_VALUE) {
            continue;
        }
        HWND menuBar = FindWindowEx(hBdCst, NULL, "AfxWnd80u", NULL);
        EnableWindow(GetDlgItem(menuBar, 1004), FALSE);
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

void init() {
    int n;
    cout << "1. Suspend service"<< endl;
    cout << "2. Resume service" << endl;
    cout << "3. Exit" << endl;
    cin >> n;
    switch(n) {
        case 1: Suspend_service(); break;
        case 2: Resume_service(); break;
        case 3: exit(0); break;
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
        MessageBox(hWnd, "极域未运行！", "Mythware Terminator", MB_OK);
        return 0;
    }
    FindProcessPid("ProcHelper64.exe", hook_pid);
    if (hook_pid != NULL) {
        SuspendProcess(hook_pid);
    }
    HMODULE hook_handle = GetModuleHandle("LibTDProcHook64.dll");
    if (hook_handle != INVALID_HANDLE_VALUE) {
        FreeModule(hook_handle);
    }
    Sleep(50);
    CreateThread(NULL, 0, KeyHookThreadProc, NULL, 0, NULL);
    Sleep(50);
    CreateThread(NULL, 0, MouseHookThreadProc, NULL, 0, NULL);
    Sleep(50);
    CreateThread(NULL, 0, MenuBarThreadProc, NULL, 0, NULL);
    init();
}
