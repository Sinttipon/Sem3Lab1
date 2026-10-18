#pragma once
#include <stdexcept>
#include <string>

template <typename T>
class BinaryTree
{
private:
    struct Node
    {
        T value;
        Node *left;
        Node *right;

        Node(const T &v) : value(v), left(nullptr), right(nullptr) {}
    };

    Node *root;
    size_t size;

    static void Destroy(Node *node)
    {
        if (node == nullptr)
            return;
        Destroy(node->left);
        Destroy(node->right);
        delete node;
    }

    static Node *Copy(const Node *node)
    {
        if (node == nullptr)
            return nullptr;
        Node *result = new Node(node->value);
        result->left = Copy(node->left);
        result->right = Copy(node->right);
        return result;
    }

public:
    BinaryTree() : root(nullptr), size(0) {}
    BinaryTree(const BinaryTree &other) : root(Copy(other.root)), size(other.size) {}
    BinaryTree &operator=(const BinaryTree &other)
    {
        if (this != &other)
        {
            Destroy(root);
            root = Copy(other.root);
            size = other.size;
        }
        return *this;
    }

    ~BinaryTree() { Destroy(root); }

    size_t GetSize() const { return size; }
    bool IsEmpty() const { return size == 0; }

    void Insert(const T &value)
    {
        Node **current = &root;
        while (*current != nullptr)
        {
            if (value < (*current)->value)
            {
                current = &(*current)->left;
            }
            else if ((*current)->value < value)
            {
                current = &(*current)->right;
            }
            else
            {
                throw std::invalid_argument("Элемент уже существует");
            }
        }
        *current = new Node(value);
        size++;
    }

    bool Contains(const T &value) const
    {
        Node *current = root;
        while (current != nullptr)
        {
            if (value < current->value)
            {
                current = current->left;
            }
            else if (current->value < value)
            {
                current = current->right;
            }
            else
            {
                return true;
            }
        }
        return false;
    }
};