#include "./../../../tools/module/big_number/big_int.hpp"

#include <cmath>
namespace tools {
	namespace big_int {
		constexpr u32 u32_max = 0xFFFFFFFF;

		big_int::big_int()
		{

		}

		big_int::big_int(const std::string& num)
		{

		}

		big_int::big_int(u64 num)
		{
			data = { u32(num & u32_max), u32(num >> 32) };
		}

		big_int::big_int(i64 num)
		{
			data = { u32(abs(num) & u32_max), u32(abs(num) >> 32) };
			if (num < 0) {
				state = sign_state::negative;
			}
		}

		big_int big_int::operator+(const big_int& other) const
		{
			big_int big_num = this->data.size() >= other.data.size() ? (big_int)*this : (big_int)other;
			big_int* min_num = this->data.size() >= other.data.size() ? (big_int*)&other : (big_int*)this;

			for (auto i : min_num->data)
			{

			}


		}

		big_int big_int::operator-(const big_int& other) const
		{
			return big_int();
		}
	}
}