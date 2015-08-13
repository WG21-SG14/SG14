**Document number**:  
**Date: 2015-08-12  
**Project**: Programming Language C++, Library Evolution WG, Evolution WG, SG14  
**Reply-to**: John McFarlane, john@mcfarlane.name

# Fixed-Point Real Numbers

## I. Motivation

The built-in floating-point types are extremely versatile in their ability to represent real numbers across a very wide range of values. However, the cost of this varsatility is a loss of accuracy and a decrease in accuracy across linear ranges of values.

A common application of floating-point types is to represent ranges of values which follow a linear distribution. Examples include absolute time and spatial measurements as well as unit values in the range [0, 1]. In such cases, having a variable exponent portion in the floating-point value has two disadvantages: 1) it takes precision away from the mantisa and 2) it ensures that the distribution of representable values is clusterd near zero. These characteristics can lead to undesirable behavior when modeling certain real-world phenomena.

In comparison, integral types devote as many bits as possible to precision and their values are distributed entirely linearly. Additionally, both signed and unsigned built-in integral types are far simpler to implement, leading lower hardware requirements. However, the trade off is that this range is entirely dictated by the number of bits and no fractional digits are possible.



## II. Related work:
* [N1169](http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1169.pdf) - Extensions to support embedded processors
* [N3352](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3352.html) - C++ Binary Fixed-Point Arithmetic
* [fpmath](https://code.google.com/p/fpmath/) - Fixed Point Math Library
* [boost::fixed_point](http://lists.boost.org/Archives/boost/2012/04/191987.php) - Prototype Boost Library