#pragma once
#include <ostream>
#include "BinaryTree.h"

template <typename T>
class Set {
private:
    BinaryTree<T> tree;

    Set(const BinaryTree<T>& t) : tree(t) {}

public:
    Set() {};

    void Add(const T& value) {
        if (!tree.Contains(value)) {
            tree.Insert(value);
        }
    }

    void Remove(const T& value) {tree.Remove(value);}

    bool Contains(const T& value) const {return tree.Contains(value);}

    size_t GetSize() const {return tree.GetSize();}

    bool IsEmpty() const {return tree.IsEmpty();}

    Set Union(const Set& other) const {
        Set result = *this;
        other.tree.Traverse("LKP",[&result](const T& value) {
            result.Add(value);
        });
        return result;
    }

    Set Intersection(const Set& other) const {
        return Set(tree.Where([&other](const T& value) {
            return other.Contains(value);
        }));
    }

    Set Difference(const Set& other) const {
        return Set(tree.Where([&other](const T& value) {
            return !other.Contains(value);
        }));
    }

    bool IsSubsetOf(const Set& other) const {
        if (GetSize() > other.GetSize()) return false;
        bool result = true;
        tree.Traverse("LKP", [&result, &other](const T& value) {
            if (!other.Contains(value)) result = false;
        });
        return result;
    }

    bool Equals(const Set& other) const {
        return GetSize() == other.GetSize() && IsSubsetOf(other);
    }


    Set Map(const std::function<T(const T&)>& f) const {return Set(tree.Map(f));}

    Set Where(const std::function<bool(const T&)>& predicate) const {
        return Set(tree.Where(predicate));
    }

    std::string ToString() const {return "{" + tree.ToString("LKP") + "}"; }


    Set operator+(const Set& other) const {return Union(other);}
    Set operator*(const Set& other) const {return Intersection(other);}
    Set operator-(const Set& other) const {return Difference(other);}
    bool operator==(const Set& other) const {return Equals(other);}
    bool operator!=(const Set &other) const { return !Equals(other);}
    bool operator<=(const Set &other) const { return IsSubsetOf(other);}

    friend std::ostream& operator<<(std::ostream& out, const Set& set) {
        return out << set.ToString();
    }
};