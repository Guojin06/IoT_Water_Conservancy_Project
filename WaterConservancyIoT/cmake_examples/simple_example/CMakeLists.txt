# 🎯 第1步：基本设置
cmake_minimum_required(VERSION 3.16)    # CMake最低版本要求
project(SimpleCalculator)               # 项目名称

# 🎯 第2步：设置C++标准
set(CMAKE_CXX_STANDARD 17)             # 使用C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)    # 强制要求C++17

# 🎯 第3步：方法1 - 最简单的方式（所有文件写在一起）
# add_executable(calculator main.cpp calculator.cpp)

# 🎯 第3步：方法2 - 使用变量（推荐）
set(SOURCES 
    main.cpp
    calculator.cpp
)
add_executable(calculator ${SOURCES})

# 🎯 第4步：设置编译选项（可选）
if(MSVC)
    target_compile_options(calculator PRIVATE /W4)      # Windows: 高警告级别
else()
    target_compile_options(calculator PRIVATE -Wall -Wextra)  # Linux/Mac: 高警告级别
endif()

# 🎯 第5步：输出信息（帮助理解）
message(STATUS "✅ 项目名称: ${PROJECT_NAME}")
message(STATUS "✅ C++标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "✅ 编译器: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "✅ 源文件: ${SOURCES}")

# 🎯 完成！运行 cmake . && cmake --build . 就能编译了
