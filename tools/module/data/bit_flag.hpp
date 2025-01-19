#pragma once
#include "../../base.hpp"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

namespace tools::data {

    template <u64 T>
    class bit_flag {
        static_assert(T > 0, "Template parameter T must be greater than 0.");

    public:
        constexpr bit_flag() : flag_{} {}

        // 通用：设置特定位的值（同时支持编译期和运行时）
        constexpr void set(u64 Bit, bool flag) {
            if (Bit >= (flag_.size() * 8)) {
                // 运行时检测越界
                throw std::out_of_range("Bit index out of range!");
            }

            // 计算索引
            u64 byte_index = Bit / 8;
            u8 bit_offset = Bit % 8;

            // 设置位值
            if (flag) {
                flag_[byte_index] |= (1 << bit_offset);
            }
            else {
                flag_[byte_index] &= ~(1 << bit_offset);
            }
        }

        // 编译期专用（模板版本）：调用通用 set()
        template <u64 Bit>
        constexpr void set(bool flag) {
            static_assert(Bit < (T * 8), "Bit index out of range!"); // 编译期越界检查
            set(Bit, flag);                                         // 调用通用版本
        }

        // 通用：获取特定位的值
        constexpr bool get(u64 Bit) const {
            if (Bit >= (flag_.size() * 8)) {
                // 运行时检测越界
                throw std::out_of_range("Bit index out of range!");
            }

            // 计算索引
            u64 byte_index = Bit / 8;
            u8 bit_offset = Bit % 8;

            // 返回特定位值
            return (flag_[byte_index] & (1 << bit_offset)) != 0;
        }

        // 编译期专用（模板版本）：调用通用 get()
        template <u64 Bit>
        constexpr bool get() const {
            static_assert(Bit < (T * 8), "Bit index out of range!"); // 编译期越界检查
            return get(Bit);                                        // 调用通用版本
        }

        // 清除所有位（置0）
        constexpr void clear() {
            for (auto& byte : flag_) {
                byte = 0;
            }
        }

    private:
        std::array<u8, T> flag_; // 固定大小数组，存储每个位的状态
    };



}