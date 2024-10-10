#pragma once 

#include <Windows.h>		// ��� ������� Windows 
#include <list>				// ��� ������ � �������� ������� STL
#include <stack>			// ��� ����� � �������� ������� STL
#include <unordered_map>	// ��� ���-������� � �������� ������� STL
#include <thread>			// ��� �������� ���������� ������


////////////////////////////////////////
// ����� ��� ��� ����� �������� ����
extern HINSTANCE hInst;	// ���������� ��������
extern HWND hWnd;		// ���������� ��������� ����
extern HWND hCalcDlg;	// ���������� ������ �����������
extern HWND hSelDlg;	// ���������� ������ ������ ���������
////////////////////////////////////////
	

////////////////////////////////////////
// ³���� ��������� ��������
LRESULT CALLBACK MainWindowProc(HWND, UINT, WPARAM, LPARAM);		// ��������� ��������� ���� ��������(����� �����)

INT_PTR CALLBACK SelectWindowProc(HWND, UINT, WPARAM, LPARAM),		// ��������� ������ ��������� ��� �����
		CALLBACK CalculatorWindowProc(HWND, UINT, WPARAM, LPARAM),  // ��������� ������������(������� ����)
		CALLBACK AboutWindowProc(HWND, UINT, WPARAM, LPARAM);		// ��������� ���� "��� ��������"
////////////////////////////////////////


////////////////////////////////////////
// ������� ����������� ���� �� ����������
void initCalcWindow(),				// ������� ����������� ��������� ����
	 loadDynamicLib(),				// ������� ������������ �������� ��������
	 upLoadDynamicLib(),			// ������� ������������ �������� ��������
	 selChangeLogCombo(),			// ��������� ������� ������ ����������� �������
	 selChangeHistoryCombo(),		// ��������� ������� ������ �������� ������
	 selChangeModeList(),			// ������� ������� ������ ������ �������� �����
	 changeSettingsMenuState(),		// ������� ����������� ���� ����������� 	
	 setWindowIcon(HWND, LPCTSTR);	// ������� ������������ ������ ��� ����
////////////////////////////////////////


////////////////////////////////////////
// ������� ������� �����, �� ������ ������ �����
BOOL termIsAdded(const std::wstring&),	// ������� �������� �� ���� ����� � ������
	 writeFileHistory(),				// ������� ������ � ���� ��������� ������
	 readFileHistory();					// ������� ������� � ����� ����� �����

void histFileProc(),	 // ������� ������� �����
     clearFileHistory(); // ������� �������� ����� �����
////////////////////////////////////////


////////////////////////////////////////
// ������� ������� ��������� ������
std::vector<std::wstring> createStrTokens(std::wstring);		// ������� �������� ��������� ������ �� ������
double strTokensProc(std::vector<std::wstring>);				// ������� ������� ������ ������
std::wstring fillConstRegEx();									// ������� ���������� ������ ��������
std::wstring selectedConst();									// ������� �� ������� ������ ���������
bool comparePriority(const std::wstring&, const std::wstring&); // ������� ��������� ��������� ��������
int getPriority(const std::wstring&);							// ������� ��������� ��������� ��� ��������
void btnClicked(int);											// ������� ������� ���������� �������� ������ ����������
void btn16Clicked(int);											// ������� ������� ���������� ���������� ������ ����������
void calcOperator(std::stack<double>&, const std::wstring&);	// ������� ������� ���������
void showErr(const std::wstring&);								// ������� ��� ����������� �������
////////////////////////////////////////


////////////////////////////////////////
// ������� ������ ��� ������� ���������� ������
std::wstring getLastNum();		// �������� ������ ������� �����
////////////////////////////////////////


////////////////////////////////////////
// ������� ������� ���� �������
double xToDecimal(std::wstring); // ������� ������������� � 10������ ������
std::wstring decimalToX(int);	 // ������� ������������� �� 10������� �������
////////////////////////////////////////
