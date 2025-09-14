# 测试构建配置

# 创建一个简单的测试，即使没有Google Test也能运行
add_executable(basic_test basic_test.cpp)

# 链接我们的库
target_link_libraries(basic_test 
    water_iot_common
    water_iot_sensors
)

# 设置包含目录
target_include_directories(basic_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/common/include
    ${CMAKE_SOURCE_DIR}/src/sensors/include
)

# 添加到测试
add_test(NAME BasicTest COMMAND basic_test)

# 如果有Google Test，添加传感器测试
if(TARGET gtest_main)
    add_executable(sensor_test test_sensors_simple.cpp)
    
    target_link_libraries(sensor_test 
        water_iot_common
        water_iot_sensors
        gtest_main
    )
    
    target_include_directories(sensor_test PRIVATE
        ${CMAKE_SOURCE_DIR}/src/common/include
        ${CMAKE_SOURCE_DIR}/src/sensors/include
    )
    
    add_test(NAME SensorTest COMMAND sensor_test)
endif()
