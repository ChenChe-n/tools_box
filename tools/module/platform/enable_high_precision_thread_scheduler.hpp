#pragma once

// 如果为windows平台
#ifdef _WIN32


class windows_high_precision_clock
{
public:
	static windows_high_precision_clock& getInstance();

	windows_high_precision_clock(const windows_high_precision_clock&) = delete;
	windows_high_precision_clock& operator=(const windows_high_precision_clock&) = delete;

private:
	windows_high_precision_clock();

	~windows_high_precision_clock() = default;
};

// 在头文件中声明一个全局变量
extern windows_high_precision_clock& __windows_high_precision_clock__;

#endif
