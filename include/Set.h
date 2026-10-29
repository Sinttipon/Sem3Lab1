#pragma once
#include <cstddef>
#include <ostream>
#include "BinaryTree.h"
#include <string>

template <typename T>
class Set {
private:
    BinaryTree<T> tree;

    Set(const BinaryTree<T>& t) : tree(t) {}

public:
    Set() = default;

    bool Add(const T& value) { return tree.Insert(value); }
    bool Remove(const T& value) { return tree.Remove(value); }
    bool Contains(const T& value) const { return tree.Contains(value); }
    std::size_t GetSize() const { return tree.GetSize(); }
    bool IsEmpty() const { return tree.IsEmpty(); }

    Set Union(const Set& other) const {
        Set result = *this;
        for (const T& value : other.tree) result.Add(value);
        return result;
    }

    Set Intersection(const Set& other) const {
        return Set(tree.Where([&](const T& value) { return other.Contains(value); }));
    }

    Set Difference(const Set& other) const {
        return Set(tree.Where([&](const T& value) { return !other.Contains(value); }));
    }

    bool IsSubsetOf(const Set& other) const {
        if (GetSize() > other.GetSize()) return false;
        std::size_t found = 0;
        for (const T& value : tree) {
            if (other.Contains(value)) found++;
        }
        return found == GetSize();
    }

    bool Equals(const Set& other) const { return GetSize() == other.GetSize() && IsSubsetOf(other); }

    std::string ToString() const { return "{" + tree.ToString(Order::LKP) + "}"; }
};

template <typename T>
Set<T> operator+(const Set<T>& a, const Set<T>& b) { return a.Union(b); }

template <typename T>
Set<T> operator*(const Set<T>& a, const Set<T>& b) { return a.Intersection(b); }

template <typename T>
Set<T> operator-(const Set<T>& a, const Set<T>& b) { return a.Difference(b); }

template <typename T>
bool operator==(const Set<T>& a, const Set<T>& b) { return a.Equals(b); }

template <typename T>
bool operator!=(const Set<T>& a, const Set<T>& b) { return !a.Equals(b); }

template <typename T>
bool operator<=(const Set<T>& a, const Set<T>& b) { return a.IsSubsetOf(b); }

template <typename T>
std::ostream& operator<<(std::ostream& out, const Set<T>& set) { return out << set.ToString(); }
