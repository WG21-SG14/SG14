#include "fixed_point.h"

#include <cassert>

using namespace sg14;

namespace sg14_test
{
	void fixed_point_test()
	{
		using namespace std;

		////////////////////////////////////////////////////////////////////////////////
		// copy assignment

		// from fixed_point
		auto rhs = fixed_point<>(123.456);
		auto lhs = rhs;
		assert(lhs == fixed_point<>(123.456));

		// from floating-point type
		lhs = 234.567;
		assert(static_cast<double>(lhs) == 234.56698608398438);

		// from integer
		lhs = 543;
		assert(static_cast<int>(lhs) == 543);

		// from alternative specialization
		lhs = fixed_point<uint8_t>(87.65);
		assert(static_cast<fixed_point<uint8_t>>(lhs) == fixed_point<uint8_t>(87.65));

		////////////////////////////////////////////////////////////////////////////////
		// Tests of Examples in Proposal 

		// Class Template

		static_assert(fixed_point<uint16_t>::integer_digits == 8, "Incorrect information in proposal section, Class Template");
		static_assert(fixed_point<uint16_t>::fractional_digits == 8, "Incorrect information in proposal section, Class Template");

		static_assert(static_cast<float>(fixed_point<int32_t, -1>(10.5)) == 10.5, "Incorrect information in proposal section, Class Template");

		static_assert(static_cast<float>(fixed_point<uint8_t, -8>(0)) == 0, "Incorrect information in proposal section, Class Template");
		static_assert(static_cast<float>(fixed_point<uint8_t, -8>(.999999)) < 1, "Incorrect information in proposal section, Class Template");
		static_assert(static_cast<float>(fixed_point<uint8_t, -8>(.999999)) > .99, "Incorrect information in proposal section, Class Template");
		
		static_assert(fixed_point<>::fractional_digits == _impl::num_bits<int>() / 2, "Incorrect information in proposal section, Class Template");

		// Conversion

		auto conversion_lhs = fixed_point<uint8_t, -4>(.006);
		auto conversion_rhs = fixed_point<uint8_t, -4>(0);
		static_assert(is_same<decltype(conversion_lhs), decltype(conversion_rhs)>::value, "Incorrect information in proposal section, Conversion");
		assert(conversion_lhs == conversion_rhs);

		// Names Constructors

		static_assert(is_same<make_fixed<8, 11, true>, fixed_point<int32_t, -23>>::value, "Incorrect information in proposal section, Named Constructors");

		// Arithmetic Operators

		auto arithmetic_op = make_fixed<4, 3>(15) * make_fixed<4, 3>(15);
		static_assert(is_same<decltype(arithmetic_op), make_fixed<4, 3>>::value, "Incorrect information in proposal section, Arithmetic Operators");
		assert(static_cast<int>(arithmetic_op) == 1);

		// Type Promotion and Demotion Functions
		auto type_promotion = promote(fixed_point<int8_t, -2>(15.5));
		static_assert(is_same<decltype(type_promotion), fixed_point<int16_t, -4>>::value, "Incorrect information in proposal section, Type Promotion and Demotion Functions");
		assert(static_cast<float>(type_promotion) == 15.5);

		// Named Arithmetic Functions
		auto sq = safe_multiply(fixed_point<uint8_t, -4>(15.9375), fixed_point<uint8_t, -4>(15.9375));  // TODO: safe_square
		assert(static_cast<double>(sq) == 254);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// sg14::_impl

////////////////////////////////////////////////////////////////////////////////
// sg14::_impl::is_integral

static_assert(_impl::is_integral<int>(), "sg14::_impl::is_integral test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::_impl::shift_left/right positive RHS

static_assert(_impl::shift_left<8, std::uint16_t>((std::uint16_t)0x1234) == 0x3400, "sg14::_impl::shift_left test failed");
static_assert(_impl::shift_left<8, std::uint16_t>((std::uint8_t)0x1234) == 0x3400, "sg14::_impl::shift_left test failed");
static_assert(_impl::shift_left<8, std::uint8_t>((std::uint16_t)0x1234) == 0x0, "sg14::_impl::shift_left test failed");
static_assert(_impl::shift_left<8, std::int16_t>(-123) == -31488, "sg14::_impl::shift_left test failed");

static_assert(_impl::shift_right<8, std::uint16_t>((std::uint16_t)0x1234) == 0x12, "sg14::_impl::shift_right test failed");
static_assert(_impl::shift_right<8, std::uint16_t>((std::uint8_t)0x1234) == 0x0, "sg14::_impl::shift_right test failed");
static_assert(_impl::shift_right<8, std::uint8_t>((std::uint16_t)0x1234) == 0x12, "sg14::_impl::shift_right test failed");
static_assert(_impl::shift_right<8, std::int16_t>(-31488) == -123, "sg14::_impl::shift_right test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::_impl::shift_left/right negative RHS

static_assert(_impl::shift_right<-8, std::uint16_t>((std::uint16_t)0x1234) == 0x3400, "sg14::_impl::shift_right test failed");
static_assert(_impl::shift_right<-8, std::uint16_t>((std::uint8_t)0x1234) == 0x3400, "sg14::_impl::shift_right test failed");
static_assert(_impl::shift_right<-8, std::uint8_t>((std::uint16_t)0x1234) == 0x0, "sg14::_impl::shift_right test failed");
static_assert(_impl::shift_right<-8, std::int16_t>(-123) == -31488, "sg14::_impl::shift_right test failed");

static_assert(_impl::shift_left<-8, std::uint16_t>((std::uint16_t)0x1234) == 0x12, "sg14::_impl::shift_left test failed");
static_assert(_impl::shift_left<-8, std::uint16_t>((std::uint8_t)0x1234) == 0x0, "sg14::_impl::shift_left test failed");
static_assert(_impl::shift_left<-8, std::uint8_t>((std::uint16_t)0x1234) == 0x12, "sg14::_impl::shift_left test failed");
static_assert(_impl::shift_left<-8, std::int16_t>(-31488) == -123, "sg14::_impl::shift_left test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::_impl::pow2

static_assert(_impl::pow2<float, 0>() == 1, "sg14::_impl::pow2 test failed");
static_assert(_impl::pow2<double, -1>() == .5, "sg14::_impl::pow2 test failed");
static_assert(_impl::pow2<long double, 1>() == 2, "sg14::_impl::pow2 test failed");
static_assert(_impl::pow2<float, -3>() == .125, "sg14::_impl::pow2 test failed");
static_assert(_impl::pow2<double, 7>() == 128, "sg14::_impl::pow2 test failed");
static_assert(_impl::pow2<long double, 10>() == 1024, "sg14::_impl::pow2 test failed");
static_assert(_impl::pow2<float, 20>() == 1048576, "sg14::_impl::pow2 test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::_impl::capacity

static_assert(_impl::capacity<0>::value == 0, "sg14::_impl::capacity test failed");
static_assert(_impl::capacity<1>::value == 1, "sg14::_impl::capacity test failed");
static_assert(_impl::capacity<2>::value == 2, "sg14::_impl::capacity test failed");
static_assert(_impl::capacity<3>::value == 2, "sg14::_impl::capacity test failed");
static_assert(_impl::capacity<4>::value == 3, "sg14::_impl::capacity test failed");
static_assert(_impl::capacity<7>::value == 3, "sg14::_impl::capacity test failed");
static_assert(_impl::capacity<8>::value == 4, "sg14::_impl::capacity test failed");
static_assert(_impl::capacity<15>::value == 4, "sg14::_impl::capacity test failed");
static_assert(_impl::capacity<16>::value == 5, "sg14::_impl::capacity test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::_impl::necessary_repr_t

static_assert(std::is_same<_impl::necessary_repr_t<1, false>, std::uint8_t>::value, "sg14::_impl::necessary_repr_t");
static_assert(std::is_same<_impl::necessary_repr_t<1, true>, std::int8_t>::value, "sg14::_impl::necessary_repr_t");
static_assert(std::is_same<_impl::necessary_repr_t<8, false>, std::uint8_t>::value, "sg14::_impl::necessary_repr_t");
static_assert(std::is_same<_impl::necessary_repr_t<8, true>, std::int8_t>::value, "sg14::_impl::necessary_repr_t");

static_assert(std::is_same<_impl::necessary_repr_t<9, false>, std::uint16_t>::value, "sg14::_impl::necessary_repr_t");
static_assert(std::is_same<_impl::necessary_repr_t<9, true>, std::int16_t>::value, "sg14::_impl::necessary_repr_t");
static_assert(std::is_same<_impl::necessary_repr_t<16, false>, std::uint16_t>::value, "sg14::_impl::necessary_repr_t");
static_assert(std::is_same<_impl::necessary_repr_t<16, true>, std::int16_t>::value, "sg14::_impl::necessary_repr_t");

static_assert(std::is_same<_impl::necessary_repr_t<17, false>, std::uint32_t>::value, "sg14::_impl::necessary_repr_t");
static_assert(std::is_same<_impl::necessary_repr_t<17, true>, std::int32_t>::value, "sg14::_impl::necessary_repr_t");
static_assert(std::is_same<_impl::necessary_repr_t<32, false>, std::uint32_t>::value, "sg14::_impl::necessary_repr_t");
static_assert(std::is_same<_impl::necessary_repr_t<32, true>, std::int32_t>::value, "sg14::_impl::necessary_repr_t");

static_assert(std::is_same<_impl::necessary_repr_t<33, false>, std::uint64_t>::value, "sg14::_impl::necessary_repr_t");
static_assert(std::is_same<_impl::necessary_repr_t<33, true>, std::int64_t>::value, "sg14::_impl::necessary_repr_t");
static_assert(std::is_same<_impl::necessary_repr_t<64, false>, std::uint64_t>::value, "sg14::_impl::necessary_repr_t");
static_assert(std::is_same<_impl::necessary_repr_t<64, true>, std::int64_t>::value, "sg14::_impl::necessary_repr_t");

#if defined(_SG14_FIXED_POINT_128)
static_assert(std::is_same<_impl::necessary_repr_t<65, false>, unsigned __int128>::value, "sg14::_impl::necessary_repr_t");
static_assert(std::is_same<_impl::necessary_repr_t<65, true>, __int128>::value, "sg14::_impl::necessary_repr_t");
static_assert(std::is_same<_impl::necessary_repr_t<128, false>, unsigned __int128>::value, "sg14::_impl::necessary_repr_t");
static_assert(std::is_same<_impl::necessary_repr_t<128, true>, __int128>::value, "sg14::_impl::necessary_repr_t");
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// sg14::fixed_point

////////////////////////////////////////////////////////////////////////////////
// default second template parameter

static_assert(std::is_same<fixed_point<std::int8_t>, fixed_point<std::int8_t, -4>>::value, "sg14::fixed_point test failed");
static_assert(std::is_same<fixed_point<std::uint8_t>, fixed_point<std::uint8_t, -4>>::value, "sg14::fixed_point test failed");
static_assert(std::is_same<fixed_point<std::int16_t>, fixed_point<std::int16_t, -8>>::value, "sg14::fixed_point test failed");
static_assert(std::is_same<fixed_point<std::uint16_t>, fixed_point<std::uint16_t, -8>>::value, "sg14::fixed_point test failed");
static_assert(std::is_same<fixed_point<std::int32_t>, fixed_point<std::int32_t, -16>>::value, "sg14::fixed_point test failed");
static_assert(std::is_same<fixed_point<std::uint32_t>, fixed_point<std::uint32_t, -16>>::value, "sg14::fixed_point test failed");
static_assert(std::is_same<fixed_point<std::int64_t>, fixed_point<std::int64_t, -32>>::value, "sg14::fixed_point test failed");
static_assert(std::is_same<fixed_point<std::uint64_t>, fixed_point<std::uint64_t, -32>>::value, "sg14::fixed_point test failed");

////////////////////////////////////////////////////////////////////////////////
// default first template parameter

static_assert(std::is_same<fixed_point<int, _impl::num_bits<int>() / -2>, fixed_point<>>::value, "sg14::fixed_point test failed");

////////////////////////////////////////////////////////////////////////////////
// conversion

// exponent == 0
static_assert(static_cast<float>(ufixed8_0_t(12.34f)) == 12.f, "sg14::fixed_point test failed");
static_assert(static_cast<double>(ufixed16_0_t(12.34f)) == 12.f, "sg14::fixed_point test failed");
static_assert(static_cast<long double>(ufixed32_0_t(12.34f)) == 12.f, "sg14::fixed_point test failed");
static_assert(static_cast<float>(ufixed64_0_t(12.34f)) == 12.f, "sg14::fixed_point test failed");

static_assert(static_cast<double>(fixed7_0_t(-12.34f)) == -12.f, "sg14::fixed_point test failed");
static_assert(static_cast<long double>(fixed15_0_t(-12.34f)) == -12.f, "sg14::fixed_point test failed");
static_assert(static_cast<float>(fixed31_0_t(-12.34f)) == -12.f, "sg14::fixed_point test failed");
static_assert(static_cast<double>(fixed63_0_t(-12.34f)) == -12.f, "sg14::fixed_point test failed");

// exponent = -1
static_assert(static_cast<float>(fixed_point<std::uint8_t, -1>(127.5)) == 127.5, "sg14::fixed_point test failed");

static_assert(static_cast<float>(fixed_point<std::int8_t, -1>(63.5)) == 63.5, "sg14::fixed_point test failed");
static_assert(static_cast<float>(fixed_point<std::int8_t, -1>(-63.5)) == -63.5, "sg14::fixed_point test failed");

// exponent == -7
static_assert(static_cast<float>(fixed_point<std::uint8_t, -7>(.125f)) == .125f, "sg14::fixed_point test failed");
static_assert(static_cast<float>(fixed_point<std::uint16_t, -8>(232.125f)) == 232.125f, "sg14::fixed_point test failed");
static_assert(static_cast<float>(fixed_point<std::uint32_t, -7>(232.125f)) == 232.125f, "sg14::fixed_point test failed");
static_assert(static_cast<float>(fixed_point<std::uint64_t, -7>(232.125f)) == 232.125f, "sg14::fixed_point test failed");

static_assert(static_cast<float>(fixed_point<std::int8_t, -7>(.125f)) == .125f, "sg14::fixed_point test failed");
static_assert(static_cast<float>(fixed_point<std::int16_t, -7>(123.125f)) == 123.125f, "sg14::fixed_point test failed");
static_assert(static_cast<float>(fixed_point<std::int32_t, -7>(123.125f)) == 123.125f, "sg14::fixed_point test failed");
static_assert(static_cast<float>(fixed_point<std::int64_t, -7>(123.125f)) == 123.125f, "sg14::fixed_point test failed");

static_assert(static_cast<double>(fixed_point<std::uint8_t, -7>(.125f)) == .125f, "sg14::fixed_point test failed");
static_assert(static_cast<long double>(fixed_point<std::uint16_t, -8>(232.125f)) == 232.125f, "sg14::fixed_point test failed");
static_assert(static_cast<double>(fixed_point<std::uint32_t, -7>(232.125f)) == 232.125f, "sg14::fixed_point test failed");
static_assert(static_cast<long double>(fixed_point<std::uint64_t, -7>(232.125f)) == 232.125f, "sg14::fixed_point test failed");

static_assert(static_cast<long double>(fixed_point<std::int8_t, -7>(1)) != 1.f, "sg14::fixed_point test failed");
static_assert(static_cast<float>(fixed_point<std::int8_t, -7>(.5)) == .5f, "sg14::fixed_point test failed");
static_assert(static_cast<long double>(fixed_point<std::int8_t, -7>(.125f)) == .125f, "sg14::fixed_point test failed");
static_assert(static_cast<int>(fixed_point<std::int16_t, -7>(123.125f)) == 123, "sg14::fixed_point test failed");
static_assert(static_cast<long double>(fixed_point<std::int32_t, -7>(123.125f)) == 123.125f, "sg14::fixed_point test failed");
static_assert(static_cast<float>(fixed_point<std::int64_t, -7>(123.125l)) == 123.125f, "sg14::fixed_point test failed");

// exponent == 16
static_assert(static_cast<float>(fixed_point<std::uint8_t, 16>(65536)) == 65536.f, "sg14::fixed_point test failed");
static_assert(static_cast<int>(fixed_point<std::uint16_t, 16>(6553.)) == 0, "sg14::fixed_point test failed");
static_assert(static_cast<double>(fixed_point<std::uint32_t, 16>(4294967296l)) == 4294967296.f, "sg14::fixed_point test failed");
static_assert(static_cast<double>(fixed_point<std::uint64_t, 16>(1125895611875328l)) == 1125895611875328l, "sg14::fixed_point test failed");

static_assert(static_cast<float>(fixed_point<std::int8_t, 16>(-65536)) == -65536.f, "sg14::fixed_point test failed");
static_assert(static_cast<int>(fixed_point<std::int16_t, 16>(-6553.)) == 0, "sg14::fixed_point test failed");
static_assert(static_cast<double>(fixed_point<std::int32_t, 16>(-4294967296l)) == -4294967296.f, "sg14::fixed_point test failed");
static_assert(static_cast<double>(fixed_point<std::int64_t, 16>(-1125895611875328l)) == -1125895611875328l, "sg14::fixed_point test failed");

// exponent = 1
static_assert(static_cast<int>(fixed_point<std::uint8_t, 1>(510)) == 510, "sg14::fixed_point test failed");
static_assert(static_cast<int>(fixed_point<std::uint8_t, 1>(511)) == 510, "sg14::fixed_point test failed");
static_assert(static_cast<float>(fixed_point<std::int8_t, 1>(123.5)) == 122, "sg14::fixed_point test failed");

static_assert(static_cast<int>(fixed_point<std::int8_t, 1>(255)) == 254, "sg14::fixed_point test failed");
static_assert(static_cast<int>(fixed_point<std::int8_t, 1>(254)) == 254, "sg14::fixed_point test failed");
static_assert(static_cast<int>(fixed_point<std::int8_t, 1>(-5)) == -6, "sg14::fixed_point test failed");

// conversion between fixed_point specializations
static_assert(static_cast<float>(ufixed4_4_t(fixed7_8_t(1.5))) == 1.5, "sg14::fixed_point test failed");
static_assert(static_cast<float>(ufixed8_8_t(fixed3_4_t(3.25))) == 3.25, "sg14::fixed_point test failed");
static_assert(static_cast<int>(fixed_point<std::uint8_t, 4>(fixed_point<std::int16_t, -4>(768))) == 768, "sg14::fixed_point test failed");
static_assert(static_cast<float>(fixed_point<std::uint64_t, -48>(fixed_point<std::uint32_t, -24>(3.141592654))) > 3.1415923f, "sg14::fixed_point test failed");
static_assert(static_cast<float>(fixed_point<std::uint64_t, -48>(fixed_point<std::uint32_t, -24>(3.141592654))) < 3.1415927f, "sg14::fixed_point test failed");

// closed_unit
static_assert(static_cast<double>(closed_unit<std::uint8_t>(.5)) == .5, "sg14::closed_unit test failed");
static_assert(static_cast<float>(closed_unit<std::uint16_t>(.125f)) == .125f, "sg14::closed_unit test failed");
static_assert(static_cast<float>(closed_unit<std::uint16_t>(.640625l)) == .640625, "sg14::closed_unit test failed");
static_assert(static_cast<float>(closed_unit<std::uint16_t>(1.640625)) == 1.640625f, "sg14::closed_unit test failed");
static_assert(static_cast<std::uint64_t>(closed_unit<std::uint16_t>(1u)) == 1, "sg14::closed_unit test failed");
static_assert(static_cast<float>(closed_unit<std::uint16_t>(2)) != 2, "sg14::closed_unit test failed");	// out-of-range test

// open_unit
static_assert(static_cast<double>(open_unit<std::uint8_t>(.5)) == .5, "sg14::closed_unit test failed");
static_assert(static_cast<float>(open_unit<std::uint16_t>(.125f)) == .125f, "sg14::closed_unit test failed");
static_assert(static_cast<float>(open_unit<std::uint16_t>(.640625l)) == .640625, "sg14::closed_unit test failed");
static_assert(static_cast<float>(open_unit<std::uint16_t>(1)) == 0, "sg14::closed_unit test failed");	// dropped bit

////////////////////////////////////////////////////////////////////////////////
// arithmetic

// addition
static_assert(static_cast<int>((fixed31_0_t(123) + fixed31_0_t(123))) == 246, "sg14::fixed_point test failed");
static_assert(static_cast<float>((fixed15_16_t(123.125) + fixed15_16_t(123.75))) == 246.875, "sg14::fixed_point test failed");

// subtraction
static_assert(static_cast<int>((fixed31_0_t(999) - fixed31_0_t(369))) == 630, "sg14::fixed_point test failed");
static_assert(static_cast<float>((fixed15_16_t(246.875) - fixed15_16_t(123.75))) == 123.125, "sg14::fixed_point test failed");
static_assert(static_cast<float>((fixed_point<std::int16_t, -4>(123.125) - fixed_point<std::int16_t, -4>(246.875))) == -123.75, "sg14::fixed_point test failed");

// multiplication
static_assert(static_cast<int>((ufixed8_0_t(0x55) * ufixed8_0_t(2))) == 0xaa, "sg14::fixed_point test failed");
static_assert(static_cast<float>((fixed15_16_t(123.75) * fixed15_16_t(44.5))) == 5506.875, "sg14::fixed_point test failed");
#if defined(_SG14_FIXED_POINT_128)
static_assert(static_cast<int>((fixed_point<std::uint64_t, -8>(1003006) * fixed_point<std::uint64_t, -8>(7))) == 7021042, "sg14::fixed_point test failed");
#endif

// division
static_assert(static_cast<float>((fixed_point<std::int8_t, -1>(63) / fixed_point<std::int8_t, -1>(-4))) == -15.5, "sg14::fixed_point test failed");
static_assert(static_cast<int>((fixed_point<std::int8_t, 1>(-255) / fixed_point<std::int8_t, 1>(-8))) == 32, "sg14::fixed_point test failed");
static_assert(static_cast<int>((fixed31_0_t(-999) / fixed31_0_t(3))) == -333, "sg14::fixed_point test failed");
#if defined(_SG14_FIXED_POINT_128)
static_assert(static_cast<int>((fixed_point<std::uint64_t, -8>(65535) / fixed_point<std::uint64_t, -8>(256))) == 255, "sg14::fixed_point test failed");
#endif

////////////////////////////////////////////////////////////////////////////////
// sg14::fixed_point_promotion_t

static_assert(std::is_same<fixed_point_promotion_t<fixed_point<std::int8_t, -4>>, fixed_point<std::int16_t, -8>>::value, "sg14::fixed_point_promotion_t test failed");
static_assert(std::is_same<fixed_point_promotion_t<fixed_point<std::uint32_t, 44>>, fixed_point<std::uint64_t, 88>>::value, "sg14::fixed_point_promotion_t test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::fixed_point_demotion_t

static_assert(std::is_same<fixed_point<std::int8_t, -4>, fixed_point_demotion_t<fixed_point<std::int16_t, -9>>>::value, "sg14::fixed_point_demotion_t test failed");
static_assert(std::is_same<fixed_point<std::uint32_t, 44>, fixed_point_demotion_t<fixed_point<std::uint64_t, 88>>>::value, "sg14::fixed_point_demotion_t test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::make_fixed_from_repr

static_assert(make_fixed_from_repr<std::uint8_t, 8>::integer_digits == 8, "sg14::make_fixed_from_repr test failed");
static_assert(make_fixed_from_repr<std::int32_t, 27>::integer_digits == 27, "sg14::make_fixed_from_repr test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::make_fixed_from_pair

static_assert(std::is_same<make_fixed_from_pair<fixed_point<std::uint8_t, -4>, fixed_point<std::int8_t, -4>>, fixed_point<int8_t, -3>>::value, "sg14::make_fixed_from_pair");
static_assert(std::is_same<make_fixed_from_pair<fixed_point<std::int16_t, -4>, fixed_point<std::int32_t, -14>>, fixed_point<int32_t, -14>>::value, "sg14::make_fixed_from_pair");
static_assert(std::is_same<make_fixed_from_pair<fixed_point<std::int16_t, 0>, fixed_point<std::uint64_t, -60>>, fixed_point<int64_t, -48>>::value, "sg14::make_fixed_from_pair");

////////////////////////////////////////////////////////////////////////////////
// comparison

static_assert(fixed_point<uint8_t>(4.5) == fixed_point<int16_t>(4.5), "sg14::fixed_point test failed");
static_assert(!(fixed_point<uint8_t>(4.5) == fixed_point<int16_t>(-4.5)), "sg14::fixed_point test failed");

static_assert(fixed_point<uint8_t>(4.5) != fixed_point<int16_t>(-4.5), "sg14::fixed_point test failed");
static_assert(!(fixed_point<uint8_t>(4.5) != fixed_point<int16_t>(4.5)), "sg14::fixed_point test failed");

static_assert(fixed_point<uint8_t>(4.5) < fixed_point<int16_t>(5.6), "sg14::fixed_point test failed");
static_assert(!(fixed_point<int8_t>(-4.5) < fixed_point<int16_t>(-5.6)), "sg14::fixed_point test failed");

static_assert(fixed_point<uint8_t>(4.6) > fixed_point<int16_t>(4.5), "sg14::fixed_point test failed");
static_assert(!(fixed_point<uint8_t>(4.6) < fixed_point<int16_t>(-4.5)), "sg14::fixed_point test failed");

static_assert(fixed_point<uint8_t>(4.5) <= fixed_point<int16_t>(4.5), "sg14::fixed_point test failed");
static_assert(!(fixed_point<uint8_t>(4.5) <= fixed_point<int16_t>(-4.5)), "sg14::fixed_point test failed");

static_assert(fixed_point<uint8_t>(4.5) >= fixed_point<int16_t>(4.5), "sg14::fixed_point test failed");
static_assert(fixed_point<uint8_t>(4.5) >= fixed_point<int16_t>(-4.5), "sg14::fixed_point test failed");
static_assert(!(fixed_point<uint8_t>(4.5) >= fixed_point<int16_t>(4.6)), "sg14::fixed_point test failed");

// TODO: Is this acceptable?
static_assert(fixed_point<uint8_t, -1>(.5) == fixed_point<uint8_t, 0>(0), "sg14::fixed_point test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::fixed_point_mul_result_t

static_assert(fixed_point_mul_result_t<fixed_point<std::uint8_t, -4>>::integer_digits == 8, "sg14::fixed_point_mul_result_t test failed");
static_assert(fixed_point_mul_result_t<fixed_point<std::int32_t, -25>>::integer_digits == 12, "sg14::fixed_point_mul_result_t test failed");
static_assert(fixed_point_mul_result_t<fixed_point<std::uint8_t, 0>>::integer_digits == 16, "sg14::fixed_point_mul_result_t test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::safe_multiply

static_assert(static_cast<int>(safe_multiply(fixed7_8_t(127), fixed7_8_t(127))) == 16129, "sg14::safe_multiply test failed");
static_assert(static_cast<int>(safe_multiply(ufixed4_4_t(15.9375), ufixed4_4_t(15.9375))) == 254, "sg14::safe_multiply test failed");
static_assert(static_cast<float>(safe_multiply(ufixed4_4_t(0.0625), ufixed4_4_t(0.0625))) == 0, "sg14::safe_multiply test failed");
static_assert(static_cast<float>(safe_multiply(ufixed8_0_t(1), ufixed8_0_t(1))) == 0, "sg14::safe_multiply test failed");
static_assert(static_cast<float>(safe_multiply(ufixed8_0_t(174), ufixed8_0_t(25))) == 4096, "sg14::safe_multiply test failed");
static_assert(static_cast<int>(safe_multiply(make_fixed<8, 0, false>(174), make_fixed<6, 2, false>(25))) == 4288, "sg14::safe_multiply test failed");
static_assert(static_cast<double>(safe_multiply(fixed4_3_t(15.875), make_fixed<16, 0>(65535))) == 1040368.125, "sg14::safe_multiply test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::fixed_point_add_result_t

static_assert(fixed_point_add_result_t<std::uint8_t, -4>::integer_digits == 5, "sg14::fixed_point_add_result_t test failed");
static_assert(fixed_point_add_result_t<std::int32_t, -25, 4>::integer_digits == 8, "sg14::fixed_point_add_result_t test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::safe_add

static_assert(static_cast<int>(safe_add(fixed_point<std::uint8_t, -1>(127), fixed_point<std::uint8_t, -1>(127))) == 254, "sg14::safe_add test failed");
static_assert(static_cast<float>(safe_add(ufixed4_4_t(15.5), ufixed4_4_t(14.25), ufixed4_4_t(13.5))) == 43.25, "sg14::safe_add test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::abs

static_assert(static_cast<int>(abs(fixed7_0_t(66))) == 66, "sg14::abs test failed");
static_assert(static_cast<int>(abs(fixed7_0_t(-123))) == 123, "sg14::abs test failed");
static_assert(static_cast<std::int64_t>(abs(fixed63_0_t(9223372036854775807))) == 9223372036854775807, "sg14::abs test failed");
static_assert(static_cast<std::int64_t>(abs(fixed63_0_t(-9223372036854775807))) == 9223372036854775807, "sg14::abs test failed");
static_assert(static_cast<int>(abs(fixed7_8_t(-5))) == 5, "sg14::abs test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::sqrt

static_assert(static_cast<int>(sqrt(ufixed8_0_t(225))) == 15, "sg14::sqrt test failed");
static_assert(static_cast<int>(sqrt(fixed7_0_t(81))) == 9, "sg14::sqrt test failed");

static_assert(static_cast<int>(sqrt(fixed_point<std::uint8_t, -1>(4))) == 2, "sg14::sqrt test failed");
static_assert(static_cast<int>(sqrt(fixed_point<std::int8_t, -2>(9))) == 3, "sg14::sqrt test failed");

static_assert(static_cast<int>(sqrt(ufixed4_4_t(4))) == 2, "sg14::sqrt test failed");
static_assert(static_cast<float>(sqrt(fixed_point<std::int32_t, -24>(3.141592654))) > 1.7724537849426, "sg14::sqrt test failed");
static_assert(static_cast<float>(sqrt(fixed_point<std::int32_t, -24>(3.141592654))) < 1.7724537849427, "sg14::sqrt test failed");
#if defined(_SG14_FIXED_POINT_128)
static_assert(static_cast<std::int64_t>(sqrt(fixed63_0_t(9223372036854775807))) == 3037000499, "sg14::sqrt test failed");
#endif

////////////////////////////////////////////////////////////////////////////////
// sg14::lerp

static_assert(static_cast<float>(lerp(ufixed1_7_t(1), ufixed1_7_t(0), .5)) == .5f, "sg14::lerp test failed");
static_assert(static_cast<float>(lerp(ufixed8_8_t(.125), ufixed8_8_t(.625), .5)) == .375f, "sg14::lerp test failed");
static_assert(static_cast<unsigned>(lerp(ufixed16_16_t(42123.51323), ufixed16_16_t(432.9191), .812)) == 8270, "sg14::lerp test failed");

static_assert(static_cast<float>(lerp(fixed_point<std::int8_t, -6>(1), fixed_point<std::int8_t, -6>(0), .5)) == .5f, "sg14::lerp test failed");
static_assert(static_cast<float>(lerp(fixed_point<std::int16_t, -10>(.125), fixed_point<std::int16_t, -10>(.625), .5)) == .375f, "sg14::lerp test failed");
static_assert(static_cast<float>(lerp(fixed_point<std::int32_t, -6>(.125), fixed_point<std::int32_t, -6>(.625), .25)) == .25f, "sg14::lerp test failed");

////////////////////////////////////////////////////////////////////////////////
// fixed_point specializations

// integer_digits
static_assert(fixed0_7_t::integer_digits == 0, "fixed_point specializations test failed");
static_assert(fixed1_6_t::integer_digits == 1, "fixed_point specializations test failed");
static_assert(fixed3_4_t::integer_digits == 3, "fixed_point specializations test failed");
static_assert(fixed4_3_t::integer_digits == 4, "fixed_point specializations test failed");
static_assert(fixed7_0_t::integer_digits == 7, "fixed_point specializations test failed");

static_assert(ufixed0_8_t::integer_digits == 0, "fixed_point specializations test failed");
static_assert(ufixed1_7_t::integer_digits == 1, "fixed_point specializations test failed");
static_assert(ufixed4_4_t::integer_digits == 4, "fixed_point specializations test failed");
static_assert(ufixed8_0_t::integer_digits == 8, "fixed_point specializations test failed");

static_assert(fixed0_15_t::integer_digits == 0, "fixed_point specializations test failed");
static_assert(fixed1_14_t::integer_digits == 1, "fixed_point specializations test failed");
static_assert(fixed7_8_t::integer_digits == 7, "fixed_point specializations test failed");
static_assert(fixed8_7_t::integer_digits == 8, "fixed_point specializations test failed");
static_assert(fixed15_0_t::integer_digits == 15, "fixed_point specializations test failed");

static_assert(ufixed0_16_t::integer_digits == 0, "fixed_point specializations test failed");
static_assert(ufixed1_15_t::integer_digits == 1, "fixed_point specializations test failed");
static_assert(ufixed8_8_t::integer_digits == 8, "fixed_point specializations test failed");
static_assert(ufixed16_0_t::integer_digits == 16, "fixed_point specializations test failed");

static_assert(fixed0_31_t::integer_digits == 0, "fixed_point specializations test failed");
static_assert(fixed1_30_t::integer_digits == 1, "fixed_point specializations test failed");
static_assert(fixed15_16_t::integer_digits == 15, "fixed_point specializations test failed");
static_assert(fixed16_15_t::integer_digits == 16, "fixed_point specializations test failed");
static_assert(fixed31_0_t::integer_digits == 31, "fixed_point specializations test failed");

static_assert(ufixed0_32_t::integer_digits == 0, "fixed_point specializations test failed");
static_assert(ufixed1_31_t::integer_digits == 1, "fixed_point specializations test failed");
static_assert(ufixed16_16_t::integer_digits == 16, "fixed_point specializations test failed");
static_assert(ufixed32_0_t::integer_digits == 32, "fixed_point specializations test failed");

static_assert(fixed0_63_t::integer_digits == 0, "fixed_point specializations test failed");
static_assert(fixed1_62_t::integer_digits == 1, "fixed_point specializations test failed");
static_assert(fixed31_32_t::integer_digits == 31, "fixed_point specializations test failed");
static_assert(fixed32_31_t::integer_digits == 32, "fixed_point specializations test failed");
static_assert(fixed63_0_t::integer_digits == 63, "fixed_point specializations test failed");

static_assert(ufixed0_64_t::integer_digits == 0, "fixed_point specializations test failed");
static_assert(ufixed1_63_t::integer_digits == 1, "fixed_point specializations test failed");
static_assert(ufixed32_32_t::integer_digits == 32, "fixed_point specializations test failed");
static_assert(ufixed64_0_t::integer_digits == 64, "fixed_point specializations test failed");

#if defined(_SG14_FIXED_POINT_128)
static_assert(fixed0_127_t::integer_digits == 0, "fixed_point specializations test failed");
static_assert(fixed1_126_t::integer_digits == 1, "fixed_point specializations test failed");
static_assert(fixed63_64_t::integer_digits == 63, "fixed_point specializations test failed");
static_assert(fixed64_63_t::integer_digits == 64, "fixed_point specializations test failed");
static_assert(fixed127_0_t::integer_digits == 127, "fixed_point specializations test failed");

static_assert(ufixed0_128_t::integer_digits == 0, "fixed_point specializations test failed");
static_assert(ufixed1_127_t::integer_digits == 1, "fixed_point specializations test failed");
static_assert(ufixed64_64_t::integer_digits == 64, "fixed_point specializations test failed");
static_assert(ufixed128_0_t::integer_digits == 128, "fixed_point specializations test failed");
#endif

// fractional_digits
static_assert(fixed0_7_t::fractional_digits == 7, "fixed_point specializations test failed");
static_assert(fixed1_6_t::fractional_digits == 6, "fixed_point specializations test failed");
static_assert(fixed3_4_t::fractional_digits == 4, "fixed_point specializations test failed");
static_assert(fixed4_3_t::fractional_digits == 3, "fixed_point specializations test failed");
static_assert(fixed7_0_t::fractional_digits == 0, "fixed_point specializations test failed");

static_assert(ufixed0_8_t::fractional_digits == 8, "fixed_point specializations test failed");
static_assert(ufixed1_7_t::fractional_digits == 7, "fixed_point specializations test failed");
static_assert(ufixed4_4_t::fractional_digits == 4, "fixed_point specializations test failed");
static_assert(ufixed8_0_t::fractional_digits == 0, "fixed_point specializations test failed");

static_assert(fixed0_15_t::fractional_digits == 15, "fixed_point specializations test failed");
static_assert(fixed1_14_t::fractional_digits == 14, "fixed_point specializations test failed");
static_assert(fixed7_8_t::fractional_digits == 8, "fixed_point specializations test failed");
static_assert(fixed8_7_t::fractional_digits == 7, "fixed_point specializations test failed");
static_assert(fixed15_0_t::fractional_digits == 0, "fixed_point specializations test failed");

static_assert(ufixed0_16_t::fractional_digits == 16, "fixed_point specializations test failed");
static_assert(ufixed1_15_t::fractional_digits == 15, "fixed_point specializations test failed");
static_assert(ufixed8_8_t::fractional_digits == 8, "fixed_point specializations test failed");
static_assert(ufixed16_0_t::fractional_digits == 0, "fixed_point specializations test failed");

static_assert(fixed0_31_t::fractional_digits == 31, "fixed_point specializations test failed");
static_assert(fixed1_30_t::fractional_digits == 30, "fixed_point specializations test failed");
static_assert(fixed15_16_t::fractional_digits == 16, "fixed_point specializations test failed");
static_assert(fixed16_15_t::fractional_digits == 15, "fixed_point specializations test failed");
static_assert(fixed31_0_t::fractional_digits == 0, "fixed_point specializations test failed");

static_assert(ufixed0_32_t::fractional_digits == 32, "fixed_point specializations test failed");
static_assert(ufixed1_31_t::fractional_digits == 31, "fixed_point specializations test failed");
static_assert(ufixed16_16_t::fractional_digits == 16, "fixed_point specializations test failed");
static_assert(ufixed32_0_t::fractional_digits == 0, "fixed_point specializations test failed");

static_assert(fixed0_63_t::fractional_digits == 63, "fixed_point specializations test failed");
static_assert(fixed1_62_t::fractional_digits == 62, "fixed_point specializations test failed");
static_assert(fixed31_32_t::fractional_digits == 32, "fixed_point specializations test failed");
static_assert(fixed32_31_t::fractional_digits == 31, "fixed_point specializations test failed");
static_assert(fixed63_0_t::fractional_digits == 0, "fixed_point specializations test failed");

static_assert(ufixed0_64_t::fractional_digits == 64, "fixed_point specializations test failed");
static_assert(ufixed1_63_t::fractional_digits == 63, "fixed_point specializations test failed");
static_assert(ufixed32_32_t::fractional_digits == 32, "fixed_point specializations test failed");
static_assert(ufixed64_0_t::fractional_digits == 0, "fixed_point specializations test failed");

#if defined(_SG14_FIXED_POINT_128)
static_assert(fixed0_127_t::fractional_digits == 127, "fixed_point specializations test failed");
static_assert(fixed1_126_t::fractional_digits == 126, "fixed_point specializations test failed");
static_assert(fixed63_64_t::fractional_digits == 64, "fixed_point specializations test failed");
static_assert(fixed64_63_t::fractional_digits == 63, "fixed_point specializations test failed");
static_assert(fixed127_0_t::fractional_digits == 0, "fixed_point specializations test failed");

static_assert(ufixed0_128_t::fractional_digits == 128, "fixed_point specializations test failed");
static_assert(ufixed1_127_t::fractional_digits == 127, "fixed_point specializations test failed");
static_assert(ufixed64_64_t::fractional_digits == 64, "fixed_point specializations test failed");
static_assert(ufixed128_0_t::fractional_digits == 0, "fixed_point specializations test failed");
#endif

////////////////////////////////////////////////////////////////////////////////
// sg14::make_fixed

// integer_digits
static_assert(std::is_same<make_fixed<0, 7, true>, fixed0_7_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<1, 6, true>, fixed1_6_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<3, 4, true>, fixed3_4_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<4, 3, true>, fixed4_3_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<7, 0, true>, fixed7_0_t>::value, "make_fixed specializations test failed");

static_assert(std::is_same<make_fixed<0, 8, false>, ufixed0_8_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<1, 7, false>, ufixed1_7_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<4, 4, false>, ufixed4_4_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<8, 0, false>, ufixed8_0_t>::value, "make_fixed specializations test failed");

static_assert(std::is_same<make_fixed<0, 15, true>, fixed0_15_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<1, 14, true>, fixed1_14_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<7, 8, true>, fixed7_8_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<8, 7, true>, fixed8_7_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<15, 0, true>, fixed15_0_t>::value, "make_fixed specializations test failed");

static_assert(std::is_same<make_fixed<0, 16, false>, ufixed0_16_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<1, 15, false>, ufixed1_15_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<8, 8, false>, ufixed8_8_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<16, 0, false>, ufixed16_0_t>::value, "make_fixed specializations test failed");

static_assert(std::is_same<make_fixed<0, 31, true>, fixed0_31_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<1, 30, true>, fixed1_30_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<15, 16, true>, fixed15_16_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<16, 15, true>, fixed16_15_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<31, 0, true>, fixed31_0_t>::value, "make_fixed specializations test failed");

static_assert(std::is_same<make_fixed<0, 32, false>, ufixed0_32_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<1, 31, false>, ufixed1_31_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<16, 16, false>, ufixed16_16_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<32, 0, false>, ufixed32_0_t>::value, "make_fixed specializations test failed");

static_assert(std::is_same<make_fixed<0, 63, true>, fixed0_63_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<1, 62, true>, fixed1_62_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<31, 32, true>, fixed31_32_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<32, 31, true>, fixed32_31_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<63, 0, true>, fixed63_0_t>::value, "make_fixed specializations test failed");

static_assert(std::is_same<make_fixed<0, 64, false>, ufixed0_64_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<1, 63, false>, ufixed1_63_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<32, 32, false>, ufixed32_32_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<64, 0, false>, ufixed64_0_t>::value, "make_fixed specializations test failed");

#if defined(_SG14_FIXED_POINT_128)
static_assert(std::is_same<make_fixed<0, 127, true>, fixed0_127_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<1, 126, true>, fixed1_126_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<63, 64, true>, fixed63_64_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<64, 63, true>, fixed64_63_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<127, 0, true>, fixed127_0_t>::value, "make_fixed specializations test failed");

static_assert(std::is_same<make_fixed<0, 128, false>, ufixed0_128_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<1, 127, false>, ufixed1_127_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<64, 64, false>, ufixed64_64_t>::value, "make_fixed specializations test failed");
static_assert(std::is_same<make_fixed<128, 0, false>, ufixed128_0_t>::value, "make_fixed specializations test failed");
#endif
