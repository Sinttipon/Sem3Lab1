#pragma once

#include "Stack.h"
#include "exceptions.hpp"
#include <functional>
#include <utility>

template <typename TKey, typename TValue>
class TreeFormatter;

template <typename TKey, typename TValue>
class BinaryTree
{
public:
    class Node
    {
    private:
        TKey key;
        TValue value;
        Node *left = nullptr;
        Node *right = nullptr;
        int height = 1;

        friend class BinaryTree<TKey, TValue>;
        template <typename K, typename V>
        friend class TreeFormatter;

    public:
        Node() = default;
        Node(TKey k, TValue v, Node *l = nullptr, Node *r = nullptr)
            : key(k), value(v), left(l), right(r), height(1) {}
        ~Node() = default;

        const TValue &GetValue() const { return value; }
        const TKey &GetKey() const { return key; }
        Node *GetLeft() const { return left; }
        Node *GetRight() const { return right; }
    };

    typedef std::function<void(const std::function<void(const TValue &)> &, Node *)> Traverser;

private:
    Node *Root = nullptr;
    std::size_t size = 0;

    template <typename K, typename V>
    friend class TreeFormatter;

    int Height(Node *node) const { return node ? node->height : 0; }

    void UpdateHeight(Node *node)
    {
        if (node)
        {
            int hl = Height(node->left);
            int hr = Height(node->right);
            node->height = (hl > hr ? hl : hr) + 1;
        }
    }

    int BalanceFactor(Node *node) const { return node ? Height(node->left) - Height(node->right) : 0; }

    Node *RotateRight(Node *y)
    {
        Node *x = y->left;
        y->left = x->right;
        x->right = y;
        UpdateHeight(y);
        UpdateHeight(x);
        return x;
    }

    Node *RotateLeft(Node *x)
    {
        Node *y = x->right;
        x->right = y->left;
        y->left = x;
        UpdateHeight(x);
        UpdateHeight(y);
        return y;
    }

    Node *Balance(Node *node)
    {
        if (!node)
            return node;
        UpdateHeight(node);
        int bf = BalanceFactor(node);
        if (bf > 1)
        {
            if (BalanceFactor(node->left) < 0)
                node->left = RotateLeft(node->left);
            return RotateRight(node);
        }
        if (bf < -1)
        {
            if (BalanceFactor(node->right) > 0)
                node->right = RotateRight(node->right);
            return RotateLeft(node);
        }
        return node;
    }

    void DeleteNodes(Node *&TempNode)
    {
        if (TempNode == nullptr)
            return;
        DeleteNodes(TempNode->right);
        DeleteNodes(TempNode->left);
        delete TempNode;
        TempNode = nullptr;
        size--;
    }

    Node *CopyNodes(Node *node)
    {
        if (!node)
            return nullptr;
        Node *newNode = new Node(node->key, node->value);
        newNode->left = CopyNodes(node->left);
        newNode->right = CopyNodes(node->right);
        newNode->height = node->height;
        size++;
        return newNode;
    }

    Node *InsertNode(Node *node, const TKey &k, const TValue &Val)
    {
        if (!node)
        {
            size++;
            return new Node(k, Val);
        }
        if (k < node->key)
            node->left = InsertNode(node->left, k, Val);
        else if (node->key < k)
            node->right = InsertNode(node->right, k, Val);
        else
            throw InvalidArgument("Добавляемый элемент уже есть в дереве.");
        return Balance(node);
    }

    Node *RemoveMin(Node *node, Node *&min)
    {
        if (!node->left)
        {
            min = node;
            return node->right;
        }
        node->left = RemoveMin(node->left, min);
        return Balance(node);
    }

    Node *RemoveNode(Node *node, const TKey &k)
    {
        if (!node)
            return nullptr;
        if (k < node->key)
        {
            node->left = RemoveNode(node->left, k);
            return Balance(node);
        }
        if (node->key < k)
        {
            node->right = RemoveNode(node->right, k);
            return Balance(node);
        }

        Node *left = node->left;
        Node *right = node->right;
        delete node;
        size--;

        if (!right)
            return left;

        Node *min = nullptr;
        right = RemoveMin(right, min);
        min->left = left;
        min->right = right;
        return Balance(min);
    }

    const Node *FindNode(const TKey &k) const
    {
        const Node *node = Root;
        while (node != nullptr)
        {
            if (k < node->key)
                node = node->left;
            else if (node->key < k)
                node = node->right;
            else
                return node;
        }
        return nullptr;
    }

public:
    class TreeEnumerator
    {
    private:
        Node *current = nullptr;
        Stack<Node *> path;
        friend class BinaryTree<TKey, TValue>;

    public:
        TreeEnumerator() = default;

        const TValue &operator*() const
        {
            if (!current)
                throw EndOfStreamException("Попытка разыменования пустого итератора.");
            return current->value;
        }

        const Node *operator->() const
        {
            if (!current)
                throw EndOfStreamException("Попытка разыменования пустого итератора.");
            return current;
        }

        TreeEnumerator &operator++()
        {
            if (!current)
                return *this;
            if (current->right)
            {
                Node *temp = current->right;
                while (temp)
                {
                    path.Push(temp);
                    temp = temp->left;
                }
            }
            if (!path.IsEmpty())
            {
                current = path.Top();
                path.Pop();
            }
            else
                current = nullptr;
            return *this;
        }

        TreeEnumerator operator++(int)
        {
            TreeEnumerator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator!=(const TreeEnumerator &other) const { return current != other.current; }
        bool operator==(const TreeEnumerator &other) const { return current == other.current; }
    };

    BinaryTree() = default;

    BinaryTree(const BinaryTree &other) : Root(nullptr), size(0) { Root = CopyNodes(other.Root); }

    BinaryTree(BinaryTree &&other) noexcept : Root(other.Root), size(other.size)
    {
        other.Root = nullptr;
        other.size = 0;
    }

    BinaryTree &operator=(const BinaryTree &other)
    {
        if (this != &other)
        {
            BinaryTree temp(other);
            std::swap(Root, temp.Root);
            std::swap(size, temp.size);
        }
        return *this;
    }

    BinaryTree &operator=(BinaryTree &&other) noexcept
    {
        if (this != &other)
        {
            Root = other.Root;
            size = other.size;
            other.Root = nullptr;
            other.size = 0;
        }
        return *this;
    }

    ~BinaryTree() { DeleteNodes(Root); }

    std::size_t GetSize() const { return size; }
    bool IsEmpty() const { return size == 0; }
    int GetHeight() const { return Height(Root); }

    void Insert(const TKey &k, const TValue &Val) { Root = InsertNode(Root, k, Val); }

    bool Search(const TKey &k) const { return FindNode(k) != nullptr; }

    void Remove(const TKey &k)
    {
        if (!Search(k))
            throw InvalidArgument("В дереве нет удаляемого элемента.");
        Root = RemoveNode(Root, k);
    }

    void Traverse(const Traverser &usersFunc, const std::function<void(const TValue &)> &action) const
    {
        usersFunc(action, Root);
    }

    TreeEnumerator begin() const
    {
        TreeEnumerator beg;
        Node *temp = Root;
        while (temp)
        {
            beg.path.Push(temp);
            temp = temp->left;
        }
        if (!beg.path.IsEmpty())
        {
            beg.current = beg.path.Top();
            beg.path.Pop();
        }
        return beg;
    }

    TreeEnumerator end() const { return TreeEnumerator(); }
};

template <typename TKey, typename TValue>
void KLPrec(const std::function<void(const TValue &)> &action, typename BinaryTree<TKey, TValue>::Node *n)
{
    if (!n)
        return;
    action(n->GetValue());
    KLPrec<TKey, TValue>(action, n->GetLeft());
    KLPrec<TKey, TValue>(action, n->GetRight());
}

template <typename TKey, typename TValue>
void KPLrec(const std::function<void(const TValue &)> &action, typename BinaryTree<TKey, TValue>::Node *n)
{
    if (!n)
        return;
    action(n->GetValue());
    KPLrec<TKey, TValue>(action, n->GetRight());
    KPLrec<TKey, TValue>(action, n->GetLeft());
}

template <typename TKey, typename TValue>
void LPKrec(const std::function<void(const TValue &)> &action, typename BinaryTree<TKey, TValue>::Node *n)
{
    if (!n)
        return;
    LPKrec<TKey, TValue>(action, n->GetLeft());
    LPKrec<TKey, TValue>(action, n->GetRight());
    action(n->GetValue());
}

template <typename TKey, typename TValue>
void LKPrec(const std::function<void(const TValue &)> &action, typename BinaryTree<TKey, TValue>::Node *n)
{
    if (!n)
        return;
    LKPrec<TKey, TValue>(action, n->GetLeft());
    action(n->GetValue());
    LKPrec<TKey, TValue>(action, n->GetRight());
}

template <typename TKey, typename TValue>
void PLKrec(const std::function<void(const TValue &)> &action, typename BinaryTree<TKey, TValue>::Node *n)
{
    if (!n)
        return;
    PLKrec<TKey, TValue>(action, n->GetRight());
    PLKrec<TKey, TValue>(action, n->GetLeft());
    action(n->GetValue());
}

template <typename TKey, typename TValue>
void PKLrec(const std::function<void(const TValue &)> &action, typename BinaryTree<TKey, TValue>::Node *n)
{
    if (!n)
        return;
    PKLrec<TKey, TValue>(action, n->GetRight());
    action(n->GetValue());
    PKLrec<TKey, TValue>(action, n->GetLeft());
}

template <typename TKey, typename TValue>
void LKPiter(const std::function<void(const TValue &)> &action, typename BinaryTree<TKey, TValue>::Node *n)
{
    Stack<typename BinaryTree<TKey, TValue>::Node *> stack;
    typename BinaryTree<TKey, TValue>::Node *current = n;
    while (current != nullptr || !stack.IsEmpty())
    {
        while (current != nullptr)
        {
            stack.Push(current);
            current = current->GetLeft();
        }
        current = stack.Top();
        stack.Pop();
        action(current->GetValue());
        current = current->GetRight();
    }
}