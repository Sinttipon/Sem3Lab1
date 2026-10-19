#pragma once
#include <functional>
#include <vector>
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
        int height;

        Node(const T &v) : value(v), left(nullptr), right(nullptr), height(1) {}
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
        result->height = node->height;
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

    static int Height(const Node* node){
        return node == nullptr ? 0 : node->height;
    }

    static int BalanceFactor(const Node* node){
        return Height(node->left) - Height(node->right);
    }

    static void UpdateHeight(Node* node){
        int h1 = Height(node->left);
        int h2 = Height(node->right);
        node->height = (h1 > h2 ? h1 : h2) + 1; 
    }

    static Node* RotateRight(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        x->right = y;
        UpdateHeight(y);
        UpdateHeight(x);
        return x;
    }

    static Node* RotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        y->left = x;
        UpdateHeight(x);
        UpdateHeight(y);
        return y;
    }

    static Node* Balance(Node* node) {
        UpdateHeight(node);
        int balance = BalanceFactor(node);
        if (balance > 1) {
            if (BalanceFactor(node->left) < 0) {
                node->left = RotateLeft(node->left);
            }
            return RotateRight(node);
        }
        if (balance < -1) {
            if (BalanceFactor(node->right) > 0) {
                node->right = RotateRight(node->right);
            }
            return RotateLeft(node);
        }
        return node;
    }

    static Node* Insert(Node* node, const T& value) {
        if (node == nullptr) {
            return new Node(value);
        }
        if (value < node->value) {
            node->left = Insert(node->left, value);
        } else if (node->value < value) {
            node->right = Insert(node->right, value);
        } else {
            throw std::invalid_argument("элемент уже существует");
        }
        return Balance(node);
    }

    static bool ISBalanced(const Node* node){
        if (node == nullptr) return true;
        int h1 = Height(node->left);
        int h2 = Height(node->right);
        int exp = (h1 > h2 ? h1 : h2) + 1;
        return node->height == expected && h1-h2 <=1 && h2-h1 <=1 && ISBalanced(node->left) && ISBalanced(node->right);
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
                Node* child = (node->left != nullptr) ? node->left : node->right;
                delete node;
                return child;
            }

            Node* final = FindMin(node->right);
            node->value = final->value;
            node->right = Remove(node->right, final->value);
        }
        return Balance(node);
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

    int GetHeight() const {return Height(root);}
    bool ISBalanced() const {return ISBalanced(root);}

    void Insert(const T& value){
        root = Insert(root, value);
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

    BinaryTree Map(const std::function<T(const T&)>& f) const {
        BinaryTree result;
        Traverse("LKP", [&result, &f](const T& value){
            T mapped = f(valued);
            if (!result.Contains(mapped)){
                result.Insert(mapped);
            }
        });
        return result;
    }

    BinaryTree Where(const std::function<bool(const T&)>& predicate) const {
        BinaryTree result;
        Traverse("LKP", [&result, &predicate](const T& value) {
            if (predicate(value)) {
                result.Insert(value);
            }
        });
        return result;
    }

    T Reduce(const std::function<T(const T&, const T&)>& f, const T& start) const {
        T accumulator = start;
        Traverse("LKP", [&accumulator, &f](const T& value) {
            accumulator = f(value, accumulator);
        });
        return accumulator
    }
}; 