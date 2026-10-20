#include <functional>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
#include "BinaryTree.h"
#include "Set.h"
#include "Types.h"

int ReadInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        int value;
        if (std::cin >> value) return value;
        if (std::cin.eof()) std::exit(0);
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Нужно целое число.\n";
    }
}

std::string ReadWord(const std::string& prompt) {
    std::cout << prompt;
    std::string word;
    if (!(std::cin >> word)) std::exit(0);
    return word;
}

std::string ReadLine(const std::string& prompt) {
    std::cout << prompt;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string line;
    std::getline(std::cin, line);
    return line;
}

template <typename T>
T ReadValue(const std::string& hint) {
    while (true) {
        std::string word = ReadWord("Значение " + hint + ": ");
        std::istringstream in(word);
        T value;
        if (in >> value && (in >> std::ws).eof()) return value;
        std::cout << "Не удалось разобрать, попробуй ещё раз.\n";
    }
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
    std::cout << "Дерево (ЛКП): [" << tree.ToString("LKP") << "]\n"
              << "Размер: " << tree.GetSize() << ", высота: " << tree.GetHeight()
              << ", AVL: " << (tree.IsBalanced() ? "да" : "НЕТ") << "\n";
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
        int choice = ReadInt("> ");
        try {
            if (choice == 0) {
                return;
            } else if (choice == 1) {
                tree.Insert(ReadValue<T>(info.hint));
            } else if (choice == 2) {
                tree.Remove(ReadValue<T>(info.hint));
            } else if (choice == 3) {
                bool found = tree.Contains(ReadValue<T>(info.hint));
                std::cout << (found ? "Найден" : "Не найден") << "\n";
            } else if (choice == 4) {
                int n = ReadInt("Сколько элементов: ");
                if (n < 0) throw std::invalid_argument("N must be non-negative");
                int added = 0;
                for (int attempts = 0; added < n && attempts < n * 10; attempts++) {
                    T value = info.random(rng);
                    if (!tree.Contains(value)) {
                        tree.Insert(value);
                        added++;
                    }
                }
                std::cout << "Добавлено: " << added << "\n";
            } else if (choice == 5) {
                tree.Clear();
            } else if (choice == 6) {
                std::string order = ReadWord("Порядок (KLP, KPL, LKP, LPK, PLK, PKL): ");
                std::cout << tree.ToString(order) << "\n";
            } else if (choice == 7) {
                std::string format = ReadWord("Формат, например {K}(L)[P]: ");
                std::cout << tree.ToFormattedString(format) << "\n";
            } else if (choice == 8) {
                std::string line = ReadLine("Элементы через пробел (" + info.hint + "): ");
                tree = BinaryTree<T>::FromString(line);
            } else if (choice == 9) {
                std::cout << "Результат: [" << tree.Map(info.map).ToString("LKP") << "]\n";
            } else if (choice == 10) {
                std::cout << "Результат: [" << tree.Where(info.where).ToString("LKP") << "]\n";
            } else if (choice == 11) {
                std::cout << "Результат: " << tree.Reduce(info.reduce, info.reduceStart) << "\n";
            } else if (choice == 12) {
                BinaryTree<T> sub = tree.ExtractSubtree(ReadValue<T>(info.hint));
                std::cout << "Поддерево: " << sub.ToFormattedString("{K}(L)[P]") << "\n";
            } else if (choice == 13) {
                std::string line = ReadLine("Поддерево в порядке KLP: ");
                BinaryTree<T> sub = BinaryTree<T>::FromString(line);
                std::cout << (tree.ContainsSubtree(sub) ? "Входит" : "Не входит") << "\n";
            } else {
                std::cout << "Нет такого пункта.\n";
            }
        } catch (const std::invalid_argument& e) {
            std::cout << "Ошибка: " << e.what() << "\n";
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
        int choice = ReadInt("> ");
        if (choice == 0) {
            return;
        } else if (choice == 1) {
            a.Add(ReadInt("Число: "));
        } else if (choice == 2) {
            b.Add(ReadInt("Число: "));
        } else if (choice == 3) {
            a = Set<int>();
            b = Set<int>();
            for (int i = 0; i < 8; i++) {
                a.Add(rng() % 15);
                b.Add(rng() % 15);
            }
        } else if (choice == 4) {
            std::cout << "A + B = " << (a + b) << "\n";
        } else if (choice == 5) {
            std::cout << "A * B = " << (a * b) << "\n";
        } else if (choice == 6) {
            std::cout << "A - B = " << (a - b) << "\n";
        } else if (choice == 7) {
            std::cout << "A <= B: " << ((a <= b) ? "да" : "нет") << "\n";
        } else if (choice == 8) {
            std::cout << "A == B: " << ((a == b) ? "да" : "нет") << "\n";
        } else {
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
                int length = 3 + rng() % 4;
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
    static const std::string lastNames[] = {"Ivanov", "Petrov", "Sidorov", "Smirnov", "Kuznetsov"};
    static const std::string firstNames[] = {"Ivan", "Petr", "Anna", "Maria", "Oleg"};
    return {"Student", "(id;Фамилия;Имя;год, например 7;Ivanov;Ivan;2005)",
            [](std::mt19937& rng) {
                return Student(static_cast<int>(rng() % 10000), lastNames[rng() % 5],
                               firstNames[rng() % 5], 2000 + static_cast<int>(rng() % 8));
            },
            "год рождения + 1", [](const Student& s) {
                Student copy = s;
                copy.birthYear++;
                return copy;
            },
            "родился после 2004", [](const Student& s) { return s.birthYear > 2004; },
            "самый молодой",
            [](const Student& s, const Student& acc) { return s.birthYear > acc.birthYear ? s : acc; },
            Student(0, "-", "-", 0)};
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    while (true) {
        std::cout << "\n===== ЛР-1: AVL-дерево =====\n"
                     "1. int   2. double   3. complex   4. string   5. Student\n"
                     "6. Множества (Set<int>)\n"
                     "0. Выход\n";
        int choice = ReadInt("> ");
        if (choice == 0) break;
        else if (choice == 1) TreeMenu(IntInfo());
        else if (choice == 2) TreeMenu(DoubleInfo());
        else if (choice == 3) TreeMenu(ComplexInfo());
        else if (choice == 4) TreeMenu(StringInfo());
        else if (choice == 5) TreeMenu(StudentInfo());
        else if (choice == 6) SetMenu();
        else std::cout << "Нет такого пункта.\n";
    }
    return 0;
}
