cmake_minimum_required(VERSION 3.16)

# 项目信息
project(WaterConservancyIoT 
    VERSION 1.0.0
    DESCRIPTION "智能水库监测与调度系统"
    LANGUAGES CXX)

# C++标准设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# 构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
    if(NOT MSVC)
        add_compile_options(-g -O0)
    endif()
else()
    if(NOT MSVC)
        add_compile_options(-O3)
    endif()
endif()

# 输出目录设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找依赖包
find_package(Threads REQUIRED)
find_package(PkgConfig QUIET)

# Redis客户端库 (hiredis)
if(PkgConfig_FOUND)
    pkg_check_modules(HIREDIS QUIET hiredis)
endif()

if(NOT HIREDIS_FOUND)
    message(STATUS "hiredis not found via pkg-config, will use manual detection")
    find_path(HIREDIS_INCLUDE_DIR hiredis/hiredis.h)
    find_library(HIREDIS_LIBRARY hiredis)
    if(HIREDIS_INCLUDE_DIR AND HIREDIS_LIBRARY)
        set(HIREDIS_FOUND TRUE)
        set(HIREDIS_INCLUDE_DIRS ${HIREDIS_INCLUDE_DIR})
        set(HIREDIS_LIBRARIES ${HIREDIS_LIBRARY})
    endif()
endif()

# 第三方库目录
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

# nlohmann/json
set(JSON_INCLUDE_DIR ${EXTERNAL_DIR}/nlohmann)
if(NOT EXISTS ${JSON_INCLUDE_DIR}/nlohmann/json.hpp)
    message(STATUS "Downloading nlohmann/json...")
    file(MAKE_DIRECTORY ${JSON_INCLUDE_DIR}/nlohmann)
    file(DOWNLOAD 
        https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
        ${JSON_INCLUDE_DIR}/nlohmann/json.hpp
        TIMEOUT 30
        STATUS JSON_DOWNLOAD_STATUS
    )
    list(GET JSON_DOWNLOAD_STATUS 0 JSON_DOWNLOAD_ERROR)
    if(JSON_DOWNLOAD_ERROR)
        message(WARNING "Failed to download nlohmann/json, please download manually")
    endif()
endif()

# httplib
set(HTTPLIB_INCLUDE_DIR ${EXTERNAL_DIR}/httplib)
if(NOT EXISTS ${HTTPLIB_INCLUDE_DIR}/httplib.h)
    message(STATUS "Downloading httplib...")
    file(MAKE_DIRECTORY ${HTTPLIB_INCLUDE_DIR})
    file(DOWNLOAD 
        https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.14.1.tar.gz
        ${CMAKE_BINARY_DIR}/httplib.tar.gz
        TIMEOUT 30
        STATUS HTTPLIB_DOWNLOAD_STATUS
    )
    list(GET HTTPLIB_DOWNLOAD_STATUS 0 HTTPLIB_DOWNLOAD_ERROR)
    if(NOT HTTPLIB_DOWNLOAD_ERROR)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_BINARY_DIR}/httplib.tar.gz
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        file(COPY ${CMAKE_BINARY_DIR}/cpp-httplib-0.14.1/httplib.h 
             DESTINATION ${HTTPLIB_INCLUDE_DIR})
    else()
        message(WARNING "Failed to download httplib, please download manually")
    endif()
endif()

# Google Test
option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
    set(GTEST_DIR ${EXTERNAL_DIR}/googletest)
    if(NOT EXISTS ${GTEST_DIR}/CMakeLists.txt)
        message(STATUS "Downloading Google Test...")
        file(DOWNLOAD 
            https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
            ${CMAKE_BINARY_DIR}/gtest.tar.gz
            TIMEOUT 30
            STATUS GTEST_DOWNLOAD_STATUS
        )
        list(GET GTEST_DOWNLOAD_STATUS 0 GTEST_DOWNLOAD_ERROR)
        if(NOT GTEST_DOWNLOAD_ERROR)
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_BINARY_DIR}/gtest.tar.gz
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            )
            file(RENAME ${CMAKE_BINARY_DIR}/googletest-1.14.0 ${GTEST_DIR})
        else()
            message(WARNING "Failed to download Google Test")
            set(BUILD_TESTING OFF)
        endif()
    endif()
    
    if(EXISTS ${GTEST_DIR}/CMakeLists.txt)
        # 禁用 Google Test 的安装
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
        add_subdirectory(${GTEST_DIR} EXCLUDE_FROM_ALL)
    endif()
endif()

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensors/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/data_processor/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/storage/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/api_server/include
    ${JSON_INCLUDE_DIR}
    ${HTTPLIB_INCLUDE_DIR}
)

if(HIREDIS_FOUND)
    include_directories(${HIREDIS_INCLUDE_DIRS})
    add_compile_definitions(HAVE_HIREDIS)
endif()

# 源文件收集
file(GLOB_RECURSE COMMON_SOURCES 
    "src/common/src/*.cpp"
)

file(GLOB_RECURSE SENSOR_SOURCES 
    "src/sensors/src/*.cpp"
)

file(GLOB_RECURSE DATA_PROCESSOR_SOURCES 
    "src/data_processor/src/*.cpp"
)

file(GLOB_RECURSE STORAGE_SOURCES 
    "src/storage/src/*.cpp"
)

file(GLOB_RECURSE API_SERVER_SOURCES 
    "src/api_server/src/*.cpp"
)

# 创建静态库
add_library(water_iot_common STATIC ${COMMON_SOURCES})
add_library(water_iot_sensors STATIC ${SENSOR_SOURCES})
add_library(water_iot_data_processor STATIC ${DATA_PROCESSOR_SOURCES})
add_library(water_iot_storage STATIC ${STORAGE_SOURCES})
add_library(water_iot_api_server STATIC ${API_SERVER_SOURCES})

# 库依赖关系
target_link_libraries(water_iot_sensors water_iot_common)
target_link_libraries(water_iot_data_processor water_iot_common water_iot_sensors)
target_link_libraries(water_iot_storage water_iot_common)
target_link_libraries(water_iot_api_server water_iot_common water_iot_data_processor water_iot_storage)

# 系统库链接
target_link_libraries(water_iot_common Threads::Threads)

if(HIREDIS_FOUND)
    target_link_libraries(water_iot_storage ${HIREDIS_LIBRARIES})
endif()

# 主程序
add_executable(water_iot_server src/main.cpp)
target_link_libraries(water_iot_server 
    water_iot_api_server
    water_iot_data_processor
    water_iot_storage
    water_iot_sensors
    water_iot_common
)

# 测试
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# 安装配置
install(TARGETS water_iot_server DESTINATION bin)
install(DIRECTORY config/ DESTINATION etc/water_iot)

# 打包配置
set(CPACK_PACKAGE_NAME "WaterConservancyIoT")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "WaterIoT Team")
set(CPACK_PACKAGE_CONTACT "developer@wateriot.com")

if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
else()
    set(CPACK_GENERATOR "TGZ;DEB")
endif()

include(CPack)

# 打印配置信息
message(STATUS "")
message(STATUS "=== 构建配置信息 ===")
message(STATUS "项目名称: ${PROJECT_NAME}")
message(STATUS "版本: ${PROJECT_VERSION}")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "编译器: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Redis支持: ${HIREDIS_FOUND}")
message(STATUS "构建测试: ${BUILD_TESTING}")
message(STATUS "安装前缀: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "===================")
message(STATUS "")
