#pragma once

#include <type_tuples.hpp>

namespace value_types
{

template<auto V>
struct ValueTag{ static constexpr auto Value = V; };

template<class T, T... ts>
using VTuple = type_tuples::TTuple<ValueTag<ts>...>;

}

// template<int N>
// consteval bool IsPrime (){
//     if constexpr (N <= 1) return false;
//     if constexpr (N == 2 || N == 3) return true;
//     else{
//         return type_lists::ListSize<
//         type_lists::Filter2P<
//             IsPrimeFilter, type_lists::Drop<2, type_lists::Take<SQRT1<N>(), Nats>>, value_types::ValueTag<N>
//             >
//         > == 0;
//     }
// };


// template<int N>
// consteval std::size_t SQRT1(){
//     if constexpr (N == 0){return std::size_t(0);}
//     if constexpr (N == 1){return std::size_t(1);}
//     if constexpr (N == 2){return std::size_t(2);}
//     if constexpr (N == 3){return std::size_t(2);}
//     std::size_t l = 1, r = N;
//     while(r - l > 1){
//         std::size_t m = (l + r) / 2;
//         if(m*m <= N) l = m;
//         else r = m;
//     }
//     return r;
// }