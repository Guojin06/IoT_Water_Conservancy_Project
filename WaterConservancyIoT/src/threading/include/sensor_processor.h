#pragma once

#include "thread_pool.h"
#include "../../sensors/include/sensor_base.h"
#include "../../storage/include/redis_storage.h"
#include "../../data_processor/include/data_processor.h"
#include <memory>
#include <atomic>
#include <chrono>

namespace WaterIoT::Threading {

/**
 * @brief 多线程传感器数据处理系统
 */
class SensorProcessor {
public:
    SensorProcessor(
        std::shared_ptr<Storage::IDataStorage> storage,
        std::shared_ptr<DataProcessor::WaterDataProcessor> processor,
        size_t numThreads = 4
    );
    
    ~SensorProcessor();
    
    // 启动/停止处理
    void start();
    void stop();
    bool isRunning() const { return running_; }
    
    // 添加传感器数据
    bool addSensorData(const Sensors::SensorData& data);
    
    // 获取统计信息
    struct Statistics {
        size_t totalProcessed = 0;
        size_t totalErrors = 0;
        size_t queueSize = 0;
        double averageProcessingTime = 0.0;
    };
    
    Statistics getStatistics() const;
    void printStatistics() const;

private:
    // 核心组件
    std::shared_ptr<Storage::IDataStorage> storage_;
    std::shared_ptr<DataProcessor::WaterDataProcessor> processor_;
    std::unique_ptr<ThreadPool> thread_pool_;
    std::unique_ptr<ProducerConsumer<Sensors::SensorData>> data_queue_;
    
    // 控制变量
    std::atomic<bool> running_;
    std::vector<std::thread> worker_threads_;
    
    // 统计信息
    mutable std::mutex stats_mutex_;
    std::atomic<size_t> total_processed_;
    std::atomic<size_t> total_errors_;
    std::atomic<double> total_processing_time_;
    
    // 工作线程函数
    void dataProcessingWorker();
    void storageWorker();
    
    // 处理单个数据项
    void processSingleData(const Sensors::SensorData& data);
};

/**
 * @brief 传感器数据生成器（模拟多个传感器）
 */
class SensorSimulator {
public:
    explicit SensorSimulator(SensorProcessor& processor);
    ~SensorSimulator();
    
    // 添加传感器
    void addSensor(std::shared_ptr<Sensors::ISensor> sensor, 
                   std::chrono::milliseconds interval = std::chrono::milliseconds(1000));
    
    // 控制模拟
    void start();
    void stop();
    bool isRunning() const { return running_; }
    
    // 获取模拟统计
    size_t getTotalGenerated() const { return total_generated_; }
    size_t getActiveSensors() const { return sensor_threads_.size(); }

private:
    SensorProcessor& processor_;
    std::atomic<bool> running_;
    std::vector<std::thread> sensor_threads_;
    std::atomic<size_t> total_generated_;
    
    struct SensorInfo {
        std::shared_ptr<Sensors::ISensor> sensor;
        std::chrono::milliseconds interval;
    };
    
    std::vector<SensorInfo> sensors_;
    
    // 单个传感器模拟线程
    void sensorWorker(SensorInfo sensorInfo);
};

} // namespace WaterIoT::Threading
