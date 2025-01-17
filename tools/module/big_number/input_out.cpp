#include "./../../../tools/module/big_number/big_int.hpp"
#include "input_out.hpp"



namespace tools {
    namespace big_int {
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


        void print_binary(const std::vector<u32>& binary) {
            bool leading_zeros = true;
            for (auto it = binary.rbegin(); it != binary.rend(); ++it) {
                if (leading_zeros) {
                    std::bitset<32> bits(*it);
                    std::string bit_string = bits.to_string();
                    size_t first_one = bit_string.find('1');
                    if (first_one != std::string::npos) {
                        std::cout << bit_string.substr(first_one);
                        leading_zeros = false;
                    }
                }
                else {
                    std::cout << std::bitset<32>(*it);
                }
            }
            if (leading_zeros) {
                std::cout << "0";
            }
            std::cout << std::endl;
        }


        int test() {
            for (size_t i = 1; i < 20; i++) {
                std::string decimal = "";  // 任意大十进制数
                for (size_t j = 0; j < pow(2, i); j++)
                {
                    decimal += ('0' + (rand() / 3333) % 10);
                }
                auto t1 = tools::time::time_now();
                auto binary = decimal_to_binary(decimal);
                auto t2 = tools::time::time_now();
                std::cout << "用时: " << (t2 - t1) << std::endl;
                std::cout << "每位用时: " << (t2 - t1) / decimal.size() << std::endl;
                //std::cout << "input: " << decimal << std::endl;
                std::cout << "input.size(): " << decimal.size() << std::endl;
                //std::cout << "out: "; print_binary(binary); std::cout << std::endl;
                std::cout << "out.size(): " << binary.size() << std::endl;
                std::cout << "exp 2^: " << i << std::endl;
                std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
                auto t3 = tools::time::time_now();
                auto decimal2 = binary_to_decimal(binary);
                auto t4 = tools::time::time_now();
                std::cout << "用时: " << (t4 - t3) << std::endl;
                std::cout << "每位用时: " << (t4 - t3) / decimal2.size() << std::endl;
                //std::cout << "input: ";  print_binary(binary); std::cout << std::endl;
                std::cout << "input.size(): " << binary.size() << std::endl;
                //std::cout << "out: " << decimal2 << std::endl;
                std::cout << "out.size(): " << decimal2.size() << std::endl;
                std::cout << "exp 2^: " << i << std::endl;
                std::cout << "input == out：" << (decimal == decimal2) << std::endl;
        
                std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl << std::endl << std::endl << std::endl;
            }
            return 0;
        }


    }
}
