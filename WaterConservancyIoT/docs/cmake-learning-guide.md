# 🚀 CMake零基础学习指南

## 🎯 学习目标

通过这个指南，您将：
- 👶 从零开始理解CMake
- 🛠️ 掌握CMake的基本语法
- 🏗️ 学会创建库和管理依赖
- 💼 能够应用到实际项目中

## 📚 第一部分：CMake是什么？

### 🤔 为什么需要CMake？

**传统编译的痛苦：**
```bash
# Linux下编译
g++ -I./include -L./lib main.cpp math.cpp -lsomelib -o program

# Windows下编译  
cl /I.\include /LIBPATH:.\lib main.cpp math.cpp somelib.lib /Fe:program.exe

# macOS下编译
clang++ -I./include -L./lib main.cpp math.cpp -lsomelib -o program
```

**问题：**
- 😵 命令复杂，容易出错
- 🌍 不同平台命令不同
- 📈 项目大了管理困难
- 👥 团队协作困难

**CMake的解决方案：**
```cmake
# 一个简单的CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(MyProgram)
add_executable(program main.cpp math.cpp)
target_link_libraries(program somelib)
```

然后在任何平台都可以：
```bash
cmake .
cmake --build .
```

### 🎭 CMake的角色

```
您的项目源码
     ⬇️
📋 CMakeLists.txt (您写的"配方")
     ⬇️
⚙️ CMake工具 (聪明的"翻译官")
     ⬇️
🔧 平台特定的构建文件
   ├── Linux: Makefile
   ├── Windows: Visual Studio 项目
   └── macOS: Xcode 项目
     ⬇️
🏭 编译器 (GCC/MSVC/Clang)
     ⬇️
🎯 最终的可执行程序
```

## 📖 第二部分：基础语法详解

### 🔰 最简单的CMakeLists.txt

```cmake
# 这是注释，以#开头
cmake_minimum_required(VERSION 3.16)  # 🎯 告诉CMake我需要什么版本
project(HelloWorld)                   # 📋 给项目起个名字
add_executable(hello main.cpp)        # 🔨 编译main.cpp成hello程序
```

**逐行解释：**
1. `cmake_minimum_required` - "我需要至少3.16版本的CMake"
2. `project()` - "这个项目叫HelloWorld"
3. `add_executable()` - "把main.cpp编译成叫hello的程序"

### 📝 变量的使用

```cmake
# 🏷️ 定义变量
set(PROJECT_NAME "AwesomeProgram")           # 字符串变量
set(VERSION_MAJOR 1)                        # 数字变量
set(VERSION_MINOR 0)                        # 数字变量
set(SOURCES main.cpp utils.cpp math.cpp)    # 列表变量

# 🔍 使用变量
project(${PROJECT_NAME})                    # 使用项目名变量
add_executable(program ${SOURCES})          # 使用源文件列表

# 📊 系统预定义变量
message("项目名: ${PROJECT_NAME}")            # 项目名称
message("源码目录: ${CMAKE_SOURCE_DIR}")       # 源代码根目录  
message("构建目录: ${CMAKE_BINARY_DIR}")       # 构建输出目录
message("编译器: ${CMAKE_CXX_COMPILER}")       # C++编译器路径
```

### 🎯 Target（目标）概念

**Target = 您想要构建的东西**

```cmake
# 📱 可执行文件目标
add_executable(my_program main.cpp)

# 📚 静态库目标  
add_library(my_static_lib STATIC lib.cpp)

# 🔄 动态库目标
add_library(my_shared_lib SHARED lib.cpp)
```

### 🔗 依赖关系

```cmake
# 创建库
add_library(math_lib STATIC math.cpp)
add_library(string_lib STATIC string.cpp)

# 创建程序
add_executable(main_program main.cpp)

# 🔗 设置依赖关系
target_link_libraries(main_program 
    math_lib        # main_program需要math_lib
    string_lib      # main_program需要string_lib
)
```

**依赖关系图：**
```
main_program
    ├── math_lib
    └── string_lib
```

## 🏗️ 第三部分：实用技巧

### 📂 目录结构管理

```cmake
# 📁 设置包含目录（头文件目录）
target_include_directories(my_target PRIVATE include/)
target_include_directories(my_target PUBLIC include/)   # 其他依赖它的目标也能看到
target_include_directories(my_target INTERFACE include/) # 只有依赖它的目标能看到
```

**可见性说明：**
- `PRIVATE` - 只有当前目标能看到
- `PUBLIC` - 当前目标和依赖它的目标都能看到  
- `INTERFACE` - 只有依赖它的目标能看到

### 🔧 编译选项设置

```cmake
# 🎚️ 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ⚠️ 设置编译警告
if(MSVC)
    target_compile_options(my_target PRIVATE /W4)          # Windows高警告
else()
    target_compile_options(my_target PRIVATE -Wall -Wextra) # Linux/Mac高警告
endif()

# 🏷️ 设置编译宏定义
target_compile_definitions(my_target PRIVATE 
    DEBUG=1
    VERSION_STRING="1.0.0"
)
```

### 🔍 条件编译

```cmake
# 🌍 根据平台设置
if(WIN32)
    message("正在为Windows编译")
    set(PLATFORM_SOURCES windows_specific.cpp)
elseif(UNIX)
    message("正在为Unix/Linux编译")
    set(PLATFORM_SOURCES unix_specific.cpp)
endif()

# 🏗️ 根据构建类型设置
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(my_target PRIVATE DEBUG_MODE=1)
else()
    target_compile_definitions(my_target PRIVATE RELEASE_MODE=1)
endif()
```

## 🧪 第四部分：实际练习

### 🎯 练习1：简单计算器

**文件结构：**
```
simple_calculator/
├── main.cpp
├── calculator.cpp
├── calculator.h
└── CMakeLists.txt
```

**CMakeLists.txt：**
```cmake
cmake_minimum_required(VERSION 3.16)
project(SimpleCalculator)

set(CMAKE_CXX_STANDARD 17)

# 方法1：直接列出所有文件
add_executable(calculator main.cpp calculator.cpp)

# 方法2：使用变量（推荐）
# set(SOURCES main.cpp calculator.cpp)
# add_executable(calculator ${SOURCES})
```

### 🎯 练习2：多库项目

**文件结构：**
```
library_project/
├── main.cpp
├── math/
│   ├── math_operations.cpp
│   └── math_operations.h
├── string/
│   ├── string_utils.cpp
│   └── string_utils.h
└── CMakeLists.txt
```

**CMakeLists.txt：**
```cmake
cmake_minimum_required(VERSION 3.16)
project(LibraryProject)

set(CMAKE_CXX_STANDARD 17)

# 📚 创建数学库
add_library(math_lib STATIC
    math/math_operations.cpp
    math/math_operations.h
)
target_include_directories(math_lib PUBLIC math/)

# 📚 创建字符串库
add_library(string_lib STATIC
    string/string_utils.cpp
    string/string_utils.h
)
target_include_directories(string_lib PUBLIC string/)

# 📱 创建主程序
add_executable(main_program main.cpp)

# 🔗 链接库
target_link_libraries(main_program math_lib string_lib)
```

## 🚀 第五部分：我们项目中的应用

### 🏗️ 我们项目的构建流程

```
1. 📚 创建基础库 (water_iot_common)
   ├── logger.cpp
   ├── config.cpp
   └── utils.cpp

2. 📡 创建传感器库 (water_iot_sensors)
   ├── sensor_base.cpp
   └── water_level_sensor.cpp
   └── 依赖: water_iot_common

3. ⚙️ 创建数据处理库 (water_iot_data_processor)
   └── 依赖: water_iot_common + water_iot_sensors

4. 💾 创建存储库 (water_iot_storage)
   └── 依赖: water_iot_common

5. 🌐 创建API服务库 (water_iot_api_server)
   └── 依赖: 所有上述库

6. 🎯 创建最终程序 (water_iot_server)
   └── 依赖: 所有库
```

### 📊 依赖关系图

```
          water_iot_server (主程序)
               ⬇️
    ┌─────────────────────────────────┐
    ⬇️                               ⬇️
water_iot_api_server        water_iot_data_processor
    ⬇️                               ⬇️
    └─────────────┬─────────────────────┘
                  ⬇️
         water_iot_storage    water_iot_sensors
                  ⬇️                 ⬇️
                  └──── water_iot_common ────┘
```

### 🔧 关键的CMake技巧

```cmake
# 🔍 自动搜索源文件
file(GLOB_RECURSE COMMON_SOURCES "src/common/src/*.cpp")

# 📁 设置包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensors/include
)

# 🌐 第三方库管理
find_package(Threads REQUIRED)
target_link_libraries(water_iot_common Threads::Threads)

# 🏗️ 条件编译
if(HIREDIS_FOUND)
    target_link_libraries(water_iot_storage ${HIREDIS_LIBRARIES})
    add_compile_definitions(HAVE_HIREDIS)
endif()
```

## 🎯 第六部分：常见问题和解决方案

### ❗ 问题1：找不到头文件

```
错误: fatal error: 'my_header.h' file not found
```

**解决方案：**
```cmake
# 添加头文件搜索路径
target_include_directories(my_target PRIVATE path/to/headers)
```

### ❗ 问题2：链接错误

```
错误: undefined reference to 'MyClass::myFunction()'
```

**解决方案：**
```cmake
# 确保链接了包含该函数的库
target_link_libraries(my_target required_library)
```

### ❗ 问题3：第三方库找不到

```
错误: Could NOT find SomeLibrary
```

**解决方案：**
```cmake
# 手动指定库路径
find_path(SOMELIB_INCLUDE_DIR somelibrary.h)
find_library(SOMELIB_LIBRARY somelibrary)

if(SOMELIB_INCLUDE_DIR AND SOMELIB_LIBRARY)
    target_include_directories(my_target PRIVATE ${SOMELIB_INCLUDE_DIR})
    target_link_libraries(my_target ${SOMELIB_LIBRARY})
endif()
```

## 🏆 第七部分：高级技巧

### 🎨 自定义函数

```cmake
# 定义自定义函数
function(add_my_library LIB_NAME)
    add_library(${LIB_NAME} STATIC ${ARGN})  # ARGN = 剩余的参数
    target_include_directories(${LIB_NAME} PUBLIC include/)
    target_compile_options(${LIB_NAME} PRIVATE -Wall)
endfunction()

# 使用自定义函数
add_my_library(my_lib source1.cpp source2.cpp source3.cpp)
```

### 🔧 生成器表达式

```cmake
# 根据构建类型设置不同选项
target_compile_options(my_target PRIVATE
    $<$<CONFIG:Debug>:-g -O0>        # Debug模式
    $<$<CONFIG:Release>:-O3 -DNDEBUG> # Release模式
)

# 根据编译器设置不同选项
target_compile_options(my_target PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall>     # GCC编译器
    $<$<CXX_COMPILER_ID:MSVC>:/W4>      # MSVC编译器
)
```

## 🎯 实际操作指南

### 🚀 如何编译我们的示例

**简单计算器示例：**
```bash
cd WaterConservancyIoT/cmake_examples/simple_example
mkdir build && cd build
cmake ..
cmake --build .
./calculator  # Linux/Mac
calculator.exe  # Windows
```

**库使用示例：**
```bash
cd WaterConservancyIoT/cmake_examples/library_example
mkdir build && cd build
cmake ..
cmake --build .
./main_program  # Linux/Mac
main_program.exe  # Windows
```

### 📊 编译过程详解

```
1. 📋 cmake .. 
   └── 读取CMakeLists.txt，生成构建文件

2. 🔍 检测系统
   ├── 找到编译器
   ├── 检查依赖库
   └── 配置构建环境

3. 📁 生成构建文件
   ├── Linux: Makefile
   ├── Windows: .sln + .vcxproj
   └── macOS: Xcode项目

4. 🔨 cmake --build .
   ├── 编译源文件为目标文件 (.o/.obj)
   ├── 创建库文件 (.a/.lib/.so/.dll)
   └── 链接生成最终程序
```

## 🎓 总结和下一步

### ✅ 您现在应该理解的概念

1. **CMake的作用** - 跨平台构建工具
2. **基本语法** - project(), add_executable(), target_link_libraries()
3. **Target概念** - 可执行文件、静态库、动态库
4. **依赖管理** - 如何链接库和管理包含目录
5. **变量使用** - set(), ${}, 系统预定义变量

### 🚀 练习建议

1. **动手试试** - 运行我们提供的示例
2. **修改练习** - 尝试添加新的源文件和库
3. **观察输出** - 看看cmake命令的输出信息
4. **解决问题** - 遇到错误时学会读错误信息

### 🎯 应用到我们的项目

现在您应该能够理解我们项目中CMakeLists.txt的每一行了：
- 为什么要创建这么多库？（模块化）
- 为什么要设置依赖关系？（确保编译顺序）
- 为什么要包含那些目录？（让编译器找到头文件）

**记住：CMake就像一个聪明的助手，帮您管理复杂的编译过程！** 🤖
