# CP-template

## 库简介

这是我个人的算法竞赛模板库。

大多数代码可以在 (gcc, C++11) ， (clang, C++11) 和 (msvc, C++14) 环境下编译运行。如有特殊情况会注明。

由于 C++17 的 std::gcd/std::lcm 以及 C++20 的位运算使用较多，所以本模板库内置了 std::bit.h 和 std_gcd_lcm.h 两个头文件。语言版本较低的使用者在导入这两个头文件之后，就可以使用 std::gcd/std::lcm/std::popcount 等。

## 库特点

1. 速度极快

   `RMQ` 问题是算法竞赛中常见的问题。如果带修，那么需要使用线段树来维护，以 $O(\log n)$ 的时间复杂度进行修改和查询。
   
   在 `atcoder` 运行 `benchmark` 显示，本模板库的 `OY::MonoMaxZkw<uint32_t>` 在 `n = 1e6` 的情况下，一秒钟可以做 `3.3e7` 次区间最值查询。惊人的速度。

2. 使用方便

   `FOR` 宏定义？ `i64` 缩写？编程老手都认识？ `No` ！本模板库中，不会使用这些花里胡哨的缩写，也不会假定使用者是老手。本模板，让任何码风的人都不会感到不适应。模板高度封装，使用的时候可以当成黑盒，而不需要对模板内部做手脚。

   每份代码都有对应的文档，提供使用范例； `TEST` 文件夹里，提供了本地运行代码，以及在若干 `oj` 题目上提交时的代码。

   ![一个支持点修改的三维线段树示例](./TEST/demo_mdseg.png)
   
## 库特色

1. 大值域的线性空间线段树（ `CompressedTree` 维护 `0~1e18` 范围的权值线段树，空间复杂度正比于操作次数）

2. 动态大小的 `bitset` （ `DynamicBitset` 效率与静态大小的 `bitset` 持平）

3. 可以动态维护全局半群信息的双向队列（ `InfoDeque` 完爆 `std::deque` ）

4. 可以维护区间加定值、区间加一次函数、区间加 `k` 次函数的树状数组 （ `KBIT` ）

5. 当只有邻项合并操作时，严格线性时间复杂度的并查集（ `LinearDSU` ）

6. 线性时间初始化， $O(1)$ 查询区间最值的状压 `RMQ` （ `MaskRMQ` ）

7. 支持区间翻转、区间剪切合并，同时维护区间半群信息的平衡树（ `MonoAVL`）

8. 查询速度极快的单点修改、区间查询线段树（ `MonoZkw` ）

9. 二维、三维以及更高维度上，维护半群信息（可以带修）的多维表/树（ `MultiDimSegTree` ）

10. 通过编写 `node` 以实现自定义操作的势能线段树（ `SegmentBeat` ）

11. 支持区间排序，并维护区间半群信息的线段树/平衡树 （ `SortFHQ` ， `SortSeg`  ）

12. 线性时间初始化， 平均 $O(1)$ 查询区间半群信息的根树（ `SqrtTree` ）

13. 单点加修改、区间和查询速度极快，碾压树状数组的数据结构 （ `WTree` ）

14. 各种存储结构和算法执行器独立分开的图模板；

15. 动态/静态模数的、32位/64位模数的、使用/不使用蒙哥马利模乘的 `modint` ；

16. 运用了蒙哥马利模乘的运行极快的质数判定（ `PrimeCheck` ）

17. 运用了蒙哥马利模乘的运行极快的因数分解（ `Pollard Rho` ）

18. 支持基于范围遍历的数论分块 （ `SqrtDecomposition` ）

19. 支持自定义个数、种类的模数的序列哈希（ `SequenceHash` ， `SequenceMultiHash` ）

20. 支持回滚操作的 `KMP`，回文自动机；

21. 线性时间复杂度的后缀数组；

22. 通过编写 `node` 以实现自定义操作的动态树（ `GBT` ， `LCT`  ）

23. 实用的 `leetcode` 输入输出工具，支持网页端的同样格式的输入输出数据。

## FAQ

1. 我的编程环境非常老旧，看到你的模板库代码花里胡哨的，能运行起来吗？

   本模板库现已放宽对语言环境的要求，绝大多数模板， `gcc` 和 `clang` 在 `C++11` 之后均可使用； `MSVC` 在 `C++14` 之后均可使用（暂无 `C++11` 测试环境）。只要你的 `C++` 语言标准在 `C++11` 之后，均可以使用我的模板库。

2. 在一些模板里，看到 `MAX_NODE` 参数，这是什么意思？

   在平衡树问题中，往往需要动态增加结点；往往这些数据结构的不同实例对象还可以互相合并。此时有两种设计方案：用指针表示结点；用数组下标表示结点。如果使用前者方案，指针是全局的概念，所以不同实例对象可以成功合并；但是结点的大小较大，占用空间较多。如果使用后者方案，数组下标是一个针对某个数组的概念，如果两个平衡树的结点是从两个不同的数组里分配的，那么就没有办法进行合并。
   
   经过权衡，采用后者方案：用数组下标表示结点。这就要求存在一个全局的数组，从这个数组里向不同的树对象分配结点；在合并时，每个结点里保存的左孩子、右孩子等下标可以保持原有的意义，而不至于失效。
   
   特别的，目前的 `LCT` 和 `GBT` 也是从全局的数组里分配结点。
   
   只要看到类内有 `s_buf` 的存在，即表示有内存池。

3. 在模板里，填写的 `MAX_NODE` 是否越大越好？如果是多组测试，是否每组测试重新构造一个数据结构对象就会触发 $O(MAXNODE)$ 的初始化导致超时？

   以下回答针对你的结点类型为平凡类型的情况（无构造函数，无初始值）。

    `MAX_NODE` 相关联的是结点内存池的大小，所以并不会出现每次构造一个数据结构对象，就导致内存池初始化的情况。

    `MAX_NODE` 并非越大越好，当 `MAX_NODE` 过大时，编译可能会失败。只要编译能通过，那么在此范围内 `MAX_NODE` 多大都没关系，也不会有任何的时间开销。

4. 我用整数做平衡树结点键值跑得非常快；换成 `std::string` 做平衡树结点键值，为什么程序突然变得很慢？甚至在很小的样例上都很慢？

   由于模板库内使用静态变量做内存池，静态变量某种意义上就是全局变量，所以在程序启动时就会对所有的结点进行初始化。而 `std::string` 对象即使是空的，也存在初始化代价；如果 `MAX_NODE` 过大，就会占用很长时间。
   
   一般来说，推荐使用平凡类型作为结点的成员变量。由于 `C++` 中，平凡类型的全局变量、静态变量初始化不消耗运行时，所以当你的数据结构的结点类型为平凡类型时，即使开再大的内存池也不会产生一丁点的运行时间。反之，如果你给结点设置了构造、析构，或者给某个成员变量设置了初始值，那么内存池的初始化就会占用时间。

5. 为什么在 `oj` (主要指 `codeforces` ) 提交平衡树代码时，提示如下？

   ```
   Can't compile file:
   Compiled file is too large
   ```

   首先需要知道，我的平衡树模板通过类似于 `MAX_NODE` 这样的模板参数控制一个模板最大的数组空间。这种方式产生的静态数组，并非是在运行期在堆上申请的，而是在编译时直接占用程序体积。

   而一些平台，例如 `codeforces` 对生成的程序大小有限制，有时候 `MAX_NODE` 过大，会生成超出大小限制的程序而无法通过编译。此时问题很好解决，将 `s_buf` 从结点数组类型修改为结点指针类型，然后将类外的 `s_buf` 初始化改写为 `s_buf = new 【结点类型】[MAX_NODE]` 即可。

   例如，如果想声明一个普通平衡树，且最多可能插入二百万个元素，则需要声明如下类： `OY::AVLMultiset<int, std::less<int>, 2000000>` 。当 `MAX_NODE = 2000000` 时，平衡树因为 `MAX_NODE` 过大而无法通过 `codeforces` 编译，则需要做如下修改：

   第 `194` 行修改为

   ```
   static node *s_buf;
   ```

   第 `509` 行修改为

   ```
   typename Tree<NodeWrapper, MAX_NODE>::node *Tree<NodeWrapper, MAX_NODE>::s_buf = new typename Tree<NodeWrapper, MAX_NODE>::node[MAX_NODE + 1]{};
   ```

   此时即可通过 `codeforces` 编译。


6. 既然不会反复初始化内存池，那么多组数据之间是否会产生影响？

   不会。内存池里的结点只会被使用一次，上一组的数据使用的结点不会在下一组数据里被复用。
   
7. 线段树只能有求和的功能吗？

   本模板库非常重视模板的泛化程度。
   
   一般来说，包括线段树在内的维护半群的数据结构，天然带有 `Min` ， `Max` ， `Gcd` ， `Lcm` ， `BitAnd` ， `BitOr` ， `BitXor` 以及 `Sum` 这八种实例类型。如果有额外的需求，可以通过 `make_` 系列来定义新的类型；或者传递自定义的 `Monoid` 半群类型。其他的容器也往往如此。

8. 线段树模板参数一大堆，填写起来老是报错？连类型名字都不能完整写出来该怎么办？

   为了防止定义各种千奇百怪运算符的使用者在使用模板时，因为无法描述出模板的完整类型名称而困扰，所以特意编写了 `make_` 系列函数。如同 `std::make_pair` 以及 `std::make_tuple` 一般，只需要填写少量参数即可创建出复杂类型的模板。例如， `make_SegTree` 可以用来创建线段树；只要打出 `make_SegTree` 之后跟随 `IDE` 的智能提示进行相应的填写即可。
   
9. 用 `make_SegTree` 可以创建一颗线段树；但是如果我要在 `std::vector` 里存放十颗线段树，我还是得把类型全称写出来，可是我写不出来，怎么办？

   既然用 `make_SegTree` 可以创建出一颗线段树，那么可以用 `using NickName = decltype(make_SegTree<...>(...));` 来捕获这棵树的类型，并给它起个别名。接下来即可用 `std::vector<NickName>` 的方式存储十颗线段树。
