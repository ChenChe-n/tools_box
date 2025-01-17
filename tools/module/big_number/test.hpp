#pragma once
#include <cassert>
#include "./../../../tools/module/big_number/big_int.hpp"

namespace tools::big_int {

    void test_addition() {
        // 测试相同符号的加法
        big_int a("12345678901234567890");
        big_int b("98765432109876543210");
        assert((a + b).to_string() == "+111111111011111111100");

        big_int c("-12345678901234567890");
        big_int d("-98765432109876543210");
        assert((c + d).to_string() == "-111111111011111111100");

        // 测试不同符号的加法
        big_int e("12345678901234567890");
        big_int f("-98765432109876543210");
        assert((e + f).to_string() == "-86419753208641975320");

        big_int g("98765432109876543210");
        big_int h("-12345678901234567890");
        assert((g + h).to_string() == "+86419753208641975320");

        std::cout << "Addition tests passed!\n";
    }

    void test_subtraction() {
        // 测试相同符号的减法
        big_int a("12345678901234567890");
        big_int b("98765432109876543210");
        assert((b - a).to_string() == "+86419753208641975320");

        big_int c("-98765432109876543210");
        big_int d("-12345678901234567890");
        assert((c - d).to_string() == "-86419753208641975320");

        // 测试不同符号的减法
        big_int e("12345678901234567890");
        big_int f("-98765432109876543210");
        assert((e - f).to_string() == "+111111111011111111100");

        big_int g("98765432109876543210");
        big_int h("-12345678901234567890");
        assert((g - h).to_string() == "+111111111011111111100");

        std::cout << "Subtraction tests passed!\n";
    }

    void test_edge_cases() {
        // 零值加减
        big_int zero("0");
        big_int a("12345678901234567890");
        assert((a + zero).to_string() == "+12345678901234567890");
        assert((a - zero).to_string() == "+12345678901234567890");
        assert((zero - a).to_string() == "-12345678901234567890");

        // 自身减自身
        assert((a - a).to_string() == "+0");

        // 大小数相减
        big_int b("1000000000000000000000000000000");
        big_int c("222222222222222222222222222");
        assert((b - c).to_string() == "+977777777777777777777777777778");

        big_int d("777777777777777777777777778");
        assert((b - d).to_string() == "+999222222222222222222222222222");

        std::cout << "Edge case tests passed!\n";
    }

    int test_input_out() {
        for (size_t i = 1; i < 15; i++) {
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

    int test_all() {
        test_addition();
        test_subtraction();
        test_edge_cases();
        test_input_out();

        std::cout << "All tests passed!\n";
        return 0;
    }
}