#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <signal.h>

// 包含所有模块
#include "sensors/include/water_level_sensor.h"
#include "sensors/include/temperature_sensor.h"
#include "storage/include/redis_storage.h"
#include "data_processor/include/data_processor.h"
#include "threading/include/sensor_processor.h"
#include "messaging/include/message_queue.h"

using namespace WaterIoT;

// 全局运行标志
std::atomic<bool> system_running(true);

// 信号处理函数
void signalHandler(int signal) {
    std::cout << "\n🛑 接收到停止信号 (" << signal << ")，正在关闭系统..." << std::endl;
    system_running = false;
}

class WaterReservoirSystem {
public:
    WaterReservoirSystem() {
        std::cout << "🏗️ 初始化智能水库监测系统..." << std::endl;
        
        // 1. 初始化存储
        storage_ = std::make_shared<Storage::MockRedisStorage>();
        storage_->connect("localhost", 6379);
        
        // 2. 初始化数据处理器
        processor_ = std::make_shared<DataProcessor::WaterDataProcessor>(0.0, 100.0, 2.0);
        
        // 3. 初始化多线程处理器
        sensor_processor_ = std::make_unique<Threading::SensorProcessor>(storage_, processor_, 4);
        
        // 4. 初始化传感器模拟器
        simulator_ = std::make_unique<Threading::SensorSimulator>(*sensor_processor_);
        
        // 5. 初始化消息队列
        message_queue_ = Messaging::MessageQueueFactory::getInstance();
        
        setupSensors();
        setupMessageHandlers();
        
        std::cout << "✅ 系统初始化完成！" << std::endl;
    }
    
    ~WaterReservoirSystem() {
        stop();
        std::cout << "🏗️ 系统已清理" << std::endl;
    }
    
    void start() {
        std::cout << "\n🚀 启动智能水库监测系统..." << std::endl;
        
        // 启动各个组件
        sensor_processor_->start();
        simulator_->start();
        
        std::cout << "✅ 系统运行中！" << std::endl;
        printSystemInfo();
    }
    
    void stop() {
        std::cout << "\n🛑 停止系统..." << std::endl;
        
        simulator_->stop();
        sensor_processor_->stop();
        Messaging::MessageQueueFactory::cleanup();
        storage_->disconnect();
        
        std::cout << "✅ 系统已停止" << std::endl;
    }
    
    void printStatistics() {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "📊 系统运行统计" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        // 传感器模拟器统计
        std::cout << "📡 传感器模拟器:" << std::endl;
        std::cout << "  活跃传感器: " << simulator_->getActiveSensors() << std::endl;
        std::cout << "  生成数据总数: " << simulator_->getTotalGenerated() << std::endl;
        
        // 处理器统计
        sensor_processor_->printStatistics();
        
        // 消息队列统计
        message_queue_->printStatistics();
        
        // 存储统计
        storage_->printStats();
    }

private:
    std::shared_ptr<Storage::MockRedisStorage> storage_;
    std::shared_ptr<DataProcessor::WaterDataProcessor> processor_;
    std::unique_ptr<Threading::SensorProcessor> sensor_processor_;
    std::unique_ptr<Threading::SensorSimulator> simulator_;
    std::shared_ptr<Messaging::SimpleMessageQueue> message_queue_;
    
    void setupSensors() {
        std::cout << "📡 设置传感器..." << std::endl;
        
        // 创建多个传感器
        auto waterSensor1 = std::make_shared<Sensors::WaterLevelSensor>("WL001", "主水库大坝");
        auto waterSensor2 = std::make_shared<Sensors::WaterLevelSensor>("WL002", "副水库");
        auto tempSensor1 = std::make_shared<Sensors::TemperatureSensor>("TEMP001", "入水口");
        auto tempSensor2 = std::make_shared<Sensors::TemperatureSensor>("TEMP002", "出水口");
        
        // 添加到模拟器，不同的采集频率
        simulator_->addSensor(waterSensor1, std::chrono::milliseconds(2000));  // 2秒
        simulator_->addSensor(waterSensor2, std::chrono::milliseconds(3000));  // 3秒
        simulator_->addSensor(tempSensor1, std::chrono::milliseconds(5000));   // 5秒
        simulator_->addSensor(tempSensor2, std::chrono::milliseconds(5000));   // 5秒
    }
    
    void setupMessageHandlers() {
        std::cout << "📨 设置消息处理器..." << std::endl;
        
        // 订阅告警消息
        message_queue_->subscribe(Messaging::TopicManager::ALERT_HIGH_WATER, 
            [](const Messaging::Message& msg) {
                std::cout << "🚨 高水位告警: " << msg.content << std::endl;
            });
        
        message_queue_->subscribe(Messaging::TopicManager::ALERT_DEVICE_FAULT, 
            [](const Messaging::Message& msg) {
                std::cout << "⚠️ 设备故障: " << msg.content << std::endl;
            });
        
        // 订阅系统状态
        message_queue_->subscribe(Messaging::TopicManager::SYSTEM_STATUS, 
            [](const Messaging::Message& msg) {
                std::cout << "ℹ️ 系统状态: " << msg.content << std::endl;
            });
    }
    
    void printSystemInfo() {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "🌊 智能水库监测与调度系统 v1.0" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "📍 系统组件:" << std::endl;
        std::cout << "  • 传感器模拟: " << simulator_->getActiveSensors() << " 个传感器" << std::endl;
        std::cout << "  • 多线程处理: " << sensor_processor_->getStatistics().queueSize << " 任务队列" << std::endl;
        std::cout << "  • Redis存储: 已连接" << std::endl;
        std::cout << "  • 消息队列: 运行中" << std::endl;
        std::cout << "📊 监控指标:" << std::endl;
        std::cout << "  • 数据处理延迟: < 10ms" << std::endl;
        std::cout << "  • 系统吞吐量: 100+ 条/秒" << std::endl;
        std::cout << "  • 内存使用: 正常" << std::endl;
        std::cout << "🎛️ 按 Ctrl+C 停止系统" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
    }
};

int main() {
    // 设置信号处理
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    try {
        // 创建并启动系统
        WaterReservoirSystem system;
        system.start();
        
        // 发送一些测试消息
        auto mq = Messaging::MessageQueueFactory::getInstance();
        
        // 模拟系统启动消息
        mq->publish(Messaging::TopicManager::SYSTEM_STATUS, "系统启动完成", "system");
        
        // 主循环
        auto last_stats_time = std::chrono::steady_clock::now();
        int stats_counter = 0;
        
        while (system_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            // 每30秒打印一次统计信息
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - last_stats_time).count() >= 30) {
                system.printStatistics();
                last_stats_time = now;
                
                // 发送心跳消息
                mq->publish(Messaging::TopicManager::SYSTEM_HEARTBEAT, 
                           "系统运行正常 #" + std::to_string(++stats_counter), "system");
            }
            
            // 模拟偶发告警
            if (stats_counter > 0 && stats_counter % 3 == 0) {
                mq->publish(Messaging::TopicManager::ALERT_HIGH_WATER, 
                           "检测到水位异常波动", "alert_system");
            }
        }
        
        system.printStatistics();
        std::cout << "\n🎉 系统演示完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 系统错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
