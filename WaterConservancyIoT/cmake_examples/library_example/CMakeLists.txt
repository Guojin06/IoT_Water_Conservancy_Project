# 🏗️ 库使用示例 - 展示如何创建和使用库
cmake_minimum_required(VERSION 3.16)
project(LibraryExample)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 📊 第1步：创建数学库
add_library(math_lib STATIC        # 创建静态库
    math/math_operations.cpp
    math/math_operations.h
)

# 📊 为库设置包含目录
target_include_directories(math_lib PUBLIC math/)  # PUBLIC: 使用这个库的程序也能看到math/目录

# 📊 第2步：创建字符串处理库
add_library(string_lib STATIC
    string/string_utils.cpp
    string/string_utils.h
)

target_include_directories(string_lib PUBLIC string/)

# 📊 第3步：创建主程序
add_executable(main_program
    main.cpp
)

# 🔗 第4步：连接库
target_link_libraries(main_program 
    math_lib        # 主程序使用数学库
    string_lib      # 主程序使用字符串库
)

# 📝 输出构建信息
message(STATUS "=== 库示例构建信息 ===")
message(STATUS "📚 创建的库: math_lib, string_lib")
message(STATUS "📱 主程序: main_program")
message(STATUS "🔗 依赖关系: main_program -> math_lib + string_lib")

# 🎯 构建命令:
# mkdir build && cd build
# cmake ..
# cmake --build .
