#pragma once
#include "using.hpp"

#include <limits>
// 整数类型
constexpr i8 i8_max = std::numeric_limits<i8>::max();
constexpr i8 i8_min = std::numeric_limits<i8>::min();
constexpr u8 u8_max = std::numeric_limits<u8>::max();
constexpr u8 u8_min = std::numeric_limits<u8>::min();

constexpr i16 i16_max = std::numeric_limits<i16>::max();
constexpr i16 i16_min = std::numeric_limits<i16>::min();
constexpr u16 u16_max = std::numeric_limits<u16>::max();
constexpr u16 u16_min = std::numeric_limits<u16>::min();

constexpr i32 i32_max = std::numeric_limits<i32>::max();
constexpr i32 i32_min = std::numeric_limits<i32>::min();
constexpr u32 u32_max = std::numeric_limits<u32>::max();
constexpr u32 u32_min = std::numeric_limits<u32>::min();

constexpr i64 i64_max = std::numeric_limits<i64>::max();
constexpr i64 i64_min = std::numeric_limits<i64>::min();
constexpr u64 u64_max = std::numeric_limits<u64>::max();
constexpr u64 u64_min = std::numeric_limits<u64>::min();


// 浮点类型
constexpr f32 f32_max = std::numeric_limits<f32>::max();
constexpr f32 f32_min = std::numeric_limits<f32>::lowest();

constexpr f64 f64_max = std::numeric_limits<f64>::max();
constexpr f64 f64_min = std::numeric_limits<f64>::lowest();