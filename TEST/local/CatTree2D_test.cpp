#include "DS/CatTree2D.h"
#include "IO/FastIO.h"

int main() {
    // 二维猫树与一维猫树的用法基本类似，所以只展示一下特殊方法
    int matrix[4][5] = {
        {3, 0, 1, 4, 2},
        {5, 6, 3, 2, 1},
        {1, 2, 0, 1, 5},
        {4, 1, 0, 1, 7},
    };
    // 除了行数、列数，还需要传递一个寻址函数
    auto T = OY::CatMaxTable2D<int, 1000>(4, 5, [&](int i, int j) { return matrix[i][j]; });
    cout << T << endl;
    // 输出[0,2]行，[1,4]列的最大值
    cout << "max(matrix[0~2][1~4])=" << T.query(0, 2, 1, 4) << endl;
}
/*
#输出如下
[[3, 0, 1, 4, 2], [5, 6, 3, 2, 1], [1, 2, 0, 1, 5], [4, 1, 0, 1, 7]]
max(matrix[0~2][1~4])=6
*/