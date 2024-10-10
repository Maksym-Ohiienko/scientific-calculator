#pragma once
#include <Windows.h>
#include <string>
#include <regex>
#include <cmath>

// Макрос що визначає необхіднсть імпорту/експорту функцій

#ifdef  LOADTIME_EXPORTS
#define LOADTIME_API __declspec(dllexport)
#else 
#define LOADTIME_API __declspec(dllimport)
#endif


// Функції реалізовані мовою С 
extern "C"
{
    // Функція пошуку відсотку від числа
    LOADTIME_API double CALLBACK percentOfNum(double a, double b);

    // Функція пошуку суми двох чисел
    LOADTIME_API double CALLBACK addNums(double a, double b);

    // Функція пошуку добутку двох чисел
    LOADTIME_API double CALLBACK multiplyNums(double a, double b);

    // Функція пошуку ділення двох чисел
    LOADTIME_API double CALLBACK divideNums(double a, double b);

    // Функція пошуку степені від числа
    LOADTIME_API double CALLBACK powerOfNum(double a, double b);

}

// Функція пошуку кореня Nго степеня від числа
LOADTIME_API double rootOfNum(double opnd, const std::wstring& op);

// Функція пошуку логарифму основи N від числа
LOADTIME_API double logOfNum(double opnd, const std::wstring& op);