# The `fixed_point` Class Template

[John McFarlane](https://groups.google.com/a/isocpp.org/forum/#!profile/sg14/APn2wQdoie4ys78eOuHNF35KKYtO4LHy0d1z8jGXJ0cr5tzmqGbDbD7BFUzrqWrU7tJiBi_UnRqo)

## 1. Introduction

This document introduces one approach to representing fixed-point real numbers in C++11 using literal class template, `fixed_point`.

### 1.1. Background

It is developed as part of [SG14](https://groups.google.com/a/isocpp.org/forum/#!forum/sg14) which explores the game development and low-latency needs of C++ developers.

### 1.2. Resources

SG14 has  an [unofficial](https://groups.google.com/forum/#!forum/unofficial-real-time-cxx) and an [official](https://groups.google.com/a/isocpp.org/forum/#!forum/sg14) discussion group which contains [a thread](https://groups.google.com/a/isocpp.org/forum/#!topic/sg14/1w5eiJsyT2Q) circulating the original fixed_point idea. A formal [proposal](Proposals/Fixed_Point_Library_Proposal.md) is also in the works.

There is an [SG14 source code repository](https://github.com/WG21-SG14/SG14) including a header file, [fixed_point.h](https://github.com/WG21-SG14/SG14/blob/master/SG14/fixed_point.h), in which a proof of concept, `sg14::fixed_point`, is actively developed. Accompanying tests can be found in [fixed_point_test.cpp](https://github.com/WG21-SG14/SG14/blob/master/SG14_test/fixed_point_test.cpp).

Anyone with an interest in game and low-latency development in C++ is welcome to contribute to the discussion and the project.

## 2 Overview

The proof of concept of fixed_point is located in [fixed_point.h](https://github.com/WG21-SG14/SG14/blob/master/SG14/fixed_point.h). For brevity, the project namespace, `sg14` - as well as standard namespace, `std` - is assumed from here on:
```
using namespace sg14;
using namespace std;
```

The header is intended to be self-contained and should only require a reasonably compliant C++11 compiler. The code is generally tested against Clang 3.6, GCC 4.9 and VC2015.

### 2.1 Basic Usage

Fixed-point real numbers are represented using the following class template:
```
fixed_point<typename REPR_TYPE, int EXPONENT>
```
where:
* `REPR_TYPE` is a built-in integral type used to store the value and
* `EXPONENT` is an integer value governing the number of bits by which the internal value must be shifted in order to represent the real number. 

Note that if `REPR_TYPE` is signed, the `fixed_point` type is also signed.

#### 2.1.1 Converting to `fixed_point` Type

To declare a variable, `pi`, containing a signed real value made up of 3 integer bits and 12 fractional bits:
```
auto pi = fixed_point<int16_t, -12>(3.141592654);
```

You can construct an object from any integral, floating-point or other fixed-point type. Rounding is the same as for float-to-integer conversion. (That is to say, it has not been addressed at all!)

#### 2.1.2 Converting from `fixed_point` to Built-in Types

Object can be explicitly cast back to built-in types:
```
cout << static_cast<float>(pi);  // output: "3.14163"
```

#### 2.1.3 Made-To-Measure Types

Most likely, you care how many integral bits are stored. There are two ways of creating a fixed_point type from this value:

You can simply specify the amounts of everything - including sign bit:
```
auto opacity = make_fixed<8, 8, false>(255.999);  // 8:8 unsigned value
```

Alternatively you can specify the underlying type and the desired number of integer bits:
```
auto degrees = make_fixed_from_repr<int32_t, 9>(360);  // 9:22 signed value
```

#### 2.1.4 Common Aliases

For convenience, you can eschew the `fixed_point<>` notation in favor of pre-defined aliases in a more commonly recognized form. Many sources use I:F notation to describe the number of integral and fractional bits. 

To instantiate a 7:8 value, the following are equivalent:
```
auto m = fixed7_8_t(1.23);
auto m = fixed_point<int16_t, -8>(1.23);
```

The complete list of aliases is at the bottom of [fixed_point.h](https://github.com/WG21-SG14/SG14/blob/master/SG14/fixed_point.h).

#### 2.1.5 Arithmetic Operations

The intent is for `fixed_point` to behave like a built-in integral or floating-point type. To this end, some basic arithmetic operations have been implemented with more to follow.

Done so far:
* unary `-`
* binary `==`, `!=`, `<`, `>`, `<=`, `>=`, `+`, `-`, `*`, `/`, `+=`, `-=`, `*=`, `/=`

#### 2.1.6 Math Functions

So far, `sqrt` and `abs` are available:
```
cout << sqrt(abs(fixed3_4_t(-5)));  // output: "2.1875"
```

#### 2.1.7 `open_unit`, `closed_unit` and `lerp`

A likely application of fixed-point types is in the expression of values in unit intervals. Two partial specializations of `fixed_point`, tentatively named `open_unit<REPR_TYPE>` and `closed_unit<REPR_TYPE>` make it slightly easier to define fixed-point types which have as much precision as possible whilst being able to store values in the range [0,1) and [0,1] respectively.

Accompanying these aliases is the function, `lerp`:
```
cout << lerp(ufixed4_4_t(2.5), ufixed4_4_t(7.25), closed_unit<uint8_t>(0.6));  // output: 5.3125
```
### 2.2 Advanced Topics

#### 2.2.1 Literal Types and `constexpr`

A quick look at [fixed_point_test.cpp](https://github.com/WG21-SG14/SG14/blob/master/SG14_test/fixed_point_test.cpp) will make it apparent that `fixed_point<>` is a literal type; anything one can do with it at run-time can also be done at compile time. 

Some advantages to this are:

1. Rudimentary automated testing without the need for a framework - simply compile the test file as part of the project. In fact, the limited expressiveness of C++11 constexpr statements ensures that all tests are minimal in scope, in keeping with unit testing practice.
2. Initialization of instances using literals of built-in type is likely to mean that the object is constructed with virtually no overhead.
3. Further to this, relatively complex calculations can be performed at compile-time, e.g.:
   ```
   enum { TWICE_THE_SQUARE_ROOT_OF_FIFTY = ufixed16_16_t(2) * sqrt(ufixed16_16_t(50)) };
   cout << TWICE_THE_SQUARE_ROOT_OF_FIFTY;  // output: 14
   ```

Disadvantages:

1. Especially when limited to C++11, `constexpr` functions are taxing to design as no loops or variables are allowed.
2. No run-time checks such as `assert` or exceptions are possible, making run-time diagnostics difficult. It may turn out that contracts fix this situation. (TBD)

#### 2.2.2 Promotion

In the context of `fixed_point<>`, promotion refers to explicit conversion of one type to a compatible type of higher capacity. For example:
```
auto a = ufixed8_8_t(5.5);
auto b = promote(a);  // type of b is ufixed16_16_t
```
This is akin to casting from `float` to `double`. One can perform precision-critical calculations using the promoted type. Finally, the `demote` function template can be used to convert a value back to the original type.

#### 2.2.3 'Safe' Conversion

Usually, it isn't worth the extra complication and performance loss associated with converting to a larger type. In the floating-point case, this is handled automatically by the type's variable exponent. Not so for `fixed_point` types!

Consider this operation:
```
cout << ufixed4_4_t(10) * ufixed4_4_t(2);  // output: 4
```
Because `ufixed4_4_t` cannot store value 20, there is an overflow. The solution is to use `safe_multiply`:
```
cout << safe_multiply(ufixed4_4_t(10), ufixed4_4_t(2));  // output: 20
```
This function, and its partner, `safe_add`, ensure that the result is returned in a type of the same size which is suitable for holding the result. In the above case, `safe_multiply` returns a `ufixed8_0_t`.

With liberal use of the `auto` keyword, much work can be performed this way without the worry of losing significant digits:
```
template <typename REPR_TYPE, int EXPONENT>
auto constexpr dot_product(
	fixed_point<REPR_TYPE, EXPONENT> x1, fixed_point<REPR_TYPE, EXPONENT> y1,
	fixed_point<REPR_TYPE, EXPONENT> x2, fixed_point<REPR_TYPE, EXPONENT> y2)
{
	return safe_add(safe_multiply(x1, x2), safe_multiply(y1, y2));
}

cout << dot_product(ufixed4_4_t(10), ufixed4_4_t(0), ufixed4_4_t(5), ufixed4_4_t(5));  // output: 50
```

#### 2.2.4 Extreme Values of Exponent

In the previous example, the function template specialisation of `dot_product` returns a value of type, `fixed_point<uint8_t, 1>`. In other words, it can only express even numbers. It may seem unusual to have a fixed-point type with a negative number of fractional bits, but it is perfectly normal for floating-point types to represent very large numbers this way.

Equally, `fixed_point<uint8_t, -9>` can only represent values in the range [0, 0.5). Again, if that is what is called for, there's no reason not to allow this. Obviously, it makes conversion between different types a little more complicated but otherwise, is simpler that not allowing it.

## 3 Future API Work

Some rough notes on where to go next with the design of the API:

### 3.1 To Discuss

* better name for `safe_` functions
* 'number of integer digits' might make a better 2nd parameter to `fixed_point<>`.
* should object be default constructed to zero value?
* should the first template parameter default to int?
* fixed_point::data is named after the std::vector member function but is this the best choice here?
* Too many disparate ways to declare specializations of fixed_point? If so, what is a good subset?

### 3.2 To Do

* Operators
  * unary: `!`, `~`
  * binary: `%`, `<<`, `>>`, `<<=`, `>>=`, `&`, `|`, `^`, `&&`, `||`
  * pre and post: `++`, `--`
* many more overloads of cmath functions
* heterogenous safe_add, safe_multiply etc.
* safe_divide? inverse_t?
* standard traits and `std::numeric_limits`
* either remove `open_unit` and `closed_unit` or replace with `open_interval` and `closed_interval`
* consider removing `lerp` as it can probably be done as well using arithmetic operations
* streaming operators are placeholder and fall back on `long double` conversion. 
* better name for `fixed_point_by_integer_digits_t`

### Possible Additions

* an alias that creates a fixed_point specialization from a maximum, e.g.:
  * auto n = fixed_point_can_hold_t<int16_t, 4000>();  // type of n is fixed_point<uint16_t, -3>
* a routine for generating text representations in arbitrary bases.
* consider complimenting them with types that can store [-1,1] for use with trig functions
