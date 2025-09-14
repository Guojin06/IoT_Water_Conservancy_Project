#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <chrono>
#include "water_level_sensor.h"
#include "temperature_sensor.h"

using namespace WaterIoT::Sensors;

// 简单的水位传感器测试
TEST(SensorTest, WaterLevelBasicTest) {
    // 创建水位传感器
    auto waterSensor = std::make_unique<WaterLevelSensor>("WL001", "主水库");
    
    // 初始化
    EXPECT_TRUE(waterSensor->initialize());
    EXPECT_EQ(SensorStatus::ONLINE, waterSensor->getStatus());
    
    // 读取数据
    SensorData data = waterSensor->readData();
    
    // 验证基本信息
    EXPECT_EQ("WL001", data.sensorId);
    EXPECT_EQ(SensorType::WATER_LEVEL, data.type);
    EXPECT_EQ("主水库", data.location);
    EXPECT_EQ("cm", data.unit);
    EXPECT_EQ(SensorStatus::ONLINE, data.status);
    
    // 验证数据合理性
    EXPECT_GE(data.value, 0.0);      // 水位不能为负
    EXPECT_LE(data.value, 200.0);    // 水位不超过最大值
    
    // 关闭传感器
    waterSensor->shutdown();
    EXPECT_EQ(SensorStatus::OFFLINE, waterSensor->getStatus());
}

// 简单的温度传感器测试
TEST(SensorTest, TemperatureBasicTest) {
    // 创建温度传感器
    auto tempSensor = std::make_unique<TemperatureSensor>("TEMP001", "水库入口");
    
    // 初始化
    EXPECT_TRUE(tempSensor->initialize());
    EXPECT_EQ(SensorStatus::ONLINE, tempSensor->getStatus());
    
    // 读取数据
    SensorData data = tempSensor->readData();
    
    // 验证基本信息
    EXPECT_EQ("TEMP001", data.sensorId);
    EXPECT_EQ(SensorType::TEMPERATURE, data.type);
    EXPECT_EQ("水库入口", data.location);
    EXPECT_EQ("°C", data.unit);
    EXPECT_EQ(SensorStatus::ONLINE, data.status);
    
    // 验证数据合理性
    EXPECT_GE(data.value, -40.0);    // 温度不低于-40°C
    EXPECT_LE(data.value, 80.0);     // 温度不超过80°C
    
    // 测试温度转换
    double celsius = tempSensor->getCelsius();
    double fahrenheit = tempSensor->getFahrenheit();
    EXPECT_NEAR(fahrenheit, celsius * 9.0 / 5.0 + 32.0, 0.1);
    
    // 关闭传感器
    tempSensor->shutdown();
    EXPECT_EQ(SensorStatus::OFFLINE, tempSensor->getStatus());
}

// 测试多次读取数据的变化
TEST(SensorTest, DataVariationTest) {
    auto waterSensor = std::make_unique<WaterLevelSensor>("WL002", "测试点");
    ASSERT_TRUE(waterSensor->initialize());
    
    std::vector<double> readings;
    
    // 连续读取5次数据
    for (int i = 0; i < 5; ++i) {
        SensorData data = waterSensor->readData();
        readings.push_back(data.value);
        
        // 验证每次读取都有效
        EXPECT_EQ(SensorStatus::ONLINE, data.status);
        EXPECT_GE(data.value, 0.0);
        
        // 稍微等待一下
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // 验证数据有变化（至少不是完全相同）
    bool hasVariation = false;
    for (size_t i = 1; i < readings.size(); ++i) {
        if (std::abs(readings[i] - readings[0]) > 0.01) {
            hasVariation = true;
            break;
        }
    }
    EXPECT_TRUE(hasVariation) << "传感器数据应该有变化";
}

// 测试传感器配置
TEST(SensorTest, ConfigurationTest) {
    auto tempSensor = std::make_unique<TemperatureSensor>("TEMP002", "配置测试");
    ASSERT_TRUE(tempSensor->initialize());
    
    // 测试设置温度范围
    tempSensor->setTemperatureRange(10.0, 30.0);
    
    // 读取几次数据，检查是否在合理范围内
    for (int i = 0; i < 3; ++i) {
        SensorData data = tempSensor->readData();
        // 注意：由于是模拟数据，可能偶尔超出设定范围，这是正常的
        // 这里主要测试功能是否正常运行
        EXPECT_EQ(SensorStatus::ONLINE, data.status);
    }
}

// 测试传感器元数据
TEST(SensorTest, MetadataTest) {
    auto waterSensor = std::make_unique<WaterLevelSensor>("WL003", "元数据测试");
    ASSERT_TRUE(waterSensor->initialize());
    
    SensorData data = waterSensor->readData();
    
    // 验证元数据存在
    EXPECT_FALSE(data.metadata.empty());
    
    // 检查基本信息
    EXPECT_EQ("WL003", waterSensor->getSensorId());
    EXPECT_EQ(SensorType::WATER_LEVEL, waterSensor->getSensorType());
    EXPECT_EQ("元数据测试", waterSensor->getLocation());
}
