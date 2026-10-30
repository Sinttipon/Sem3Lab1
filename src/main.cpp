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

int ReadInt(const std::string &prompt)
{
    while (true)
    {
        std::cout << prompt;
        int value;
        if (std::cin >> value)
            return value;
        if (std::cin.eof())
            throw EndOfStreamException("ввод закончился");
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Нужно целое число.\n";
    }
}

std::string ReadWord(const std::string &prompt)
{
    std::cout << prompt;
    std::string word;
    if (!(std::cin >> word))
        throw EndOfStreamException("ввод закончился");
    return word;
}

std::string ReadLine(const std::string &prompt)
{
    std::cout << prompt;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string line;
    if (!std::getline(std::cin, line))
        throw EndOfStreamException("ввод закончился");
    return line;
}

template <typename T>
T ReadValue(const std::string &hint)
{
    while (true)
    {
        std::istringstream in(ReadWord("Значение " + hint + ": "));
        T value;
        if (in >> value && (in >> std::ws).eof())
            return value;
        std::cout << "Не удалось разобрать, попробуй ещё раз.\n";
    }
}

template <typename TKey, typename TValue>
struct TypeInfo
{
    std::string name;
    std::string hint;
    std::function<TKey(const TValue &)> getKey;
    std::function<TValue(std::mt19937 &)> random;
    std::string mapName;
    std::function<TValue(const TValue &)> map;
    std::string whereName;
    std::function<bool(const TValue &)> where;
    std::string reduceName;
    std::function<TValue(const TValue &, const TValue &)> reduce;
    TValue reduceStart;
};

template <typename TKey, typename TValue>
void PrintTree(const BinaryTree<TKey, TValue> &tree)
{
    std::cout << "Размер: " << tree.GetSize() << ", высота: " << tree.GetHeight() << "\n";
}

template <typename TKey, typename TValue>
void FillRandom(BinaryTree<TKey, TValue> &tree, const TypeInfo<TKey, TValue> &info, std::mt19937 &rng)
{
    int n = ReadInt("Сколько элементов: ");
    if (n < 0)
    {
        std::cout << "Количество не может быть отрицательным\n";
        return;
    }
    int added = 0;
    for (int attempts = 0; added < n && attempts < n * 10; attempts++)
    {
        TValue v = info.random(rng);
        TKey k = info.getKey(v);
        try
        {
            tree.Insert(k, v);
            added++;
        }
        catch (...)
        {
        }
    }
    std::cout << "Добавлено: " << added << "\n";
}

template <typename TKey, typename TValue>
void TreeMenu(const TypeInfo<TKey, TValue> &info)
{
    BinaryTree<TKey, TValue> tree;
    std::mt19937 rng(std::random_device{}());

    while (true)
    {
        std::cout << "\n===== Дерево: " << info.name << " =====\n";
        PrintTree(tree);
        std::cout << " 1. Вставить          2. Удалить          3. Найти\n"
                  << " 4. Заполнить случайно  5. Очистить\n"
                  << " 6. Обход (KLP, LKP, ...)  7. Строка форматирования\n"
                  << " 8. Загрузить из строки  9. map: " << info.mapName << "\n"
                  << "10. where: " << info.whereName << "\n"
                  << "11. reduce: " << info.reduceName << "\n"
                  << " 0. Назад\n";

        switch (ReadInt("> "))
        {
        case 0:
            return;
        case 1:
        {
            TValue v = ReadValue<TValue>(info.hint);
            TKey k = info.getKey(v);
            try
            {
                tree.Insert(k, v);
                std::cout << "Добавлено\n";
            }
            catch (...)
            {
                std::cout << "Такой элемент уже есть\n";
            }
            break;
        }
        case 2:
        {
            TValue v = ReadValue<TValue>(info.hint);
            TKey k = info.getKey(v);
            try
            {
                tree.Remove(k);
                std::cout << "Удалено\n";
            }
            catch (...)
            {
                std::cout << "Элемент не найден\n";
            }
            break;
        }
        case 3:
        {
            TValue v = ReadValue<TValue>(info.hint);
            TKey k = info.getKey(v);
            std::cout << (tree.Search(k) ? "Найден" : "Не найден") << "\n";
            break;
        }
        case 4:
            FillRandom(tree, info, rng);
            break;
        case 5:
            tree = BinaryTree<TKey, TValue>();
            break;
        case 6:
        {
            std::string order = ReadWord("Порядок (KLP, KPL, LKP, LPK, PLK, PKL): ");
            auto printAction = [](const TValue &value)
            { std::cout << value << ' '; };
            if (order == "KLP")
                tree.Traverse(KLPrec<TKey, TValue>, printAction);
            else if (order == "KPL")
                tree.Traverse(KPLrec<TKey, TValue>, printAction);
            else if (order == "LKP")
                tree.Traverse(LKPrec<TKey, TValue>, printAction);
            else if (order == "LPK")
                tree.Traverse(LPKrec<TKey, TValue>, printAction);
            else if (order == "PLK")
                tree.Traverse(PLKrec<TKey, TValue>, printAction);
            else if (order == "PKL")
                tree.Traverse(PKLrec<TKey, TValue>, printAction);
            else
                std::cout << "Неизвестный порядок\n";
            std::cout << "\n";
            break;
        }
        case 7:
            std::cout << TreeFormatter<TKey, TValue>(ReadWord("Формат, например {K}(V)[P]: ")).Format(tree) << "\n";
            break;
        case 8:
        {
            std::string text = ReadLine("Элементы через пробел " + info.hint + ": ");
            std::istringstream in(text);
            BinaryTree<TKey, TValue> loaded;
            bool ok = true;
            while (in >> std::ws && !in.eof())
            {
                TValue value;
                if (!(in >> value))
                {
                    std::cout << "Ошибка ввода\n";
                    ok = false;
                    break;
                }
                TKey key = info.getKey(value);
                try
                {
                    loaded.Insert(key, value);
                }
                catch (...)
                {
                }
            }
            if (ok)
                tree = std::move(loaded);
            break;
        }
        case 9:
        {
            BinaryTree<TKey, TValue> result;
            tree.Traverse(LKPrec<TKey, TValue>, [&](const TValue &value)
                          {
                    TValue mapped = info.map(value);
                    TKey key = info.getKey(mapped);
                    try { result.Insert(key, mapped); } catch (...) {} });
            std::cout << "Результат: ";
            result.Traverse(LKPrec<TKey, TValue>, [](const TValue &v)
                            { std::cout << v << ' '; });
            std::cout << "\n";
            break;
        }
        case 10:
        {
            BinaryTree<TKey, TValue> result;
            tree.Traverse(LKPrec<TKey, TValue>, [&](const TValue &value)
                          {
                    if (info.where(value)) {
                        TKey key = info.getKey(value);
                        try { result.Insert(key, value); } catch (...) {}
                    } });
            std::cout << "Результат: ";
            result.Traverse(LKPrec<TKey, TValue>, [](const TValue &v)
                            { std::cout << v << ' '; });
            std::cout << "\n";
            break;
        }
        case 11:
        {
            TValue acc = info.reduceStart;
            tree.Traverse(LKPrec<TKey, TValue>, [&](const TValue &value)
                          { acc = info.reduce(value, acc); });
            std::cout << "Результат: " << acc << "\n";
            break;
        }
        default:
            std::cout << "Нет такого пункта.\n";
        }
    }
}

void SetMenu()
{
    Set<int> a, b;
    std::mt19937 rng(std::random_device{}());

    while (true)
    {
        std::cout << "\n===== Множества целых чисел =====\n"
                  << "A = " << a << "\nB = " << b << "\n"
                  << " 1. Добавить в A     2. Добавить в B     3. Заполнить оба случайно\n"
                  << " 4. A + B (объединение)    5. A * B (пересечение)    6. A - B (разность)\n"
                  << " 7. A <= B (подмножество)  8. A == B (равенство)\n"
                  << " 0. Назад\n";

        switch (ReadInt("> "))
        {
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
            for (int i = 0; i < 8; i++)
            {
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

TypeInfo<int, int> IntInfo()
{
    return {"int", "(целое, например 42)",
            [](const int &v)
            { return v; },
            [](std::mt19937 &rng)
            { return static_cast<int>(rng() % 1000); },
            "x * 2", [](const int &x)
            { return x * 2; },
            "x чётное", [](const int &x)
            { return x % 2 == 0; },
            "сумма", [](const int &x, const int &acc)
            { return x + acc; }, 0};
}

TypeInfo<double, double> DoubleInfo()
{
    return {"double", "(вещественное, например 3.14)",
            [](const double &v)
            { return v; },
            [](std::mt19937 &rng)
            { return static_cast<int>(rng() % 10000) / 100.0; },
            "x / 2", [](const double &x)
            { return x / 2; },
            "x > 50", [](const double &x)
            { return x > 50; },
            "сумма", [](const double &x, const double &acc)
            { return x + acc; }, 0.0};
}

TypeInfo<Complex, Complex> ComplexInfo()
{
    return {"complex", "(re,im, например 1.5,-2)",
            [](const Complex &v)
            { return v; },
            [](std::mt19937 &rng)
            { return Complex(static_cast<int>(rng() % 21) - 10, static_cast<int>(rng() % 21) - 10); },
            "z * i", [](const Complex &z)
            { return z * Complex(0, 1); },
            "|z| < 5", [](const Complex &z)
            { return z.Abs() < 5; },
            "сумма", [](const Complex &z, const Complex &acc)
            { return z + acc; }, Complex(0, 0)};
}

TypeInfo<std::string, std::string> StringInfo()
{
    return {"string", "(слово без пробелов)",
            [](const std::string &v)
            { return v; },
            [](std::mt19937 &rng)
            {
                std::string word;
                int length = 3 + static_cast<int>(rng() % 4);
                for (int i = 0; i < length; i++)
                    word += static_cast<char>('a' + rng() % 26);
                return word;
            },
            "слово + \"!\"", [](const std::string &s)
            { return s + "!"; },
            "длина > 4", [](const std::string &s)
            { return s.size() > 4; },
            "склеить через -",
            [](const std::string &s, const std::string &acc)
            { return acc.empty() ? s : acc + "-" + s; },
            std::string()};
}

TypeInfo<int, Student> StudentInfo()
{
    return {"Student", "(id;Фамилия;Имя;год, например 7;Ivanov;Ivan;2005)",
            [](const Student &s)
            { return s.id; },
            [](std::mt19937 &rng)
            {
                const char *lastNames[] = {"Ivanov", "Petrov", "Sidorov", "Smirnov", "Kuznetsov"};
                const char *firstNames[] = {"Ivan", "Petr", "Anna", "Maria", "Oleg"};
                return Student(static_cast<int>(rng() % 10000), lastNames[rng() % 5],
                               firstNames[rng() % 5], 2000 + static_cast<int>(rng() % 8));
            },
            "год + 1",
            [](const Student &s)
            { Student copy = s; copy.birthYear++; return copy; },
            "родился после 2004", [](const Student &s)
            { return s.birthYear > 2004; },
            "самый молодой",
            [](const Student &s, const Student &acc)
            { return s.birthYear > acc.birthYear ? s : acc; },
            Student(0, "-", "-", 0)};
}

void MainMenu()
{
    while (true)
    {
        std::cout << "\n===== ЛР-1: AVL-дерево =====\n"
                  << "1. int   2. double   3. complex   4. string   5. Student\n"
                  << "6. Множества (Set<int>)\n"
                  << "0. Выход\n";
        switch (ReadInt("> "))
        {
        case 0:
            return;
        case 1:
            TreeMenu<int, int>(IntInfo());
            break;
        case 2:
            TreeMenu<double, double>(DoubleInfo());
            break;
        case 3:
            TreeMenu<Complex, Complex>(ComplexInfo());
            break;
        case 4:
            TreeMenu<std::string, std::string>(StringInfo());
            break;
        case 5:
            TreeMenu<int, Student>(StudentInfo());
            break;
        case 6:
            SetMenu();
            break;
        default:
            std::cout << "Нет такого пункта.\n";
        }
    }
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    try
    {
        MainMenu();
    }
    catch (const EndOfStreamException &)
    {
        std::cout << "\nВвод закончился, выход.\n";
    }
    catch (const IException &e)
    {
        std::cout << "\nОшибка: " << e.what() << "\n";
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cout << "\nОшибка: " << e.what() << "\n";
        return 1;
    }
    return 0;
}