**Document number**: (draft)  
**Date**: 2015-08-29  
**Project**: Programming Language C++, Library Evolution WG, Evolution WG, SG14  
**Reply-to**: John McFarlane, [fixed-point@john.mcfarlane.name](mailto:fixed-point@john.mcfarlane.name)

# Fixed-Point Real Numbers

## I. Introduction

This proposal introduces a system for performing binary fixed-point
arithmetic using built-in integral types.

## II. Motivation

Floating-point types are an exceedingly versatile and widely supported
method of expressing real numbers on modern architectures.

However, there are certain situations where fixed-point arithmetic is
preferable. Some system lack native floating-point registers and must
emulate them in software. Many others are capable of performing some
or all operations more efficiently using integer arithmetic. Certain
applications can suffer from the variability in precision which comes
from a dynamic radix point [\[1\]](http://www.pathengine.com/Contents/Overview/FundamentalConcepts/WhyIntegerCoordinates/page.php).
In situations where a variable exponent is not desired, it takes
valuable space away from the mantissa and reduces precision.

Built-in integer types provide the basis for an efficient
representation of binary fixed-point real numbers. However, laborious,
error-prone steps are required to normalize the results of certain
operations and to convert to and from fixed-point types.

A set of tools for defining and manipulating fixed-point types is
proposed. These tools are designed to make work easier for those who
traditionally use integers to perform low-level, high-performance
fixed-point computation.

## III. Impact On the Standard

This proposal is a pure library extension. It does not require
changes to any standard classes, functions or headers.

## IV. Design Decisions

The design is driven by the following aims in roughly descending
order:

1. to automate the task of using integer types to perform low-level
   binary fixed-point arithmetic;
2. to facilitate a style of code that is intuitive to anyone who is
   comfortable with integer and floating-point arithmetic;
3. to avoid type promotion, implicit conversion or other behavior that
   might lead to surprising results and
4. to preserve significant digits at the expense of insignificant
   digits, i.e. to prefer underflow to overflow.

### Class Template

Fixed-point numbers are specializations of

    template <typename REPR_TYPE, int EXPONENT>
    class fixed_point

where the template parameters are described as follows.

#### `REPR_TYPE` Type Template Parameter

This parameter identifies the capacity and signedness of the
underlying type used to represent the value. In other words, the size
of the resulting type will be `sizeof(REPR_TYPE)` and it will be
signed iff `is_signed<REPR_TYPE>::value` is true. The default is
`int`.

`REPR_TYPE` must be a fundamental integral type and should not be the
largest size. Suitable types include: `std::int8_t`, `std::uint8_t`,
`std::int16_t`, `std::uint16_t`, `std::int32_t` and `std::uint32_t`.
In limited situations, `std::int64_t` and `std::uint64_t` can be used.
The  reasons for these limitations relate to the difficulty in finding
a type that is suitable for performing lossless integer
multiplication.

#### `EXPONENT` Non-Type Template Parameter

The exponent of a fixed-point type is the equivalent of the exponent
field in a floating-point type and shifts the stored value by the
requisite number of bits necessary to produce the desired range.

The default value is dependent on `REPR_TYPE` and ensures that half of
the bits of the type are allocated to fractional digits. The other
half go to integer digits and (if `REPR_TYPE` is signed) the sign bit.

The resolution of a specialization of `fixed_point` is

    2 ^ EXPONENT

and the minimum and maximum values are

    std::numeric_limits<REPR_TYPE>::min() * 2 ^ EXPONENT

and

    std::numeric_limits<REPR_TYPE>::max() * 2 ^ EXPONENT

respectively.

### `make_fixed` and `make_ufixed` Helper Type

The `EXPONENT` template parameter is versatile and concise. It is an
intuitive scale to use when considering the full range of positive and
negative exponents a fixed-point type might possess. It also
corresponds to the exponent field of built-in floating-point types.

However, most fixed-point formats can be described more intuitively by
the cardinal number of integer and/or fractional digits they contain.
Most users will prefer to distinguish fixed-point types using these
parameters.

For this reason, two 'named constructors' are defined. They take the
form of helper types in the style of `make_signed`.

These aliases are declared as:

    template <unsigned INTEGER_DIGITS, unsigned FRACTIONAL_DIGITS = 0, bool IS_SIGNED = true>
  	using make_fixed;

and

    template <unsigned INTEGER_DIGITS, unsigned FRACTIONAL_DIGITS = 0>
    using make_ufixed;

They resolve to a `fixed_point` specialization with the given
signedness and number of integer and fractional digits. They may
contain additional integer and fractional digits.

### Conversion

Fixed-point numbers can be explicitly converted to and from built-in
arithmetic types.

While effort is made to ensure that significant digits are not lost
during conversion, no effort is made to avoid rounding errors.
Whatever would happen when converting to and from an integer type
largely applies to `fixed_point` objects also. For example:

    make_ufixed<4, 4>(.006) == make_ufixed<4, 4>(0)

...equates to `true` and is considered a acceptable rounding error.

### Arithmetic Operators

Any operators that might be applied to integer types can also be
applied to `fixed_point` specializations. Input parameters and return
value are all of the same type. (A possible exception to this is the
right hand parameter of bit shift operators.)

Because of this choice, arithmetic operators carry a risk of overflow.
For instance,

    make_fixed<4, 3>(15) + make_fixed<4, 3>(1)

produces undefined behavior because a type with 4 integer bits cannot
store value 16. (Note that unsigned types also overflow - unlike their
unsigned integer equivalents.)

This represents the first of three alternative approaches to dealing
with overflow:

1. ignore the problem and leave it to the user to avoid overflow;
2. promote the result to a larger type to ensure sufficient capacity
   or
3. adjust the exponent of the result upward to ensure that the top
   limit of the type is sufficient to preserve the most significant
   digits at the expense of the less significant digits.

For arithmetic operators, choice 1) is preferred because it most
closely follows the behavior of integer types. Thus it should cause
the least surprise to the fewest users. This makes it far easier to
reason about in code where functions are written with a particular
type in mind. It also requires the least computation.

Choices 2) and 3) are more robust to overflow events. However, they
represent different trade-offs and neither one is the best fit in all
situations. For these reasons, they are offered via named functions
instead.

### Type Promotion

Function template, `promote`, borrows a term from the language feature
which avoids integer overflow prior to certain operations. It takes a
`fixed_point` object and returns the same value represented by a
larger `fixed_point` specialization.

For example,

    promote(make_fixed<5, 2>(15.5))

is equivalent to

    make_fixed<11, 4>(15.5)

Complimentary function template, `demote`, inverts the process,
returning a value of a smaller type.

### Named Arithmetic Functions

The following named function templates can be used as alternatives to
arithmetic operators in situations where the aim is to avoid overflow
without falling back on higher-capacity types.

    trunc_multiply(FIXED_POINT_1, FIXED_POINT_2)
    trunc_add(FIXED_POINT_1, FIXED_POINT_2)
    trunc_subtract(FIXED_POINT_1, FIXED_POINT_2)
    trunc_square(FIXED_POINT_1)
    trunc_sqrt(FIXED_POINT_1)

These templates return results whose types have adjusted `EXPONENT`
values which prevent overflow in the vast majority of cases. The
`trunc_` function templates have two drawbacks:

Firstly, because the capacity of the return type is not increased,
precision may be lost. For example:

    trunc_square(make_ufixed<4, 4>(15.9375))

returns `make_ufixed<8, 0>(254)`. This result is far closer to
the correct value than the result returned by `operator *`.

Secondly, there are rare cases where overflow can still occur due to
the nature of twos-compliment binary arithmetic and the *most negative
number*. For instance, the value of

    trunc_square(make_fixed<7, 0>(-128))

is zero because 16384 cannot be represented in a
`fixed_point<uint8_t, 6>` type.

### Underflow

The major disadvantage of the way in which `trunc_` functions protect
the more significant digits is that the less insignificant digits are
dropped. In the extreme case of underflow, all the bits are lost and
the value becomes zero.

For example,

    trunc_square(make_ufixed<8, 0>(15))

causes the object to be flushed to zero.

While often not as serious as overflow, underflow can cause knock-on
effects, such as when a flushed value is subsequently used as a
divisor.

Although there is a strong desire to catch situations where a
`fixed_point` value transitions to a flushed state, it is unacceptable
that this should necessarily require a run-time check.

For this reason, it is left to the user to avoid flush events and for
such events to result in undefined behavior. This is not an ideal
solution but is necessary to ensure that future extension to the API
involving the option of run-time checks has enough freedom to
implement a satisfactory solution.

See the 'Future Issues' section below for discussion on possible
strategies for catching overflow and underflow events.

### Example

The following example calculates the magnitude of a 3-dimensional vector.

    template <typename FP>
    constexpr auto magnitude(FP const & x, FP const & y, FP const & z)
    -> decltype(trunc_sqrt(trunc_add(trunc_square(x), trunc_square(y), trunc_square(z))))
    {
        return trunc_sqrt(trunc_add(trunc_square(x), trunc_square(y), trunc_square(z)));
    }

Calling the above function as follows

    static_cast<double>(magnitude(
        make_ufixed<4, 12>(1),
        make_ufixed<4, 12>(4),
        make_ufixed<4, 12>(9)));

returns the value, 9.890625.

## V. Technical Specification

### Header \<fixed_point\> Synopsis

    namespace std {
      template <typename REPR_TYPE, int EXPONENT> class fixed_point;

      template <unsigned INTEGER_DIGITS, unsigned FRACTIONAL_DIGITS = 0, bool IS_SIGNED = true>
        using make_fixed;
      template <unsigned INTEGER_DIGITS, unsigned FRACTIONAL_DIGITS = 0>
        using make_ufixed;

      template <typename FIXED_POINT>
        using fixed_point_promotion_t;
      template <typename FIXED_POINT>
        fixed_point_promotion_t<FIXED_POINT>
          constexpr promote(const FIXED_POINT & from) noexcept

      template <typename FIXED_POINT>
        using fixed_point_demotion_t;
      template <typename FIXED_POINT>
        fixed_point_demotion_t<FIXED_POINT>
          constexpr demote(const FIXED_POINT & from) noexcept

      template <typename LHS, typename RHS>
        constexpr bool operator ==(LHS const & lhs, RHS const & rhs) noexcept;
      template <typename LHS, typename RHS>
        constexpr bool operator !=(LHS const & lhs, RHS const & rhs) noexcept;
      template <typename LHS, typename RHS>
        constexpr bool operator <(LHS const & lhs, RHS const & rhs) noexcept;
      template <typename LHS, typename RHS>
        constexpr bool operator >(LHS const & lhs, RHS const & rhs) noexcept;
      template <typename LHS, typename RHS>
        constexpr bool operator >=(LHS const & lhs, RHS const & rhs) noexcept;
      template <typename LHS, typename RHS>
        constexpr bool operator <=(LHS const & lhs, RHS const & rhs) noexcept;

      template <typename LHS, typename RHS = LHS>
        using trunc_multiply_result_t;
      template <typename LHS, typename RHS>
        trunc_multiply_result_t<LHS, RHS>
          constexpr trunc_multiply(const LHS & factor1, const RHS & factor2) noexcept;

      template <typename REPR_TYPE, int EXPONENT, unsigned N = 2>
        using trunc_add_result_t;
      template <typename REPR_TYPE, int EXPONENT, typename ... TAIL>
        trunc_add_result_t<REPR_TYPE, EXPONENT, sizeof...(TAIL) + 1>
          constexpr trunc_add(fixed_point<REPR_TYPE, EXPONENT> const & addend1, TAIL const & ... addend_tail)

      template <typename REPR_TYPE, int EXPONENT, unsigned N = 2>
        using trunc_subtract_result_t;
      template <typename REPR_TYPE, int EXPONENT, typename ... TAIL>
        trunc_subtract_result_t<REPR_TYPE, EXPONENT, sizeof...(TAIL) + 1>
          constexpr trunc_subtract(fixed_point<REPR_TYPE, EXPONENT> const & addend1, TAIL const & ... addend_tail)

      template <typename FIXED_POINT>
        using trunc_square_result_t;
      template <typename FIXED_POINT>
        trunc_square_result_t<FIXED_POINT>
          constexpr trunc_square(const FIXED_POINT & root) noexcept;

      template <typename FIXED_POINT>
        using trunc_sqrt_result_t;
      template <typename FIXED_POINT>
        trunc_sqrt_result_t<FIXED_POINT>
          constexpr trunc_sqrt(const FIXED_POINT & root) noexcept;
    }

#### `fixed_point<>` Class Template

    template <typename REPR_TYPE, int EXPONENT>
    class fixed_point
    {
    public:
      using repr_type;

      constexpr static int exponent;
      constexpr static int digits;
      constexpr static int integer_digits;
      constexpr static int fractional_digits;

      constexpr fixed_point() noexcept;
      template <typename S>
        explicit constexpr fixed_point(S s) noexcept;
      template <typename FROM_REPR_TYPE, int FROM_EXPONENT>
        explicit constexpr fixed_point(fixed_point<FROM_REPR_TYPE, FROM_EXPONENT> const & rhs) noexcept;

      template <typename S>
        fixed_point & operator=(S s) noexcept;
      template <typename FROM_REPR_TYPE, int FROM_EXPONENT>
        fixed_point & operator=(fixed_point<FROM_REPR_TYPE, FROM_EXPONENT> const & rhs) noexcept

      template <typename S>
        explicit constexpr operator S() const noexcept;
      explicit constexpr operator bool() const noexcept;

      constexpr repr_type data() const noexcept;
      static constexpr fixed_point from_data(repr_type repr) noexcept;

      friend constexpr bool operator==(fixed_point const & lhs, fixed_point const & rhs) noexcept;
      friend constexpr bool operator!=(fixed_point const & lhs, fixed_point const & rhs) noexcept;
      friend constexpr bool operator>(fixed_point const & lhs, fixed_point const & rhs) noexcept;
      friend constexpr bool operator<(fixed_point const & lhs, fixed_point const & rhs) noexcept;
      friend constexpr bool operator>=(fixed_point const & lhs, fixed_point const & rhs) noexcept;
      friend constexpr bool operator<=(fixed_point const & lhs, fixed_point const & rhs) noexcept;

      friend constexpr fixed_point operator-(fixed_point const & rhs) noexcept;
      friend constexpr fixed_point operator+(fixed_point const & lhs, fixed_point const & rhs) noexcept;
      friend constexpr fixed_point operator-(fixed_point const & lhs, fixed_point const & rhs) noexcept;
      friend constexpr fixed_point operator*(fixed_point const & lhs, fixed_point const & rhs) noexcept;
      friend constexpr fixed_point operator/(fixed_point const & lhs, fixed_point const & rhs) noexcept;

      friend fixed_point & operator+=(fixed_point & lhs, fixed_point const & rhs) noexcept;
      friend fixed_point & operator-=(fixed_point & lhs, fixed_point const & rhs) noexcept;
      friend fixed_point & operator*=(fixed_point & lhs, fixed_point const & rhs) noexcept;
      friend fixed_point & operator/=(fixed_point & lhs, fixed_point const & rhs) noexcept;

      // ...
    };

## VI. Future Issues

### Alternative Specializations

The `REPR_TYPE` template parameter is not necessarily the type of the
member variable used to store the value. If there is a more
appropriate choice, the option to take it is reserved.

For example, it is conceivable that explicit template specializations
could be defined which exploit the language-level extensions to the C
type system outlined in proposal N1169 [\[2\]](http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1169.pdf).
It defines built-in fixed-point types which might improve performance
on target systems with suitably-equipped processors.

If practical, the design of the `fixed_point` template should leave
open the option to exploit such system-specific features.

Alternatively, it may be clearer to stipulate that a user may
explicitly specify a type other than one which satisfies `is_integral`
in order to invoke an explicit `fixed_point` specialization, e.g.:

    fixed_point<signed short _Fract>

### Relaxed Rules Surrounding Return Types of Arithmetic Operations

Currently, is is not possible to use a binary operator with
non-matching input types. This leaves open the future possibility of
allowing such input patterns and returning the result in a type which
tries to encompasses both input types.

However, it is not always possible to produce a type which covers the
capabilities of two other types without increasing the size of the
resultant type beyond that of both inputs. For example the most
concise type which can represent the values of both `int8_t` and
`uint16_t` is `int32_t`. In such a case, it may be preferable to
return a value based on `int16_t` and lose the least significant bit
of precision from the `uint16_t` input.

### Library Support

Because the aim is to provide an alternative to existing arithmetic
types which are supported by the standard library, it is conceivable
that a future proposal might specialize existing class templates and
overload existing functions.

Possible candidates for overloading include the functions defined in
<cmath> and a templated specialization of `numeric_limits`. A new type
trait, `is_fixed_point`, would also be useful.

While `fixed_point` is intended to provide drop-in replacements to
existing built-ins, it may be preferable to deviate slightly from the
behavior of certain standard functions. For example, overloads of
functions from <cmath> will be considerably less concise, efficient
and versatile if they obey rules surrounding error cases. In
particular, the guarantee of setting `errno` in the case of an error
prevents a function from being defined as pure. This highlights a
wider issue surrounding the adoption of the functional approach and
compile-time computation that is beyond the scope of this document.

### Bounded Integers

The bounded::integer library [\[3\]](http://doublewise.net/c++/bounded/)
exemplifies the benefits of keeping track of ranges of values in
arithmetic types at compile time.

To a limited extent, the `trunc_` functions defined here also keep
track of - and modify - the limits of values. However, a combination
of techniques is capable of producing superior results.

For instance, consider the following expression:

    make_ufixed<2, 6> three(3);
    auto n = trunc_square(trunc_square(three));

The type of `n` is `make_ufixed<8, 0>` but its value does not
exceed 81. Hence, an unused integer bit has been allocated. It may be
possible to track more accurate limits in the same manner as the
bounded::integer library in order to improve the precision of types
returned by `trunc_` functions. For this reason, details surrounding
these return types are omitted from this proposal.

Notes:
* Bounded::integer is already supported by fixed-point library,
fp [\[4\]](https://github.com/mizvekov/fp).
* A similar library is the boost constrained_value library
[\[5\]](http://rk.hekko.pl/constrained_value/).

### Alternative Policies

The behavior of the types specialized from `fixed_point` represent
one sub-set of all potentially desirable behaviors. Alternative
characteristics include:

* different rounding strategies - other than truncation;
* overflow and underflow checks - possibly throwing exceptions;
* operator return type - adopting `trunc_` or `promote` behavior and
* default-initialize to zero - not done by default.

One way to extend `fixed_point` to cover these alternatives would be
to add a third template parameter containing bit flags. The default
set of values would reflect `fixed_point` as it stands currently.

An example of a fixed-point proposal which takes a similar approach to
rounding and error cases can be found in N3352 [\[6\]](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3352.html).

## VII. References

1. Why Integer Coordinates?, http://www.pathengine.com/Contents/Overview/FundamentalConcepts/WhyIntegerCoordinates/page.php
2. N1169, Extensions to support embedded processors, <http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1169.pdf>
3. C++ bounded::integer library, <http://doublewise.net/c++/bounded/>
4. fp, C++14 Fixed Point Library, <https://github.com/mizvekov/fp>
5. Boost Constrained Value Libarary, <http://rk.hekko.pl/constrained_value/>
6. N3352, C++ Binary Fixed-Point Arithmetic, <http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3352.html>
