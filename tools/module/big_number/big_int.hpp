#pragma once

#include "./../../../tools/base.hpp"

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

    // 从字符串构造
    big_int(const std::string& num);

    // 从数字构造
    big_int(u64 num);
    big_int(i64 num);

private:
    std::vector<u32> data = {0};            // 数据
    sign_state state = sign_state::positive;// 状态
    u64 max_bits = 16384;                   // 最大长度
};
