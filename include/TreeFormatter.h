#pragma once
#include <ostream>
#include <sstream>
#include "BinaryTree.h"
#include <string>

template <typename T>
class TreeFormatter {
private:
    using Node = typename BinaryTree<T>::Node;

    std::string format;

    void Write(const Node* node, std::ostream& out) const {
        if (node == nullptr) return;
        for (char c : format) {
            if (c == 'K') out << node->value;
            else if (c == 'L') Write(node->left, out);
            else if (c == 'P') Write(node->right, out);
            else out << c;
        }
    }

public:
    TreeFormatter(const std::string& f) : format(f) {}

    std::string Format(const BinaryTree<T>& tree) const {
        std::ostringstream out;
        Write(tree.root, out);
        return out.str();
    }
};
