#include "./../tools/module/big_number/test.hpp"


int main() {
	//tools::big_int::test_all();
	tools::big_int::big_int a = { "17285691565913756196519563975" };
    a = tools::big_int::big_int("141798415515");
    std::cout << a.to_string() << std::endl;
    std::cout << a.to_hex() << std::endl;
    std::cout << a.to_bit() << std::endl;
    std::cout << a.to_u32() << std::endl;
	return 0;
}