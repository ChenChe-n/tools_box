#pragma once

#include "../../base.hpp"



#include <atomic>
#include <memory>
#include <optional>
#include <thread>
#include <stdexcept>
#include <chrono>
#include <utility>

namespace tools::thread {

    // 线程安全队列
    // 同时进和出
    template<typename T>
    class queue {
    public:
        // 节点类
        struct node {
            T data;                      // 数据
            std::atomic<node*> next;     // 下一个节点指针

            // 节点构造函数
            node(T value) : data(value), next(nullptr) {}
        };

        // 禁止拷贝和赋值
        queue(const queue&) = delete;
        queue& operator=(const queue&) = delete;

        // 构造函数
        queue() {
            // 初始化队列时，创建一个哨兵节点，头尾指针都指向它
            node* dummy = new node(T{});
            head.store(dummy);
            tail.store(dummy);
        }

        // 析构函数
        ~queue() {
            // 清空队列并释放所有节点
            while (pop().has_value());  // 清空队列
            delete head.load();         // 删除最后的哨兵节点
        }

        // 入队操作
        void push(T value) {
            // 创建一个新节点用于存储数据
            node* new_node = new node(value);

            // 尝试在队列尾部插入新节点
            node* old_tail;
            while (true) {
                old_tail = tail.load(); // 获取当前尾节点
                node* next = old_tail->next.load();

                // 如果当前尾节点的 next 为空，说明尾节点是实际的尾部
                if (next == nullptr) {
                    // 尝试将 new_node 设置为尾节点的 next
                    if (old_tail->next.compare_exchange_weak(next, new_node)) {
                        break; // 插入成功，退出循环
                    }
                }
                else {
                    // 如果尾节点已被其他线程修改，则更新尾指针
                    tail.compare_exchange_weak(old_tail, next);
                }
            }

            // 更新尾指针，指向新节点
            tail.compare_exchange_weak(old_tail, new_node);
        }

        // 出队操作
        std::optional<T> pop() {
            node* old_head;
            while (true) {
                old_head = head.load();  // 获取当前头节点
                node* next = old_head->next.load();

                // 如果 next 为空，说明队列为空
                if (next == nullptr) {
                    return std::nullopt;
                }

                // 尝试移动头指针到下一个节点
                if (head.compare_exchange_weak(old_head, next)) {
                    break; // 成功移动头指针
                }
            }

            // 取出下一个节点的数据并释放旧的头节点
            T result = old_head->next.load()->data;
            delete old_head;
            return result;
        }

        // 检查队列是否为空
        bool empty() const {
            // 如果头节点的 `next` 为空，则队列为空
            return head.load()->next.load() == nullptr;
        }

    private:
        std::atomic<node*> head; // 队列头指针
        std::atomic<node*> tail; // 队列尾指针
    };

    // 线程安全盒子
    // 保证只要有一个线程持有，其他线程都会获取不到
    // 支持同一线程多次持有
    template<typename T>
    class box {
    public:
        // 构造函数：支持左值和右值
        template<typename U>
        explicit box(U&& data)
            : lock_(std::thread::id()), lock_count_(0), data_(std::forward<U>(data)) {
        }

        // 禁止拷贝和赋值
        box(const box&) = delete;
        box& operator=(const box&) = delete;

        // 设置新数据（阻塞直到 box 未被占用）
        template<typename U>
        void set(U&& data) {
            wait_for_unlock();  // 等待其他线程释放锁
            data_ = std::forward<U>(data);
        }

        // 尝试获取数据的所有权
        std::optional<T*> get() {
            std::thread::id current_id = std::this_thread::get_id();

            // 如果当前线程已经持有锁，递增锁计数
            if (lock_.load(std::memory_order_acquire) == current_id) {
                ++lock_count_;
                return &data_;
            }

            // 如果当前没有线程持有锁，尝试占有
            std::thread::id expected = std::thread::id();  // 空线程 ID 表示未占用
            if (lock_.compare_exchange_strong(expected, current_id, std::memory_order_acquire)) {
                lock_count_ = 1;  // 初次占用，计数为 1
                return &data_;
            }

            // 如果被其他线程占用，返回空
            return std::nullopt;
        }

        // 释放当前线程对数据的所有权
        void release() {
            std::thread::id current_id = std::this_thread::get_id();

            // 确保当前线程是锁的所有者
            if (lock_.load(std::memory_order_acquire) != current_id) {
                throw std::runtime_error("Release called by a thread that doesn't own the lock");
            }

            // 递减锁计数，如果计数归零，释放锁
            if (--lock_count_ == 0) {
                lock_.store(std::thread::id(), std::memory_order_release);  // 释放锁
            }
        }

    private:
        // 等待锁释放
        void wait_for_unlock() {
            while (lock_.load(std::memory_order_acquire) != std::thread::id()) {
                std::this_thread::sleep_for(std::chrono::microseconds(10));  // 自旋等待
            }
        }

    private:
        std::atomic<std::thread::id> lock_;  // 当前持有锁的线程 ID
        uint64_t lock_count_;                // 当前线程的递归锁计数
        T data_;                             // 存储的数据
    };

}