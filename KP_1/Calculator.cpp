#include "HCalculator.h"    // Підключення заголовного файлу калькулятора
#include "..\HLoadTime.h"   // Статична бібліотека простих математичних функцій
#include "..\HRunTime.h"    // Динамічна бібліотека складних математичних функцій
#include "resource.h"

////////////////////////////////////////
// Необхідні інструкції компілятору

// Підключення бібліотеки статично
#pragma comment (lib, "LTDLL.lib")

// Підключення стилів елементів версії Windows 6.0.0.0
#pragma comment (linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
////////////////////////////////////////

// Оголошення глобальних змінних

////////////////////////////////////////
// Вказівники на функції завантажені динамічно

HMODULE      hRunTime       = NULL; // Дескриптор динамічної бібліотеки

GETCONSTPTR  getConst       = NULL; // Вказівник на функцію отримання константи
DECTOBINPTR  decimalToBin   = NULL; // Вказівник на функцію конвертації десяткового числа в двійкове
DECTOOCTPTR  decimalToOct   = NULL; // Вказівник на функцію конвертації десяткового числа в вісімкове
DECTOHEXPTR  decimalToHex   = NULL; // Вказівник на функцію конвертації десяткового числа в шістнадцяткове
BINTODECPTR  binToDecimal   = NULL; // Вказівник на функцію конвертації двійкового числа в десяткове
OCTTODECPTR  octalToDecimal = NULL; // Вказівник на функцію конвертації вісімкового числа в десяткове
HEXTODECPTR  hexToDecimal   = NULL; // Вказівник на функцію конвертації шістнадцяткового числа в десяткове
DEGTORADPTR  degToRad       = NULL; // Вказівник на функцію переведення градусів в радіани
RADTODEGPTR  radToDeg       = NULL; // Вказівник на функцію переведення радіанів в градуси
FUNCOFNUMPTR funcOfNum      = NULL; // Вказівник на функцію обчислення тригонометричного виразу
////////////////////////////////////////


////////////////////////////////////////
std::wstring mainInputStr;								// Текст для зберігання введеного виразу
HWND hCalcDlg;											// Дескриптор діалогу калькулятор
HWND hSelDlg;											// Дескриптор діалогу обрати константу
std::wstring constRegStr;								// Текст для зберігання доступного списку констант
std::list<std::wstring> historyList;					// Список для збереження змісту файлу історії вводу
std::unordered_map<std::wstring, double> constantsHash; // Таблиця констант з динамічної бібліотеки
int currHistInd;										// Поточний вибраний елемент в списку історії вводу(0 - верхній)

enum  NumFormats {  // Види форматів
      NBINARY,		// 2
      NDECIMAL,		// 10
      NHEXADECIMAL,	// 16
      NOCTAL		// 8
};
NumFormats currNFormat; // Для зберігання теперішнього формату
////////////////////////////////////////

// Функція конвертує число з різних систем числення в десятковий формат
double xToDecimal(std::wstring num) {

    // Перевіряємо поточний формат числа
    switch (currNFormat) {

        // Якщо поточний формат - двійковий
    case NBINARY:
        // Викликаємо функцію binToDecimal для конвертації з двійкового у десятковий
        return binToDecimal(num);

        // Якщо поточний формат - восьмеричний
    case NOCTAL:
        // Викликаємо функцію octalToDecimal для конвертації з восьмеричного у десятковий
        return octalToDecimal(num);

        // Якщо поточний формат - шістнадцятковий
    case NHEXADECIMAL:
        // Викликаємо функцію hexToDecimal для конвертації з шістнадцяткового у десятковий
        return hexToDecimal(num);

        // У всіх інших випадках
    default:
        // Використовуємо стандартну функцію std::stod для конвертації рядка у десяткове число
        return std::stod(num);
    }
}

// Функція конвертує десяткове число у різні системи числення
std::wstring decimalToX(double num) {

    // Перевіряємо поточний формат чисел
    switch (currNFormat) {

        // Якщо поточний формат - двійковий
    case NBINARY:
        // Викликаємо функцію decimalToBin для конвертації у двійковий формат
        return decimalToBin(num);

        // Якщо поточний формат - восьмеричний
    case NOCTAL:
        // Викликаємо функцію decimalToOct для конвертації у восьмеричний формат
        return decimalToOct(num);

        // Якщо поточний формат - шістнадцятковий
    case NHEXADECIMAL:
        // Викликаємо функцію decimalToHex для конвертації у шістнадцятковий формат
        return decimalToHex(num);

        // У всіх інших випадках
    default:
        // Використовуємо стандартну функцію std::to_wstring для конвертації числа у рядок
        return std::to_wstring(num);
    }
}

// Функція встановлює іконку для вікна
void setWindowIcon(HWND hWnd, LPCTSTR path) {

    // Завантажуємо іконку з вказаного шляху
    HICON hIcon = (HICON)LoadImage(hInst, path, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);

    // Перевіряємо, чи іконка успішно завантажена
    if (hIcon != NULL) {
        // Якщо іконка завантажена успішно, встановлюємо її для вікна
        SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    }
}

// Функція відображає повідомлення про помилку у полі виводу
void showErr(const std::wstring& msg) {

    mainInputStr = L""; // Очищаємо введений вираз
    
    SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, msg.c_str()); // Відображаємо текст помилки
}

// Функція визначає пріоритет операції
int getPriority(const std::wstring& op) {

    // Перевіряємо, чи є операція математичною функцією
    if (std::regex_match(op, std::wregex(
        L"-?(sinh|sin|asinh|asin|cosh|cos|acosh|acos|tanh|tan|atanh|atan|cot|acot|sec|csc)"
        L"|-?log(\\d+)?|-?lg2|-?ln"
        L"|-?sqrt(\\d+)?"))) {

        // Якщо так, повертаємо найвищий пріоритет
        return 4;
    }
    // Якщо операція - піднесення до степеня, повертаємо пріоритет 3
    else if (op == L"^") {
        return 3;
    }
    // Якщо операція - залишок від ділення, множення або ділення, повертаємо пріоритет 2
    else if (op == L"%" || op == L"*" || op == L"/") {
        return 2;
    }
    // Якщо операція - додавання або віднімання, повертаємо пріоритет 1
    else if (op == L"+" || op == L"-") {
        return 1;
    }
    // В іншому випадку повертаємо пріоритет 0
    return 0;
}

// Функція порівнює пріоритети операцій
bool comparePriority(const std::wstring& op1, const std::wstring& op2) {
    return getPriority(op1) >= getPriority(op2);
}

// Функція отримує останнє введене число чи константу
std::wstring getLastNum() {
    // Регулярний вираз для пошуку останнього числа у введеному рядку
    std::wregex regex(L"-?\\d+(\\.\\d+)?" + constRegStr); 

    // Створення ітератора для перегляду всіх входжень регулярного виразу в рядок
    std::wsregex_iterator it(mainInputStr.begin(), mainInputStr.end(), regex);

    // Створення кінцевого ітератора для визначення кінця послідовності
    std::wsregex_iterator end;

    // Результуючий рядок для зберігання знайденого числа
    std::wstring result = L"";

    // Пошук останнього числа в рядку
    while (it != end) {
        result = it->str(); // Зберігання поточного числа у результат
        ++it; // Перехід до наступного входження
    }

    return result; // Повернення знайденого останнього числа
}

// Функція обробляє оператор та стек операндів
void calcOperator(std::stack<double>& operands, const std::wstring& op) {

    // Якщо стек операндів пустий або оператор - дужка
    if (operands.empty() || std::regex_match(op, std::wregex(L"-?\\("))) {

        // Інвертуємо результат у дужках, якщо потрібно
        if (!operands.empty() && op[0] == L'-') { 
            double invRes = operands.top() * (-1.0);
            operands.pop();
            operands.push(invRes);
        }

        return; // Пропускаємо, а потім видаляємо оператор
    }

    // Обробка оператора "|": взяття модуля числа
    if (op == L"|") {
        double operand = operands.top();
        operands.pop();

        operands.push(fabs(operand));
        return;
    }

    // Обробка логарифмічних операторів: log~?~, lg2, ln
    if (std::regex_match(op, std::wregex(L"-?log(\\d+)?|-?lg2|-?ln"))) {
        double operand = operands.top();
        operands.pop();

        operands.push(logOfNum(operand, op));
        return;
    }

    // Обробка тригонометричних функцій
    if (std::regex_match(op, std::wregex(
        L"-?(sinh|sin|asinh|asin|cosh|cos|acosh|acos|tanh|tan|atanh|atan|coth|cot|acoth|acot|sech|sec|asech|asec|csch|csc|acsch|acsc)"))) {

        double operand = operands.top();
        operands.pop();

        operands.push(funcOfNum(operand, op, IsDlgButtonChecked(hCalcDlg, IDC_RADIODG)));
        return;
    }

    // Обробка коренів виду: sqrt~?~
    if (std::regex_match(op, std::wregex(L"-?sqrt(\\d+)?"))) {

        double operand = operands.top();
        operands.pop();

        operands.push(rootOfNum(operand, op));
        return;
    }

    // Якщо у стеці менше 2 операндів, вийти
    if (operands.size() < 2) {
        return;
    }

    // Взяття двох операндів зі стеку
    double second = operands.top();
    operands.pop();

    double first = operands.top();
    operands.pop();

    // Обчислення результату відповідно до бінарного оператора
    if (op == L"+") {
        operands.push(addNums(first, second));
    }
    else if (op == L"*") {
        operands.push(multiplyNums(first, second));
    }
    else if (op == L"/") {
        operands.push(second ? divideNums(first, second) : -INFINITY); // Перевірка ділення на нуль
    }
    else if (op == L"%") {
        operands.push(percentOfNum(first, second));
    }
    else if (op == L"^") {
        operands.push(powerOfNum(first, second));
    }
}

// Функція обробляє додаткові клавіші 16го формату
void btn16Clicked(int id) {
    // Отримання тексту клавіші
    wchar_t btnTxt[2];
    GetDlgItemText(hCalcDlg, id, btnTxt, 2);
    std::wstring btnTxtStr(btnTxt);

    // Додавання тексту клавіші до рядка введення та відображення результату
    mainInputStr += btnTxtStr;
    SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
}

// Функція обробляє текстовий файл історії вводу
void histFileProc() {
    // Запис історії в файл
    if (!writeFileHistory()) {
        showErr(L"Cannot write data in file!");
    }
    // Читання історії з файлу
    if (!readFileHistory()) {
        showErr(L"Cannot read data from file!");
    }
}

// Функція відображає у полі введену інформацію
void btnClicked(int id) {

    // Отримання тексту кнопки
    wchar_t btnTxt[10];
    GetDlgItemText(hCalcDlg, id, btnTxt, 10);

    std::wstring btnTxtStr(btnTxt);

    // Очищення рядка введення та оновлення відображення
    if (btnTxtStr == L"AC") {
        mainInputStr = L"";
        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // Додавання цифри або дужки до рядка та оновлення відображення
    else if (std::regex_match(btnTxtStr, std::wregex(L"\\d")) || btnTxtStr == L"(" || btnTxtStr == L")") {

        mainInputStr += btnTxtStr;
        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // Додавання тригонометричної функції
    else if (std::regex_match(btnTxtStr,
    std::wregex(L"sinh|sin|asinh|asin|cosh|cos|acosh|acos|tanh|tan|atanh|atan|coth|cot|acoth|acot|sech|sec|asech|asec|csch|csc|acsch|acsc"))) {

        mainInputStr += btnTxtStr + L"(";
        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // Додавання кореня
    else if (std::regex_match(btnTxtStr, std::wregex(L"sqrt\\(x\\)|sqrt3\\(x\\)|sqrt4\\(x\\)"))) {

        btnTxtStr.pop_back(); btnTxtStr.pop_back();
        mainInputStr += btnTxtStr;
        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // Додавання десяткового логарифму
    else if (btnTxtStr == L"10^x") {
        mainInputStr += L"10^(";
        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // Додавання кореня Y степеня
    else if (btnTxtStr == L"sqrtY(x)") {
        mainInputStr += L"sqrt(";
        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // Для подальших дій mainInputStr не повинна бути порожньою
    else if (mainInputStr.empty()) {
        return;
    }

    // Заміна числа на його абсолютне значення
    else if (btnTxtStr == L"|x|") {
        // Перевірка чи останній символ - цифра
        if (!std::regex_search(std::wstring(1, mainInputStr.back()), std::wregex(L"\\d"))) {
            return; // Якщо останній символ не є цифрою, виходимо
        }

        // Знаходження позиції останнього числа у рядку
        size_t lastNumPos = mainInputStr.find_last_of(L" ()+-*/%^");
        // Формування абсолютного значення останнього числа
        std::wstring lastNabs = L"|(" + getLastNum() + L")|";

        // Заміна останнього числа на його абсолютне значення
        if (lastNumPos == std::wstring::npos) {
            mainInputStr = lastNabs; // Якщо останнє число єдине, замінюємо весь рядок
        }
        else if (mainInputStr[lastNumPos] == L'-') {
            mainInputStr.replace(lastNumPos, getLastNum().length(), lastNabs); // Якщо останнє число від'ємне
        }
        else {
            mainInputStr.replace(lastNumPos + 1, getLastNum().length(), lastNabs); // Якщо останнє число додатнє
        }

        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // Заміна числа на його обернене значення
    else if (btnTxtStr == L"+-") {
        // Отримання останнього числа з рядка
        std::wstring lastN = getLastNum();

        // Опрацювання констант
        if (std::regex_match(lastN, std::wregex(constRegStr))) {
            // Зміна знаку константи та оновлення рядка введення
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
        // Перевірка чи останній символ - цифра
        if (!std::regex_search(std::wstring(1, lastChar), std::wregex(L"\\d"))) {
            return; // Якщо останній символ не є цифрою, виходимо
        }

        // Опрацювання звичайного числа
        double lastNDbl = std::stod(lastN);
        lastNDbl *= -1; // Зміна знаку числа
        size_t lastNumPos = mainInputStr.find_last_of(L" ()|+-*/%^=");

        // Заміна останнього числа на змінене значення
        if (lastNumPos == std::wstring::npos) {
            mainInputStr = std::to_wstring(lastNDbl); // Якщо останнє число єдине, замінюємо весь рядок
        }
        else if (mainInputStr[lastNumPos] == L'-') {
            mainInputStr.replace(lastNumPos, lastN.length(), std::to_wstring(lastNDbl)); // Якщо останнє число від'ємне
        }
        else {
            mainInputStr.replace(lastNumPos + 1, lastN.length(), std::to_wstring(lastNDbl)); // Якщо останнє число додатнє
        }

        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    // Видалення останнього символу з рядка
    else if (btnTxtStr == L"<--")
    {
        mainInputStr.erase(mainInputStr.size() - 1);
        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

    else { // Додавання інших операторів

        // Обробка натискання отримання результату
        if (btnTxtStr == L"=") {
            
            // Розбиття рядка на токени та обчислення виразу
            std::vector<std::wstring> tokens = createStrTokens(mainInputStr);
            double result = strTokensProc(tokens);

            if (result == -INFINITY) { // Перевірка на помилку введення
                showErr(L"Input error!");
                return;
            }

            // Обробка історії вводу та конвертація результату в потрібний формат
            histFileProc();
            mainInputStr = decimalToX(result);
        }

        // Обробка натискання бінарного віднімання
        else if (btnTxtStr == L"-") {
            
            if (mainInputStr.back() == L'-')
            {
                mainInputStr.pop_back();
                SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
                return;
            }

            // Додаємо символи "+" та "-" як оператор додавання/віднімання для відмінності у- та бі- нарного операторів
            mainInputStr += L"+ -";
        }

        // Обробка натискання крапки
        else if (btnTxtStr == L".")
        {
            // Перевірка, чи можна додати крапку до останнього числа
            if (!std::regex_search(std::wstring(1, mainInputStr.back()), std::wregex(L"\\d"))) {
                return;
            }

            std::wstring lastN = getLastNum();
            if (lastN.find(L'.') != std::wstring::npos) {
                return;
            }

            mainInputStr += L'.'; // Якщо все добре, додаємо
        }

        // Обробка квадратного та кубічного степенів
        else if (btnTxtStr == L"x^2" || btnTxtStr == L"x^3") {
            btnTxtStr.erase(0, 1);
            mainInputStr += btnTxtStr;
        }

        // Обробка y степеня
        else if (btnTxtStr == L"x^y") {
            mainInputStr += L"^";
        }

        // Обробка всіх інших випадків
        else {
            mainInputStr += btnTxtStr;
        }

        SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
    }

}

// Функція обробляє токени за допомогою Reverse Polish Notation (RPN) алгоритму
double strTokensProc(std::vector<std::wstring> tokens) {
    // Стек для чисел (операндів)
    std::stack<double> operands;
    // Стек для операторів
    std::stack<std::wstring> operators;

    // Проходимося по кожному токену
    for (int i = 0; i < tokens.size(); ++i) {
        // Якщо токен є числом
        if (std::regex_match(tokens[i], std::wregex(L"-?\\d+[A-F]+?|-?\\d+\\.[A-F]|-?[A-F]+|-?\\d+(\\.\\d+)?"))) {
            // Перетворюємо число в десяткову систему та додаємо до стеку операндів
            operands.push(xToDecimal(tokens[i]));
        }
        // Якщо токен є константою
        else if (std::regex_match(tokens[i], std::wregex(constRegStr))) {
            // Отримуємо значення константи та додаємо до стеку операндів
            operands.push(getConst(tokens[i]));
        }
        // Якщо токен є оператором
        else if (std::regex_match(tokens[i], std::wregex(
            L"[+*/%^]"
            L"|-?(sinh|sin|asinh|asin|cosh|cos|acosh|acos|tanh|tan|atanh|atan|coth|cot|acoth|acot|sech|sec|asech|asec|csch|csc|acsch|acsc)"
            L"|-?log(\\d+)?|-?lg2|-?ln"
            L"|-?sqrt(\\d+)?"))) {
            // Обробка операторів
            while (!operators.empty() && comparePriority(operators.top(), tokens[i])) {
                // Викликаємо функцію для обчислення виразу зі стеку
                calcOperator(operands, operators.top());
                // Видаляємо оброблений оператор зі стеку операторів
                operators.pop();
            }
            // Додаємо поточний оператор до стеку операторів
            operators.push(tokens[i]);
        }
        // Якщо токен є відкриваючою дужкою
        else if (std::regex_match(tokens[i], std::wregex(L"-?\\("))) {
            // Додаємо відкриваючу дужку до стеку операторів
            operators.push(tokens[i]);
        }
        // Якщо токен є модулем
        else if (tokens[i] == L"|" && (i != 0) && tokens[i - 1] == L")") {
            // Додаємо та обробляємо його
            operators.push(tokens[i]);
            calcOperator(operands, operators.top());
            operators.pop(); // Видаляємо оброблений оператор
        }
        // Якщо токен є закриваючою дужкою
        else if (tokens[i] == L")") {
            // Обробка закриваючої дужки
            while (!operators.empty() && !std::regex_match(operators.top(), std::wregex(L"-?\\("))) {
                // Викликаємо функцію для обчислення виразу зі стеку
                calcOperator(operands, operators.top());
                // Видаляємо оброблений оператор зі стеку операторів
                operators.pop();
            }
            // Якщо стек операторів не порожній і верхній елемент є відкриваючою дужкою
            if (!operators.empty() && std::regex_match(operators.top(), std::wregex(L"-?\\("))) {
                // Якщо потрібно, інвертуємо результат у дужках
                if (operators.top()[0] == L'-') {
                    double invRes = operands.top() * (-1.0);
                    operands.pop();
                    operands.push(invRes);
                }
                // Видаляємо відкриваючу дужку зі стеку операторів
                operators.pop();
            }
        }
    }

    // Обробка решти операторів у стеці операторів
    while (!operators.empty()) {
        // Викликаємо функцію для обчислення виразу зі стеку
        calcOperator(operands, operators.top());
        // Видаляємо оброблений оператор зі стеку операторів
        operators.pop();
    }

    // Повертаємо результат (верхній елемент зі стеку операндів)
    return operands.empty() ? -INFINITY : operands.top();
}

// Функція заповнює регулярний вираз з таблиці констант
std::wstring fillConstRegEx() {
    // Результат - рядок, що містить всі константи з таблиці
    std::wstring result;

    // Проходимося по кожній константі у таблиці
    for (const auto& pair : constantsHash) {
        // Формуємо рядок, що містить поточну константу з можливим знаком мінуса
        std::wstring constant = L"|-?" + pair.first;
        // Додаємо поточну константу до результату
        result += constant;
    }
    // Повертаємо регулярний вираз, що містить всі константи
    return result;
}

// Функція розбиває введений рядок на токени
std::vector<std::wstring> createStrTokens(std::wstring str) {

    // Вибір регулярного виразу для чисел в залежності від активних налаштувань
    std::wstring numberRegStr = ((IsDlgButtonChecked(hCalcDlg, IDC_ACTIVEGPCK) && currNFormat == NHEXADECIMAL) ?
        L"-?\\d+[A-F]+?|-?\\d+\\.[A-F]|-?[A-F]+|-?\\d+(\\.\\d+)?" : L"-?\\d+(\\.\\d+)?");

    // Вибір регулярного виразу для тригонометричних функцій в залежності від активних налаштувань
    std::wstring trigRegStr = (IsDlgButtonChecked(hCalcDlg, IDC_ACTIVEGPCK) ?
        L"|-?(sinh|sin|asinh|asin|cosh|cos|acosh|acos|tanh|tan|atanh|atan|coth|cot|acoth|acot|sech|sec|asech|asec|csch|csc|acsch|acsc)" : L"");

    // Зібрання всіх можливих токенів у виразі
    std::wregex regex(numberRegStr +
        L"|-?\\("
        L"|[+*/%)^|]"
        L"|-?sqrt(\\d+)?"
        +
        constRegStr
        +
        L"|-?log(\\d+)?|-?lg2|-?ln"
        + trigRegStr);

    // Ітератор для пошуку токенів у виразі
    std::wsregex_iterator it(str.begin(), str.end(), regex);
    std::wsregex_iterator end;

    std::vector<std::wstring> tokens;

    // Додавання знайдених токенів до вектора
    while (it != end) {
        tokens.push_back(it->str());
        ++it;
    }

    // Повертаємо вектор токенів
    return tokens;
}

// Функція рухає вгору ітератор історії вводу
void upListProc() {

    // Перевірка чи можна піднятися вище
    if (currHistInd <= 0) {
        return;
    }
   
    currHistInd--;

    HWND histCb = GetDlgItem(hCalcDlg, IDC_CBHISTORY);
    SendMessage(histCb, CB_SETCURSEL, currHistInd, 0);
    selChangeHistoryCombo(); // Обробка вибору елемента
}

// Функція рухає вниз ітератор історії вводу
void downListProc() {

    // Перевірка чи можна опуститися нижче
    if (currHistInd >= historyList.size() - 1) {
        return;
    }

    currHistInd++;

    HWND histCb = GetDlgItem(hCalcDlg, IDC_CBHISTORY);
    SendMessage(histCb, CB_SETCURSEL, currHistInd, 0);
    selChangeHistoryCombo(); // Обробка вибору елемента
}

// Функція ініціалізує вікно калькулятора
void initCalcWindow() {

    currNFormat = NDECIMAL; // Встановлення початкового формату числа

    setWindowIcon (hCalcDlg, L"calculator_icon.ico"); // Встановлення значка вікна

    // Встановлення початкових станів елементів
    CheckDlgButton(hCalcDlg, IDC_RADIODG, BST_CHECKED);
    CheckDlgButton(hCalcDlg, IDC_ACTIVEGPCK, BST_UNCHECKED);

    // Читання історії з файлу
    if (!readFileHistory()) {
        showErr(L"History file is damaged!");
    }

    // Ініціалізація комбо-боксу для логарифмів
    HWND comboBox = GetDlgItem(hCalcDlg, IDC_LOGCB);
    if (comboBox)
    {
        SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)(L"log(x)"));
        SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)(L"ln(x)"));
        SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)(L"lg2(x)"));

        SendMessage(comboBox, CB_SETCURSEL, 2, 0);

    }

    // Ініціалізація комбо-боксу для історії
    HWND hComboBox = GetDlgItem(hCalcDlg, IDC_CBHISTORY);
    int index = SendMessage(GetDlgItem(hCalcDlg, IDC_CBHISTORY), CB_INSERTSTRING, (WPARAM)-1, (LPARAM)L"--- History ---");
    SendMessage(hComboBox, CB_SETCURSEL, index, 0);

    // Ініціалізація списку доступних форматів введення
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

// Функція обробляє вибір елемента з історії вводу 
void selChangeHistoryCombo() {

    HWND comboBox = GetDlgItem(hCalcDlg, IDC_CBHISTORY);
    wchar_t itemTxt[255]{ '\0' };

    // Отримання індексу обраного елемента
    currHistInd = SendMessage(comboBox, CB_GETCURSEL, 0, 0);
    if (currHistInd != CB_ERR) {
        SendMessage(comboBox, CB_GETLBTEXT, currHistInd, (LPARAM)itemTxt);
    }
    
    std::wstring itemTxtStr(itemTxt);
    if (itemTxtStr == L"--- History ---") {
        return;
    }

    // Оновлення головного рядка вводу з обраним текстом з історії
    mainInputStr = itemTxtStr;
    SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());

}

// Функція обробляє вибір елемента зі списку логарифмічних функцій
void selChangeLogCombo() {

    HWND comboBox = GetDlgItem(hCalcDlg, IDC_LOGCB);
    wchar_t itemTxt[10]{ '\0' };

    // Отримання індексу вибраного елемента
    int selInd = SendMessage(comboBox, CB_GETCURSEL, 0, 0);
    if (selInd != CB_ERR) {
        SendMessage(comboBox, CB_GETLBTEXT, selInd, (LPARAM)itemTxt);
    }

    // Видалення зайвих символів перед додаванням
    std::wstring itemTxtStr(itemTxt);
    itemTxtStr.pop_back(); itemTxtStr.pop_back();
    
    // Додавання тексту логарифму до головного рядка вводу
    mainInputStr += itemTxtStr;
    SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());

}

// Функція обробляє вибір елемента зі списку форматів
void selChangeModeList() {

    HWND modeLst = GetDlgItem(hCalcDlg, IDC_INPUTMODELST);

    // Отримання індексу обраного елемента
    int selInd = SendMessage(modeLst, LB_GETCURSEL, 0, 0);
    if (selInd == LB_ERR) {
        return;
    }

    // Встановлення поточного формату чисел відповідно до обраного елемента
    currNFormat = (NumFormats)selInd;

    // Показ або приховання клавіш для шістнадцяткового режиму в залежності від обраного формату
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16ABTN), currNFormat == NHEXADECIMAL ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16BBTN), currNFormat == NHEXADECIMAL ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16CBTN), currNFormat == NHEXADECIMAL ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16DBTN), currNFormat == NHEXADECIMAL ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16EBTN), currNFormat == NHEXADECIMAL ? SW_SHOWNORMAL : SW_HIDE);
    ShowWindow(GetDlgItem(hCalcDlg, IDC_16FBTN), currNFormat == NHEXADECIMAL ? SW_SHOWNORMAL : SW_HIDE);

}

// Функція обробляє зміну видимості додаткового меню
void changeSettingsMenuState() {

    BOOL isChecked = IsDlgButtonChecked(hCalcDlg, IDC_ACTIVEGPCK);

    // Додаткова панель активна чи ні
    if (isChecked) loadDynamicLib(); // Завантаження динамічної бібліотеки
    else upLoadDynamicLib(); // Вивантаження динамічної бібліотеки
    
    // Встановлення доступності елементів групи налаштувань
    HWND settingsGp = GetDlgItem(hCalcDlg, IDC_SETTINGSGP);
    EnableWindow(settingsGp, isChecked);

    // Відображення або приховування елементів групи
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

    // Оновлення вікна
    InvalidateRect(hCalcDlg, NULL, TRUE);
}

// Функція завантажує динамічну бібліотеку
void loadDynamicLib() {

    hRunTime = LoadLibrary(TEXT("RTDLL.dll"));
    if (!hRunTime) {
        showErr(L"Cannot load dynamic library!");
        return;
    }

    // Отримання адрес імпортних функцій з бібліотеки
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

    // Отримання адреси таблиці констант
    CONSTANTSPTR constantsPtr = (CONSTANTSPTR)GetProcAddress(hRunTime, "constants");
    constantsHash = *constantsPtr;

    // Заповнення регулярного виразу з таблиці констант
    constRegStr = fillConstRegEx();
}

// Функція вивантажує динамічну бібліотеку
void upLoadDynamicLib() {

    // Скидання вказівників на функції
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

    // Очищення таблиці констант та регулярного виразу
    constantsHash = {};
    constRegStr   = L"";

    // Звільнення ресурсів, пов'язаних з динамічною бібліотекою
    if (hRunTime) {
        FreeLibrary(hRunTime);
        hRunTime = NULL;
    }

    // Повернення до десяткового формату
    currNFormat = NDECIMAL;
    SendMessage(GetDlgItem(hCalcDlg, IDC_INPUTMODELST), LB_SETCURSEL, (int)currNFormat, 0);

    // Очищення введеного рядка та виведення на екран
    mainInputStr = L"";
    SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
}

// Функція записує останній вираз до файлу історії
BOOL writeFileHistory() {

    HANDLE hFile = CreateFile(
        L"history.txt",
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS, // Відкриваємо або створюємо, якщо файл не існує
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        return FALSE;
    }

    // Перехід до кінця файлу
    SetFilePointer(hFile, 0, NULL, FILE_END);

    DWORD bytesWritten;
    std::wstring term = mainInputStr + L"\n";

    // Перевірка, чи такий вираз вже існує у файлі історії
    if (termIsAdded(mainInputStr)) {
        CloseHandle(hFile);
        return FALSE;
    }

    // Запис виразу у файл
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

// Функція зчитує історію з файлу
BOOL readFileHistory() {

    historyList.clear();

    HANDLE hFile = CreateFile(
        L"history.txt",
        GENERIC_READ,
        FILE_SHARE_READ, // Дозвіл на читання з інших процесів
        NULL,
        OPEN_EXISTING,   // Відкрити тільки існуючий файл
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        return FALSE; // Помилка відкриття файлу
    }

    DWORD bytesRead;
    WCHAR buff[1024];
    std::wstring currLn;

    // Зчитування даних з файлу
    while (ReadFile(hFile, buff, 1024 * sizeof(WCHAR), &bytesRead, NULL) && bytesRead != 0) {

        for (DWORD i = 0; i < bytesRead / sizeof(WCHAR); ++i) {
            if (buff[i] == L'\n') {
                historyList.push_back(currLn); // Додавання поточного рядка до списку історії
                currLn.clear(); // Очищення поточного рядка
            }
            else {
                currLn += buff[i]; // Додавання символу до поточного рядка
            }
        }
    }  
    
    // Додавання останнього рядка, якщо він не був завершений символом нового рядка
    if (!currLn.empty()) {
        historyList.push_back(currLn);
    }

    // Завершення роботи з файлом
    CloseHandle(hFile);

    HWND histCb = GetDlgItem(hCalcDlg, IDC_CBHISTORY);
    SendMessage(histCb, CB_RESETCONTENT, 0, 0);

    // Додавання історії до комбо-боксу
    for (const auto& term : historyList) {
        SendMessage(histCb, CB_INSERTSTRING, 0, (LPARAM)(term.c_str()));
    }
   
    // Встановлення поточного індексу історії
    currHistInd = 0;
    SendMessage(histCb, CB_SETCURSEL, currHistInd, 0);
    
    return TRUE;
}

// Функція очищує історію вводу
void clearFileHistory() {

    HANDLE hFile = CreateFile(
        L"history.txt",
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS, // Створити файл, замінюючи існуючий (якщо такий є)
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        showErr(L"Cannot open file to clear!"); // Помилка відкриття файлу
        return;
    }

    DWORD bytesWritten;
    std::wstring emptyStr = L"\0";

    // Записуємо порожній рядок до файлу
    WriteFile(
        hFile,
        emptyStr.c_str(),
        emptyStr.length() * sizeof(wchar_t),
        &bytesWritten,
        NULL
    );

    // Закриття файлу
    CloseHandle(hFile); 

    // Очищення вмісту комбо-боксу
    HWND histCb = GetDlgItem(hCalcDlg, IDC_CBHISTORY);
    SendMessage(histCb, CB_RESETCONTENT, 0, 0);

    // Додавання пункту "History" до списку історії
    int index = SendMessage(histCb, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)L"--- History ---");
    SendMessage(histCb, CB_SETCURSEL, index, 0);

    historyList.clear(); // Очищення вмісту списку історії
}

// Функція перевіряє на спробу запису дублікату
BOOL termIsAdded(const std::wstring& term) {

    for (const auto& item : historyList) {
        if (item == term) { // Вираз вже доданий до історії
            return TRUE; 
        }
    }
    return FALSE; // Вираз ще не доданий до історії
}

// Функція повертає обраний користувачем елемент зі списку констант
std::wstring selectedConst() {

    std::wstring result;

    // Отримання індексу обраного елемента зі списку констант
    int selInd = SendDlgItemMessage(hSelDlg, IDC_CONSTLST, LB_GETCURSEL, 0, 0);
    if (selInd != LB_ERR) {
        wchar_t selConst[10]{ '\0' };
        SendDlgItemMessage(hSelDlg, IDC_CONSTLST, LB_GETTEXT, selInd, (LPARAM)selConst);

        // Збереження отриманого тексту у змінну result
        result = selConst;
    }

    return result;
}

// Віконна процедура обробляє повідомлення головного вікна програми
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {

    case WM_CREATE: // Обробка події створення вікна
        break;

    case WM_COMMAND:

        switch (LOWORD(wParam)) {

            case ID_CALCULATOR: // Відкриття вікна діалогу "Калькулятор"
            {
                DialogBox(hInst, MAKEINTRESOURCE(IDD_MAIN), NULL, CalculatorWindowProc);
                break;
            }
            case IDC_ABOUT: // Відкриття вікна діалогу "Про програму"
            { 
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTDLG), NULL, AboutWindowProc);
                break;
            }
            case IDC_EXIT:
            {
                // Вихід з програми при натисканні кнопки "Exit"
                PostQuitMessage(0);
                break;
            }
        }
        break;

    case WM_DESTROY:  // Обробка події закриття вікна
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

// Віконна процедура обробляє повідомлення вікна калькулятора
INT_PTR CALLBACK CalculatorWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
		
    switch (message) {

        // Ініціалізація вікна
        case WM_INITDIALOG:
        {
            hCalcDlg = hwnd;
            initCalcWindow();
            return TRUE;
        }

        // Перемалювання вікна
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Отримання координат рамки групи налаштувань
            RECT grRect;
            HWND hGB = GetDlgItem(hwnd, IDC_SETTINGSGP);
            GetWindowRect(hGB, &grRect);
            ScreenToClient(hCalcDlg, (LPPOINT)&grRect.left);
            ScreenToClient(hCalcDlg, (LPPOINT)&grRect.right);

            // Зафарбування груп-боксу відповідно до його активності
            HBRUSH hBrush =
                CreateSolidBrush(IsDlgButtonChecked(hCalcDlg, IDC_ACTIVEGPCK) ? RGB(158, 158, 158) : RGB(45, 45, 45));

            FillRect(hdc, &grRect, hBrush);
            DeleteObject(hBrush);

            EndPaint(hwnd, &ps);
            return TRUE;
        }


        // Перемалювання елементів виводу
        case WM_CTLCOLORSTATIC:
        {
            HDC  hdcSt  = (HDC)wParam;
            HWND hwndSt = (HWND)lParam;

            int stId = GetDlgCtrlID(hwndSt);
            SetTextColor(hdcSt, RGB(0, 0, 0)); // Встановлення загального кольору тексту

            // Встановлення кольору фону для виводу результату
            if (stId == IDC_OUTPUTRES) {
                SetBkColor(hdcSt, RGB(0, 192, 0));
                return (INT_PTR)CreateSolidBrush(RGB(0, 192, 0));
            }
            
            // Встановлення кольору фону елементів відповідно до активності додаткової панелі
            bool isActive = IsDlgButtonChecked(hCalcDlg, IDC_ACTIVEGPCK);

            SetBkColor(hdcSt, isActive ? RGB(158, 158, 158) : RGB(45, 45, 45));
            return (INT_PTR)CreateSolidBrush(isActive ? RGB(158, 158, 158) : RGB(45, 45, 45));
        }

        // Перемалювання фону вікна
        case WM_ERASEBKGND:
        {
            HDC hdc = (HDC)wParam;
            RECT rc;
            GetClientRect(hCalcDlg, &rc);

            // Заповнення клієнтської області фоновою кистю
            HBRUSH hBrush = CreateSolidBrush(RGB(45, 45, 45)); 
            FillRect(hdc, &rc, hBrush);
            DeleteObject(hBrush);

            return (LRESULT)TRUE;
        }


        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                 
                // Елементи тригонометричних функцій
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


                // Елементи всіх інших функцій
                case IDC_POWERTWOBTN:
                case IDC_POWERTHREEBTN:
                case IDC_POWERYBTN:
                case IDC_TENPOWERBTN:

                case IDC_SQRTXBTN:
                case IDC_3SQRTXBTN:
                case IDC_4SQRTXBTN:
                case IDC_YSQRTXBTN:
                case IDC_ABSBTN:

                    
                // Елементи операторів
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

                // Елементи цифрового введення
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
                    btnClicked(LOWORD(wParam)); // Обробка клавіш ітерфейсу
                    break;
                }

                case IDC_CBHISTORY: // Обробка списку минулих виразів
                {
                    if (HIWORD(wParam) == CBN_SELCHANGE) {
                        selChangeHistoryCombo();
                    }
                    break;
                }

                case IDC_LOGCB: // Обробка списку логарифмів
                {
                    if (HIWORD(wParam) == CBN_SELCHANGE) {
                        selChangeLogCombo();
                    }
                    break;
                }

                case IDC_INPUTMODELST: // Обробка списку форматів
                {
                    if (HIWORD(wParam) == LBN_SELCHANGE) {
                        selChangeModeList();
                    }
                    break;
                }

                case IDC_ACTIVEGPCK: // Зміна стану меню налаштувань
                {
                    changeSettingsMenuState();
                    break;
                }

                // Обробка додаткової панелі для 16го формату
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

                // Обробка зміщення ітератору історії вгору
                case IDC_CBUP:
                {
                    upListProc();
                    break;
                }

                // Обробка зміщення ітератору історії вниз
                case IDC_CBDOWN:
                {
                    downListProc();
                    break;
                }

                // Обробка очищення вмісту файлу історії
                case IDC_CLEARHISTBTN:
                {
                    clearFileHistory();
                    break;
                }

                // Обробка виклику діалогу "Вибір константи"
                case IDC_SELCONSTBTN:
                {
                    // Створення окремого потоку для створення та відображення діалогового вікна
                    std::thread dialogThread([]() {
                        HWND hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_CONSTDLG), NULL, SelectWindowProc);
                        if (hDialog) {
                            ShowWindow(hDialog, SW_SHOW);

                            // Обробка повідомлень діалогового вікна
                            MSG msg;
                            while (GetMessage(&msg, NULL, 0, 0)) {
                                if (!IsDialogMessage(hDialog, &msg)) {
                                    TranslateMessage(&msg);
                                    DispatchMessage(&msg);
                                }
                            }
                        }
                        });

                    dialogThread.detach(); // Відокремлення потоку від основного потоку програми

                    break;
                }

            }
            break;

        // Обробка повідомлення закриття вікна
        case WM_CLOSE:
        {
            upLoadDynamicLib();  // Вивантаження динамічної бібліотеки
            DestroyWindow(hwnd); // Знищення вікна
            return TRUE;
        }


        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
	return 0;
}

// Віконна процедура обробляє повідомлення вікна вибору константи
INT_PTR CALLBACK SelectWindowProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {

    case WM_INITDIALOG:
    {   
        hSelDlg = hwndDlg;
        HWND constLst = GetDlgItem(hwndDlg, IDC_CONSTLST);

        if (constLst)
        {
            // Додавання елементів у список констант
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

        case IDOK: // Додавання обраної константи до головного виразу
        {
            mainInputStr += selectedConst();
            SetDlgItemText(hCalcDlg, IDC_OUTPUTRES, mainInputStr.c_str());
            
            EndDialog(hwndDlg, IDOK);
        }
        break;

        case IDC_CONSTLST: // Оновлення інформації про обрану константу
        {
            if (HIWORD(wParam) == LBN_SELCHANGE) {
                std::wstring lastConstInfo = L"This is " + selectedConst() + L" = " + std::to_wstring(constantsHash[selectedConst()]);
                SetDlgItemText(hSelDlg, IDC_CONSTINFO, lastConstInfo.c_str());
            }
            break;
        }
        break;

        case IDCANCEL: // Закриття вікна діалогу
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

// Віконна процедура обробляє повідомлення вікна "Про програму"
INT_PTR CALLBACK AboutWindowProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {

    case WM_INITDIALOG:
    {
        // Створення статичного вікна з іконкою
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

        // Завантаження іконки з файлу та встановлення її в статичне вікно
        HICON hIcon = (HICON)LoadImage(hInst, L"calculator_icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
        if (hIcon != NULL) {
            SendMessage(iconSt, STM_SETICON, (WPARAM)hIcon, 0);
        }

        // Встановлення позиції статичного вікна
        SetWindowPos(iconSt, HWND_TOPMOST, 60, 40, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        // Встановлення тексту в інформаційне поле
        SetWindowText(GetDlgItem(hwndDlg, IDC_ABOUTINFOEDIT),
            L" Info:"
            L"This is software for performing complex\r\n mathematical and engineering calculations");

        return TRUE;
    }

    case WM_CLOSE: // Закриття вікна діалогу
        EndDialog(hwndDlg, IDCANCEL);
        return TRUE;
    }

    return FALSE;
}