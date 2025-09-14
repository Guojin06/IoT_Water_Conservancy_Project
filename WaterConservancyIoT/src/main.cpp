#include <iostream>
#include <vector>
#include "common/include/logger.h"
#include "common/include/config.h" 
#include "common/include/utils.h"
#include "sensors/include/water_level_sensor.h"
#include "sensors/include/temperature_sensor.h"
#include "data_processor/include/data_processor.h"
#include "storage/include/redis_storage.h"

using namespace WaterIoT;

int main() {
    std::cout << "=== 智能水库监测系统 - 第3天测试 ===" << std::endl;
    
    // 1. 创建传感器并生成模拟数据
    Sensors::WaterLevelSensor waterSensor("WL001", "主水库");
    Sensors::TemperatureSensor tempSensor("TEMP001", "入口温度");
    
    std::vector<Sensors::SensorData> testData;
    
    // 生成一些测试数据（包含正常值和异常值）
    for (int i = 0; i < 10; ++i) {
        auto waterData = waterSensor.readData();
        auto tempData = tempSensor.readData();
        testData.push_back(waterData);
        testData.push_back(tempData);
    }
    
    // 手动添加一些异常值进行测试
    Sensors::SensorData outlierData;
    outlierData.sensorId = "OUTLIER1";
    outlierData.type = Sensors::SensorType::WATER_LEVEL;
    outlierData.value = 999.9;
    outlierData.timestamp = std::chrono::system_clock::now();
    outlierData.location = "test_location";
    outlierData.status = Sensors::SensorStatus::ONLINE;
    testData.push_back(outlierData);
    
    Sensors::SensorData invalidData;
    invalidData.sensorId = "INVALID";
    invalidData.type = Sensors::SensorType::TEMPERATURE;
    invalidData.value = std::numeric_limits<double>::quiet_NaN();
    invalidData.timestamp = std::chrono::system_clock::now();
    invalidData.location = "test_location";
    invalidData.status = Sensors::SensorStatus::ERROR;
    testData.push_back(invalidData);
    
    std::cout << "\n📊 原始数据总数: " << testData.size() << " 条" << std::endl;
    
    // 2. 创建数据处理器
    DataProcessor::WaterDataProcessor processor(0.0, 100.0, 2.0);
    
    // 3. 执行完整的数据处理流水线
    std::cout << "\n🔄 开始数据处理流水线..." << std::endl;
    auto result = processor.processDataPipeline(testData);
    
    // 4. 显示处理结果
    std::cout << "\n📈 数据处理结果:" << std::endl;
    std::cout << "  数据点数量: " << result.count << std::endl;
    std::cout << "  平均值: " << result.average << std::endl;
    std::cout << "  最小值: " << result.minimum << std::endl;
    std::cout << "  最大值: " << result.maximum << std::endl;
    std::cout << "  标准差: " << result.standardDev << std::endl;
    std::cout << "  摘要: " << result.summary << std::endl;
    
    // 5. 测试分步处理
    std::cout << "\n🔍 分步处理测试:" << std::endl;
    auto cleaned = processor.cleanData(testData);
    std::cout << "  清洗: " << cleaned.processInfo << std::endl;
    
    auto filtered = processor.filterData(cleaned);
    std::cout << "  过滤: " << filtered.processInfo << std::endl;
    
    auto aggregated = processor.aggregateData(filtered);
    std::cout << "  聚合: " << aggregated.summary << std::endl;
    
    std::cout << "\n✅ 数据处理模块测试完成！" << std::endl;
    
    // 6. 测试Redis存储模块
    std::cout << "\n🗃️ 测试Redis存储模块..." << std::endl;
    
    // 创建Redis存储实例
    Storage::MockRedisStorage redisStorage;
    
    // 连接Redis
    bool connected = redisStorage.connect("localhost", 6379);
    std::cout << "Redis连接结果: " << (connected ? "成功" : "失败") << std::endl;
    
    // 存储传感器数据
    std::cout << "\n📤 存储传感器数据..." << std::endl;
    auto storeResult = redisStorage.storeBatchData(testData);
    std::cout << "批量存储结果: " << storeResult.message 
              << " (成功: " << storeResult.affectedCount << "条)" << std::endl;
    
    // 存储处理结果
    std::cout << "\n📤 存储处理结果..." << std::endl;
    auto resultStoreResult = redisStorage.storeProcessedResult("daily_summary", result);
    std::cout << "处理结果存储: " << resultStoreResult.message << std::endl;
    
    // 测试缓存功能
    std::cout << "\n🗂️ 测试缓存功能..." << std::endl;
    redisStorage.setCache("current_temp", "23.5", 300);  // 5分钟过期
    redisStorage.setCache("system_status", "online", 0); // 永不过期
    
    auto tempValue = redisStorage.getCache("current_temp");
    auto statusValue = redisStorage.getCache("system_status");
    
    std::cout << "缓存读取 - 当前温度: " << (tempValue ? *tempValue : "未找到") << std::endl;
    std::cout << "缓存读取 - 系统状态: " << (statusValue ? *statusValue : "未找到") << std::endl;
    
    // 查询最近数据
    std::cout << "\n🔍 查询最近数据..." << std::endl;
    auto recentData = redisStorage.getRecentData("WL001", 3);
    std::cout << "WL001最近3条数据: " << recentData.size() << "条" << std::endl;
    for (const auto& data : recentData) {
        std::cout << "  - " << data.sensorId << ": " << data.value << std::endl;
    }
    
    // 显示存储统计
    redisStorage.printStats();
    
    // 断开连接
    redisStorage.disconnect();
    
    std::cout << "\n✅ Redis存储模块测试完成！" << std::endl;
    std::cout << "\n🎉 第3天模块测试全部完成！" << std::endl;
    return 0;
}