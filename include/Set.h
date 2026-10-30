#pragma once
#include <cstddef>
#include <ostream>
#include "BinaryTree.h"
#include <string>

template <typename T>
class Set
{
private:
    BinaryTree<T, T> tree;
    Set(const BinaryTree<T, T> &t) : tree(t) {}

public:
    Set() = default;

    bool Add(const T &value)
    {
        try
        {
            tree.Insert(value, value);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool Remove(const T &value)
    {
        try
        {
            tree.Remove(value);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool Contains(const T &value) const { return tree.Search(value); }
    std::size_t GetSize() const { return tree.GetSize(); }
    bool IsEmpty() const { return tree.IsEmpty(); }

    Set Union(const Set &other) const
    {
        Set result = *this;
        other.tree.Traverse(LKPrec<T, T>, [&](const T &value)
                            {
            try { result.tree.Insert(value, value); } catch (...) {} });
        return result;
    }

    Set Intersection(const Set &other) const
    {
        Set result;
        tree.Traverse(LKPrec<T, T>, [&](const T &value)
                      {
            if (other.tree.Search(value)) {
                try { result.tree.Insert(value, value); } catch (...) {}
            } });
        return result;
    }

    Set Difference(const Set &other) const
    {
        Set result;
        tree.Traverse(LKPrec<T, T>, [&](const T &value)
                      {
            if (!other.tree.Search(value)) {
                try { result.tree.Insert(value, value); } catch (...) {}
            } });
        return result;
    }

    bool IsSubsetOf(const Set &other) const
    {
        if (GetSize() > other.GetSize())
            return false;
        std::size_t found = 0;
        tree.Traverse(LKPrec<T, T>, [&](const T &value)
                      {
            if (other.tree.Search(value)) found++; });
        return found == GetSize();
    }

    bool Equals(const Set &other) const { return GetSize() == other.GetSize() && IsSubsetOf(other); }

    std::string ToString() const
    {
        std::ostringstream out;
        out << "{";
        bool first = true;
        tree.Traverse(LKPrec<T, T>, [&](const T &value)
                      {
            if (!first) out << ' ';
            out << value;
            first = false; });
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