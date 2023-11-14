#pragma once

#include <concepts>

#include "type_tuples.hpp"


namespace type_lists
{

template<typename TL>
concept TypeSequence =
    requires {
        typename TL::Head;
        typename TL::Tail;
    };

struct Nil {};

template<typename TL>
concept Empty = std::derived_from<TL, Nil>;

template<typename TL>
concept TypeList = Empty<TL> || TypeSequence<TL>;

template<typename T, TypeList TL>
struct Cons {
    using Head = T;
    using Tail = TL;
};





template <TypeList TL, std::size_t N>
struct SizeHelper {
    static constexpr std::size_t Sz = SizeHelper<typename TL::Tail, N + 1>::Sz;
};

template <Empty TL, std::size_t N>
struct SizeHelper<TL, N> {
    static constexpr std::size_t Sz = N;
};
template<TypeList TL>
constexpr std::size_t ListSize = SizeHelper<TL, 0>::Sz;





////////////////////////////////////////////////////////////////////////////

template<type_tuples::TypeTuple>
struct FromTuple;

template<typename T, typename... Args>
struct FromTuple<type_tuples::TTuple<T, Args...>> { 
    using Head = T;
    using Tail = FromTuple<type_tuples::TTuple<Args...>>;
};

template<>
struct FromTuple<type_tuples::TTuple<>> : Nil{};

/////////////////////////////////////////////////////////////////

template <TypeList TL, typename ... Accumulated>
struct ToTupleHelper;

template <TypeSequence TL, typename... Accumulated>
struct ToTupleHelper<TL, Accumulated...>{
    using V = typename ToTupleHelper<typename TL::Tail, Accumulated..., typename TL::Head>::V;
};

template<Empty TL, typename ... Accumulated>
struct ToTupleHelper<TL, Accumulated ...> {
    using V = typename type_tuples::TTuple<Accumulated ...>;
};

template<TypeList TL>
using ToTuple = typename ToTupleHelper<TL>::V;

template<typename T>
struct Repeat {
    using Head = T;
    using Tail = Repeat<T>;
};

template <std::size_t N, TypeList TL>
struct Take { 
    using Head = typename TL::Head;
    using Tail = Take<N - 1, typename TL::Tail>;
};

template<TypeSequence TL>
struct Take<0, TL> : Nil{};

template<std::size_t N, Empty TL>
struct Take<N, TL> : Nil{};  



template<std::size_t N, TypeList TL>
struct Drop : Drop<N - 1, typename TL::Tail> {};

template<TypeSequence TL>
struct Drop<0, TL> {
    using Head = typename TL::Head;
    using Tail = typename TL::Tail;
};

template<std::size_t N, Empty TL>
struct Drop<N, TL> : Nil{};

// Your fun, fun metaalgorithms :)


///***///

template<std::size_t N, class T>
struct Replicate {
    using Head = T;
    using Tail = Replicate<N - 1, T>;
};

template<class T>
struct Replicate<0, T> : Nil {};

///***///

template<template<typename> typename F, TypeList TL>
struct Map{
    using Head = F<typename TL::Head>;
    using Tail = Map<F, typename TL::Tail>;
};

template<template<typename> typename F, Empty TL>
struct Map<F, TL> : Nil {};


template<template<typename> typename P, TypeList TL>
struct Filter;

template<template<typename> typename P, TypeList TL> requires(!P<typename TL::Head>::Value)
struct Filter<P, TL> : Filter<P, typename TL::Tail>{};

template<template<typename> typename P, TypeList TL> requires(P<typename TL::Head>::Value)
struct Filter<P, TL> {
    using Head = typename TL::Head;
    using Tail = Filter<P, typename TL::Tail>;
};

template<template<typename> typename P, Empty TL>
struct Filter<P, TL> : Nil{};



template<template<typename> typename F, class T>
struct Iterate {
    using Head = T;
    using Tail = Iterate<F, F<T>>;
};

///


///

template<TypeList TL, TypeList TLBuf>
struct CycleHelper;

template <TypeList TL, Empty TLBuf>
struct CycleHelper<TL, TLBuf> : Nil{};

template <TypeSequence TL, TypeSequence TLBuf>
struct CycleHelper<TL, TLBuf>{
    using Head = typename TL::Head;
    using Tail = CycleHelper<typename TL::Tail, TLBuf>;
};

template <Empty TL, TypeSequence TLBuf>
struct CycleHelper<TL, TLBuf>{
    using Head = typename TLBuf::Head;
    using Tail = CycleHelper<typename TLBuf::Tail, TLBuf>;
};

template<TypeList TL>
using Cycle = CycleHelper<TL, TL>;




template <TypeList TL, std::size_t N, TypeList TLBuf>
struct InitsHelper{
    using Head = Take<N, TLBuf>;
    using Tail = InitsHelper<typename TL::Tail, N + 1, TLBuf>;
};

template <Empty TL, std::size_t N, TypeList TLBuf>
struct InitsHelper<TL, N, TLBuf> {
    using Head = TLBuf;
    using Tail = Nil;
};
template<TypeList TL>
using Inits = InitsHelper<TL, 0, TL>;



template <TypeList TL, std::size_t N, TypeList TLBuf>
struct TailsHelper{
    using Head = Drop<N, TLBuf>;
    using Tail = TailsHelper<typename TL::Tail, N + 1, TLBuf>;
};

template <Empty TL, std::size_t N, TypeList TLBuf>
struct TailsHelper<TL, N, TLBuf>{
    using Head = Nil;
    using Tail = Nil;
};
template<TypeList TL>
using Tails = TailsHelper<TL, 0, TL>;




///////////////////


template<template<typename, typename> typename OP, class T, TypeList TL>
struct Scanl {
    using Head = T;
    using Tail = Scanl<OP, OP<T, typename TL::Head>, typename TL::Tail>;
};

template<template<typename, typename> typename OP, class T, Empty TL>
struct Scanl<OP, T, TL> {
    using Head = T;
    using Tail = Nil;
};


template<template<typename, typename> typename OP, class T, TypeList TL>
struct FoldlHelper {
    using V = typename FoldlHelper<OP, OP<T, typename TL::Head>, typename TL::Tail>::V;
};

template<template<typename, typename> typename OP, class T, Empty empty>
struct FoldlHelper<OP, T, empty> {
    using V = T;
};

template<template<typename, typename> typename OP, class T, TypeList TL>
using Foldl = typename FoldlHelper<OP, T, TL>::V;


//

template<TypeList L, TypeList R>
struct Zip2 {
    using Head = type_tuples::TTuple<typename L::Head, typename R::Head>;
    using Tail = Zip2<typename L::Tail, typename R::Tail>;
};

template<TypeList L, TypeList R> requires(Empty<L> || Empty<R>)
struct Zip2<L, R> : Nil {};

//

template<TypeList TL>
using getHead = typename TL::Head;

template<TypeList TL>
using getTail = typename TL::Tail;

template<TypeList TL>
struct ZipHelper {
    using Head = ToTuple<Map<getHead, TL>>;
    using Tail = ZipHelper<Map<getTail, TL>>;
};

template<Empty TL>
struct ZipHelper<TL> : Nil {};


template<TypeList... TL>
using Zip = ZipHelper<FromTuple<type_tuples::TTuple<TL...>>>;


///

template <template<typename, typename> typename EQ, TypeList TL, TypeList TLBuf, std::size_t N>
struct GroupByHelper;


template <template<typename, typename> typename EQ, TypeList TL, Empty TLBuf, std::size_t N>
struct GroupByHelper<EQ, TL, TLBuf, N> : Nil{};

template <template<typename, typename> typename EQ, Empty TL, TypeSequence TLBuf, std::size_t N>
struct GroupByHelper<EQ, TL, TLBuf, N> : Nil{};

template <template<typename, typename> typename EQ, TypeSequence TL, TypeSequence TLBuf, std::size_t N> 
requires (Empty<typename TL::Tail> || (TypeSequence<typename TL::Tail> && !EQ<typename TL::Head, typename TL::Tail::Head>::Value))
struct GroupByHelper<EQ, TL, TLBuf, N> {
    using Head = Take<N, TLBuf>;
    using Tail = GroupByHelper<EQ, Drop<N, TLBuf>, Drop<N, TLBuf>, 1>;
};

template <template<typename, typename> typename EQ, TypeSequence TL, TypeSequence TLBuf, std::size_t N> 
requires (TypeSequence<typename TL::Tail> && EQ<typename TL::Head, typename TL::Tail::Head>::Value)
struct GroupByHelper<EQ, TL, TLBuf, N> : GroupByHelper<EQ, typename TL::Tail, TLBuf, N+1> {};

template<template<typename, typename> typename EQ, TypeList TL>
using GroupBy = GroupByHelper<EQ, TL, TL, 1>;




template<template<typename, typename> typename P, TypeList TL, typename First>
struct Filter2P;

template<template<typename, typename> typename P, TypeList TL, typename First> requires(!P<First, typename TL::Head>::Value)
struct Filter2P<P, TL, First> : Filter2P<P, typename TL::Tail, First>{};

template<template<typename, typename> typename P, TypeList TL, typename First> requires(P<First, typename TL::Head>::Value)
struct Filter2P<P, TL, First> {
    using Head = typename TL::Head;
    using Tail = Filter2P<P, typename TL::Tail, First>;
};

template<template<typename, typename> typename P, Empty TL, typename First>
struct Filter2P<P, TL, First> : Nil{};


/**/

template <typename T, TypeList TL, std::size_t N>
struct PairReplace {
    using Head = typename TL::Head;
    using Tail = PairReplace<T, typename TL::Tail, N - 1>;
};

template <typename T, Empty TL, std::size_t N> 
struct PairReplace<T, TL, N> : Nil {};

template <typename T, TypeSequence TL>
struct PairReplace<T, TL, 0> {
    using Head = T;
    using Tail = typename TL::Tail;
};


template<TypeSequence TL, std::size_t N>
struct GetElementN : GetElementN<typename TL::Tail, N - 1> {};

template<TypeSequence TL>
struct GetElementN<TL, 0> {
    using Element = typename TL::Head;
};



template<template<typename, typename> typename P, TypeList TLPref, typename Mid, TypeList TLSuf, std::size_t I, std::size_t N>
struct TopSortHelper;

template<template<typename, typename> typename P, TypeList TLPref, typename Mid, TypeList TLSuf, std::size_t I, std::size_t N>
requires(I < N && P<Mid, typename GetElementN<TLSuf, I>::Element>::Value && N > 0)
struct TopSortHelper<P, TLPref, Mid, TLSuf, I, N> {
    using Result = TopSortHelper<P, TLPref, Mid, TLSuf, I + 1, N>::Result;
};

template<template<typename, typename> typename P, TypeList TLPref, typename Mid, TypeList TLSuf, std::size_t I, std::size_t N>
requires(I < N && !P<Mid, typename GetElementN<TLSuf, I>::Element>::Value && N > 0)
struct TopSortHelper<P, TLPref, Mid, TLSuf, I, N> {
    using Result = TopSortHelper<P, TLPref, typename GetElementN<TLSuf, I>::Element, PairReplace<Mid, TLSuf, I>, I + 1, N>::Result;
};

template<template<typename, typename> typename P, TypeList TLPref, typename Mid, TypeList TLSuf, std::size_t I, std::size_t N>
requires(I == N  && N > 0)
struct TopSortHelper<P, TLPref, Mid, TLSuf, I, N>  {
    using Result = TopSortHelper<P, Cons<Mid, TLPref>, typename TLSuf::Head, typename TLSuf::Tail, 0, N - 1>::Result;   
};

template<template<typename, typename> typename P, TypeList TLPref, typename Mid, TypeList TLSuf, std::size_t I>
struct TopSortHelper<P, TLPref, Mid, TLSuf, I, 0>  {
    using Result = Cons<Mid, TLPref>;   
};


template<template<typename, typename> typename P, TypeList TL>
struct TopSort {
    using SortedList = typename TopSortHelper<P, Nil, typename TL::Head, typename TL::Tail, 0, ListSize<typename TL::Tail>>::Result;
};

template<template<typename, typename> typename P, Empty TL>
struct TopSort<P, TL> {
    using SortedList = Nil;
};

// template<class X, class Y>
// struct InheritPredicate{
//     static constexpr bool Value = !std::derived_from<X, Y>;
// };

// struct Base{int x1;};
// struct A : Base{int x2;};
// struct B : A{int x3;};
// struct C : B{int x4;};
// struct D {int x5;};


// static_assert(std::is_same<

// ToTuple<
// TopSort<
// InheritPredicate, FromTuple<
// type_tuples::TTuple<A, B, D, B, C, A>
// >
// >
// >, 
// type_tuples::TTuple<C, B, B, D, A, A>

// >);

// static_assert(
// std::is_same_v<
//     ToTuple<
//         TopSort<
//             InheritPredicate, 
//             FromTuple<
//             type_tuples::TTuple<A, B, D, B, C, A>
//             >
//         >::SortedList
//     >
// , type_tuples::TTuple<C, B, B, D, A, A>
// >);



} // namespace type_lists
