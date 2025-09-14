#include <iostream>
#include <vector>
#include "src/sensors/include/water_level_sensor.h"
#include "src/storage/include/redis_storage.h"
#include "src/data_processor/include/data_processor.h"

using namespace WaterIoT;

int main() {
    std::cout << "=== 简单模块测试 ===" << std::endl;
    
    try {
        // 1. 测试传感器
        std::cout << "\n📡 测试传感器..." << std::endl;
        Sensors::WaterLevelSensor sensor("TEST001", "测试位置");
        auto data = sensor.readData();
        std::cout << "传感器ID: " << data.sensorId << std::endl;
        std::cout << "数值: " << data.value << std::endl;
        
        // 2. 测试Redis存储
        std::cout << "\n🗃️ 测试Redis存储..." << std::endl;
        Storage::MockRedisStorage redis;
        bool connected = redis.connect("localhost", 6379);
        std::cout << "连接结果: " << (connected ? "成功" : "失败") << std::endl;
        
        auto result = redis.storeSensorData(data);
        std::cout << "存储结果: " << result.message << std::endl;
        
        // 3. 测试数据处理
        std::cout << "\n📊 测试数据处理..." << std::endl;
        DataProcessor::WaterDataProcessor processor(0.0, 100.0, 2.0);
        std::vector<Sensors::SensorData> testData = {data};
        auto processed = processor.processDataPipeline(testData);
        std::cout << "处理摘要: " << processed.summary << std::endl;
        
        std::cout << "\n✅ 所有模块测试通过！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
