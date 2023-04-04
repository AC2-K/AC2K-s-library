#pragma once
#include"math/dynamic_modint.hpp"
#include"math/rho.hpp"
inline uint64_t primitive_root(uint64_t p) {
    if (p == 2) return 1;
    auto pf = prime::rho::factorize(p - 1);
    pf.erase(unique(all(pf)), pf.end());
    for (auto& q : pf) {
        q = (p - 1) / q;
    }
    using ull = unsigned long long;
    if (dynamic_modint<uint64_t, __uint128_t>::get_mod() != p) {
        dynamic_modint<uint64_t, __uint128_t>::set_mod(p);
    }
    static ull rng = 2020;
    while (1) {
        rng ^= rng << 13; rng ^= rng >> 7; rng ^= rng << 17;
        dynamic_modint<uint64_t, __uint128_t> g(rng);
        if (g.val() == 0) continue;
        bool is_ok = true;
        for (const auto& q : pf) {
            if (dynamic_modint<uint64_t, __uint128_t>(g).pow(q).val() == 1) {
                is_ok = false;
                break;
            }
        }
        if (is_ok) {
            return g.val();
        }
    }
}

//@brief primitive root(原始根)