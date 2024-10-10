#pragma once
#include <Windows.h>
#include <cmath>
#include <string>
#include <unordered_map>
#include <regex>

// ������� �� ���� ������� ������� �����

// ����� - ���-������� ��������� ��������
typedef std::unordered_map<std::wstring, double>* CONSTANTSPTR;

// ������� ������������� ����� �� 10 �� ������ �������
typedef std::wstring(CALLBACK* DECTOBINPTR)(double);
typedef std::wstring(CALLBACK* DECTOOCTPTR)(double);
typedef std::wstring(CALLBACK* DECTOHEXPTR)(double);

// ������� ������������� ����� �� ������ ������� �� 10
typedef double(CALLBACK* BINTODECPTR)(std::wstring);
typedef double(CALLBACK* OCTTODECPTR)(std::wstring);
typedef double(CALLBACK* HEXTODECPTR)(std::wstring);

// ������� ������������� ����������������� �������
typedef double(CALLBACK* DEGTORADPTR)(double);
typedef double(CALLBACK* RADTODEGPTR)(double);

// ������� ����� ��� ��������� �������� ���������
typedef double(CALLBACK* GETCONSTPTR)(std::wstring&);

// ������� ������� ����������������� ������
typedef double(CALLBACK* FUNCOFNUMPTR)(double, const std::wstring&, int);