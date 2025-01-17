#pragma once

#include "./../../tools/base/using.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>
namespace tools {
    namespace time {
        // 获取当前的时间点
        inline std::chrono::steady_clock::time_point time_now() {
            return std::chrono::steady_clock::now();
        }
        using time_point = std::chrono::steady_clock::time_point;
        constexpr static auto default_time_point = std::chrono::steady_clock::time_point();

        // 时间单位（秒）
        using s = std::chrono::duration<f64>;
        // 时间单位（毫秒）
        using ms = std::chrono::duration<f64, std::milli>;
        // 时间单位（微秒）
        using us = std::chrono::duration<f64, std::micro>;
        // 时间单位（纳秒）
        using ns = std::chrono::duration<f64, std::nano>;

        // 单例类：启动时间基准点
        class program_startup_time_point {
        public:
            // 获取单例实例
            static program_startup_time_point& instance();

            // 获取启动时的 system_clock 时间点
            const std::chrono::system_clock::time_point& system_clock_start() const;

            // 获取启动时的 steady_clock 时间点
            const std::chrono::steady_clock::time_point& steady_clock_start() const;

        private:
            program_startup_time_point();

            std::chrono::system_clock::time_point system_start_;
            std::chrono::steady_clock::time_point steady_start_;
        };

        // 将 steady_clock 时间点转换为 system_clock 时间点
        std::chrono::system_clock::time_point steady_clock_to_system_clock(const std::chrono::steady_clock::time_point& tp);

        // 将时间点转换为 y/m/d h:m:s格式字符串
        // mode 0(默认) 1970/0/0 0:0:0
        // mode 01      1970/0/0 0:0:0.0
        // mode 02      1970/0/0 0h:0m:0s:0ms
        // mode 03      1970/0/0 0h:0m:0s:0ms:0us
        // mode 10      1970年0月0日 0时0分0秒
        // mode 11      1970年0月0日 0时0分0秒0毫秒
        // mode 12      1970年0月0日 0时0分0秒0毫秒0微秒
        // mode 255     1970Y0M0D0h0m0s0ms0us0ns
        std::string format_time_point(const std::chrono::steady_clock::time_point& tp, const u8 mode = 0);

    }
}

