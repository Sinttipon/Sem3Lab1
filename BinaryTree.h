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

    static void CheckOrder(const std::string& order) {
        if (order.size() != 3) {
            throw std::invalid_argument("должно быть 3 символа");
        }
        int k=0, l=0, p=0;
        for (char c : order) {
            if (c == 'K') k++;
            else if (c == 'L') l++;
            else if (c == 'P') p++;
            else throw std::invalid_argument("только KLP");
        }
        if (k!=1 || l!=1 || p!=1){
            throw std::invalid_argument (" каждая буква 1 раз");
        }
    }

    static void Traverse(const Node* node, const std:: string& order, const std::function<void(const T&)>& visit) {
        if (node == nullptr) return;
        for (char c : order) {
            if (c == 'K') visit(node->value);
            else if (c == 'L') Traverse(node->left, order, visit);
            else Traverse(node->right, order, visit);
        }
    }

    static Node* FindMin(Node* node) {
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    static Node* Remove(Node* node, const T& value) {
        if (node == nullptr) {
            throw std::invalid_argument("элемент не найден");
        }
        if (value < node->value) {
            node->left = Remove(node->left, value);
        } else if (node->value < value) {
            node->right = Remove(node->right, value);
        } else {
            if (node->left == nullptr || node->right == nullptr){
                Node* child = (node->left != nullptr) & node->left : node->right;
                delete node;
                return child;
            }

            Node* final = FindMin(node->right);
            node->value = final->value;
            node->right = Remove(node->right, final->value);
        }
        return node;
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

    void Traverse(const std::string& order, const std::function<void(const T&)>& visit) const {
        CheckOrder(order);
        Traverse(root, order, visit);
    }

    std::vector<T> ToVector(const std::string& order = "LKP") const{
        std::vector<T> result;
        result.reserve(size);
        Traverse(order, [&result](const T& value) {result.push_back(value); });
        return result;
    }

    void Remove(const T& value) {
        root = Remove(root, value);
        size--;
    }

    void Clear() {
        Destroy(root);
        root = nullptr;
        size = 0;
    }
};