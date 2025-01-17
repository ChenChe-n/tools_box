#pragma once

#include "./../../../tools/base.hpp"


#include <iostream>
#include <thread>
#include <bitset>

namespace tools {
    namespace big_int {
        // 将十进制大数转换为二进制，按 32 位存储
        std::vector<u32> decimal_to_binary(const std::string& decimal);

        // 将二进制（按 32 位块存储）转换为十进制字符串
        std::string binary_to_decimal(const std::vector<u32>& binary);

        // 判断字符串是否为有效大数
        bool is_valid_number(const std::string& str);

    }
}