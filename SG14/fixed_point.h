#if ! defined(_SG14_FIXED_POINT)
#define _SG14_FIXED_POINT 1

#include <climits>
#include <cmath>
#include <cinttypes>
#include <limits>
#include <type_traits>
#include <ostream>
#include <istream>

#if defined(__clang__) || defined(__GNUG__)
// sg14::float_point only fully supports 64-bit types with the help of 128-bit ints.
// Clang and GCC use (__int128) and (unsigned __int128) for 128-bit ints.
#define _SG14_FIXED_POINT_128
#endif

namespace sg14
{
	namespace _impl
	{
		////////////////////////////////////////////////////////////////////////////////
		// sg14::_impl::get_int_t

		template <bool SIGNED, int NUM_BYTES>
		struct get_int;

		template <bool SIGNED, int NUM_BYTES>
		using get_int_t = typename get_int<SIGNED, NUM_BYTES>::type;

		// specializations
		template <> struct get_int<false, 1> { using type = std::uint8_t; };
		template <> struct get_int<true, 1> { using type = std::int8_t; };
		template <> struct get_int<false, 2> { using type = std::uint16_t; };
		template <> struct get_int<true, 2> { using type = std::int16_t; };
		template <> struct get_int<false, 4> { using type = std::uint32_t; };
		template <> struct get_int<true, 4> { using type = std::int32_t; };
		template <> struct get_int<false, 8> { using type = std::uint64_t; };
		template <> struct get_int<true, 8> { using type = std::int64_t; };
#if defined(_SG14_FIXED_POINT_128)
		template <> struct get_int<false, 16> { using type = unsigned __int128_t; };
		template <> struct get_int<true, 16> { using type = std::__int128_t; };
#endif

		////////////////////////////////////////////////////////////////////////////////
		// sg14::_impl::next_size_t

		// given an integral type, INT_TYPE, 
		// provides the integral type of the equivalent type with twice the size
		template <typename INT_TYPE>
		using next_size_t = typename get_int_t<std::is_signed<INT_TYPE>::value, sizeof(INT_TYPE) * 2>;

		////////////////////////////////////////////////////////////////////////////////
		// sg14::_impl::previous_size_t

		// given an integral type, INT_TYPE, 
		// provides the integral type of the equivalent type with half the size
		template <typename INT_TYPE>
		using previous_size_t = typename get_int_t<std::is_signed<INT_TYPE>::value, sizeof(INT_TYPE) / 2>;

		////////////////////////////////////////////////////////////////////////////////
		// sg14::_impl::is_integral

		template <typename T>
		struct is_integral;

		// possible exception to std::is_integral as fixed_point<bool, X> seems pointless
		template <>
		struct is_integral<bool> : std::false_type { };

#if defined(_SG14_FIXED_POINT_128)
		// addresses https://llvm.org/bugs/show_bug.cgi?id=23156
		template <>
		struct is_integral<__int128> : std::true_type { };

		template <>
		struct is_integral<unsigned __int128> : std::true_type { };
#endif

		template <typename T>
		struct is_integral : std::is_integral<T> { };

		////////////////////////////////////////////////////////////////////////////////
		// sg14::_impl::make_signed

		template <typename T>
		struct make_signed;

#if defined(_SG14_FIXED_POINT_64)
		template <>
		struct make_signed<__int128>
		{
			using type = __int128;
		};

		template <>
		struct make_signed<unsigned __int128>
		{
			using type = __int128;
		};
#endif

		template <typename T>
		struct make_signed : std::make_signed<T> { };

		////////////////////////////////////////////////////////////////////////////////
		// sg14::_impl::make_unsigned

		template <typename T>
		struct make_unsigned;

#if defined(_SG14_FIXED_POINT_64)
		template <>
		struct make_unsigned<__int128>
		{
			using type = unsigned __int128;
		};

		template <>
		struct make_unsigned<unsigned __int128>
		{
			using type = unsigned __int128;
		};
#endif

		template <typename T>
		struct make_unsigned : std::make_unsigned<T> { };

		////////////////////////////////////////////////////////////////////////////////
		// sg14::_impl::shift_left and sg14::_impl::shift_right

		// performs a shift operation by a fixed number of bits avoiding two pitfalls:
		// 1) shifting by a negative amount causes undefined behavior
		// 2) converting between integer types of different sizes can lose significant bits during shift right

		// EXPONENT >= 0 && sizeof(OUTPUT) <= sizeof(INPUT) && is_unsigned<INPUT>
		template <
			int EXPONENT,
			typename OUTPUT,
			typename INPUT,
			typename std::enable_if<
				EXPONENT >= 0 && sizeof(OUTPUT) <= sizeof(INPUT) && std::is_unsigned<INPUT>::value, 
				int>::type dummy = 0>
			constexpr OUTPUT shift_left(INPUT i) noexcept
		{
			static_assert(_impl::is_integral<INPUT>::value, "INPUT must be integral type");
			static_assert(_impl::is_integral<OUTPUT>::value, "OUTPUT must be integral type");

			return static_cast<OUTPUT>(i) << EXPONENT;
		}

		// EXPONENT >= 0 && sizeof(OUTPUT) <= sizeof(INPUT) && is_signed<INPUT>
		template <
			int EXPONENT,
			typename OUTPUT,
			typename INPUT,
			typename std::enable_if<
			EXPONENT >= 0 && sizeof(OUTPUT) <= sizeof(INPUT) && std::is_signed<INPUT>::value,
			int>::type dummy = 0>
			constexpr OUTPUT shift_left(INPUT i) noexcept
		{
			static_assert(_impl::is_integral<INPUT>::value, "INPUT must be integral type");
			static_assert(_impl::is_integral<OUTPUT>::value, "OUTPUT must be integral type");

			using signed_type = typename _impl::make_signed<OUTPUT>::type;

			return (i >= 0)
				? static_cast<OUTPUT>(i) << EXPONENT
				: static_cast<OUTPUT>(-(static_cast<signed_type>(-i) << EXPONENT));
		}

		template <int EXPONENT, typename OUTPUT, typename INPUT, typename std::enable_if<EXPONENT >= 0 && sizeof(OUTPUT) <= sizeof(INPUT), int>::type dummy = 0>
		constexpr OUTPUT shift_right(INPUT i) noexcept
		{
			static_assert(_impl::is_integral<INPUT>::value, "INPUT must be integral type");
			static_assert(_impl::is_integral<OUTPUT>::value, "OUTPUT must be integral type");
			return static_cast<OUTPUT>(i >> EXPONENT);
		}

		// EXPONENT >= 0 && sizeof(OUTPUT) > sizeof(INPUT) && is_unsigned<INPUT>
		template <
			int EXPONENT, 
			typename OUTPUT, 
			typename INPUT, 
			typename std::enable_if<
				EXPONENT >= 0 && ! (sizeof(OUTPUT) <= sizeof(INPUT)) && std::is_unsigned<INPUT>::value, 
				char>::type dummy = 0>
		constexpr OUTPUT shift_left(INPUT i) noexcept
		{
			static_assert(_impl::is_integral<INPUT>::value, "INPUT must be integral type");
			static_assert(_impl::is_integral<OUTPUT>::value, "OUTPUT must be integral type");

			return static_cast<OUTPUT>(i) << EXPONENT;
		}

		// EXPONENT >= 0 && sizeof(OUTPUT) > sizeof(INPUT) && is_signed<INPUT>
		template <
			int EXPONENT,
			typename OUTPUT,
			typename INPUT,
			typename std::enable_if<
			EXPONENT >= 0 && !(sizeof(OUTPUT) <= sizeof(INPUT)) && std::is_signed<INPUT>::value,
			char>::type dummy = 0>
			constexpr OUTPUT shift_left(INPUT i) noexcept
		{
			static_assert(_impl::is_integral<INPUT>::value, "INPUT must be integral type");
			static_assert(_impl::is_integral<OUTPUT>::value, "OUTPUT must be integral type");

			using signed_type = typename _impl::make_signed<OUTPUT>::type;

			return (i >= 0)
				? static_cast<OUTPUT>(i) << EXPONENT
				: static_cast<OUTPUT>(-(static_cast<signed_type>(-i) << EXPONENT));
		}

		template <int EXPONENT, typename OUTPUT, typename INPUT, typename std::enable_if<EXPONENT >= 0 && !(sizeof(OUTPUT) <= sizeof(INPUT)), char>::type dummy = 0>
		constexpr OUTPUT shift_right(INPUT i) noexcept
		{
			static_assert(_impl::is_integral<INPUT>::value, "INPUT must be integral type");
			static_assert(_impl::is_integral<OUTPUT>::value, "OUTPUT must be integral type");
			return static_cast<OUTPUT>(i) >> EXPONENT;
		}

		// pass bit-shifts with negative EXPONENTS to their complimentary positive-EXPONENT equivalents
		template <int EXPONENT, typename OUTPUT, typename INPUT, typename std::enable_if<(EXPONENT < 0), int>::type dummy = 0>
		constexpr OUTPUT shift_left(INPUT i) noexcept
		{
			return shift_right<-EXPONENT, OUTPUT, INPUT>(i);
		}

		template <int EXPONENT, typename OUTPUT, typename INPUT, typename std::enable_if<EXPONENT < 0, int>::type dummy = 0>
		constexpr OUTPUT shift_right(INPUT i) noexcept
		{
			return shift_left<-EXPONENT, OUTPUT, INPUT>(i);
		}

		////////////////////////////////////////////////////////////////////////////////
		// sg14::_impl::pow2

		// returns given power of 2
		template <typename S, int EXPONENT, typename std::enable_if<EXPONENT == 0, int>::type dummy = 0>
		constexpr S pow2() noexcept
		{
			static_assert(std::is_floating_point<S>::value, "S must be floating-point type");
			return 1;
		}

		template <typename S, int EXPONENT, typename std::enable_if<!(EXPONENT <= 0), int>::type dummy = 0>
		constexpr S pow2() noexcept
		{
			static_assert(std::is_floating_point<S>::value, "S must be floating-point type");
			return pow2<S, EXPONENT - 1>() * S(2);
		}

		template <typename S, int EXPONENT, typename std::enable_if<!(EXPONENT >= 0), int>::type dummy = 0>
		constexpr S pow2() noexcept
		{
			static_assert(std::is_floating_point<S>::value, "S must be floating-point type");
			return pow2<S, EXPONENT + 1>() * S(.5);
		}

		////////////////////////////////////////////////////////////////////////////////
		// sg14::_impl::capacity

		// has value that, given a value N, 
		// returns number of bits necessary to represent it in binary
		template <unsigned N> struct capacity;

		template <>
		struct capacity<0>
		{
			static constexpr int value = 0;
		};

		template <unsigned N>
		struct capacity
		{
			static constexpr int value = capacity<N / 2>::value + 1;
		};

		////////////////////////////////////////////////////////////////////////////////
		// sg14::sqrt helper functions

		template <typename REPR_TYPE>
		constexpr REPR_TYPE sqrt_bit(
			REPR_TYPE n,
			REPR_TYPE bit = REPR_TYPE(1) << (sizeof(REPR_TYPE) * CHAR_BIT - 2)) noexcept
		{
			return (bit > n) ? sqrt_bit<REPR_TYPE>(n, bit >> 2) : bit;
		}

		template <typename REPR_TYPE>
		constexpr REPR_TYPE sqrt_solve3(
			REPR_TYPE n,
			REPR_TYPE bit,
			REPR_TYPE result) noexcept
		{
			return bit
				   ? (n >= result + bit)
					 ? sqrt_solve3<REPR_TYPE>(n - (result + bit), bit >> 2, (result >> 1) + bit)
					 : sqrt_solve3<REPR_TYPE>(n, bit >> 2, result >> 1)
				   : result;
		}

		template <typename REPR_TYPE>
		constexpr REPR_TYPE sqrt_solve1(REPR_TYPE n) noexcept
		{
			return sqrt_solve3<REPR_TYPE>(n, sqrt_bit<REPR_TYPE>(n), 0);
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// sg14::fixed_point class template definition
	//
	// approximates a real number using a built-in integral type;
	// somewhat like a floating-point number but - with exponent determined at run-time

	template <typename REPR_TYPE, int EXPONENT = 0>
	class fixed_point
	{
	public:
		////////////////////////////////////////////////////////////////////////////////
		// types

		using repr_type = REPR_TYPE;

		////////////////////////////////////////////////////////////////////////////////
		// constants

		constexpr static int exponent = EXPONENT;
		constexpr static int digits = std::numeric_limits<repr_type>::digits;
		constexpr static int integer_digits = digits + exponent;
		constexpr static int fractional_digits = digits - integer_digits;

		////////////////////////////////////////////////////////////////////////////////
		// functions

	private:
		// constructor taking representation explicitly using operator++(int)-style trick
		constexpr fixed_point(repr_type repr, int) noexcept
			: _repr(repr)
		{
		}
	public:
		// default c'tor
		constexpr fixed_point() noexcept {}

		// c'tor taking an integer type
		template <typename S, typename std::enable_if<_impl::is_integral<S>::value, int>::type dummy = 0>
		constexpr fixed_point(S s) noexcept
			: _repr(int_to_repr(s))
		{
		}

		// c'tor taking a floating-point type
		template <typename S, typename std::enable_if<std::is_floating_point<S>::value, int>::type dummy = 0>
		constexpr fixed_point(S s) noexcept
			: _repr(float_to_repr(s))
		{
		}

		// c'tor taking a fixed-point type
		template <typename FROM_REPR_TYPE, int FROM_EXPONENT>
		constexpr fixed_point(fixed_point<FROM_REPR_TYPE, FROM_EXPONENT> const & rhs) noexcept
			: _repr(_impl::shift_right<(exponent - FROM_EXPONENT), repr_type>(rhs.data()))
		{
		}

		// returns value represented as a floating-point
		template <typename S, typename std::enable_if<_impl::is_integral<S>::value, int>::type dummy = 0>
		constexpr S get() const noexcept
		{
			return repr_to_int<S>(_repr);
		}

		// returns value represented as integral
		template <typename S, typename std::enable_if<std::is_floating_point<S>::value, int>::type dummy = 0>
		constexpr S get() const noexcept
		{
			return repr_to_float<S>(_repr);
		}

		// returns internal representation of value
		constexpr repr_type data() const noexcept
		{
			return _repr;
		}

		// creates an instance given the underlying representation value
		// TODO: constexpr with c++14?
		static constexpr fixed_point from_data(repr_type repr) noexcept
		{
			return fixed_point(repr, 0);
		}

		// comparison
		friend constexpr bool operator==(fixed_point const & lhs, fixed_point const & rhs) noexcept
		{
			return lhs._repr == rhs._repr;
		}
		friend constexpr bool operator!=(fixed_point const & lhs, fixed_point const & rhs) noexcept
		{
			return ! (lhs == rhs);
		}

		friend constexpr bool operator>(fixed_point const & lhs, fixed_point const & rhs) noexcept
		{
			return lhs._repr > rhs._repr;
		}
		friend constexpr bool operator<(fixed_point const & lhs, fixed_point const & rhs) noexcept
		{
			return lhs._repr < rhs._repr;
		}

		friend constexpr bool operator>=(fixed_point const & lhs, fixed_point const & rhs) noexcept
		{
			return lhs._repr >= rhs._repr;
		}
		friend constexpr bool operator<=(fixed_point const & lhs, fixed_point const & rhs) noexcept
		{
			return lhs._repr <= rhs._repr;
		}

		// arithmetic
		friend constexpr fixed_point operator-(fixed_point const & rhs) noexcept
		{
			static_assert(std::is_signed<repr_type>::value, "unary negation of unsigned value");

			return fixed_point(- rhs._repr, 0);
		}

		friend constexpr fixed_point operator+(fixed_point const & lhs, fixed_point const & rhs) noexcept
		{
			return fixed_point(lhs._repr + rhs._repr, 0);
		}
		friend constexpr fixed_point operator-(fixed_point const & lhs, fixed_point const & rhs) noexcept
		{
			return fixed_point(lhs._repr - rhs._repr, 0);
		}
		friend constexpr fixed_point operator*(fixed_point const & lhs, fixed_point const & rhs) noexcept
		{
			return fixed_point(_impl::shift_left<exponent, repr_type>(_impl::next_size_t<repr_type>(lhs._repr) * rhs._repr), 0);
		}
		friend constexpr fixed_point operator/(fixed_point const & lhs, fixed_point const & rhs) noexcept
		{
			return fixed_point(repr_type(_impl::shift_right<exponent, _impl::next_size_t<repr_type>>(lhs._repr) / rhs._repr), 0);
		}

		friend fixed_point & operator+=(fixed_point & lhs, fixed_point const & rhs) noexcept
		{
			return lhs = lhs + rhs;
		}
		friend fixed_point & operator-=(fixed_point & lhs, fixed_point const & rhs) noexcept
		{
			return lhs = lhs - rhs;
		}
		friend fixed_point & operator*=(fixed_point & lhs, fixed_point const & rhs) noexcept
		{
			return lhs = lhs * rhs;
		}
		friend fixed_point & operator/=(fixed_point & lhs, fixed_point const & rhs) noexcept
		{
			return lhs = lhs / rhs;
		}

	private:
		template <typename S, typename std::enable_if<std::is_floating_point<S>::value, int>::type dummy = 0>
		static constexpr S one() noexcept
		{
			return _impl::pow2<S, - exponent>();
		}

		template <typename S, typename std::enable_if<_impl::is_integral<S>::value, int>::type dummy = 0>
		static constexpr S one() noexcept
		{
			return int_to_repr<S>(1);
		}

		template <typename S>
		static constexpr S inverse_one() noexcept
		{
			static_assert(std::is_floating_point<S>::value, "S must be floating-point type");
			return _impl::pow2<S, exponent>();
		}

		template <typename S>
		static constexpr repr_type int_to_repr(S s) noexcept
		{
			static_assert(_impl::is_integral<S>::value, "S must be unsigned integral type");

			return _impl::shift_right<exponent, repr_type>(s);
		}

		template <typename S>
		static constexpr S repr_to_int(repr_type r) noexcept
		{
			static_assert(_impl::is_integral<S>::value, "S must be unsigned integral type");

			return _impl::shift_left<exponent, S>(r);
		}

		template <typename S>
		static constexpr repr_type float_to_repr(S s) noexcept
		{
			static_assert(std::is_floating_point<S>::value, "S must be floating-point type");
			return static_cast<repr_type>(s * one<S>());
		}

		template <typename S>
		static constexpr S repr_to_float(repr_type r) noexcept
		{
			static_assert(std::is_floating_point<S>::value, "S must be floating-point type");
			return S(r) * inverse_one<S>();
		}

		////////////////////////////////////////////////////////////////////////////////
		// variables

		repr_type _repr = 0;
	};

	////////////////////////////////////////////////////////////////////////////////
	// sg14::open_unit and sg14::closed_unit partial specializations of fixed_point

	// fixed-point type capable of storing values in the range [0, 1);
	// a bit more precise than closed_unit
	template <typename REPR_TYPE>
	using open_unit = fixed_point<REPR_TYPE, - static_cast<int>(sizeof(REPR_TYPE)) * CHAR_BIT>;

	// fixed-point type capable of storing values in the range [0, 1];
	// actually storing values in the range [0, 2);
	// a bit less precise than closed_unit
	template <typename REPR_TYPE>
	using closed_unit = fixed_point<
		typename std::enable_if<std::is_unsigned<REPR_TYPE>::value, REPR_TYPE>::type,
		1 - static_cast<int>(sizeof(REPR_TYPE)) * CHAR_BIT>;

	////////////////////////////////////////////////////////////////////////////////
	// sg14::fixed_point_promotion_t

	// given template parameters of a fixed_point specialization, 
	// yields alternative specialization with twice the fractional bits
	// and twice the integral/sign bits
	template <typename REPR_TYPE, int EXPONENT>
	using fixed_point_promotion_t = fixed_point<_impl::next_size_t<REPR_TYPE>, EXPONENT * 2>;

	////////////////////////////////////////////////////////////////////////////////
	// sg14::fixed_point_demotion_t

	// given template parameters of a fixed_point specialization, 
	// yields alternative specialization with half the fractional bits
	// and half the integral/sign bits (assuming EXPONENT is even)
	template <typename REPR_TYPE, int EXPONENT>
	using fixed_point_demotion_t = fixed_point<_impl::previous_size_t<REPR_TYPE>, EXPONENT / 2>;

	////////////////////////////////////////////////////////////////////////////////
	// sg14::fixed_point_by_integer_digits_t

	// yields a float_point with EXPONENT calculated such that 
	// fixed_point<REPR_TYPE, EXPONENT>::integer_bits == INTEGER_BITS
	template <typename REPR_TYPE, int INTEGER_BITS>
	using fixed_point_by_integer_digits_t = fixed_point<
		REPR_TYPE, 
		INTEGER_BITS + std::is_signed<REPR_TYPE>::value - sizeof(REPR_TYPE) * CHAR_BIT>;

	////////////////////////////////////////////////////////////////////////////////
	// sg14::lerp

	// linear interpolation between two fixed_point values
	// given floatint-point `t` for which result is `from` when t==0 and `to` when t==1
	template <typename REPR_TYPE, int EXPONENT, typename S, typename>
	constexpr fixed_point<REPR_TYPE, EXPONENT> lerp(
		fixed_point<REPR_TYPE, EXPONENT> from,
		fixed_point<REPR_TYPE, EXPONENT> to,
		S t)
	{
		using closed_unit = closed_unit<REPR_TYPE>;
		return lerp<REPR_TYPE, EXPONENT>(from, to, closed_unit(t));
	}

	template <typename REPR_TYPE, int EXPONENT>
	constexpr fixed_point<REPR_TYPE, EXPONENT> lerp(
		fixed_point<REPR_TYPE, EXPONENT> from,
		fixed_point<REPR_TYPE, EXPONENT> to,
		closed_unit<typename _impl::make_unsigned<REPR_TYPE>::type> t)
	{
		using fixed_point = fixed_point<REPR_TYPE, EXPONENT>;
		using repr_type = typename fixed_point::repr_type;
		using next_repr_type = typename _impl::next_size_t<repr_type>;
		using closed_unit = closed_unit<typename  _impl::make_unsigned<REPR_TYPE>::type>;

		return fixed_point::from_data(
			_impl::shift_left<closed_unit::exponent, repr_type>(
				(static_cast<next_repr_type>(from.data()) * (closed_unit(1).data() - t.data())) +
				(static_cast<next_repr_type>(to.data()) * t.data())));
	}

	////////////////////////////////////////////////////////////////////////////////
	// sg14::abs

	template <typename REPR_TYPE, int EXPONENT, typename std::enable_if<std::is_signed<REPR_TYPE>::value, int>::type dummy = 0>
	constexpr fixed_point<REPR_TYPE, EXPONENT> abs(fixed_point<REPR_TYPE, EXPONENT> const & x) noexcept
	{
		return (x >= fixed_point<REPR_TYPE, EXPONENT>(0)) ? x.data() : - x.data();
	}

	////////////////////////////////////////////////////////////////////////////////
	// sg14::sqrt

	// https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Binary_numeral_system_.28base_2.29
	// slow when calculated at run-time?
	template <typename REPR_TYPE, int EXPONENT>
	constexpr fixed_point<REPR_TYPE, EXPONENT> sqrt(fixed_point<REPR_TYPE, EXPONENT> const & x) noexcept
	{
		return fixed_point<REPR_TYPE, EXPONENT>::from_data(
			static_cast<REPR_TYPE>(_impl::sqrt_solve1(fixed_point_promotion_t<REPR_TYPE, EXPONENT>(x).data())));
	}

	////////////////////////////////////////////////////////////////////////////////
	// sg14::fixed_point streaming - (placeholder implementation)

	template <typename REPR_TYPE, int EXPONENT>
	::std::ostream & operator << (::std::ostream & out, fixed_point<REPR_TYPE, EXPONENT> const & fp)
	{
		return out << fp.template get<long double>();
	}

	template <typename REPR_TYPE, int EXPONENT>
	::std::istream & operator >> (::std::istream & in, fixed_point<REPR_TYPE, EXPONENT> & fp)
	{
		long double ld;
		in >> ld;
		fp = ld;
		return in;
	}
}

#endif	// defined(_SG14_FIXED_POINT)
