#include "time.hpp"
namespace tools::time {


        // 获取单例实例
        program_startup_time_point& program_startup_time_point::instance() {
            static program_startup_time_point instance;
            return instance;
        }

        // 获取启动时的 system_clock 时间点
        const std::chrono::system_clock::time_point& program_startup_time_point::system_clock_start() const {
            return system_start_;
        }

        // 获取启动时的 steady_clock 时间点
        const std::chrono::steady_clock::time_point& program_startup_time_point::steady_clock_start() const {
            return steady_start_;
        }

        // 私有构造函数
        program_startup_time_point::program_startup_time_point()
            : system_start_(std::chrono::system_clock::now()),
            steady_start_(std::chrono::steady_clock::now()) {
        }


        std::chrono::system_clock::time_point steady_clock_to_system_clock(const std::chrono::steady_clock::time_point& tp) {
            // 获取基准点
            const auto& base = program_startup_time_point::instance();

            // 计算时间点对应的 system_clock 时间点
            auto elapsed = tp - base.steady_clock_start(); // 偏移量
            auto elapsed_system_duration = std::chrono::duration_cast<std::chrono::system_clock::duration>(elapsed);
            auto current_system_time = base.system_clock_start() + elapsed_system_duration;

            return current_system_time;
        }


        std::string format_time_point(const std::chrono::steady_clock::time_point& tp, const u8 mode) {
            // 计算时间点对应的 system_clock 时间点
            auto current_system_time = steady_clock_to_system_clock(tp);

            // 提取秒和子秒部分
            auto duration_since_epoch = current_system_time.time_since_epoch();
            auto seconds_since_epoch = std::chrono::floor<std::chrono::seconds>(duration_since_epoch);
            auto total_nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration_since_epoch - seconds_since_epoch).count();

            auto milliseconds = total_nanoseconds / 1'000'000;       // 毫秒
            auto microseconds = (total_nanoseconds / 1'000) % 1'000; // 微秒
            auto nanoseconds = total_nanoseconds % 1'000;            // 纳秒

            // 提取时间点中的日期和时间
            std::time_t time_t_epoch = std::chrono::system_clock::to_time_t(current_system_time);
            std::tm tm_epoch{};
#ifdef _WIN32
            localtime_s(&tm_epoch, &time_t_epoch);
#else
            localtime_r(&time_t_epoch, &tm_epoch);
#endif

            // 格式化日期和时间
            std::ostringstream oss;

            switch (mode) {
            case 0: // 1970/0/0 0:0:0
                oss << (tm_epoch.tm_year + 1900) << "/"
                    << (tm_epoch.tm_mon + 1) << "/"
                    << tm_epoch.tm_mday << " "
                    << tm_epoch.tm_hour << ":"
                    << tm_epoch.tm_min << ":"
                    << tm_epoch.tm_sec;
                break;

            case 1: // 1970/0/0 0:0:0.0
                oss << (tm_epoch.tm_year + 1900) << "/"
                    << (tm_epoch.tm_mon + 1) << "/"
                    << tm_epoch.tm_mday << " "
                    << tm_epoch.tm_hour << ":"
                    << tm_epoch.tm_min << ":"
                    << tm_epoch.tm_sec << "."
                    << milliseconds;
                break;

            case 2: // 1970/0/0 0h:0m:0s:0ms
                oss << (tm_epoch.tm_year + 1900) << "/"
                    << (tm_epoch.tm_mon + 1) << "/"
                    << tm_epoch.tm_mday << " "
                    << tm_epoch.tm_hour << "h:"
                    << tm_epoch.tm_min << "m:"
                    << tm_epoch.tm_sec << "s:"
                    << milliseconds << "ms";
                break;

            case 3: // 1970/0/0 0h:0m:0s:0ms:0us
                oss << (tm_epoch.tm_year + 1900) << "/"
                    << (tm_epoch.tm_mon + 1) << "/"
                    << tm_epoch.tm_mday << " "
                    << tm_epoch.tm_hour << "h:"
                    << tm_epoch.tm_min << "m:"
                    << tm_epoch.tm_sec << "s:"
                    << milliseconds << "ms:"
                    << microseconds << "us";
                break;

            case 10: // 1970年0月0日 0时0分0秒
                oss << (tm_epoch.tm_year + 1900) << "年"
                    << (tm_epoch.tm_mon + 1) << "月"
                    << tm_epoch.tm_mday << "日 "
                    << tm_epoch.tm_hour << "时"
                    << tm_epoch.tm_min << "分"
                    << tm_epoch.tm_sec << "秒";
                break;

            case 11: // 1970年0月0日 0时0分0秒0毫秒
                oss << (tm_epoch.tm_year + 1900) << "年"
                    << (tm_epoch.tm_mon + 1) << "月"
                    << tm_epoch.tm_mday << "日 "
                    << tm_epoch.tm_hour << "时"
                    << tm_epoch.tm_min << "分"
                    << tm_epoch.tm_sec << "秒"
                    << milliseconds << "毫秒";
                break;

            case 12: // 1970年0月0日 0时0分0秒0毫秒0微秒
                oss << (tm_epoch.tm_year + 1900) << "年"
                    << (tm_epoch.tm_mon + 1) << "月"
                    << tm_epoch.tm_mday << "日 "
                    << tm_epoch.tm_hour << "时"
                    << tm_epoch.tm_min << "分"
                    << tm_epoch.tm_sec << "秒"
                    << milliseconds << "毫秒"
                    << microseconds << "微秒";
                break;

            case 255: // 1970Y0M0D0h0m0s0ms0us0ns
                oss << (tm_epoch.tm_year + 1900) << "Y"
                    << (tm_epoch.tm_mon + 1) << "M"
                    << tm_epoch.tm_mday << "D"
                    << tm_epoch.tm_hour << "h"
                    << tm_epoch.tm_min << "m"
                    << tm_epoch.tm_sec << "s"
                    << milliseconds << "ms"
                    << microseconds << "us"
                    << nanoseconds << "ns";
                break;

            default: // 兜底处理，等同于 mode 0
                oss << (tm_epoch.tm_year + 1900) << "/"
                    << (tm_epoch.tm_mon + 1) << "/"
                    << tm_epoch.tm_mday << " "
                    << tm_epoch.tm_hour << ":"
                    << tm_epoch.tm_min << ":"
                    << tm_epoch.tm_sec;
                break;
            }

            return oss.str();
        }


}