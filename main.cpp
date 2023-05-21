#include <bits/stdc++.h>
#include <Windows.h>
using namespace std;

void init(); 

void Stop_service() {
    system("PsKill.exe StudentMain.exe");
    Sleep(3000);
    system("cls");
    init();
}

void Suspend_service() {
    system("PsSuspend.exe StudentMain.exe");
    Sleep(3000);
    system("cls");
    init();
}

void Resume_service() {
    system("PsSuspend.exe -r StudentMain.exe");
    Sleep(3000);
    system("cls");
    init();
}

void Open_network_access() {
    system("PsService.exe stop TDNetFilter");
    Sleep(3000);
    system("cls");
    init();
}

void init() {
    int n;
    cout << "1. Stop service" << endl;
    cout << "2. Suspend service"<< endl;
    cout << "3. Resume service" << endl;
    cout << "4. Open network access" << endl;
    cout << "5. Exit" << endl;
    cin >> n;
    switch(n) {
        case 1: Stop_service(); break;
        case 2: Suspend_service(); break;
        case 3: Resume_service(); break;
        case 4: Open_network_access(); break;
        case 5: exit(0); break;
        default: system("cls"); init();
    }
}

int main() {
    SetConsoleTitle("Mythware Terminator");
    HWND hWnd = ::GetForegroundWindow();
	::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 100, 100, SWP_NOMOVE | SWP_NOSIZE);
    init();
}
