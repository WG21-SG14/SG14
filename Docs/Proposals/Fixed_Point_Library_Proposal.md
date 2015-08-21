**Document number**: (draft)  
**Date**: 2015-08-12  
**Project**: Programming Language C++, Library Evolution WG, Evolution WG, SG14  
**Reply-to**: John McFarlane, [john@mcfarlane.name](mailto:john@mcfarlane.name)

# Fixed-Point Real Numbers

## I. Motivation

Floating-point types are highly versatile in their ability to 
represent real numbers across a very wide range of values. However, 
the cost of this versatility is a loss of precision and a variation 
in accuracy across linear ranges of values. In particular, absolute 
ranges of value with linear distribution - such as temporal and 
spatial coordinates and unit intervals - gain very little from being
represented with a floating point.

Conversely, integer types are ideal at representing values across a
linear range and are far more simple to deal with. However, they lack
an exponent and their radix point if fixed at zero. This means their
range is predetermined and they do not express fractional digits.

Fixed-point arithmetic is the middle-ground between integer and 
floating-point arithmetic. The exponent of a fixed-point type is 
determined at compile time. 

The fixed_point library is intended to make fixed-point arithmetic as
easy and effective as possible. It is intended to be used in 
situations where integer types and bit manipulation operations would 
normally be used.

## II. Impact On the Standard

This proposal is a pure library extension. It does not require 
changes to any standard classes, functions or headers. 

## III. Design Decisions

The class is designed to function as closely as possible to built-in
integral type with the major exception of disallowing implicit
conversion. Additionally, a number of accompanying definitions are
intended to address some of the specific difficulties of working with
fixed-point values.

### Class Template

Fixed-point numbers are specializations of `template <typename
REPR_TYPE, int EXPONENT> class fixed_point` where:

* `REPR_TYPE` is the underlying type used to store the value. A type
  which satisfies `is_integral` is an appropriate choice. Whether the
  number is signed is determined by whether `REPR_TYPE` is signed. 
  The default is `int`.

* `EXPONENT` is the equivalent of a floating-point exponent and
  shifts the stored value by the requisite number of bits necessary
  to produce the desired range. The default value depends on 
  `REPR_TYPE` and is whatever value divides capacity equally between 
  bits used to store fractional digits and bits used to store integer
  digits and sign.

Some examples specializations include an unsigned 16-bit type with 8 
integer digits and 8 fractional digits:

    fixed_point<uint16_t>

a signed 32-bit type which can represent half values:

    fixed_point<int32_t, -1>

an unsigned 8-bit type with 8 fractional digits that can be used to
store values in the range, [0, 1):

    fixed_point<uint8_t, -8>

and a signed value - typically in 32 bits - with half the bits
dedicated to fractional digits:

    fixed_point<>

### Conversion

Fixed-point numbers can be converted explicitly to and from built-in
arithmetic types. While effort is made to ensure that large enough
types are used to perform the calculations without losing significant
digits, no effort is made to avoid rounding errors. 

Whatever would happen when converting to and from an integer type
largely goes for `fixed_point` objects also. For example:

    fixed_point<uint8_t, -4>(.006) == fixed_point<uint8_t, -4>(0)

...equates to `true` and is considered an acceptable rounding error.

The priority here is preserving the highest-value digits of a value 
and avoiding any run-time performance penalties involved with 
non-trivial rounding strategies.

### Named Constructors

Fixed-point numbers are traditionally notated in a variety of ways.
The `EXPONENT` template parameter is a versatile way of expressing 
the full range of values a type may contain - including very large 
positive and negative values. However, it is not necessarily the most
intuitive choice for many common use cases.

A number of named constructors in the spirit of existing `make_X` 
functions are defined to make specialization of `fixed_point<>` 
more convenient.

To create a specialization with *I* integer digits, *F* fractional
digits and signed digit iff S, there is:

    make_fixed<I, F, S>

This alias automatically determines what underlying type is needed to
accommodate the desired amount of precisions so for example,

    make_fixed<8, 11, true>

is guaranteed to be a signed fixed-point type with 8 integer digits 
and at least 11 fractional digits. As there is no standard 20-bit
integer type, a `REPR_TYPE` of `int32_t` is used and an additional 12
bits are devoted to fractional digits: `fixed_point<int32_t, -23>`.

While the number of fractional digits is often the attribute of most
attention when discussing a fixed-point type, it is really the number
of integer digits that are - literally - most significant. In cases
where the underlying type and the number of integer digits are known,
one can use:

    make_fixed_from_repr<REPR_TYPE, I>

And when the greater of two fixed-point types is required, such as 
when comparing heterogeneous values or taking their maximum, use:

    make_fixed_from_pair<FIXED_POINT_1, FIXED_POINT_2>

### Arithmetic Operators

Any operators that might be applied to integer types can also be
applied to `fixed_point` specializations. Input parameters and return
value are all of the same type. (The possible exceptions to this are
the right hand parameter of bit shift operators.)

Because of this decision, arithmetic operators carry a risk of 
overflowing. For instance,

    make_fixed<4, 3>(15) * make_fixed<4, 3>(15)

returns `make_fixed<4, 3>(1)` because the input type does not have 
the capacity to represent the value, 225.

This presents a design choice between three alternative strategies:

1. ignore the problem and leave it to the user to avoid overflow;
2. promote the result to a larger type to ensure sufficient capacity
   exists to store all integer and fractional digits and
3. adjust the exponent upward to ensure that the top limit of the 
   type is sufficient to preserve the most significant digits at the 
   expense of the less significant digits.

For arithmetic operators, choice 1) is preferred because it most 
closely follows the behavior of integer types. Thus it should hide 
the fewest surprises from the most users. It does not produce a 
result of a larger type or different `EXPONENT` value. This makes it
far easier to manage in code where functions are written to expect
specific types.

Choices 2) and 3) are still of great value. However, they represent
different trade-offs and neither one is the best fit in all 
situations. For this reason, they are available via named functions.

### Type Promotion and Demotion Functions

Borrowing a term from the language feature which avoids integer 
overflow prior to certain operations, the `promote` function template
takes a `fixed_point<>` value and returns the same value represented 
by a larger `fixed_point<>` specialization.

For example,

    promote(fixed_point<int8_t, -2>(15.5))

is equivalent to

    fixed_point<int16_t, -4>(15.5)

while `demotion` reverses the process, returning a value of a smaller
type. The functions double and half, respectively, the number of 
integer and fractional digits available in the value.

The way `promotion` is used similar to the way high and low digits 
are preserved for precision-critical calculations by first converting 
values from `float` to `double`. To perform the calculations 
themselves, regular arithmetic operators are then used.

### Named Arithmetic Functions

The following named functions can be used as alternatives to 
arithmetic operators in situations where the aim is to avoid overflow
without falling back on higher-capacity types.

    safe_multiply(FIXED_POINT_1, FIXED_POINT_2)
    safe_add(FIXED_POINT_1, FIXED_POINT_2)
    safe_square(FIXED_POINT_1)

These functions return specializations which are guaranteed to 
accommodate the result of the operations. However, the capacity of
the return type is not increased so precision may be lost. For 
example:

    safe_square(fixed_point<uint8_t, -4>(15.9375))

returns `fixed_point<uint8_t, 0>(254)`. This result is far closer to
the correct value than the result returned by `operator *` - hence 
the 'safe_' prefix.

## IV. Technical Specification

### Header

All proposed additions to the library are contained in header,
`<fixed_point>`.

### Class template `fixed_point`

#### Header `<fixed_point>` synopsis

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
        using safe_multiply_result_t;
      template <typename LHS, typename RHS>
        safe_multiply_result_t<LHS, RHS>
          constexpr safe_multiply(const LHS & factor1, const RHS & factor2) noexcept;
    
      template <typename REPR_TYPE, int EXPONENT, unsigned N = 2>
        using safe_add_result_t;
      template <typename REPR_TYPE, int EXPONENT, typename ... TAIL>
        safe_add_result_t<REPR_TYPE, EXPONENT, sizeof...(TAIL) + 1>
          constexpr safe_add(fixed_point<REPR_TYPE, EXPONENT> const & addend1, TAIL const & ... addend_tail)
    
      template <typename FIXED_POINT>
        using safe_square_result_t;
      template <typename FIXED_POINT>
        safe_square_result_t<FIXED_POINT>
          constexpr safe_square(const FIXED_POINT & root) noexcept;
    }
    
## V. Future Issues

### Explicit Template Specialization of N1169 Types

TODO

### Relaxed Rules Surrounding Return Types of Arithmetic Operations

(An exception could be made in the case of heterogeneous inputs where
the resultant type becomes the more appropriate of the two input 
types as this would loosely emulate integer promotion. But for now,
this will not compile.)

### Library Support

Because it aims to provide an alternative to existing arithmetic 
types which are supported by the standard library, it is conceivable
that future proposals might specialize existing class templates and 
overload existing functions to that end.

### Keeping track of maximum values - like bounded to avoid unnecessary precision loss

### TODO: min, max, <cmath> etc.

But not numeric_limits, <type_traits>, right?

### Run-time Checks

Perhaps a type for `REPR_TYPE` could be chosen which behaves like an 
integer but catches overflow. In this case the concern of run-time 
checkcs could be separated from fixed-point arithmetic.

## V. Technical Specification

TODO

## VI. Related Work

* [N1169](http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1169.pdf) - Extensions to support embedded processors
* [N3352](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3352.html) - C++ Binary Fixed-Point Arithmetic
* [fpmath](https://code.google.com/p/fpmath/) - Fixed Point Math Library
* [boost::fixed_point](http://lists.boost.org/Archives/boost/2012/04/191987.php) - Prototype Boost Library
* [fp](https://github.com/mizvekov/fp) - C++14 Fixed Point Library
* [bounded](http://doublewise.net/c++/bounded/) - C++ bounded::integer library
