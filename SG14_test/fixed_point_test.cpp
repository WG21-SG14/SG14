#include "fixed_point.h"

using namespace sg14;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// sg14::_impl

////////////////////////////////////////////////////////////////////////////////
// sg14::_impl::shift_left/right positive RHS

static_assert(_impl::shift_left<8, std::uint16_t>((std::uint16_t)0x1234) == 0x3400, "sg14::_impl::shift_left test failed");
static_assert(_impl::shift_left<8, std::uint16_t>((std::uint8_t)0x1234) == 0x3400, "sg14::_impl::shift_left test failed");
static_assert(_impl::shift_left<8, std::uint8_t>((std::uint16_t)0x1234) == 0x0, "sg14::_impl::shift_left test failed");
static_assert(_impl::shift_left<8, std::int16_t>(-123) == -31488, "sg14::_impl::shift_left test failed");

static_assert(_impl::shift_right<8, std::uint16_t, std::uint16_t>(0x1234) == 0x12, "sg14::_impl::shift_right test failed");
static_assert(_impl::shift_right<8, std::uint16_t, std::uint8_t>(0x1234) == 0x0, "sg14::_impl::shift_right test failed");
static_assert(_impl::shift_right<8, std::uint8_t, std::uint16_t>(0x1234) == 0x12, "sg14::_impl::shift_right test failed");
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
////////////////////////////////////////////////////////////////////////////////
// sg14::fixed_point

template <typename T, int E> using fp = fixed_point<T, E>;

template <int E> using fp_u8 = fixed_point<std::uint8_t, E>;
template <int E> using fp_u16 = fixed_point<std::uint16_t, E>;
template <int E> using fp_u32 = fixed_point<std::uint32_t, E>;
template <int E> using fp_u64 = fixed_point<std::uint64_t, E>;

template <int E> using fp_s8 = fixed_point<std::int8_t, E>;
template <int E> using fp_s16 = fixed_point<std::int16_t, E>;
template <int E> using fp_s32 = fixed_point<std::int32_t, E>;
template <int E> using fp_s64 = fixed_point<std::int64_t, E>;

////////////////////////////////////////////////////////////////////////////////
// conversion

// exponent == 0
static_assert(fp_u8<0>(12.34f).get<float>() == 12.f, "sg14::fixed_point test failed");
static_assert(fp_u16<0>(12.34f).get<double>() == 12.f, "sg14::fixed_point test failed");
static_assert(fp_u32<0>(12.34f).get<long double>() == 12.f, "sg14::fixed_point test failed");
static_assert(fp_u64<0>(12.34f).get<float>() == 12.f, "sg14::fixed_point test failed");

static_assert(fp_s8<0>(-12.34f).get<double>() == -12.f, "sg14::fixed_point test failed");
static_assert(fp_s16<0>(-12.34f).get<long double>() == -12.f, "sg14::fixed_point test failed");
static_assert(fp_s32<0>(-12.34f).get<float>() == -12.f, "sg14::fixed_point test failed");
static_assert(fp_s64<0>(-12.34f).get<double>() == -12.f, "sg14::fixed_point test failed");

// exponent = -1
static_assert(fp_u8<-1>(127.5).get<float>() == 127.5, "sg14::fixed_point test failed");

static_assert(fp_s8<-1>(63.5).get<float>() == 63.5, "sg14::fixed_point test failed");
static_assert(fp_s8<-1>(-63.5).get<float>() == -63.5, "sg14::fixed_point test failed");

// exponent == -7
static_assert(fp_u8<-7>(.125f).get<float>() == .125f, "sg14::fixed_point test failed");
static_assert(fp_u16<-7>(232.125f).get<float>() == 232.125f, "sg14::fixed_point test failed");
static_assert(fp_u32<-7>(232.125f).get<float>() == 232.125f, "sg14::fixed_point test failed");
static_assert(fp_u64<-7>(232.125f).get<float>() == 232.125f, "sg14::fixed_point test failed");

static_assert(fp_s8<-7>(.125f).get<float>() == .125f, "sg14::fixed_point test failed");
static_assert(fp_s16<-7>(123.125f).get<float>() == 123.125f, "sg14::fixed_point test failed");
static_assert(fp_s32<-7>(123.125f).get<float>() == 123.125f, "sg14::fixed_point test failed");
static_assert(fp_s64<-7>(123.125f).get<float>() == 123.125f, "sg14::fixed_point test failed");

static_assert(fp_u8<-7>(.125f).get<double>() == .125f, "sg14::fixed_point test failed");
static_assert(fp_u16<-7>(232.125f).get<long double>() == 232.125f, "sg14::fixed_point test failed");
static_assert(fp_u32<-7>(232.125f).get<double>() == 232.125f, "sg14::fixed_point test failed");
static_assert(fp_u64<-7>(232.125f).get<long double>() == 232.125f, "sg14::fixed_point test failed");

static_assert(fp_s8<-7>(1).get<long double>() != 1.f, "sg14::fixed_point test failed");
static_assert(fp_s8<-7>(.5).get<float>() == .5f, "sg14::fixed_point test failed");
static_assert(fp_s8<-7>(.125f).get<long double>() == .125f, "sg14::fixed_point test failed");
static_assert(fp_s16<-7>(123.125f).get<int>() == 123, "sg14::fixed_point test failed");
static_assert(fp_s32<-7>(123.125f).get<long double>() == 123.125f, "sg14::fixed_point test failed");
static_assert(fp_s64<-7>(123.125l).get<float>() == 123.125f, "sg14::fixed_point test failed");

// exponent == 16
static_assert(fp_u8<16>(65536).get<float>() == 65536.f, "sg14::fixed_point test failed");
static_assert(fp_u16<16>(6553.).get<int>() == 0, "sg14::fixed_point test failed");
static_assert(fp_u32<16>(4294967296l).get<double>() == 4294967296.f, "sg14::fixed_point test failed");
static_assert(fp_u64<16>(1125895611875328l).get<double>() == 1125895611875328l, "sg14::fixed_point test failed");

static_assert(fp_s8<16>(-65536).get<float>() == -65536.f, "sg14::fixed_point test failed");
static_assert(fp_s16<16>(-6553.).get<int>() == 0, "sg14::fixed_point test failed");
static_assert(fp_s32<16>(-4294967296l).get<double>() == -4294967296.f, "sg14::fixed_point test failed");
static_assert(fp_s64<16>(-1125895611875328l).get<double>() == -1125895611875328l, "sg14::fixed_point test failed");

// exponent = 1
static_assert(fp_u8<1>(510).get<int>() == 510, "sg14::fixed_point test failed");
static_assert(fp_u8<1>(511).get<int>() == 510, "sg14::fixed_point test failed");
static_assert(fp_s8<1>(123.5).get<float>() == 122, "sg14::fixed_point test failed");

static_assert(fp_s8<1>(255).get<int>() == 254, "sg14::fixed_point test failed");
static_assert(fp_s8<1>(254).get<int>() == 254, "sg14::fixed_point test failed");
static_assert(fp_s8<1>(-5).get<int>() == -6, "sg14::fixed_point test failed");

// closed_unit
template <typename T> using cu = closed_unit<T>;
static_assert(cu<std::uint8_t>(.5).get<double>() == .5, "sg14::closed_unit test failed");
static_assert(cu<std::uint16_t>(.125f).get<float>() == .125f, "sg14::closed_unit test failed");
static_assert(cu<std::uint16_t>(.640625l).get<float>() == .640625, "sg14::closed_unit test failed");
static_assert(cu<std::uint16_t>(1.640625).get<float>() == 1.640625f, "sg14::closed_unit test failed");
static_assert(cu<std::uint16_t>(1u).get<std::uint64_t>() == 1, "sg14::closed_unit test failed");
static_assert(cu<std::uint16_t>(2).get<float>() != 2, "sg14::closed_unit test failed");	// out-of-range test

// open_unit
template <typename T> using ou = open_unit<T>;
static_assert(ou<std::uint8_t>(.5).get<double>() == .5, "sg14::closed_unit test failed");
static_assert(ou<std::uint16_t>(.125f).get<float>() == .125f, "sg14::closed_unit test failed");
static_assert(ou<std::uint16_t>(.640625l).get<float>() == .640625, "sg14::closed_unit test failed");
static_assert(ou<std::uint16_t>(1).get<float>() == 0, "sg14::closed_unit test failed");	// dropped bit

////////////////////////////////////////////////////////////////////////////////
// arithmetic

// addition
static_assert((fp_s32<0>(123) + fp_s32<0>(123)).get<int>() == 246, "sg14::fixed_point test failed");
static_assert((fp_s32<-16>(123.125) + fp_s32<-16>(123.75)).get<float>() == 246.875, "sg14::fixed_point test failed");

// subtraction
static_assert((fp_s32<0>(999) - fp_s32<0>(369)).get<int>() == 630, "sg14::fixed_point test failed");
static_assert((fp_s32<-16>(246.875) - fp_s32<-16>(123.75)).get<float>() == 123.125, "sg14::fixed_point test failed");
static_assert((fp_s16<-4>(123.125) - fp_s16<-4>(246.875)).get<float>() == -123.75, "sg14::fixed_point test failed");

// multiplication
static_assert((fp_u8<0>(0x55) * fp_u8<0>(2)).get<int>() == 0xaa, "sg14::fixed_point test failed");
static_assert((fp_u64<-8>(1003006) * fp_u64<-8>(7)).get<int>() == 7021042, "sg14::fixed_point test failed");
static_assert((fp_s32<-16>(123.75) * fp_s32<-16>(44.5)).get<float>() == 5506.875, "sg14::fixed_point test failed");

// division
static_assert((fp_s8<-1>(63) / fp_s8<-1>(-4)).get<float>() == -15.5, "sg14::fixed_point test failed");
static_assert((fp_s8<1>(-255) / fp_s8<1>(-8)).get<int>() == 32, "sg14::fixed_point test failed");
static_assert((fp_s32<0>(-999) / fp_s32<0>(3)).get<int>() == -333, "sg14::fixed_point test failed");
static_assert((fp_u64<-8>(65535) / fp_u64<-8>(256)).get<int>() == 255, "sg14::fixed_point test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::abs

static_assert(sg14::abs(fp_s8<0>(66)).get<int>() == 66, "sg14::sqrt test failed");
static_assert(sg14::abs(fp_s8<0>(-123)).get<int>() == 123, "sg14::sqrt test failed");
static_assert(sg14::abs(fp_s64<0>(9223372036854775807)).get<std::int64_t>() == 9223372036854775807, "sg14::sqrt test failed");
static_assert(sg14::abs(fp_s64<0>(-9223372036854775807)).get<std::int64_t>() == 9223372036854775807, "sg14::sqrt test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::sqrt

static_assert(sg14::sqrt(fp_u8<0>(225)).get<int>() == 15, "sg14::sqrt test failed");
//static_assert(sg14::sqrt(fp_s32<-24>(3.141592654)).get<float>() == 1.772453851, "sg14::sqrt test failed");
static_assert(sg14::sqrt(fp_s64<0>(9223372036854775807)).get<std::int64_t>() == 3037000499, "sg14::sqrt test failed");

////////////////////////////////////////////////////////////////////////////////
// sg14::lerp

static_assert(sg14::lerp(fp_u8<-7>(1), fp_u8<-7>(0), .5).get<float>() == .5f, "sg14::lerp test failed");
static_assert(sg14::lerp(fp_u16<-7>(.125), fp_u16<-7>(.625), .5).get<float>() == .375f, "sg14::lerp test failed");
static_assert(sg14::lerp(fp_u32<-16>(42123.51323), fp_u32<-16>(432.9191), .812).get<unsigned>() == 8270, "sg14::lerp test failed");

static_assert(sg14::lerp(fp_s8<-6>(1), fp_s8<-6>(0), .5).get<float>() == .5f, "sg14::lerp test failed");
static_assert(sg14::lerp(fp_s16<-10>(.125), fp_s16<-10>(.625), .5).get<float>() == .375f, "sg14::lerp test failed");
static_assert(sg14::lerp(fp_s32<-6>(.125), fp_s32<-6>(.625), .25).get<float>() == .25f, "sg14::lerp test failed");
