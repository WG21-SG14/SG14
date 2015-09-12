**Document number**: LEWG, EWG, SG14, SG6: D0037R0  
**Date**: 2015-09-12  
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

Any usage that results in values of `EXPONENT` which lie outside the
range, (INT_MIN / 2, INT_MAX / 2), may result in undefined
behavior and/or overflow or underflow. This range of exponent values
is far in excess of the largest built-in floting-point type and should
be adequate for all intents and purposes.

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
applied to `fixed_point` specializations. A guiding principle of
operator overloads is that they perform as little run-time computation
as is practically possible.

With the exception of shift operators, binary operators can take any
combination of:

* one or two arguments of a single specialization of `fixed_point` and
* zero or one arguments of any arithmetic type.

The return value is the same `fixed_point` type as the input(s) in all
cases. The reason that heterogeneous specializations are not accepted
is that the type of the result would be problematic to determine and
possibly require shift operations to produce.

Shift operator overloads require an integer type as the right-hand
parameter and return a type which is adjusted to accommodate the new
value without risk of overflow or underflow.

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

    trunc_add(FIXED_POINT_1, FIXED_POINT_2)
    trunc_subtract(FIXED_POINT_1, FIXED_POINT_2)
    trunc_multiply(FIXED_POINT_1, FIXED_POINT_2)
    trunc_divide(FIXED_POINT_1, FIXED_POINT_2)
    trunc_invert(FIXED_POINT)
    trunc_square(FIXED_POINT)
    trunc_sqrt(FIXED_POINT)
    trunc_shift_left(FIXED_POINT, INTEGER)
    trunc_shift_right(FIXED_POINT, INTEGER)
    promote_multiply(FIXED_POINT_1, FIXED_POINT_2)
    promote_divide(FIXED_POINT_1, FIXED_POINT_2)
    promote_invert(FIXED_POINT)
    promote_square(FIXED_POINT)

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
6. the `_add`, `_subtract` and `_multiply` functions take
   heterogeneous `fixed_point` specializations;
7. the `_divide` and `_invert` functions in no way guard against
   divide-by-zero errors and
8. The `trunc_shift_` functions return results of the same type as
  their first input parameter.

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

      // arithmetic operators
      ...

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

      // additional named arithmetic functions
      ...
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

### Relaxed Rules Surrounding Arithmetic Operatior Types

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

One situation where heterogeneous input types is desired is when
introducing a literal to an expression, for example:

    auto x = a * t + b * (1.0 - t)

Currently, this requires cumbersome casting to convert the literal to
a suitable fixed-point type.

One possible solution is to introduce a user-defined literal which
produces a fixed_point object with a suitable exponent. However,
choosing a specialization that is appropriate in all situations may
be unrealistic.

Another possibility is to overload arithmetic operators for all types
which satisfy `is_arithmetic`. This would require many function
template definitions.

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

### Alternatives to Built-in Integer Types

The reason that `REPR_TYPE` is restricted to built-in integer types
is that a number of features require the use of a higher - or
lower-capacity type. Supporting alias templates are defined to
provide `fixed_point` with the means to invoke integer types of
specific capacity and signedness at compile time.

There is no general purpose way of deducing a higher or
lower-capacity type given a source type in the same manner as
`make_signed` and `make_unsigned`. If there were, this might be
adequate to allow alternative choices for `REPR_TYPE`.

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
exceed 81. Hence, an unused integer bit has been allocated. It may be
possible to track more accurate limits in the same manner as the
bounded::integer library in order to improve the precision of types
returned by `trunc_` functions. For this reason, details surrounding
these return types are omitted from this proposal.

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
* default-initialize to zero - not done by default and
* saturation arithmetic - as opposed to modular arithmetic.

One way to extend `fixed_point` to cover these alternatives would be
to add non-type template parameters containing bit flags or enumerated
types. The default set of values would reflect `fixed_point` as it
stands currently.

## VII. Prior Art

Many examples of fixed-point support in C and C++ exist. While almost
all of them aim for low run-time cost and expressive alternatives to
raw integer manipulation, they vary greatly in terms of their
interface.

One especially interesting dichotomy is between solutions which offer
a discrete selection of fixed-point types and libraries which contain
a continuous range of types through type parameterization.

### N1169

One example of the former is found in proposal N1169
[\[5\]](http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1169.pdf),
the intent of which is to expose features found in certain embedded
hardware. It introduces a succinct set of language-level fixed-point
types with certain constraints on the number of integer or fractional
digits each can possess.

As with all examples of discrete-type fixed-point support, the limited
choice of exponents is a considerable restriction on the versatility
and expressiveness of the API.

Nevertheless, it may be possible to harness performance gains provided
by N1169 fixed-point types through explicit template specialization.
This is likely to be a valuable proposition to potential users of the
library who find themselves targeting architectures which already
benefit from N1169.

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
templates the storage in bits and - for fractional types - the
resolution.

The `fixed_point` class template could probably - with a few caveats
- be generated using the two fractional types, `nonnegative` and
`negatable`, replacing the REPR_TYPE parameter with the integer bit
count of REPR_TYPE, specifying either `fastest` or `truncated` for the
rounding mode and specifying `undefined` as the overflow mode.

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
  vital machine-level details;
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
Code: Peter Schregle, Ryhor Spivak
Design: Marco Foco, Joël Lamotte
Discussion: Ed Ainsley, Billy Baker, Clément Grégoire,
xSean Middleditch

## IX. References

1. Why Integer Coordinates?, <http://www.pathengine.com/Contents/Overview/FundamentalConcepts/WhyIntegerCoordinates/page.php>
2. C++ bounded::integer library, <http://doublewise.net/c++/bounded/>
3. fp, C++14 Fixed Point Library, <https://github.com/mizvekov/fp>
4. Boost Constrained Value Libarary, <http://rk.hekko.pl/constrained_value/>
5. N1169, Extensions to support embedded processors, <http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1169.pdf>
6. fpmath, Fixed Point Math Library, <http://www.codeproject.com/Articles/37636/Fixed-Point-Class>
7. Boost fixed_point (proposed), Fixed point integral and fractional types, <https://github.com/viboes/fixed_point>
8. N3352, C++ Binary Fixed-Point Arithmetic, <http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3352.html>
