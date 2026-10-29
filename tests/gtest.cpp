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

namespace {

template <typename T>
BinaryTree<T> Load(const std::string& text) {
    BinaryTree<T> tree;
    tree.LoadFromString(text);
    return tree;
}

BinaryTree<int> MakeTree(std::initializer_list<int> values) {
    BinaryTree<int> tree;
    for (int v : values) tree.Insert(v);
    return tree;
}

Set<int> MakeSet(std::initializer_list<int> values) {
    Set<int> set;
    for (int v : values) set.Add(v);
    return set;
}

}

TEST(TreeTest, EmptyTree) {
    BinaryTree<int> tree;
    EXPECT_TRUE(tree.IsEmpty());
    EXPECT_EQ(tree.GetSize(), 0u);
    EXPECT_EQ(tree.GetHeight(), 0);
    EXPECT_FALSE(tree.Contains(1));
    EXPECT_EQ(tree.ToString(), "");
    EXPECT_TRUE(tree.IsBalanced());
}

TEST(TreeTest, InsertContains) {
    BinaryTree<int> tree = MakeTree({50, 30, 70, 20, 40});
    EXPECT_EQ(tree.GetSize(), 5u);
    EXPECT_TRUE(tree.Contains(50));
    EXPECT_TRUE(tree.Contains(20));
    EXPECT_FALSE(tree.Contains(45));
    EXPECT_FALSE(tree.Insert(30));
    EXPECT_EQ(tree.GetSize(), 5u);
}

TEST(TreeTest, Traversals) {
    BinaryTree<int> tree = MakeTree({4, 2, 6, 1, 3, 5, 7});
    EXPECT_EQ(tree.ToString(Order::KLP), "4 2 1 3 6 5 7");
    EXPECT_EQ(tree.ToString(Order::KPL), "4 6 7 5 2 3 1");
    EXPECT_EQ(tree.ToString(Order::LKP), "1 2 3 4 5 6 7");
    EXPECT_EQ(tree.ToString(Order::LPK), "1 3 2 5 7 6 4");
    EXPECT_EQ(tree.ToString(Order::PLK), "7 5 6 3 1 2 4");
    EXPECT_EQ(tree.ToString(Order::PKL), "7 6 5 4 3 2 1");
}

TEST(TreeTest, RemoveCases) {
    BinaryTree<int> tree = MakeTree({4, 2, 6, 1, 3, 5, 7});
    EXPECT_TRUE(tree.Remove(1));
    EXPECT_EQ(tree.ToString(Order::LKP), "2 3 4 5 6 7");
    EXPECT_TRUE(tree.Remove(2));
    EXPECT_EQ(tree.ToString(Order::LKP), "3 4 5 6 7");
    EXPECT_TRUE(tree.Remove(4));
    EXPECT_EQ(tree.ToString(Order::LKP), "3 5 6 7");
    EXPECT_EQ(tree.GetSize(), 4u);
    EXPECT_TRUE(tree.IsBalanced());
    EXPECT_FALSE(tree.Remove(100));
    EXPECT_EQ(tree.GetSize(), 4u);
}

TEST(TreeTest, RemoveAll) {
    BinaryTree<int> tree = MakeTree({5, 3, 8, 1, 4});
    for (int v : {3, 5, 1, 8, 4}) EXPECT_TRUE(tree.Remove(v));
    EXPECT_TRUE(tree.IsEmpty());
    EXPECT_EQ(tree.GetHeight(), 0);
    EXPECT_FALSE(tree.Remove(5));
}

TEST(TreeTest, BalanceSorted) {
    BinaryTree<int> tree;
    for (int i = 1; i <= 1023; i++) tree.Insert(i);
    EXPECT_EQ(tree.GetHeight(), 10);
    EXPECT_TRUE(tree.IsBalanced());
    for (int i = 1; i <= 1023; i += 2) tree.Remove(i);
    EXPECT_TRUE(tree.IsBalanced());
    EXPECT_EQ(tree.GetSize(), 511u);
}

TEST(TreeTest, BalanceRandom) {
    const int range = 1000;
    bool present[range] = {};
    BinaryTree<int> tree;
    unsigned seed = 12345;
    for (int step = 0; step < 20000; step++) {
        seed = seed * 1103515245 + 12345;
        int v = static_cast<int>((seed >> 8) % range);
        if (present[v]) EXPECT_TRUE(tree.Remove(v));
        else EXPECT_TRUE(tree.Insert(v));
        present[v] = !present[v];
        if (step % 500 == 0) {
            ASSERT_TRUE(tree.IsBalanced()) << "step " << step;
        }
    }
    std::size_t expected = 0;
    for (int v = 0; v < range; v++) {
        if (present[v]) expected++;
        EXPECT_EQ(tree.Contains(v), present[v]);
    }
    EXPECT_EQ(tree.GetSize(), expected);
}

TEST(TreeTest, CopyAndAssign) {
    BinaryTree<int> a = MakeTree({2, 1, 3});
    BinaryTree<int> b = a;
    b.Insert(4);
    EXPECT_FALSE(a.Contains(4));
    EXPECT_TRUE(b.Contains(4));
    EXPECT_TRUE(a == MakeTree({2, 1, 3}));
    EXPECT_TRUE(a != b);
    BinaryTree<int> c;
    c = a;
    EXPECT_TRUE(c == a);
    c.Remove(1);
    EXPECT_TRUE(a.Contains(1));
    BinaryTree<int>& self = a;
    a = self;
    EXPECT_EQ(a.GetSize(), 3u);
}

TEST(TreeTest, Move) {
    BinaryTree<int> a = MakeTree({2, 1, 3});
    BinaryTree<int> b = std::move(a);
    EXPECT_EQ(b.ToString(Order::LKP), "1 2 3");
    EXPECT_TRUE(a.IsEmpty());
    BinaryTree<int> c = MakeTree({10});
    c = std::move(b);
    EXPECT_EQ(c.ToString(Order::LKP), "1 2 3");
    EXPECT_TRUE(b.IsEmpty());
    a.Insert(5);
    EXPECT_EQ(a.GetSize(), 1u);
}

TEST(TreeTest, Equality) {
    EXPECT_TRUE(MakeTree({2, 1, 3}) == MakeTree({2, 3, 1}));
    EXPECT_TRUE(BinaryTree<int>() == BinaryTree<int>());
    EXPECT_TRUE(MakeTree({1, 2}) != MakeTree({2, 1}));
    EXPECT_TRUE(MakeTree({1, 2}) != MakeTree({1, 2, 3}));
}

TEST(TreeTest, Clear) {
    BinaryTree<int> tree = MakeTree({1, 2, 3});
    tree.Clear();
    EXPECT_TRUE(tree.IsEmpty());
    tree.Insert(10);
    EXPECT_EQ(tree.GetSize(), 1u);
}

TEST(TreeTest, MapWhereReduce) {
    BinaryTree<int> tree = MakeTree({1, 2, 3, 4, 5});
    EXPECT_EQ(tree.Map([](const int& x) { return x * x; }).ToString(Order::LKP), "1 4 9 16 25");
    EXPECT_EQ(tree.Map([](const int& x) { return x % 2; }).ToString(Order::LKP), "0 1");
    EXPECT_TRUE(tree.Map([](const int& x) { return -x; }).IsBalanced());
    EXPECT_EQ(tree.Where([](const int& x) { return x > 2; }).ToString(Order::LKP), "3 4 5");
    EXPECT_TRUE(tree.Where([](const int&) { return false; }).IsEmpty());
    EXPECT_EQ(tree.Reduce([](const int& x, const int& acc) { return x + acc; }, 0), 15);
    BinaryTree<int> small = MakeTree({1, 2, 3});
    EXPECT_EQ(small.Reduce([](const int& x1, const int& x2) { return 2 * x1 + 3 * x2; }, 4), 144);
    EXPECT_EQ(BinaryTree<int>().Reduce([](const int& x, const int& acc) { return x + acc; }, 7), 7);
    EXPECT_EQ(tree.GetSize(), 5u);
}

TEST(TreeTest, Strings) {
    BinaryTree<int> tree = MakeTree({4, 2, 6, 1, 3});
    EXPECT_EQ(TreeFormatter<int>("{K}(L)[P]").Format(tree), "{4}({2}({1}()[])[{3}()[]])[{6}()[]]");
    EXPECT_EQ(TreeFormatter<int>("K ").Format(tree), "4 ");
    EXPECT_EQ(TreeFormatter<int>("LK P").Format(tree), "1 2 3 4 6 ");
    EXPECT_EQ(TreeFormatter<int>("{K}").Format(BinaryTree<int>()), "");
    EXPECT_TRUE(Load<int>(tree.ToString()) == tree);
    EXPECT_TRUE(Load<int>("").IsEmpty());
    EXPECT_TRUE(Load<int>("   ").IsEmpty());
    EXPECT_EQ(Load<int>("  3   1  2 ").ToString(Order::LKP), "1 2 3");
    EXPECT_THROW(Load<int>("1 2 abc"), InvalidArgument);
    EXPECT_EQ(Load<int>("1 1").GetSize(), 1u);

    BinaryTree<int> kept = MakeTree({1, 2, 3});
    EXPECT_THROW(kept.LoadFromString("4 5 bad"), InvalidArgument);
    EXPECT_EQ(kept.ToString(Order::LKP), "1 2 3");

    BinaryTree<int> chain = Load<int>("1 2 3 4 5 6 7");
    EXPECT_TRUE(chain.IsBalanced());
    EXPECT_EQ(chain.ToString(Order::LKP), "1 2 3 4 5 6 7");

    BinaryTree<int> big;
    for (int i = 0; i < 100; i++) big.Insert(i);
    EXPECT_TRUE(Load<int>(big.ToString(Order::KLP)) == big);
}

TEST(TreeTest, Subtrees) {
    BinaryTree<int> tree = MakeTree({4, 2, 6, 1, 3, 5, 7});
    BinaryTree<int> sub = tree.ExtractSubtree(2);
    EXPECT_EQ(sub.ToString(Order::KLP), "2 1 3");
    EXPECT_EQ(sub.GetSize(), 3u);
    EXPECT_TRUE(tree.ContainsSubtree(sub));
    EXPECT_TRUE(tree.ContainsSubtree(tree));
    EXPECT_TRUE(tree.ContainsSubtree(BinaryTree<int>()));
    EXPECT_FALSE(tree.ContainsSubtree(MakeTree({2, 1})));
    EXPECT_FALSE(tree.ContainsSubtree(MakeTree({100})));
    EXPECT_TRUE(tree.ContainsSubtree(MakeTree({7})));
    EXPECT_TRUE(tree.ExtractSubtree(42).IsEmpty());
    sub.Insert(0);
    EXPECT_FALSE(tree.Contains(0));
}

TEST(SetTest, Operations) {
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

TEST(SetTest, Relations) {
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

TEST(SetTest, Print) {
    std::ostringstream out;
    out << MakeSet({3, 1, 2});
    EXPECT_EQ(out.str(), "{1 2 3}");
    EXPECT_EQ(Set<int>().ToString(), "{}");
}

TEST(TypesTest, StringsAndDoubles) {
    EXPECT_EQ(Load<std::string>("pear apple melon").ToString(Order::LKP), "apple melon pear");
    EXPECT_EQ(Load<double>("2.5 -1 0.75").ToString(Order::LKP), "-1 0.75 2.5");
}

TEST(TypesTest, Complex) {
    BinaryTree<Complex> nums = Load<Complex>("1,2 1,-1 0,5");
    EXPECT_EQ(nums.ToString(Order::LKP), "0,5 1,-1 1,2");
    EXPECT_THROW(Load<Complex>("1;2"), InvalidArgument);

    Complex z(7, 7);
    std::istringstream bad("3;4");
    EXPECT_FALSE(bad >> z);
    EXPECT_EQ(z.re, 7);
    EXPECT_EQ(z.im, 7);
}

TEST(TypesTest, Student) {
    BinaryTree<Student> group;
    group.Insert(Student(3, "Ivanov", "Ivan", 2005));
    group.Insert(Student(1, "Petrov", "Petr", 2004));
    EXPECT_TRUE(group.Contains(Student(3)));
    EXPECT_EQ(group.ToString(Order::LKP), "1;Petrov;Petr;2004 3;Ivanov;Ivan;2005");
    EXPECT_TRUE(Load<Student>(group.ToString()) == group);
    EXPECT_THROW(Load<Student>("1;Petrov"), InvalidArgument);
    EXPECT_THROW(Load<Student>("x;Petrov;Petr;2004"), InvalidArgument);
    EXPECT_THROW(Load<Student>("1;Petrov;Petr;2004;extra"), InvalidArgument);

    Student s(9, "Old", "Name", 1999);
    std::istringstream bad("1;Petrov");
    EXPECT_FALSE(bad >> s);
    EXPECT_EQ(s.lastName, "Old");
}

TEST(StackTest, PushPopCopy) {
    Stack<int> s;
    EXPECT_TRUE(s.IsEmpty());
    EXPECT_THROW(s.Top(), EmptyStructureException);
    EXPECT_THROW(s.Pop(), EmptyStructureException);
    for (int i = 1; i <= 3; i++) s.Push(i);
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

TEST(IteratorTest, InOrder) {
    BinaryTree<int> tree = MakeTree({4, 2, 6, 1, 3, 5, 7});
    std::ostringstream text;
    for (int v : tree) text << v << ' ';
    EXPECT_EQ(text.str(), "1 2 3 4 5 6 7 ");

    BinaryTree<int> empty;
    EXPECT_TRUE(empty.begin() == empty.end());
    EXPECT_THROW(*empty.begin(), EndOfStreamException);

    BinaryTree<int> big;
    for (int i = 1000; i > 0; i--) big.Insert(i);
    int expected = 1;
    for (int v : big) EXPECT_EQ(v, expected++);
    EXPECT_EQ(expected, 1001);
}

TEST(IteratorTest, CopyAndPostIncrement) {
    BinaryTree<int> tree = MakeTree({2, 1, 3});
    BinaryTree<int>::Iterator it = tree.begin();
    BinaryTree<int>::Iterator old = it++;
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

TEST(IteratorTest, Students) {
    BinaryTree<Student> group;
    group.Insert(Student(3, "Ivanov", "Ivan", 2005));
    group.Insert(Student(1, "Petrov", "Petr", 2004));
    auto it = group.begin();
    EXPECT_EQ(it->lastName, "Petrov");
}

TEST(TreeTest, TraverseWithAction) {
    BinaryTree<int> tree = MakeTree({4, 2, 6, 1, 3, 5, 7});
    std::ostringstream out;
    tree.Traverse(Order::LPK, [&](const int& value) { out << value << ' '; });
    EXPECT_EQ(out.str(), "1 3 2 5 7 6 4 ");
    int sum = 0;
    tree.Traverse(Order::PKL, [&](const int& value) { sum += value; });
    EXPECT_EQ(sum, 28);
}
