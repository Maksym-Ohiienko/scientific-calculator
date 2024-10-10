#include "HLoadTime.h"

// Макрос що визначає параметри експорту відповідно до розрядності системи

#ifdef  _WIN64
#define EXPORTFUNC
#else 
#define EXPORTFUNC comment(linker, "/export:" __FUNCTION__ "=" __FUNCDNAME__)
#endif

// Реалізація функцій для експорту

LOADTIME_API double CALLBACK percentOfNum(double a, double b) {
#pragma EXPORTFUNC
    return (a * b) / 100.0;
}

LOADTIME_API double CALLBACK addNums(double a, double b) {
#pragma EXPORTFUNC
    return a + b;
}

LOADTIME_API double CALLBACK multiplyNums(double a, double b) {
#pragma EXPORTFUNC
    return a * b;
}

LOADTIME_API double CALLBACK divideNums(double a, double b) {
#pragma EXPORTFUNC
    return a / b;
}

LOADTIME_API double CALLBACK powerOfNum(double a, double b) {
#pragma EXPORTFUNC
    if (b == -INFINITY) return -INFINITY;

    if (a < 0 && std::fmod(b, 1.0) != 0) { // Перевірка спроби взяти корень з від'ємного числа

        return std::fmod((1.0 / b), 2.0) == 0 ? -INFINITY : -std::pow(-a, b);
    }

    return std::pow(a, b);
}

LOADTIME_API double rootOfNum(double opnd, const std::wstring& op) {
#pragma EXPORTFUNC
    std::wsmatch match;
    double result = -INFINITY;
    if (std::regex_match(op, match, std::wregex(L"-?sqrt(\\d+)?"))) {
        if (match[1].matched) { // Перевірка спроби введеня степені N < 2
            double base = std::stod(match[1].str());
            base = base > 1 ? base : -INFINITY;

            result = powerOfNum(opnd, 1.0 / base);
        }
        else {
            result = powerOfNum(opnd, 1.0 / 2.0);
        }
    }

    return std::isnan(result) ? -INFINITY : (op[0] == L'-' ? -result : result);
}

LOADTIME_API double logOfNum(double opnd, const std::wstring& op) {
#pragma EXPORTFUNC
    double result = -INFINITY;
    std::wsmatch match;
    if (std::regex_match(op, match, std::wregex(L"-?log(\\d+)?"))) {
        if (match[1].matched) { // Знаходимо логарим основи N при N > 1
            double base = std::stod(match[1].str());
            base = base > 1.0 ? base : -INFINITY;
            result = std::log10(opnd) / std::log10(base); // Формула зміни основи логарифму
        }
        else {
            result = std::log10(opnd);
        }
    }
    // Знаходимо логарим основи 2
    else if (std::regex_match(op, std::wregex(L"-?lg2"))) {
        result = log2(opnd);
    }
    // Знаходимо натуральний логарим основи e
    else if (std::regex_match(op, std::wregex(L"-?ln"))) {
        result = log(opnd);
    }
    return std::isnan(result) ? -INFINITY : (op[0] == L'-' ? -result : result);
}