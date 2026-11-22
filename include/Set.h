#pragma once
#include <cstddef>
#include <ostream>
#include <sstream>
#include <string>
#include "BinaryTree.h"

template <typename T>
class Set
{
private:
    BinaryTree<T, T> tree;

    auto first() const { return tree.template begin<LkpStrategy>(); }
    auto last() const { return tree.template end<LkpStrategy>(); }

public:
    Set() = default;

    bool Add(const T &value)
    {
        if (tree.contains(value))
            return false;
        tree.insert(value, value);
        return true;
    }
    bool Remove(const T &value)
    {
        if (!tree.contains(value))
            return false;
        tree.remove(value);
        return true;
    }
    bool Contains(const T &value) const { return tree.contains(value); }

    std::size_t GetSize() const { return tree.GetSize(); }
    bool IsEmpty() const { return first() == last(); }

    Set Union(const Set &other) const
    {
        Set result = *this;
        for (auto it = other.first(); it != other.last(); ++it)
            result.Add(*it);
        return result;
    }

    Set Intersection(const Set &other) const
    {
        Set result;
        for (auto it = first(); it != last(); ++it)
            if (other.tree.contains(*it))
                result.Add(*it);
        return result;
    }

    Set Difference(const Set &other) const
    {
        Set result;
        for (auto it = first(); it != last(); ++it)
            if (!other.tree.contains(*it))
                result.Add(*it);
        return result;
    }

    bool IsSubsetOf(const Set &other) const
    {
        for (auto it = first(); it != last(); ++it)
            if (!other.tree.contains(*it))
                return false;
        return true;
    }

    bool Equals(const Set &other) const
    {
        return GetSize() == other.GetSize() && IsSubsetOf(other);
    }

    std::string ToString() const
    {
        std::ostringstream out;
        out << "{";
        bool firstItem = true;
        for (auto it = first(); it != last(); ++it)
        {
            if (!firstItem)
                out << ' ';
            out << *it;
            firstItem = false;
        }
        out << "}";
        return out.str();
    }
};

template <typename T>
Set<T> operator+(const Set<T> &a, const Set<T> &b) { return a.Union(b); }
template <typename T>
Set<T> operator*(const Set<T> &a, const Set<T> &b) { return a.Intersection(b); }
template <typename T>
Set<T> operator-(const Set<T> &a, const Set<T> &b) { return a.Difference(b); }
template <typename T>
bool operator==(const Set<T> &a, const Set<T> &b) { return a.Equals(b); }
template <typename T>
bool operator!=(const Set<T> &a, const Set<T> &b) { return !a.Equals(b); }
template <typename T>
bool operator<=(const Set<T> &a, const Set<T> &b) { return a.IsSubsetOf(b); }
template <typename T>
std::ostream &operator<<(std::ostream &out, const Set<T> &set) { return out << set.ToString(); }
