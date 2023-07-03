#pragma once
#include <cassert>
#include <memory>
#include <utility>
#include "../../random/xor_shift.hpp"

namespace kyopro {
/**
 * @brief 反転可能平衡二分探索木
 * @tparam S モノイド
 * @tparam op Sの二項演算
 * @tparam e Sの単位元
 */
template <class S, S (*op)(S, S), S (*e)()> class reversible_bbst {
    using u32 = uint32_t;
    xor_shift32 rng;
    struct Node {
        std::unique_ptr<Node> l, r;
        u32 priority;
        S value, prod;
        int size;
        bool rev;

        Node(S v, u32 prio)
            : l(),
              r(),
              priority(prio),
              value(v),
              prod(v),
              size(1),
              rev(false) {}
    };

    using ptr = std::unique_ptr<Node>;
    int size(const ptr& p) const { return p ? p->size : 0; }
    S fold(const ptr& p) { return p ? p->prod : e(); }

    void reverse(const ptr& p) {
        if (p) {
            p->rev ^= 1;
        }
    }

    void push(const ptr& p) {
        if (p->rev) {
            p->rev = false;
            std::swap(p->l, p->r);
            reverse(p->l), reverse(p->r);
        }
    }

    void update(const ptr& p) {
        p->size = size(p->l) + size(p->r) + 1;
        p->prod = op(p->value, op(fold(p->l), fold(p->r)));
    }

    std::pair<ptr, ptr> split(ptr p, int k) {
        if (!p) return {nullptr, nullptr};

        push(p);
        int s = size(p->l);
        if (s >= k) {
            auto [l, r] = split(std::move(p->l), k);
            p->l = std::move(r);
            update(p);

            return {std::move(l), std::move(p)};
        } else {
            auto [l, r] = split(std::move(p->r), k - s - 1);

            p->r = std::move(l);
            update(p);

            return {std::move(p), std::move(r)};
        }
    }

    ptr merge(ptr l, ptr r) {
        if (!l) return r;
        if (!r) return l;

        if (l->priority < r->priority) {
            push(r);
            r->l = merge(std::move(l), std::move(r->l));
            update(r);
            return r;
        } else {
            push(l);
            l->r = merge(std::move(l->r), std::move(r));
            update(l);

            return l;
        }
    }

    ptr root = nullptr;

public:
    constexpr explicit reversible_bbst() : rng(2023) {}
    void insert(int i, S a) {
        auto [l, r] = split(std::move(root), i);
        ptr item = std::make_unique<Node>(a, rng());
        root = merge(std::move(l), std::move(item));
        root = merge(std::move(root), std::move(r));
    }
    S fold(int l, int r) {
        assert(0 <= l && l <= r && r <= size(root));
        auto [xy, z] = split(std::move(root), r);
        auto [x, y] = split(std::move(xy), l);
        auto res = fold(y);
        xy = merge(std::move(x), std::move(y));
        root = merge(std::move(xy), std::move(z));
        return res;
    }
    void reverse(int l, int r) {
        assert(0 <= l && l <= r && r <= size(root));
        auto [xy, z] = split(std::move(root), r);
        auto [x, y] = split(std::move(xy), l);
        reverse(y);
        xy = merge(std::move(x), std::move(y));
        root = merge(std::move(xy), std::move(z));
    }
};
};  // namespace kyopro

/**
 * @docs docs/data-structure/bbst/reversible_bbst.md
 * @ref
 * https://github.com/yosupo06/library-checker-problems/blob/master/datastructure/range_reverse_range_sum/sol/correct.cpp
 */