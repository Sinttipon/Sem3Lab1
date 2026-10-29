#pragma once
#include <cstddef>
#include "exceptions.hpp"
#include <utility>

template <typename T>
class Stack {
private:
    struct Item {
        T value;
        Item* next;

        Item(const T& v, Item* n) : value(v), next(n) {}
    };

    Item* top;
    std::size_t size;

    Item* CopyItems(const Item* item) {
        if (item == nullptr) return nullptr;
        Item* next = CopyItems(item->next);
        try {
            return new Item(item->value, next);
        } catch (...) {
            DeleteItems(next);
            throw;
        }
    }

    void DeleteItems(Item* item) {
        while (item != nullptr) {
            Item* next = item->next;
            delete item;
            item = next;
        }
    }

    void Swap(Stack& other) noexcept {
        std::swap(top, other.top);
        std::swap(size, other.size);
    }

public:
    Stack() : top(nullptr), size(0) {}

    Stack(const Stack& other) : top(nullptr), size(other.size) { top = CopyItems(other.top); }

    Stack(Stack&& other) noexcept : top(other.top), size(other.size) {
        other.top = nullptr;
        other.size = 0;
    }

    Stack& operator=(Stack other) noexcept {
        Swap(other);
        return *this;
    }

    ~Stack() { DeleteItems(top); }

    bool IsEmpty() const { return top == nullptr; }
    std::size_t GetSize() const { return size; }

    void Push(const T& value) {
        top = new Item(value, top);
        size++;
    }

    const T& Top() const {
        if (top == nullptr) throw EmptyStructureException("Stack::Top");
        return top->value;
    }

    void Pop() {
        if (top == nullptr) throw EmptyStructureException("Stack::Pop");
        Item* next = top->next;
        delete top;
        top = next;
        size--;
    }
};
