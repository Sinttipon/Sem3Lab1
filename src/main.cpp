#include <iostream>
#include <string>
#include <sstream>
#include <limits>
#include <random>
#ifdef _WIN32
#include <windows.h>
#endif
#include "BinaryTree.h"
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
            throw EndOfStreamException("Ввод закончился");
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка. Требуется целое число.\n";
    }
}

std::string ReadWord(const std::string &prompt)
{
    std::cout << prompt;
    std::string word;
    if (!(std::cin >> word))
        throw EndOfStreamException("Ввод закончился");
    return word;
}

template <typename T>
T ReadKeyValue(const std::string &prompt)
{
    while (true)
    {
        std::cout << prompt;
        std::string input;
        if (!(std::cin >> input))
            throw EndOfStreamException("Ввод закончился");

        std::istringstream stream(input);
        T val;
        if (stream >> val && (stream >> std::ws).eof())
            return val;

        std::cout << "Не удалось распознать значение, попробуйте еще раз.\n";
    }
}

template <typename TKey, typename TValue>
void TreeMenu(const std::string &typeName)
{
    BinaryTree<TKey, TValue> tree;

    while (true)
    {
        std::cout << "\n===== Дерево [Ключ: " << typeName << "] =====\n"
                  << "Размер дерева: " << tree.GetSize() << "\n"
                  << " 1. Вставить элемент (Ключ + Значение)\n"
                  << " 2. Удалить элемент по ключу\n"
                  << " 3. Проверить наличие ключа\n"
                  << " 4. Обход дерева (Klp, Kpl, Lkp, Lpk, Plk, Pkl)\n"
                  << " 5. Очистить дерево\n"
                  << " 0. Назад к выбору типов\n";

        int choice = ReadInt("> ");
        switch (choice)
        {
        case 0:
            return;

        case 1:
        {
            TKey key = ReadKeyValue<TKey>("Введите ключ (" + typeName + ", по нему идет сортировка): ");
            TValue value = ReadKeyValue<TValue>("Введите значение: ");
            try
            {
                tree.insert(key, value);
                std::cout << "Элемент успешно добавлен и сбалансирован.\n";
            }
            catch (const IException &e)
            {
                std::cout << "Ошибка вставки: " << e.what() << "\n";
            }
            break;
        }

        case 2:
        {
            TKey key = ReadKeyValue<TKey>("Введите ключ для удаления: ");
            try
            {
                tree.remove(key);
                std::cout << "Элемент удален.\n";
            }
            catch (const IException &e)
            {
                std::cout << "Ошибка удаления: " << e.what() << "\n";
            }
            break;
        }

        case 3:
        {
            TKey key = ReadKeyValue<TKey>("Введите ключ для поиска: ");
            if (tree.contains(key))
            {
                std::cout << "Ключ найден в дереве.\n";
            }
            else
            {
                std::cout << "Ключ не найден.\n";
            }
            break;
        }

        case 4:
        {
            std::string strategy = ReadWord("Выберите стратегию обхода (Klp, Kpl, Lkp, Lpk, Plk, Pkl): ");
            auto printAction = [](const TValue &val)
            {
                std::cout << val << " ";
            };

            std::cout << "Результат обхода: ";
            try
            {
                if (strategy == "Klp")
                    tree.template traverse<KlpStrategy>(printAction);
                else if (strategy == "Kpl")
                    tree.template traverse<KplStrategy>(printAction);
                else if (strategy == "Lkp")
                    tree.template traverse<LkpStrategy>(printAction);
                else if (strategy == "Lpk")
                    tree.template traverse<LpkStrategy>(printAction);
                else if (strategy == "Plk")
                    tree.template traverse<PlkStrategy>(printAction);
                else if (strategy == "Pkl")
                    tree.template traverse<PklStrategy>(printAction);
                else
                    std::cout << "Неизвестная стратегия обхода!";
            }
            catch (const IException &e)
            {
                std::cout << "Ошибка при обходе: " << e.what();
            }
            std::cout << "\n";
            break;
        }

        case 5:
        {
            tree = BinaryTree<TKey, TValue>();
            std::cout << "Дерево очищено.\n";
            break;
        }

        default:
            std::cout << "Нет такого пункта меню.\n";
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
        while (true)
        {
            std::cout << "\n===== Лабораторная работа: АВЛ-дерево =====\n"
                      << "Выберите тип ключа для сортировки:\n"
                      << "1. int (целые числа)\n"
                      << "2. std::string (строки)\n"
                      << "3. double (вещественные числа)\n"
                      << "0. Выход\n";

            int choice = ReadInt("> ");
            if (choice == 0)
                break;

            if (choice == 1)
            {
                TreeMenu<int, std::string>("int");
            }
            else if (choice == 2)
            {
                TreeMenu<std::string, std::string>("string");
            }
            else if (choice == 3)
            {
                TreeMenu<double, std::string>("double");
            }
            else
            {
                std::cout << "Неверный выбор.\n";
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Критическая ошибка: " << e.what() << "\n";
        return 1;
    }

    return 0;
}