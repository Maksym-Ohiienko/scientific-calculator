#pragma once
#include <Windows.h> // Підключення функцій Windows

HINSTANCE hInst; // Дескриптор програми
HWND hWnd;		 // Дескриптор головного вікна

LPCWSTR classNameWnd = L"Scientific calculator"; // Назва класу вікна
LPCWSTR titleWnd	 = L"Main menu";			 // Заголовок вікна