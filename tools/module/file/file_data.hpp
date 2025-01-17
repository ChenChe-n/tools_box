#pragma once


#include "./../../../tools/base.hpp"



// 路径IO
#include <filesystem>
// 文件IO
#include <fstream>


namespace tools {
	namespace file {
		namespace fs = std::filesystem;

		class file_data
		{
		public:
			file_data();
			~file_data();

		private:
			// 文件名
			fs::path file_name_;
			// 文件内容
			char* file_data_;
			// 文件大小
            size_t file_size_;
		};

	}
}