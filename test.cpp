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
	std::u8string str = u8"一二三四五六七八九十，1234567890，依而伞思无流起吧就是";

	
	std::vector<byte> data((byte*)(str.data()), (byte*)(str.data() + str.size()));

	tools::file::file_task_pool fsp;
	std::vector<std::vector<byte>> out = {};
	out.reserve(10000);
	u64 i = 1;

	while (i > 0) {
		i--;
		{
			std::string str = "t/";
			str += ('a' + (rand() % 26));
			fsp.add_write(str, data,tools::file::mode::addend);
		}
		//{
		//	std::string str = "t/";
		//	str += ('a' + (rand() % 26));
		//	out.push_back(std::vector<byte>());
		//	fsp.add_read(str, out.back());
		//}
		std::cout << "i : " << i << std::endl;
		std::cout << "task:" << fsp.get_task_count() << std::endl;
	}

	fsp.wait();

	for (std::vector<byte>& i : out)
	{
		std::cout << std::string(i.data(), i.size()).c_str() << std::endl << std::endl;
	}

	return 0;
}