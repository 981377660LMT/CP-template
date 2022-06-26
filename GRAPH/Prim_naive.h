#ifndef __OY_PRIM_NAIVE__
#define __OY_PRIM_NAIVE__

#include <limits>
#include "Graph.h"

namespace OY {
    template <typename _Tp>
    struct Prim_naive {
        struct _RawEdge {
            uint32_t from, to;
            _Tp cost;
        };
        struct _Edge {
            uint32_t index, to;
            _Tp cost;
        };
        std::vector<_RawEdge> m_rawEdges;
        std::vector<_Edge> m_edges;
        std::vector<bool> m_used;
        std::vector<uint32_t> m_starts;
        uint32_t m_vertexNum;
        Prim_naive(uint32_t __vertexNum, uint32_t __edgeNum) : m_starts(__vertexNum + 1, 0), m_vertexNum(__vertexNum) { m_rawEdges.reserve(__edgeNum); }
        void addEdge(uint32_t __a, uint32_t __b, _Tp __cost) { m_rawEdges.push_back({__a, __b, __cost}); }
        void prepare() {
            for (auto &[from, to, cost] : m_rawEdges)
                if (from != to) {
                    m_starts[from + 1]++;
                    m_starts[to + 1]++;
                }
            std::partial_sum(m_starts.begin(), m_starts.end(), m_starts.begin());
            m_edges.resize(m_starts.back());
            uint32_t cursor[m_vertexNum];
            std::copy(m_starts.begin(), m_starts.begin() + m_vertexNum, cursor);
            for (uint32_t index = 0; index < m_rawEdges.size(); index++)
                if (auto &[from, to, cost] = m_rawEdges[index]; from != to) {
                    m_edges[cursor[from]++] = _Edge{index, to, cost};
                    m_edges[cursor[to]++] = _Edge{index, from, cost};
                }
        }
        bool calc(_Tp __infiniteCost = std::numeric_limits<_Tp>::max() / 2) {
            m_used.resize(m_rawEdges.size(), false);
            struct _edge {
                uint32_t index;
                _Tp cost;
                bool operator<(const _edge &other) const { return cost < other.cost; }
            } closest[m_vertexNum];
            std::fill(closest, closest + m_vertexNum, _edge{uint32_t(-1), __infiniteCost});
            std::vector<bool> visit(m_vertexNum, false);
            uint32_t lastUpdate = 0;
            while (true) {
                visit[lastUpdate] = true;
                closest[lastUpdate] = _edge{uint32_t(-1), __infiniteCost};
                for (uint32_t cur = m_starts[lastUpdate], end = m_starts[lastUpdate + 1]; cur < end; cur++) {
                    auto &[index, to, cost] = m_edges[cur];
                    if (!visit[to]) chmin(closest[to], _edge{index, cost});
                }
                lastUpdate = std::min_element(closest, closest + m_vertexNum) - closest;
                if (!~closest[lastUpdate].index) break;
                m_used[closest[lastUpdate].index] = true;
            }
            return !std::count(visit.begin(), visit.end(), false);
        }
        _Tp totalCost() const {
            _Tp res = 0;
            for (uint32_t index = 0; index < m_used.size(); index++)
                if (m_used[index]) res += m_rawEdges[index].cost;
            return res;
        }
    };
}

#endif