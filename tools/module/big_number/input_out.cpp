#include "input_out.hpp"
#include "big_int.hpp"



namespace tools::big_int {
    std::vector<u32> decimal_to_binary(const std::string& decimal) {
        // 将十进制大数存储为逆序的 9 位块（以 10^9 为单位）
        const u64 BASE = 1000000000;  // 每块存储 9 位十进制数
        std::vector<u64> blocks;

        // 将十进制字符串分块存储
        for (i64 i = decimal.size(); i > 0; i -= 9) {
            i64 start = std::max(i64(0), i - 9);
            blocks.push_back(std::stoull(decimal.substr(start, i - start)));
        }

        std::vector<u32> binary_result; // 存储二进制结果（按 32 位块）

        u64 current_block = 0;    // 当前 32 位块
        int current_bit_count = 0; // 当前块的已填充位数

        // 持续进行高精度除法，直到所有块被处理完
        while (!blocks.empty()) {
            u64 remainder = 0;

            // 按高精度处理每个块
            for (i64 i = blocks.size() - 1; i >= 0; --i) {
                u64 current = remainder * BASE + blocks[i];
                blocks[i] = current >> 1;  // 商作为新的块值
                remainder = current & 1; // 更新余数
            }

            // 将余数（当前位）存入当前块
            current_block |= (remainder << current_bit_count);
            current_bit_count++;

            // 当前块填满 32 位后，存储并重置
            if (current_bit_count == 32) {
                binary_result.push_back(static_cast<u32>(current_block));
                current_block = 0;
                current_bit_count = 0;
            }

            // 删除所有前导零块
            while (!blocks.empty() && blocks.back() == 0) {
                blocks.pop_back();
            }
        }

        // 处理剩余未填满的块
        if (current_bit_count > 0) {
            binary_result.push_back(static_cast<u32>(current_block));
        }

        return binary_result;
    }

    std::string binary_to_decimal(const std::vector<u32>& binary) {
        if (binary.empty()) return "0";
        if (binary.size() == 1 and binary[0] == 0) return "0";
        const u64 BASE = 1000000000; // 每块存储 9 位十进制数
        std::vector<u64> decimal_blocks; // 存储十进制大数的块（逆序）

        // 遍历每个 32 位块
        for (auto it = binary.rbegin(); it != binary.rend(); ++it) {
            u64 carry = *it; // 当前块作为进位加入十进制

            // 更新现有十进制块
            for (size_t i = 0; i < decimal_blocks.size(); ++i) {
                carry += decimal_blocks[i] * (1ULL << 32);
                decimal_blocks[i] = carry % BASE;
                carry /= BASE;
            }

            // 如果有剩余的进位，创建新块
            while (carry > 0) {
                decimal_blocks.push_back(carry % BASE);
                carry /= BASE;
            }
        }

        // 构建最终结果字符串
        std::string result = std::to_string(decimal_blocks.back());
        for (i64 i = decimal_blocks.size() - 2; i >= 0; --i) {
            std::string block = std::to_string(decimal_blocks[i]);
            result += std::string(9 - block.size(), '0') + block; // 填充零补齐块长度
        }

        return result;
    }

    bool is_valid_number(const std::string& str) {
        // 空字符串非法
        if (str.empty()) {
            return false;
        }

        // 第一个字符必须是数字或符号（+/-）
        size_t start = 0;
        if (str[0] == '+' || str[0] == '-') {
            if (str.size() == 1) {
                return false; // 只有符号没有数字
            }
            start = 1; // 跳过符号
        }

        // 检查后续字符是否全为数字
        for (size_t i = start; i < str.size(); ++i) {
            if (!std::isdigit(str[i])) {
                return false; // 非数字字符非法
            }
        }

        return true;
    }

}
