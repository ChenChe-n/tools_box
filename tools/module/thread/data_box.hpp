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
            std::shared_ptr<T> data;      // 数据存储在智能指针中
            std::atomic<node*> next;     // 下一个节点指针

            explicit node(T value) : data(std::make_shared<T>(std::move(value))), next(nullptr) {}
            node() : data(nullptr), next(nullptr) {} // 空节点构造函数
        };

        queue() : head(new node()), tail(head.load()) {}

        ~queue() {
            while (auto old_head = head.exchange(nullptr)) {
                delete old_head;
            }
        }

        // 禁止拷贝和赋值
        queue(const queue&) = delete;
        queue& operator=(const queue&) = delete;

        // 入队操作
        void push(T value) {
            auto new_node = new node(std::move(value)); // 创建新节点
            node* old_tail = nullptr;

            while (true) {
                old_tail = tail.load(std::memory_order_acquire); // 获取当前尾节点
                node* next = old_tail->next.load(std::memory_order_acquire);
                if (next == nullptr) { // 确保尾节点的 next 为空
                    if (old_tail->next.compare_exchange_weak(next, new_node, std::memory_order_release, std::memory_order_relaxed)) {
                        break;
                    }
                }
                else {
                    // 如果 tail 不一致，更新 tail 到最新节点
                    tail.compare_exchange_weak(old_tail, next, std::memory_order_release, std::memory_order_relaxed);
                }
            }

            // 更新尾节点
            tail.compare_exchange_strong(old_tail, new_node, std::memory_order_release, std::memory_order_relaxed);
        }

        // 出队操作
        std::optional<T> pop() {
            node* old_head = nullptr;
            while (true) {
                old_head = head.load(std::memory_order_acquire); // 获取当前头节点
                node* next = old_head->next.load(std::memory_order_acquire);
                if (next == nullptr) {
                    // 队列为空
                    return std::nullopt;
                }
                // CAS 更新 head 到下一个节点
                if (head.compare_exchange_weak(old_head, next, std::memory_order_release, std::memory_order_relaxed)) {
                    break;
                }
            }

            // 提取数据
            auto result = *(old_head->next.load(std::memory_order_acquire)->data);
            delete old_head; // 释放旧的头节点
            return result;
        }

        // 检查队列是否为空
        bool empty() const {
            node* head_ptr = head.load(std::memory_order_acquire);
            node* next_ptr = head_ptr->next.load(std::memory_order_acquire);
            return next_ptr == nullptr;
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