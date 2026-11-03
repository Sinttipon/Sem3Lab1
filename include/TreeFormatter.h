#pragma once
#include <ostream>
#include <sstream>
#include <string>
#include "BinaryTree.h"

template <typename TKey, typename TValue>
class TreeFormatter
{
private:
    using Node = typename BinaryTree<TKey, TValue>::Node;
    std::string format;

    void Write(const Node *node, std::ostream &out) const
    {
        if (node == nullptr)
            return;
        for (char c : format)
        {
            if (c == 'K')
                out << node->getKey();
            else if (c == 'V')
                out << node->getValue();
            else if (c == 'L')
                Write(node->getLeft().get(), out);
            else if (c == 'P')
                Write(node->getRight().get(), out);
            else
                out << c;
        }
    }

public:
    TreeFormatter(const std::string &f) : format(f) {}

    std::string Format(const BinaryTree<TKey, TValue> &tree) const
    {
        std::ostringstream out;
        Write(tree.getRoot().get(), out);
        return out.str();
    }
};