#ifndef MASTER_H
#define MASTER_H

#include <Windows.h>
#include <TlHelp32.h>
#include <cstring>
using namespace std;

HWND hWnd;
DWORD main_hide = 0;
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
        if (!strcmp(ProcessName, (const char *)pe32.szExeFile)) {
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
        hBoardWindow = FindWindowA(NULL, "屏幕广播");
        if (hBoardWindow == NULL) {
            hBoardWindow = FindWindowA(NULL, "共享屏幕");
        }
        if (hBoardWindow == NULL) {
            hBoardWindow = FindWindowA(NULL, "BlackScreen Window");
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
    RegisterHotKey(NULL, 1, NULL, VK_F2);
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
            WindowHookProc();
        }
    }
}

void WindowSwitchProc() {
    if (!main_hide) {
        ShowWindow(hWnd, SW_HIDE);
        main_hide = 1;
    }
    else {
        ShowWindow(hWnd, SW_RESTORE);
        main_hide = 0;
    }
}

DWORD WINAPI WindowSwitchThreadProc(LPVOID lpParameter) {
    RegisterHotKey(NULL, 1, NULL, VK_F1);
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
            WindowSwitchProc();
        }
    }
}

void Suspend_service() {
    if (main_pid != 0) {
        SuspendProcess(main_pid);
    }
    else {
        MessageBoxA(NULL, "未找到极域进程", "Mythware-Terminator", MB_OK);
    }
}

void Resume_service() {
    if (main_pid != 0) {
        ResumeProcess(main_pid);
    }
    else {
        MessageBoxA(NULL, "未找到极域进程", "Mythware-Terminator", MB_OK);
    }
}

void Check_Mythware_service() {
    FindProcessPid("StudentMain.exe", main_pid);
    if (main_pid == 0) {
        MessageBoxA(NULL, "未找到极域进程", "Mythware-Terminator", MB_OK);
    }
    else {
        MessageBoxA(NULL, "成功找到极域进程", "Mythware-Terminator", MB_OK);
    }
}

#endif // MASTER_H
