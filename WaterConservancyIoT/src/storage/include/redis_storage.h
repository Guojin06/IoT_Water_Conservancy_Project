#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <map>
#include "../../sensors/include/sensor_base.h"
#include "../../data_processor/include/data_processor.h"

namespace WaterIoT::Storage {

// Redis连接状态
enum class ConnectionStatus {
    CONNECTED,
    DISCONNECTED,
    ERROR
};

// 存储结果
struct StorageResult {
    bool success;
    std::string message;
    size_t affectedCount;
    
    StorageResult(bool s, const std::string& msg, size_t count = 0)
        : success(s), message(msg), affectedCount(count) {}
};

// 存储接口
class IDataStorage {
public:
    virtual ~IDataStorage() = default;
    
    // 连接管理
    virtual bool connect(const std::string& host, int port, const std::string& password = "") = 0;
    virtual void disconnect() = 0;
    virtual ConnectionStatus getStatus() const = 0;
    
    // 传感器数据存储
    virtual StorageResult storeSensorData(const WaterIoT::Sensors::SensorData& data) = 0;
    virtual StorageResult storeBatchData(const std::vector<WaterIoT::Sensors::SensorData>& dataList) = 0;
    
    // 处理结果存储
    virtual StorageResult storeProcessedResult(const std::string& key, 
                                             const WaterIoT::DataProcessor::AggregatedResult& result) = 0;
    
    // 数据查询
    virtual std::optional<WaterIoT::Sensors::SensorData> getSensorData(const std::string& sensorId, 
                                                                       const std::string& timestamp) = 0;
    virtual std::vector<WaterIoT::Sensors::SensorData> getRecentData(const std::string& sensorId, 
                                                                     int count = 10) = 0;
    
    // 缓存管理
    virtual StorageResult setCache(const std::string& key, const std::string& value, int expireSeconds = 0) = 0;
    virtual std::optional<std::string> getCache(const std::string& key) = 0;
    virtual bool deleteCache(const std::string& key) = 0;
};

// Redis存储实现（模拟版本，无需真实Redis服务器）
class MockRedisStorage : public IDataStorage {
private:
    ConnectionStatus status_;
    std::string host_;
    int port_;
    
    // 模拟存储（实际项目中这些数据在Redis服务器中）
    std::vector<WaterIoT::Sensors::SensorData> mockDataStore_;
    std::map<std::string, std::string> mockCacheStore_;
    std::map<std::string, WaterIoT::DataProcessor::AggregatedResult> mockResultStore_;

public:
    MockRedisStorage();
    
    // 连接管理
    bool connect(const std::string& host, int port, const std::string& password = "") override;
    void disconnect() override;
    ConnectionStatus getStatus() const override;
    
    // 数据存储
    StorageResult storeSensorData(const WaterIoT::Sensors::SensorData& data) override;
    StorageResult storeBatchData(const std::vector<WaterIoT::Sensors::SensorData>& dataList) override;
    StorageResult storeProcessedResult(const std::string& key, 
                                     const WaterIoT::DataProcessor::AggregatedResult& result) override;
    
    // 数据查询
    std::optional<WaterIoT::Sensors::SensorData> getSensorData(const std::string& sensorId, 
                                                               const std::string& timestamp) override;
    std::vector<WaterIoT::Sensors::SensorData> getRecentData(const std::string& sensorId, 
                                                             int count = 10) override;
    
    // 缓存管理
    StorageResult setCache(const std::string& key, const std::string& value, int expireSeconds = 0) override;
    std::optional<std::string> getCache(const std::string& key) override;
    bool deleteCache(const std::string& key) override;
    
    // 调试方法
    void printStats() const;
};

} // namespace WaterIoT::Storage
