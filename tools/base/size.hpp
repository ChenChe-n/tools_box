#pragma once
#include "using.hpp"

#include <limits>

namespace tools::size {
    // 获取最大值
    template <typename T>
    constexpr T max() noexcept {
        return std::numeric_limits<T>::max();
    }
    // 获取最小值
    template <typename T>
    constexpr T min() noexcept {
        return std::numeric_limits<T>::lowest();
    }

	constexpr u64 k		= 1000;
	constexpr u64 ki	= 1024;
    constexpr u64 m		= k * k;
    constexpr u64 mi	= ki * ki;
    constexpr u64 g		= m * k;
    constexpr u64 gi	= mi * ki;
    constexpr u64 t		= g * k;
    constexpr u64 ti	= gi * ki;
    constexpr u64 p		= t * k;
    constexpr u64 pi	= ti * ki;
    constexpr u64 e		= p * k;
    constexpr u64 ei	= pi * ki;
    constexpr u64 z		= e * k;
    constexpr u64 zi	= ei * ki;
}