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


template <std::size_t extent> class SpanBase {
protected:
  static consteval std::size_t Size() {return extent;}
  SpanBase(std::size_t){};
};

template <> class SpanBase<std::dynamic_extent> {
protected:
  std::size_t Size() const {return extent_;}
  std::size_t extent_;
  SpanBase(std::size_t extent) : extent_(extent) {}
  SpanBase(): extent_(0){}
};

template <class T, std::size_t extent = std::dynamic_extent>
class Span : private SpanBase<extent> {
public:

  using SpanBase<extent>::Size; 

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

  Span(T *data, size_t size)
      : SpanBase<extent>(size), data_(data) {}

  template <std::contiguous_iterator Iter>
  Span(Iter it, std::size_t size)
      : SpanBase<extent>(size), data_(std::to_address(it)) {}

  template<typename R>
  Span(R&& rng) requires (std::ranges::range<R>) : 
  SpanBase<extent>(std::ranges::size(rng)), data_(std::ranges::data(rng)) {}

  template <class Cont>
  Span(Cont& it)
    requires(std::same_as<typename Cont::value_type, value_type> &&
    requires(Cont cont){
        {cont.begin()}-> std::contiguous_iterator;
        cont.size();
    })
      : SpanBase<extent>(it.size()), data_(std::to_address(it.begin())) {
          assert(Size() >= it.size());
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




    iterator begin() const {
      return data_;
    }

    iterator end() const
  {
    assert(Size() > 0);
    return data_ + Size();
  }


  reverse_iterator rend() const {
      return reverse_iterator(data_);
    }

    reverse_iterator rbegin() const {
      return reverse_iterator(data_ + Size());
    }


    template<std::size_t Sz>
    Span<T, Sz> First () {
        return Span<T, Sz>(data_, Sz);
    }

    Span<T> First (std::size_t sz) {
        assert(extent >= sz);
        return Span<T>(data_, sz);
    }

    template<std::size_t Sz>
    Span<T, Sz> Last () {
        return Span<T, Sz>(data_ + Size() - Sz, Sz);
    }

    Span<T> Last (std::size_t sz){
        assert(extent >= sz);
        return Span<T>(data_ + Size() - sz, sz);
    }

    T* Data(){
      return data_;
    }

private:
  T *data_;
  // std::size_t extent_; ?
};


template <typename T>
Span(std::vector<T>&) -> Span<std::remove_reference_t<T>>;

template<typename T>
Span(T) -> Span<std::remove_reference_t<typename T::value_type>>;

template <typename T, std::size_t extent>
Span(std::array<T, extent>&) -> Span<std::remove_reference_t<T>, extent>;

template <std::contiguous_iterator Iterator, typename extent>
Span(Iterator, extent) -> Span<std::remove_reference_t<typename Iterator::value_type>>;





