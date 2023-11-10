#pragma once

#include <optional>

#include <concepts>

template <class From, auto target>
struct Mapping{
  using getFrom = From;
  static constexpr auto getTarget{target};
};

template<class Mapping>
concept RealMapping = requires(){Mapping::getFrom; Mapping::getTarget;};

template<class Mapping, class Base, class Target>
concept CorrectMapping = RealMapping<Mapping> && std::derived_from<Mapping::getFrom, Base> && std::same_as(Target, decltype(Mapping::getTarget));

template <class Base, class Target, CorrectMapping<Base, Target>... Mappings> // requires(CorrectMapping<Mappings, Base, Target> ... )
struct PolymorphicMapper {

  template<TypeList TL>
  static std::optional<Target> map(const Base& object);
};
