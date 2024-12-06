#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <stack>
#include <vector>

using namespace std;

unordered_map<string, int> ravno = { /**/{"?L", 1}, /**/{"L?", 2}, {"I(", 3}, {"(E", 4}, {"E)", 5},
                                    {"E+", 6}, {"+T", 7}, {"E-", 8}, {"-T", 9}, {"T*", 10},
                                    {"*M", 11}, {"T/", 12}, {"/M", 13}, /**/{"LS", 14}};

unordered_map<string, int> menishe = { {"?I", 1}, {"(T", 2}, {"(M", 3}, {"((", 4}, {"(I", 5},
                                      {"(C", 6}, {"(E", 7}, {"+M", 8}, {"+(", 9}, {"+I", 10},
                                      {"+C", 11}, {"-M", 12}, {"-(", 13}, {"-I", 14}, {"-C", 15},
                                      {"*(", 16}, {"*I", 17}, {"*C", 18}, {"/(", 19}, {"/I", 20},
                                      {"/C", 21}, {"+T", 22}, {"-T", 23}, /**/{"?L", 24} , /**/{"?S", 25}, /**/{"LI", 26}};

unordered_map<string, int> bolshe = { {")?", 1}, {"T)", 2}, {"M)", 3}, {"))", 4}, {"I)", 5},
                                     {"C)", 6}, /**/{"SI", 7}, {"T+", 8}, {"M+", 9}, {")+", 10},
                                     {"I+", 11}, {"C+", 12}, {"T-", 13}, {"M-", 14}, {")-", 15},
                                     {"I-", 16}, {"C-", 17}, {"M*", 18}, {")*", 19}, {"I*", 20},
                                     {"C*", 21}, {"M/", 22}, {")/", 23}, {"I/", 24}, {"C/", 25}, /**/{")I", 26}, /**/{"S?", 27} };

unordered_map<string, string> highrules = {
    {"I(E)", "S"}
};

unordered_map<string, string> normalrules = {
    {"E+T", "E"}, {"E-T", "E"}, {"T*M", "T"}, {"T/M", "T"},{"(E)", "M"}
};

unordered_map<string, string> low2rules = {
    {"LS", "S"}
};

unordered_map<string, string> lowrules = {
    {"C", "M"}, {"M", "T"}, {"T", "E"}, {"I", "M"},{"S", "L"},
};

stack<char> parseStack;   
string inputstroke;
size_t pos = 0;

void skipWhitespace() {
    while (isspace(inputstroke[pos])) pos++;
}

char peek() {
    skipWhitespace();
    return inputstroke[pos];
}

char get() {
    skipWhitespace();
    return inputstroke[pos++];
}

void error(const string& msg) {
    cerr << "Error: " << msg << " at position " << pos << endl;
    exit(1);
}

bool isLetter(char ch) {
    return (ch >= 'a' && ch <= 'z') || ch == '_';
}

bool isDigit(char ch) {
    return ch >= '0' && ch <= '9';
}

string parseC() {
    string value = "";
    if (!isDigit(peek())) error("Expected digit in C");
    while (isDigit(peek())) {
        value += get();
    }
    return "C";
}

void parseK(string& id) {
    while (isLetter(peek()) || isDigit(peek())) {
        if (isLetter(peek())) {
            id += get();
        }
        else if (isDigit(peek())) {
            id += get();
        }
    }
}

string parseI() {
    string id = "";
    if (isLetter(peek())) {
        id += get();
        parseK(id);
    }
    else {
        error("Expected identifier in I");
    }
    return "I";
}

string parseExpression() {
    string result = "";

    while (pos < inputstroke.length()) {
        char current = peek();

        if (isLetter(current)) {
            result += parseI();
        }
        else if (isDigit(current)) {
            result += parseC();
        }
        else {
            result += get();
        }
    }
    
    return result;
}

void logAction(ofstream& outFile, const string& action, const stack<char>& stk, char currentInput) {
    outFile << "Стек: ";
    stack<char> tempStack = stk;
    vector<char> elements;
    while (!tempStack.empty()) {
        elements.push_back(tempStack.top());
        tempStack.pop();
    }
    for (auto it = elements.rbegin(); it != elements.rend(); ++it) {
        outFile << *it;
    }

    outFile << " | Y: " << currentInput << " | " << action << endl;
}



void shift(char symbol, ofstream& outFile) {
    logAction(outFile, "Сдвиг", parseStack, symbol);
    parseStack.push(symbol);
    logAction(outFile, "", parseStack, '-');
}



void reduce(ofstream& outFile) {
    string rightPart;
    vector<string> poppedSymbols; // Для хранения извлечённых символов
    string temp;

    // Извлекаем до 4 символов из стека
    for (int i = 0; i < 4 && !parseStack.empty(); i++) {
        temp = parseStack.top();
        parseStack.pop();
        poppedSymbols.push_back(temp);
        rightPart = temp + rightPart;
    }

    // Проверяем правила длиной 4 символа
    if (highrules.find(rightPart) != highrules.end()) {
        string leftPart = highrules[rightPart];
        parseStack.push(leftPart[0]);
        logAction(outFile, "Свертка: " + rightPart + " -> " + leftPart, parseStack, '-');
        return;
    }

    // Если правило не найдено, возвращаем последний символ обратно в стек и проверяем 3-символьные правила
    while (poppedSymbols.size() > 3) {
        parseStack.push(poppedSymbols.back()[0]);
        poppedSymbols.pop_back();
        rightPart = rightPart.substr(1); // Уменьшаем правую часть
    }

    if (normalrules.find(rightPart) != normalrules.end()) {
        string leftPart = normalrules[rightPart];
        parseStack.push(leftPart[0]);
        logAction(outFile, "Свертка: " + rightPart + " -> " + leftPart, parseStack, '-');
        return;
    }

    while (poppedSymbols.size() > 2) {
        parseStack.push(poppedSymbols.back()[0]);
        poppedSymbols.pop_back();
        rightPart = rightPart.substr(1); // Уменьшаем правую часть
    }

    if (low2rules.find(rightPart) != low2rules.end()) {
        string leftPart = low2rules[rightPart];
        parseStack.push(leftPart[0]);
        logAction(outFile, "Свертка: " + rightPart + " -> " + leftPart, parseStack, '-');
        return;
    }

    // Если правило не найдено, возвращаем ещё один символ и проверяем 1-символьные правила
    while (poppedSymbols.size() > 1) {
        parseStack.push(poppedSymbols.back()[0]);
        poppedSymbols.pop_back();
        rightPart = rightPart.substr(1); // Уменьшаем правую часть
    }

    rightPart = poppedSymbols.back();
    if (lowrules.find(rightPart) != lowrules.end()) {
        string leftPart = lowrules[rightPart];
        parseStack.push(leftPart[0]);
        logAction(outFile, "Свертка: " + rightPart + " -> " + leftPart, parseStack, '-');
        return;
    }

    // Если правило так и не найдено, выводим ошибку
    cerr << "Ошибка: не найдено правило для свёртки " << rightPart << endl;
    exit(1);
}


void shiftReduce(const string& lexemes, ofstream& outFile) {
    parseStack.push('?'); 
    size_t pos = 0;

    while (pos < lexemes.size()) {
        char topStack = parseStack.top(); 
        char currentInput = lexemes[pos]; 
        
        string pair = string(1, topStack) + string(1, currentInput);
        if (parseStack.size() == 2 && parseStack.top() == 'L' && lexemes[pos] == '?') {
            logAction(outFile, "Цепочка принята!", parseStack, currentInput);
            return;
        }
        if (menishe.find(pair) != menishe.end()) {
            shift(currentInput, outFile);
            pos++;
        }
        else if (bolshe.find(pair) != bolshe.end()) {
            reduce(outFile);
        }
        else if (ravno.find(pair) != ravno.end()) {
            shift(currentInput, outFile);
            pos++;
        }
        else {
            cerr << "Ошибка: неизвестная пара символов " << pair << endl;
            exit(1);
        }
    }
    
    cerr << "Ошибка: цепочка не завершена корректно." << endl;
    exit(1);
}

int main() {
    setlocale(LC_ALL, "russian");
    ifstream inputFile("program.c");
    if (!inputFile) {
        cerr << "Ошибка: невозможно открыть файл input.с" << endl;
        return 1;
    }

    ofstream outputFile("output.c");
    if (!outputFile) {
        cerr << "Ошибка: невозможно открыть файл output.с" << endl;
        return 1;
    }

    string line;
    int lineNumber = 1;
    while (getline(inputFile, line)) {
        inputstroke = line;
        pos = 0;
        parseStack = stack<char>();

        string lexemes = parseExpression();
        lexemes += "?"; 

        outputFile << "Строка " << lineNumber << ": " << line << endl;
        outputFile << "Лексемы: " << lexemes << endl;

        shiftReduce(lexemes, outputFile);

        outputFile << "---- Конец обработки строки " << lineNumber << " ----" << endl << endl;
        lineNumber++;
    }

    inputFile.close();
    outputFile.close();

    cout << "Обработка завершена. Результаты записаны в файл output.c." << endl;
    return 0;
}
