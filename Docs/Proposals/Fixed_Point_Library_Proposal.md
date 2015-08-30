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

* `REPR_TYPE` implies the underlying type used to store the value. A
  type which satisfies `is_integral` is an appropriate choice. The
  size of the resulting type will be `sizeof(REPR_TYPE)` and it will
  be signed iff `is_signed<REPR_TYPE>::value`. The default is `int`.

* `EXPONENT` is the equivalent of a floating-point exponent and
  shifts the stored value by the requisite number of bits necessary
  to produce the desired range. The default value depends on
  `REPR_TYPE` and is whatever value divides capacity equally between
  bits used to store fractional digits and bits used to store integer
  digits and sign.

Some example specializations include an unsigned 16-bit type with 8
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
    safe_sqrt(FIXED_POINT_1)

These functions return specializations which are guaranteed to
accommodate the result of the operations. However, the capacity of
the return type is not increased so precision may be lost. For
example:

    safe_square(fixed_point<uint8_t, -4>(15.9375))

returns `fixed_point<uint8_t, 0>(254)`. This result is far closer to
the correct value than the result returned by `operator *` - hence
the 'safe_' prefix.

### Overflow and Underflow

The major disadvantage of `safe_` functions preserving the more
significant digits of their input values is that less significant
digits are sacrificed. In the extreme case of underflow, all the bits
are lost and the value becomes zero.

For example,

    safe_square(fixed_pointuint8_t, 0>(15))

causes the object to be flushed to zero.

While often not as serious as overflow, underflow can cause knock-on
effects, such as when a flushed value is subsequently used as a
divisor.

There is a strong desire to catch situations where a `fixed_point`
value transitions to a flushed state to causes any bits to overflow.
However, it is unacceptable that this should necessarily require a
run-time check. Additionally it is not net established by which
method an error should be flagged.

For this reason, it is left to the user to avoid flush events and for
such events to result in undefined behavior. This is not an ideal
solution but is necessary to ensure that future extension to the API
involving the option of run-time checks has enough freedom to
implement a satisfactory solution.

See the 'Future Issues' section below for discussion on possible
future strategies for catching overflow and underflow events.

### Alternative Specializations

The `REPR_TYPE` template parameter is not necessarily the type of the
member variable used to store the value. If there is a more
appropriate choice, the option to take it is reserved.

For example, it is conceivable that explicit template specializations
could be defined which exploit the language-level extensions to the C
type system outlined in proposal N1169[1](http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1169.pdf).
It defines built-in fixed-point types which might improve performance
on target systems with suitably-equipped processors.

If reasonably possible, the design of the `fixed_point` template
should leave open the option to exploit such system-specific
features.

### Example

The following example calculates the magnitude of a 3-dimensional vector.

    template <typename FP>
    constexpr auto magnitude(FP const & x, FP const & y, FP const & z)
    -> decltype(safe_sqrt(safe_add(safe_square(x), safe_square(y), safe_square(z))))
    {
        return safe_sqrt(safe_add(safe_square(x), safe_square(y), safe_square(z)));
    }

Calling the above function as follows

    static_cast<double>(magnitude(
        fixed_point<uint16_t, -12>(1),
        fixed_point<uint16_t, -12>(4),
        fixed_point<uint16_t, -12>(9)));

returns the value, 9.890625.

## IV. Technical Specification

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
        using safe_multiply_result_t;
      template <typename LHS, typename RHS>
        safe_multiply_result_t<LHS, RHS>
          constexpr safe_multiply(const LHS & factor1, const RHS & factor2) noexcept;

      template <typename REPR_TYPE, int EXPONENT, unsigned N = 2>
        using safe_add_result_t;
      template <typename REPR_TYPE, int EXPONENT, typename ... TAIL>
        safe_add_result_t<REPR_TYPE, EXPONENT, sizeof...(TAIL) + 1>
          constexpr safe_add(fixed_point<REPR_TYPE, EXPONENT> const & addend1, TAIL const & ... addend_tail)

      template <typename REPR_TYPE, int EXPONENT, unsigned N = 2>
        using safe_subtract_result_t;
      template <typename REPR_TYPE, int EXPONENT, typename ... TAIL>
        safe_subtract_result_t<REPR_TYPE, EXPONENT, sizeof...(TAIL) + 1>
          constexpr safe_subtract(fixed_point<REPR_TYPE, EXPONENT> const & addend1, TAIL const & ... addend_tail)

      template <typename FIXED_POINT>
        using safe_square_result_t;
      template <typename FIXED_POINT>
        safe_square_result_t<FIXED_POINT>
          constexpr safe_square(const FIXED_POINT & root) noexcept;

      template <typename FIXED_POINT>
        using safe_sqrt_result_t;
      template <typename FIXED_POINT>
        safe_sqrt_result_t<FIXED_POINT>
          constexpr safe_sqrt(const FIXED_POINT & root) noexcept;
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

## V. Future Issues

### Explicit Specializations

As suggested above, the `fixed_point` class template might be
explicitly specialized to take advantage of language extensions and
non-standard built-in types. However, it is not clear that this is a
wise decision. In particular, it leaves the return type of such
functions as `fixed_point<>::data()` unclear.

It may be wiser to say that a user may explicitly specify a type other
than one which satisfies `is_integral` in order to invoke a custom
specialization, e.g.:

    fixed_point<signed short _Fract>

### Relaxed Rules Surrounding Return Types of Arithmetic Operations

Currently, is is not possible to use a binary operator with
non-matching input types. This leaves open the future possibility of
allowing such input patterns and returning the result in a type which
usually encompasses both input types.

However, it is not always possible to produce a type which covers the
capabilities of two other types without increasing the size of the
resultant type beyond that of both inputs. For example the most
concise type which can represent the values of both `int8_t` and
`uint16_t` is `int32_t`. In such a case, it may be preferable to
return a value based on `int16_t` and lose the least significant bit
of precision from the `uint16_t` input.

This dilemma is one reason to defer a decision.

### Library Support

Because it aims to provide an alternative to existing arithmetic types
which are supported by the standard library, it is conceivable that a
future proposal might specialize existing class templates and overload
existing functions.

Possible candidates for overloading include the functions defined in
<cmath> and a templated specialization of `numeric_limits`. A type
trait, `is_fixed_point`, would also be useful.

While `fixed_point` is intended to provide drop-in replacements to
existing built-ins, it may be preferable to deviate slightly from the
behavior of certain standard functions. For example, overloads of
functions from <cmath> will be considerably less concise, efficient
and versatile if they obey rules surrounding error cases. In
particular, the guarantee of setting `errno` in the case of an error
prevents a function from being defined as pure. This highlights a
wider debate surrounding the adoption of functional techniques and
compile-time computation that is beyond the scope of this document.

### Bounded Integers

The bounded::integer library[2](http://doublewise.net/c++/bounded/)
exemplifies the benefits of keeping track of ranges of values in
arithmetic types at compile time.

To a limited extent the `safe_` functions defined here also keep track
of - and modify - the limits of values. However, a combination of
techniques is capable of producing superior results.

For instance, consider the following expression:

    make_fixed<2, 6, false> three(3);
    auto n = safe_multiply((safe_multiply(three, three), safe_multiply(three, three));

The type of `n` is `fixed_point<uint8_t, 0>` but its value does not
exceed 81. Hence, an unused integer bit has been allocated. It may be
possible to track more accurate limits in the same manner as the
bounded::integer library in order to improve the precision of types
returned by `safe_` functions. For this reason, details surrounding
these return types are omitted from this proposal.

(Note: bounded::integer is already supported by fixed-point library,
fp[3](https://github.com/mizvekov/fp).)

### Alternative Policies

The behavior of the types specialized from `fixed_point<>` represents
one sub-set of all potentially desirable behaviors. Alternative
characteristics include:

* different rounding strategies - other than truncation
* overflow and underflow checks - possibly throwing exceptions
* operator return type - adopting `safe_` function behavior or even
  promotion
* default-initialize to zero - not done by default

One way to extend `fixed_point` to cover these alternatives would be
to add a third template parameter containing bit flags. The default
set of values would reflect `fixed_point` as it stands currently.

An example of a fixed-point proposal which takes a similar approach to
rounding and error cases can be found in N3352[4]

 similar apprach is taken by

## VI. References

1. N1169, Extensions to support embedded processors, [http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1169.pdf]
2. C++ bounded::integer library, [http://doublewise.net/c++/bounded/]
3. fp, C++14 Fixed Point Library, [https://github.com/mizvekov/fp]
4. N3352, C++ Binary Fixed-Point Arithmetic, [http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3352.html]
