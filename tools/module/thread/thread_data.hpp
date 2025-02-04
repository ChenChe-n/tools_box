#pragma once

#include "../../base.hpp"



#include <atomic>
#include <memory>
#include <optional>
#include <thread>
#include <stdexcept>
#include <chrono>
#include <utility>

namespace tools::thread::data {

    // 线程安全队列
    // 同时进和出
    template<typename T>
    class queue {
    private:
        struct node {
            T data;
            std::atomic<node*> next;

            explicit node(T&& value) : data(std::move(value)), next(nullptr) {}
            node() : next(nullptr) {}  // 哨兵节点构造函数
        };

        std::atomic<node*> head;
        std::atomic<node*> tail;
        std::atomic<u64> count{ 0 };  // 原子计数器

    public:
        queue() {
            // 初始化哨兵节点
            node* dummy = new node();
            head.store(dummy);
            tail.store(dummy);
        }

        ~queue() {
            // 清空延迟删除的节点
            static thread_local std::vector<node*> garbage;
            for (auto p : garbage) delete p;
            garbage.clear();
            // 清空队列
            while (pop().has_value());
            delete head.load();  // 删除最后的哨兵节点
        }

        void push(T value) {
            node* new_node = new node(std::move(value));
            node* old_tail = tail.load(std::memory_order_relaxed);

            while (true) {
                node* next = old_tail->next.load(std::memory_order_relaxed);

                if (!next) {
                    if (old_tail->next.compare_exchange_weak(
                        next, new_node,
                        std::memory_order_acq_rel,
                        std::memory_order_acquire))
                    {
                        count.fetch_add(1, std::memory_order_relaxed);
                        break;
                    }
                }
                else {
                    // 帮助推进尾指针
                    tail.compare_exchange_weak(
                        old_tail, next,
                        std::memory_order_relaxed,
                        std::memory_order_relaxed);
                }
                old_tail = tail.load(std::memory_order_relaxed);
            }

            // 更新尾指针
            tail.compare_exchange_weak(
                old_tail, new_node,
                std::memory_order_release,
                std::memory_order_relaxed);
        }

        std::optional<T> pop() {
            node* old_head;
            node* next;
            node* expected_next;

            do {
                old_head = head.load(std::memory_order_acquire);
                next = old_head->next.load(std::memory_order_acquire);

                if (!next) return std::nullopt;

                // 预加载数据防止移动后访问
                expected_next = next;
            } while (!head.compare_exchange_weak(
                old_head, expected_next,
                std::memory_order_acq_rel,
                std::memory_order_acquire));

            T result = std::move(expected_next->data);
            count.fetch_sub(1, std::memory_order_relaxed);

            // 延迟删除旧节点
            static thread_local std::vector<node*> garbage;
            garbage.push_back(old_head);

            if (garbage.size() > 64) {  // 批量清理
                for (auto p : garbage) delete p;
                garbage.clear();
            }

            return result;
        }

        u64 size() const noexcept {
            return count.load(std::memory_order_relaxed);
        }

        bool empty() const noexcept {
            return size() == 0;
        }
    };




}