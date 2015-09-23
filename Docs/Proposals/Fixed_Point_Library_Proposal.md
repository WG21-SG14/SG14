**Document number**: LEWG, EWG, SG14, SG6: D0037R3  
**Date**: 2015-09-19  
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
preferable. Some systems lack native floating-point registers and must
emulate them in software. Many others are capable of performing some
or all operations more efficiently using integer arithmetic. Certain
applications can suffer from the variability in precision which comes
from a dynamic radix point [\[1\]](http://www.pathengine.com/Contents/Overview/FundamentalConcepts/WhyIntegerCoordinates/page.php).
In situations where a variable exponent is not desired, it takes
valuable space away from the significand and reduces precision.

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

    template <class ReprType, int Exponent>
    class fixed_point;

where the template parameters are described as follows.

#### `ReprType` Type Template Parameter

This parameter identifies the capacity and signedness of the
underlying type used to represent the value. In other words, the size
of the resulting type will be `sizeof(ReprType)` and it will be
signed iff `is_signed<ReprType>::value` is true. The default is
`int`.

`ReprType` must be a fundamental integral type and should not be the
largest size. Suitable types include: `std::int8_t`, `std::uint8_t`,
`std::int16_t`, `std::uint16_t`, `std::int32_t` and `std::uint32_t`.
In limited situations, `std::int64_t` and `std::uint64_t` can be used.
The  reasons for these limitations relate to the difficulty in finding
a type that is suitable for performing lossless integer
multiplication.

#### `Exponent` Non-Type Template Parameter

The exponent of a fixed-point type is the equivalent of the exponent
field in a floating-point type and shifts the stored value by the
requisite number of bits necessary to produce the desired range. The
default value of `Exponent` is zero, giving `fixed_point<T>` the same
range as `T`.

The resolution of a specialization of `fixed_point` is

    pow(2, Exponent)

and the minimum and maximum values are

    std::numeric_limits<ReprType>::min() * pow(2, Exponent)

and

    std::numeric_limits<ReprType>::max() * pow(2, Exponent)

respectively.

Any usage that results in values of `Exponent` which lie outside the
range, (`INT_MIN / 2`, `INT_MAX / 2`), may result in undefined
behavior and/or overflow or underflow. This range of exponent values
is far in excess of the largest built-in floting-point type and should
be adequate for all intents and purposes.

### `make_fixed` and `make_ufixed` Helper Type

The `Exponent` template parameter is versatile and concise. It is an
intuitive scale to use when considering the full range of positive and
negative exponents a fixed-point type might possess. It also
corresponds to the exponent field of built-in floating-point types.

However, most fixed-point formats can be described more intuitively by
the cardinal number of integer and/or fractional digits they contain.
Most users will prefer to distinguish fixed-point types using these
parameters.

For this reason, two aliases are defined in the style of
`make_signed`.

These aliases are declared as:

    template <unsigned IntegerDigits, unsigned FractionalDigits = 0, bool IsSigned = true>
  	using make_fixed;

and

    template <unsigned IntegerDigits, unsigned FractionalDigits = 0>
    using make_ufixed;

They resolve to a `fixed_point` specialization with the given
signedness and number of integer and fractional digits. They may
contain additional integer and fractional digits.

For example, one could define and initialize an 8-bit, unsigned,
fixed-point variable with four integer digits and four fractional
digits:

    make_ufixed<4, 4> value { 15.9375 };

or a 32-bit, signed, fixed-point number with two integer digits and 29
fractional digits:

    make_fixed<2, 29> value { 3.141592653 };

### Conversion

Fixed-point numbers can be explicitly converted to and from built-in
arithmetic types.

While effort is made to ensure that significant digits are not lost
during conversion, no effort is made to avoid rounding errors.
Whatever would happen when converting to and from an integer type
largely applies to `fixed_point` objects also. For example:

    make_ufixed<4, 4>(.006) == make_ufixed<4, 4>(0)

...equates to `true` and is considered a acceptable rounding error.

### Operator Overloads

Any operators that might be applied to integer types can also be
applied to fixed-point types. A guiding principle of operator
overloads is that they perform as little run-time computation as is
practically possible.

With the exception of shift and comparison operators, binary operators
can take any combination of:

* one or two fixed-point arguments and
* zero or one arguments of any arithmetic type, i.e. a type for which
  `is_arithmetic` is true.

Where the inputs are not identical fixed-point types, a simple set of
promotion-like rules are applied to determine the return type:

1. If both arguments are fixed-point, a type is chosen which is the
   size of the larger type, is signed if either input is signed and
   has the maximum integer bits of the two inputs, i.e. cannot lose
   high-significance bits through conversion alone.
2. If one of the arguments is a floating-point type, then the type of
   the result is the smallest floating-point type of equal or greater
   size than the inputs.
3. If one of the arguments is an integral type, then the result is the
   other, fixed-point type.

Some examples:

    make_ufixed<5, 3>{8} + make_ufixed<4, 4>{3} == make_ufixed<5, 3>{11};  
    make_ufixed<5, 3>{8} + 3 == make_ufixed<5, 3>{11};  
    make_ufixed<5, 3>{8} + float{3} == float{11};  

The reasoning behind this choice is a combination of predictability
and performance. It is explained for each rule as follows:

1. ensures that the least computation is performed where fixed-point
   types are used exclusively. Aside from multiplication and division
   requiring shift operations, should require similar computational
   costs to equivalent integer operations;
2. loosely follows the promotion rules for mixed-mode arithmetic,
   ensures values with exponents far beyond the range of the
   fixed-point type are catered for and avoids costly conversion from
   floating-point to integer and
3. preserves the input fixed-point type whose range is far more likely
   to be of deliberate importance to the operation.

Shift operator overloads require an integer type as the right-hand
parameter and return a type which is adjusted to accommodate the new
value without risk of overflow or underflow.

Comparison operators convert the inputs to a common result type
following the rules above before performing a comparison and returning
`true` or `false`.

#### Overflow

Because arithmetic operators return a result of equal capacity to
their inputs, they carry a risk of overflow. For instance,

    make_fixed<4, 3>(15) + make_fixed<4, 3>(1)

causes overflow because because a type with 4 integer bits cannot
store a value of 16.

Overflow of any bits in a signed or unsigned fixed-point type is
classed as undefined behavior. This is a minor deviation from
built-in integer arithmetic where only signed overflow results in
undefined behavior.

#### Underflow

The other typical cause of lost bits is underflow where, for example,

    make_fixed<7, 0>(15) / make_fixed<7, 0>(2)

results in a value of 7. This results in loss of precision but is
generally considered acceptable.

However, when all bits are lost due to underflow, the value is said
to be flushed and this is classed as undefined behavior.

### Dealing With Overflow and Flushes

Errors resulting from overflow and flushes are two of the biggest
headaches related to fixed-point arithmetic. Integers suffer the same
kinds of errors but are somewhat easier to reason about as they lack
fractional digits. Floating-point numbers are largely shielded from
these errors by their variable exponent and implicit bit.

Three strategies for avoiding overflow in fixed-point types are
presented:

1. simply leave it to the user to avoid overflow;
2. promote the result to a larger type to ensure sufficient capacity
   or
3. adjust the exponent of the result upward to ensure that the top
   limit of the type is sufficient to preserve the most significant
   digits at the expense of the less significant digits.

For arithmetic operators, choice 1) is taken because it most closely
follows the behavior of integer types. Thus it should cause the least
surprise to the fewest users. This makes it far easier to reason
about in code where functions are written with a particular type in
mind. It also requires the least computation in most cases.

Choices 2) and 3) are more robust to overflow events. However, they
represent different trade-offs and neither one is the best fit in all
situations. For these reasons, they are presented as named functions.

#### Type Promotion

Function template, `promote`, borrows a term from the language
feature which avoids integer overflow prior to certain operations. It
takes a `fixed_point` object and returns the same value represented
by a larger `fixed_point` specialization.

For example,

    promote(make_fixed<5, 2>(15.5))

is equivalent to

    make_fixed<11, 4>(15.5)

Complimentary function template, `demote`, reverses the process,
returning a value of a smaller type.

#### Named Arithmetic Functions

The following named function templates can be used as a hassle-free
alternative to arithmetic operators in situations where the aim is
to avoid overflow:

    trunc_add(FixedPoint1, FixedPoint2)
    trunc_subtract(FixedPoint1, FixedPoint2)
    trunc_multiply(FixedPoint1, FixedPoint2)
    trunc_divide(FixedPoint1, FixedPoint2)
    trunc_reciprocal(FixedPoint)
    trunc_square(FixedPoint)
    trunc_sqrt(FixedPoint)
    trunc_shift_left(FixedPoint, Integer)
    trunc_shift_right(FixedPoint, Integer)
    promote_multiply(FixedPoint1, FixedPoint2)
    promote_divide(FixedPoint1, FixedPoint2)
    promote_reciprocal(FixedPoint)
    promote_square(FixedPoint)

Some notes:

1. The `trunc_` functions return the result as a type no larger than
   the inputs and with an exponent adjusted to avoid overflow;
2. the `promote_` functions return the result as a type large enough
   to avoid overflow and underflow;
3. the `_multiply` and `_square` functions are not guaranteed to be
   available for 64-bit types;
4. the `_multiply` and `_square` functions produce undefined behavior
   when all input parameters are the *most negative number*;
5. the `_square` functions return an unsigned type;
6. the `_add`, `_subtract`, `_multiply` and `_divide` functions take
   heterogeneous `fixed_point` specializations;
7. the `_divide` and `_reciprocal` functions in no way guard against
   divide-by-zero errors and
8. the `trunc_shift_` functions return results of the same type as
  their first input parameter.

### Example

The following example calculates the magnitude of a 3-dimensional vector.

    template <class Fp>
    constexpr auto magnitude(const Fp & x, const Fp & y, const Fp & z)
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
      template <class ReprType, int Exponent> class fixed_point;

      template <unsigned FractionalDigits, unsigned FractionalDigits = 0, bool IsSigned = true>
        using make_fixed;
      template <unsigned IntegerDigits, unsigned FractionalDigits = 0>
        using make_ufixed;

      template <class FixedPoint>
        using fixed_point_promotion_t;
      template <class FixedPoint>
        fixed_point_promotion_t<FixedPoint>
          constexpr promote(const FixedPoint & from) noexcept

      template <class FixedPoint>
        using fixed_point_demotion_t;
      template <class FixedPoint>
        fixed_point_demotion_t<FixedPoint>
          constexpr demote(const FixedPoint & from) noexcept

      template <class Lhs, class Rhs>
        constexpr bool operator ==(const Lhs & lhs, const Rhs & rhs) noexcept;
      template <class Lhs, class Rhs>
        constexpr bool operator !=(const Lhs & lhs, const Rhs & rhs) noexcept;
      template <class Lhs, class Rhs>
        constexpr bool operator <(const Lhs & lhs, const Rhs & rhs) noexcept;
      template <class Lhs, class Rhs>
        constexpr bool operator >(const Lhs & lhs, const Rhs & rhs) noexcept;
      template <class Lhs, class Rhs>
        constexpr bool operator >=(const Lhs & lhs, const Rhs & rhs) noexcept;
      template <class Lhs, class Rhs>
        constexpr bool operator <=(const Lhs & lhs, const Rhs & rhs) noexcept;

      // arithmetic operators
      ...

      template <class Lhs, class Rhs = Lhs>
        using trunc_multiply_result_t;
      template <class Lhs, class Rhs>
        trunc_multiply_result_t<Lhs, Rhs>
          constexpr trunc_multiply(const Lhs & factor1, const Rhs & factor2) noexcept;

      template <class ReprType, int Exponent, unsigned N = 2>
        using trunc_add_result_t;
      template <class ReprType, int Exponent, class ... Tail>
        trunc_add_result_t<ReprType, Exponent, sizeof...(Tail) + 1>
          constexpr trunc_add(const fixed_point<ReprType, Exponent> & addend1, const Tail & ... addend_tail)

      template <class ReprType, int Exponent, unsigned N = 2>
        using trunc_subtract_result_t;
      template <class ReprType, int Exponent, class ... Tail>
        trunc_subtract_result_t<ReprType, Exponent, sizeof...(Tail) + 1>
          constexpr trunc_subtract(const fixed_point<ReprType, Exponent> & addend1, const Tail & ... addend_tail)

      template <class FixedPoint>
        using trunc_square_result_t;
      template <class FixedPoint>
        trunc_square_result_t<FixedPoint>
          constexpr trunc_square(const FixedPoint & root) noexcept;

      template <class FixedPoint>
        using trunc_sqrt_result_t;
      template <class FixedPoint>
        trunc_sqrt_result_t<FixedPoint>
          constexpr trunc_sqrt(const FixedPoint & root) noexcept;

      // additional named arithmetic functions
      ...
    }

#### `fixed_point<>` Class Template

    template <class ReprType, int Exponent>
    class fixed_point
    {
    public:
      using ReprType;

      constexpr static int exponent;
      constexpr static int digits;
      constexpr static int integer_digits;
      constexpr static int fractional_digits;

      constexpr fixed_point() noexcept;
      template <class S>
        explicit constexpr fixed_point(S s) noexcept;
      template <class FromReprType, int FromExponent>
        explicit constexpr fixed_point(const fixed_point<FromReprType, FromExponent> & rhs) noexcept;

      template <class S>
        fixed_point & operator=(S s) noexcept;
      template <class FromReprType, int FromExponent>
        fixed_point & operator=(const fixed_point<FromReprType, FromExponent> & rhs) noexcept

      template <class S>
        explicit constexpr operator S() const noexcept;
      explicit constexpr operator bool() const noexcept;

      constexpr ReprType data() const noexcept;
      static constexpr fixed_point from_data(ReprType repr) noexcept;

      friend constexpr bool operator==(const fixed_point & lhs, const fixed_point & rhs) noexcept;
      friend constexpr bool operator!=(const fixed_point & lhs, const fixed_point & rhs) noexcept;
      friend constexpr bool operator>(const fixed_point & lhs, const fixed_point & rhs) noexcept;
      friend constexpr bool operator<(const fixed_point & lhs, const fixed_point & rhs) noexcept;
      friend constexpr bool operator>=(const fixed_point & lhs, const fixed_point & rhs) noexcept;
      friend constexpr bool operator<=(const fixed_point & lhs, const fixed_point & rhs) noexcept;

      friend constexpr fixed_point operator-(const fixed_point & rhs) noexcept;
      friend constexpr fixed_point operator+(const fixed_point & lhs, const fixed_point & rhs) noexcept;
      friend constexpr fixed_point operator-(const fixed_point & lhs, const fixed_point & rhs) noexcept;
      friend constexpr fixed_point operator*(const fixed_point & lhs, const fixed_point & rhs) noexcept;
      friend constexpr fixed_point operator/(const fixed_point & lhs, const fixed_point & rhs) noexcept;

      friend fixed_point & operator+=(fixed_point & lhs, const fixed_point & rhs) noexcept;
      friend fixed_point & operator-=(fixed_point & lhs, const fixed_point & rhs) noexcept;
      friend fixed_point & operator*=(fixed_point & lhs, const fixed_point & rhs) noexcept;
      friend fixed_point & operator/=(fixed_point & lhs, const fixed_point & rhs) noexcept;

      // ...
    };

## VI. Future Issues

### Library Support

Because the aim is to provide an alternative to existing arithmetic
types which are supported by the standard library, it is conceivable
that a future proposal might specialize existing class templates and
overload existing functions.

Possible candidates for overloading include the functions defined in
\<cmath\> and a templated specialization of `numeric_limits`. A new type
trait, `is_fixed_point`, would also be useful.

While `fixed_point` is intended to provide drop-in replacements to
existing built-ins, it may be preferable to deviate slightly from the
behavior of certain standard functions. For example, overloads of
functions from \<cmath\> will be considerably less concise, efficient
and versatile if they obey rules surrounding error cases. In
particular, the guarantee of setting `errno` in the case of an error
prevents a function from being defined as pure. This highlights a
wider issue surrounding the adoption of the functional approach and
compile-time computation that is beyond the scope of this document.

### Alternatives to Built-in Integer Types

The reason that `ReprType` is restricted to built-in integer types
is that a number of features require the use of a higher - or
lower-capacity type. Supporting alias templates are defined to
provide `fixed_point` with the means to invoke integer types of
specific capacity and signedness at compile time.

There is no general purpose way of deducing a higher or
lower-capacity type given a source type in the same manner as
`make_signed` and `make_unsigned`. If there were, this might be
adequate to allow alternative choices for `ReprType`.

### Bounded Integers

The bounded::integer library [\[2\]](http://doublewise.net/c++/bounded/)
exemplifies the benefits of keeping track of ranges of values in
arithmetic types at compile time.

To a limited extent, the `trunc_` functions defined here also keep
track of - and modify - the limits of values. However, a combination
of techniques is capable of producing superior results.

For instance, consider the following expression:

    make_ufixed<2, 6> three(3);
    auto n = trunc_square(trunc_square(three));

The type of `n` is `make_ufixed<8, 0>` but its value does not
exceed 81. Hence, an integer bit has been wasted. It may be possible
to track more accurate limits in the same manner as the
bounded::integer library in order to improve the precision of types
returned by `trunc_` functions. For this reason, the exact value of
the exponents of these return types is not given.

Notes:
* Bounded::integer is already supported by fixed-point library,
fp [\[3\]](https://github.com/mizvekov/fp).
* A similar library is the boost constrained_value library
[\[4\]](http://rk.hekko.pl/constrained_value/).

### Alternative Policies

The behavior of the types specialized from `fixed_point` represent
one sub-set of all potentially desirable behaviors. Alternative
characteristics include:

* different rounding strategies - other than truncation;
* overflow and underflow checks - possibly throwing exceptions;
* operator return type - adopting `trunc_` or `promote_` behavior;
* default-initialize to zero - currently uninitialized and
* saturation arithmetic - as opposed to modular arithmetic.

One way to extend `fixed_point` to cover these alternatives would be
to add non-type template parameters containing bit flags or enumerated
types. The default set of values would reflect `fixed_point` as it
stands currently.

## VII. Prior Art

Many examples of fixed-point support in C and C++ exist. While almost
all of them aim for low run-time cost and expressive alternatives to
raw integer manipulation, they vary greatly in detail and in terms of
their interface.

One especially interesting dichotomy is between solutions which offer
a discrete selection of fixed-point types and libraries which contain
a continuous range of exponents through type parameterization.

### N1169

One example of the former is found in proposal N1169
[\[5\]](http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1169.pdf),
the intent of which is to expose features found in certain embedded
hardware. It introduces a succinct set of language-level fixed-point
types and impose constraints on the number of integer or fractional
digits each can possess.

As with all examples of discrete-type fixed-point support, the limited
choice of exponents is a considerable restriction on the versatility
and expressiveness of the API.

Nevertheless, it may be possible to harness performance gains provided
by N1169 fixed-point types through explicit template specialization.
This is likely to be a valuable proposition to potential users of the
library who find themselves targeting platforms which support
fixed-point arithmetic at the hardware level.

### N3352

There are many other C++ libraries available which fall into the
latter category of continuous-range fixed-point arithmetic
[\[3\]](https://github.com/mizvekov/fp)
[\[6\]](http://www.codeproject.com/Articles/37636/Fixed-Point-Class)
[\[7\]](https://github.com/viboes/fixed_point). In particular, an
existing library proposal, N3352 [\[8\]](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3352.html),
aims to achieve very similar goals through similar means and warrants
closer comparison than N1169.

N3352 introduces four class templates covering the quadrant of signed
versus unsigned and fractional versus integer numeric types. It is
intended to replace built-in types in a wide variety of situations and
accordingly, is highly compile-time configurable in terms of how
rounding and overflow are handled. Parameters to these four class
templates include the storage in bits and - for fractional types - the
resolution.

The `fixed_point` class template could probably - with a few caveats -
be generated using the two fractional types, `nonnegative` and
`negatable`, replacing the `ReprType` parameter with the integer bit
count of `ReprType`, specifying `fastest` for the rounding mode and
specifying `undefined` as the overflow mode.

However, fixed_point more closely and concisely caters to the needs of
users who already use integer types and simply desire a more concise,
less error-prone form. It more closely follows the four design aims of
the library and - it can be argued - more closely follows the spirit
of the standard in its pursuit of zero-cost abstraction.

Some aspects of the design of the N3352 API which back up these
conclusion are that:

* the result of arithmetic operations closely resemble the `trunc_`
  function templates and are potentially more costly at run-time;
* the nature of the range-specifying template parameters - through
  careful framing in mathematical terms - abstracts away valuable
  information regarding machine-critical type size information;
* the breaking up of duties amongst four separate class templates
  introduces four new concepts and incurs additional mental load for
  relatively little gain while further detaching the interface from
  vital machine-level details and
* the absence of the most negative number from signed types reduces
  the capacity of all types by one.

The added versatility that the N3352 API provides regarding rounding
and overflow handling are of relatively low priority to users who
already bear the scars of battles with raw integer types.
Nevertheless, providing them as options to be turned on or off at
compile time is an ideal way to leave the choice in the hands of the
user.

Many high-performance applications - in which fixed-point is of
potential value - favor run-time checks during development which are
subsequently deactivated in production builds. The N3352 interface is
highly conducive to this style of development. It is an aim of the
fixed_point design to be similarly extensible in future revisions.

## VIII. Acknowledgements

Subgroup: Guy Davidson, Michael Wong  
Contributors: Ed Ainsley, Billy Baker, Lance Dyson, Marco Foco,
Clément Grégoire, Nicolas Guillemot, Matt Kinzelman, Joël Lamotte,
Sean Middleditch, Patrice Roy, Peter Schregle, Ryhor Spivak

## IX. References

1. Why Integer Coordinates?, <http://www.pathengine.com/Contents/Overview/FundamentalConcepts/WhyIntegerCoordinates/page.php>
2. C++ bounded::integer library, <http://doublewise.net/c++/bounded/>
3. fp, C++14 Fixed Point Library, <https://github.com/mizvekov/fp>
4. Boost Constrained Value Libarary, <http://rk.hekko.pl/constrained_value/>
5. N1169, Extensions to support embedded processors, <http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1169.pdf>
6. fpmath, Fixed Point Math Library, <http://www.codeproject.com/Articles/37636/Fixed-Point-Class>
7. Boost fixed_point (proposed), Fixed point integral and fractional types, <https://github.com/viboes/fixed_point>
8. N3352, C++ Binary Fixed-Point Arithmetic, <http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3352.html>
