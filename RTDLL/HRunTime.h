#pragma once
#include <Windows.h>
#include <cmath>
#include <string>
#include <unordered_map>
#include <regex>

// Функції та змінні обробки варіації вводу

// Змінна - хеш-таблиця доступних констант
typedef std::unordered_map<std::wstring, double>* CONSTANTSPTR;

// Функції конвертування чисел від 10 до різного формату
typedef std::wstring(CALLBACK* DECTOBINPTR)(double);
typedef std::wstring(CALLBACK* DECTOOCTPTR)(double);
typedef std::wstring(CALLBACK* DECTOHEXPTR)(double);

// Функції конвертування чисел від різного формату до 10
typedef double(CALLBACK* BINTODECPTR)(std::wstring);
typedef double(CALLBACK* OCTTODECPTR)(std::wstring);
typedef double(CALLBACK* HEXTODECPTR)(std::wstring);

// Функції конвертування тригонометричного варіанту
typedef double(CALLBACK* DEGTORADPTR)(double);
typedef double(CALLBACK* RADTODEGPTR)(double);

// Функція гетер для отримання значення константи
typedef double(CALLBACK* GETCONSTPTR)(std::wstring&);

// Функція обробки тригонометричного виразу
typedef double(CALLBACK* FUNCOFNUMPTR)(double, const std::wstring&, int);