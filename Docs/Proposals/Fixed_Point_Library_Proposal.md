**Document number**: (draft)
**Date**: 2015-08-12  
**Project**: Programming Language C++, Library Evolution WG, Evolution WG, SG14  
**Reply-to**: John McFarlane, [john@mcfarlane.name](mailto:john@mcfarlane.name)

# Fixed-Point Real Numbers

## I. Motivation

Floating-point types are extremely versatile in their ability to 
represent real numbers across a very wide range of values. However, 
the cost of this versatility is a loss of accuracy and a variation 
in accuracy across linear ranges of values. In particular, absolute 
ranges of value with linear distribution - such as temporal and 
spatial coordinates and unit intervals - gain very little from being
represented with a floating point.

Conversely, integer types are ideal at representing values across a
linear range and are far more simple to deal with. However, they lack
an exponent and their radix point if fixed at zero. This means their
range is predetermined and they cannot express fractional digits.

Fixed-point arithmetic can be seen as a middle-ground between these
two approaches in which the radix point of a variable is determined
at compile time. The `fixed_point` literal class template is an
implementation of `fixed-point` intended to make it easier to perform
low-level fixed-point arithmetic in situations where integer types
and bit manipulation operations would normally be used.

## II. Impact On the Standard

This proposal is a pure library extension. It does not require 
changes to any standard classes, functions or headers. Ideally
however, it would add specializations to various type trait templates
such as `is_signed` as well as `numeric_limits` and `hash`. It would
also benefit from overloads to many of the functions found in the
<cmath> header.

## III. Design Decisions

The class is designed to function as closely as possible to built-in
integral type with the major exception of disallowing implicit
conversion. Additionally, a number of accompanying definitions are
intended to address some of the specific difficulties of working with
fixed-point values.

### Class Template

Fixed-point numbers are specializations of `template <typename
REPR_TYPE, int EXPONENT> class fixed_point;` where:
* `REPR_TYPE` is the underlying type used to store the value. (A type
  which satisfies `is_integral` is an appropriate choice.) Whether
  the number is signed is determined by whether REPR_TYPE is signed.
  The default is `int`.
* `EXPONENT` is the equivalent of a floating-point exponent and
  shifts the stored value by the requisite number of bits necessary
  to produce the desired range. The default is whatever value divides
  capacity equally between fractional digits and bits used to store
  integer digits and sign.

Some examples include an unsigned 16-bit type with 8 integer digits
and 8 fractional digits:

    fixed_point<uint16_t>

a signed 32-bit type which can represent numbers ending in .5:

    fixed_point<int32_t, -1>

an unsigned 8-bit type with 8 fractional digits that can be used to
store values in the range, [0, 0):

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

    static_cast<double>(fixed_point<uint8_t, 0>(.99)) == 0
    fixed_point<uint8_t, -1>(.5) == fixed_point<int8_t, -1>(0)

...both equate to `true`.

These are considered acceptable rounding errors. They can be avoided
by devoting more bits to fractional digits and - if necessary - using
a larger underlying integer type.

The priority here is preserving the highest-value digits and avoiding
the run-time performance penalties involved with non-trivial rounding
strategies.

### Named Constructors

Fixed-point numbers are notated in a variety of ways and while 
intuitive for concisely expressing the bit shift operation associate
with a specialization, the EXPONENT template parameter may not always
be the ideal way to assign digits to different roles.

A number of named constructors in the spirit of `make_pair` etc. 
exist to allow greater expression of `fixed_point` specializations.

To create a specialization with *I* integer digits, *F* fractional
digits and signed digit iff S, there is:

    make_fixed<I, F, S>

This alias automatically determines what underlying type is needed to
accommodate the desired amount of precisions so for example,

    make_fixed<12, 12, true>

is equivalent to fixed_point<int32_t, -19>. Note that 25 bits were
requested and so the type's capacity was rounded up to 32 bits with 
the additional 7 bits being devoted to fractional component.

While the number of fractional digits is often the attribute of most
interest when discussing a fixed-point type, it is really the number
of integer digits that are - literally - most significant. In cases
where the underlying type and the number of integer digits are known,
one can use:

    make_fixed_from_repr<REPR_TYPE, I>

And when the combination of two fixed-point types is required, such
as when comparing heterogeneous values or taking their maximum, use:

    make_fixed_from_pair<FIXED_POINT_1, FIXED_POINT_2>

This is indeed the alias used by the heterogeneous comparison 
operators discussed above.

### Arithmetic

Any operators that might be applied to integer types can also be
applied to `fixed_point` specializations. Initially at least, input
parameters and return value should all be the same type. (One 
possible exception to this are bit shift operators.)

The reasons for this limitation are twofold:

1. it avoids complicated promotion rules and manifold overloads and
2. it restricts the usage pattern of the type to a simplified subset
   of integer types.

It should be noted that unlike comparison operators, arithmetic
operators carry a risk of overflowing. For instance,

    fixed_point<uint8_t, -4>(15) * fixed_point<uint8_t, -4>(15)

returns a value of 1.

The only way to prevent this at the library level would be to return
a specialization with additional integer digits, i.e. 
`fixed_point<uint8_t, -1>`. This idea has been suggested elsewhere 
and has even implemented in at least one existing fixed-point
library. 

However, anyone who is using `fixed_point` as a convenient
replacement for raw integer types will be expecting this behavior. 
There are varying strategies for addressing overflow which are best
left to the user to choose, which involve some combination of 
increasing the size of the underlying type or incrementing the 
exponent.

An exception may be made in the case of heterogeneous inputs where 
the resultant type becomes the more appropriate of the two input 
types as this would loosely emulate integer promotion. But for now,
this will not compile. 

For alternative strategies to deal with overflow during arithmetic
operations, a collection of named functions are defined, as described
in the following section.

### Conversions Between Specializations

Certain arithmetic operations raise the possibility of overflow or
precision loss. The two techniques for avoiding these errors are
promotion and exponent shift respectively. Overflow is the most
serious as it affects the most significant digits of the value.

The following named functions can be used as alternatives to 
arithmetic operators in situations where overflow must be avoided at
all cost:

    safe_multiply(FIXED_POINT_1, FIXED_POINT_2)
	safe_add(FIXED_POINT_1, FIXED_POINT_2)
    safe_square(FIXED_POINT_1)

These functions return a specialization which is guaranteed to 
accommodate the result of the operations. However, the capacity of
the return type is not increased so precision may be lost. For 
example:

    safe_square(fixed_point<uint8_t, -4>(15.9375))

returns `fixed_point<uint8_t, 0>(225)`. This result is far closer to
the correct value than the result returned by `operator *` in the 
previous section and is considered 'safe'.

However, in cases where greater precision is required the type can be
'promoted' to the next largest type:

    auto f = promote(fixed_point<uint8_t, -4>(15.9375));

produces a variable, f, of type `fixed_point<uint16_t, -8>`. Now:

	f * f

returns `fixed_point<uint16_t, -8>(254.004)`.

## IV. Future Issues

### Explicit Template Specialization of N1169 Types

TODO

### Relaxed Rules Surrounding Return Types of Arithmetic Operations

(As discussed briefly above)

### Library Support

TODO: min, max, numeric_limits, <type_traits>, <cmath> etc.

## V. Technical Specification

TODO

## VI. Related Work

* [N1169](http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1169.pdf) - Extensions to support embedded processors
* [N3352](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3352.html) - C++ Binary Fixed-Point Arithmetic
* [fpmath](https://code.google.com/p/fpmath/) - Fixed Point Math Library
* [boost::fixed_point](http://lists.boost.org/Archives/boost/2012/04/191987.php) - Prototype Boost Library
* [fp](https://github.com/mizvekov/fp) - C++14 Fixed Point Library