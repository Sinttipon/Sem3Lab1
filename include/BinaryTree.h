#pragma once
#include <cstddef>
#include <functional>
#include <sstream>
#include <utility>
#include "Stack.h"
#include <string>
#include "exceptions.hpp"

enum class Order { KLP, KPL, LKP, LPK, PLK, PKL };

template <typename T>
class TreeFormatter;

template <typename T>
class BinaryTree {
private:
    friend class TreeFormatter<T>;

    struct Node {
        T value;
        Node* left;
        Node* right;
        int height;

        Node(const T& v) : value(v), left(nullptr), right(nullptr), height(1) {}
    };

    using Action = std::function<void(const T&)>;

    Node* root;
    std::size_t size;

    bool Same(const T& a, const T& b) const { return !(a < b) && !(b < a); }

    Node* CreateNode(const T& value) {
        Node* node = new Node(value);
        size++;
        return node;
    }

    void DeleteNode(Node* node) {
        delete node;
        size--;
    }

    void DeleteNodes(Node* node) {
        if (node == nullptr) return;
        DeleteNodes(node->left);
        DeleteNodes(node->right);
        DeleteNode(node);
    }

    Node* CopyNodes(const Node* node) {
        if (node == nullptr) return nullptr;
        Node* result = CreateNode(node->value);
        try {
            result->left = CopyNodes(node->left);
            result->right = CopyNodes(node->right);
        } catch (...) {
            DeleteNodes(result);
            throw;
        }
        result->height = node->height;
        return result;
    }

    int Height(const Node* node) const { return node == nullptr ? 0 : node->height; }

    void UpdateHeight(Node* node) {
        int hl = Height(node->left);
        int hr = Height(node->right);
        node->height = (hl > hr ? hl : hr) + 1;
    }

    int BalanceFactor(const Node* node) const { return Height(node->left) - Height(node->right); }

    Node* RotateRight(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        x->right = y;
        UpdateHeight(y);
        UpdateHeight(x);
        return x;
    }

    Node* RotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        y->left = x;
        UpdateHeight(x);
        UpdateHeight(y);
        return y;
    }

    Node* Balance(Node* node) {
        UpdateHeight(node);
        int bf = BalanceFactor(node);
        if (bf > 1) {
            if (BalanceFactor(node->left) < 0) node->left = RotateLeft(node->left);
            return RotateRight(node);
        }
        if (bf < -1) {
            if (BalanceFactor(node->right) > 0) node->right = RotateRight(node->right);
            return RotateLeft(node);
        }
        return node;
    }

    Node* InsertNode(Node* node, const T& value, bool balance) {
        if (node == nullptr) return CreateNode(value);
        if (value < node->value) node->left = InsertNode(node->left, value, balance);
        else if (node->value < value) node->right = InsertNode(node->right, value, balance);
        else return node;
        if (balance) return Balance(node);
        UpdateHeight(node);
        return node;
    }

    Node* RemoveMin(Node* node, Node*& min) {
        if (node->left == nullptr) {
            min = node;
            return node->right;
        }
        node->left = RemoveMin(node->left, min);
        return Balance(node);
    }

    Node* RemoveNode(Node* node, const T& value) {
        if (node == nullptr) return nullptr;
        if (value < node->value) {
            node->left = RemoveNode(node->left, value);
            return Balance(node);
        }
        if (node->value < value) {
            node->right = RemoveNode(node->right, value);
            return Balance(node);
        }
        Node* left = node->left;
        Node* right = node->right;
        DeleteNode(node);
        if (right == nullptr) return left;
        Node* min = nullptr;
        right = RemoveMin(right, min);
        min->left = left;
        min->right = right;
        return Balance(min);
    }

    const Node* FindNode(const T& value) const {
        const Node* node = root;
        while (node != nullptr) {
            if (value < node->value) node = node->left;
            else if (node->value < value) node = node->right;
            else return node;
        }
        return nullptr;
    }

    bool CheckBalance(const Node* node) const {
        if (node == nullptr) return true;
        int hl = Height(node->left);
        int hr = Height(node->right);
        int bf = hl - hr;
        return node->height == (hl > hr ? hl : hr) + 1 && bf >= -1 && bf <= 1
            && CheckBalance(node->left) && CheckBalance(node->right);
    }

    bool EqualNodes(const Node* a, const Node* b) const {
        if (a == nullptr || b == nullptr) return a == b;
        return Same(a->value, b->value) && EqualNodes(a->left, b->left) && EqualNodes(a->right, b->right);
    }

    void KLP(const Node* node, const Action& action) const {
        if (node == nullptr) return;
        action(node->value);
        KLP(node->left, action);
        KLP(node->right, action);
    }

    void KPL(const Node* node, const Action& action) const {
        if (node == nullptr) return;
        action(node->value);
        KPL(node->right, action);
        KPL(node->left, action);
    }

    void LKP(const Node* node, const Action& action) const {
        if (node == nullptr) return;
        LKP(node->left, action);
        action(node->value);
        LKP(node->right, action);
    }

    void LPK(const Node* node, const Action& action) const {
        if (node == nullptr) return;
        LPK(node->left, action);
        LPK(node->right, action);
        action(node->value);
    }

    void PLK(const Node* node, const Action& action) const {
        if (node == nullptr) return;
        PLK(node->right, action);
        PLK(node->left, action);
        action(node->value);
    }

    void PKL(const Node* node, const Action& action) const {
        if (node == nullptr) return;
        PKL(node->right, action);
        action(node->value);
        PKL(node->left, action);
    }

    bool InsertValue(const T& value, bool balance) {
        std::size_t before = size;
        root = InsertNode(root, value, balance);
        return size != before;
    }

    void Swap(BinaryTree& other) noexcept {
        std::swap(root, other.root);
        std::swap(size, other.size);
    }

public:
    class Iterator {
    private:
        friend class BinaryTree;

        Stack<const Node*> path;
        const Node* current = nullptr;

        Iterator(const Node* root) {
            PushLeft(root);
            Advance();
        }

        void PushLeft(const Node* node) {
            while (node != nullptr) {
                path.Push(node);
                node = node->left;
            }
        }

        void Advance() {
            if (path.IsEmpty()) {
                current = nullptr;
                return;
            }
            current = path.Top();
            path.Pop();
            PushLeft(current->right);
        }

    public:
        Iterator() = default;

        const T& operator*() const {
            if (current == nullptr) throw EndOfStreamException("BinaryTree::Iterator: dereferencing end");
            return current->value;
        }

        const T* operator->() const { return &**this; }

        Iterator& operator++() {
            if (current != nullptr) Advance();
            return *this;
        }

        Iterator operator++(int) {
            Iterator old = *this;
            ++*this;
            return old;
        }

        bool operator==(const Iterator& other) const { return current == other.current; }
        bool operator!=(const Iterator& other) const { return current != other.current; }
    };

    BinaryTree() : root(nullptr), size(0) {}

    BinaryTree(const BinaryTree& other) : root(nullptr), size(0) { root = CopyNodes(other.root); }

    BinaryTree(BinaryTree&& other) noexcept : root(other.root), size(other.size) {
        other.root = nullptr;
        other.size = 0;
    }

    BinaryTree& operator=(const BinaryTree& other) {
        BinaryTree copy(other);
        Swap(copy);
        return *this;
    }

    BinaryTree& operator=(BinaryTree&& other) noexcept {
        BinaryTree moved(std::move(other));
        Swap(moved);
        return *this;
    }

    ~BinaryTree() { DeleteNodes(root); }

    void Clear() {
        DeleteNodes(root);
        root = nullptr;
    }

    std::size_t GetSize() const { return size; }
    bool IsEmpty() const { return size == 0; }
    int GetHeight() const { return Height(root); }
    bool IsBalanced() const { return CheckBalance(root); }

    bool Insert(const T& value) { return InsertValue(value, true); }

    bool Remove(const T& value) {
        std::size_t before = size;
        root = RemoveNode(root, value);
        return size != before;
    }

    bool Contains(const T& value) const { return FindNode(value) != nullptr; }

    Iterator begin() const { return Iterator(root); }
    Iterator end() const { return Iterator(); }

    void Traverse(Order order, const Action& action) const {
        switch (order) {
            case Order::KLP: KLP(root, action); break;
            case Order::KPL: KPL(root, action); break;
            case Order::LKP: LKP(root, action); break;
            case Order::LPK: LPK(root, action); break;
            case Order::PLK: PLK(root, action); break;
            case Order::PKL: PKL(root, action); break;
        }
    }

    template <typename F>
    BinaryTree Map(F f) const {
        BinaryTree result;
        Traverse(Order::KLP, [&](const T& value) { result.Insert(f(value)); });
        return result;
    }

    template <typename Predicate>
    BinaryTree Where(Predicate predicate) const {
        BinaryTree result;
        Traverse(Order::KLP, [&](const T& value) {
            if (predicate(value)) result.Insert(value);
        });
        return result;
    }

    template <typename F>
    T Reduce(F f, const T& start) const {
        T accumulator = start;
        Traverse(Order::LKP, [&](const T& value) { accumulator = f(value, accumulator); });
        return accumulator;
    }

    std::string ToString(Order order = Order::KLP) const {
        std::ostringstream out;
        bool first = true;
        Action print = [&](const T& value) {
            if (!first) out << ' ';
            out << value;
            first = false;
        };
        Traverse(order, print);
        return out.str();
    }

    void LoadFromString(const std::string& text) {
        BinaryTree loaded;
        std::istringstream in(text);
        while (in >> std::ws && !in.eof()) {
            T value;
            if (!(in >> value)) throw InvalidArgument("BinaryTree::LoadFromString: bad element");
            loaded.InsertValue(value, false);
        }
        if (!loaded.IsBalanced()) {
            BinaryTree balanced;
            loaded.Traverse(Order::KLP, [&](const T& value) { balanced.Insert(value); });
            loaded.Swap(balanced);
        }
        Swap(loaded);
    }

    BinaryTree ExtractSubtree(const T& value) const {
        BinaryTree result;
        result.root = result.CopyNodes(FindNode(value));
        return result;
    }

    bool ContainsSubtree(const BinaryTree& other) const {
        if (other.root == nullptr) return true;
        return EqualNodes(FindNode(other.root->value), other.root);
    }

    bool operator==(const BinaryTree& other) const {
        return size == other.size && EqualNodes(root, other.root);
    }

    bool operator!=(const BinaryTree& other) const { return !(*this == other); }
};
