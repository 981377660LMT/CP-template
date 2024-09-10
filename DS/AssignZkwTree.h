/*
最后修改:
20240907
测试环境:
gcc11.2,c++11
clang12.0,C++11
msvc14.2,C++14
*/
#ifndef __OY_ASZKWTREE__
#define __OY_ASZKWTREE__

#include <algorithm>
#include <cstdint>
#include <vector>

#include "../TEST/std_bit.h"

namespace OY {
    namespace ASZKW {
        using size_type = uint32_t;
        template <typename Tp>
        struct BaseMonoid {
            using value_type = Tp;
        };
        template <typename Tp>
        struct SumMonoid {
            using value_type = Tp;
            static Tp identity() { return Tp{}; }
            static Tp op(const Tp &x, const Tp &y) { return x + y; }
            static Tp square(const Tp &x, size_type n) { return x * n; }
        };
        template <typename Tp, Tp Identity, typename Operation, typename Square>
        struct FastSquareMonoid {
            using value_type = Tp;
            static Tp identity() { return Identity; }
            static Tp op(const Tp &x, const Tp &y) { return Operation()(x, y); }
            static Tp square(const Tp &x, size_type n) { return Square()(x, n); }
        };
        template <typename Tp, Tp Identity, typename Operation>
        struct LazyMonoid {
            using value_type = Tp;
            static Tp identity() { return Identity; }
            static Tp op(const Tp &x, const Tp &y) { return Operation()(x, y); }
        };
#ifdef __cpp_lib_void_t
        template <typename... Tp>
        using void_t = std::void_t<Tp...>;
#else
        template <typename... Tp>
        struct make_void {
            using type = void;
        };
        template <typename... Tp>
        using void_t = typename make_void<Tp...>::type;
#endif
        template <typename Tp, typename ValueType, typename SizeType, typename = void>
        struct Has_Square : std::false_type {};
        template <typename Tp, typename ValueType, typename SizeType>
        struct Has_Square<Tp, ValueType, SizeType, void_t<decltype(Tp::square(std::declval<ValueType>(), std::declval<SizeType>()))>> : std::true_type {};
        template <typename Tp, typename ValueType>
        struct Has_Square<Tp, ValueType, void, void_t<decltype(Tp::square(std::declval<ValueType>()))>> : std::true_type {};
        template <typename Tp, typename ValueType, typename = void>
        struct Has_Op : std::false_type {};
        template <typename Tp, typename ValueType>
        struct Has_Op<Tp, ValueType, void_t<decltype(Tp::op(std::declval<ValueType>(), std::declval<ValueType>()))>> : std::true_type {};
        template <typename Tp, bool Info, bool FastCalc>
        struct AssignNode {
            Tp m_val;
            bool m_cover;
            bool has_lazy() const { return m_cover; }
            const Tp &get_lazy() const { return m_val; }
            void clear_lazy() { m_cover = false; }
        };
        template <typename Tp>
        struct AssignNode<Tp, true, true> {
            Tp m_val, m_lazy;
            bool m_cover;
            bool has_lazy() const { return m_cover; }
            const Tp &get_lazy() const { return m_lazy; }
            void clear_lazy() { m_cover = false; }
        };
        template <typename Tp>
        struct AssignNode<Tp, true, false> {
            Tp m_val;
            const Tp *m_lazy;
            bool has_lazy() const { return m_lazy; }
            const Tp *get_lazy() const { return m_lazy; }
            void clear_lazy() { m_lazy = nullptr; }
        };
        template <size_type BATCH = 1 << 17>
        struct VectorBufferWrap {
            template <typename Tp>
            struct type {
                static std::vector<std::vector<Tp>> s_bufs;
                static Tp *s_cur;
                static size_type s_rem;
                static void malloc(Tp *&ptr, size_type len) {
                    if (len > s_rem) s_bufs.emplace_back(BATCH), s_cur = s_bufs.back().data(), s_rem = BATCH;
                    ptr = s_cur, s_cur += len, s_rem -= len;
                }
            };
        };
        template <size_type BATCH>
        template <typename Tp>
        std::vector<std::vector<Tp>> VectorBufferWrap<BATCH>::template type<Tp>::s_bufs;
        template <size_type BATCH>
        template <typename Tp>
        Tp *VectorBufferWrap<BATCH>::template type<Tp>::s_cur;
        template <size_type BATCH>
        template <typename Tp>
        size_type VectorBufferWrap<BATCH>::template type<Tp>::s_rem;
        template <typename Monoid, template <typename> typename BufferType = VectorBufferWrap<>::type>
        class Tree {
        public:
            using value_type = typename Monoid::value_type;
            static constexpr bool has_fast_square = Has_Square<Monoid, value_type, size_type>::value, has_op = Has_Op<Monoid, value_type>::value;
            using node = AssignNode<value_type, has_op, has_fast_square>;
            struct iterator {
                size_type m_index;
                node *m_ptr;
            };
        private:
            mutable std::vector<node> m_sub;
            size_type m_size, m_cap, m_dep;
            static void _apply(node *p, const value_type *lazy, size_type level) { p->m_val = lazy[level], p->m_lazy = lazy; }
            static void _apply(node *p, const value_type &lazy, size_type level) {
                if constexpr (!has_op)
                    p->m_val = lazy;
                else if constexpr (has_fast_square)
                    p->m_val = Monoid::square(lazy, 1 << level);
                p->m_cover = true;
            }
            static void _pushdown(node *sub, size_type i, size_type level) {
                if (!sub[i].has_lazy()) return;
                _apply(sub + i * 2, sub[i].get_lazy(), level - 1);
                _apply(sub + (i * 2 + 1), sub[i].get_lazy(), level - 1);
                sub[i].clear_lazy();
            }
            static void _pushup(node *sub, size_type i, size_type len) {
                if constexpr (has_op) sub[i].m_val = Monoid::op(sub[i * 2].m_val, sub[i * 2 + 1].m_val);
            }
            static void _fetch(node *sub, size_type l, size_type r, size_type level) {
                if (l == 1) return;
                _fetch(sub, l >> 1, r >> 1, level + 1);
                for (size_type i = l >> 1; i <= r >> 1; i++) _pushdown(sub, i, level);
            }
        public:
            Tree(size_type length = 0) { resize(length); }
            template <typename InitMapping>
            Tree(size_type length, InitMapping mapping) { resize(length, mapping); }
            template <typename Iterator>
            Tree(Iterator first, Iterator last) { reset(first, last); }
            size_type size() const { return m_size; }
            void resize(size_type length) {
                if (!(m_size = length)) return;
                m_dep = std::max<size_type>(1, std::bit_width(m_size - 1)), m_cap = 1 << m_dep;
                m_sub.assign(m_cap * 2, Monoid::identity());
            }
            template <typename InitMapping>
            void resize(size_type length, InitMapping mapping) {
                if (!(m_size = length)) return;
                m_dep = std::max<size_type>(1, std::bit_width(m_size - 1)), m_cap = 1 << m_dep;
                m_sub.assign(m_cap * 2, {});
                node *sub = m_sub.data();
                for (size_type i = 0; i != m_size; i++) sub[m_cap + i].m_val = mapping(i);
                if constexpr (has_op)
                    for (size_type i = m_size; i != m_cap; i++) sub[m_cap + i].m_val = Monoid::identity();
                for (size_type len = m_cap / 2, k = 2; len; len >>= 1, k <<= 1)
                    for (size_type i = len; i != len << 1; i++) _pushup(sub, i, k);
            }
            template <typename Iterator>
            void reset(Iterator first, Iterator last) {
                resize(last - first, [&](size_type i) { return *(first + i); });
            }
            void modify(size_type i, value_type val) {
                do_for_node<false>(i, [&val](node *p) { p->m_val = val; });
            }
            void modify(size_type left, size_type right, value_type val) {
                if (left == right) return modify(left, val);
                left += m_cap, right += m_cap;
                node *sub = m_sub.data();
                size_type j = std::bit_width(left ^ right) - 1;
                for (size_type d = m_dep; d > j; d--) _pushdown(sub, left >> d, d);
                for (size_type d = j; d; d--) _pushdown(sub, left >> d, d), _pushdown(sub, right >> d, d);
                sub[left].m_val = sub[right].m_val = val;
                size_type level = 0;
                if (j)
                    if constexpr (!has_op || has_fast_square)
                        while (left >> 1 < right >> 1) {
                            if (!(left & 1)) _apply(sub + (left + 1), val, level);
                            _pushup(sub, left >>= 1, 1 << (level + 1));
                            if (right & 1) _apply(sub + (right - 1), val, level);
                            _pushup(sub, right >>= 1, 1 << (++level));
                        }
                    else {
                        value_type *lazy;
                        BufferType<value_type>::malloc(lazy, j);
                        lazy[0] = val;
                        for (size_type i = 1; i != j; i++)
                            if constexpr (Has_Square<Monoid, value_type, void>::value)
                                lazy[i] = Monoid::square(lazy[i - 1]);
                            else
                                lazy[i] = Monoid::op(lazy[i - 1], lazy[i - 1]);
                        while (left >> 1 < right >> 1) {
                            if (!(left & 1)) _apply(sub + (left + 1), lazy, level);
                            _pushup(sub, left >>= 1, 1 << (level + 1));
                            if (right & 1) _apply(sub + (right - 1), lazy, level);
                            _pushup(sub, right >>= 1, 1 << (++level));
                        }
                    }
                while (left >>= 1) _pushup(sub, left, 1 << ++level);
            }
            value_type query(size_type i) const {
                return ((Tree *)(this))->do_for_node<true>(i, [&](const node *p) { return p->m_val; });
            }
            value_type query(size_type left, size_type right) const {
                if (left == right) return query(left);
                left += m_cap, right += m_cap;
                node *sub = m_sub.data();
                size_type j = std::bit_width(left ^ right) - 1;
                for (size_type d = m_dep; d > j; d--) _pushdown(sub, left >> d, d);
                for (size_type d = j; d; d--) _pushdown(sub, left >> d, d), _pushdown(sub, right >> d, d);
                value_type resl = sub[left].m_val, resr = sub[right].m_val;
                for (; left >> 1 != right >> 1; left >>= 1, right >>= 1) {
                    if (!(left & 1)) resl = Monoid::op(resl, sub[left ^ 1].m_val);
                    if (right & 1) resr = Monoid::op(sub[right ^ 1].m_val, resr);
                }
                return Monoid::op(resl, resr);
            }
            value_type query_all() const { return m_sub[1].m_val; }
            template <typename Judger>
            size_type max_right(size_type left, Judger &&judge) const {
                left += m_cap;
                node *sub = m_sub.data();
                for (size_type d = m_dep; d; d--) _pushdown(sub, left >> d, d);
                value_type val = sub[left].m_val;
                if (!judge(val)) return left - m_cap - 1;
                left++;
                for (size_type level = 0; std::popcount(left) > 1;) {
                    size_type ctz = std::countr_zero(left);
                    value_type a = Monoid::op(val, sub[left >>= ctz].m_val);
                    level += ctz;
                    if (judge(a))
                        val = a, left++;
                    else {
                        for (; left < m_cap; level--) {
                            _pushdown(sub, left, level);
                            value_type a = Monoid::op(val, sub[left <<= 1].m_val);
                            if (judge(a)) val = a, left++;
                        }
                        return std::min(left - m_cap, m_size) - 1;
                    }
                }
                return m_size - 1;
            }
            template <typename Judger>
            size_type min_left(size_type right, Judger &&judge) const {
                right += m_cap;
                node *sub = m_sub.data();
                for (size_type d = m_dep; d; d--) _pushdown(sub, right >> d, d);
                value_type val = sub[right].m_val;
                if (!judge(val)) return right - m_cap + 1;
                for (size_type level = 0; std::popcount(right) > 1;) {
                    size_type ctz = std::countr_zero(right - m_cap);
                    value_type a = Monoid::op(sub[(right >>= ctz) - 1].m_val, val);
                    level -= ctz;
                    if (judge(a))
                        val = a, right--;
                    else {
                        for (; right <= m_cap; level--) {
                            _pushdown(sub, right - 1, level);
                            value_type a = Monoid::op(sub[(right <<= 1) - 1].m_val, val);
                            if (judge(a)) val = a, right--;
                        }
                        return right - m_cap;
                    }
                }
                return 0;
            }
            iterator kth(value_type k) const {
                static_assert(has_op, "Kth Operation Cannot Be Done Without Range Sum");
                node *sub = m_sub.data();
                size_type i = 1, level = m_dep;
                for (; i < 1 << m_dep; level--) {
                    _pushdown(sub, i, level);
                    if (sub[i <<= 1].m_val <= k) k -= sub[i++].m_val;
                }
                return {i - (1 << m_dep), sub + i};
            }
            template <bool ReadOnly, typename Callback>
            auto do_for_node(size_type i, Callback &&call) const -> decltype(call(m_sub.data())) {
                i += m_cap;
                node *sub = m_sub.data();
                for (size_type d = m_dep; d; d--) _pushdown(sub, i >> d, d);
                if constexpr (ReadOnly)
                    return call(sub + i);
                else {
                    call(sub + i);
                    for (size_type len = 1; len <<= 1, i >>= 1;) _pushup(sub, i, len);
                }
            }
            template <typename Callback>
            void do_for_each(Callback &&call) const {
                node *sub = m_sub.data();
                _fetch(sub, m_cap, m_cap + m_size - 1, 1);
                for (size_type i = m_cap, j = 0; j != m_size; i++, j++) call(sub + i);
            }
        };
        template <typename Ostream, typename Monoid, template <typename> typename BufferType>
        Ostream &operator<<(Ostream &out, const Tree<Monoid, BufferType> &x) {
            out << "[";
            for (size_type i = 0; i != x.size(); i++) {
                if (i) out << ", ";
                out << x.query(i);
            }
            return out << "]";
        }
    }
    template <typename Tp, Tp Identity, template <typename> typename BufferType = ASZKW::VectorBufferWrap<>::type, typename Operation, typename Square, typename InitMapping, typename TreeType = ASZKW::Tree<ASZKW::FastSquareMonoid<Tp, Identity, Operation, Square>, BufferType>>
    auto make_fast_square_AssignZkwTree(ASZKW::size_type length, Operation op, Square square, InitMapping mapping) -> TreeType { return TreeType(length, mapping); }
    template <typename Tp, Tp Identity, template <typename> typename BufferType = ASZKW::VectorBufferWrap<>::type, typename Operation, typename InitMapping, typename TreeType = ASZKW::Tree<ASZKW::LazyMonoid<Tp, Identity, Operation>, BufferType>>
    auto make_lazy_AssignZkwTree(ASZKW::size_type length, Operation op, InitMapping mapping) -> TreeType { return TreeType(length, mapping); }
    template <typename Tp, template <typename> typename BufferType = ASZKW::VectorBufferWrap<>::type>
    using AssignZkw = ASZKW::Tree<ASZKW::BaseMonoid<Tp>, BufferType>;
    template <typename Tp, template <typename> typename BufferType = ASZKW::VectorBufferWrap<>::type>
    using AssignSumZkw = ASZKW::Tree<ASZKW::SumMonoid<Tp>, BufferType>;
}

#endif