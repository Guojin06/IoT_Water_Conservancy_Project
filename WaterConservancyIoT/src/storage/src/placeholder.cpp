#include "../include/redis_storage.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace WaterIoT::Storage {

MockRedisStorage::MockRedisStorage() : status_(ConnectionStatus::DISCONNECTED), host_(""), port_(0) {}

bool MockRedisStorage::connect(const std::string& host, int port, const std::string& password) {
    host_ = host;
    port_ = port;
    
    // 模拟连接过程
    std::cout << "🔌 连接Redis服务器: " << host << ":" << port << std::endl;
    
    // 模拟连接成功
    status_ = ConnectionStatus::CONNECTED;
    std::cout << "✅ Redis连接成功！" << std::endl;
    return true;
}

void MockRedisStorage::disconnect() {
    status_ = ConnectionStatus::DISCONNECTED;
    std::cout << "🔌 Redis连接已断开" << std::endl;
}

ConnectionStatus MockRedisStorage::getStatus() const {
    return status_;
}

StorageResult MockRedisStorage::storeSensorData(const WaterIoT::Sensors::SensorData& data) {
    if (status_ != ConnectionStatus::CONNECTED) {
        return StorageResult(false, "Redis未连接", 0);
    }
    
    // 模拟存储传感器数据
    mockDataStore_.push_back(data);
    
    // 同时更新缓存（最新数据）
    std::string cacheKey = "latest:" + data.sensorId;
    std::string cacheValue = std::to_string(data.value);
    mockCacheStore_[cacheKey] = cacheValue;
    
    return StorageResult(true, "传感器数据存储成功", 1);
}

StorageResult MockRedisStorage::storeBatchData(const std::vector<WaterIoT::Sensors::SensorData>& dataList) {
    if (status_ != ConnectionStatus::CONNECTED) {
        return StorageResult(false, "Redis未连接", 0);
    }
    
    size_t successCount = 0;
    for (const auto& data : dataList) {
        auto result = storeSensorData(data);
        if (result.success) {
            successCount++;
        }
    }
    
    return StorageResult(successCount == dataList.size(), 
                        "批量存储完成", successCount);
}

StorageResult MockRedisStorage::storeProcessedResult(const std::string& key, 
                                                    const WaterIoT::DataProcessor::AggregatedResult& result) {
    if (status_ != ConnectionStatus::CONNECTED) {
        return StorageResult(false, "Redis未连接", 0);
    }
    
    // 存储处理结果
    mockResultStore_[key] = result;
    
    // 同时缓存结果摘要
    std::string cacheKey = "summary:" + key;
    mockCacheStore_[cacheKey] = result.summary;
    
    return StorageResult(true, "处理结果存储成功", 1);
}

std::optional<WaterIoT::Sensors::SensorData> MockRedisStorage::getSensorData(const std::string& sensorId, 
                                                                             const std::string& timestamp) {
    if (status_ != ConnectionStatus::CONNECTED) {
        return std::nullopt;
    }
    
    // 模拟根据ID和时间戳查找数据
    for (const auto& data : mockDataStore_) {
        if (data.sensorId == sensorId) {
            // 简化：返回找到的第一个匹配ID的数据
            return data;
        }
    }
    
    return std::nullopt;
}

std::vector<WaterIoT::Sensors::SensorData> MockRedisStorage::getRecentData(const std::string& sensorId, int count) {
    std::vector<WaterIoT::Sensors::SensorData> result;
    
    if (status_ != ConnectionStatus::CONNECTED) {
        return result;
    }
    
    // 找到指定传感器的数据
    for (auto it = mockDataStore_.rbegin(); it != mockDataStore_.rend() && result.size() < static_cast<size_t>(count); ++it) {
        if (it->sensorId == sensorId) {
            result.push_back(*it);
        }
    }
    
    return result;
}

StorageResult MockRedisStorage::setCache(const std::string& key, const std::string& value, int expireSeconds) {
    if (status_ != ConnectionStatus::CONNECTED) {
        return StorageResult(false, "Redis未连接", 0);
    }
    
    mockCacheStore_[key] = value;
    // 注意：这里忽略了expireSeconds，实际Redis会自动过期
    
    return StorageResult(true, "缓存设置成功", 1);
}

std::optional<std::string> MockRedisStorage::getCache(const std::string& key) {
    if (status_ != ConnectionStatus::CONNECTED) {
        return std::nullopt;
    }
    
    auto it = mockCacheStore_.find(key);
    if (it != mockCacheStore_.end()) {
        return it->second;
    }
    
    return std::nullopt;
}

bool MockRedisStorage::deleteCache(const std::string& key) {
    if (status_ != ConnectionStatus::CONNECTED) {
        return false;
    }
    
    auto erased = mockCacheStore_.erase(key);
    return erased > 0;
}

void MockRedisStorage::printStats() const {
    std::cout << "\n📊 Redis存储统计:" << std::endl;
    std::cout << "  连接状态: " << (status_ == ConnectionStatus::CONNECTED ? "已连接" : "未连接") << std::endl;
    std::cout << "  传感器数据: " << mockDataStore_.size() << " 条" << std::endl;
    std::cout << "  缓存数据: " << mockCacheStore_.size() << " 条" << std::endl;
    std::cout << "  处理结果: " << mockResultStore_.size() << " 条" << std::endl;
}

} // namespace WaterIoT::Storage
