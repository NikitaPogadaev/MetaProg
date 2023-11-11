#pragma once
#include<string_view>

template<std::size_t max_length>
struct FixedString {
  constexpr FixedString(const char* string, std::size_t length): data_(), sz_(length){
    for(std::size_t i = 0; i < length; ++i){
        data_[i] = *(string + i);
    }
  }
  constexpr operator std::string_view() const{
    if (sz_ == 0 || max_length == 0){
        return std::string_view();
    }
    else{
        return std::string_view(&(data_[0]), sz_);
    }
  }



// private:
  char data_[max_length];
  std::size_t sz_{0};
};

// operator ""_cstr ?

constexpr FixedString<256> operator ""_cstr(const char* str, std::size_t ln){
    return FixedString<256>(str, ln);
}
