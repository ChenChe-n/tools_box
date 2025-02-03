

#ifdef _WIN32
#include "enable_high_precision_thread_scheduler.hpp"

#include <Windows.h>
#include <stdexcept>
#include <mmsystem.h> 
#pragma comment(lib, "winmm.lib")

// 初始化windows平台为高精度线程调度器
windows_high_precision_clock& windows_high_precision_clock::getInstance()
{
	static windows_high_precision_clock instance;
	return instance;
}
windows_high_precision_clock::windows_high_precision_clock()
{
	timeBeginPeriod(1);
}
// 创建一个全局对象，这会自动调用构造函数
windows_high_precision_clock& _windows_high_precision_clock_ = windows_high_precision_clock::getInstance();
// 在头文件中声明一个全局变量
extern windows_high_precision_clock& __windows_high_precision_clock__;
#endif
