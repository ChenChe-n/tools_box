#pragma once

#include "../../base.hpp"

#include <string>
#include <vector>

namespace tools::net {

    // 客户端ip和端口结构体
    struct ip_address__ {
        std::string ip_;
        u16 port_;

        // 比较运算符，用于 unordered_map 的键
        bool operator==(const ip_address__& other) const {
            return ip_ == other.ip_ && port_ == other.port_;
        }
    };

    // 消息结构体
    struct message__ {
        ip_address__ ip_address_;
        std::vector<char> data_;
    };

}

// 自定义 ip_address__ 的哈希函数
namespace std {
    template <>
    struct hash<tools::net::ip_address__> {
        size_t operator()(const tools::net::ip_address__& ip) const noexcept {
            return hash<string>()(ip.ip_) ^ (hash<u16>()(ip.port_));
        }
    };
}