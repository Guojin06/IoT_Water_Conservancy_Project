# 静态库 vs 动态库对比示例
cmake_minimum_required(VERSION 3.16)
project(LibraryComparison)

set(CMAKE_CXX_STANDARD 17)

# 📚 创建静态库
add_library(math_static STATIC math_functions.cpp)

# 🔄 创建动态库  
add_library(math_shared SHARED math_functions.cpp)

# 📱 使用静态库的程序
add_executable(program_static main.cpp)
target_link_libraries(program_static math_static)

# 📱 使用动态库的程序
add_executable(program_shared main.cpp)
target_link_libraries(program_shared math_shared)

# 📊 输出信息
message(STATUS "📚 静态库程序: program_static")
message(STATUS "🔄 动态库程序: program_shared")
message(STATUS "编译后比较两个程序的大小差异！")
