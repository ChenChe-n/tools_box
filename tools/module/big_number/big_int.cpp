#include "big_int.hpp"
#include "input_out.hpp"


namespace tools::big_int {
    constexpr u32 u32_max = 0xFFFFFFFF;

    big_int::big_int() {
    }

    big_int::big_int(const big_int& num) {
        this->state = num.state;
        this->data = num.data;
        this->max_length = num.max_length;
    }

    big_int::big_int(big_int&& num) noexcept {
        // 移动数据
        this->state = num.state;
        this->data = std::move(num.data);
        this->max_length = num.max_length;
        // 清空数据
        num.state = sign_state::undefined;
        num.data = { 0 };
        num.max_length = num.max_length;
    }

    big_int::big_int(const std::string& num) {
        // 处理非数字或未定义状态
        if (!is_valid_number(num)) {
            this->data = { 0 };
            this->state = sign_state::undefined;
            return;
        }
        // 处理正常字符串
        if (num[0] == '-') {
            this->state = sign_state::negative;
            this->data = decimal_to_binary(num.substr(1));
        }
        else if (num[0] == '+') {
            this->state = sign_state::positive;
            this->data = decimal_to_binary(num.substr(1));
        }
        else {
            this->state = sign_state::positive;
            this->data = decimal_to_binary(num);
        }
        format();
    }

    big_int::big_int(i64 num) {
        data = { u32(std::abs(num) & u32_max), u32(std::abs(num) >> 32) };
        if (num < 0) {
            state = sign_state::negative;
        }
        format();
    }

    big_int& big_int::operator=(const big_int& other) {
        if (this != &other) {  // 防止自我赋值
            this->data = other.data;
            this->state = other.state;
            this->max_length = other.max_length;
        }
        return *this;
    }

    big_int& big_int::operator=(big_int&& other) noexcept {
        if (this != &other) {  // 防止自我赋值
            this->data = std::move(other.data);
            this->state = other.state;
            this->max_length = other.max_length;
        }
        return *this;
    }

    std::string big_int::to_string() const
    {
        switch (this->state)
        {
        case sign_state::positive:
            return "+" + binary_to_decimal(this->data);
            break;
        case sign_state::negative:
            return "-" + binary_to_decimal(this->data);
            break;
        case sign_state::positive_overflow:
            return "positive_overflow";
            break;
        case sign_state::negative_overflow:
            return "negative_overflow";
            break;
        case sign_state::not_a_number:
            return "not_a_number";
            break;
        case sign_state::undefined:
            return "undefined";
        default:
            return "undefined";
            break;
        }
        return "undefined";
    }

    std::string big_int::to_bit() const
    {
        std::ostringstream ss;
        bool leading_zeros = true;
        switch (this->state)
        {
        case sign_state::positive:
        case sign_state::negative:
            for (auto it = this->data.rbegin(); it != this->data.rend(); ++it) {
                if (leading_zeros) {
                    std::bitset<32> bits(*it);
                    std::string bit_string = bits.to_string();
                    size_t first_one = bit_string.find('1');
                    if (first_one != std::string::npos) {
                        ss << bit_string.substr(first_one);
                        leading_zeros = false;
                    }
                }
                else {
                    ss << std::bitset<32>(*it);
                }
            }
            if (leading_zeros) {
                ss << "0";
            }
            break;
        case sign_state::positive_overflow:
            ss << "positive_overflow";
            break;
        case sign_state::negative_overflow:
            ss << "negative_overflow";
            break;
        case sign_state::not_a_number:
            ss << "not_a_number";
            break;
        case sign_state::undefined:
            ss << "undefined";
            break;
        default:
            ss << "undefined";
            break;
        }
        return ss.str();
    }

    std::string big_int::to_hex() const
    {
        std::ostringstream ss;
        switch (this->state)
        {
        case sign_state::positive:
        case sign_state::negative:
            for (auto it = this->data.rbegin(); it != this->data.rend(); ++it) {
                ss << std::hex << (*it) << " ";
            }
            return ss.str();
            break;
        case sign_state::positive_overflow:
            ss << "positive_overflow";
            break;
        case sign_state::negative_overflow:
            ss << "negative_overflow";
            break;
        case sign_state::not_a_number:
            ss << "not_a_number";
            break;
        case sign_state::undefined:
            ss << "undefined";
            break;
        default:
            ss << "undefined";
            break;
        }
        return ss.str();

    }

    std::string big_int::to_u32() const
    {
        std::ostringstream ss;
        switch (this->state)
        {
        case sign_state::positive:
        case sign_state::negative:
            for (auto it = this->data.rbegin(); it != this->data.rend(); ++it) {
                ss << (*it) << " ";
            }
            break;
        case sign_state::positive_overflow:
            ss << "positive_overflow";
            break;
        case sign_state::negative_overflow:
            ss << "negative_overflow";
            break;
        case sign_state::not_a_number:
            ss << "not_a_number";
            break;
        case sign_state::undefined:
            ss << "undefined";
            break;
        default:
            ss << "undefined";
            break;
        }

        return ss.str();
    }

    big_int big_int::operator-() const
    {
        big_int result = *this;
        // 处理非数字或未定义状态
        if (error(result)) {
            return result;
        }
        // 返回相反数
        result.state = (this->state == sign_state::positive) ? sign_state::negative : sign_state::positive;
        return result;
    }

    // 封装 + 运算符
    big_int big_int::operator+(const big_int& other) const {
        big_int result;
        // 处理非数字或未定义状态
        if (error(result, other)) {
            return result;
        }
        if (this->state == other.state) {
            // 同号，直接做绝对值加法
            result = abs_add(*this, other);
            result.state = this->state; // 保持符号
        }
        else {
            // 异号，做绝对值减法，结果符号取决于绝对值较大者
            if (this->abs_greater(other)) {
                result = abs_sub(*this, other);
                result.state = this->state; // 保持符号
            }
            else {
                result = abs_sub(other, *this);
                result.state = other.state; // 保持符号
            }
        }

        return result;
    }
    // 封装 - 运算符
    big_int big_int::operator-(const big_int& other) const {
        big_int result;
        // 处理非数字或未定义状态
        if (error(result, other)) {
            return result;
        }

        if (this->state != other.state) {
            // 异号，转为绝对值加法
            result = abs_add(*this, other);
            result.state = this->state; // 保持符号
        }
        else {
            // 同号，做绝对值减法，结果符号取决于绝对值较大者
            if (this->abs_greater(other)) {
                result = abs_sub(*this, other);
                result.state = this->state; // 保持符号
            }
            else {
                result = abs_sub(other, *this);
                result.state = (this->state == sign_state::positive) ? sign_state::negative : sign_state::positive;
            }
        }

        return result;
    }
    // 封装 * 运算符
    big_int big_int::operator*(const big_int& other) const
    {
        big_int result;
        // 处理非数字或未定义状态
        if (error(result, other)) {
            return result;
        }

        // 符号处理
        result.state = (this->state == other.state) ? sign_state::positive : sign_state::negative;

        // 初始化结果数据
        u64 out_length = this->data.size() + other.data.size();
        result.data.resize(out_length, 0);

        // 处理溢出
        if (out_length > this->max_length) {
            if (result.state == sign_state::positive)
            {
                result.state = sign_state::positive_overflow;
            }
            else
            {
                result.state = sign_state::negative_overflow;
            }
            return result;
        }

        // 大整数乘法逻辑
        for (size_t i = 0; i < this->data.size(); ++i) {
            u64 carry = 0; // 进位
            for (size_t j = 0; j < other.data.size(); ++j) {
                // 乘积 + 当前位 + 进位
                u64 product = static_cast<u64>(this->data[i]) * static_cast<u64>(other.data[j]) +
                    static_cast<u64>(result.data[i + j]) + carry;

                result.data[i + j] = static_cast<u32>(product & u32_max); // 低 32 位
                carry = product >> 32; // 高 32 位为进位
            }

            // 处理最后的进位
            if (carry > 0) {
                result.data[i + other.data.size()] += static_cast<u32>(carry);
            }
        }

        // 格式化结果（去除多余的高位 0）
        result.format();
        return result;
    }
    // 封装 / 运算符
    big_int big_int::operator/(const big_int& other) const
    {
        big_int remainder{ 0 };
        return division(*this, other, remainder);
    }
    // 封装 % 运算符
    big_int big_int::operator%(const big_int& other) const
    {
        big_int remainder{ 0 };
        division(*this, other, remainder);
        return remainder;
    }

    // 运算符重载
    big_int big_int::operator+=(const big_int& other)
    {
        *this = (*this + other);
        return *this;
    }
    big_int big_int::operator-=(const big_int& other)
    {
        *this = (*this - other);
        return *this;
    }
    big_int big_int::operator*=(const big_int& other)
    {
        *this = (*this * other);
        return *this;
    }
    big_int big_int::operator/=(const big_int& other)
    {
        *this = (*this / other);
        return *this;
    }
    big_int big_int::operator%=(const big_int& other)
    {
        *this = (*this % other);
        return *this;
    }
    // 比较运算
    bool big_int::operator==(const big_int& other) const {
        // 符号和数据必须都相同
        return this->state == other.state && this->data == other.data;
    }

    bool big_int::operator!=(const big_int& other) const {
        return !(*this == other);
    }

    bool big_int::operator>(const big_int& other) const {
        // 处理非数字或未定义状态
        big_int tmp = {};
        if (error(tmp, other)) {
            return false;
        }

        // 根据符号状态进行判断
        if (this->state == sign_state::positive) {
            if (other.state == sign_state::negative) return true;  // 正 > 负
            if (other.state == sign_state::positive) return abs_greater(other);  // 同为正数
        }
        else if (this->state == sign_state::negative) {
            if (other.state == sign_state::positive) return false;  // 负 < 正
            if (other.state == sign_state::negative) return abs_less(other);  // 同为负数
        }

        // 默认返回 false（不应到达此处）
        return false;
    }

    bool big_int::operator<(const big_int& other) const
    {
        // 处理非数字或未定义状态
        big_int tmp = {};
        if (error(tmp, other)) {
            return false;
        }

        // 根据符号状态进行判断
        if (this->state == sign_state::positive) {
            if (other.state == sign_state::negative) return false;  // 正 > 负
            if (other.state == sign_state::positive) return abs_less(other);  // 同为正数
        }
        else if (this->state == sign_state::negative) {
            if (other.state == sign_state::positive) return true;  // 负 < 正
            if (other.state == sign_state::negative) return abs_greater(other);  // 同为负数
        }

        // 默认返回 false（不应到达此处）
        return false;
    }

    bool big_int::operator>=(const big_int& other) const
    {
        // 处理非数字或未定义状态
        big_int tmp = {};
        if (error(tmp, other)) {
            return false;
        }
        return !(*this < other);
    }

    bool big_int::operator<=(const big_int& other) const
    {
        // 处理非数字或未定义状态
        big_int tmp = {};
        if (error(tmp, other)) {
            return false;
        }
        return !(*this > other);
    }

    big_int big_int::operator<<(u64 shift) const {
        // 提前分配空间
        big_int result;

        // 错误处理
        if (error(result)) {
            return result;
        }

        u64 block_shift = shift / 32;
        u64 bit_shift = shift % 32;

        // 溢出检查
        if (data.size() + block_shift > max_length / 32) {
            big_int overflow_result = *this;
            overflow_result.state = (state == sign_state::negative) ? sign_state::negative_overflow : sign_state::positive_overflow;
            return overflow_result;
        }

        result.data.resize(data.size() + block_shift, 0);

        // 位移操作
        u32 carry = 0;
        for (size_t i = 0; i < data.size(); ++i) {
            u64 shifted_value = static_cast<u64>(data[i]) << bit_shift | carry;
            result.data[block_shift + i] = static_cast<u32>(shifted_value & 0xFFFFFFFF);
            carry = static_cast<u32>(shifted_value >> 32);
        }

        // 添加可能的进位
        if (carry != 0) {
            result.data.push_back(carry);
        }

        result.format();
        return result;
    }

    big_int big_int::operator>>(u64 shift) const {
        // 提前分配空间
        big_int result;

        // 错误处理
        if (error(result)) {
            return result;
        }

        u64 block_shift = shift / 32;
        u64 bit_shift = shift % 32;

        // 如果移位超出数据总长度
        if (block_shift >= data.size()) {
            big_int result;
            result.data = { 0 };
            result.state = sign_state::positive;
            return result;
        }

        result.data.resize(data.size() - block_shift);

        // 位移操作
        u32 carry = 0;
        for (size_t i = data.size(); i > block_shift; --i) {
            size_t current_index = i - 1;
            u64 shifted_value = (static_cast<u64>(data[current_index]) >> bit_shift) | (static_cast<u64>(carry) << (32 - bit_shift));
            result.data[current_index - block_shift] = static_cast<u32>(shifted_value);
            carry = static_cast<u32>(data[current_index] & ((1ULL << bit_shift) - 1));
        }

        result.format();
        return result;
    }

    big_int big_int::operator<<=(u64 shift)
    {
        *this = (*this << shift);
        return *this;
    }
    big_int big_int::operator>>=(u64 shift)
    {
        *this = (*this >> shift);
        return *this;
    }

    // 辅助方法
    // 大整数绝对值加法
    big_int big_int::abs_add(const big_int& a, const big_int& b) {
        big_int result;
        // 处理非数字或未定义状态
        if (a.error(result, b)) {
            return result;
        }

        result.data.clear(); // 确保结果数据为空

        const size_t max_size = std::max(a.data.size(), b.data.size());
        result.data.resize(max_size, 0);

        u64 carry = 0; // 进位
        for (size_t i = 0; i < max_size; ++i) {
            u64 sum = carry;
            if (i < a.data.size()) {
                sum += a.data[i];
            }
            if (i < b.data.size()) {
                sum += b.data[i];
            }

            result.data[i] = static_cast<u32>(sum & 0xFFFFFFFF); // 保留低 32 位
            carry = sum >> 32; // 提取高位作为进位
        }

        // 如果最后有进位，添加一个新的块
        if (carry > 0) {
            result.data.push_back(static_cast<u32>(carry));
        }

        result.format(); // 格式化数据
        return result;
    }

    // 大整数绝对值减法
    big_int big_int::abs_sub(const big_int& a, const big_int& b) {
        // 假设 a >= b (调用前需要保证，否则行为未定义)
        big_int result = a;
        // 处理非数字或未定义状态
        if (a.error(result, b)) {
            return result;
        }

        u64 borrow = 0; // 借位
        for (size_t i = 0; i < a.data.size(); ++i) {
            u64 sub = static_cast<u64>(a.data[i]) - borrow;
            if (i < b.data.size()) {
                sub -= b.data[i];
            }

            if (sub >= (1ULL << 32)) { // 借位
                sub += (1ULL << 32);
                borrow = 1;
            }
            else {
                borrow = 0;
            }

            result.data[i] = static_cast<u32>(sub);
        }

        // 如果 a 的长度大于 b，确保借位正确传播到剩余块
        for (size_t i = b.data.size(); i < a.data.size() && borrow > 0; ++i) {
            u64 sub = static_cast<u64>(a.data[i]) - borrow;
            if (sub >= (1ULL << 32)) {
                sub += (1ULL << 32);
                borrow = 1;
            }
            else {
                borrow = 0;
            }
            result.data[i] = static_cast<u32>(sub);
        }

        // 最终借位应为 0，若不为 0，则说明调用时未保证 a >= b
        if (borrow > 0) {
            throw std::runtime_error("abs_sub called with a < b, which is undefined behavior");
        }

        result.format(); // 格式化数据
        return result;
    }

    // 大整数除法
    big_int big_int::division(const big_int& a, const big_int& b, big_int& remainder) {
        big_int result;

        // 处理非数字或未定义状态
        if (a.error(result, b)) {
            return result;
        }

        // 处理除 0
        if (b.data.size() == 1 && b.data[0] == 0) {
            result.state = sign_state::not_a_number;
            return result;
        }

        // 处理 abs(a) == abs(b)
        if (a.data == b.data) {
            remainder = 0;
            result = 1;
            if (a.state == b.state) {
                result.state = sign_state::positive;
            }
            else {
                result.state = sign_state::negative;
            }
            return result; // 结果为 1
        }

        // 处理 a < b
        if (a < b) {
            remainder = a;
            result = 0;
            return result; // 结果为 1
        }


        // 被除数
        remainder = a;
        // 临时除数
        big_int temp = b;
        // 临时倍率
        big_int temp_multiplier = big_int(1);

        // 快速左移
        if (remainder.data.size() > temp.data.size() + 1) {
            size_t shift_blocks = (remainder.data.size() - temp.data.size() - 1);
            temp <<= shift_blocks * 32;
            temp_multiplier <<= shift_blocks * 32;
        }


        // 循环左移，直到 temp >= a
        while (temp < remainder) {
            temp <<= 1;
            temp_multiplier <<= 1;
        }

        // 右移到恰好小于等于 remainder
        temp >>= 1;
        temp_multiplier >>= 1;

        // 开始除法
        while ((temp_multiplier >= big_int(1))) {
            // 输出调试信息
            //std::cout << "temp: " << temp.to_string() << std::endl;
            //std::cout << "temp_multiplier: " << temp_multiplier.to_string() << std::endl;
            //std::cout << "remainder: " << remainder.to_string() << std::endl;
            //std::cout << "result: " << result.to_string() << std::endl;

            if (remainder >= temp) {
                remainder -= temp;
                result += temp_multiplier;
            }

            temp >>= 1;
            temp_multiplier >>= 1;
        }

        // 调整符号
        if ((a.state == sign_state::negative) != (b.state == sign_state::negative)) {
            result.state = sign_state::negative;
        }
        remainder.state = a.state;

        return result;
    }

    big_int big_int::abs(const big_int& num)
    {
        big_int result = num;

        // 处理非数字或未定义状态
        if (result.error(result)) {
            return result;
        }

        // 绝对值
        result.state = sign_state::positive;
        return result;
    }


    bool big_int::abs_less(const big_int& other) const {
        const auto this_length = this->data.size();
        const auto other_length = other.data.size();

        // 1. 比较长度
        if (this_length != other_length) {
            return this_length < other_length;
        }

        // 2. 比较数据（从高位到低位）
        const auto& this_data = this->data;
        const auto& other_data = other.data;

        for (i64 i = this_length - 1; i >= 0; --i) {
            if (this_data[i] != other_data[i]) {
                return this_data[i] < other_data[i];
            }
        }

        // 3. 数据完全相等
        return false;
    }

    bool big_int::abs_greater(const big_int& other) const {
        u64 this_length = this->data.size();
        u64 other_length = other.data.size();

        // 比较长度
        if (this_length != other_length) {
            return this_length > other_length;
        }

        // 长度相同，从高位向低位逐位比较
        for (i64 i = this_length - 1; i >= 0; --i) {
            if (this->data[i] != other.data[i]) {
                return this->data[i] > other.data[i];
            }
        }

        // 如果完全相等，返回 false
        return false;
    }

    // 格式化函数
    void big_int::format() {
        //  移除高位的多余 0
        while (data.size() > 1 && data.back() == 0) {
            data.pop_back();
        }

        // 如果数据为零，确保符号为正
        if (data.size() == 1 && data[0] == 0) {
            state = sign_state::positive;
        }
    }

    // 如果 this为 非数，返回 true
    bool big_int::error(big_int& out) const
    {
        if (this->state == sign_state::not_a_number) {
            out.state = sign_state::not_a_number;
            out.data = { 0 };
            return true;
        }
        else if (this->state == sign_state::undefined) {
            out.state = sign_state::undefined;
            out.data = { 0 };
            return true;
        }
        else if (this->state == sign_state::negative_overflow) {
            out.state = sign_state::negative_overflow;
            out.data = { 0 };
            return true;
        }
        else if (this->state == sign_state::positive_overflow) {
            out.state = sign_state::positive_overflow;
            out.data = { 0 };
            return true;
        }
        return false;
    }

    // 如果 this 和 other 其一为 非数，返回 true
    bool big_int::error(big_int& out, const big_int& other) const
    {
        if (this->state == sign_state::undefined or other.state == sign_state::undefined) {
            out.state = sign_state::undefined;
            out.data = { 0 };
            return true;
        }
        else if (this->state == sign_state::not_a_number or other.state == sign_state::not_a_number) {
            out.state = sign_state::not_a_number;
            out.data = { 0 };
            return true;
        }
        else if (this->state == sign_state::negative_overflow or other.state == sign_state::negative_overflow) {
            out.state = sign_state::negative_overflow;
            out.data = { 0 };
            return true;
        }
        else if (this->state == sign_state::positive_overflow or other.state == sign_state::positive_overflow) {
            out.state = sign_state::positive_overflow;
            out.data = { 0 };
            return true;
        }
        return false;
    }

}