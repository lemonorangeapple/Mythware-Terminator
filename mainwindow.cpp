#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "master.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui -> setupUi(this);
    SetConsoleTitleA("Mythware Terminator");
    hWnd = (HWND)this -> winId();
    ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 100, 100, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREPOSITION);
    FindProcessPid("StudentMain.exe", main_pid);
    if (main_pid == NULL) {
        MessageBoxA(hWnd, "未识别到极域运行\n本程序仅会提供基础功能", "Mythware Terminator", MB_OK);
    }
    CreateThread(NULL, 0, KeyHookThreadProc, NULL, 0, NULL);
    CreateThread(NULL, 0, MouseHookThreadProc, NULL, 0, NULL);
    CreateThread(NULL, 0, WindowHookThreadProc, NULL, 0, NULL);
    CreateThread(NULL, 0, WindowSwitchThreadProc, NULL, 0, NULL);
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::on_pushButton_clicked() {
    WindowSwitchProc();
}


void MainWindow::on_pushButton_4_clicked() {
    WindowHookProc();
}


void MainWindow::on_pushButton_2_clicked() {
    Suspend_service();
}


void MainWindow::on_pushButton_3_clicked() {
    Resume_service();
}


void MainWindow::on_pushButton_5_clicked() {
    Check_Mythware_service();
}

