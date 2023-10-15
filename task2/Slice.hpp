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
#include <array>
#include <cassert>
#include <compare>

inline constexpr std::ptrdiff_t dynamic_stride = -1;

////////////


template <std::ptrdiff_t stride> 
class StrideBase {
protected: 
  static consteval std::ptrdiff_t Stride() {return stride;}

  StrideBase(std::ptrdiff_t){};
  StrideBase() = default;
};

template <> class StrideBase<dynamic_stride> {
protected: 
  std::ptrdiff_t Stride()const{return stride_;}

  std::ptrdiff_t stride_;
  StrideBase(std::ptrdiff_t step) : stride_(step) {}
  StrideBase(): stride_(1){}
};


/////////////////////////////////////////////////////



template <std::size_t extent, std::ptrdiff_t stride> 
class ExtentBase : protected StrideBase<stride>{
protected:
  using StrideBase<stride>::Stride;
  static consteval std::size_t Size() {return extent;}
  
  ExtentBase(size_t size, std::ptrdiff_t step) : StrideBase<stride>(step) {}
  ExtentBase() = default;
};

template <std::ptrdiff_t stride> 
class ExtentBase<std::dynamic_extent, stride> : protected StrideBase<stride>{
protected:
  using StrideBase<stride>::Stride;
  std::size_t Size() const {return extent_;}
  
  std::size_t extent_;
  ExtentBase(std::size_t size, std::ptrdiff_t step) : StrideBase<stride>(step), extent_(size) {}
  ExtentBase(): extent_(0){}
};


/*
template
  < class T
  , std::size_t extent
  , std::ptrdiff_t stride
  >
class SliceHelper {
  std::conditional_t<extent == std::dynamic_extent, T*, std::pair<T*, std::size_t>> firstPart_;ExtentBase
protected:
  SliceHelper(T *data, size_t size) requires (extent == std::dynamic_extent) : firstPart{data, size} {}
  SliceHelper(T *data) requires (extent != std::dynamic_extent) : firstPart(data) {}
  Stride(){
    return stride;
  }
};

template
  < class T
  , std::size_t extent>
class SliceHelper<T, extent, dynamic_stride> {
  std::conditional_t<extent == std::dynamic_extent, T*, std::pair<T*, std::size_t>> firstPart_;
  std::size_t stride_;
protected:
  SliceHelper(T *data, size_t size, size_t step) requires (extent == std::dynamic_extent) : firstPart{data, size}, stride_(step) {}
  SliceHelper(T *data, size_t step) requires (extent != std::dynamic_extent) : firstPart(data), stride_(step) {}
  Stride(){
    return stride_;
  }
};
*/




template
  < class T
  , std::size_t extent = std::dynamic_extent
  , std::ptrdiff_t stride = 1
  >
class Slice : protected ExtentBase<extent, stride>{


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
template <std::ptrdiff_t StepT> 
class IterBase {
protected: 
  constexpr std::ptrdiff_t Step() const{return StepT;}

  IterBase(std::ptrdiff_t){};
  IterBase() = default;
};

template <> class IterBase<dynamic_stride> {
protected: 
  std::ptrdiff_t Step()const{return step_;}

  std::ptrdiff_t step_;
  IterBase(std::ptrdiff_t step) : step_(step) {}
  IterBase(): step_(1){}
};*/
////
template<bool IsConst>
    struct common_iterator //: public IterBase<stride>
    {
    private:
        using T_type = std::conditional_t<IsConst, const T&, T&>;
        using T_ptr_type = std::conditional_t<IsConst, const T*, T*>;
    public:
        // using IterBase<stride>::Step;


        using iterator_category = std::random_access_iterator_tag;
        using value_type = std::conditional_t<IsConst, const T, T>;
        using pointer = T_ptr_type;
        using reference = T_type;
        using difference_type = std::ptrdiff_t;


        // common_iterator(T_ptr_type data, std::ptrdiff_t step = 
        // (stride == dynamic_stride ? 1 : stride)): IterBase<stride>(step), dataptr_(data){}


        common_iterator(T_ptr_type data, std::ptrdiff_t step): dataptr_(data), step_(step){}
        common_iterator(): common_iterator(nullptr, 1){}

        std::ptrdiff_t Step(){return step_;}

        common_iterator(const common_iterator& it) = default;
        common_iterator(common_iterator&& it) = default;
        common_iterator& operator =(const common_iterator& it) = default;
        common_iterator& operator =(common_iterator&& it) = default;

        reference operator*() const{
            return *dataptr_;
        }
        pointer operator->() const{
            return dataptr_;
        }

        common_iterator<IsConst>& operator++() {
            dataptr_ += Step();
            return *this;
        }
        common_iterator<IsConst> operator++(int) {
            common_iterator<IsConst> i(dataptr_, Step());
            dataptr_ += Step();
            return i;
        }
        common_iterator<IsConst>& operator--() {
            dataptr_ -= Step();
            return *this;
        }
        common_iterator<IsConst> operator--(int) {
            common_iterator<IsConst> i(dataptr_, Step());
            dataptr_ -= Step();
            return i;
        }
        common_iterator<IsConst>& operator+=(const std::ptrdiff_t a) {
            dataptr_ += Step() * a;
            return *this;
        }
        common_iterator<IsConst>& operator-=(const std::ptrdiff_t a) {
            dataptr_ -= Step() * a;
            return *this;
        }



        bool operator== (const common_iterator<IsConst>& iter) const{
            return dataptr_ == iter.dataptr_;
        }
        std::strong_ordering operator<=> (const common_iterator<IsConst>& iter) const{
            return dataptr_ <=> iter.dataptr_;
        }





        common_iterator<IsConst> operator+(std::ptrdiff_t b) const{

            common_iterator<IsConst> temp(dataptr_);
            temp += b;
            return temp;
        }
        common_iterator<IsConst> operator-(std::ptrdiff_t b) const{

            common_iterator<IsConst> temp(dataptr_);
            temp -= b;
            return temp;
        }
        
        
        friend common_iterator<IsConst> operator+(std::ptrdiff_t b, const common_iterator& it) {
            return it + b;
        }
        

        std::ptrdiff_t operator-(const common_iterator& it) const
        {
          assert(step_ == it.step_);
          return (dataptr_ - it.dataptr_) / step_;
        }

        T_type operator [](std::size_t ind) const{
            return *(dataptr_ + ind * step_);
        }

        
        operator common_iterator<true> () const 
        {
            return common_iterator<true>(dataptr_, step_);
        }

    private:
        T_ptr_type dataptr_;
        std::ptrdiff_t step_;
    };


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


public:

  using ExtentBase<extent, stride>::Size;
  using ExtentBase<extent, stride>::Stride;

  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;
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


  template<typename R>
  Slice(const R&& rng) requires (requires (const R&& range) {range.Stride();}) : 
  data_(std::ranges::data(rng)), ExtentBase<extent, stride>(std::ranges::size(rng), rng.Stride()) {}


  template<class Cont>
  Slice(Cont& it)
    requires(std::same_as<typename Cont::value_type, value_type> &&
    requires(Cont cont){
        {cont.begin()}-> std::contiguous_iterator;
        cont.size();
    })
      : ExtentBase<extent, stride>(it.size() / (stride == dynamic_stride ? 1 : stride), 1), data_(std::to_address(it.begin())) {
          assert(Size() >= it.size());
    }

    Slice(): ExtentBase<extent, stride>(0, 1), data_(nullptr){}

    template<typename X1, std::size_t X2, std::ptrdiff_t X3>
    Slice(const Slice<X1, X2, X3>& xx) 
    requires( 
        (extent == std::dynamic_extent || extent == X2)
        && (stride == dynamic_stride || stride == X3)
        && requires(){requires std::convertible_to<X1, T>;}) : ExtentBase<extent, stride>(xx.Size(), xx.Stride()), data_(xx.Data()){}

    // Slice(const Slice& other) = default;
    // Slice& operator=(const Slice& other) = default;
    // Slice(Slice&& other) = default;
    // Slice& operator=(Slice&& other) = default;

  template <std::contiguous_iterator It>
  Slice(It first, std::size_t count = 0, std::ptrdiff_t skip = 1) : 
  ExtentBase<extent, stride>(count, skip), data_(std::to_address(first)) {}
  
  Slice(T* first, std::size_t count = 0, std::ptrdiff_t skip = 1) : 
  ExtentBase<extent, stride>(count, skip), data_(first) {}



  // Data, Size, Stride, begin, end, casts, etc...

  T* Data() const{
    return data_;
  }

  Slice<T, std::dynamic_extent, stride>
    First(std::size_t count) const{
        assert(count <= Size());
        return Slice<T, std::dynamic_extent, stride> (data_, count, Stride());
    }

  template <std::size_t count>
  Slice<T, count, stride>
    First() const{
        assert(count <= Size());
        return Slice<T, count, stride> (data_, count, Stride());
    }

  Slice<T, std::dynamic_extent, stride>
    Last(std::size_t count) const{
        assert(count <= Size());
        return Slice<T, std::dynamic_extent, stride> (data_ + Stride() * (Size() - count), count, Stride());
    }

  template <std::size_t count>
  Slice<T, count, stride>
    Last() const{
        assert(count <= Size());
        return Slice<T, count, stride> (data_ + Stride() * (Size() - count), count, Stride());
    }



  Slice<T, std::dynamic_extent, stride>
    DropFirst(std::size_t count) const{
        assert(count <= Size());
        return Last(Size() - count);
    }

  template <std::size_t count> requires (extent == std::dynamic_extent || extent >= count)
  Slice<T, (extent == std::dynamic_extent ? std::dynamic_extent : extent - count), stride>
    DropFirst() const{
        assert(count <= Size());
        if constexpr (extent == std::dynamic_extent){
            return Last(Size() - count);
        }
        else{
            return Last<extent - count>();
        }
    }

  Slice<T, std::dynamic_extent, stride>
    DropLast(std::size_t count) const{
        assert(count <= Size());
        return First(Size() - count);
    }

  template <std::size_t count> requires (extent == std::dynamic_extent || extent >= count)
  Slice<T, (extent == std::dynamic_extent ? std::dynamic_extent : extent - count), stride>
    DropLast() const{
        assert(count <= Size());
        if constexpr (extent == std::dynamic_extent){
            return First(Size() - count);
        }
        else{
            return First<extent - count>();
        }
    }




  Slice<T, std::dynamic_extent, dynamic_stride>
    Skip(std::ptrdiff_t skip) const{
        static_assert(extent > 0);
        return Slice<T, std::dynamic_extent, dynamic_stride>(data_, (Size()-1) / skip + 1, Stride() * skip);
    }

  template <std::ptrdiff_t skip>
  Slice<T
  , (stride == dynamic_stride || extent == std::dynamic_extent ? std::dynamic_extent : (extent-1) / skip + 1)
  , (stride == dynamic_stride ? stride : stride * skip)>
    Skip() const{
        return Slice<T
        , (stride == dynamic_stride || extent == std::dynamic_extent ? std::dynamic_extent : (extent-1) / skip + 1)
        , (stride == dynamic_stride ? stride : stride * skip)>
        (data_, (Size()-1) / skip + 1, Stride() * skip);
    }


    T& operator[](const std::size_t ind){
        assert(ind < Size());
        return *(data_ + ind * Stride());
    }

    T& operator[](const std::size_t ind) const{
        assert(ind < Size());
        return *(data_ + ind * Stride());
    }



    iterator begin() const {
      return iterator(data_, Stride());
    }

    iterator end() const
    {
        assert(Size() > 0);
        return iterator(data_ + (Size())*Stride(), Stride());
    }

  reverse_iterator rend() const {
    return reverse_iterator(data_, Stride());
    }

    reverse_iterator rbegin() const {
      return reverse_iterator(data_ + (Size()-1)*Stride(), Stride());
    }

    template<typename X1, std::size_t X2, std::ptrdiff_t X3>
    bool operator == (const Slice<X1, X2, X3>& xx) const{
        return (data_ == xx.Data() && Size() == xx.Size() && Stride() == xx.Stride());
    }

private:
  T* data_;
};






template<typename T>
Slice(T) 
-> Slice<std::remove_reference_t<typename T::value_type>>;

template <typename T, std::size_t extent>
Slice(std::array<T, extent>&) 
-> Slice<std::remove_reference_t<T>, extent, 1>;

template <typename T>
Slice(std::vector<T>&) 
-> Slice<std::remove_reference_t<typename T::value_type>, std::dynamic_extent>;

template <std::contiguous_iterator Iterator, typename extent, typename stride>
Slice(Iterator, extent, stride) -> 
Slice<std::remove_reference_t<typename Iterator::value_type>, std::dynamic_extent, dynamic_stride>;


