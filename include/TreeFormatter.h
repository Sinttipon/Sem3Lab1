#pragma once
#include <ostream>
#include <sstream>
#include "BinaryTree.h"
#include <string>

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
                out << node->GetKey();
            else if (c == 'V')
                out << node->GetValue();
            else if (c == 'L')
                Write(node->GetLeft(), out);
            else if (c == 'P')
                Write(node->GetRight(), out);
            else
                out << c;
        }
    }

public:
    TreeFormatter(const std::string &f) : format(f) {}

    std::string Format(const BinaryTree<TKey, TValue> &tree) const
    {
        std::ostringstream out;
        Write(tree.Root, out);
        return out.str();
    }
};