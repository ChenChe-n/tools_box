#pragma once

#include "../../base.hpp"

// 多线程数据容器
#include "thread_data.hpp"

#include <vector>
#include <thread>
#include <atomic>
#include <functional>

namespace tools::thread {
    class pool {
    public:
        explicit pool(size_t thread_count = std::thread::hardware_concurrency())
            : stop(false) {
            for (size_t i = 0; i < thread_count; ++i) {
                threads.emplace_back(&pool::worker, this);
            }
        }

        template<typename F, typename... Args>
        bool insert(F&& f, Args&&... args) {
            using func_type = void();

            // 包装任务并提交到队列
            auto task = std::make_shared<std::function<func_type>>(
                [=] { std::invoke(f, args...); }
            );

            if (stop.load(std::memory_order_relaxed)) {
                return false;
            }

            task_queue.push([task] { (*task)(); });
            return true;
        }

        u64 task_count() const {
            return task_queue.size();
        }

        void wait() {
            // 先等待任务完成
            while (task_queue.size() != 0 ||
                count.load(std::memory_order_acquire) > 0) {
                std::this_thread::yield();
            }

            // 再停止线程
            stop.store(true, std::memory_order_release);

            for (auto& thread : threads) {
                if (thread.joinable()) thread.join();
            }
        }

        u64 thread_count() const {
            return threads.size();
        }

        void join() {
            stop.store(true, std::memory_order_release);

            for (auto& thread : threads) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
        }

        ~pool() {
            wait();
        }

    private:
        void worker() {
            constexpr int max_spin = 100;     // 最大自旋次数
            constexpr int base_delay = 100;   // 基础延迟(微秒)
            int current_spin = max_spin;      // 当前剩余自旋次数
            int current_delay = base_delay;   // 当前延迟时间

            while (!stop.load(std::memory_order_acquire)) {
                // 尝试获取任务
                if (auto task = task_queue.pop()) {
                    ++count;
                    (*task)();
                    --count;

                    // 成功获取任务后重置状态
                    current_spin = max_spin;
                    current_delay = base_delay;
                    continue;
                }

                // 自旋阶段控制
                if (--current_spin > 0) {
                    //std::this_thread::yield(); // 轻度退让
                    continue;
                }

                // 进入退避阶段
                std::this_thread::sleep_for(
                    std::chrono::microseconds(current_delay));

                // 指数退避策略（上限1ms）
                current_delay = std::min(current_delay * 2, 1000);
            }
        }
        // 任务队列
        tools::thread::data::queue<std::function<void()>> task_queue;
        // 线程池
        std::vector<std::thread> threads;
        // 停止标志
        std::atomic<bool> stop;
        // 任务计数器
        std::atomic<u64> count{ 0 };
    };
};