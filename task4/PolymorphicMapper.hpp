#pragma once

#include <optional>

#include <concepts>
#include "../task3/type_lists.hpp"

template <class From, auto target>
struct Mapping{
  using getFrom = From;
  using getTargetType = decltype(target);
  static constexpr auto getTarget{target};
};

template<class Mapping>
concept RealMapping = requires(){typename Mapping::getFrom; Mapping::getTarget;};

template<class Mapping, class Base, class Target>
concept CorrectMapping = RealMapping<Mapping> && 
std::derived_from<typename Mapping::getFrom, Base> && 
std::same_as<Target, typename Mapping::getTargetType>;


template<RealMapping X, RealMapping Y>
  struct InheritMapping{
    static constexpr bool Value = !std::derived_from<typename X::getFrom, typename Y::getFrom>;
  };

  template<class X, class Y>
  struct InheritPredicate{
    static constexpr bool Value = !std::derived_from<X, Y>;
  };


template <class Base, class Target, CorrectMapping<Base, Target>... Mappings> // requires(CorrectMapping<Mappings, Base, Target> ... )
struct PolymorphicMapper {

  template<type_lists::TypeSequence TL>
  static std::optional<Target> mapHelper(const Base& object){
    if(const typename TL::Head::getFrom * ptr = dynamic_cast<const typename TL::Head::getFrom *>(&object); ptr != nullptr){
      return TL::Head::getTarget;
    }
    else{
      return mapHelper<typename TL::Tail>(object);
    }
  }
  template<type_lists::Empty TL>
  static std::optional<Target> mapHelper(const Base&){
    return std::nullopt;
  }

  static std::optional<Target> map(const Base& object){
    return mapHelper<typename type_lists::TopSort<InheritMapping, type_lists::FromTuple<type_tuples::TTuple<Mappings ...>> >::SortedList >(object);
  }

  


};
