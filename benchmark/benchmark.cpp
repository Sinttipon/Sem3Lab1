#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include "BinaryTree.h"

using Clock = std::chrono::high_resolution_clock;
using Ms = std::chrono::milliseconds;

int main(int argc, char *argv[])
{
    int n = (argc > 1) ? std::atoi(argv[1]) : 100000;
    int q = (argc > 2) ? std::atoi(argv[2]) : 10000;

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, n * 10);

    std::vector<int> values(n);
    for (int &v : values)
        v = dist(rng);

    std::vector<int> queries(q);
    for (int &v : queries)
        v = dist(rng);

    // Случайная вставка, поиск, удаление
    {
        BinaryTree<int, int> tree;
        auto t0 = Clock::now();
        for (int v : values)
        {
            try
            {
                tree.Insert(v, v);
            }
            catch (...)
            {
            }
        }
        auto t1 = Clock::now();
        std::cout << "Insert " << n << " random: "
                  << std::chrono::duration_cast<Ms>(t1 - t0).count() << " ms\n";

        int found = 0;
        t0 = Clock::now();
        for (int v : queries)
        {
            if (tree.Search(v))
                found++;
        }
        t1 = Clock::now();
        std::cout << "Search " << q << " random: "
                  << std::chrono::duration_cast<Ms>(t1 - t0).count() << " ms (found " << found << ")\n";

        t0 = Clock::now();
        for (int v : queries)
        {
            try
            {
                tree.Remove(v);
            }
            catch (...)
            {
            }
        }
        t1 = Clock::now();
        std::cout << "Remove " << q << " random: "
                  << std::chrono::duration_cast<Ms>(t1 - t0).count() << " ms\n";

        if (found != 0 || !tree.IsEmpty())
        {
            std::cerr << "benchmark: unexpected state\n";
            return 1;
        }
    }

    // Отсортированная вставка (стресс-тест балансировки)
    {
        BinaryTree<int, int> sorted;
        auto t0 = Clock::now();
        for (int i = 0; i < n; i++)
        {
            try
            {
                sorted.Insert(i, i);
            }
            catch (...)
            {
            }
        }
        auto t1 = Clock::now();
        std::cout << "Insert " << n << " sorted: "
                  << std::chrono::duration_cast<Ms>(t1 - t0).count() << " ms\n";

        if (sorted.GetSize() != static_cast<std::size_t>(n))
        {
            std::cerr << "benchmark: size mismatch\n";
            return 1;
        }
        std::cout << "Height: " << sorted.GetHeight()
                  << " (log2(n) ~ " << 3.32 * std::log10(n) << ")\n";
    }

    return 0;
}