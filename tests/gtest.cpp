#include <gtest/gtest.h>
#include <initializer_list>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <random>
#include "BinaryTree.h"
#include "Set.h"
#include "Stack.h"
#include "Types.h"
#include "exceptions.hpp"

namespace
{
    template <typename TKey, typename TValue>
    std::string ToStringLKP(BinaryTree<TKey, TValue> &tree)
    {
        std::string result;
        tree.template traverse<LkpStrategy>([&](const TValue &v)
                                            {
            std::ostringstream oss; oss << v;
            if (!result.empty()) result += ' ';
            result += oss.str(); });
        return result;
    }

    template <typename TKey, typename TValue>
    std::string ToStringKLP(BinaryTree<TKey, TValue> &tree)
    {
        std::string result;
        tree.template traverse<KlpStrategy>([&](const TValue &v)
                                            {
            std::ostringstream oss; oss << v;
            if (!result.empty()) result += ' ';
            result += oss.str(); });
        return result;
    }

    BinaryTree<int, int> MakeTree(std::initializer_list<int> values)
    {
        BinaryTree<int, int> tree;
        for (int v : values)
            tree.insert(v, v);
        return tree;
    }

    template <typename TValue>
    void SortByKey(std::vector<TValue> &values,
                   const std::function<int(const TValue &)> &getKey)
    {
        std::sort(values.begin(), values.end(),
                  [&](const TValue &a, const TValue &b)
                  { return getKey(a) < getKey(b); });
    }

    template <typename TValue>
    std::vector<TValue> MedianOrder(const std::vector<TValue> &sorted)
    {
        std::vector<TValue> result;
        result.reserve(sorted.size());
        std::function<void(std::size_t, std::size_t)> build =
            [&](std::size_t lo, std::size_t hi)
        {
            if (lo >= hi)
                return;
            std::size_t mid = lo + (hi - lo) / 2;
            result.push_back(sorted[mid]);
            build(lo, mid);
            build(mid + 1, hi);
        };
        build(0, sorted.size());
        return result;
    }

    template <typename TKey, typename TValue>
    int InsertSorted(BinaryTree<TKey, TValue> &tree,
                     std::vector<TValue> values,
                     const std::function<TKey(const TValue &)> &getKey)
    {
        SortByKey(values, [&](const TValue &v) -> int
                  { return static_cast<int>(getKey(v)); });
        int inserted = 0;
        for (const auto &v : values)
        {
            try
            {
                tree.insert(getKey(v), v);
                inserted++;
            }
            catch (...)
            {
            }
        }
        return inserted;
    }

    template <typename TKey, typename TValue>
    int InsertMedian(BinaryTree<TKey, TValue> &tree,
                     std::vector<TValue> values,
                     const std::function<TKey(const TValue &)> &getKey)
    {
        SortByKey(values, [&](const TValue &v) -> int
                  { return static_cast<int>(getKey(v)); });
        auto ordered = MedianOrder(values);
        int inserted = 0;
        for (const auto &v : ordered)
        {
            try
            {
                tree.insert(getKey(v), v);
                inserted++;
            }
            catch (...)
            {
            }
        }
        return inserted;
    }
}


TEST(SortedInsertTest, SortedAscending)
{
    BinaryTree<int, int> tree;
    std::vector<int> values = {5, 3, 8, 1, 4, 7, 9, 2, 6};
    int added = InsertSorted<int, int>(tree, values, [](const int &v)
                                       { return v; });
    EXPECT_EQ(added, 9);
    EXPECT_EQ(ToStringLKP(tree), "1 2 3 4 5 6 7 8 9");
}

TEST(SortedInsertTest, SortedWithDuplicates)
{
    BinaryTree<int, int> tree;
    std::vector<int> values = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    int added = InsertSorted<int, int>(tree, values, [](const int &v)
                                       { return v; });
    EXPECT_EQ(added, 7);
    EXPECT_EQ(ToStringLKP(tree), "1 2 3 4 5 6 9");
}

TEST(SortedInsertTest, SortedEmpty)
{
    BinaryTree<int, int> tree;
    std::vector<int> values;
    int added = InsertSorted<int, int>(tree, values, [](const int &v)
                                       { return v; });
    EXPECT_EQ(added, 0);
    EXPECT_TRUE(tree.begin<LkpStrategy>() == tree.end<LkpStrategy>());
}

TEST(SortedInsertTest, SortedMaintainsBalanceAndIterator)
{
    BinaryTree<int, int> tree;
    std::vector<int> values;
    for (int i = 1; i <= 200; i++)
        values.push_back(i);

    int added = InsertSorted<int, int>(tree, values, [](const int &v)
                                       { return v; });
    EXPECT_EQ(added, 200);
    EXPECT_EQ(tree.GetSize(), 200u);

    int expected = 1;
    for (auto it = tree.begin<LkpStrategy>(); it != tree.end<LkpStrategy>(); ++it)
    {
        EXPECT_EQ(*it, expected++);
    }
    EXPECT_EQ(expected, 201);
}

TEST(SortedInsertTest, ContainsAndRemove)
{
    BinaryTree<int, int> tree = MakeTree({10, 20, 30, 40, 50});
    EXPECT_TRUE(tree.contains(30));
    EXPECT_FALSE(tree.contains(99));

    tree.remove(30);
    EXPECT_FALSE(tree.contains(30));
    EXPECT_EQ(tree.GetSize(), 4u);
}

TEST(MedianInsertTest, MedianBalanced)
{
    BinaryTree<int, int> tree;
    std::vector<int> values;
    for (int i = 1; i <= 100; i++)
        values.push_back(i);

    int added = InsertMedian<int, int>(tree, values, [](const int &v)
                                       { return v; });
    EXPECT_EQ(added, 100);
    EXPECT_EQ(tree.GetSize(), 100u);

    int expected = 1;
    for (auto it = tree.begin<LkpStrategy>(); it != tree.end<LkpStrategy>(); ++it)
        EXPECT_EQ(*it, expected++);
}

TEST(SortedInsertTest, StringKeysAndStrategies)
{
    BinaryTree<std::string, std::string> tree;
    std::vector<std::string> values = {"pear", "apple", "zebra", "melon", "banana", "cherry"};
    int added = InsertSorted<std::string, std::string>(tree, values, [](const std::string &s)
                                                       { return s; });
    EXPECT_EQ(added, 6);

    std::string lkpResult;
    tree.traverse<LkpStrategy>([&](const std::string &v)
                               {
        if (!lkpResult.empty()) lkpResult += " ";
        lkpResult += v; });
    EXPECT_EQ(lkpResult, "apple banana cherry melon pear zebra");
}