#include "HWinMain.h"       // Підключення заголовного файлу головного вікна
#include "HCalculator.h"    // Підключення заголовного файлу калькулятора
#include "resource.h"       // Підключення заголовного файлу ресурсів

// Визначення точки входу в програму
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    // Ініціалізація структури вікна
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWindowProc; // Встановлення функції обробки повідомлень вікна
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Встановлення фону вікна
    wc.lpszClassName = classNameWnd; // Встановлення імені класу вікна
    hInst = hInstance; // Збереження дескриптора програми

    // Реєстрація класу вікна
    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Отримання розмірів екрану
    int sWidth = GetSystemMetrics(SM_CXSCREEN);
    int sHeight = GetSystemMetrics(SM_CYSCREEN);

    // Встановлення початкового розміру вікна
    int wndWidth = 600;
    int wndHeight = 200;

    // Створення головного вікна програми
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

    // Перевірка успішного створення вікна
    if (hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Встановлення іконки для вікна
    setWindowIcon(hWnd, L"calculator_icon.ico");

    // Завантаження меню з ресурсів та встановлення його для вікна
    HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
    SetMenu(hWnd, hMenu);

    // Відображення та оновлення вікна
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;

    // Цикл обробки повідомлень вікна
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // Переклад повідомлень
        DispatchMessage(&msg);  // Передача повідомлення обробнику
    }

    return msg.wParam; // Повернення коду виходу з програми
}

