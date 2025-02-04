#pragma once


#include "../../base.hpp"

#include "../thread.hpp"


#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <atomic>
#include <memory>

namespace tools::file {
    using byte = char;
    namespace fs = std::filesystem;

    class file_thread {
    public:
        // 使用智能指针管理线程池所有权
        explicit file_thread(tools::thread::pool* thread_pool = nullptr) noexcept
            : pool_(thread_pool), owner_pool_(nullptr) {
            if (!pool_) {
                owner_pool_ = std::make_unique<tools::thread::pool>();
                pool_ = owner_pool_.get();
            }
        }

        ~file_thread() noexcept {
            wait();
        }

        void add_read(std::vector<byte>& data,
            const fs::path& path,
            std::ios_base::openmode mode = std::ios::in | std::ios::binary) noexcept {
            try {
                // 获取文件大小并预分配内存
                const u64 file_size = fs::file_size(path);
                data.resize(file_size);

                // 自动计算最佳线程数（至少1个，每个线程处理至少64MB）
                u64 thread_num = calculate_optimal_threads(file_size, 64ull * 1024 * 1024);

                const u64 block_size = file_size / thread_num;
                u64 remaining = file_size % thread_num;

                // 使用智能指针保证数据生命周期
                auto shared_data = std::make_shared<std::vector<byte>*>(&data);

                for (u64 i = 0; i < thread_num; ++i) {
                    const u64 offset = i * block_size;
                    u64 size = block_size + (i == thread_num - 1 ? remaining : 0);

                    task_count_.fetch_add(1, std::memory_order_relaxed);
                    pool_->insert([this, shared_data, path, offset, size, mode]() {
                        this->read_impl((*shared_data)->data() + offset,
                            offset, size, path, mode);
                        task_count_.fetch_sub(1, std::memory_order_relaxed);
                        });
                }
            }
            catch (...) {
                // 记录错误日志
            }
        }

        void add_write(const std::vector<byte>& data,
            const fs::path& path,
            std::ios_base::openmode mode = std::ios::out | std::ios::binary) noexcept {
            const u64 total_size = data.size();

            // 自动计算最佳线程数（至少1个，每个线程处理至少64MB）
            u64 thread_num = calculate_optimal_threads(total_size, 64ull * 1024 * 1024);

            const u64 block_size = total_size / thread_num;
            u64 remaining = total_size % thread_num;

            // 使用智能指针延长数据生命周期
            auto shared_data = std::make_shared<std::vector<byte>>(data);

            for (u64 i = 0; i < thread_num; ++i) {
                const u64 offset = i * block_size;
                u64 size = block_size + (i == thread_num - 1 ? remaining : 0);

                task_count_.fetch_add(1, std::memory_order_relaxed);
                pool_->insert([this, shared_data, path, offset, size, mode]() {
                    this->write_impl(shared_data->data() + offset,
                        offset, size, path, mode);
                    task_count_.fetch_sub(1, std::memory_order_relaxed);
                    });
            }
        }

        void wait() noexcept {
            while (task_count_.load(std::memory_order_acquire) > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

    private:
        // 计算最佳线程数（确保每个块不小于min_block_size）
        static u64 calculate_optimal_threads(u64 total_size, u64 min_block_size) noexcept {
            u64 thread_num = std::thread::hardware_concurrency();

            // 保证每个块至少min_block_size
            while (total_size / thread_num < min_block_size && thread_num > 1) {
                thread_num /= 2;
            }

            // 处理极小文件情况
            return (total_size == 0) ? 1 : std::max(1ull, thread_num);
        }

        bool read_impl(byte* data, u64 offset, u64 size,
            const fs::path& path,
            std::ios_base::openmode mode) noexcept {
            try {
                std::ifstream file(path, mode);
                if (!file.is_open()) return false;

                file.seekg(offset);
                if (!file.read(data, size)) return false;

                return file.gcount() == static_cast<std::streamsize>(size);
            }
            catch (...) {
                return false;
            }
        }

        bool write_impl(const byte* data, u64 offset, u64 size,
            const fs::path& path,
            std::ios_base::openmode mode) noexcept {
            try {
                // 先尝试打开现有文件
                std::fstream file;
                file.open(path, mode | std::ios::in | std::ios::out);

                // 文件不存在时创建新文件
                if (!file.is_open()) {
                    file.open(path, mode | std::ios::out);
                    if (!file.is_open()) return false;
                }

                file.seekp(offset);
                file.write(data, size);
                return file.good();
            }
            catch (...) {
                return false;
            }
        }

        // 线程池管理
        tools::thread::pool* pool_;
        std::unique_ptr<tools::thread::pool> owner_pool_;

        // 任务计数器（使用memory_order_relaxed优化）
        std::atomic<u64> task_count_{ 0 };
    };
}