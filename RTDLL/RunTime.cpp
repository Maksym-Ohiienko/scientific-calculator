#include "HRunTime.h"

std::unordered_map<std::wstring, double> constants
{ // ������� ��������
       {L"Pi",          3.141592653589},
       {L"EP",          2.718281828459},
       {L"RS",          1.4513692348},
       {L"Fi",          1.618033988749},
       {L"Tau",         6.283185307178},
       {L"K",           2.6854520010},
       {L"Light",       299792458.0},
       {L"Euler",       0.5772156649}, 
       {L"Lambda",      1.30357},
       {L"Bern",        0.2801694990},
       {L"Gauss",       0.8346268414},
       {L"Avogadro",    6.02214076e+23},
       {L"Sound",       343.2}
};


double CALLBACK getConst(std::wstring& token) {

    if (token[0] == L'-') { // ���������, ���� �������
        token.erase(0, 1);
        return -constants[token];
    }

    return constants[token]; // ��������� �������� ��������� � �������
}
 
std::wstring CALLBACK decimalToBin(double number) {

    // ��������, �� ����� ��'����
    bool isNegative = (number < 0);
    if (isNegative) {
        number = -number;
    }

    // ��������� ����� �� ���� �� ������� �������
    int intPart = static_cast<int>(number);
    double fractPart = number - intPart;

    // ����������� �����-����������
    std::wstring result = L"";

    // ���� ����� �������, ��������� ����� ������������� �����
    if (intPart == 0 && fractPart == 0) return std::to_wstring(number);

    // ���� ���� ������� �������, ������ "0" �� ����������
    if (intPart == 0) result += L"0";

    // ������������ ���� ������� �� ������� �������������
    while (intPart > 0) {
        result = std::to_wstring(intPart % 2) + result;
        intPart /= 2;
    }

    if (fractPart > 0) {
        result += L'.';

        // ������������ ������� ������� �� ������� �������������
        for (int i = 0; i < 6; ++i) {
            fractPart *= 2;
            int digit = static_cast<int>(fractPart);
            result += std::to_wstring(digit);
            fractPart -= digit;
        }
    }

    // ��������� ����� "-" �� ����������, ���� ����� ��'����
    if (isNegative) {
        result = L'-' + result;
    }

    // ���������� ��������� ������������� ����� � ������ �����
    return result;
}

std::wstring CALLBACK decimalToOct(double number) {

    // ��������, �� ����� ��'����
    bool isNegative = (number < 0);
    if (isNegative) {
        number = -number;
    }

    // ��������� ����� �� ���� �� ������� �������
    int intPart = static_cast<int>(number);
    double fractPart = number - intPart;

    // ����������� �����-����������
    std::wstring result = L"";

    // ���� ����� �������, ��������� ����� ������������� �����
    if (intPart == 0 && fractPart == 0) return std::to_wstring(number);

    // ���� ���� ������� �������, ������ "0" �� ����������
    if (intPart == 0) result += L"0";

    // ������������ ���� ������� �� ����������� �������������
    while (intPart > 0) {
        result = std::to_wstring(intPart % 8) + result;
        intPart /= 8;
    }

    if (fractPart > 0) {
        result += L'.';

        // ������������ ������� ������� �� ����������� �������������
        for (int i = 0; i < 6; ++i) {
            fractPart *= 8;
            int digit = static_cast<int>(fractPart);
            result += std::to_wstring(digit);
            fractPart -= digit;
        }
    }

    // ��������� ����� "-" �� ����������, ���� ����� ��'����
    if (isNegative) {
        result = L'-' + result;
    }

    // ���������� ������������� ������������� ����� � ������ �����
    return result;
}

std::wstring CALLBACK decimalToHex(double number) {

    // ��������, �� ����� ��'����
    bool isNegative = (number < 0);
    if (isNegative) {
        number = -number;
    }

    // ��������� ����� �� ���� �� ������� �������
    int intPart = static_cast<int>(number);
    double fractPart = number - intPart;

    // ����������� �����-����������
    std::wstring result = L"";

    // ���� ����� �������, ��������� ����� ������������� �����
    if (intPart == 0 && fractPart == 0) return std::to_wstring(number);

    // ���� ���� ������� �������, ������ "0" �� ����������
    if (intPart == 0) result += L"0";

    // ������������ ���� ������� �� �������������� �������������
    while (intPart > 0) {
        int remainder = intPart % 16;
        if (remainder < 10) {
            result += L'0' + remainder;
        }
        else {
            result += L'A' + (remainder - 10);
        }
        intPart /= 16;
    }

    // ��������� �����, ������� ����� ����������� � ����
    std::reverse(result.begin(), result.end());

    if (fractPart > 0) {
        result += L'.';

        // ������������ ������� ������� �� �������������� �������������
        for (int i = 0; i < 6; ++i) {
            fractPart *= 16;
            int digit = static_cast<int>(fractPart);
            if (digit < 10) {
                result += L'0' + digit;
            }
            else {
                result += L'A' + (digit - 10);
            }
            fractPart -= digit;
        }
    }

    // ��������� ����� "-" �� ����������, ���� ����� ��'����
    if (isNegative) {
        result = L'-' + result;
    }

    // ���������� ���������������� ������������� ����� � ������ �����
    return result;
}


double CALLBACK binToDecimal(std::wstring code) {

    double result = 0.0;
    double currPower = 1.0;

    bool isNegative = (code[0] == L'-');
    size_t startIndex = isNegative ? 1 : 0;

    size_t dotPos = code.find(L'.'); // ������ ������� ������ � ����

    if (dotPos != std::wstring::npos) {
        // ���� ��������, �������� ����� �� �� �������: �� �� ���� ������ 
        std::wstring beforeDot = code.substr(startIndex, dotPos);
        std::wstring afterDot = code.substr(dotPos + 1);

        // ��� ������� �� ������ ���������� ����������
        for (int i = beforeDot.size() - 1; i >= (int)startIndex; --i) {
            if (beforeDot[i] == L'1') {
                result += currPower;
            }
            currPower *= 2.0;
        }

        double fracPart = 0.0;
        double fracPower = 0.5; // ������ ������ ����� ��� ������� �������

        for (size_t i = 0; i < afterDot.size(); ++i) {
            if (afterDot[i] == L'1') {
                fracPart += fracPower;
            }
            fracPower /= 2; // �������� ������ ����� ��� ���������� ���
        }

        result += fracPart;
    }
    else {
        // ���� ������ �� ��������, ������ ������� �� �� �������� ������� �����
        for (int i = code.size() - 1; i >= (int)startIndex; --i) {
            if (code[i] == L'1') {
                result += currPower;
            }
            currPower *= 2.0;
        }
    }

    return isNegative ? -result : result;
}

double CALLBACK octalToDecimal(std::wstring number) {

    double result = 0.0;
    double currPower = 1.0;

    bool isNegative = (number[0] == L'-');
    size_t startIndex = isNegative ? 1 : 0;

    // ����������, �� � ������� ������� � ����
    size_t dotPos = number.find(L'.');
    if (dotPos != std::wstring::npos) {
        std::wstring beforeDot = number.substr(startIndex, dotPos);
        std::wstring afterDot = number.substr(dotPos + 1);

        // ������������ ���� ������� �����
        for (int i = beforeDot.size() - 1; i >= (int)startIndex; --i) {
            int digit = beforeDot[i] - L'0';
            if (digit >= 0 && digit <= 7) {
                result += digit * currPower;
            }
            else {
                return -INFINITY;
            }
            currPower *= 8.0;
        }

        currPower = 0.125; // 1/8 - �������� � ������� �������
        // ������������ ������� ������� �����
        for (size_t i = 0; i < afterDot.size(); ++i) {
            int digit = afterDot[i] - L'0';
            if (digit >= 0 && digit <= 7) {
                result += digit * currPower;
            }
            else {
                return -INFINITY;
            }
            currPower /= 8.0;
        }
    }
    else {
        // ���� ���� ������� �������, ������ ������������ ���� ������� �����
        for (int i = number.size() - 1; i >= (int)startIndex; --i) {
            int digit = number[i] - L'0';
            if (digit >= 0 && digit <= 7) {
                result += digit * currPower;
            }
            else {
                return -INFINITY;
            }
            currPower *= 8.0;
        }
    }

    return isNegative ? -result : result;
}

double CALLBACK hexToDecimal(std::wstring number) {

    double result = 0.0;
    double currPower = 1.0;

    bool isNegative = (number[0] == L'-');
    size_t startIndex = isNegative ? 1 : 0;

    // ����������, �� � ������� ������� � ����
    size_t dotPos = number.find(L'.');
    if (dotPos != std::wstring::npos) {
        std::wstring beforeDot = number.substr(startIndex, dotPos);
        std::wstring afterDot = number.substr(dotPos + 1);

        // ������������ ���� ������� �����
        for (int i = beforeDot.size() - 1; i >= (int)startIndex; --i) {
            int decimalValue = 0;
            if (isdigit(number[i])) {
                decimalValue = number[i] - L'0';
            }
            else {
                decimalValue = toupper(number[i]) - L'A' + 10;
            }
            result += decimalValue * currPower;
            currPower *= 16.0;
        }

        currPower = 0.0625; // 1/16 - �������� � ������� �������
        // ������������ ������� ������� �����
        for (size_t i = 0; i < afterDot.size(); ++i) {
            int decimalValue = 0;
            if (isdigit(afterDot[i])) {
                decimalValue = afterDot[i] - L'0';
            }
            else {
                decimalValue = toupper(afterDot[i]) - L'A' + 10;
            }
            result += decimalValue * currPower;
            currPower /= 16.0;
        }
    }
    else {
        // ���� ���� ������� �������, ������ ������������ ���� ������� �����
        for (int i = number.size() - 1; i >= (int)startIndex; --i) {
            int decimalValue = 0;
            if (isdigit(number[i])) {
                decimalValue = number[i] - L'0';
            }
            else {
                decimalValue = toupper(number[i]) - L'A' + 10;
            }
            result += decimalValue * currPower;
            currPower *= 16.0;
        }
    }

    return isNegative ? -result : result;
}


double CALLBACK degToRad(double deg) {
    return deg * constants[L"Pi"] / 180.0; // ������� ����������� � ������� � ������
}

double CALLBACK radToDeg(double rad) {
    return rad * 180.0 / constants[L"Pi"]; // ������� ����������� � ����� � �������
}

double CALLBACK funcOfNum(double opnd, const std::wstring& op, int radioDgState) {

    double result = -INFINITY; // ��� ���������� �������

    // ������� ������
    if (std::regex_match(op, std::wregex(L"-?sin"))) {
        result = radioDgState == BST_CHECKED ? sin(degToRad(opnd)) : sin(opnd);
    }
    else if (std::regex_match(op, std::wregex(L"-?asin"))) {
        result = radioDgState == BST_CHECKED ? asin(degToRad(opnd)) : asin(opnd);
    }
    else if (std::regex_match(op, std::wregex(L"-?sinh"))) {
        result = radioDgState == BST_CHECKED ? sinh(degToRad(opnd)) : sinh(opnd);
    }
    else if (std::regex_match(op, std::wregex(L"-?asinh"))) {
        result = radioDgState == BST_CHECKED ? asinh(degToRad(opnd)) : asinh(opnd);
    }

    // ������� ��������
    else if (std::regex_match(op, std::wregex(L"-?cos"))) {
        result = radioDgState == BST_CHECKED ? cos(degToRad(opnd)) : cos(opnd);
    }
    else if (std::regex_match(op, std::wregex(L"-?acos"))) {
        result = radioDgState == BST_CHECKED ? acos(degToRad(opnd)) : acos(opnd);
    }
    else if (std::regex_match(op, std::wregex(L"-?cosh"))) {
        result = radioDgState == BST_CHECKED ? cosh(degToRad(opnd)) : cosh(opnd);
    }
    else if (std::regex_match(op, std::wregex(L"-?acosh"))) {
        result = radioDgState == BST_CHECKED ? acosh(degToRad(opnd)) : acosh(opnd);
    }

    // ������� ��������
    else if (std::regex_match(op, std::wregex(L"-?tan"))) {
        result = radioDgState == BST_CHECKED ? tan(degToRad(opnd)) : tan(opnd);
    }
    else if (std::regex_match(op, std::wregex(L"-?atan"))) {
        result = radioDgState == BST_CHECKED ? atan(degToRad(opnd)) : atan(opnd);
    }
    else if (std::regex_match(op, std::wregex(L"-?tanh"))) {
        result = radioDgState == BST_CHECKED ? tanh(degToRad(opnd)) : tanh(opnd);
    }
    else if (std::regex_match(op, std::wregex(L"-?atanh"))) {
        result = radioDgState == BST_CHECKED ? atanh(degToRad(opnd)) : atanh(opnd);
    }

    // ������� ����������
    else if (std::regex_match(op, std::wregex(L"-?cot"))) {
        result = radioDgState == BST_CHECKED ? (1.0 / tan(degToRad(opnd))) : (1.0 / tan(opnd));
    }
    else if (std::regex_match(op, std::wregex(L"-?acot"))) {
        result = radioDgState == BST_CHECKED ? (constants[L"Pi"] / 2 - atan(degToRad(opnd))) : (constants[L"Pi"] / 2 - atan(opnd));
    }
    else if (std::regex_match(op, std::wregex(L"-?coth"))) {
        result = radioDgState == BST_CHECKED ? (1.0 / tanh(degToRad(opnd))) : (1.0 / tanh(opnd));
    }
    else if (std::regex_match(op, std::wregex(L"-?acoth"))) {
        result = radioDgState == BST_CHECKED ? (0.5 * log((opnd + 1) / (opnd - 1))) : (0.5 * log((opnd + 1) / (opnd - 1)));
    }

    // ������� ������� (1/cos)
    else if (std::regex_match(op, std::wregex(L"-?sec"))) {
        result = radioDgState == BST_CHECKED ? (1.0 / cos(degToRad(opnd))) : (1.0 / cos(opnd));
    }
    else if (std::regex_match(op, std::wregex(L"-?asec"))) {
        result = radioDgState == BST_CHECKED ? radToDeg(acos(1.0 / opnd)) : acos(1.0 / opnd);
    }
    else if (std::regex_match(op, std::wregex(L"-?sech"))) {
        result = radioDgState == BST_CHECKED ? (1.0 / cosh(degToRad(opnd))) : (1.0 / cosh(opnd));
    }
    else if (std::regex_match(op, std::wregex(L"-?asech"))) {
        result = radioDgState == BST_CHECKED ? radToDeg(acosh(1.0 / opnd)) : acosh(1.0 / opnd);
    }

    // ������� ��������� (1/sin)
    else if (std::regex_match(op, std::wregex(L"-?csc"))) {
        result = radioDgState == BST_CHECKED ? (1.0 / sin(degToRad(opnd))) : (1.0 / sin(opnd));
    }
    else if (std::regex_match(op, std::wregex(L"-?acsc"))) {
        result = radioDgState == BST_CHECKED ? radToDeg(asin(1.0 / opnd)) : asin(1.0 / opnd);
    }
    else if (std::regex_match(op, std::wregex(L"-?csch"))) {
        result = radioDgState == BST_CHECKED ? (1.0 / sinh(degToRad(opnd))) : (1.0 / sinh(opnd));
    }
    else if (std::regex_match(op, std::wregex(L"-?acsch"))) {
        result = radioDgState == BST_CHECKED ? radToDeg(asinh(1.0 / opnd)) : asinh(1.0 / opnd);
    }

    // ��������� ���������
    return std::isnan(result) ? -INFINITY : (op[0] == L'-' ? -result : result);
}

