#include <exception>
#include <functional>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
#include "BinaryTree.h"
#include "Set.h"
#include "TreeFormatter.h"
#include "Types.h"
#include "exceptions.hpp"

int ReadInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        int value;
        if (std::cin >> value) return value;
        if (std::cin.eof()) throw EndOfStreamException("ввод закончился");
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Нужно целое число.\n";
    }
}

std::string ReadWord(const std::string& prompt) {
    std::cout << prompt;
    std::string word;
    if (!(std::cin >> word)) throw EndOfStreamException("ввод закончился");
    return word;
}

std::string ReadLine(const std::string& prompt) {
    std::cout << prompt;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string line;
    if (!std::getline(std::cin, line)) throw EndOfStreamException("ввод закончился");
    return line;
}

template <typename T>
T ReadValue(const std::string& hint) {
    while (true) {
        std::istringstream in(ReadWord("Значение " + hint + ": "));
        T value;
        if (in >> value && (in >> std::ws).eof()) return value;
        std::cout << "Не удалось разобрать, попробуй ещё раз.\n";
    }
}

bool ParseOrder(const std::string& text, Order& order) {
    const char* names[] = {"KLP", "KPL", "LKP", "LPK", "PLK", "PKL"};
    const Order orders[] = {Order::KLP, Order::KPL, Order::LKP, Order::LPK, Order::PLK, Order::PKL};
    for (int i = 0; i < 6; i++) {
        if (text == names[i]) {
            order = orders[i];
            return true;
        }
    }
    return false;
}

template <typename T>
struct TypeInfo {
    std::string name;
    std::string hint;
    std::function<T(std::mt19937&)> random;
    std::string mapName;
    std::function<T(const T&)> map;
    std::string whereName;
    std::function<bool(const T&)> where;
    std::string reduceName;
    std::function<T(const T&, const T&)> reduce;
    T reduceStart;
};

template <typename T>
void PrintTree(const BinaryTree<T>& tree) {
    std::cout << "Дерево (ЛКП): [" << tree.ToString(Order::LKP) << "]\n"
              << "Размер: " << tree.GetSize() << ", высота: " << tree.GetHeight()
              << ", AVL: " << (tree.IsBalanced() ? "да" : "НЕТ") << "\n";
}

template <typename T>
void FillRandom(BinaryTree<T>& tree, const TypeInfo<T>& info, std::mt19937& rng) {
    int n = ReadInt("Сколько элементов: ");
    if (n < 0) {
        std::cout << "Количество не может быть отрицательным\n";
        return;
    }
    int added = 0;
    for (int attempts = 0; added < n && attempts < n * 10; attempts++) {
        if (tree.Insert(info.random(rng))) added++;
    }
    std::cout << "Добавлено: " << added << "\n";
}

template <typename T>
bool ReadTree(const std::string& prompt, BinaryTree<T>& tree) {
    try {
        tree.LoadFromString(ReadLine(prompt));
        return true;
    } catch (const InvalidArgument& e) {
        std::cout << "Ошибка: " << e.what() << "\n";
        return false;
    }
}

template <typename T>
void TreeMenu(const TypeInfo<T>& info) {
    BinaryTree<T> tree;
    std::mt19937 rng(std::random_device{}());

    while (true) {
        std::cout << "\n===== Дерево: " << info.name << " =====\n";
        PrintTree(tree);
        std::cout << " 1. Вставить          2. Удалить          3. Найти\n"
                     " 4. Заполнить случайно (N штук)          5. Очистить\n"
                     " 6. Обход (KLP, LKP, ...)                7. Строка форматирования\n"
                     " 8. Загрузить из строки                  9. map: " << info.mapName << "\n"
                     "10. where: " << info.whereName << "\n"
                     "11. reduce: " << info.reduceName << "\n"
                     "12. Извлечь поддерево                   13. Проверить вхождение поддерева\n"
                     " 0. Назад\n";
        switch (ReadInt("> ")) {
            case 0:
                return;
            case 1:
                if (!tree.Insert(ReadValue<T>(info.hint))) std::cout << "Такой элемент уже есть\n";
                break;
            case 2:
                if (!tree.Remove(ReadValue<T>(info.hint))) std::cout << "Элемент не найден\n";
                break;
            case 3:
                std::cout << (tree.Contains(ReadValue<T>(info.hint)) ? "Найден" : "Не найден") << "\n";
                break;
            case 4:
                FillRandom(tree, info, rng);
                break;
            case 5:
                tree.Clear();
                break;
            case 6: {
                Order order;
                if (ParseOrder(ReadWord("Порядок (KLP, KPL, LKP, LPK, PLK, PKL): "), order)) {
                    tree.Traverse(order, [](const T& value) { std::cout << value << ' '; });
                    std::cout << "\n";
                } else
                    std::cout << "Неизвестный порядок обхода\n";
                break;
            }
            case 7:
                std::cout << TreeFormatter<T>(ReadWord("Формат, например {K}(L)[P]: ")).Format(tree) << "\n";
                break;
            case 8:
                ReadTree("Элементы через пробел " + info.hint + ": ", tree);
                break;
            case 9:
                std::cout << "Результат: [" << tree.Map(info.map).ToString(Order::LKP) << "]\n";
                break;
            case 10:
                std::cout << "Результат: [" << tree.Where(info.where).ToString(Order::LKP) << "]\n";
                break;
            case 11:
                std::cout << "Результат: " << tree.Reduce(info.reduce, info.reduceStart) << "\n";
                break;
            case 12: {
                BinaryTree<T> sub = tree.ExtractSubtree(ReadValue<T>(info.hint));
                if (sub.IsEmpty()) std::cout << "Элемент не найден\n";
                else std::cout << "Поддерево: " << TreeFormatter<T>("{K}(L)[P]").Format(sub) << "\n";
                break;
            }
            case 13: {
                BinaryTree<T> sub;
                if (ReadTree("Поддерево в порядке KLP: ", sub))
                    std::cout << (tree.ContainsSubtree(sub) ? "Входит" : "Не входит") << "\n";
                break;
            }
            default:
                std::cout << "Нет такого пункта.\n";
        }
    }
}

void SetMenu() {
    Set<int> a;
    Set<int> b;
    std::mt19937 rng(std::random_device{}());

    while (true) {
        std::cout << "\n===== Множества целых чисел =====\n"
                  << "A = " << a << "\nB = " << b << "\n"
                  << " 1. Добавить в A     2. Добавить в B     3. Заполнить оба случайно\n"
                  << " 4. A + B (объединение)    5. A * B (пересечение)    6. A - B (разность)\n"
                  << " 7. A <= B (подмножество)  8. A == B (равенство)\n"
                  << " 0. Назад\n";
        switch (ReadInt("> ")) {
            case 0:
                return;
            case 1:
                a.Add(ReadInt("Число: "));
                break;
            case 2:
                b.Add(ReadInt("Число: "));
                break;
            case 3:
                a = Set<int>();
                b = Set<int>();
                for (int i = 0; i < 8; i++) {
                    a.Add(static_cast<int>(rng() % 15));
                    b.Add(static_cast<int>(rng() % 15));
                }
                break;
            case 4:
                std::cout << "A + B = " << (a + b) << "\n";
                break;
            case 5:
                std::cout << "A * B = " << (a * b) << "\n";
                break;
            case 6:
                std::cout << "A - B = " << (a - b) << "\n";
                break;
            case 7:
                std::cout << "A <= B: " << (a <= b ? "да" : "нет") << "\n";
                break;
            case 8:
                std::cout << "A == B: " << (a == b ? "да" : "нет") << "\n";
                break;
            default:
                std::cout << "Нет такого пункта.\n";
        }
    }
}

TypeInfo<int> IntInfo() {
    return {"int", "(целое, например 42)",
            [](std::mt19937& rng) { return static_cast<int>(rng() % 1000); },
            "x * 2", [](const int& x) { return x * 2; },
            "x чётное", [](const int& x) { return x % 2 == 0; },
            "сумма", [](const int& x, const int& acc) { return x + acc; }, 0};
}

TypeInfo<double> DoubleInfo() {
    return {"double", "(вещественное, например 3.14)",
            [](std::mt19937& rng) { return static_cast<int>(rng() % 10000) / 100.0; },
            "x / 2", [](const double& x) { return x / 2; },
            "x > 50", [](const double& x) { return x > 50; },
            "сумма", [](const double& x, const double& acc) { return x + acc; }, 0.0};
}

TypeInfo<Complex> ComplexInfo() {
    return {"complex", "(re,im, например 1.5,-2)",
            [](std::mt19937& rng) {
                return Complex(static_cast<int>(rng() % 21) - 10, static_cast<int>(rng() % 21) - 10);
            },
            "z * i (поворот на 90°)", [](const Complex& z) { return z * Complex(0, 1); },
            "|z| < 5", [](const Complex& z) { return z.Abs() < 5; },
            "сумма", [](const Complex& z, const Complex& acc) { return z + acc; }, Complex(0, 0)};
}

TypeInfo<std::string> StringInfo() {
    return {"string", "(слово без пробелов)",
            [](std::mt19937& rng) {
                std::string word;
                int length = 3 + static_cast<int>(rng() % 4);
                for (int i = 0; i < length; i++) word += static_cast<char>('a' + rng() % 26);
                return word;
            },
            "слово + \"!\"", [](const std::string& s) { return s + "!"; },
            "длина > 4", [](const std::string& s) { return s.size() > 4; },
            "склеить через -",
            [](const std::string& s, const std::string& acc) { return acc.empty() ? s : acc + "-" + s; },
            std::string()};
}

TypeInfo<Student> StudentInfo() {
    return {"Student", "(id;Фамилия;Имя;год, например 7;Ivanov;Ivan;2005)",
            [](std::mt19937& rng) {
                const char* lastNames[] = {"Ivanov", "Petrov", "Sidorov", "Smirnov", "Kuznetsov"};
                const char* firstNames[] = {"Ivan", "Petr", "Anna", "Maria", "Oleg"};
                return Student(static_cast<int>(rng() % 10000), lastNames[rng() % 5],
                               firstNames[rng() % 5], 2000 + static_cast<int>(rng() % 8));
            },
            "год рождения + 1",
            [](const Student& s) {
                Student copy = s;
                copy.birthYear++;
                return copy;
            },
            "родился после 2004", [](const Student& s) { return s.birthYear > 2004; },
            "самый молодой",
            [](const Student& s, const Student& acc) { return s.birthYear > acc.birthYear ? s : acc; },
            Student(0, "-", "-", 0)};
}

void MainMenu() {
    while (true) {
        std::cout << "\n===== ЛР-1: AVL-дерево =====\n"
                     "1. int   2. double   3. complex   4. string   5. Student\n"
                     "6. Множества (Set<int>)\n"
                     "0. Выход\n";
        switch (ReadInt("> ")) {
            case 0: return;
            case 1: TreeMenu(IntInfo()); break;
            case 2: TreeMenu(DoubleInfo()); break;
            case 3: TreeMenu(ComplexInfo()); break;
            case 4: TreeMenu(StringInfo()); break;
            case 5: TreeMenu(StudentInfo()); break;
            case 6: SetMenu(); break;
            default: std::cout << "Нет такого пункта.\n";
        }
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    try {
        MainMenu();
    } catch (const EndOfStreamException&) {
        std::cout << "\nВвод закончился, выход.\n";
    } catch (const IException& e) {
        std::cout << "\nОшибка: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cout << "\nОшибка: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
