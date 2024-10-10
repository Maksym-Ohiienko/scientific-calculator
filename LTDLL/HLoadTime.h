#pragma once
#include <Windows.h>
#include <string>
#include <regex>
#include <cmath>

// ������ �� ������� ����������� �������/�������� �������

#ifdef  LOADTIME_EXPORTS
#define LOADTIME_API __declspec(dllexport)
#else 
#define LOADTIME_API __declspec(dllimport)
#endif


// ������� ��������� ����� � 
extern "C"
{
    // ������� ������ ������� �� �����
    LOADTIME_API double CALLBACK percentOfNum(double a, double b);

    // ������� ������ ���� ���� �����
    LOADTIME_API double CALLBACK addNums(double a, double b);

    // ������� ������ ������� ���� �����
    LOADTIME_API double CALLBACK multiplyNums(double a, double b);

    // ������� ������ ������ ���� �����
    LOADTIME_API double CALLBACK divideNums(double a, double b);

    // ������� ������ ������ �� �����
    LOADTIME_API double CALLBACK powerOfNum(double a, double b);

}

// ������� ������ ������ N�� ������� �� �����
LOADTIME_API double rootOfNum(double opnd, const std::wstring& op);

// ������� ������ ��������� ������ N �� �����
LOADTIME_API double logOfNum(double opnd, const std::wstring& op);