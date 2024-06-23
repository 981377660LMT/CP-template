#include "IO/FastIO.h"
#include "MATH/SqrtDecomposition.h"

int main() {
    uint32_t n = 105;
    OY::SqrtDecomposition sd(n);
    cout << "there are " << sd.size() << " blocks\n";
    for (auto range : sd) {
        cout << "floor(" << n << " / x) = " << range.m_quot << ", \t" << range.m_left << " <= x <= " << range.m_right << endl;
    }

    // 也可以反向迭代
    cout << "\nreversed:\n";
    for (auto it = sd.rbegin(); it != sd.rend(); ++it) {
        cout << "floor(" << n << " / x) = " << it->m_quot << ", \t" << it->m_left << " <= x <= " << it->m_right << endl;
    }

    // 可以二分找到一个迭代器
    cout << "\nlower_bound:\n";
    auto it = sd.lower_bound(13);
    cout << "floor(" << n << " / x) = " << it->m_quot << ", \t" << it->m_left << " <= x <= " << it->m_right << endl;
    it = sd.upper_bound(13);
    cout << "floor(" << n << " / x) = " << it->m_quot << ", \t" << it->m_left << " <= x <= " << it->m_right << endl;
}
/*
#输出如下
there are 19 blocks
floor(105 / x) = 1, 	53 <= x <= 105
floor(105 / x) = 2, 	36 <= x <= 52
floor(105 / x) = 3, 	27 <= x <= 35
floor(105 / x) = 4, 	22 <= x <= 26
floor(105 / x) = 5, 	18 <= x <= 21
floor(105 / x) = 6, 	16 <= x <= 17
floor(105 / x) = 7, 	14 <= x <= 15
floor(105 / x) = 8, 	12 <= x <= 13
floor(105 / x) = 9, 	11 <= x <= 11
floor(105 / x) = 10, 	10 <= x <= 10
floor(105 / x) = 11, 	9 <= x <= 9
floor(105 / x) = 13, 	8 <= x <= 8
floor(105 / x) = 15, 	7 <= x <= 7
floor(105 / x) = 17, 	6 <= x <= 6
floor(105 / x) = 21, 	5 <= x <= 5
floor(105 / x) = 26, 	4 <= x <= 4
floor(105 / x) = 35, 	3 <= x <= 3
floor(105 / x) = 52, 	2 <= x <= 2
floor(105 / x) = 105, 	1 <= x <= 1

reversed:
floor(105 / x) = 105, 	1 <= x <= 1
floor(105 / x) = 52, 	2 <= x <= 2
floor(105 / x) = 35, 	3 <= x <= 3
floor(105 / x) = 26, 	4 <= x <= 4
floor(105 / x) = 21, 	5 <= x <= 5
floor(105 / x) = 17, 	6 <= x <= 6
floor(105 / x) = 15, 	7 <= x <= 7
floor(105 / x) = 13, 	8 <= x <= 8
floor(105 / x) = 11, 	9 <= x <= 9
floor(105 / x) = 10, 	10 <= x <= 10
floor(105 / x) = 9, 	11 <= x <= 11
floor(105 / x) = 8, 	12 <= x <= 13
floor(105 / x) = 7, 	14 <= x <= 15
floor(105 / x) = 6, 	16 <= x <= 17
floor(105 / x) = 5, 	18 <= x <= 21
floor(105 / x) = 4, 	22 <= x <= 26
floor(105 / x) = 3, 	27 <= x <= 35
floor(105 / x) = 2, 	36 <= x <= 52
floor(105 / x) = 1, 	53 <= x <= 105

lower_bound:
floor(105 / x) = 13, 	8 <= x <= 8
floor(105 / x) = 15, 	7 <= x <= 7

*/