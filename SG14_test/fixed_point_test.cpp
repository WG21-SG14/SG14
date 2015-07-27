#include "fixed_point.h"

using namespace sg14;

////////////////////////////////////////////////////////////////////////////////
// crag::core::_impl::shift_left

static_assert(_impl::shift_left<8, std::uint16_t, std::uint16_t>(0x1234) == 0x3400, "crag::core::_impl::shift_left test failed");
static_assert(_impl::shift_left<8, std::uint16_t, std::uint8_t>(0x12) == 0x1200, "crag::core::_impl::shift_left test failed");
static_assert(_impl::shift_left<8, std::uint8_t, std::uint16_t>(0x1234) == 0x0, "crag::core::_impl::shift_left test failed");

static_assert(_impl::shift_left<-8, std::uint16_t, std::uint16_t>(0x1234) == 0x12, "crag::core::_impl::shift_left test failed");
static_assert(_impl::shift_left<-8, std::uint16_t, std::uint8_t>(0x12) == 0x0, "crag::core::_impl::shift_left test failed");
static_assert(_impl::shift_left<-8, std::uint8_t, std::uint16_t>(0x1234) == 0x12, "crag::core::_impl::shift_left test failed");

////////////////////////////////////////////////////////////////////////////////
// crag::core::_impl::shift_right

static_assert(_impl::shift_right<8, std::uint16_t, std::uint16_t>(0x1234) == 0x12, "crag::core::_impl::shift_right test failed");
static_assert(_impl::shift_right<8, std::uint16_t, std::uint8_t>(0x12) == 0x0, "crag::core::_impl::shift_right test failed");
static_assert(_impl::shift_right<8, std::uint8_t, std::uint16_t>(0x1234) == 0x12, "crag::core::_impl::shift_right test failed");

static_assert(_impl::shift_right<-8, std::uint16_t, std::uint16_t>(0x1234) == 0x3400, "crag::core::_impl::shift_right test failed");
static_assert(_impl::shift_right<-8, std::uint16_t, std::uint8_t>(0x12) == 0x1200, "crag::core::_impl::shift_right test failed");
static_assert(_impl::shift_right<-8, std::uint8_t, std::uint16_t>(0x1234) == 0x0, "crag::core::_impl::shift_right test failed");

////////////////////////////////////////////////////////////////////////////////
// crag::core::_impl::pow2

static_assert(_impl::pow2<float, 0>() == 1, "crag::core::_impl::pow2 test failed");
static_assert(_impl::pow2<double, -1>() == .5, "crag::core::_impl::pow2 test failed");
static_assert(_impl::pow2<long double, 1>() == 2, "crag::core::_impl::pow2 test failed");
static_assert(_impl::pow2<float, -3>() == .125, "crag::core::_impl::pow2 test failed");
static_assert(_impl::pow2<double, 7>() == 128, "crag::core::_impl::pow2 test failed");
static_assert(_impl::pow2<long double, 10>() == 1024, "crag::core::_impl::pow2 test failed");
static_assert(_impl::pow2<float, 20>() == 1048576, "crag::core::_impl::pow2 test failed");

////////////////////////////////////////////////////////////////////////////////
// crag::core::fixed_point

template <typename T, int E> using fp = fixed_point<T, E>;

template <int E> using fp_u8 = fixed_point<std::uint8_t, E>;
template <int E> using fp_u16 = fixed_point<std::uint16_t, E>;
template <int E> using fp_u32 = fixed_point<std::uint32_t, E>;
template <int E> using fp_u64 = fixed_point<std::uint64_t, E>;

template <int E> using fp_s8 = fixed_point<std::int8_t, E>;
template <int E> using fp_s16 = fixed_point<std::int16_t, E>;
template <int E> using fp_s32 = fixed_point<std::int32_t, E>;
template <int E> using fp_s64 = fixed_point<std::int64_t, E>;

// exponent == 0
static_assert(fp_u8<0>(12.34f).get<float>() == 12.f, "crag::core::fixed_point test failed");
static_assert(fp_u16<0>(12.34f).get<double>() == 12.f, "crag::core::fixed_point test failed");
static_assert(fp_u32<0>(12.34f).get<long double>() == 12.f, "crag::core::fixed_point test failed");
static_assert(fp_u64<0>(12.34f).get<float>() == 12.f, "crag::core::fixed_point test failed");

static_assert(fp_s8<0>(-12.34f).get<double>() == -12.f, "crag::core::fixed_point test failed");
static_assert(fp_s16<0>(-12.34f).get<long double>() == -12.f, "crag::core::fixed_point test failed");
static_assert(fp_s32<0>(-12.34f).get<float>() == -12.f, "crag::core::fixed_point test failed");
static_assert(fp_s64<0>(-12.34f).get<double>() == -12.f, "crag::core::fixed_point test failed");

// exponent == -7
static_assert(fp_u8<-7>(.125f).get<float>() == .125f, "crag::core::fixed_point test failed");
static_assert(fp_u16<-7>(232.125f).get<float>() == 232.125f, "crag::core::fixed_point test failed");
static_assert(fp_u32<-7>(232.125f).get<float>() == 232.125f, "crag::core::fixed_point test failed");
static_assert(fp_u64<-7>(232.125f).get<float>() == 232.125f, "crag::core::fixed_point test failed");

static_assert(fp_s8<-7>(.125f).get<float>() == .125f, "crag::core::fixed_point test failed");
static_assert(fp_s16<-7>(123.125f).get<float>() == 123.125f, "crag::core::fixed_point test failed");
static_assert(fp_s32<-7>(123.125f).get<float>() == 123.125f, "crag::core::fixed_point test failed");
static_assert(fp_s64<-7>(123.125f).get<float>() == 123.125f, "crag::core::fixed_point test failed");

static_assert(fp_u8<-7>(.125f).get<double>() == .125f, "crag::core::fixed_point test failed");
static_assert(fp_u16<-7>(232.125f).get<long double>() == 232.125f, "crag::core::fixed_point test failed");
static_assert(fp_u32<-7>(232.125f).get<double>() == 232.125f, "crag::core::fixed_point test failed");
static_assert(fp_u64<-7>(232.125f).get<long double>() == 232.125f, "crag::core::fixed_point test failed");

static_assert(fp_s8<-7>(1).get<long double>() != 1.f, "crag::core::fixed_point test failed");
static_assert(fp_s8<-7>(.5).get<float>() == .5f, "crag::core::fixed_point test failed");
static_assert(fp_s8<-7>(.125f).get<long double>() == .125f, "crag::core::fixed_point test failed");
static_assert(fp_s16<-7>(123.125f).get<int>() == 123, "crag::core::fixed_point test failed");
static_assert(fp_s32<-7>(123.125f).get<long double>() == 123.125f, "crag::core::fixed_point test failed");
static_assert(fp_s64<-7>(123.125l).get<float>() == 123.125f, "crag::core::fixed_point test failed");

// exponent == 16
static_assert(fp_u8<16>(65536).get<float>() == 65536.f, "crag::core::fixed_point test failed");
static_assert(fp_u16<16>(6553.).get<int>() == 0, "crag::core::fixed_point test failed");
static_assert(fp_u32<16>(4294967296l).get<double>() == 4294967296.f, "crag::core::fixed_point test failed");
static_assert(fp_u64<16>(1125895611875328l).get<double>() == 1125895611875328l, "crag::core::fixed_point test failed");

static_assert(fp_s8<16>(-65536).get<float>() == -65536.f, "crag::core::fixed_point test failed");
static_assert(fp_s16<16>(-6553.).get<int>() == 0, "crag::core::fixed_point test failed");
static_assert(fp_s32<16>(-4294967296l).get<double>() == -4294967296.f, "crag::core::fixed_point test failed");
static_assert(fp_s64<16>(-1125895611875328l).get<double>() == -1125895611875328l, "crag::core::fixed_point test failed");

// closed_unit
template <typename T> using cu = closed_unit<T>;
static_assert(cu<std::uint8_t>(.5).get<double>() == .5, "crag::core::closed_unit test failed");
static_assert(cu<std::uint16_t>(.125f).get<float>() == .125f, "crag::core::closed_unit test failed");
static_assert(cu<std::uint16_t>(.640625l).get<float>() == .640625, "crag::core::closed_unit test failed");
static_assert(cu<std::uint16_t>(1.640625).get<float>() == 1.640625f, "crag::core::closed_unit test failed");
static_assert(cu<std::uint16_t>(1u).get<std::uint64_t>() == 1, "crag::core::closed_unit test failed");
static_assert(cu<std::uint16_t>(2).get<float>() != 2, "crag::core::closed_unit test failed");	// out-of-range test

// open_unit
template <typename T> using ou = open_unit<T>;
static_assert(ou<std::uint8_t>(.5).get<double>() == .5, "crag::core::closed_unit test failed");
static_assert(ou<std::uint16_t>(.125f).get<float>() == .125f, "crag::core::closed_unit test failed");
static_assert(ou<std::uint16_t>(.640625l).get<float>() == .640625, "crag::core::closed_unit test failed");
static_assert(ou<std::uint16_t>(1).get<float>() == 0, "crag::core::closed_unit test failed");	// dropped bit

////////////////////////////////////////////////////////////////////////////////
// crag::core::lerp

static_assert(lerp(fp_u8<-7>(1), fp_u8<-7>(0), .5).get<float>() == .5f, "crag::core::lerp test failed");
static_assert(lerp(fp_u16<-7>(.125), fp_u16<-7>(.625), .5).get<float>() == .375f, "crag::core::lerp test failed");
static_assert(lerp(fp_u32<-16>(42123.51323), fp_u32<-16>(432.9191), .812).get<unsigned>() == 8270, "crag::core::lerp test failed");

static_assert(lerp(fp_s8<-6>(1), fp_s8<-6>(0), .5).get<float>() == .5f, "crag::core::lerp test failed");
static_assert(lerp(fp_s16<-10>(.125), fp_s16<-10>(.625), .5).get<float>() == .375f, "crag::core::lerp test failed");
static_assert(lerp(fp_s32<-6>(.125), fp_s32<-6>(.625), .25).get<float>() == .25f, "crag::core::lerp test failed");
