#include "SG14_test.h"
#include "structured_binding_traits.h"

#include <array>
#include <complex>
#include <memory>
#include <tuple>
#include <vector>

struct key_16_8_t {
    uint16_t index;
    uint8_t generation;
};
struct key_11_5_t {
    uint16_t index : 11;
    uint8_t generation : 5;
};
struct unique_view {
    std::unique_ptr<char> p;
    int z;
};
struct tuple_ish {
    tuple_ish(int x, int y) : x(x), y(y) {}
    int x, y;
};
namespace std { template<> class tuple_size<tuple_ish> : public std::integral_constant<size_t, 2> {}; }

template<template<class> class Pred, class T, bool B>
int static_assert_predicate()
{
    static_assert(Pred<T>::value == B, "");
    return 0;
}

template<template<class> class Pred, class... Ts>
void static_assert_true()
{
    int dummy[] = {
        static_assert_predicate<Pred, Ts, true>() ...
    };
    (void)dummy;
}

template<template<class> class Pred, class... Ts>
void static_assert_false()
{
    int dummy[] = {
        static_assert_predicate<Pred, Ts, false>() ...
    };
    (void)dummy;
}

template<class T> using is_0_constructible = stdext::is_n_constructible<0, T>;
template<class T> using is_1_constructible = stdext::is_n_constructible<1, T>;
template<class T> using is_2_constructible = stdext::is_n_constructible<2, T>;
template<class T> using is_3_constructible = stdext::is_n_constructible<3, T>;

template<class T> using is_aggregate_size_0 = std::integral_constant<bool, stdext::aggregate_size<T>::value == 0>;
template<class T> using is_aggregate_size_1 = std::integral_constant<bool, stdext::aggregate_size<T>::value == 1>;
template<class T> using is_aggregate_size_2 = std::integral_constant<bool, stdext::aggregate_size<T>::value == 2>;

template<class T> using is_bindable_0 = stdext::is_structured_bindable<0, T>;
template<class T> using is_bindable_1 = stdext::is_structured_bindable<1, T>;
template<class T> using is_bindable_2 = stdext::is_structured_bindable<2, T>;

void sg14_test::structured_binding_traits_test()
{
    using A0 = int[0];
    using A1 = int[1];
    using A2 = int[2];
    using F = int(int);
    using I = int;
    using IP = int*;
    using K2 = key_16_8_t;
    using KB2 = key_11_5_t;
    using KU2 = unique_view;
    using SA0 = std::array<int, 0>;
    using SA1 = std::array<int, 1>;
    using SA2 = std::array<int, 2>;
    using SC2 = std::complex<double>;
    using SP = std::pair<int, int>;
    using ST0 = std::tuple<>;
    using ST1 = std::tuple<int>;
    using ST2 = std::tuple<int, double>;
    using SV = std::vector<int>;
    using T2 = tuple_ish;

    static_assert_true<stdext::is_tuple_like, SA0, SA1, SA2, SP, ST0, ST1, ST2, T2>();
    static_assert_false<stdext::is_tuple_like, K2, KB2, A0, A1, A2, F, I, IP, SC2, SV>();

    static_assert_true<is_0_constructible, K2, KB2, KU2, A0, A1, A2, I, IP, SA0, SA1, SA2, SC2, SP, ST0, ST1, ST2, SV>();
    static_assert_false<is_0_constructible, F, T2>();

    static_assert_true<is_1_constructible, A1, A2, I, IP, K2, KB2, KU2, SA0, SA1, SA2, SC2, SP, ST1, ST2, SV, T2>();
    static_assert_false<is_1_constructible, A0, F>();

    static_assert_true<is_2_constructible, A2, K2, KB2, KU2, SA2, SC2, SP, ST2, T2>();
    static_assert_false<is_2_constructible, A0, A1, SA0, SA1, F, I, IP>();

    static_assert_true<is_3_constructible>();
    static_assert_false<is_3_constructible, A0, A1, A2, F, I, IP, K2, KB2, KU2, SA0, SA1, SA2, SC2, SP, T2>();

    static_assert_true<is_aggregate_size_1, SA1>();
    static_assert_true<is_aggregate_size_2, K2, KB2, SA2>();

    static_assert_false<is_bindable_0, A0, SA0, ST0>();
    static_assert_true<is_bindable_1, A1, SA1, ST1>();
    static_assert_false<is_bindable_1, A2, K2, KB2, KU2, SA2, ST2, T2>();
    static_assert_true<is_bindable_2, A2, K2, KB2, KU2, SA2, ST2, T2>();
    static_assert_false<is_bindable_2, A1, SA1, SC2, ST1>();
}

#ifdef TEST_MAIN
int main()
{
    sg14_test::structured_binding_traits_test();
}
#endif
