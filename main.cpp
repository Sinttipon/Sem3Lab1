#include <iostream>
#include "BinaryTree.h"

int main(){
    BinaryTree<int> tree;
    int values[] = {1,5,2,7,4};

    for (int v : values) {
        tree.Insert(v);
    }
    std::cout << "size" << tree.GetSize() << "\n";
    std::cout << "contains 4 " << tree.Contains(4) << "\n";
    std::cout << "contains 0 " << tree.Contains(0) << "\n";

    try {
        tree.Insert(4);
    } catch (const std::invalid_argument& e) {
        std::cout << "err" << e.what() << "\n";
    }

    BinaryTree<int> copy = tree;
    std::cout << "copy size" << copy.GetSize() << "\n";
};