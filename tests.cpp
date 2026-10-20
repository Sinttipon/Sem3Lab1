#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include <vector>
#include "BinaryTree.h"
#include "Set.h"
#include "Types.h"

static BinaryTree<int> MakeTree(const std::vector<int> &values)
{
    BinaryTree<int> tree;
    for (int v : values)
        tree.Insert(v);
    return tree;
}


TEST(TreeTest, EmptyTree)
{
    BinaryTree<int> tree;
    EXPECT_TRUE(tree.IsEmpty());
    EXPECT_TRUE(tree.GetSize() == 0);
    EXPECT_TRUE(tree.GetHeight() == 0);
    EXPECT_TRUE(!tree.Contains(1));
    EXPECT_TRUE(tree.ToString() == "");
    EXPECT_TRUE(tree.IsBalanced());
}

TEST(TreeTest, InsertContains)
{
    BinaryTree<int> tree = MakeTree({50, 30, 70, 20, 40});
    EXPECT_TRUE(tree.GetSize() == 5);
    EXPECT_TRUE(tree.Contains(50));
    EXPECT_TRUE(tree.Contains(20));
    EXPECT_TRUE(!tree.Contains(45));
    EXPECT_THROW(tree.Insert(30), std::invalid_argument);
    EXPECT_TRUE(tree.GetSize() == 5);
}

TEST(TreeTest, Traversals)
{
    BinaryTree<int> tree = MakeTree({4, 2, 6, 1, 3, 5, 7});
    EXPECT_TRUE(tree.ToString("KLP") == "4 2 1 3 6 5 7");
    EXPECT_TRUE(tree.ToString("KPL") == "4 6 7 5 2 3 1");
    EXPECT_TRUE(tree.ToString("LKP") == "1 2 3 4 5 6 7");
    EXPECT_TRUE(tree.ToString("LPK") == "1 3 2 5 7 6 4");
    EXPECT_TRUE(tree.ToString("PLK") == "7 5 6 3 1 2 4");
    EXPECT_TRUE(tree.ToString("PKL") == "7 6 5 4 3 2 1");
    EXPECT_THROW(tree.ToString("KL"), std::invalid_argument);
    EXPECT_THROW(tree.ToString("KKP"), std::invalid_argument);
    EXPECT_THROW(tree.ToString("KLX"), std::invalid_argument);
}

TEST(TreeTest, RemoveCases)
{
    BinaryTree<int> tree = MakeTree({4, 2, 6, 1, 3, 5, 7});
    tree.Remove(1); 
    EXPECT_TRUE(tree.ToString("LKP") == "2 3 4 5 6 7");
    tree.Remove(2); 
    EXPECT_TRUE(tree.ToString("LKP") == "3 4 5 6 7");
    tree.Remove(4); 
    EXPECT_TRUE(tree.ToString("LKP") == "3 5 6 7");
    EXPECT_TRUE(tree.GetSize() == 4);
    EXPECT_TRUE(tree.IsBalanced());
    EXPECT_THROW(tree.Remove(100), std::invalid_argument);
    EXPECT_TRUE(tree.GetSize() == 4);
}

TEST(TreeTest, RemoveAll)
{
    BinaryTree<int> tree = MakeTree({5, 3, 8, 1, 4});
    for (int v : {3, 5, 1, 8, 4})
        tree.Remove(v);
    EXPECT_TRUE(tree.IsEmpty());
    EXPECT_TRUE(tree.GetHeight() == 0);
    EXPECT_THROW(tree.Remove(5), std::invalid_argument);
}

TEST(TreeTest, BalanceSorted)
{
    BinaryTree<int> tree;
    for (int i = 1; i <= 1023; i++)
        tree.Insert(i);
    EXPECT_TRUE(tree.GetHeight() == 10); 
    EXPECT_TRUE(tree.IsBalanced());
    for (int i = 1; i <= 1023; i += 2)
        tree.Remove(i);
    EXPECT_TRUE(tree.IsBalanced());
    EXPECT_TRUE(tree.GetSize() == 511);
}

TEST(TreeTest, BalanceRandom)
{
    BinaryTree<int> tree;
    std::vector<int> present(1000, 0);
    unsigned seed = 12345;
    bool ok = true;
    for (int step = 0; step < 20000; step++)
    {
        seed = seed * 1103515245 + 12345;
        int v = (seed >> 8) % 1000;
        if (present[v])
        {
            tree.Remove(v);
            present[v] = 0;
        }
        else
        {
            tree.Insert(v);
            present[v] = 1;
        }
        if (step % 500 == 0 && !tree.IsBalanced())
            ok = false;
    }
    EXPECT_TRUE(ok);
    size_t expected = 0;
    for (int p : present)
        expected += p;
    EXPECT_TRUE(tree.GetSize() == expected);
}

TEST(TreeTest, CopyAndAssign)
{
    BinaryTree<int> a = MakeTree({2, 1, 3});
    BinaryTree<int> b = a;
    b.Insert(4);
    EXPECT_TRUE(!a.Contains(4));
    EXPECT_TRUE(b.Contains(4));
    BinaryTree<int> c;
    c = a;
    c.Remove(1);
    EXPECT_TRUE(a.Contains(1));
    a = a;
    EXPECT_TRUE(a.GetSize() == 3);
}

TEST(TreeTest, Clear)
{
    BinaryTree<int> tree = MakeTree({1, 2, 3});
    tree.Clear();
    EXPECT_TRUE(tree.IsEmpty());
    tree.Insert(10);
    EXPECT_TRUE(tree.GetSize() == 1);
}

TEST(TreeTest, MapWhereReduce)
{
    BinaryTree<int> tree = MakeTree({1, 2, 3, 4, 5});
    EXPECT_TRUE(tree.Map([](const int &x)
                         { return x * x; })
                    .ToString("LKP") == "1 4 9 16 25");
    EXPECT_TRUE(tree.Map([](const int &x)
                         { return x % 2; })
                    .ToString("LKP") == "0 1");
    EXPECT_TRUE(tree.Map([](const int &x)
                         { return -x; })
                    .IsBalanced());
    EXPECT_TRUE(tree.Where([](const int &x)
                           { return x > 2; })
                    .ToString("LKP") == "3 4 5");
    EXPECT_TRUE(tree.Where([](const int &)
                           { return false; })
                    .IsEmpty());
    EXPECT_TRUE(tree.Reduce([](const int &x, const int &acc)
                            { return x + acc; }, 0) == 15);
    BinaryTree<int> small = MakeTree({1, 2, 3});
    EXPECT_TRUE(small.Reduce([](const int &x1, const int &x2)
                             { return 2 * x1 + 3 * x2; }, 4) == 144);
    BinaryTree<int> empty;
    EXPECT_TRUE(empty.Reduce([](const int &x, const int &acc)
                             { return x + acc; }, 7) == 7);
    EXPECT_TRUE(tree.GetSize() == 5); 
}

TEST(TreeTest, Strings)
{
    BinaryTree<int> tree = MakeTree({4, 2, 6, 1, 3});
    EXPECT_TRUE(tree.ToFormattedString("{K}(L)[P]") == "{4}({2}({1}()[])[{3}()[]])[{6}()[]]");
    EXPECT_THROW(tree.ToFormattedString("{K}(L)"), std::invalid_argument);
    BinaryTree<int> loaded = BinaryTree<int>::FromString(tree.ToString());
    EXPECT_TRUE(loaded.ToString("LKP") == tree.ToString("LKP"));
    EXPECT_TRUE(BinaryTree<int>::FromString("").IsEmpty());
    EXPECT_TRUE(BinaryTree<int>::FromString("  3   1  2 ").ToString("LKP") == "1 2 3");
    EXPECT_THROW(BinaryTree<int>::FromString("1 2 abc"), std::invalid_argument);
    EXPECT_THROW(BinaryTree<int>::FromString("1 1"), std::invalid_argument);
}

TEST(TreeTest, Subtrees)
{
    BinaryTree<int> tree = MakeTree({4, 2, 6, 1, 3, 5, 7});
    BinaryTree<int> sub = tree.ExtractSubtree(2);
    EXPECT_TRUE(sub.ToString("KLP") == "2 1 3");
    EXPECT_TRUE(sub.GetSize() == 3);
    EXPECT_TRUE(tree.ContainsSubtree(sub));
    EXPECT_TRUE(tree.ContainsSubtree(tree));
    EXPECT_TRUE(tree.ContainsSubtree(BinaryTree<int>()));
    EXPECT_TRUE(!tree.ContainsSubtree(MakeTree({2, 1})));
    EXPECT_TRUE(!tree.ContainsSubtree(MakeTree({100})));
    EXPECT_TRUE(tree.ContainsSubtree(MakeTree({7})));
    EXPECT_THROW(tree.ExtractSubtree(42), std::invalid_argument);
    sub.Insert(0);
    EXPECT_TRUE(!tree.Contains(0));
}

static Set<int> MakeSet(const std::vector<int> &values)
{
    Set<int> set;
    for (int v : values)
        set.Add(v);
    return set;
}

TEST(SetTest, SetOperations)
{
    Set<int> a = MakeSet({1, 2, 3, 4});
    Set<int> b = MakeSet({3, 4, 5});
    Set<int> empty;
    EXPECT_TRUE((a + b) == MakeSet({1, 2, 3, 4, 5}));
    EXPECT_TRUE((a * b) == MakeSet({3, 4}));
    EXPECT_TRUE((a - b) == MakeSet({1, 2}));
    EXPECT_TRUE((b - a) == MakeSet({5}));
    EXPECT_TRUE((a + empty) == a);
    EXPECT_TRUE((a * empty) == empty);
    EXPECT_TRUE((a - a) == empty);
    EXPECT_TRUE((a * b) == (b * a));
}

TEST(SetTest, SetRelations)
{
    Set<int> a = MakeSet({1, 2, 3});
    EXPECT_TRUE(MakeSet({1, 3}) <= a);
    EXPECT_TRUE(!(MakeSet({1, 4}) <= a));
    EXPECT_TRUE(Set<int>() <= a);
    EXPECT_TRUE(a <= a);
    EXPECT_TRUE(a == MakeSet({3, 2, 1}));
    EXPECT_TRUE(a != MakeSet({1, 2}));
    Set<int> b = a;
    b.Add(2); 
    EXPECT_TRUE(b.GetSize() == 3);
    EXPECT_THROW(b.Remove(10), std::invalid_argument);
}

TEST(TypesTest, OtherTypes)
{
    BinaryTree<std::string> words = BinaryTree<std::string>::FromString("pear apple melon");
    EXPECT_TRUE(words.ToString("LKP") == "apple melon pear");

    BinaryTree<double> reals = BinaryTree<double>::FromString("2.5 -1 0.75");
    EXPECT_TRUE(reals.ToString("LKP") == "-1 0.75 2.5");

    BinaryTree<Complex> nums = BinaryTree<Complex>::FromString("1,2 1,-1 0,5");
    EXPECT_TRUE(nums.ToString("LKP") == "0,5 1,-1 1,2");
    EXPECT_THROW(BinaryTree<Complex>::FromString("1;2"), std::invalid_argument);

    BinaryTree<Student> group;
    group.Insert(Student(3, "Ivanov", "Ivan", 2005));
    group.Insert(Student(1, "Petrov", "Petr", 2004));
    EXPECT_TRUE(group.Contains(Student(3))); 
    EXPECT_TRUE(group.ToString("LKP") == "1;Petrov;Petr;2004 3;Ivanov;Ivan;2005");
    BinaryTree<Student> loaded = BinaryTree<Student>::FromString(group.ToString());
    EXPECT_TRUE(loaded.ToString("LKP") == group.ToString("LKP"));
    EXPECT_THROW(BinaryTree<Student>::FromString("1;Petrov"), std::invalid_argument);
}
