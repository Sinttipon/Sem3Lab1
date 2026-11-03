#pragma once

#include "Stack.h"
#include <stack>
#include "exceptions.hpp"
#include <functional>
#include "Pair.hpp"
#include <memory>
#include <concepts>

struct KlpStrategy;
struct KplStrategy;
struct LkpStrategy;
struct PklStrategy;
struct LpkStrategy;
struct PlkStrategy;

template <typename Policy, typename NodePtr, typename StackRef>
concept IsTraversalPolicy = requires(NodePtr node, StackRef stack) {
    { Policy::start(node, stack) } -> std::convertible_to<NodePtr>;
    { Policy::increment(node, stack) } -> std::convertible_to<NodePtr>;
};

template <std::totally_ordered TKey, typename TValue>
class BinaryTree
{
public:
    class Node
    {
    private:
        Pair<TKey, TValue> data;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
        unsigned short height = 1;

        friend class BinaryTree<TKey, TValue>;

    public:
        Node() = default;
        Node(TKey key, TValue value, std::shared_ptr<Node> leftNode = nullptr, std::shared_ptr<Node> rightNode = nullptr)
            : data(key, value), left(std::move(leftNode)), right(std::move(rightNode)), height(1) {}
        ~Node() = default;

        const TValue &getValue() const
        {
            return data.second;
        }

        const TKey &getKey() const
        {
            return data.first;
        }

        std::shared_ptr<Node> getLeft() const
        {
            return left;
        }

        std::shared_ptr<Node> getRight() const
        {
            return right;
        }
    };

private:
    std::shared_ptr<Node> root;

    std::shared_ptr<Node> copyNodes(const std::shared_ptr<const Node> &node)
    {
        if (!node)
            return nullptr;
        auto newNode = std::make_shared<Node>(node->getKey(), node->getValue());
        newNode->height = node->height;
        newNode->left = copyNodes(node->getLeft());
        newNode->right = copyNodes(node->getRight());
        return newNode;
    }

    unsigned short getNodeHeight(const std::shared_ptr<const Node> &node) const
    {
        return node ? node->height : 0;
    }

    void updateHeight(const std::shared_ptr<Node> &node)
    {
        unsigned short leftHeight = getNodeHeight(node->getLeft());
        unsigned short rightHeight = getNodeHeight(node->getRight());
        node->height = (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
    }

    short getBalanceFactor(const std::shared_ptr<const Node> &node) const
    {
        return static_cast<short>(getNodeHeight(node->getLeft())) - static_cast<short>(getNodeHeight(node->getRight()));
    }

    std::shared_ptr<Node> rotateRight(std::shared_ptr<Node> yNode)
    {
        std::shared_ptr<Node> xNode = std::move(yNode->left);
        yNode->left = std::move(xNode->right);
        updateHeight(yNode);
        xNode->right = std::move(yNode);
        updateHeight(xNode);
        return xNode;
    }

    std::shared_ptr<Node> rotateLeft(std::shared_ptr<Node> xNode)
    {
        std::shared_ptr<Node> yNode = std::move(xNode->right);
        xNode->right = std::move(yNode->left);
        updateHeight(xNode);
        yNode->left = std::move(xNode);
        updateHeight(yNode);
        return yNode;
    }

    std::shared_ptr<Node> balanceNode(std::shared_ptr<Node> node)
    {
        updateHeight(node);
        short balanceFactor = getBalanceFactor(node);

        if (balanceFactor > 1)
        {
            if (getBalanceFactor(node->getLeft()) < 0)
                node->left = rotateLeft(std::move(node->left));
            return rotateRight(std::move(node));
        }
        if (balanceFactor < -1)
        {
            if (getBalanceFactor(node->getRight()) > 0)
                node->right = rotateRight(std::move(node->right));
            return rotateLeft(std::move(node));
        }
        return node;
    }

    std::shared_ptr<Node> insertNode(std::shared_ptr<Node> node, const TKey &key, const TValue &value)
    {
        if (!node)
            return std::make_shared<Node>(key, value);
        if (key < node->data.first)
        {
            node->left = insertNode(std::move(node->left), key, value);
        }
        else if (key > node->data.first)
        {
            node->right = insertNode(std::move(node->right), key, value);
        }
        else
        {
            throw InvalidArgument("Добавляемый элемент уже есть в дереве.");
        }
        return balanceNode(std::move(node));
    }

    std::shared_ptr<Node> removeMin(std::shared_ptr<Node> node, std::shared_ptr<Node> &minNode)
    {
        if (!node->left)
        {
            minNode = std::move(node);
            return std::move(minNode->right);
        }
        node->left = removeMin(std::move(node->left), minNode);
        return balanceNode(std::move(node));
    }

    std::shared_ptr<Node> removeNode(std::shared_ptr<Node> node, const TKey &key)
    {
        if (!node)
            throw InvalidArgument("В дереве нет удаляемого элемента.");
        if (key < node->data.first)
        {
            node->left = removeNode(std::move(node->left), key);
            return balanceNode(std::move(node));
        }
        if (key > node->data.first)
        {
            node->right = removeNode(std::move(node->right), key);
            return balanceNode(std::move(node));
        }

        std::shared_ptr<Node> leftNode = std::move(node->left);
        std::shared_ptr<Node> rightNode = std::move(node->right);
        node.reset();

        if (!rightNode)
            return leftNode;

        std::shared_ptr<Node> minNode;
        rightNode = removeMin(std::move(rightNode), minNode);
        minNode->left = std::move(leftNode);
        minNode->right = std::move(rightNode);
        return balanceNode(std::move(minNode));
    }

public:
    template <typename TraversalPolicy>
        requires IsTraversalPolicy<TraversalPolicy, std::shared_ptr<Node>, Stack<std::shared_ptr<Node>> &>
    class TreeEnumerator
    {
    private:
        std::weak_ptr<Node> current;
        Stack<std::shared_ptr<Node>> path;

        friend class BinaryTree<TKey, TValue>;

    public:
        TreeEnumerator() = default;

        const TValue &operator*() const
        {
            auto sharedCurrent = current.lock();
            if (!sharedCurrent)
            {
                throw NullPointerException("Попытка разыменования пустого итератора.");
            }
            return sharedCurrent->data.second;
        }

        TreeEnumerator &operator++()
        {
            current = TraversalPolicy::increment(current.lock(), path);
            return *this;
        }

        TreeEnumerator operator++(int)
        {
            TreeEnumerator temporaryIterator = *this;
            ++(*this);
            return temporaryIterator;
        }

        bool operator!=(const TreeEnumerator &other) const
        {
            return current.lock() != other.current.lock();
        }

        bool operator==(const TreeEnumerator &other) const
        {
            return current.lock() == other.current.lock();
        }
    };

    BinaryTree() = default;

    BinaryTree(const BinaryTree &other)
    {
        root = copyNodes(other.root);
    }

    BinaryTree &operator=(const BinaryTree &other)
    {
        if (this == &other)
            return *this;
        root = copyNodes(other.root);
        return *this;
    }

    BinaryTree(BinaryTree &&other) noexcept : root(std::move(other.root)) {}

    BinaryTree &operator=(BinaryTree &&other) noexcept
    {
        if (this == &other)
            return *this;
        root = std::move(other.root);
        return *this;
    }

    ~BinaryTree() = default;

    void insert(const TKey &key, const TValue &value)
    {
        root = insertNode(std::move(root), key, value);
    }

    bool contains(const TKey &key) const
    {
        if (!root)
            return false;
        std::shared_ptr<Node> current = root;
        while (current)
        {
            if (key < current->data.first)
            {
                current = current->getLeft();
            }
            else if (key > current->data.first)
            {
                current = current->getRight();
            }
            else
            {
                return true;
            }
        }
        return false;
    }

    void remove(const TKey &key)
    {
        root = removeNode(std::move(root), key);
    }

    std::size_t GetSize() const
    {
        std::size_t size = 0;
        for (auto it = begin<LkpStrategy>(); it != end<LkpStrategy>(); ++it)
        {
            ++size;
        }
        return size;
    }

    template <typename TraversalPolicy>
    void traverse(const std::function<void(const TValue &)> &action) const
    {
        for (auto iterator = begin<TraversalPolicy>(); iterator != end<TraversalPolicy>(); ++iterator)
        {
            action(*iterator);
        }
    }

    template <typename TraversalPolicy>
        requires IsTraversalPolicy<TraversalPolicy, std::shared_ptr<Node>, Stack<std::shared_ptr<Node>> &>
    TreeEnumerator<TraversalPolicy> begin() const
    {
        TreeEnumerator<TraversalPolicy> beginIterator;
        beginIterator.current = TraversalPolicy::start(root, beginIterator.path);
        return beginIterator;
    }

    template <typename TraversalPolicy>
        requires IsTraversalPolicy<TraversalPolicy, std::shared_ptr<Node>, Stack<std::shared_ptr<Node>> &>
    TreeEnumerator<TraversalPolicy> end() const
    {
        return TreeEnumerator<TraversalPolicy>();
    }
};

struct KlpStrategy
{
    template <typename NodePtr, typename StackRef>
    static NodePtr start(NodePtr root, StackRef &path)
    {
        return root;
    }

    template <typename NodePtr, typename StackRef>
    static NodePtr increment(NodePtr current, StackRef &path)
    {
        if (current)
        {
            if (current->getRight())
                path.Push(current->getRight());
            if (current->getLeft())
                return current->getLeft();
        }
        if (!path.IsEmpty())
        {
            auto nextNode = path.Top();
            path.Pop();
            return nextNode;
        }
        return nullptr;
    }
};

struct KplStrategy
{
    template <typename NodePtr, typename StackRef>
    static NodePtr start(NodePtr root, StackRef &path)
    {
        return root;
    }

    template <typename NodePtr, typename StackRef>
    static NodePtr increment(NodePtr current, StackRef &path)
    {
        if (current)
        {
            if (current->getLeft())
                path.Push(current->getLeft());
            if (current->getRight())
                return current->getRight();
        }
        if (!path.IsEmpty())
        {
            auto nextNode = path.Top();
            path.Pop();
            return nextNode;
        }
        return nullptr;
    }
};

struct LkpStrategy
{
    template <typename NodePtr, typename StackRef>
    static NodePtr start(NodePtr root, StackRef &path)
    {
        if (!root)
            return nullptr;
        auto current = root;
        while (current->getLeft())
        {
            path.Push(current);
            current = current->getLeft();
        }
        return current;
    }

    template <typename NodePtr, typename StackRef>
    static NodePtr increment(NodePtr current, StackRef &path)
    {
        if (!current)
            return current;
        if (current->getRight())
        {
            auto tempNode = current->getRight();
            path.Push(tempNode);
            while (tempNode->getLeft())
            {
                tempNode = tempNode->getLeft();
                path.Push(tempNode);
            }
        }
        if (!path.IsEmpty())
        {
            current = path.Top();
            path.Pop();
        }
        else
        {
            current = nullptr;
        }
        return current;
    }
};

struct PklStrategy
{
    template <typename NodePtr, typename StackRef>
    static NodePtr start(NodePtr root, StackRef &path)
    {
        if (!root)
            return nullptr;
        auto current = root;
        while (current->getRight())
        {
            path.Push(current);
            current = current->getRight();
        }
        return current;
    }

    template <typename NodePtr, typename StackRef>
    static NodePtr increment(NodePtr current, StackRef &path)
    {
        if (!current)
            return nullptr;
        if (current->getLeft())
        {
            auto tempNode = current->getLeft();
            path.Push(tempNode);
            while (tempNode->getRight())
            {
                tempNode = tempNode->getRight();
                path.Push(tempNode);
            }
        }
        if (!path.IsEmpty())
        {
            current = path.Top();
            path.Pop();
        }
        else
        {
            current = nullptr;
        }
        return current;
    }
};

struct LpkStrategy
{
    template <typename NodePtr, typename StackRef>
    static NodePtr start(NodePtr root, StackRef &path)
    {
        if (!root)
            return nullptr;
        auto current = root;
        while (current->getLeft() || current->getRight())
        {
            path.Push(current);
            if (current->getLeft())
                current = current->getLeft();
            else
                current = current->getRight();
        }
        return current;
    }

    template <typename NodePtr, typename StackRef>
    static NodePtr increment(NodePtr current, StackRef &path)
    {
        if (path.IsEmpty())
            return nullptr;
        auto parent = path.Top();
        if (parent->getRight() && parent->getRight() != current)
        {
            auto tempNode = parent->getRight();
            while (tempNode->getLeft() || tempNode->getRight())
            {
                path.Push(tempNode);
                if (tempNode->getLeft())
                    tempNode = tempNode->getLeft();
                else
                    tempNode = tempNode->getRight();
            }
            return tempNode;
        }
        else
        {
            path.Pop();
            return parent;
        }
    }
};

struct PlkStrategy
{
    template <typename NodePtr, typename StackRef>
    static NodePtr start(NodePtr root, StackRef &path)
    {
        if (!root)
            return nullptr;
        auto current = root;
        while (current->getRight() || current->getLeft())
        {
            path.Push(current);
            if (current->getRight())
                current = current->getRight();
            else
                current = current->getLeft();
        }
        return current;
    }

    template <typename NodePtr, typename StackRef>
    static NodePtr increment(NodePtr current, StackRef &path)
    {
        if (path.IsEmpty())
            return nullptr;
        auto parent = path.Top();
        if (parent->getLeft() && parent->getLeft() != current)
        {
            auto tempNode = parent->getLeft();
            while (tempNode->getRight() || tempNode->getLeft())
            {
                path.Push(tempNode);
                if (tempNode->getRight())
                    tempNode = tempNode->getRight();
                else
                    tempNode = tempNode->getLeft();
            }
            return tempNode;
        }
        else
        {
            path.Pop();
            return parent;
        }
    }
};