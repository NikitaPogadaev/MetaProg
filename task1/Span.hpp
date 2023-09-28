#include <span>
#include <iostream>
#include <concepts>
#include <cstdlib>
#include <iterator>
#include <string_view>

#include <cstddef>
#include <vector>


template <class T, std::size_t extent>
class SpanBase {
  SpanBase()=default;

};

template <class T>
class SpanBase <T, std::dynamic_extent>{
  std::size_t extent_;
  SpanBase(std::size_t extent): extent_(extent){}
};


template
  < class T
  , std::size_t extent = std::dynamic_extent
  >
class Span
//: private SpanBase
{
public:
  // Span()
  // Reimplement the standard span interface here
  // (some more exotic methods are not checked by the tests and can be sipped)
  // Note that unliike std, the methods name should be Capitalized!
  // E.g. instead of subspan, do Subspan.
  // Note that this does not apply to iterator methods like begin/end/etc.

private:
  T* data_;
  // std::size_t extent_; ?
};

int main (){
  std::cout << "dsf";
}
