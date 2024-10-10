#include "HWinMain.h"       // ϳ��������� ����������� ����� ��������� ����
#include "HCalculator.h"    // ϳ��������� ����������� ����� ������������
#include "resource.h"       // ϳ��������� ����������� ����� �������

// ���������� ����� ����� � ��������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    // ����������� ��������� ����
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWindowProc; // ������������ ������� ������� ���������� ����
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // ������������ ���� ����
    wc.lpszClassName = classNameWnd; // ������������ ���� ����� ����
    hInst = hInstance; // ���������� ����������� ��������

    // ��������� ����� ����
    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // ��������� ������ ������
    int sWidth = GetSystemMetrics(SM_CXSCREEN);
    int sHeight = GetSystemMetrics(SM_CYSCREEN);

    // ������������ ����������� ������ ����
    int wndWidth = 600;
    int wndHeight = 200;

    // ��������� ��������� ���� ��������
    hWnd = CreateWindowEx(0,
        classNameWnd,
        titleWnd,
        WS_OVERLAPPEDWINDOW,
        (sWidth - wndWidth) / 2,
        (sHeight - wndHeight) / 2,
        wndWidth,
        wndHeight,
        NULL,
        NULL,
        hInstance,
        NULL);

    // �������� �������� ��������� ����
    if (hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // ������������ ������ ��� ����
    setWindowIcon(hWnd, L"calculator_icon.ico");

    // ������������ ���� � ������� �� ������������ ���� ��� ����
    HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
    SetMenu(hWnd, hMenu);

    // ³���������� �� ��������� ����
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;

    // ���� ������� ���������� ����
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // �������� ����������
        DispatchMessage(&msg);  // �������� ����������� ���������
    }

    return msg.wParam; // ���������� ���� ������ � ��������
}

