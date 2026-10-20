#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include "BinaryTree.h"

template <typename Action>
double MeasureMs(Action action) {
    auto start = std::chrono::steady_clock::now();
    action();
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

int main(int argc, char* argv[]) {

    long long maxN = (argc > 1) ? std::stoll(argv[1]) : 10000000;
    std::vector<long long> sizes = {10000, 30000, 100000, 300000, 1000000, 3000000, 10000000};

    std::ofstream csv("results.csv");
    csv << "n,insert_random,insert_sorted,search,remove\n";
    std::cout << "n\tinsert_random\tinsert_sorted\tsearch\tremove   (ms)\n";

    std::mt19937 rng(42);
    for (long long n : sizes) {
        if (n > maxN) break;

        std::vector<int> values(n);
        std::iota(values.begin(), values.end(), 0);
        std::shuffle(values.begin(), values.end(), rng);

        BinaryTree<int> tree;
        double insertRandom = MeasureMs([&] {
            for (int v : values) tree.Insert(v);
        });

        std::vector<int> queries = values;
        std::shuffle(queries.begin(), queries.end(), rng);
        long long found = 0;
        double search = MeasureMs([&] {
            for (int v : queries) found += tree.Contains(v);
        });

        double remove = MeasureMs([&] {
            for (int v : queries) tree.Remove(v);
        });

        BinaryTree<int> sorted;
        double insertSorted = MeasureMs([&] {
            for (long long i = 0; i < n; i++) sorted.Insert(static_cast<int>(i));
        });

        if (found != n || !tree.IsEmpty()) {
            std::cerr << "Error: wrong result at n = " << n << "\n";
            return 1;
        }

        csv << n << ',' << insertRandom << ',' << insertSorted << ',' << search << ',' << remove << '\n';
        std::cout << n << '\t' << insertRandom << '\t' << insertSorted << '\t'
                  << search << '\t' << remove << std::endl;
    }
    std::cout << "Saved to results.csv\n";
    return 0;
}
