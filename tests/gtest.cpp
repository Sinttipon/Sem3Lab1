#include <gtest/gtest.h>
#include <initializer_list>
#include <sstream>
#include <string>
#include <utility>
#include "BinaryTree.h"
#include "Set.h"
#include "Stack.h"
#include "TreeFormatter.h"
#include "Types.h"
#include "exceptions.hpp"

namespace
{
    template <typename TKey, typename TValue>
    std::string ToStringLKP(BinaryTree<TKey, TValue> &tree)
    {
        std::string result;
        tree.Traverse(LKPrec<TKey, TValue>, [&](const TValue &v)
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
        tree.Traverse(KLPrec<TKey, TValue>, [&](const TValue &v)
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
            tree.Insert(v, v);
        return tree;
    }

    Set<int> MakeSet(std::initializer_list<int> values)
    {
        Set<int> set;
        for (int v : values)
            set.Add(v);
        return set;
    }
}

TEST(TreeTest, EmptyTree)
{
    BinaryTree<int, int> tree;
    EXPECT_TRUE(tree.IsEmpty());
    EXPECT_EQ(tree.GetSize(), 0u);
    EXPECT_EQ(tree.GetHeight(), 0);
    EXPECT_FALSE(tree.Search(1));
    EXPECT_EQ(ToStringLKP(tree), "");
}

TEST(TreeTest, InsertContains)
{
    BinaryTree<int, int> tree = MakeTree({50, 30, 70, 20, 40});
    EXPECT_EQ(tree.GetSize(), 5u);
    EXPECT_TRUE(tree.Search(50));
    EXPECT_TRUE(tree.Search(20));
    EXPECT_FALSE(tree.Search(45));
    EXPECT_THROW(tree.Insert(30, 30), InvalidArgument);
    EXPECT_EQ(tree.GetSize(), 5u);
}

TEST(TreeTest, Traversals)
{
    BinaryTree<int, int> tree = MakeTree({4, 2, 6, 1, 3, 5, 7});
    EXPECT_EQ(ToStringKLP(tree), "4 2 1 3 6 5 7");
    EXPECT_EQ(ToStringLKP(tree), "1 2 3 4 5 6 7");

    std::string lpk;
    tree.Traverse(LPKrec<int, int>, [&](const int &v)
                  { lpk += std::to_string(v) + ' '; });
    EXPECT_EQ(lpk, "1 3 2 5 7 6 4 ");
}

TEST(TreeTest, RemoveCases)
{
    BinaryTree<int, int> tree = MakeTree({4, 2, 6, 1, 3, 5, 7});
    EXPECT_NO_THROW(tree.Remove(1));
    EXPECT_EQ(ToStringLKP(tree), "2 3 4 5 6 7");
    EXPECT_NO_THROW(tree.Remove(2));
    EXPECT_EQ(ToStringLKP(tree), "3 4 5 6 7");
    EXPECT_NO_THROW(tree.Remove(4));
    EXPECT_EQ(ToStringLKP(tree), "3 5 6 7");
    EXPECT_EQ(tree.GetSize(), 4u);
    EXPECT_THROW(tree.Remove(100), InvalidArgument);
}

TEST(TreeTest, RemoveAll)
{
    BinaryTree<int, int> tree = MakeTree({5, 3, 8, 1, 4});
    for (int v : {3, 5, 1, 8, 4})
        EXPECT_NO_THROW(tree.Remove(v));
    EXPECT_TRUE(tree.IsEmpty());
    EXPECT_EQ(tree.GetHeight(), 0);
    EXPECT_THROW(tree.Remove(5), InvalidArgument);
}

TEST(TreeTest, BalanceSorted)
{
    BinaryTree<int, int> tree;
    for (int i = 1; i <= 1023; i++)
        tree.Insert(i, i);
    EXPECT_LE(tree.GetHeight(), 11);
    for (int i = 1; i <= 1023; i += 2)
        tree.Remove(i);
    EXPECT_LE(tree.GetHeight(), 11);
    EXPECT_EQ(tree.GetSize(), 511u);
}

TEST(TreeTest, BalanceRandom)
{
    const int range = 1000;
    bool present[range] = {};
    BinaryTree<int, int> tree;
    unsigned seed = 12345;
    for (int step = 0; step < 20000; step++)
    {
        seed = seed * 1103515245 + 12345;
        int v = static_cast<int>((seed >> 8) % range);
        if (present[v])
            EXPECT_NO_THROW(tree.Remove(v));
        else
            EXPECT_NO_THROW(tree.Insert(v, v));
        present[v] = !present[v];
    }
    for (int v = 0; v < range; v++)
    {
        EXPECT_EQ(tree.Search(v), present[v]);
    }
}

TEST(TreeTest, CopyAndAssign)
{
    BinaryTree<int, int> a = MakeTree({2, 1, 3});
    BinaryTree<int, int> b = a;
    b.Insert(4, 4);
    EXPECT_FALSE(a.Search(4));
    EXPECT_TRUE(b.Search(4));
    EXPECT_EQ(ToStringLKP(a), "1 2 3");
    EXPECT_EQ(ToStringLKP(b), "1 2 3 4");

    BinaryTree<int, int> c;
    c = a;
    EXPECT_EQ(ToStringLKP(c), ToStringLKP(a));
    c.Remove(1);
    EXPECT_TRUE(a.Search(1));

    BinaryTree<int, int> &self = a;
    a = self;
    EXPECT_EQ(a.GetSize(), 3u);
}

TEST(TreeTest, Move)
{
    BinaryTree<int, int> a = MakeTree({2, 1, 3});
    BinaryTree<int, int> b = std::move(a);
    EXPECT_EQ(ToStringLKP(b), "1 2 3");
    EXPECT_TRUE(a.IsEmpty());

    BinaryTree<int, int> c = MakeTree({10});
    c = std::move(b);
    EXPECT_EQ(ToStringLKP(c), "1 2 3");
    EXPECT_TRUE(b.IsEmpty());

    a.Insert(5, 5);
    EXPECT_EQ(a.GetSize(), 1u);
}

TEST(TreeTest, Clear)
{
    BinaryTree<int, int> tree = MakeTree({1, 2, 3});
    tree = BinaryTree<int, int>();
    EXPECT_TRUE(tree.IsEmpty());
    tree.Insert(10, 10);
    EXPECT_EQ(tree.GetSize(), 1u);
}

TEST(TreeTest, MapWhereReduce)
{
    BinaryTree<int, int> tree = MakeTree({1, 2, 3, 4, 5});

    BinaryTree<int, int> squared;
    tree.Traverse(LKPrec<int, int>, [&](const int &x)
                  { squared.Insert(x * x, x * x); });
    EXPECT_EQ(ToStringLKP(squared), "1 4 9 16 25");

    BinaryTree<int, int> filtered;
    tree.Traverse(LKPrec<int, int>, [&](const int &x)
                  {
        if (x > 2) filtered.Insert(x, x); });
    EXPECT_EQ(ToStringLKP(filtered), "3 4 5");

    int sum = 0;
    tree.Traverse(LKPrec<int, int>, [&](const int &x)
                  { sum += x; });
    EXPECT_EQ(sum, 15);
}

TEST(TreeTest, Strings)
{
    BinaryTree<int, int> tree = MakeTree({4, 2, 6, 1, 3});

    // ВАЖНО: выражения с запятыми внутри макросов нужно оборачивать в скобки!
    EXPECT_EQ((TreeFormatter<int, int>("{K}(L)[P]").Format(tree)), "{4}({2}({1}()[])[{3}()[]])[{6}()[]]");
    EXPECT_EQ((TreeFormatter<int, int>("K ").Format(tree)), "4 ");

    std::string text = ToStringLKP(tree);
    std::istringstream in(text);
    BinaryTree<int, int> loaded;
    int v;
    while (in >> v)
        loaded.Insert(v, v);
    EXPECT_EQ(ToStringLKP(loaded), ToStringLKP(tree));

    EXPECT_TRUE((BinaryTree<int, int>().IsEmpty()));
}

TEST(SetTest, Operations)
{
    Set<int> a = MakeSet({1, 2, 3, 4});
    Set<int> b = MakeSet({3, 4, 5});
    Set<int> empty;
    EXPECT_EQ(a + b, MakeSet({1, 2, 3, 4, 5}));
    EXPECT_EQ(a * b, MakeSet({3, 4}));
    EXPECT_EQ(a - b, MakeSet({1, 2}));
    EXPECT_EQ(b - a, MakeSet({5}));
    EXPECT_EQ(a + empty, a);
    EXPECT_EQ(a * empty, empty);
    EXPECT_EQ(a - a, empty);
    EXPECT_EQ(a * b, b * a);
    EXPECT_EQ(MakeSet({1, 2}) + MakeSet({2, 1}), MakeSet({1, 2}));
}

TEST(SetTest, Relations)
{
    Set<int> a = MakeSet({1, 2, 3});
    EXPECT_TRUE(MakeSet({1, 3}) <= a);
    EXPECT_FALSE(MakeSet({1, 4}) <= a);
    EXPECT_TRUE(Set<int>() <= a);
    EXPECT_TRUE(a <= a);
    EXPECT_EQ(a, MakeSet({3, 2, 1}));
    EXPECT_NE(a, MakeSet({1, 2}));
    Set<int> b = a;
    EXPECT_FALSE(b.Add(2));
    EXPECT_EQ(b.GetSize(), 3u);
    EXPECT_FALSE(b.Remove(10));
    EXPECT_TRUE(b.Remove(1));
    EXPECT_TRUE(a.Contains(1));
}

TEST(SetTest, Print)
{
    std::ostringstream out;
    out << MakeSet({3, 1, 2});
    EXPECT_EQ(out.str(), "{1 2 3}");
    EXPECT_EQ(Set<int>().ToString(), "{}");
}

TEST(TypesTest, StringsAndDoubles)
{
    BinaryTree<std::string, std::string> strTree;
    for (const auto &s : {"pear", "apple", "melon"})
        strTree.Insert(s, s);
    EXPECT_EQ(ToStringLKP(strTree), "apple melon pear");

    BinaryTree<double, double> dblTree;
    // ВАЖНО: -1 это int, нужно -1.0 чтобы вывод типов сработал
    for (double v : {2.5, -1.0, 0.75})
        dblTree.Insert(v, v);
    EXPECT_EQ(ToStringLKP(dblTree), "-1 0.75 2.5");
}

TEST(TypesTest, Complex)
{
    BinaryTree<Complex, Complex> nums;
    for (const auto &c : {Complex(1, 2), Complex(1, -1), Complex(0, 5)})
        nums.Insert(c, c);
    EXPECT_EQ(ToStringLKP(nums), "0,5 1,-1 1,2");

    Complex z(7, 7);
    std::istringstream bad("3;4");
    EXPECT_FALSE(bad >> z);
    EXPECT_EQ(z.re, 7);
    EXPECT_EQ(z.im, 7);
}

TEST(TypesTest, Student)
{
    BinaryTree<int, Student> group;
    group.Insert(3, Student(3, "Ivanov", "Ivan", 2005));
    group.Insert(1, Student(1, "Petrov", "Petr", 2004));
    EXPECT_TRUE(group.Search(3));
    EXPECT_EQ(ToStringLKP(group), "1;Petrov;Petr;2004 3;Ivanov;Ivan;2005");

    Student s(9, "Old", "Name", 1999);
    std::istringstream bad("1;Petrov");
    EXPECT_FALSE(bad >> s);
    EXPECT_EQ(s.lastName, "Old");
}

TEST(StackTest, PushPopCopy)
{
    Stack<int> s;
    EXPECT_TRUE(s.IsEmpty());
    EXPECT_THROW(s.Top(), EmptyStructureException);
    EXPECT_THROW(s.Pop(), EmptyStructureException);
    for (int i = 1; i <= 3; i++)
        s.Push(i);
    Stack<int> copy = s;
    EXPECT_EQ(s.Top(), 3);
    s.Pop();
    EXPECT_EQ(s.Top(), 2);
    EXPECT_EQ(s.GetSize(), 2u);
    EXPECT_EQ(copy.Top(), 3);
    EXPECT_EQ(copy.GetSize(), 3u);
    Stack<int> moved = std::move(copy);
    EXPECT_EQ(moved.Top(), 3);
    EXPECT_TRUE(copy.IsEmpty());
    s = moved;
    EXPECT_EQ(s.GetSize(), 3u);
}

TEST(IteratorTest, InOrder)
{
    BinaryTree<int, int> tree = MakeTree({4, 2, 6, 1, 3, 5, 7});
    std::ostringstream text;
    for (int v : tree)
        text << v << ' ';
    EXPECT_EQ(text.str(), "1 2 3 4 5 6 7 ");

    BinaryTree<int, int> empty;
    EXPECT_TRUE(empty.begin() == empty.end());
    EXPECT_THROW(*empty.begin(), EndOfStreamException);

    BinaryTree<int, int> big;
    for (int i = 1000; i > 0; i--)
        big.Insert(i, i);
    int expected = 1;
    for (int v : big)
        EXPECT_EQ(v, expected++);
    EXPECT_EQ(expected, 1001);
}

TEST(IteratorTest, CopyAndPostIncrement)
{
    BinaryTree<int, int> tree = MakeTree({2, 1, 3});
    BinaryTree<int, int>::TreeEnumerator it = tree.begin();
    BinaryTree<int, int>::TreeEnumerator old = it++;
    EXPECT_EQ(*old, 1);
    EXPECT_EQ(*it, 2);
    ++old;
    EXPECT_EQ(*old, 2);
    EXPECT_TRUE(old == it);
    ++it;
    ++it;
    EXPECT_TRUE(it == tree.end());
    ++it;
    EXPECT_TRUE(it == tree.end());
}

TEST(IteratorTest, Students)
{
    BinaryTree<int, Student> group;
    group.Insert(3, Student(3, "Ivanov", "Ivan", 2005));
    group.Insert(1, Student(1, "Petrov", "Petr", 2004));
    auto it = group.begin();
    // ВАЖНО: operator-> возвращает Node*, поэтому используем (*it).
    EXPECT_EQ((*it).lastName, "Petrov");
}

TEST(TreeTest, TraverseWithAction)
{
    BinaryTree<int, int> tree = MakeTree({4, 2, 6, 1, 3, 5, 7});
    std::ostringstream out;
    tree.Traverse(LPKrec<int, int>, [&](const int &value)
                  { out << value << ' '; });
    EXPECT_EQ(out.str(), "1 3 2 5 7 6 4 ");

    int sum = 0;
    tree.Traverse(PKLrec<int, int>, [&](const int &value)
                  { sum += value; });
    EXPECT_EQ(sum, 28);
}