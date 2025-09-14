# 🧪 Google Test 示例项目
cmake_minimum_required(VERSION 3.16)
project(GoogleTestDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 🔍 查找Google Test (如果系统安装了)
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    # 📦 如果没找到，我们下载并构建Google Test
    message(STATUS "Google Test not found, downloading...")
    
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
else()
    message(STATUS "Found Google Test")
endif()

# 🧪 创建测试可执行文件
add_executable(basic_test basic_assertions_simple.cpp)
add_executable(sensor_test sensor_test_simple.cpp)

# 🔗 链接Google Test
target_link_libraries(basic_test gtest_main)
target_link_libraries(sensor_test gtest_main)

# ✅ 添加测试
enable_testing()
add_test(NAME BasicAssertions COMMAND basic_test)
add_test(NAME SensorTests COMMAND sensor_test)

# 📊 输出信息
message(STATUS "🧪 Google Test示例项目配置完成")
message(STATUS "📁 构建目录: ${CMAKE_BINARY_DIR}")
message(STATUS "🔨 构建命令: cmake --build .")
message(STATUS "🏃 运行测试: ctest 或直接运行可执行文件")
