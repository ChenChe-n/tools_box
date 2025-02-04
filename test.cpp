#include <iostream>
#include <cstdio>
#include "tools/module/thread.hpp"
#include "tools/module/file.hpp"
#include "tools/module/big_number.hpp"
#include "tools/module/data.hpp"
#include "tools/module/time.hpp"
#include "tools/module/virtual_machine.hpp"

namespace fi = tools::file;
int main() {
	tools::thread::pool pool(1);
	auto f = fi::file_thread(&pool);
	std::vector<byte> v;
	f.add_read(v, "output_log.txt");
	f.wait();
	std::cout << std::string(v.data(), v.size()) << std::endl;
	return 0;
}

//void cout(u64 a) {
//    std::cout << a << std::endl;
//}
//
//int main() {
//    tools::thread::pool pool;
//
//    for (size_t i = 0; i < 1'000'000; i++)
//    {
//        pool.insert(cout, i);
//    }
//
//    pool.wait();
//    //pool.join();
//    return 0;
//}