#pragma once

#include <type_traits>
#include <optional>
#include <vector>
#include <set>
#include <map>
#include <concepts>

template <typename ty>
struct is_optional : std::false_type {};

template <typename ty>
struct is_optional<std::optional<ty>> : std::true_type {};

template <typename ty>
struct is_myIntegral {
   static constexpr bool value = std::is_integral<ty>::value && !std::is_same<bool, ty>::value;
   };

template <typename ty, typename _ = void>
struct is_vector {
   static constexpr bool value = false;
};

template <typename ty>
struct is_vector<ty, typename std::enable_if<std::is_same<ty, std::vector<typename ty::value_type,
   typename ty::allocator_type>>::value>::type> {
   static constexpr bool value = true;
};

template <typename ty, typename _ = void>
struct is_set {
   static constexpr bool value = false;
};

template <typename ty>
struct is_set<ty, typename std::enable_if<std::is_same<ty, std::set<typename ty::key_type,
   typename ty::key_compare, typename ty::allocator_type>>::value>::type> {
   static constexpr bool value = true;
};

template <typename ty, typename _ = void>
struct is_map {
   static constexpr bool value = false;
};

template <typename ty>
struct is_map<ty, typename std::enable_if<std::is_same<ty, std::map<typename ty::key_type, typename ty::mapped_type,
   typename ty::key_compare, typename ty::allocator_type>>::value>::type> {
   static constexpr bool value = true;
};


template <typename ty>
concept myIntegral = is_myIntegral<ty>::value;

template <typename ty>
concept myVector = is_vector<ty>::value;

template <typename ty>
concept mySet = is_set<ty>::value;

template <typename ty>
concept myMap = is_map<ty>::value;

template <typename ty>
concept myContainer = is_set<ty>::value || is_vector<ty>::value || is_map<ty>::value;

