#include "DS/LichaoSegTree.h"
#include "IO/FastIO.h"

/*
[Segment Add Get Min](https://judge.yosupo.jp/problem/segment_add_get_min)(https://github.com/yosupo06/library-checker-problems/issues/211)
*/
/**
 * 本题为李超线段树模板题，且仅在全局插入直线，而不在部分区间插入线段
 * 另外，区间要从 [-1e9,1e9] 平移到 [0, 2e9]
 */

int main() {
    uint32_t n, q;
    cin >> n >> q;
    static constexpr uint32_t M = 1000000000;
    static constexpr int64_t inf = 4e18;
    OY::VectorLichaoSlopeChminSegTree<int64_t, uint32_t>::_reserve(10000000);
    OY::VectorLichaoSlopeChminSegTree<int64_t, uint32_t> S(M * 2 + 1, {0, inf});
    for (uint32_t i = 0; i != n; i++) {
        int l, r;
        int64_t a, b;
        cin >> l >> r >> a >> b;
        S.add(l + M, r + M - 1, {a, b - a * M});
    }
    for (uint32_t i = 0; i != q; i++) {
        char op;
        cin >> op;
        if (op == '0') {
            int l, r;
            int64_t a, b;
            cin >> l >> r >> a >> b;
            S.add(l + M, r + M - 1, {a, b - a * M});
        } else {
            int p;
            cin >> p;
            auto y = S.query(p + M).calc(p + M);
            if (y == inf)
                cout << "INFINITY\n";
            else
                cout << y << endl;
        }
    }
}