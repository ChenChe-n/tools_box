# 指定 CMake 的最低版本
cmake_minimum_required(VERSION 3.14)

# 定义项目名称
project(tools_box)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项，默认 Release 模式
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# 包含所有 tools 目录中的源文件
file(GLOB_RECURSE TOOLS_SOURCES tools/*.cpp)

# 包含目录（头文件所在目录）
include_directories(${CMAKE_SOURCE_DIR}/tools)

# 定义主源文件
set(MAIN_FILE ${CMAKE_SOURCE_DIR}/test.cpp)

# 添加可执行目标
add_executable(${PROJECT_NAME} ${MAIN_FILE} ${TOOLS_SOURCES})
