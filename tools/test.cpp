#include "./../tools/module/big_number.hpp"

void run_tests() {

}

int main() {
    auto t3 = tools::time::time_now();

    run_tests();

    auto t4 = tools::time::time_now();
    std::cout << "time：" << std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count() << "us" << std::endl;

	return 0;
}