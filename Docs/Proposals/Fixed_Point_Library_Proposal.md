**Document number**: (draft)  
**Date**: 2015-08-29  
**Project**: Programming Language C++, Library Evolution WG, Evolution WG, SG14  
**Reply-to**: John McFarlane, [john@mcfarlane.name](mailto:john@mcfarlane.name)

# Fixed-Point Real Numbers

## I. Introduction

This proposal introduces a system for performing binary fixed-point
arithmetic using built-in integral types.

## II. Motivation

Floating-point types are an exceedingly versatile and widely supported
method of expressing real numbers on modern architectures. However,
there are certain situations where fixed-point arithmetic is
preferable.

Some system lack native floating-point registers and must emulate them
in software. Many others are capable of performing some or
all operations more efficiently using integer arithmetic. Certain
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

Fixed-point numbers are specializations of `template <typename
REPR_TYPE, int EXPONENT> class fixed_point` where:

* `REPR_TYPE` implies the underlying type used to store the value. A
  type which satisfies `is_integral` is an ideal choice. The size of
  the resulting type will be `sizeof(REPR_TYPE)` and it will be signed
  iff `is_signed<REPR_TYPE>::value` is true.

  The default is `int`.

* `EXPONENT` is the equivalent of the exponent field in a
  floating-point type and shifts the stored value by the requisite
  number of bits necessary to produce the desired range.

  The default value is dependent on `REPR_TYPE` and ensures that half
  of the bits of the type are allocated to fractional digits. The
  other half go to integer digits and the signe bit.

Some example specializations include the default -- typically a 32-bit
object with a sign bit, 15 integer digits and 16 fractional digits:

    fixed_point<>

an unsigned 16-bit type with 8 integer digits and 8 fractional digits:

    fixed_point<uint16_t>

a signed 32-bit type which can represent half values:

    fixed_point<int32_t, -1>

and an unsigned 8-bit type with 8 fractional digits that can be used
to store values in the range, [0, 1):

    fixed_point<uint8_t, -8>

### Conversion

Fixed-point numbers can be explicitly converted to and from built-in
arithmetic types.

While effort is made to ensure that significant digits are not lost
during conversion, no effort is made to avoid rounding errors.
Whatever would happen when converting to and from an integer type
largely applies to `fixed_point` objects also. For example:

    fixed_point<uint8_t, -4>(.006) == fixed_point<uint8_t, -4>(0)

...equates to `true` and is considered a tolerable rounding error.

### Named Constructors

The `EXPONENT` template parameter is a versatile way to express the
full range of values a type might contain - including very large
positive and negative values. However, it is not necessarily the most
intuitive choice for a typical user.

There are a number of conventions by which fixed-point number types
are traditionally expressed. And because the template parameters of
`fixed_point` do not necessarily fit with these conventions, a number
of named constructors are defined in the spirit of existing `make_X`
functions to make specialization of `fixed_point<>` more convenient.

#### `make_fixed`

To create a specialization with *I* integer digits, *F* fractional
digits and signed digit *S*, there is:

    template <unsigned I, unsigned F, bool S = true> make_fixed

This alias automatically determines what underlying type is needed to
accommodate the desired amount of precisions. For example,

    make_fixed<8, 11, true>

is guaranteed to be a signed fixed-point type with 8 integer digits
and at least 11 fractional digits. As there is no standard 20-bit
integer type, a `REPR_TYPE` of `int32_t` is used and an additional 12
bits are devoted to fractional digits: `fixed_point<int32_t, -23>`.

#### `make_fixed_from_repr`

When discussing a fixed-point type, the number of fractional digits
often hogs the limelight. It is really the number of integer digits
that are - literally - most significant. In cases where the underlying
type and the number of integer digits are known, one can use

    template <typename REPR_TYPE, int I> make_fixed_from_repr

to instantiate a type. For example:

    make_fixed_from_repr<uint16_t, 4>

is an alias for a 16-bit unsigned type, capable of storing value, 15.

#### `make_fixed_from_pair`

When the greater of two fixed-point types is required, such as when
comparing heterogeneous values or taking their maximum, use:

    make_fixed_from_pair<FIXED_POINT_1, FIXED_POINT_2>

### Arithmetic Operators

Any operators that might be applied to integer types can also be
applied to `fixed_point` specializations. Input parameters and return
value are all of the same type. (A possible exception to this is the
right hand parameter of bit shift operators.)

Because of this choice, arithmetic operators carry a risk of overflow.
For instance,

    fixed_point<uint8_t, -4>(15) * fixed_point<uint8_t, -4>(15)

returns `fixed_point<uint8_t, -4>(1)` as the input type does not have
the capacity to represent the value, 225.

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
type in mind.

Choices 2) and 3) are more robust to overflow events. However, they
represent different trade-offs and neither one is the best fit in all
situations. For these reasons, they are offered via named functions.

### Type Promotion

Function template, `promote`, borrows a term from the language feature
which avoids integer overflow prior to certain operations. It takes a
`fixed_point<>` object and returns the same value represented by a
larger `fixed_point<>` specialization.

For example,

    promote(fixed_point<int8_t, -2>(15.5))

is equivalent to

    fixed_point<int16_t, -4>(15.5)

Complimentary function template, `demote`, inverts the process,
returning a value of a smaller type.

### Named Arithmetic Functions

The following named function templates can be used as alternatives to
arithmetic operators in situations where the aim is to avoid overflow
without falling back on higher-capacity types.

    shift_multiply(FIXED_POINT_1, FIXED_POINT_2)
    shift_add(FIXED_POINT_1, FIXED_POINT_2)
    shift_subtract(FIXED_POINT_1, FIXED_POINT_2)
    shift_square(FIXED_POINT_1)
    shift_sqrt(FIXED_POINT_1)

These templates return results whose types have adjusted `EXPONENT`
values which prevent overflow in the vast majority of cases. The
`shift_` function templates have two drawbacks:

Firstly, because the capacity of the return type is not increased,
precision may be lost. For example:

    shift_square(fixed_point<uint8_t, -4>(15.9375))

returns `fixed_point<uint8_t, 0>(254)`. This result is far closer to
the correct value than the result returned by `operator *`.

Secondly, there are rare cases where overflow can still occur due to
the nature of twos-compliment binary arithmetic and the *most negative
number*. For instance, the value of

    shift_square(fixed_point<int8_t, 0>(-128))

is zero because 16384 cannot be represented in a
`fixed_point<uint8_t, 6>` type.

### Overflow and Underflow

The major disadvantage of the way in which `shift_` functions protect
the more significant digits is that the less insignificant digits are
dropped. In the extreme case of underflow, all the bits are lost and
the value becomes zero.

For example,

    shift_square(fixed_point<uint8_t, 0>(15))

causes the object to be flushed to zero.

While often not as serious as overflow, underflow can cause knock-on
effects, such as when a flushed value is subsequently used as a
divisor.

There is a strong desire to catch situations where a `fixed_point`
value transitions to a flushed state. However, it is unacceptable that
this should necessarily require a run-time check.

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
    -> decltype(shift_sqrt(shift_add(shift_square(x), shift_square(y), shift_square(z))))
    {
        return shift_sqrt(shift_add(shift_square(x), shift_square(y), shift_square(z)));
    }

Calling the above function as follows

    static_cast<double>(magnitude(
        fixed_point<uint16_t, -12>(1),
        fixed_point<uint16_t, -12>(4),
        fixed_point<uint16_t, -12>(9)));

returns the value, 9.890625.

## V. Technical Specification

### Header

All proposed additions to the library are contained in header,
`<fixed_point>`.

### Class template `fixed_point`

#### Header `<fixed_point>` Synopsis

    namespace std {
      template <typename REPR_TYPE, int EXPONENT> class fixed_point;

      template <unsigned INTEGER_DIGITS, unsigned FRACTIONAL_DIGITS, bool IS_SIGNED = true>
        using make_fixed;
      template <typename REPR_TYPE, int INTEGER_BITS>
        using make_fixed_from_repr;
      template <typename LHS_FP, typename RHS_FP>
        using make_fixed_from_pair;

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
        using shift_multiply_result_t;
      template <typename LHS, typename RHS>
        shift_multiply_result_t<LHS, RHS>
          constexpr shift_multiply(const LHS & factor1, const RHS & factor2) noexcept;

      template <typename REPR_TYPE, int EXPONENT, unsigned N = 2>
        using shift_add_result_t;
      template <typename REPR_TYPE, int EXPONENT, typename ... TAIL>
        shift_add_result_t<REPR_TYPE, EXPONENT, sizeof...(TAIL) + 1>
          constexpr shift_add(fixed_point<REPR_TYPE, EXPONENT> const & addend1, TAIL const & ... addend_tail)

      template <typename REPR_TYPE, int EXPONENT, unsigned N = 2>
        using shift_subtract_result_t;
      template <typename REPR_TYPE, int EXPONENT, typename ... TAIL>
        shift_subtract_result_t<REPR_TYPE, EXPONENT, sizeof...(TAIL) + 1>
          constexpr shift_subtract(fixed_point<REPR_TYPE, EXPONENT> const & addend1, TAIL const & ... addend_tail)

      template <typename FIXED_POINT>
        using shift_square_result_t;
      template <typename FIXED_POINT>
        shift_square_result_t<FIXED_POINT>
          constexpr shift_square(const FIXED_POINT & root) noexcept;

      template <typename FIXED_POINT>
        using shift_sqrt_result_t;
      template <typename FIXED_POINT>
        shift_sqrt_result_t<FIXED_POINT>
          constexpr shift_sqrt(const FIXED_POINT & root) noexcept;
    }

#### Class template `fixed_point<>`

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

To a limited extent, the `shift_` functions defined here also keep
track of - and modify - the limits of values. However, a combination
of techniques is capable of producing superior results.

For instance, consider the following expression:

    fixed_point<uint8_t, -6> three(3);
    auto n = shift_square(shift_square(three));

The type of `n` is `fixed_point<uint8_t, 0>` but its value does not
exceed 81. Hence, an unused integer bit has been allocated. It may be
possible to track more accurate limits in the same manner as the
bounded::integer library in order to improve the precision of types
returned by `shift_` functions. For this reason, details surrounding
these return types are omitted from this proposal.

(Note: bounded::integer is already supported by fixed-point library,
fp [\[4\]](https://github.com/mizvekov/fp).)

### Alternative Policies

The behavior of the types specialized from `fixed_point<>` represent
one sub-set of all potentially desirable behaviors. Alternative
characteristics include:

* different rounding strategies - other than truncation;
* overflow and underflow checks - possibly throwing exceptions;
* operator return type - adopting `shift_` or `promote` behavior and
* default-initialize to zero - not done by default.

One way to extend `fixed_point` to cover these alternatives would be
to add a third template parameter containing bit flags. The default
set of values would reflect `fixed_point` as it stands currently.

An example of a fixed-point proposal which takes a similar approach to
rounding and error cases can be found in N3352 [\[5\]](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3352.html).

## VII. References

1. Why Integer Coordinates?, http://www.pathengine.com/Contents/Overview/FundamentalConcepts/WhyIntegerCoordinates/page.php
2. N1169, Extensions to support embedded processors, <http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1169.pdf>
3. C++ bounded::integer library, <http://doublewise.net/c++/bounded/>
4. fp, C++14 Fixed Point Library, <https://github.com/mizvekov/fp>
5. N3352, C++ Binary Fixed-Point Arithmetic, <http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3352.html>
