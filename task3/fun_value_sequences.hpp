#pragma once

#include <value_types.hpp>
#include <type_lists.hpp>

template <int N>
struct NutsHelper {
    using Head = value_types::ValueTag<N>;
    using Tail = NutsHelper<N + 1>;
};

using Nats = NutsHelper<0>;

template <int N1, int N2>
struct FibHelper {
    using Head = value_types::ValueTag<N1>;
    using Tail = FibHelper<N2, N1 + N2>;
};

using Fib = FibHelper<0, 1>;




template<typename V, typename T>
struct IsPrimeFilter;

template<int N, int I>
struct IsPrimeFilter<value_types::ValueTag<N>, value_types::ValueTag<I>> { static constexpr bool Value = (N % I == 0); };



template<std::size_t L, std::size_t R, int N>
struct SQRT2Helper;

template<std::size_t L, std::size_t R, int N> requires(R - L > 1 && ((R + L) / 2)*((R + L) / 2) <= N )
struct SQRT2Helper<L, R, N>{
    static constexpr std::size_t Root = SQRT2Helper<(R + L) / 2, R, N>::Root;
};

template<std::size_t L, std::size_t R, int N> requires(R - L > 1 && ((R + L) / 2)*((R + L) / 2) > N )
struct SQRT2Helper<L, R, N>{
    static constexpr std::size_t Root = SQRT2Helper<L, (R + L) / 2, N>::Root;
};

template<std::size_t L, std::size_t R, int N> requires(R - L <= 1)
struct SQRT2Helper<L, R, N>{
    static constexpr std::size_t Root = R;
};


template<int N>
constexpr std::size_t SQRT2 = SQRT2Helper<1, std::size_t(N), N>::Root;

template<>
constexpr std::size_t SQRT2<0> = 0;
template<>
constexpr std::size_t SQRT2<1> = 1;
template<>
constexpr std::size_t SQRT2<2> = 2;
template<>
constexpr std::size_t SQRT2<3> = 2;

template<typename T>
struct PrimesFilter;

template<int N>
struct PrimesFilter<value_types::ValueTag<N>> { static constexpr bool Value = (N != 0) && (N != 1) && ( (N == 2 || N == 3) ||
(
    type_lists::ListSize<
        type_lists::Filter2P<
            IsPrimeFilter, type_lists::Drop<2, type_lists::Take<SQRT2<N>, Nats>>, value_types::ValueTag<N>
            >
        > == 0
)
)
; };

using Primes = type_lists::Filter<PrimesFilter, Nats>;

