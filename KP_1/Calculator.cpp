#include "HCalculator.h"    // ϳ��������� ����������� ����� ������������
#include "..\HLoadTime.h"   // �������� �������� ������� ������������ �������
#include "..\HRunTime.h"    // �������� �������� �������� ������������ �������
#include "resource.h"

////////////////////////////////////////
// �������� ���������� ����������

// ϳ��������� �������� ��������
#pragma comment (lib, "LTDLL.lib")

// ϳ��������� ����� �������� ���� Windows 6.0.0.0
#pragma comment (linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
////////////////////////////////////////

// ���������� ���������� ������

////////////////////////////////////////
// ��������� �� ������� ���������� ��������

HMODULE      hRunTime       = NULL; // ���������� �������� ��������

GETCONSTPTR  getConst       = NULL; // �������� �� ������� ��������� ���������
DECTOBINPTR  decimalToBin   = NULL; // �������� �� ������� ����������� ����������� ����� � �������
DECTOOCTPTR  decimalToOct   = NULL; // �������� �� ������� ����������� ����������� ����� � �������
DECTOHEXPTR  decimalToHex   = NULL; // �������� �� ������� ����������� ����������� ����� � ��������������
BINTODECPTR  binToDecimal   = NULL; // �������� �� ������� ����������� ��������� ����� � ���������
OCTTODECPTR  octalToDecimal = NULL; // �������� �� ������� ����������� ��������� ����� � ���������
HEXTODECPTR  hexToDecimal   = NULL; // �������� �� ������� ����������� ���������������� ����� � ���������
DEGTORADPTR  degToRad       = NULL; // �������� �� ������� ����������� ������� � ������
RADTODEGPTR  radToDeg       = NULL; // �������� �� ������� ����������� ������ � �������
FUNCOFNUMPTR funcOfNum      = NULL; // �������� �� ������� ���������� ����������������� ������
////////////////////////////////////////


////////////////////////////////////////
std::wstring mainInputStr;								// ����� ��� ��������� ��������� ������
HWND hCalcDlg;											// ���������� ������ �����������
HWND hSelDlg;											// ���������� ������ ������ ���������
std::wstring constRegStr;								// ����� ��� ��������� ���������� ������ ��������
std::list<std::wstring> historyList;					// ������ ��� ���������� ����� ����� ����� �����
std::unordered_map<std::wstring, double> constantsHash; // ������� �������� � �������� ��������
int currHistInd;										// �������� �������� ������� � ������ ����� �����(0 - ������)

enum  NumFormats {  // ���� �������
      NBINARY,		// 2
      NDECIMAL,		// 10
      NHEXADECIMAL,	// 16
      NOCTAL		// 8
};
NumFormats currNFormat; // ��� ��������� ����������� �������
////////////////////////////////////////

// ������� �������� ����� � ����� ������ �������� � ���������� ������
double xToDecimal(std::wstring num) {

    // ���������� �������� ������ �����
    switch (currNFormat) {

        // ���� �������� ������ - ��������
    case NBINARY:
        // ��������� ������� binToDecimal ��� ����������� � ��������� � ����������
        return binToDecimal(num);

        // ���� �������� ������ - ������������
    case NOCTAL:
        // ��������� ������� octalToDecimal ��� ����������� � ������������� � ����������
        return octalToDecimal(num);

        // ���� �������� ������ - ���������������
    case NHEXADECIMAL:
        // ��������� ������� hexToDecimal ��� ����������� � ���������������� � ����������
        return hexToDecimal(num);

        // � ��� ����� ��������
    default:
        // ������������� ���������� ������� std::stod ��� ����������� ����� � ��������� �����
        return std::stod(num);
    }
}

// ������� �������� ��������� ����� � ��� ������� ��������
std::wstring decimalToX(double num) {

    // ���������� �������� ������ �����
    switch (currNFormat) {

        // ���� �������� ������ - ��������
    case NBINARY:
        // ��������� ������� decimalToBin ��� ����������� � �������� ������
        return decimalToBin(num);

        // ���� �������� ������ - ������������
    case NOCTAL:
        // ��������� ������� decimalToOct ��� ����������� � ������������ ������
        return decimalToOct(num);

        // ���� �������� ������ - ���������������
    case NHEXADECIMAL:
        // ��������� ������� decimalToHex ��� ����������� � ��������������� ������
        return decimalToHex(num);

        // � ��� ����� ��������
    default:
        // ������������� ���������� ������� std::to_wstring ��� ����������� ����� � �����
        return std::to_wstring(num);
    }
}

// ������� ���������� ������ ��� ����
void setWindowIcon(HWND hWnd, LPCTSTR path) {

    // ����������� ������ � ��������� �����
    HICON hIcon = (HICON)LoadImage(hInst, path, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);

    // ����������, �� ������ ������ �����������
    if (hIcon != NULL) {
        // ���� ������ ����������� ������, ������������ �� ��� ����
        SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    }
}

// ������� �������� ����������� ��� ������� � ��� ������
void showErr(const std::wstring& msg) {

    mainInputStr = L""; // ������� �������� �����
    
    SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, msg.c_str()); // ³��������� ����� �������
}

// ������� ������� �������� ��������
int getPriority(const std::wstring& op) {

    // ����������, �� � �������� ������������ ��������
    if (std::regex_match(op, std::wregex(
        L"-?(sinh|sin|asinh|asin|cosh|cos|acosh|acos|tanh|tan|atanh|atan|cot|acot|sec|csc)"
        L"|-?log(\\d+)?|-?lg2|-?ln"
        L"|-?sqrt(\\d+)?"))) {

        // ���� ���, ��������� �������� ��������
        return 4;
    }
    // ���� �������� - ��������� �� �������, ��������� �������� 3
    else if (op == L"^") {
        return 3;
    }
    // ���� �������� - ������� �� ������, �������� ��� ������, ��������� �������� 2
    else if (op == L"%" || op == L"*" || op == L"/") {
        return 2;
    }
    // ���� �������� - ��������� ��� ��������, ��������� �������� 1
    else if (op == L"+" || op == L"-") {
        return 1;
    }
    // � ������ ������� ��������� �������� 0
    return 0;
}

// ������� ������� ��������� ��������
bool comparePriority(const std::wstring& op1, const std::wstring& op2) {
    return getPriority(op1) >= getPriority(op2);
}

// ������� ������ ������ ������� ����� �� ���������
std::wstring getLastNum() {
    // ���������� ����� ��� ������ ���������� ����� � ��������� �����
    std::wregex regex(L"-?\\d+(\\.\\d+)?" + constRegStr); 

    // ��������� ��������� ��� ��������� ��� �������� ����������� ������ � �����
    std::wsregex_iterator it(mainInputStr.begin(), mainInputStr.end(), regex);

    // ��������� �������� ��������� ��� ���������� ���� �����������
    std::wsregex_iterator end;

    // ������������ ����� ��� ��������� ���������� �����
    std::wstring result = L"";

    // ����� ���������� ����� � �����
    while (it != end) {
        result = it->str(); // ��������� ��������� ����� � ���������
        ++it; // ������� �� ���������� ���������
    }

    return result; // ���������� ���������� ���������� �����
}

// ������� �������� �������� �� ���� ��������
void calcOperator(std::stack<double>& operands, const std::wstring& op) {

    // ���� ���� �������� ������ ��� �������� - �����
    if (operands.empty() || std::regex_match(op, std::wregex(L"-?\\("))) {

        // ��������� ��������� � ������, ���� �������
        if (!operands.empty() && op[0] == L'-') { 
            double invRes = operands.top() * (-1.0);
            operands.pop();
            operands.push(invRes);
        }

        return; // ����������, � ���� ��������� ��������
    }

    // ������� ��������� "|": ������ ������ �����
    if (op == L"|") {
        double operand = operands.top();
        operands.pop();

        operands.push(fabs(operand));
        return;
    }

    // ������� ������������ ���������: log~?~, lg2, ln
    if (std::regex_match(op, std::wregex(L"-?log(\\d+)?|-?lg2|-?ln"))) {
        double operand = operands.top();
        operands.pop();

        operands.push(logOfNum(operand, op));
        return;
    }

    // ������� ���������������� �������
    if (std::regex_match(op, std::wregex(
        L"-?(sinh|sin|asinh|asin|cosh|cos|acosh|acos|tanh|tan|atanh|atan|coth|cot|acoth|acot|sech|sec|asech|asec|csch|csc|acsch|acsc)"))) {

        double operand = operands.top();
        operands.pop();

        operands.push(funcOfNum(operand, op, IsDlgButtonChecked(hCalcDlg, IDC_RADIODG)));
        return;
    }

    // ������� ������ ����: sqrt~?~
    if (std::regex_match(op, std::wregex(L"-?sqrt(\\d+)?"))) {

        double operand = operands.top();
        operands.pop();

        operands.push(rootOfNum(operand, op));
        return;
    }

    // ���� � ����� ����� 2 ��������, �����
    if (operands.size() < 2) {
        return;
    }

    // ������ ���� �������� � �����
    double second = operands.top();
    operands.pop();

    double first = operands.top();
    operands.pop();

    // ���������� ���������� �������� �� �������� ���������
    if (op == L"+") {
        operands.push(addNums(first, second));
    }
    else if (op == L"*") {
        operands.push(multiplyNums(first, second));
    }
    else if (op == L"/") {
        operands.push(second ? divideNums(first, second) : -INFINITY); // �������� ������ �� ����
    }
    else if (op == L"%") {
        operands.push(percentOfNum(first, second));
    }
    else if (op == L"^") {
        operands.push(powerOfNum(first, second));
    }
}

// ������� �������� �������� ������ 16�� �������
void btn16Clicked(int id) {
    // ��������� ������ ������
    wchar_t btnTxt[2];
    GetDlgItemText(hCalcDlg, id, btnTxt, 2);
    std::wstring btnTxtStr(btnTxt);

    // ��������� ������ ������ �� ����� �������� �� ����������� ����������
    mainInputStr += btnTxtStr;
    SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
}

// ������� �������� ��������� ���� ����� �����
void histFileProc() {
    // ����� ����� � ����
    if (!writeFileHistory()) {
        showErr(L"Cannot write data in file!");
    }
    // ������� ����� � �����
    if (!readFileHistory()) {
        showErr(L"Cannot read data from file!");
    }
}

// ������� �������� � ��� ������� ����������
void btnClicked(int id) {

    // ��������� ������ ������
    wchar_t btnTxt[10];
    GetDlgItemText(hCalcDlg, id, btnTxt, 10);

    std::wstring btnTxtStr(btnTxt);

    // �������� ����� �������� �� ��������� �����������
    if (btnTxtStr == L"AC") {
        mainInputStr = L"";
        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // ��������� ����� ��� ����� �� ����� �� ��������� �����������
    else if (std::regex_match(btnTxtStr, std::wregex(L"\\d")) || btnTxtStr == L"(" || btnTxtStr == L")") {

        mainInputStr += btnTxtStr;
        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // ��������� ��������������� �������
    else if (std::regex_match(btnTxtStr,
    std::wregex(L"sinh|sin|asinh|asin|cosh|cos|acosh|acos|tanh|tan|atanh|atan|coth|cot|acoth|acot|sech|sec|asech|asec|csch|csc|acsch|acsc"))) {

        mainInputStr += btnTxtStr + L"(";
        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // ��������� ������
    else if (std::regex_match(btnTxtStr, std::wregex(L"sqrt\\(x\\)|sqrt3\\(x\\)|sqrt4\\(x\\)"))) {

        btnTxtStr.pop_back(); btnTxtStr.pop_back();
        mainInputStr += btnTxtStr;
        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // ��������� ����������� ���������
    else if (btnTxtStr == L"10^x") {
        mainInputStr += L"10^(";
        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // ��������� ������ Y �������
    else if (btnTxtStr == L"sqrtY(x)") {
        mainInputStr += L"sqrt(";
        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // ��� ��������� �� mainInputStr �� ������� ���� ���������
    else if (mainInputStr.empty()) {
        return;
    }

    // ����� ����� �� ���� ��������� ��������
    else if (btnTxtStr == L"|x|") {
        // �������� �� ������� ������ - �����
        if (!std::regex_search(std::wstring(1, mainInputStr.back()), std::wregex(L"\\d"))) {
            return; // ���� ������� ������ �� � ������, ��������
        }

        // ����������� ������� ���������� ����� � �����
        size_t lastNumPos = mainInputStr.find_last_of(L" ()+-*/%^");
        // ���������� ����������� �������� ���������� �����
        std::wstring lastNabs = L"|(" + getLastNum() + L")|";

        // ����� ���������� ����� �� ���� ��������� ��������
        if (lastNumPos == std::wstring::npos) {
            mainInputStr = lastNabs; // ���� ������ ����� �����, �������� ���� �����
        }
        else if (mainInputStr[lastNumPos] == L'-') {
            mainInputStr.replace(lastNumPos, getLastNum().length(), lastNabs); // ���� ������ ����� ��'����
        }
        else {
            mainInputStr.replace(lastNumPos + 1, getLastNum().length(), lastNabs); // ���� ������ ����� ������
        }

        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // ����� ����� �� ���� �������� ��������
    else if (btnTxtStr == L"+-") {
        // ��������� ���������� ����� � �����
        std::wstring lastN = getLastNum();

        // ����������� ��������
        if (std::regex_match(lastN, std::wregex(constRegStr))) {
            // ���� ����� ��������� �� ��������� ����� ��������
            int indToCk = mainInputStr.size() - lastN.size();
            double hashValue;
            if (mainInputStr[indToCk] == L'-') {
                lastN.erase(0, 1);
                hashValue = constantsHash[lastN];
            }
            else {
                hashValue = -constantsHash[lastN];
            }
            mainInputStr.replace(indToCk, lastN.size() + 1, std::to_wstring(hashValue));
            SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
            return;
        }

        wchar_t lastChar = mainInputStr.back();
        // �������� �� ������� ������ - �����
        if (!std::regex_search(std::wstring(1, lastChar), std::wregex(L"\\d"))) {
            return; // ���� ������� ������ �� � ������, ��������
        }

        // ����������� ���������� �����
        double lastNDbl = std::stod(lastN);
        lastNDbl *= -1; // ���� ����� �����
        size_t lastNumPos = mainInputStr.find_last_of(L" ()|+-*/%^=");

        // ����� ���������� ����� �� ������ ��������
        if (lastNumPos == std::wstring::npos) {
            mainInputStr = std::to_wstring(lastNDbl); // ���� ������ ����� �����, �������� ���� �����
        }
        else if (mainInputStr[lastNumPos] == L'-') {
            mainInputStr.replace(lastNumPos, lastN.length(), std::to_wstring(lastNDbl)); // ���� ������ ����� ��'����
        }
        else {
            mainInputStr.replace(lastNumPos + 1, lastN.length(), std::to_wstring(lastNDbl)); // ���� ������ ����� ������
        }

        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // ��������� ���������� ������� � �����
    else if (btnTxtStr == L"<--")
    {
        mainInputStr.erase(mainInputStr.size() - 1);
        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    else { // ��������� ����� ���������

        // ������� ���������� ��������� ����������
        if (btnTxtStr == L"=") {
            
            // �������� ����� �� ������ �� ���������� ������
            std::vector<std::wstring> tokens = createStrTokens(mainInputStr);
            double result = strTokensProc(tokens);

            if (result == -INFINITY) { // �������� �� ������� ��������
                showErr(L"Input error!");
                return;
            }

            // ������� ����� ����� �� ����������� ���������� � �������� ������
            histFileProc();
            mainInputStr = decimalToX(result);
        }

        // ������� ���������� �������� ��������
        else if (btnTxtStr == L"-") {
            
            if (mainInputStr.back() == L'-')
            {
                mainInputStr.pop_back();
                SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
                return;
            }

            // ������ ������� "+" �� "-" �� �������� ���������/�������� ��� �������� �- �� �- ������� ���������
            mainInputStr += L"+ -";
        }

        // ������� ���������� ������
        else if (btnTxtStr == L".")
        {
            // ��������, �� ����� ������ ������ �� ���������� �����
            if (!std::regex_search(std::wstring(1, mainInputStr.back()), std::wregex(L"\\d"))) {
                return;
            }

            std::wstring lastN = getLastNum();
            if (lastN.find(L'.') != std::wstring::npos) {
                return;
            }

            mainInputStr += L'.'; // ���� ��� �����, ������
        }

        // ������� ����������� �� �������� �������
        else if (btnTxtStr == L"x^2" || btnTxtStr == L"x^3") {
            btnTxtStr.erase(0, 1);
            mainInputStr += btnTxtStr;
        }

        // ������� y �������
        else if (btnTxtStr == L"x^y") {
            mainInputStr += L"^";
        }

        // ������� ��� ����� �������
        else {
            mainInputStr += btnTxtStr;
        }

        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

}

// ������� �������� ������ �� ��������� Reverse Polish Notation (RPN) ���������
double strTokensProc(std::vector<std::wstring> tokens) {
    // ���� ��� ����� (��������)
    std::stack<double> operands;
    // ���� ��� ���������
    std::stack<std::wstring> operators;

    // ����������� �� ������� ������
    for (int i = 0; i < tokens.size(); ++i) {
        // ���� ����� � ������
        if (std::regex_match(tokens[i], std::wregex(L"-?\\d+[A-F]+?|-?\\d+\\.[A-F]|-?[A-F]+|-?\\d+(\\.\\d+)?"))) {
            // ������������ ����� � ��������� ������� �� ������ �� ����� ��������
            operands.push(xToDecimal(tokens[i]));
        }
        // ���� ����� � ����������
        else if (std::regex_match(tokens[i], std::wregex(constRegStr))) {
            // �������� �������� ��������� �� ������ �� ����� ��������
            operands.push(getConst(tokens[i]));
        }
        // ���� ����� � ����������
        else if (std::regex_match(tokens[i], std::wregex(
            L"[+*/%^]"
            L"|-?(sinh|sin|asinh|asin|cosh|cos|acosh|acos|tanh|tan|atanh|atan|coth|cot|acoth|acot|sech|sec|asech|asec|csch|csc|acsch|acsc)"
            L"|-?log(\\d+)?|-?lg2|-?ln"
            L"|-?sqrt(\\d+)?"))) {
            // ������� ���������
            while (!operators.empty() && comparePriority(operators.top(), tokens[i])) {
                // ��������� ������� ��� ���������� ������ � �����
                calcOperator(operands, operators.top());
                // ��������� ���������� �������� � ����� ���������
                operators.pop();
            }
            // ������ �������� �������� �� ����� ���������
            operators.push(tokens[i]);
        }
        // ���� ����� � ����������� ������
        else if (std::regex_match(tokens[i], std::wregex(L"-?\\("))) {
            // ������ ���������� ����� �� ����� ���������
            operators.push(tokens[i]);
        }
        // ���� ����� � �������
        else if (tokens[i] == L"|" && (i != 0) && tokens[i - 1] == L")") {
            // ������ �� ���������� ����
            operators.push(tokens[i]);
            calcOperator(operands, operators.top());
            operators.pop(); // ��������� ���������� ��������
        }
        // ���� ����� � ����������� ������
        else if (tokens[i] == L")") {
            // ������� ���������� �����
            while (!operators.empty() && !std::regex_match(operators.top(), std::wregex(L"-?\\("))) {
                // ��������� ������� ��� ���������� ������ � �����
                calcOperator(operands, operators.top());
                // ��������� ���������� �������� � ����� ���������
                operators.pop();
            }
            // ���� ���� ��������� �� ������� � ������ ������� � ����������� ������
            if (!operators.empty() && std::regex_match(operators.top(), std::wregex(L"-?\\("))) {
                // ���� �������, ��������� ��������� � ������
                if (operators.top()[0] == L'-') {
                    double invRes = operands.top() * (-1.0);
                    operands.pop();
                    operands.push(invRes);
                }
                // ��������� ���������� ����� � ����� ���������
                operators.pop();
            }
        }
    }

    // ������� ����� ��������� � ����� ���������
    while (!operators.empty()) {
        // ��������� ������� ��� ���������� ������ � �����
        calcOperator(operands, operators.top());
        // ��������� ���������� �������� � ����� ���������
        operators.pop();
    }

    // ��������� ��������� (������ ������� � ����� ��������)
    return operands.empty() ? -INFINITY : operands.top();
}

// ������� �������� ���������� ����� � ������� ��������
std::wstring fillConstRegEx() {
    // ��������� - �����, �� ������ �� ��������� � �������
    std::wstring result;

    // ����������� �� ����� �������� � �������
    for (const auto& pair : constantsHash) {
        // ������� �����, �� ������ ������� ��������� � �������� ������ �����
        std::wstring constant = L"|-?" + pair.first;
        // ������ ������� ��������� �� ����������
        result += constant;
    }
    // ��������� ���������� �����, �� ������ �� ���������
    return result;
}

// ������� ������� �������� ����� �� ������
std::vector<std::wstring> createStrTokens(std::wstring str) {

    // ���� ����������� ������ ��� ����� � ��������� �� �������� �����������
    std::wstring numberRegStr = ((IsDlgButtonChecked(hCalcDlg, IDC_ACTIVEGPCK) && currNFormat == NHEXADECIMAL) ?
        L"-?\\d+[A-F]+?|-?\\d+\\.[A-F]|-?[A-F]+|-?\\d+(\\.\\d+)?" : L"-?\\d+(\\.\\d+)?");

    // ���� ����������� ������ ��� ���������������� ������� � ��������� �� �������� �����������
    std::wstring trigRegStr = (IsDlgButtonChecked(hCalcDlg, IDC_ACTIVEGPCK) ?
        L"|-?(sinh|sin|asinh|asin|cosh|cos|acosh|acos|tanh|tan|atanh|atan|coth|cot|acoth|acot|sech|sec|asech|asec|csch|csc|acsch|acsc)" : L"");

    // ǳ������ ��� �������� ������ � �����
    std::wregex regex(numberRegStr +
        L"|-?\\("
        L"|[+*/%)^|]"
        L"|-?sqrt(\\d+)?"
        +
        constRegStr
        +
        L"|-?log(\\d+)?|-?lg2|-?ln"
        + trigRegStr);

    // �������� ��� ������ ������ � �����
    std::wsregex_iterator it(str.begin(), str.end(), regex);
    std::wsregex_iterator end;

    std::vector<std::wstring> tokens;

    // ��������� ��������� ������ �� �������
    while (it != end) {
        tokens.push_back(it->str());
        ++it;
    }

    // ��������� ������ ������
    return tokens;
}

// ������� ���� ����� �������� ����� �����
void upListProc() {

    // �������� �� ����� �������� ����
    if (currHistInd <= 0) {
        return;
    }
   
    currHistInd--;

    HWND histCb = GetDlgItem(hCalcDlg, IDC_CBHISTORY);
    SendMessage(histCb, CB_SETCURSEL, currHistInd, 0);
    selChangeHistoryCombo(); // ������� ������ ��������
}

// ������� ���� ���� �������� ����� �����
void downListProc() {

    // �������� �� ����� ���������� �����
    if (currHistInd >= historyList.size() - 1) {
        return;
    }

    currHistInd++;

    HWND histCb = GetDlgItem(hCalcDlg, IDC_CBHISTORY);
    SendMessage(histCb, CB_SETCURSEL, currHistInd, 0);
    selChangeHistoryCombo(); // ������� ������ ��������
}

// ������� �������� ���� ������������
void initCalcWindow() {

    currNFormat = NDECIMAL; // ������������ ����������� ������� �����

    setWindowIcon (hCalcDlg, L"calculator_icon.ico"); // ������������ ������ ����

    // ������������ ���������� ����� ��������
    CheckDlgButton(hCalcDlg, IDC_RADIODG, BST_CHECKED);
    CheckDlgButton(hCalcDlg, IDC_ACTIVEGPCK, BST_UNCHECKED);

    // ������� ����� � �����
    if (!readFileHistory()) {
        showErr(L"History file is damaged!");
    }

    // ����������� �����-����� ��� ���������
    HWND comboBox = GetDlgItem(hCalcDlg, IDC_LOGCB);
    if (comboBox)
    {
        SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)(L"log(x)"));
        SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)(L"ln(x)"));
        SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)(L"lg2(x)"));

        SendMessage(comboBox, CB_SETCURSEL, 2, 0);

    }

    // ����������� �����-����� ��� �����
    HWND hComboBox = GetDlgItem(hCalcDlg, IDC_CBHISTORY);
    int index = SendMessage(GetDlgItem(hCalcDlg, IDC_CBHISTORY), CB_INSERTSTRING, (WPARAM)-1, (LPARAM)L"--- History ---");
    SendMessage(hComboBox, CB_SETCURSEL, index, 0);

    // ����������� ������ ��������� ������� ��������
    HWND listBox = GetDlgItem(hCalcDlg, IDC_INPUTMODELST);
    if (listBox)
    {
        SendMessage(listBox, LB_ADDSTRING, 0, (LPARAM)(L"binary(2)"));
        SendMessage(listBox, LB_ADDSTRING, 0, (LPARAM)(L"octal(8)"));
        SendMessage(listBox, LB_ADDSTRING, 0, (LPARAM)(L"decimal(10)"));
        SendMessage(listBox, LB_ADDSTRING, 0, (LPARAM)(L"hexadecimal(16)"));

        SendMessage(listBox, LB_SETCURSEL, (int)NDECIMAL, 0);

    }
}

// ������� �������� ���� �������� � ����� ����� 
void selChangeHistoryCombo() {

    HWND comboBox = GetDlgItem(hCalcDlg, IDC_CBHISTORY);
    wchar_t itemTxt[255]{ '\0' };

    // ��������� ������� �������� ��������
    currHistInd = SendMessage(comboBox, CB_GETCURSEL, 0, 0);
    if (currHistInd != CB_ERR) {
        SendMessage(comboBox, CB_GETLBTEXT, currHistInd, (LPARAM)itemTxt);
    }
    
    std::wstring itemTxtStr(itemTxt);
    if (itemTxtStr == L"--- History ---") {
        return;
    }

    // ��������� ��������� ����� ����� � ������� ������� � �����
    mainInputStr = itemTxtStr;
    SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());

}

// ������� �������� ���� �������� � ������ ������������ �������
void selChangeLogCombo() {

    HWND comboBox = GetDlgItem(hCalcDlg, IDC_LOGCB);
    wchar_t itemTxt[10]{ '\0' };

    // ��������� ������� ��������� ��������
    int selInd = SendMessage(comboBox, CB_GETCURSEL, 0, 0);
    if (selInd != CB_ERR) {
        SendMessage(comboBox, CB_GETLBTEXT, selInd, (LPARAM)itemTxt);
    }

    // ��������� ������ ������� ����� ����������
    std::wstring itemTxtStr(itemTxt);
    itemTxtStr.pop_back(); itemTxtStr.pop_back();
    
    // ��������� ������ ��������� �� ��������� ����� �����
    mainInputStr += itemTxtStr;
    SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());

}

// ������� �������� ���� �������� � ������ �������
void selChangeModeList() {

    HWND modeLst = GetDlgItem(hCalcDlg, IDC_INPUTMODELST);

    // ��������� ������� �������� ��������
    int selInd = SendMessage(modeLst, LB_GETCURSEL, 0, 0);
    if (selInd == LB_ERR) {
        return;
    }

    // ������������ ��������� ������� ����� �������� �� �������� ��������
    currNFormat = (NumFormats)selInd;

    // ����� ��� ���������� ����� ��� ���������������� ������ � ��������� �� �������� �������
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16ABTN), currNFormat == NHEXADECIMAL ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16BBTN), currNFormat == NHEXADECIMAL ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16CBTN), currNFormat == NHEXADECIMAL ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16DBTN), currNFormat == NHEXADECIMAL ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16EBTN), currNFormat == NHEXADECIMAL ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16FBTN), currNFormat == NHEXADECIMAL ? SW_SHOWNORMAL : SW_HIDE);

}

// ������� �������� ���� �������� ����������� ����
void changeSettingsMenuState() {

    BOOL isChecked = IsDlgButtonChecked(hCalcDlg, IDC_ACTIVEGPCK);

    // ��������� ������ ������� �� �
    if (isChecked) loadDynamicLib(); // ������������ �������� ��������
    else upLoadDynamicLib(); // ������������ �������� ��������
    
    // ������������ ���������� �������� ����� �����������
    HWND settingsGp = GetDlgItem(hCalcDlg, IDC_SETTINGSGP);
    EnableWindow(settingsGp, isChecked);

    // ³���������� ��� ������������ �������� �����
    ShowWindow(GetDlgItem(hCalcDlg, IDC_INPUTMODELST),  isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_RADIORD),       isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_RADIODG),       isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_SELCONSTBTN),   isChecked ? SW_SHOWNORMAL : SW_HIDE);
    
    ShowWindow(GetDlgItem(hCalcDlg, IDC_SINBTN),    isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_ASINBTN),   isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_SINHBTN),   isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_ASINHBTN),  isChecked ? SW_SHOWNORMAL : SW_HIDE);

    ShowWindow(GetDlgItem(hCalcDlg, IDC_COSBTN),    isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_ACOSBTN),   isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_COSHBTN),   isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_ACOSHBTN),  isChecked ? SW_SHOWNORMAL : SW_HIDE);

    ShowWindow(GetDlgItem(hCalcDlg, IDC_TANBTN),    isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_ATANBTN),   isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_TANHBTN),   isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_ATANHBTN),  isChecked ? SW_SHOWNORMAL : SW_HIDE);

    ShowWindow(GetDlgItem(hCalcDlg, IDC_COTBTN),    isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_ACOTBTN),   isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_COTHBTN),   isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_ACOTHBTN),  isChecked ? SW_SHOWNORMAL : SW_HIDE);

    ShowWindow(GetDlgItem(hCalcDlg, IDC_SECBTN),    isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_ASECBTN),   isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_SECHBTN),   isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_ASECHBTN),  isChecked ? SW_SHOWNORMAL : SW_HIDE);

    ShowWindow(GetDlgItem(hCalcDlg, IDC_CSCBTN),    isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_ACSCBTN),   isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_CSCHBTN),   isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_ACSCHBTN),  isChecked ? SW_SHOWNORMAL : SW_HIDE);



    ShowWindow(GetDlgItem(hCalcDlg, IDC_DGSTATIC),      isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_RDSTATIC),      isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_MODESTATIC),    isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_FUNCSTATIC),    isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_TRIGONSTATIC),  isChecked ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_CONSTSTATIC),   isChecked ? SW_SHOWNORMAL : SW_HIDE);


    ShowWindow(GetDlgItem(hCalcDlg, IDC_16ABTN), SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16BBTN), SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16CBTN), SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16DBTN), SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16EBTN), SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16FBTN), SW_HIDE);

    // ��������� ����
    InvalidateRect(hCalcDlg, NULL, TRUE);
}

// ������� ��������� �������� ��������
void loadDynamicLib() {

    hRunTime = LoadLibrary(TEXT("RTDLL.dll"));
    if (!hRunTime) {
        showErr(L"Cannot load dynamic library!");
        return;
    }

    // ��������� ����� ��������� ������� � ��������
    decimalToBin = (DECTOBINPTR)GetProcAddress(hRunTime, "decimalToBin");
    decimalToOct = (DECTOOCTPTR)GetProcAddress(hRunTime, "decimalToOct");
    decimalToHex = (DECTOHEXPTR)GetProcAddress(hRunTime, "decimalToHex");

    binToDecimal    = (BINTODECPTR)GetProcAddress(hRunTime, "binToDecimal");
    octalToDecimal  = (OCTTODECPTR)GetProcAddress(hRunTime, "octalToDecimal");
    hexToDecimal    = (HEXTODECPTR)GetProcAddress(hRunTime, "hexToDecimal");
    degToRad        = (DEGTORADPTR)GetProcAddress(hRunTime, "degToRad");
    radToDeg        = (RADTODEGPTR)GetProcAddress(hRunTime, "radToDeg");
    funcOfNum       = (FUNCOFNUMPTR)GetProcAddress(hRunTime,"funcOfNum");
    getConst        = (GETCONSTPTR)GetProcAddress(hRunTime, "getConst");

    // ��������� ������ ������� ��������
    CONSTANTSPTR constantsPtr = (CONSTANTSPTR)GetProcAddress(hRunTime, "constants");
    constantsHash = *constantsPtr;

    // ���������� ����������� ������ � ������� ��������
    constRegStr = fillConstRegEx();
}

// ������� ��������� �������� ��������
void upLoadDynamicLib() {

    // �������� ��������� �� �������
    getConst        = NULL;
    decimalToBin    = NULL;
    decimalToOct    = NULL;
    decimalToHex    = NULL;
    binToDecimal    = NULL;
    octalToDecimal  = NULL;
    hexToDecimal    = NULL;
    degToRad        = NULL;
    radToDeg        = NULL;
    funcOfNum       = NULL;

    // �������� ������� �������� �� ����������� ������
    constantsHash = {};
    constRegStr   = L"";

    // ��������� �������, ���'������ � ��������� ���������
    if (hRunTime) {
        FreeLibrary(hRunTime);
        hRunTime = NULL;
    }

    // ���������� �� ����������� �������
    currNFormat = NDECIMAL;
    SendMessage(GetDlgItem(hCalcDlg, IDC_INPUTMODELST), LB_SETCURSEL, (int)currNFormat, 0);

    // �������� ��������� ����� �� ��������� �� �����
    mainInputStr = L"";
    SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
}

// ������� ������ ������� ����� �� ����� �����
BOOL writeFileHistory() {

    HANDLE hFile = CreateFile(
        L"history.txt",
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS, // ³�������� ��� ���������, ���� ���� �� ����
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        return FALSE;
    }

    // ������� �� ���� �����
    SetFilePointer(hFile, 0, NULL, FILE_END);

    DWORD bytesWritten;
    std::wstring term = mainInputStr + L"\n";

    // ��������, �� ����� ����� ��� ���� � ���� �����
    if (termIsAdded(mainInputStr)) {
        CloseHandle(hFile);
        return FALSE;
    }

    // ����� ������ � ����
    WriteFile(
        hFile,
        term.c_str(),
        term.length() * sizeof(wchar_t),
        &bytesWritten,
        NULL
    );

    CloseHandle(hFile);
    return TRUE;
}

// ������� ����� ������ � �����
BOOL readFileHistory() {

    historyList.clear();

    HANDLE hFile = CreateFile(
        L"history.txt",
        GENERIC_READ,
        FILE_SHARE_READ, // ����� �� ������� � ����� �������
        NULL,
        OPEN_EXISTING,   // ³������ ����� �������� ����
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        return FALSE; // ������� �������� �����
    }

    DWORD bytesRead;
    WCHAR buff[1024];
    std::wstring currLn;

    // ���������� ����� � �����
    while (ReadFile(hFile, buff, 1024 * sizeof(WCHAR), &bytesRead, NULL) && bytesRead != 0) {

        for (DWORD i = 0; i < bytesRead / sizeof(WCHAR); ++i) {
            if (buff[i] == L'\n') {
                historyList.push_back(currLn); // ��������� ��������� ����� �� ������ �����
                currLn.clear(); // �������� ��������� �����
            }
            else {
                currLn += buff[i]; // ��������� ������� �� ��������� �����
            }
        }
    }  
    
    // ��������� ���������� �����, ���� �� �� ��� ���������� �������� ������ �����
    if (!currLn.empty()) {
        historyList.push_back(currLn);
    }

    // ���������� ������ � ������
    CloseHandle(hFile);

    HWND histCb = GetDlgItem(hCalcDlg, IDC_CBHISTORY);
    SendMessage(histCb, CB_RESETCONTENT, 0, 0);

    // ��������� ����� �� �����-�����
    for (const auto& term : historyList) {
        SendMessage(histCb, CB_INSERTSTRING, 0, (LPARAM)(term.c_str()));
    }
   
    // ������������ ��������� ������� �����
    currHistInd = 0;
    SendMessage(histCb, CB_SETCURSEL, currHistInd, 0);
    
    return TRUE;
}

// ������� ����� ������ �����
void clearFileHistory() {

    HANDLE hFile = CreateFile(
        L"history.txt",
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS, // �������� ����, �������� �������� (���� ����� �)
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        showErr(L"Cannot open file to clear!"); // ������� �������� �����
        return;
    }

    DWORD bytesWritten;
    std::wstring emptyStr = L"\0";

    // �������� ������� ����� �� �����
    WriteFile(
        hFile,
        emptyStr.c_str(),
        emptyStr.length() * sizeof(wchar_t),
        &bytesWritten,
        NULL
    );

    // �������� �����
    CloseHandle(hFile); 

    // �������� ����� �����-�����
    HWND histCb = GetDlgItem(hCalcDlg, IDC_CBHISTORY);
    SendMessage(histCb, CB_RESETCONTENT, 0, 0);

    // ��������� ������ "History" �� ������ �����
    int index = SendMessage(histCb, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)L"--- History ---");
    SendMessage(histCb, CB_SETCURSEL, index, 0);

    historyList.clear(); // �������� ����� ������ �����
}

// ������� �������� �� ������ ������ ��������
BOOL termIsAdded(const std::wstring& term) {

    for (const auto& item : historyList) {
        if (item == term) { // ����� ��� ������� �� �����
            return TRUE; 
        }
    }
    return FALSE; // ����� �� �� ������� �� �����
}

// ������� ������� ������� ������������ ������� � ������ ��������
std::wstring selectedConst() {

    std::wstring result;

    // ��������� ������� �������� �������� � ������ ��������
    int selInd = SendDlgItemMessage(hSelDlg, IDC_CONSTLST, LB_GETCURSEL, 0, 0);
    if (selInd != LB_ERR) {
        wchar_t selConst[10]{ '\0' };
        SendDlgItemMessage(hSelDlg, IDC_CONSTLST, LB_GETTEXT, selInd, (LPARAM)selConst);

        // ���������� ���������� ������ � ����� result
        result = selConst;
    }

    return result;
}

// ³����� ��������� �������� ����������� ��������� ���� ��������
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {

    case WM_CREATE: // ������� ��䳿 ��������� ����
        break;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

            case ID_CALCULATOR: // ³������� ���� ������ "�����������"
            {
                DialogBox(hInst, MAKEINTRESOURCE(IDD_MAIN), NULL, CalculatorWindowProc);
                break;
            }
            case IDC_ABOUT: // ³������� ���� ������ "��� ��������"
            { 
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTDLG), NULL, AboutWindowProc);
                break;
            }
            case IDC_EXIT:
            {
                // ����� � �������� ��� ��������� ������ "Exit"
                PostQuitMessage(0);
                break;
            }
        }
        break;

    case WM_DESTROY:  // ������� ��䳿 �������� ����
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

// ³����� ��������� �������� ����������� ���� ������������
INT_PTR CALLBACK CalculatorWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
		
    switch (message) {

        // ����������� ����
        case WM_INITDIALOG:
        {
            hCalcDlg = hwnd;
            initCalcWindow();
            return TRUE;
        }

        // ������������� ����
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // ��������� ��������� ����� ����� �����������
            RECT grRect;
            HWND hGB = GetDlgItem(hwnd, IDC_SETTINGSGP);
            GetWindowRect(hGB, &grRect);
            ScreenToClient(hCalcDlg, (LPPOINT)&grRect.left);
            ScreenToClient(hCalcDlg, (LPPOINT)&grRect.right);

            // ������������ ����-����� �������� �� ���� ���������
            HBRUSH hBrush =
                CreateSolidBrush(IsDlgButtonChecked(hCalcDlg, IDC_ACTIVEGPCK) ? RGB(158, 158, 158) : RGB(45, 45, 45));

            FillRect(hdc, &grRect, hBrush);
            DeleteObject(hBrush);

            EndPaint(hwnd, &ps);
            return TRUE;
        }


        // ������������� �������� ������
        case WM_CTLCOLORSTATIC:
        {
            HDC  hdcSt  = (HDC)wParam;
            HWND hwndSt = (HWND)lParam;

            int stId = GetDlgCtrlID(hwndSt);
            SetTextColor(hdcSt, RGB(0, 0, 0)); // ������������ ���������� ������� ������

            // ������������ ������� ���� ��� ������ ����������
            if (stId == IDC_OUTPUTRES) {
                SetBkColor(hdcSt, RGB(0, 192, 0));
                return (INT_PTR)CreateSolidBrush(RGB(0, 192, 0));
            }
            
            // ������������ ������� ���� �������� �������� �� ��������� ��������� �����
            bool isActive = IsDlgButtonChecked(hCalcDlg, IDC_ACTIVEGPCK);

            SetBkColor(hdcSt, isActive ? RGB(158, 158, 158) : RGB(45, 45, 45));
            return (INT_PTR)CreateSolidBrush(isActive ? RGB(158, 158, 158) : RGB(45, 45, 45));
        }

        // ������������� ���� ����
        case WM_ERASEBKGND:
        {
            HDC hdc = (HDC)wParam;
            RECT rc;
            GetClientRect(hCalcDlg, &rc);

            // ���������� �볺������ ������ ������� �����
            HBRUSH hBrush = CreateSolidBrush(RGB(45, 45, 45)); 
            FillRect(hdc, &rc, hBrush);
            DeleteObject(hBrush);

            return (LRESULT)TRUE;
        }


        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                 
                // �������� ���������������� �������
                case IDC_SINBTN:
                case IDC_ASINBTN:
                case IDC_SINHBTN:
                case IDC_ASINHBTN:

                case IDC_COSBTN:
                case IDC_ACOSBTN:
                case IDC_COSHBTN:
                case IDC_ACOSHBTN:

                case IDC_TANBTN:
                case IDC_ATANBTN:
                case IDC_TANHBTN:
                case IDC_ATANHBTN:

                case IDC_COTBTN:
                case IDC_ACOTBTN:
                case IDC_COTHBTN:
                case IDC_ACOTHBTN:

                case IDC_SECBTN:
                case IDC_ASECBTN:
                case IDC_SECHBTN:
                case IDC_ASECHBTN:

                case IDC_CSCBTN:
                case IDC_ACSCBTN:
                case IDC_CSCHBTN:
                case IDC_ACSCHBTN:


                // �������� ��� ����� �������
                case IDC_POWERTWOBTN:
                case IDC_POWERTHREEBTN:
                case IDC_POWERYBTN:
                case IDC_TENPOWERBTN:

                case IDC_SQRTXBTN:
                case IDC_3SQRTXBTN:
                case IDC_4SQRTXBTN:
                case IDC_YSQRTXBTN:
                case IDC_ABSBTN:

                    
                // �������� ���������
                case IDC_LBRACKETBTN:
                case IDC_RBRACKETBTN:
                case IDC_PERCENTBTN:
                case IDC_EQUALBTN:
                case IDC_ADDBTN:
                case IDC_SUBTRACTBTN:
                case IDC_DIVIDEBTN:
                case IDC_MULTIPLYBTN:

                case IDC_DOTBTN:
                case IDC_NEGPOSBTN:
                case IDC_ERASEBTN:
                case IDC_CLEARBTN:

                // �������� ��������� ��������
                case IDC_ZEROBTN:
                case IDC_ONEBTN:
                case IDC_TWOBTN:
                case IDC_THREEBTN:
                case IDC_FOURBTN:
                case IDC_FIVEBTN:
                case IDC_SIXBTN:
                case IDC_SEVENBTN:
                case IDC_EIGHTBTN:
                case IDC_NINEBTN:

                {
                    btnClicked(LOWORD(wParam)); // ������� ����� ���������
                    break;
                }

                case IDC_CBHISTORY: // ������� ������ ������� ������
                {
                    if (HIWORD(wParam) == CBN_SELCHANGE) {
                        selChangeHistoryCombo();
                    }
                    break;
                }

                case IDC_LOGCB: // ������� ������ ���������
                {
                    if (HIWORD(wParam) == CBN_SELCHANGE) {
                        selChangeLogCombo();
                    }
                    break;
                }

                case IDC_INPUTMODELST: // ������� ������ �������
                {
                    if (HIWORD(wParam) == LBN_SELCHANGE) {
                        selChangeModeList();
                    }
                    break;
                }

                case IDC_ACTIVEGPCK: // ���� ����� ���� �����������
                {
                    changeSettingsMenuState();
                    break;
                }

                // ������� ��������� ����� ��� 16�� �������
                case IDC_16ABTN:
                case IDC_16BBTN:
                case IDC_16CBTN:
                case IDC_16DBTN:
                case IDC_16EBTN:
                case IDC_16FBTN:
                {
                    btn16Clicked(LOWORD(wParam));
                    break;
                }

                // ������� ������� ��������� ����� �����
                case IDC_CBUP:
                {
                    upListProc();
                    break;
                }

                // ������� ������� ��������� ����� ����
                case IDC_CBDOWN:
                {
                    downListProc();
                    break;
                }

                // ������� �������� ����� ����� �����
                case IDC_CLEARHISTBTN:
                {
                    clearFileHistory();
                    break;
                }

                // ������� ������� ������ "���� ���������"
                case IDC_SELCONSTBTN:
                {
                    // ��������� �������� ������ ��� ��������� �� ����������� ���������� ����
                    std::thread dialogThread([]() {
                        HWND hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_CONSTDLG), NULL, SelectWindowProc);
                        if (hDialog) {
                            ShowWindow(hDialog, SW_SHOW);

                            // ������� ���������� ���������� ����
                            MSG msg;
                            while (GetMessage(&msg, NULL, 0, 0)) {
                                if (!IsDialogMessage(hDialog, &msg)) {
                                    TranslateMessage(&msg);
                                    DispatchMessage(&msg);
                                }
                            }
                        }
                        });

                    dialogThread.detach(); // ³����������� ������ �� ��������� ������ ��������

                    break;
                }

            }
            break;

        // ������� ����������� �������� ����
        case WM_CLOSE:
        {
            upLoadDynamicLib();  // ������������ �������� ��������
            DestroyWindow(hwnd); // �������� ����
            return TRUE;
        }


        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
	return 0;
}

// ³����� ��������� �������� ����������� ���� ������ ���������
INT_PTR CALLBACK SelectWindowProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {

    case WM_INITDIALOG:
    {   
        hSelDlg = hwndDlg;
        HWND constLst = GetDlgItem(hwndDlg, IDC_CONSTLST);

        if (constLst)
        {
            // ��������� �������� � ������ ��������
            for (const auto& pair : constantsHash) {
                std::wstring constant = pair.first;
                SendMessage(constLst, LB_ADDSTRING, 0, (LPARAM)(constant.c_str()));
            }

            SendMessage(constLst, LB_SETCURSEL, 0, 0);
        }

        return TRUE;
    }
       
    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case IDOK: // ��������� ������ ��������� �� ��������� ������
        {
            mainInputStr += selectedConst();
            SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
            
            EndDialog(hwndDlg, IDOK);
        }
        break;

        case IDC_CONSTLST: // ��������� ���������� ��� ������ ���������
        {
            if (HIWORD(wParam) == LBN_SELCHANGE) {
                std::wstring lastConstInfo = L"This is " + selectedConst() + L" = " + std::to_wstring(constantsHash[selectedConst()]);
                SetDlgItemText(hSelDlg, IDC_CONSTINFO, lastConstInfo.c_str());
            }
            break;
        }
        break;

        case IDCANCEL: // �������� ���� ������
            EndDialog(hwndDlg, IDCANCEL);
            break;
        }
        break;

    case WM_CLOSE:
        EndDialog(hwndDlg, IDCANCEL);
        return TRUE;
    }

    return FALSE;
}

// ³����� ��������� �������� ����������� ���� "��� ��������"
INT_PTR CALLBACK AboutWindowProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {

    case WM_INITDIALOG:
    {
        // ��������� ���������� ���� � �������
        HWND iconSt = CreateWindow(
            L"STATIC",           
            L"",                 
            WS_VISIBLE | WS_CHILD | SS_ICON, 
            0,                   
            0,                   
            GetSystemMetrics(SM_CXICON),  
            GetSystemMetrics(SM_CYICON),
            hwndDlg,             
            NULL,               
            hInst,               
            NULL                 
        );

        // ������������ ������ � ����� �� ������������ �� � �������� ����
        HICON hIcon = (HICON)LoadImage(hInst, L"calculator_icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
        if (hIcon != NULL) {
            SendMessage(iconSt, STM_SETICON, (WPARAM)hIcon, 0);
        }

        // ������������ ������� ���������� ����
        SetWindowPos(iconSt, HWND_TOPMOST, 60, 40, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        // ������������ ������ � ������������ ����
        SetWindowText(GetDlgItem(hwndDlg, IDC_ABOUTINFOEDIT),
            L" Info:"
            L"This is software for performing complex\r\n mathematical and engineering calculations");

        return TRUE;
    }

    case WM_CLOSE: // �������� ���� ������
        EndDialog(hwndDlg, IDCANCEL);
        return TRUE;
    }

    return FALSE;
}