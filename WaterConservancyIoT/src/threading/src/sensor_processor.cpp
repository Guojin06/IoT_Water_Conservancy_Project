#include "../include/sensor_processor.h"
#include <iostream>
#include <chrono>

namespace WaterIoT::Threading {

SensorProcessor::SensorProcessor(
    std::shared_ptr<Storage::IDataStorage> storage,
    std::shared_ptr<DataProcessor::WaterDataProcessor> processor,
    size_t numThreads
) : storage_(storage), processor_(processor), running_(false),
    total_processed_(0), total_errors_(0), total_processing_time_(0.0) {
    
    thread_pool_ = std::make_unique<ThreadPool>(numThreads);
    data_queue_ = std::make_unique<ProducerConsumer<Sensors::SensorData>>(1000);
    
    std::cout << "🏭 传感器处理器初始化完成" << std::endl;
}

SensorProcessor::~SensorProcessor() {
    stop();
    std::cout << "🏭 传感器处理器已销毁" << std::endl;
}

void SensorProcessor::start() {
    if (running_) return;
    
    running_ = true;
    
    // 启动数据处理工作线程
    worker_threads_.emplace_back([this] { dataProcessingWorker(); });
    worker_threads_.emplace_back([this] { storageWorker(); });
    
    std::cout << "🚀 传感器处理系统启动" << std::endl;
}

void SensorProcessor::stop() {
    if (!running_) return;
    
    running_ = false;
    data_queue_->stop();
    
    for (auto& thread : worker_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    worker_threads_.clear();
    
    std::cout << "🛑 传感器处理系统停止" << std::endl;
}

bool SensorProcessor::addSensorData(const Sensors::SensorData& data) {
    if (!running_) return false;
    
    return data_queue_->produce(data);
}

void SensorProcessor::dataProcessingWorker() {
    std::cout << "📊 数据处理工作线程启动" << std::endl;
    
    while (running_ || !data_queue_->empty()) {
        Sensors::SensorData data;
        
        if (data_queue_->consume(data, std::chrono::milliseconds(100))) {
            auto start = std::chrono::high_resolution_clock::now();
            
            try {
                processSingleData(data);
                
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration<double, std::milli>(end - start);
                
                total_processed_++;
                total_processing_time_ += duration.count();
                
            } catch (const std::exception& e) {
                std::cout << "❌ 数据处理错误: " << e.what() << std::endl;
                total_errors_++;
            }
        }
    }
    
    std::cout << "📊 数据处理工作线程结束" << std::endl;
}

void SensorProcessor::storageWorker() {
    std::cout << "💾 存储工作线程启动" << std::endl;
    
    // 这里可以实现批量存储优化
    // 例如：收集一定数量的数据后批量写入Redis
    
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // 存储相关的后台任务
    }
    
    std::cout << "💾 存储工作线程结束" << std::endl;
}

void SensorProcessor::processSingleData(const Sensors::SensorData& data) {
    // 1. 数据验证和清洗
    if (std::isnan(data.value) || std::isinf(data.value)) {
        throw std::runtime_error("无效数据值: " + std::to_string(data.value));
    }
    
    // 2. 存储到Redis
    if (storage_) {
        auto result = storage_->storeSensorData(data);
        if (!result.success) {
            throw std::runtime_error("存储失败: " + result.message);
        }
        
        // 3. 更新缓存
        std::string cacheKey = "latest:" + data.sensorId;
        storage_->setCache(cacheKey, std::to_string(data.value), 300);
    }
    
    // 4. 检查是否需要告警
    // 这里可以添加告警逻辑
    if (data.value > 90.0) {  // 假设90以上为高水位
        std::cout << "⚠️ 高水位告警: " << data.sensorId 
                  << " = " << data.value << std::endl;
    }
}

SensorProcessor::Statistics SensorProcessor::getStatistics() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    
    Statistics stats;
    stats.totalProcessed = total_processed_;
    stats.totalErrors = total_errors_;
    stats.queueSize = data_queue_->size();
    
    if (total_processed_ > 0) {
        stats.averageProcessingTime = total_processing_time_ / total_processed_;
    }
    
    return stats;
}

void SensorProcessor::printStatistics() const {
    auto stats = getStatistics();
    
    std::cout << "\n📈 处理系统统计:" << std::endl;
    std::cout << "  已处理数据: " << stats.totalProcessed << " 条" << std::endl;
    std::cout << "  处理错误: " << stats.totalErrors << " 条" << std::endl;
    std::cout << "  队列大小: " << stats.queueSize << " 条" << std::endl;
    std::cout << "  平均处理时间: " << std::fixed << std::setprecision(2) 
              << stats.averageProcessingTime << " ms" << std::endl;
}

// SensorSimulator implementation
SensorSimulator::SensorSimulator(SensorProcessor& processor)
    : processor_(processor), running_(false), total_generated_(0) {
    std::cout << "🎭 传感器模拟器初始化" << std::endl;
}

SensorSimulator::~SensorSimulator() {
    stop();
    std::cout << "🎭 传感器模拟器已销毁" << std::endl;
}

void SensorSimulator::addSensor(std::shared_ptr<Sensors::ISensor> sensor,
                               std::chrono::milliseconds interval) {
    sensors_.push_back({sensor, interval});
    std::cout << "📡 添加传感器: " << sensor->getSensorId() 
              << " (间隔: " << interval.count() << "ms)" << std::endl;
}

void SensorSimulator::start() {
    if (running_) return;
    
    running_ = true;
    
    for (const auto& sensorInfo : sensors_) {
        sensor_threads_.emplace_back([this, sensorInfo] {
            sensorWorker(sensorInfo);
        });
    }
    
    std::cout << "🎭 传感器模拟器启动，活跃传感器: " << sensors_.size() << std::endl;
}

void SensorSimulator::stop() {
    if (!running_) return;
    
    running_ = false;
    
    for (auto& thread : sensor_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    sensor_threads_.clear();
    
    std::cout << "🎭 传感器模拟器停止" << std::endl;
}

void SensorSimulator::sensorWorker(SensorInfo sensorInfo) {
    std::cout << "📡 传感器线程启动: " << sensorInfo.sensor->getSensorId() << std::endl;
    
    while (running_) {
        try {
            auto data = sensorInfo.sensor->readData();
            
            if (processor_.addSensorData(data)) {
                total_generated_++;
            } else {
                std::cout << "⚠️ 数据队列已满，丢弃数据: " 
                          << sensorInfo.sensor->getSensorId() << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "❌ 传感器读取错误 [" << sensorInfo.sensor->getSensorId() 
                      << "]: " << e.what() << std::endl;
        }
        
        std::this_thread::sleep_for(sensorInfo.interval);
    }
    
    std::cout << "📡 传感器线程结束: " << sensorInfo.sensor->getSensorId() << std::endl;
}

} // namespace WaterIoT::Threading
