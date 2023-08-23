#ifndef __OY_SEGBIT__
#define __OY_SEGBIT__

#include <algorithm>
#include <cstdint>
#include <numeric>

namespace OY {
    namespace SegBIT {
        using index_type = uint32_t;
        struct NoInit {};
        template <typename ValueType>
        struct BaseNode {
            using value_type = ValueType;
            using modify_type = ValueType;
            value_type m_val;
            const value_type &get() const { return m_val; }
            void set(const value_type &val) { m_val = val; }
        };
        template <typename RangeMapping, bool Complete>
        struct TreeBase {
            RangeMapping m_range_mapping;
            TreeBase(RangeMapping mapping) : m_range_mapping(mapping) {}
        };
        template <>
        struct TreeBase<NoInit, false> {
            TreeBase(NoInit) {}
        };
        template <>
        struct TreeBase<NoInit, true> {
            template <typename InitMapping>
            TreeBase(InitMapping mapping) {}
        };
        template <typename Node, typename RangeMapping, bool Complete, typename SizeType, index_type MAX_TREENODE, index_type MAX_NODE>
        struct Tree : TreeBase<RangeMapping, Complete> {
            using value_type = typename Node::value_type;
            using modify_type = typename Node::modify_type;
            struct node : Node {
                index_type m_lchild, m_rchild;
                bool is_null() const { return this == s_buffer; }
                node *lchild() const { return s_buffer + m_lchild; }
                node *rchild() const { return s_buffer + m_rchild; }
            };
            static node s_buffer[MAX_NODE];
            static index_type s_use_count;
            node *m_tree_root;
            index_type m_row;
            SizeType m_column;
            static SizeType _lowbit(SizeType x) { return x & -x; }
            index_type _newnode(index_type row, SizeType column_floor, SizeType column_ceil) const {
                if constexpr (!Complete && !std::is_same<RangeMapping, NoInit>::value) s_buffer[s_use_count].set(TreeBase<RangeMapping, Complete>::m_range_mapping(row - _lowbit(row + 1) + 1, row, column_floor, column_ceil));
                return s_use_count++;
            }
            template <typename InitMapping>
            void _initnode(node *cur, index_type row, SizeType column_floor, SizeType column_ceil, InitMapping mapping) {
                if (column_floor == column_ceil) {
                    if constexpr (!std::is_same<InitMapping, NoInit>::value) cur->set(cur->get() + mapping(row, column_floor));
                } else {
                    SizeType mid = (column_floor + column_ceil) >> 1;
                    if (!cur->m_lchild) cur->m_lchild = _newnode(row, column_floor, mid);
                    _initnode(cur->lchild(), row, column_floor, mid, mapping);
                    if (!cur->m_rchild) cur->m_rchild = _newnode(row, mid + 1, column_ceil);
                    _initnode(cur->rchild(), row, mid + 1, column_ceil, mapping);
                    cur->set(cur->lchild()->get() + cur->rchild()->get());
                }
            }
            template <typename InitMapping>
            void _initnode(node *cur, node *parent, index_type row, SizeType column_floor, SizeType column_ceil, InitMapping mapping) {
                if (column_floor == column_ceil) {
                    if constexpr (!std::is_same<InitMapping, NoInit>::value) cur->set(cur->get() + mapping(row, column_floor));
                    parent->set(parent->get() + cur->get());
                } else {
                    SizeType mid = (column_floor + column_ceil) >> 1;
                    if (!cur->m_lchild) cur->m_lchild = _newnode(row, column_floor, mid);
                    if (!parent->m_lchild) parent->m_lchild = _newnode(row + _lowbit(row + 1), column_floor, mid);
                    _initnode(cur->lchild(), parent->lchild(), row, column_floor, mid, mapping);
                    if (!cur->m_rchild) cur->m_rchild = _newnode(row, mid + 1, column_ceil);
                    if (!parent->m_rchild) parent->m_rchild = _newnode(row + _lowbit(row + 1), mid + 1, column_ceil);
                    _initnode(cur->rchild(), parent->rchild(), row, mid + 1, column_ceil, mapping);
                    cur->set(cur->lchild()->get() + cur->rchild()->get());
                }
            }
            template <typename InitMapping>
            void _inittreenode(InitMapping mapping) {
                for (index_type i = 0; i < m_row; i++) {
                    index_type j = i + _lowbit(i + 1);
                    if (j < m_row)
                        _initnode(m_tree_root + i, m_tree_root + j, i, 0, m_column - 1, mapping);
                    else
                        _initnode(m_tree_root + i, i, 0, m_column - 1, mapping);
                }
            }
            node *_lchild(node *cur, index_type row, SizeType column_floor, SizeType column_ceil, SizeType mid) const {
                if constexpr (!Complete)
                    if (!cur->m_lchild) cur->m_lchild = _newnode(row, column_floor, mid);
                return cur->lchild();
            }
            node *_rchild(node *cur, index_type row, SizeType column_floor, SizeType column_ceil, SizeType mid) const {
                if constexpr (!Complete)
                    if (!cur->m_rchild) cur->m_rchild = _newnode(row, mid + 1, column_ceil);
                return cur->rchild();
            }
            void _add(node *cur, index_type row, SizeType column_floor, SizeType column_ceil, SizeType j, const modify_type &modify) {
                cur->set(cur->get() + modify);
                if (column_floor < column_ceil) {
                    SizeType mid = (column_floor + column_ceil) >> 1;
                    if (j <= mid)
                        _add(_lchild(cur, row, column_floor, column_ceil, mid), row, column_floor, mid, j, modify);
                    else
                        _add(_rchild(cur, row, column_floor, column_ceil, mid), row, mid + 1, column_ceil, j, modify);
                }
            }
            value_type _modify(node *cur, index_type row, SizeType column_floor, SizeType column_ceil, SizeType j, const value_type &val) {
                value_type inc;
                if (column_floor == column_ceil)
                    inc = val - cur->get();
                else {
                    SizeType mid = (column_floor + column_ceil) >> 1;
                    if (j <= mid)
                        inc = _modify(_lchild(cur, row, column_floor, column_ceil, mid), row, column_floor, mid, j, val);
                    else
                        inc = _modify(_rchild(cur, row, column_floor, column_ceil, mid), row, mid + 1, column_ceil, j, val);
                }
                cur->set(cur->get() + inc);
                return inc;
            }
            value_type _query(node *cur, index_type row, SizeType column_floor, SizeType column_ceil, SizeType j) const {
                if (column_floor == column_ceil) return cur->get();
                SizeType mid = (column_floor + column_ceil) >> 1;
                if (j <= mid) {
                    if (!cur->m_lchild)
                        if constexpr (!std::is_same<RangeMapping, NoInit>::value)
                            return TreeBase<RangeMapping, Complete>::m_range_mapping(row - _lowbit(row + 1) + 1, row, j, j);
                        else
                            return 0;
                    return _query(cur->lchild(), row, column_floor, mid, j);
                } else {
                    if (!cur->m_rchild)
                        if constexpr (!std::is_same<RangeMapping, NoInit>::value)
                            return TreeBase<RangeMapping, Complete>::m_range_mapping(row - _lowbit(row + 1) + 1, row, j, j);
                        else
                            return 0;
                    return _query(cur->rchild(), row, mid + 1, column_ceil, j);
                }
            }
            value_type _query(node *cur, index_type row, SizeType column_floor, SizeType column_ceil, SizeType column1, SizeType column2) const {
                if (column1 <= column_floor && column2 >= column_ceil) return cur->get();
                SizeType mid = (column_floor + column_ceil) >> 1;
                value_type lval = 0, rval = 0;
                if (column1 <= mid) {
                    if (cur->m_lchild)
                        lval = _query(cur->lchild(), row, column_floor, mid, column1, column2);
                    else if constexpr (!std::is_same<RangeMapping, NoInit>::value)
                        lval = TreeBase<RangeMapping, Complete>::m_range_mapping(row - _lowbit(row + 1) + 1, row, std::max(column_floor, column1), mid);
                }
                if (column2 > mid) {
                    if (cur->m_rchild)
                        rval = _query(cur->rchild(), row, mid + 1, column_ceil, column1, column2);
                    else if constexpr (!std::is_same<RangeMapping, NoInit>::value)
                        rval = TreeBase<RangeMapping, Complete>::m_range_mapping(row - _lowbit(row + 1) + 1, mid + 1, std::min(column_ceil, column2));
                } else
                    return lval;
                if (column1 > mid) return rval;
                return lval + rval;
            }
            Tree(index_type row = 0, SizeType column = 0, RangeMapping mapping = RangeMapping()) : TreeBase<RangeMapping, Complete>::TreeBase(mapping) { resize(row, column); }
            template <typename InitMapping = NoInit>
            Tree(index_type row, SizeType column, InitMapping mapping = InitMapping()) : TreeBase<RangeMapping, Complete>::TreeBase(NoInit()) { resize(row, column, mapping); }
            template <typename InitMapping = NoInit>
            void resize(index_type row, SizeType column, InitMapping mapping = InitMapping()) {
                if ((m_row = row) && (m_column = column)) {
                    m_tree_root = s_buffer + s_use_count, s_use_count += m_row;
                    if constexpr (Complete || !std::is_same<InitMapping, NoInit>::value) _inittreenode(mapping);
                }
            }
            void add(index_type i, SizeType j, const modify_type &inc) {
                while (i < m_row) _add(m_tree_root + i, i, 0, m_column - 1, j, inc), i += _lowbit(i + 1);
            }
            void modify(index_type i, SizeType j, const value_type &val) {
                while (i < m_row) _modify(m_tree_root + i, i, 0, m_column - 1, j, val), i += _lowbit(i + 1);
            }
            value_type presum(index_type i, SizeType j) const {
                value_type res{};
                while (~i) res += _query(m_tree_root + i, i, 0, m_column - 1, j), i -= _lowbit(i + 1);
                return res;
            }
            value_type query(index_type i, SizeType j) const {
                const index_type rend = i - _lowbit(i + 1);
                value_type res{};
                for (index_type k = i; k != rend; k -= _lowbit(k + 1)) res += _query(m_tree_root + k, k, 0, m_column - 1, j);
                for (index_type k = i - 1; k != rend; k -= _lowbit(k + 1)) res -= _query(m_tree_root + k, k, 0, m_column - 1, j);
                return res;
            }
            value_type presum(index_type i, SizeType column1, SizeType column2) const {
                value_type res{};
                while (~i) res += _query(m_tree_root + i, i, 0, m_column - 1, column1, column2), i -= _lowbit(i + 1);
                return res;
            }
            value_type query(index_type row1, index_type row2, SizeType column1, SizeType column2) const {
                const index_type rend = (row1 & ~(std::bit_ceil((row1 ^ (row2 + 1)) + 1) - 1)) - 1;
                value_type res{};
                for (index_type k = row2; k != rend; k -= _lowbit(k + 1)) res += _query(m_tree_root + k, k, 0, m_column - 1, column1, column2);
                for (index_type k = row1 - 1; k != rend; k -= _lowbit(k + 1)) res -= _query(m_tree_root + k, k, 0, m_column - 1, column1, column2);
                return res;
            }
            index_type kth(index_type row1, index_type row2, value_type k) const {
                static node *buffer_plus[64], *buffer_minus[64];
                index_type count_plus = 0, count_minus = 0, low = 0, high = m_column - 1, rend = (row1 & ~(std::bit_ceil((row1 ^ (row2 + 1)) + 1) - 1)) - 1;
                for (index_type k = row2; k != rend; k -= _lowbit(k + 1)) buffer_plus[count_plus++] = m_tree_root + k;
                for (index_type k = row1 - 1; k != rend; k -= _lowbit(k + 1)) buffer_minus[count_minus++] = m_tree_root + k;
                while (low < high) {
                    value_type cnt = 0;
                    for (index_type i = 0; i < count_plus; i++) cnt += buffer_plus[i]->lchild()->get();
                    for (index_type i = 0; i < count_minus; i++) cnt -= buffer_minus[i]->lchild()->get();
                    if (cnt <= k) {
                        k -= cnt, low = (low + high) / 2 + 1;
                        index_type i = 0;
                        for (index_type j = 0; j < count_plus; j++)
                            if (buffer_plus[j]->m_rchild) buffer_plus[i++] = buffer_plus[j]->rchild();
                        count_plus = i, i = 0;
                        for (index_type j = 0; j < count_minus; j++)
                            if (buffer_minus[j]->m_rchild) buffer_minus[i++] = buffer_minus[j]->rchild();
                        count_minus = i;
                    } else {
                        high = (low + high) / 2;
                        index_type i = 0;
                        for (index_type j = 0; j < count_plus; j++)
                            if (buffer_plus[j]->m_lchild) buffer_plus[i++] = buffer_plus[j]->lchild();
                        count_plus = i, i = 0;
                        for (index_type j = 0; j < count_minus; j++)
                            if (buffer_minus[j]->m_lchild) buffer_minus[i++] = buffer_minus[j]->lchild();
                        count_minus = i;
                    }
                }
                return low;
            }
        };
        template <typename Ostream, typename Node, typename RangeMapping, bool Complete, typename SizeType, index_type MAX_TREENODE, index_type MAX_NODE>
        Ostream &operator<<(Ostream &out, const Tree<Node, RangeMapping, Complete, SizeType, MAX_TREENODE, MAX_NODE> &x) {
            out << "[";
            for (index_type i = 0; i < x.m_row; i++)
                for (SizeType j = 0; j < x.m_column; j++) out << (j ? " " : (i ? ", [" : "[")) << x.query(i, j) << (j == x.m_column - 1 ? ']' : ',');
            return out << "]";
        }
        template <typename Node, typename RangeMapping, bool Complete, typename SizeType, index_type MAX_TREENODE, index_type MAX_NODE>
        typename Tree<Node, RangeMapping, Complete, SizeType, MAX_TREENODE, MAX_NODE>::node Tree<Node, RangeMapping, Complete, SizeType, MAX_TREENODE, MAX_NODE>::s_buffer[MAX_NODE];
        template <typename Node, typename RangeMapping, bool Complete, typename SizeType, index_type MAX_TREENODE, index_type MAX_NODE>
        index_type Tree<Node, RangeMapping, Complete, SizeType, MAX_TREENODE, MAX_NODE>::s_use_count = 1;
    }
    template <bool Complete = false, typename SizeType = uint32_t, SegBIT::index_type MAX_TREENODE = 1 << 20, SegBIT::index_type MAX_NODE = 1 << 23>
    using SegBITSumTree = SegBIT::Tree<SegBIT::BaseNode<int64_t>, SegBIT::NoInit, Complete, SizeType, MAX_TREENODE, MAX_NODE>;
}

#endif