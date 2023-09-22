/*
最后修改:
20230922
测试环境:
gcc11.2,c++11
clang12.0,C++11
msvc14.2,C++14
*/
#ifndef __OY_HEAVYLIGHTDECOMPOSITION__
#define __OY_HEAVYLIGHTDECOMPOSITION__

#include <algorithm>
#include <cstdint>
#include <functional>
#include <numeric>

namespace OY {
    namespace HLD {
        using size_type = uint32_t;
        template <typename Tree, size_type MAX_VERTEX>
        struct Table {
            struct node {
                size_type m_top_dfn, m_top_dep, m_parent, m_dfn, m_dep, m_size, m_heavy;
            };
            static node s_buffer[MAX_VERTEX];
            static size_type s_seq_buffer[MAX_VERTEX], s_use_count;
            Tree *m_rooted_tree;
            node *m_info;
            size_type *m_seq, m_cursor;
            void _tree_dfs1(size_type a, size_type p) {
                m_info[a].m_size = 1, m_info[a].m_heavy = -1;
                m_rooted_tree->do_for_each_adj_vertex(a, [&](size_type to) {
                    if (to != p) {
                        _tree_dfs1(to, a);
                        size_type to_size = m_info[to].m_size;
                        m_info[a].m_size += to_size;
                        if (!~m_info[a].m_heavy || to_size > m_info[m_info[a].m_heavy].m_size) m_info[a].m_heavy = to;
                    }
                });
            }
            void _tree_dfs2(size_type a, size_type p) {
                m_seq[m_cursor] = a;
                if (~p) m_info[a].m_dep = m_info[p].m_dep + 1;
                m_info[a].m_dfn = m_cursor++;
                if (~p && a == m_info[p].m_heavy)
                    m_info[a].m_top_dfn = m_info[p].m_top_dfn, m_info[a].m_top_dep = m_info[p].m_top_dep, m_info[a].m_parent = m_info[p].m_parent;
                else
                    m_info[a].m_top_dfn = m_info[a].m_dfn, m_info[a].m_top_dep = m_info[a].m_dep, m_info[a].m_parent = p;
                size_type heavy = m_info[a].m_heavy;
                if (~heavy) _tree_dfs2(heavy, a);
                m_rooted_tree->do_for_each_adj_vertex(a, [&](size_type to) { if (to != p && to != heavy) _tree_dfs2(to, a); });
            }
            Table(Tree *rooted_tree = nullptr) { reset(rooted_tree); }
            void reset(Tree *rooted_tree) {
                if (!(m_rooted_tree = rooted_tree)) return;
                m_info = s_buffer + s_use_count, m_seq = s_seq_buffer + s_use_count, m_cursor = 0, s_use_count += m_rooted_tree->vertex_cnt();
                _tree_dfs1(m_rooted_tree->m_root, -1);
                _tree_dfs2(m_rooted_tree->m_root, -1);
            }
            size_type get_ancestor(size_type a, size_type n) const {
                if (n > m_info[a].m_dep) return -1;
                size_type dep = m_info[a].m_dep, target_dep = dep - n;
                while (target_dep < m_info[a].m_dep) dep = m_info[a].m_dep - 1, a = m_info[a].m_parent;
                return m_seq[m_info[a].m_dfn - dep + target_dep];
            }
            template <bool LCA, typename Callback>
            void do_for_path(size_type a, size_type b, Callback &&call) const {
                while (m_info[a].m_top_dfn != m_info[b].m_top_dfn) {
                    if (m_info[a].m_top_dep < m_info[b].m_top_dep)
                        call(m_info[b].m_top_dfn, m_info[b].m_dfn), b = m_info[b].m_parent;
                    else
                        call(m_info[a].m_top_dfn, m_info[a].m_dfn), a = m_info[a].m_parent;
                }
                if (m_info[a].m_dep > m_info[b].m_dep) std::swap(a, b);
                if constexpr (LCA)
                    call(m_info[a].m_dfn, m_info[b].m_dfn);
                else if (a != b)
                    call(m_info[a].m_dfn + 1, m_info[b].m_dfn);
            }
            template <typename Callback>
            void do_for_subtree(size_type a, Callback &&call) const { call(m_info[a].m_dfn, m_info[a].m_dfn + m_info[a].m_size - 1); }
            size_type calc(size_type a, size_type b) const {
                while (m_info[a].m_top_dfn != m_info[b].m_top_dfn)
                    if (m_info[a].m_top_dep < m_info[b].m_top_dep)
                        b = m_info[b].m_parent;
                    else
                        a = m_info[a].m_parent;
                return m_info[a].m_dep < m_info[b].m_dep ? a : b;
            }
        };
        template <typename Tree, size_type MAX_VERTEX>
        typename Table<Tree, MAX_VERTEX>::node Table<Tree, MAX_VERTEX>::s_buffer[MAX_VERTEX];
        template <typename Tree, size_type MAX_VERTEX>
        size_type Table<Tree, MAX_VERTEX>::s_seq_buffer[MAX_VERTEX];
        template <typename Tree, size_type MAX_VERTEX>
        size_type Table<Tree, MAX_VERTEX>::s_use_count;
    }
}

#endif