#pragma once


#include "../../base.hpp"

#include "../thread.hpp"


#include <filesystem>
#include <fstream>
#include <atomic>

namespace tools::file {
    using byte = char;
    namespace fs = std::filesystem;

    enum class mode {
        // 覆盖模式
        cover,
        // 追加模式
        addend,
    };

    class file_task_pool {
    public:
        // 初始化
        file_task_pool(tools::thread::pool* thread_pool = nullptr, u64 block_size = tools::size::max<u64>())  noexcept;
        // 析构
        ~file_task_pool() noexcept;
        // 输出剩余任务数
        u64 get_task_count()    const noexcept;
        // 停止任务
        void stop()             noexcept;
        // 等待任务完成
        void wait()             const noexcept;

        // 添加写入任务
        void add_write(fs::path path, std::vector<byte>& data, mode mode = mode::cover) noexcept;
        // 添加读取任务
        void add_read(fs::path path, std::vector<byte>& data) noexcept;
    private:
        // 剩余任务计数器
        std::atomic<u64>    task_count_{ 0 };
        // 运行标志
        std::atomic<bool>   is_running_{ true };
        // 分块大小
        u64                 block_size_ = tools::size::max<u64>();
        // 线程池
        tools::thread::pool* thread_pool_;
        bool                owner_pool_ = false;
    private:
        // 写入函数
        void _write_(fs::path path, byte* data, u64 byte_size, u64 skip_byte_size)  noexcept;
        // 读取函数
        void _read_(fs::path path, byte* data, u64 byte_size, u64 skip_byte_size)   noexcept;
    };

}