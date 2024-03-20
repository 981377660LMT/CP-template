#include "DS/BIT.h"
#include "DS/Bitset.h"
#include "DS/LazyBitset.h"
#include "IO/FastIO.h"

/*
[P5057 [CQOI2006] 简单题](https://www.luogu.com.cn/problem/P5057)
*/
/**
 * 本题为 Bitset 模板题
 * 由于状态只有开关两种，也可以重载树状数组的符号为异或，用差分来实现F
 */

static constexpr uint32_t N = 100000;
void solve_bitset() {
    uint32_t n, m;
    cin >> n >> m;
    OY::Bitset::Table<N> S;
    // OY::LazyBitset::Tree<uint32_t,200000>S;
    for (uint32_t i = 0; i < m; i++) {
        char op;
        cin >> op;
        if (op == '1') {
            uint32_t l, r;
            cin >> l >> r;
            S.flip(l - 1, r - 1);
        } else {
            uint32_t i;
            cin >> i;
            cout << S[i - 1] << endl;
        }
    }
}

struct XorNode {
    uint8_t m_val;
    XorNode &operator+=(const XorNode &rhs) {
        m_val = m_val != rhs.m_val;
        return *this;
    }
    XorNode &operator-=(const XorNode &rhs) {
        m_val = m_val != rhs.m_val;
        return *this;
    }
    XorNode operator+(const XorNode &rhs) const {
        XorNode res;
        res.m_val = m_val != rhs.m_val;
        return *this;
    }
    XorNode operator-(const XorNode &rhs) const {
        XorNode res;
        res.m_val = m_val != rhs.m_val;
        return *this;
    }
};
void solve_bit() {
    uint32_t n, m;
    cin >> n >> m;
    OY::BIT::Tree<XorNode, false, N << 1> S(n + 1);
    for (uint32_t i = 0; i < m; i++) {
        char op;
        cin >> op;
        if (op == '1') {
            uint32_t l, r;
            cin >> l >> r;
            S.add(l - 1, {1});
            S.add(r, {1});
        } else {
            uint32_t i;
            cin >> i;
            cout << S.presum(i - 1).m_val << endl;
        }
    }
}

int main() {
    solve_bit();
    // solve_bitset();
}