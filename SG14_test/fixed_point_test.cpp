#include "fixed_point.h"

using namespace sg14;

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
static_assert(_impl::shift_left<-8, std::int16_t>(-31488) == -123, "sg14::_impl::shift_right test failed");

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
////////////////////////////////////////////////////////////////////////////////
// sg14::fixed_point

////////////////////////////////////////////////////////////////////////////////
// conversion

// exponent == 0
static_assert(ufixed8_0_t(12.34f).get<float>() == 12.f, "sg14::fixed_point test failed");
static_assert(ufixed16_0_t(12.34f).get<double>() == 12.f, "sg14::fixed_point test failed");
static_assert(ufixed32_0_t(12.34f).get<long double>() == 12.f, "sg14::fixed_point test failed");
static_assert(ufixed64_0_t(12.34f).get<float>() == 12.f, "sg14::fixed_point test failed");

static_assert(fixed7_0_t(-12.34f).get<double>() == -12.f, "sg14::fixed_point test failed");
static_assert(fixed15_0_t(-12.34f).get<long double>() == -12.f, "sg14::fixed_point test failed");
static_assert(fixed31_0_t(-12.34f).get<float>() == -12.f, "sg14::fixed_point test failed");
static_assert(fixed63_0_t(-12.34f).get<double>() == -12.f, "sg14::fixed_point test failed");

// exponent = -1
static_assert(fixed_point<std::uint8_t, -1>(127.5).get<float>() == 127.5, "sg14::fixed_point test failed");

static_assert(fixed_point<std::int8_t, -1>(63.5).get<float>() == 63.5, "sg14::fixed_point test failed");
static_assert(fixed_point<std::int8_t, -1>(-63.5).get<float>() == -63.5, "sg14::fixed_point test failed");

// exponent == -7
static_assert(fixed_point<std::uint8_t, -7>(.125f).get<float>() == .125f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::uint16_t, -8>(232.125f).get<float>() == 232.125f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::uint32_t, -7>(232.125f).get<float>() == 232.125f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::uint64_t, -7>(232.125f).get<float>() == 232.125f, "sg14::fixed_point test failed");

static_assert(fixed_point<std::int8_t, -7>(.125f).get<float>() == .125f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::int16_t, -7>(123.125f).get<float>() == 123.125f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::int32_t, -7>(123.125f).get<float>() == 123.125f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::int64_t, -7>(123.125f).get<float>() == 123.125f, "sg14::fixed_point test failed");

static_assert(fixed_point<std::uint8_t, -7>(.125f).get<double>() == .125f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::uint16_t, -8>(232.125f).get<long double>() == 232.125f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::uint32_t, -7>(232.125f).get<double>() == 232.125f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::uint64_t, -7>(232.125f).get<long double>() == 232.125f, "sg14::fixed_point test failed");

static_assert(fixed_point<std::int8_t, -7>(1).get<long double>() != 1.f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::int8_t, -7>(.5).get<float>() == .5f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::int8_t, -7>(.125f).get<long double>() == .125f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::int16_t, -7>(123.125f).get<int>() == 123, "sg14::fixed_point test failed");
static_assert(fixed_point<std::int32_t, -7>(123.125f).get<long double>() == 123.125f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::int64_t, -7>(123.125l).get<float>() == 123.125f, "sg14::fixed_point test failed");

// exponent == 16
static_assert(fixed_point<std::uint8_t, 16>(65536).get<float>() == 65536.f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::uint16_t, 16>(6553.).get<int>() == 0, "sg14::fixed_point test failed");
static_assert(fixed_point<std::uint32_t, 16>(4294967296l).get<double>() == 4294967296.f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::uint64_t, 16>(1125895611875328l).get<double>() == 1125895611875328l, "sg14::fixed_point test failed");

static_assert(fixed_point<std::int8_t, 16>(-65536).get<float>() == -65536.f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::int16_t, 16>(-6553.).get<int>() == 0, "sg14::fixed_point test failed");
static_assert(fixed_point<std::int32_t, 16>(-4294967296l).get<double>() == -4294967296.f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::int64_t, 16>(-1125895611875328l).get<double>() == -1125895611875328l, "sg14::fixed_point test failed");

// exponent = 1
static_assert(fixed_point<std::uint8_t, 1>(510).get<int>() == 510, "sg14::fixed_point test failed");
static_assert(fixed_point<std::uint8_t, 1>(511).get<int>() == 510, "sg14::fixed_point test failed");
static_assert(fixed_point<std::int8_t, 1>(123.5).get<float>() == 122, "sg14::fixed_point test failed");

static_assert(fixed_point<std::int8_t, 1>(255).get<int>() == 254, "sg14::fixed_point test failed");
static_assert(fixed_point<std::int8_t, 1>(254).get<int>() == 254, "sg14::fixed_point test failed");
static_assert(fixed_point<std::int8_t, 1>(-5).get<int>() == -6, "sg14::fixed_point test failed");

// conversion between fixed_point specializations
static_assert(ufixed4_4_t(fixed7_8_t(1.5)).get<float>() == 1.5, "sg14::fixed_point test failed");
static_assert(ufixed8_8_t(fixed3_4_t(3.25)).get<float>() == 3.25, "sg14::fixed_point test failed");
static_assert(fixed_point<std::uint8_t, 4>(fixed_point<std::int16_t, -4>(768)).get<int>() == 768, "sg14::fixed_point test failed");
static_assert(fixed_point<std::uint64_t, -48>(fixed_point<std::uint32_t, -24>(3.141592654)).get<float>() > 3.1415923f, "sg14::fixed_point test failed");
static_assert(fixed_point<std::uint64_t, -48>(fixed_point<std::uint32_t, -24>(3.141592654)).get<float>() < 3.1415927f, "sg14::fixed_point test failed");

// closed_unit
static_assert(closed_unit<std::uint8_t>(.5).get<double>() == .5, "sg14::closed_unit test failed");
static_assert(closed_unit<std::uint16_t>(.125f).get<float>() == .125f, "sg14::closed_unit test failed");
static_assert(closed_unit<std::uint16_t>(.640625l).get<float>() == .640625, "sg14::closed_unit test failed");
static_assert(closed_unit<std::uint16_t>(1.640625).get<float>() == 1.640625f, "sg14::closed_unit test failed");
static_assert(closed_unit<std::uint16_t>(1u).get<std::uint64_t>() == 1, "sg14::closed_unit test failed");
static_assert(closed_unit<std::uint16_t>(2).get<float>() != 2, "sg14::closed_unit test failed");	// out-of-range test

// open_unit
static_assert(open_unit<std::uint8_t>(.5).get<double>() == .5, "sg14::closed_unit test failed");
static_assert(open_unit<std::uint16_t>(.125f).get<float>() == .125f, "sg14::closed_unit test failed");
static_assert(open_unit<std::uint16_t>(.640625l).get<float>() == .640625, "sg14::closed_unit test failed");
static_assert(open_unit<std::uint16_t>(1).get<float>() == 0, "sg14::closed_unit test failed");	// dropped bit

////////////////////////////////////////////////////////////////////////////////
// arithmetic

// addition
static_assert((fixed31_0_t(123) + fixed31_0_t(123)).get<int>() == 246, "sg14::fixed_point test failed");
static_assert((fixed15_16_t(123.125) + fixed15_16_t(123.75)).get<float>() == 246.875, "sg14::fixed_point test failed");

// subtraction
static_assert((fixed31_0_t(999) - fixed31_0_t(369)).get<int>() == 630, "sg14::fixed_point test failed");
static_assert((fixed15_16_t(246.875) - fixed15_16_t(123.75)).get<float>() == 123.125, "sg14::fixed_point test failed");
static_assert((fixed_point<std::int16_t, -4>(123.125) - fixed_point<std::int16_t, -4>(246.875)).get<float>() == -123.75, "sg14::fixed_point test failed");

// multiplication
static_assert((ufixed8_0_t(0x55) * ufixed8_0_t(2)).get<int>() == 0xaa, "sg14::fixed_point test failed");
static_assert((fixed15_16_t(123.75) * fixed15_16_t(44.5)).get<float>() == 5506.875, "sg14::fixed_point test failed");
#if defined(_SG14_FIXED_POINT_128)
static_assert((fixed_point<std::uint64_t, -8>(1003006) * fixed_point<std::uint64_t, -8>(7)).get<int>() == 7021042, "sg14::fixed_point test failed");
#endif

// division
static_assert((fixed_point<std::int8_t, -1>(63) / fixed_point<std::int8_t, -1>(-4)).get<float>() == -15.5, "sg14::fixed_point test failed");
static_assert((fixed_point<std::int8_t, 1>(-255) / fixed_point<std::int8_t, 1>(-8)).get<int>() == 32, "sg14::fixed_point test failed");
static_assert((fixed31_0_t(-999) / fixed31_0_t(3)).get<int>() == -333, "sg14::fixed_point test failed");
#if defined(_SG14_FIXED_POINT_128)
static_assert((fixed_point<std::uint64_t, -8>(65535) / fixed_point<std::uint64_t, -8>(256)).get<int>() == 255, "sg14::fixed_point test failed");
#endif

////////////////////////////////////////////////////////////////////////////////
// sg14::fixed_point_promotion_t

static_assert(std::is_same<fixed_point_promotion_t<std::int8_t, -4>, fixed_point<std::int16_t, -8>>::value, "sg14::promotion test failed");
static_assert(std::is_same<fixed_point_promotion_t<std::uint32_t, 44>, fixed_point<std::uint64_t, 88>>::value, "sg14::promotion test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::fixed_point_demotion_t

static_assert(std::is_same<fixed_point<std::int8_t, -4>, fixed_point_demotion_t<std::int16_t, -8>>::value, "sg14::promotion test failed");
static_assert(std::is_same<fixed_point<std::uint32_t, 44>, fixed_point_demotion_t<std::uint64_t, 88>>::value, "sg14::promotion test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::fixed_point_by_integer_digits_t

static_assert(fixed_point_by_integer_digits_t<std::uint8_t, 8>::integer_digits == 8, "sg14::fixed_point_by_integer_digits_t test failed");
static_assert(fixed_point_by_integer_digits_t<std::int32_t, 27>::integer_digits == 27, "sg14::fixed_point_by_integer_digits_t test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::fixed_point_mul_result_t

static_assert(fixed_point_mul_result_t<std::uint8_t, -4>::integer_digits == 8, "sg14::fixed_point_by_integer_digits_t test failed");
static_assert(fixed_point_mul_result_t<std::int32_t, -25>::integer_digits == 12, "sg14::fixed_point_by_integer_digits_t test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::safe_multiply

static_assert(safe_multiply(fixed7_8_t(127), fixed7_8_t(127)).get<int>() == 16129, "sg14::safe_multiply test failed");
static_assert(safe_multiply<std::uint8_t, -4>(15.9375, 15.9375).get<int>() == 254, "sg14::safe_multiply test failed");
static_assert(safe_multiply(ufixed4_4_t(0.0625), ufixed4_4_t(0.0625)).get<float>() == 0, "sg14::safe_multiply test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::fixed_point_add_result_t

static_assert(fixed_point_add_result_t<std::uint8_t, -4>::integer_digits == 5, "sg14::fixed_point_by_integer_digits_t test failed");
static_assert(fixed_point_add_result_t<std::int32_t, -25, 4>::integer_digits == 8, "sg14::fixed_point_by_integer_digits_t test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::safe_add

static_assert(safe_add(fixed_point<std::uint8_t, -1>(127), fixed_point<std::uint8_t, -1>(127)).get<int>() == 254, "sg14::safe_add test failed");
static_assert(safe_add<std::uint8_t, -4>(15, 13).get<int>() == 28, "sg14::safe_add test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::abs

static_assert(abs(fixed7_0_t(66)).get<int>() == 66, "sg14::sqrt test failed");
static_assert(abs(fixed7_0_t(-123)).get<int>() == 123, "sg14::sqrt test failed");
static_assert(abs(fixed63_0_t(9223372036854775807)).get<std::int64_t>() == 9223372036854775807, "sg14::sqrt test failed");
static_assert(abs(fixed63_0_t(-9223372036854775807)).get<std::int64_t>() == 9223372036854775807, "sg14::sqrt test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::sqrt

static_assert(sqrt(ufixed8_0_t(225)).get<int>() == 15, "sg14::sqrt test failed");
static_assert(sqrt(fixed7_0_t(81)).get<int>() == 9, "sg14::sqrt test failed");

static_assert(sqrt(fixed_point<std::uint8_t, -1>(4)).get<int>() == 2, "sg14::sqrt test failed");
static_assert(sqrt(fixed_point<std::int8_t, -2>(9)).get<int>() == 3, "sg14::sqrt test failed");

static_assert(sqrt(ufixed4_4_t(4)).get<int>() == 2, "sg14::sqrt test failed");
static_assert(sqrt(fixed_point<std::int32_t, -24>(3.141592654)).get<float>() > 1.7724537849426, "sg14::sqrt test failed");
static_assert(sqrt(fixed_point<std::int32_t, -24>(3.141592654)).get<float>() < 1.7724537849427, "sg14::sqrt test failed");
#if defined(_SG14_FIXED_POINT_128)
static_assert(sqrt(fixed63_0_t(9223372036854775807)).get<std::int64_t>() == 3037000499, "sg14::sqrt test failed");
#endif

////////////////////////////////////////////////////////////////////////////////
// sg14::lerp

static_assert(lerp(ufixed1_7_t(1), ufixed1_7_t(0), .5).get<float>() == .5f, "sg14::lerp test failed");
static_assert(lerp(ufixed8_8_t(.125), ufixed8_8_t(.625), .5).get<float>() == .375f, "sg14::lerp test failed");
static_assert(lerp(ufixed16_16_t(42123.51323), ufixed16_16_t(432.9191), .812).get<unsigned>() == 8270, "sg14::lerp test failed");

static_assert(lerp(fixed_point<std::int8_t, -6>(1), fixed_point<std::int8_t, -6>(0), .5).get<float>() == .5f, "sg14::lerp test failed");
static_assert(lerp(fixed_point<std::int16_t, -10>(.125), fixed_point<std::int16_t, -10>(.625), .5).get<float>() == .375f, "sg14::lerp test failed");
static_assert(lerp(fixed_point<std::int32_t, -6>(.125), fixed_point<std::int32_t, -6>(.625), .25).get<float>() == .25f, "sg14::lerp test failed");

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
