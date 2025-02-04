#pragma once
#include <cassert>
#include <utility>
#include "../data-structure/sparse_table.hpp"
#include "../internal/CSR.hpp"

namespace kyopro {

class EulerTour {
    int n;

    std::vector<std::pair<int, int>> es;
    std::vector<int> tour;
    std::vector<int> in, out, depth;

    struct get_min_pair {
        using value_t = std::pair<int, int>;
        static value_t op(value_t x, value_t y) { return std::min(x, y); }
    };

    sparse_table<get_min_pair::value_t, get_min_pair::op> rmq;

public:
    explicit EulerTour(int n)
        : n(n), in(n, -1), out(n, -1), depth(n, -1), rmq(2 * n - 1) {
        tour.reserve(2 * n);
        es.reserve(2 * (n - 1));
    }

    void add_edge(int u, int v) {
        assert(0 <= v && v < n);
        assert(0 <= u && u < n);
        es.emplace_back(u, v);
        es.emplace_back(v, u);
    }

    int get_depth(int v) const {
        assert(0 <= v && v < n);
        return depth[v];
    }

    void build(int r = 0) {
        internal::csr g(n, es);
        auto dfs = [&](const auto& self, int v, int p) -> void {
            in[v] = tour.size();
            tour.emplace_back(v);
            for (auto nv : g[v]) {
                if (nv != p) {
                    depth[nv] = depth[v] + 1;
                    self(self, nv, v);
                    tour.emplace_back(v);
                }
            }
            out[v] = tour.size() - 1;
        };
        dfs(dfs, r, -1);
        for (int i = 0; i < (int)tour.size(); i++) {
            rmq.set(i, {depth[tour[i]], tour[i]});
        }
        rmq.build();
    }

    std::pair<int, int> idx(int v) const {
        assert(0 <= v && v < n);
        return {in[v], out[v]};
    }
    
    int lca(int v, int u) const {
        assert(0 <= v && v < n);
        assert(0 <= u && u < n);
        if (u == v) return v;
        if (in[v] > in[u] + 1) {
            std::swap(u, v);
        }
        return rmq.fold(in[v], in[u] + 1).second;
    }

    int dist(int v, int u) const {
        assert(0 <= v && v < n);
        assert(0 <= u && u < n);
        int p = lca(v, u);
        return depth[v] + depth[u] - 2 * depth[p];
    }

    bool is_in_subtree(int par, int v) const {
        assert(0 <= par && par < n);
        assert(0 <= v && v < n);

        return (in[par] <= in[v] && out[v] <= out[par]);
    }
};
};  // namespace kyopro

/**
 * @brief Euler Tour
 * @docs docs/tree/EulerTour.md
 */