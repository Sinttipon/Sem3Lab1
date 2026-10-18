#include <iostream>
#include <functional>
#include <vector>
#include "BinaryTree.h"

int main(){
    BinaryTree<int> tree;
    int values[] = {5,3,7,2,4,6,8};

    for (int v : values) {
        tree.Insert(v);
    }

    std::string orders[] = {"KLP", "KPL", "LKP", "LPK", "PLK", "PKL"};
    for (const std::string& order : orders){
        std::cout << order << ";";
        tree.Traverse(order, [](const int& value) {std::cout << value << " "; });
        std::cout << "\n";
    }

    std::vector<int> sorted = tree.ToVector();
    std::cout << "Sorted, count = " << sorted.size() << "\n";

    try {
        tree.Traverse("KLL", [](const int&) {});
    } catch (const std::invalid_argument& e) {
        std::cout << "error:" << e.what() << "\n";
    }
    return 0;
};