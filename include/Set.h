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

public:
    Set() = default;

    bool Add(const T &value) { return tree.insert(value, value); }
    bool Remove(const T &value) { return tree.remove(value); }
    bool Contains(const T &value) const { return tree.contains(value); }

    std::size_t GetSize() const
    {
        std::size_t n = 0;
        for (auto it = tree.begin(); it != tree.end(); ++it)
            ++n;
        return n;
    }
    bool IsEmpty() const { return tree.begin() == tree.end(); }

    Set Union(const Set &other) const
    {
        Set result = *this;
        for (auto it = other.tree.begin(); it != other.tree.end(); ++it)
            result.tree.insert(*it, *it);
        return result;
    }

    Set Intersection(const Set &other) const
    {
        Set result;
        for (auto it = tree.begin(); it != tree.end(); ++it)
            if (other.tree.contains(*it))
                result.tree.insert(*it, *it);
        return result;
    }

    Set Difference(const Set &other) const
    {
        Set result;
        for (auto it = tree.begin(); it != tree.end(); ++it)
            if (!other.tree.contains(*it))
                result.tree.insert(*it, *it);
        return result;
    }

    bool IsSubsetOf(const Set &other) const
    {
        if (GetSize() > other.GetSize())
            return false;
        for (auto it = tree.begin(); it != tree.end(); ++it)
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
        bool first = true;
        for (auto it = tree.begin(); it != tree.end(); ++it)
        {
            if (!first)
                out << ' ';
            out << *it;
            first = false;
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