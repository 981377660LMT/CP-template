#ifndef __OY_BFS__
#define __OY_BFS__

#include "Graph.h"

namespace OY {
    template <typename _Graph, typename _Tp = typename _Graph::value_type>
    struct BFS {
        _Graph &m_graph;
        std::vector<_Tp> m_distance;
        std::vector<uint32_t> m_from;
        std::vector<uint32_t> m_queue;
        uint32_t m_cursor;
        BFS(_Graph &__graph, _Tp __initDistance = std::numeric_limits<_Tp>::max() / 2) : m_graph(__graph), m_distance(__graph.m_vertexNum, __initDistance), m_from(__graph.m_vertexNum, -1), m_cursor(0) { m_queue.reserve(m_graph.m_vertexNum); }
        void init(uint32_t __i, _Tp __distance = 0) {
            m_distance[__i] = _Tp(0);
            m_queue.push_back(__i);
        }
        void calc() {
            while (m_cursor < m_queue.size()) {
                uint32_t cur = m_queue[m_cursor++];
                _Tp curDistance = m_distance[cur];
                for (auto [index, from, to, value] : m_graph.getEdgesOf(cur))
                    if (chmin(m_distance[to], curDistance + value)) {
                        m_queue.push_back(to);
                        m_from[to] = index;
                    }
            }
        }
        std::vector<uint32_t> getPath_edge(uint32_t __target) const {
            std::vector<uint32_t> path;
            for (uint32_t cur = __target; ~m_from[cur]; cur = m_graph.getEdge(m_from[cur]).from) path.push_back(m_from[cur]);
            std::reverse(path.begin(), path.end());
            return path;
        }
        std::vector<uint32_t> getPath_vertex(uint32_t __target) const {
            std::vector<uint32_t> path;
            path.push_back(__target);
            for (uint32_t cur = __target; ~m_from[cur];) path.push_back(cur = m_graph.getEdge(m_from[cur]).from);
            std::reverse(path.begin(), path.end());
            return path;
        }
    };
}

#endif