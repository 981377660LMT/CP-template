#include "DS/GlobalHashMap.h"
#include "DS/LazyBitset.h"
#include "DS/StaticBufferWrapWithoutCollect.h"
#include "IO/FastIO.h"

/*
[P10998 【MX-J3-T3+】Tuple+](https://www.luogu.com.cn/problem/P10998)
*/
/**
 * 本题一个简单的想法就是查哈希表
 * 除此之外，需要对 bitset 的与进行优化
 */

using Bitset = OY::LazyBitset::Tree<uint32_t, false, OY::StaticBufferWrapWithoutCollect<6000000>::type>;
OY::GHASH::UnorderedMap<uint64_t, Bitset, false, 400007> GS;
int main() {
    uint32_t n, m;
    cin >> n >> m;
    struct Tuple {
        uint32_t a, b, c;
    };
    std::vector<Tuple> ts(m);
    for (auto &&[a, b, c] : ts) {
        cin >> a >> b >> c;
        a--, b--, c--;
        auto [ptr, flag] = GS.insert((uint64_t(a) << 20) + b);
        if (flag) ptr->m_mapped.resize(n);
        ptr->m_mapped.set(c);
    }
    uint32_t ans = 0;
    auto call = [&](auto...) { ans++; };
    for (auto &&[a, b, c] : ts) {
        auto itab = GS.find((uint64_t(a) << 20) + b);
        if (!itab) continue;
        auto itac = GS.find((uint64_t(a) << 20) + c);
        if (!itac) continue;
        auto itbc = GS.find((uint64_t(b) << 20) + c);
        if (!itbc) continue;
        Bitset::enumerate_bit_and_ones(itab->m_mapped, itac->m_mapped, itbc->m_mapped, call);
    }
    cout << ans;
}
