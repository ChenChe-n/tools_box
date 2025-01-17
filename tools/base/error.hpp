#pragma once

#include "./../../tools/base/using.hpp"
#include "./../../tools/base/time.hpp"

#include <string>


namespace tools {
	class base_error{
	public:
		class error_code {
		public:
			error_code(const u64 code) {
				error_code_ = code;
				error_message_ = "";
				error_time_ = tools::time::time_now();
			}
			error_code(const u64 code, const tools::time::time_point& time_point) {
				error_code_ = code;
				error_message_ = "";
				error_time_ = time_point;
			}
			error_code(const u64 code,const std::string& error_message) {
				error_time_ = tools::time::time_now();
				error_message_ = error_message;
				error_code_ = code;
			}
			error_code(const u64 code, const std::string& error_message, const tools::time::time_point& time_point) {
				error_time_ = time_point;
				error_message_ = error_message;
				error_code_ = code;
			}
			// 错误码
            u64 error_code_;
			// 错误信息
			std::string error_message_;
			// 错误时间
			std::chrono::steady_clock::time_point error_time_;
		};
		// 构造函数
		base_error() {

		}
		// 析构函数
		~base_error() {
			
		}
		// 添加错误码
		void add(u64 code){
			error_code_.emplace_back(code);
			return;
		}
        // 清空错误码
		void clear(){
			error_code_.clear();
			return;
		}
		// 获取错误码
		std::vector<error_code> get(){
			return error_code_;
		}
		// 是否有错误码
        size_t error_num(){
			return error_code_.size();
		}
	private:
		// 附加错误码
		std::vector<error_code> error_code_ = {};
	};
}
