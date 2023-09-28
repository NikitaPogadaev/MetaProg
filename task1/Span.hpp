#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <numeric>
#include <span>
#include <string_view>
#include <type_traits>
#include <vector>
#include <cassert>


template <class T, std::size_t extent> class SpanBase {
protected:
  // public:
  SpanBase() = default;
};

template <class T> class SpanBase<T, std::dynamic_extent> {
protected:
  // public:
  std::size_t extent_;
  SpanBase(std::size_t extent) : extent_(extent) {}
  SpanBase(): extent_(0){}
};

template <class T, std::size_t extent = std::dynamic_extent>
class Span : private SpanBase<T, extent> {
public:


  using iterator = T *;
  using const_iterator = const T *;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  using value_type = std::decay_t<T>;
  using element_type = T;
  using size_type = std::size_t;
  using pointer = std::decay_t<T>*;
  using const_pointer = const std::decay_t<T>*;
  using reference = std::decay_t<T>&;
  using const_reference = const std::decay_t<T>&;
  using difference_type = std::ptrdiff_t;

    // Span(): SpanBase<T, extent>(){}
  Span(T *data, size_t size)
    requires(extent == std::dynamic_extent)
      : SpanBase<T, extent>(size), data_(data) {}
  Span(T *data)
    requires(extent != std::dynamic_extent)
      : SpanBase<T, extent>(), data_(data) {}

  template <std::contiguous_iterator Iter>
  Span(Iter it, size_t size)
    requires(extent == std::dynamic_extent)
      : SpanBase<T, extent>(size), data_(std::to_address(it)) {}
  template <std::contiguous_iterator Iter>
  Span(Iter it)
    requires(extent != std::dynamic_extent)
      : SpanBase<T, extent>(), data_(std::to_address(it)) {}



  template <class Cont>
  Span(Cont it)
    requires(extent == std::dynamic_extent && std::same_as<typename Cont::value_type, value_type> &&
    requires(Cont cont){
        {cont.begin()}-> std::contiguous_iterator;
        cont.size();
    })
      : SpanBase<T, extent>(it.size()), data_(std::to_address(it.begin())) {}

  template <class Cont>
  Span(Cont it)
    requires(extent != std::dynamic_extent && std::same_as<typename Cont::value_type, value_type> &&
    requires(Cont cont){
        {cont.begin()}-> std::contiguous_iterator;
        cont.size();
    })
      : SpanBase<T, extent>(), data_(std::to_address(it.begin())) {
          assert(extent >= it.size());
    }



  std::size_t Size() const requires(extent != std::dynamic_extent) {
      return extent;
}

  std::size_t Size() const requires(extent == std::dynamic_extent){
      return this->SpanBase<T, extent>::extent_;
}

T& operator[](const std::size_t ind){
    assert(ind < Size());
    return *(data_ + ind);
}

T& operator[](const std::size_t ind) const{
    assert(ind < Size());
    return *(data_ + ind);
}

T& Front() {
    assert(Size() > 0);
    return (this)->operator[](0);
}

T& Front() const{
    assert(Size() > 0);
    return (this)->operator[](0);
}
T& Back() {
    assert(Size() > 0);
    return (this)->operator[](Size() - 1);
}

T& Back() const{
    assert(Size() > 0);
    return (this)->operator[](Size() - 1);
}



  // iterator begin() { return data_; }
  //
  // iterator end()
  //   requires(extent == std::dynamic_extent)
  // {
  //   assert((this->SpanBase<T, extent>::extent_ > 0));
  //   return data_ + this->SpanBase<T, extent>::extent_;
  // }
  //
  // iterator end()
  //   requires (extent != std::dynamic_extent && extent > 0)
  // {
  //   return data_ + extent;
  // }
  //
  //
  //   reverse_iterator rend() { return reverse_iterator(begin() - 1); }
  //
  //   reverse_iterator rbegin() { return reverse_iterator(end() - 1); }


    iterator begin() const {
      return data_;
    }

    iterator end() const
    requires(extent == std::dynamic_extent)
  {
    assert((this->SpanBase<T, extent>::extent_ > 0));
    return data_ + this->SpanBase<T, extent>::extent_;
  }

  iterator end() const
    requires (extent != std::dynamic_extent && extent > 0)
  {
    return data_ + extent;
  }

    // reverse_iterator rend() const {
    //   return reverse_iterator(begin() - 1);
    // }
    //
    // reverse_iterator rbegin() const {
    //   return reverse_iterator(end() - 1);
    // }

  reverse_iterator rend() const {
      return reverse_iterator(data_);
    }

    reverse_iterator rbegin() const {
      return reverse_iterator(data_ + Size());
    }


    template<std::size_t Sz>
    Span<T, Sz> First () requires (extent != std::dynamic_extent && extent >= Sz){
        return Span<T, Sz>(data_);
    }

    Span<T> First (std::size_t sz) requires (extent == std::dynamic_extent){
        assert(extent >= sz);
        return Span<T>(data_, sz);
    }

    template<std::size_t Sz>
    Span<T, Sz> Last () requires (extent != std::dynamic_extent && extent >= Sz){
        return Span<T, Sz>(data_ + extent - Sz);
    }

    Span<T> Last (std::size_t sz) requires (extent == std::dynamic_extent){
        assert(extent >= sz);
        return Span<T>(data_ + Size() - sz, sz);
    }


private:
  T *data_;
  // std::size_t extent_; ?
};









// static_assert(sizeof(Span<int>) == sizeof(void*) + sizeof(std::size_t));

// static_assert(sizeof(Span<int, 2>) == sizeof(void*));

// template<class T>
// concept IsConstLvalRef = std::is_lvalue_reference_v<T> && std::is_const_v<std::remove_reference_t<T>>;
// template<class T>
// concept IsNonConstLvalRef = std::is_lvalue_reference_v<T> && !std::is_const_v<std::remove_reference_t<T>>;

// template<typename T>
// void keke()
// requires

// requires(const Span<int> s1, const Span<int, 42> s2, size_t idx) {
//     { *s1.begin() } -> IsNonConstLvalRef;
//     { *s1.end() } -> IsNonConstLvalRef;
//     { *s2.begin() } -> IsNonConstLvalRef;
//     { *s2.end() } -> IsNonConstLvalRef;
//     { s1[idx] } -> IsNonConstLvalRef;
//     { s2[idx] } -> IsNonConstLvalRef;
//     { s1.Front() } -> IsNonConstLvalRef;
//     { s2.Front() } -> IsNonConstLvalRef;
//     { s1.Back() } -> IsNonConstLvalRef;
//     { s2.Back() } -> IsNonConstLvalRef;
//   }

//   {
//     return;
// }



