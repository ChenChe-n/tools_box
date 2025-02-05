#pragma once

#include "../../base.hpp"

#include <vector>
#include <sstream>


namespace tools::big_int {
    // 大整数类
    // 存储2^32进制的数据
    class big_int {
    public:
        enum class sign_state : u8 {
            positive,            // 正数
            negative,            // 负数
            positive_overflow,   // 正溢出
            negative_overflow,   // 负溢出
            not_a_number,        // 非数字
            undefined            // 未定义
        };

        // 默认构造为0
        big_int();

        // 从 big_int 拷贝
        big_int(const big_int& num);

        // 从 big_int 移动
        big_int(big_int&& num) noexcept;

        // 从字符串构造
        big_int(const std::string& num);

        // 从数字构造
        big_int(i64 num);

        // 赋值
        big_int& operator=(const big_int& other);
        big_int& operator=(big_int&& other) noexcept;

        // 转换为字符串
        std::string to_string() const;
        std::string to_bit() const;
        std::string to_hex() const;
        std::string to_u32() const;

        // 单目运算符
        big_int operator-() const;

        // 四则运算
        big_int operator+(const big_int& other) const;
        big_int operator-(const big_int& other) const;
        big_int operator*(const big_int& other) const;
        big_int operator/(const big_int& other) const;
        big_int operator%(const big_int& other) const;

        big_int operator+=(const big_int& other);
        big_int operator-=(const big_int& other);
        big_int operator*=(const big_int& other);
        big_int operator/=(const big_int& other);
        big_int operator%=(const big_int& other);

        static big_int division(const big_int& a, const big_int& b, big_int& remainder);
        static big_int abs(const big_int& num);

        // 比较运算
        bool operator==(const big_int& other) const;
        bool operator!=(const big_int& other) const;

        bool operator>(const big_int& other) const;
        bool operator<(const big_int& other) const;

        bool operator>=(const big_int& other) const;
        bool operator<=(const big_int& other) const;

        // 位运算
        big_int operator<<(u64 shift) const;
        big_int operator>>(u64 shift) const;

        big_int operator<<=(u64 shift);
        big_int operator>>=(u64 shift);
    private:
        std::vector<u32> data = { 0 }; // 数据存储 (32 位为一个块)
        sign_state state = sign_state::positive; // 符号状态
        u64 max_length = 0xffffffff; // 最大长度
    private:
        // 辅助方法
        static big_int abs_add(const big_int& a, const big_int& b);
        static big_int abs_sub(const big_int& a, const big_int& b);
        bool abs_less(const big_int& other) const;
        bool abs_greater(const big_int& other) const;
        void format();
        bool error(big_int& out) const;
        bool error(big_int& out, const big_int& other) const;
    };
}