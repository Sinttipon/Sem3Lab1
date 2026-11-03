#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <iomanip>
#include "BinaryTree.h"


void RunBenchmarkForSize(int n)
{
    std::cout << "\n--- Запуск бенчмарка для N = " << n << " элементов ---\n";

    BinaryTree<int, int> tree;
    std::vector<int> keys(n);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, n * 10);
    for (int i = 0; i < n; ++i)
    {
        keys[i] = dist(gen);
    }

    auto startInsert = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; ++i)
    {
        try
        {
            tree.insert(keys[i], i);
        }
        catch (...)
        {
            
        }
    }
    auto endInsert = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> insertDuration = endInsert - startInsert;
    std::cout << "Время вставки " << n << " элементов: " << insertDuration.count() << " мс\n";

    auto startSearch = std::chrono::high_resolution_clock::now();
    int foundCount = 0;
    for (int i = 0; i < n; ++i)
    {
        if (tree.contains(keys[i]))
        {
            foundCount++;
        }
    }
    auto endSearch = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> searchDuration = endSearch - startSearch;
    std::cout << "Время поиска " << n << " элементов:  " << searchDuration.count() << " мс (Найдено: " << foundCount << ")\n";

    auto startRemove = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n / 2; ++i)
    {
        try
        {
            tree.remove(keys[i]);
        }
        catch (...)
        {
        }
    }
    auto endRemove = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> removeDuration = endRemove - startRemove;
    std::cout << "Время удаления " << (n / 2) << " элементов: " << removeDuration.count() << " мс\n";
    std::cout << "Итоговый размер дерева после удаления: " << tree.GetSize() << "\n";
}

int main()
{
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    std::cout << "=== СТАРТ ТЕСТИРОВАНИЯ ПРОИЗВОДИТЕЛЬНОСТИ АВЛ-ДЕРЕВА ===\n";

    RunBenchmarkForSize(1000);
    RunBenchmarkForSize(10000);
    RunBenchmarkForSize(50000);

    std::cout << "=== БЕНЧМАРК УСПЕШНО ЗАВЕРШЕН ===\n";
    return 0;
}