#pragma once

#include <cstddef>
#include <type_traits>
#include <tuple>

#ifndef AGGREGATE_SIZE_FUDGE
 #define AGGREGATE_SIZE_FUDGE 2
#endif

#if __cplusplus >= 201703L
 #define INLINE_IN_CXX17 inline
#else
 #define INLINE_IN_CXX17
#endif

namespace stdext {

#if __cplusplus >= 201703L
using std::is_aggregate;
using std::is_aggregate_v;
#elif __has_extension(is_aggregate)
template<class T> struct is_aggregate : std::integral_constant<bool, __is_aggregate(T)> {};
template<class T> INLINE_IN_CXX17 constexpr bool is_aggregate_v = is_aggregate<T>::value;
#else
template<class T> struct is_aggregate : std::integral_constant<bool, std::is_class<T>::value || std::is_array<T>::value> {};
template<class T> INLINE_IN_CXX17 constexpr bool is_aggregate_v = is_aggregate<T>::value;
#endif

namespace detail {
    template<class T, class> struct is_tuple_like_impl : std::false_type {};
    template<class T> struct is_tuple_like_impl<T, decltype(void(std::tuple_size<T>::value))> : std::true_type {};
} // namespace detail

template<class T> struct is_tuple_like : detail::is_tuple_like_impl<T, void> {};
template<class T> INLINE_IN_CXX17 constexpr bool is_tuple_like_v = is_tuple_like<T>::value;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
namespace detail {
    struct convertible_to_anything { template<class T> operator T() const noexcept; };
    template<class T, class, class> struct is_n_constructible_impl : std::false_type {};
    template<class T, size_t... Is> struct is_n_constructible_impl<T, std::index_sequence<Is...>, decltype(void(T{(void(Is), convertible_to_anything{})...}))> : std::true_type {};
} // namespace detail
#pragma GCC diagnostic pop

template<size_t N, class T> struct is_n_constructible : detail::is_n_constructible_impl<T, std::make_index_sequence<N>, void> {};
template<size_t N, class T> INLINE_IN_CXX17 constexpr bool is_n_constructible_v = is_n_constructible<N, T>::value;

namespace detail {
    template<class T, size_t M, bool StopHere> struct aggregate_size_impl2 : aggregate_size_impl2<T, M-1, is_n_constructible_v<M-1, T>> {};
    template<class T, size_t M> struct aggregate_size_impl2<T, M, true> : std::integral_constant<size_t, M> {};
    template<class T> struct aggregate_size_impl2<T, 0, false> {};

    template<class T, bool> struct aggregate_size_impl : aggregate_size_impl2<T, sizeof (T) + AGGREGATE_SIZE_FUDGE, is_n_constructible_v<sizeof (T) + AGGREGATE_SIZE_FUDGE, T>> {};
    template<class T> struct aggregate_size_impl<T, false> {};
} // namespace detail

template<class T> struct aggregate_size : detail::aggregate_size_impl<T, is_aggregate<T>::value> {};
template<class T> INLINE_IN_CXX17 constexpr auto aggregate_size_v = aggregate_size<T>::value;

namespace detail {
    template<class T, size_t M, class> struct is_structured_bindable_impl : std::false_type {};
    template<class T, size_t M> struct is_structured_bindable_impl<T, M, std::enable_if_t<M != 0 && is_tuple_like<T>::value && std::tuple_size<T>::value == M>> : std::true_type {};
    template<class T, size_t M> struct is_structured_bindable_impl<T, M, std::enable_if_t<M != 0 && !is_tuple_like<T>::value && is_aggregate<T>::value && aggregate_size<T>::value == M>> : std::true_type {};
} // namespace detail

template<size_t N, class T> struct is_structured_bindable : detail::is_structured_bindable_impl<T, N, void> {};
template<size_t N, class T> INLINE_IN_CXX17 constexpr bool is_structured_bindable_v = is_structured_bindable<N, T>::value;

} // namespace stdext
