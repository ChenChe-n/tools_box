#include "file.hpp"
#include <iostream>
namespace tools::file {
	file_task_pool::file_task_pool(tools::thread::pool* thread_pool,u64 block_size) noexcept
	{
        if (thread_pool == nullptr)
        {
            thread_pool = new thread::pool();
            owner_pool_ = true;
        }
        thread_pool_ = thread_pool;


        if (block_size <= tools::size::mi * 16) {
            block_size = tools::size::mi * 16;
		}
		block_size_ = block_size;
	}

	file_task_pool::~file_task_pool() noexcept
	{
		stop();
        if (owner_pool_) {
            delete thread_pool_;
        }
		return;
	}

	u64 file_task_pool::get_task_count()const noexcept
	{
        return task_count_.load(std::memory_order_relaxed);
	}

	void file_task_pool::stop()			noexcept
	{
		is_running_.store(false, std::memory_order_relaxed);
		wait();
		return;
	}

	void file_task_pool::wait()			const noexcept
	{
		while (task_count_.load(std::memory_order_relaxed) > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		return;
	}

    void file_task_pool::add_write(
		fs::path path,
		std::vector<byte>& data,
		mode mode
    ) noexcept {
        if (
            // 检查是否运行
            !is_running_.load(std::memory_order_relaxed)
            // 检查线程池是否可用
            or !thread_pool_
            ) {
            return;
        }

        try {
            u64 file_size = 0;
            // 追加模式
            if (mode == mode::addend) {
                if (!fs::exists(path)) {
                    std::ofstream file(path, std::ios::out | std::ios::binary);
                    file.close();
                }
                file_size = fs::file_size(path);
            }

            // 清空文件（覆盖模式）
            if (mode == mode::cover) {
                std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);
                if (!file.is_open()) {
                    throw std::ios::failure("Failed to open file for clearing: " + path.string());
                }
            }

            // 计算块数
            u64 data_size = data.size();
            u64 now_data = 0;

            // 创建任务
            while (now_data < data_size) {
                task_count_.fetch_add(1, std::memory_order_relaxed); // 增加任务计数
                // 添加任务
                thread_pool_->insert([this, path, now_data, &data, data_size, file_size]() {
                    _write_(path,
                        const_cast<byte*>(&(data)[now_data]),
                        std::min(block_size_, (data_size - now_data)), 
                        now_data + file_size);
                    });
                now_data += std::min(block_size_, (data_size - now_data));
            }
        }
        catch (...) {

        }
        return;
    }

	void file_task_pool::add_read(
		fs::path path,
		std::vector<byte>& data
	) noexcept
	{
        if (
            // 检查是否运行
            !is_running_.load(std::memory_order_relaxed)
            // 检查线程池是否可用
            or !thread_pool_
            // 检查文件是否存在
            or !fs::exists(path)
            // 检查是否为文件
            or !fs::is_regular_file(path)
            ) {
            data.resize(0);
            return;
        }

        // 获取文件大小并设置缓冲区
        u64 file_size = fs::file_size(path);
        data.resize(file_size);



        try {
            // 计算块数
            u64 data_size = file_size;
            u64 now_data = 0;

            // 创建任务
            while (now_data < data_size) {
                task_count_.fetch_add(1, std::memory_order_relaxed); // 增加任务计数
                // 添加任务
                thread_pool_->insert([this, path, now_data, &data, data_size]() {
                    _read_(path,
                        const_cast<byte*>(&(data)[now_data]),
                        std::min(block_size_, (data_size - now_data)),
                        now_data);
                    });
                now_data += std::min(block_size_, (data_size - now_data));
            }
        }
        catch (...) {

        }
        return;
	}

    void file_task_pool::_write_(fs::path path, byte* data, u64 byte_size, u64 skip_byte_size) noexcept {
        std::fstream file;

        if (is_running_.load(std::memory_order_relaxed)) {
            try {
                // 打开文件（读写模式，不清空内容）
                file.open(path, std::ios::in | std::ios::out | std::ios::binary);
                if (!file.is_open()) {
                    throw std::ios::failure("Failed to open file for writing: " + path.string());
                }

                // 设置偏移量并写入数据
                file.seekp(skip_byte_size);
                file.write(reinterpret_cast<char*>(data), byte_size);

                // 检查写入是否成功
                if (!file) {
                    throw std::ios::failure("Failed to write data to file: " + path.string());
                }

                file.close();
            }
            catch (...) {

            }
        }

        // 减少任务计数器
        task_count_.fetch_sub(1, std::memory_order_relaxed);
    }

    void file_task_pool::_read_(fs::path path, byte* data, u64 byte_size, u64 skip_byte_size) noexcept {
        std::fstream file;

        if (is_running_.load(std::memory_order_relaxed)) {
            try {
                // 打开文件（读取模式）
                file.open(path, std::ios::in | std::ios::binary);
                if (!file.is_open()) {
                    throw std::ios::failure("Failed to open file for reading: " + path.string());
                }

                // 设置偏移量并读取数据
                file.seekg(skip_byte_size);
                file.read(reinterpret_cast<char*>(data), byte_size);

                // 检查读取是否成功
                if (!file) {
                    throw std::ios::failure("Failed to read data from file: " + path.string());
                }

                file.close();
            }
            catch (...) {

            }
        }

        // 减少任务计数器
        task_count_.fetch_sub(1, std::memory_order_relaxed);
    }
}