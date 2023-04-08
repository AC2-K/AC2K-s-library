#pragma once
#include "../data-structure/sparse_table.hpp"
namespace kyopro {
/// @brief EulerTour(オイラーツアー)
class EulerTour {
    int n;
    std::vector<std::vector<int>> g;
    std::vector<int> tour;
    std::vector<int> in, out, depth;
    sparse_table<std::pair<int, int>> rmq;

  public:
    EulerTour(int n)
        : n(n), g(n), in(n, -1), out(n, -1), depth(n, -1), rmq(2 * n - 1) {
        tour.reserve(2 * n - 1);
    }
    inline void add_edge(int u, int v) {
        g[u].emplace_back(v);
        g[v].emplace_back(u);
    }
    inline std::vector<std::vector<int>> get_graph() { return g; }
    inline std::vector<int> get_tour() { return tour; }
    inline int get_depth(int v) const { return depth[v]; }

  public:
    void build(int r = 0) {
        auto dfs = [&](auto self, int v, int p) -> void {
            in[v] = tour.size();
            tour.emplace_back(v);
            for (const auto& nv : g[v])
                if (nv != p) {
                    depth[nv] = depth[v] + 1;
                    self(self, nv, v);
                    tour.emplace_back(v);
                }
            out[v] = tour.size() - 1;
        };
        dfs(dfs, r, -1);
        for (int i = 0; i < (int)tour.size(); i++) {
            rmq.set(i, {depth[tour[i]], tour[i]});
        }
        rmq.build();
    }

    inline std::pair<int, int> idx(int v) { return {in[v], out[v]}; }
    int lca(int v, int u) {
        if (in[v] > in[u] + 1) {
            std::swap(u, v);
        }
        return rmq.prod(in[v], in[u] + 1).second;
    }

    inline int dist(int v, int u) {
        int p = lca(v, u);
        return depth[v] + depth[u] - 2 * depth[p];
    }

    inline bool is_in_subtree(int par, int v) {
        return (in[par] <= in[v] && out[v] <= out[par]);
    }
};
};  // namespace kyopro