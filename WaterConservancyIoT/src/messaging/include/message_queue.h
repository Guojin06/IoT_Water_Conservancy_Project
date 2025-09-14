#pragma once

#include <string>
#include <functional>
#include <map>
#include <vector>
#include <mutex>
#include <memory>
#include <queue>
#include <thread>
#include <atomic>

namespace WaterIoT::Messaging {

/**
 * @brief 消息结构
 */
struct Message {
    std::string topic;          // 主题
    std::string content;        // 消息内容
    std::chrono::system_clock::time_point timestamp;  // 时间戳
    std::string senderId;       // 发送者ID
    
    Message() = default;
    Message(const std::string& t, const std::string& c, const std::string& sid = "")
        : topic(t), content(c), timestamp(std::chrono::system_clock::now()), senderId(sid) {}
};

/**
 * @brief 消息处理回调函数类型
 */
using MessageHandler = std::function<void(const Message&)>;

/**
 * @brief 简单的内存消息队列实现
 */
class SimpleMessageQueue {
public:
    SimpleMessageQueue();
    ~SimpleMessageQueue();
    
    // 发布消息
    bool publish(const std::string& topic, const std::string& content, const std::string& senderId = "");
    bool publish(const Message& message);
    
    // 订阅主题
    bool subscribe(const std::string& topic, MessageHandler handler);
    bool unsubscribe(const std::string& topic);
    
    // 控制队列
    void start();
    void stop();
    bool isRunning() const { return running_; }
    
    // 统计信息
    struct Statistics {
        size_t totalPublished = 0;
        size_t totalConsumed = 0;
        size_t currentQueueSize = 0;
        size_t subscriberCount = 0;
    };
    
    Statistics getStatistics() const;
    void printStatistics() const;

private:
    std::atomic<bool> running_;
    std::thread worker_thread_;
    
    // 消息存储
    std::queue<Message> message_queue_;
    mutable std::mutex queue_mutex_;
    std::condition_variable queue_condition_;
    
    // 订阅者管理
    std::map<std::string, std::vector<MessageHandler>> subscribers_;
    mutable std::mutex subscribers_mutex_;
    
    // 统计信息
    mutable std::mutex stats_mutex_;
    std::atomic<size_t> total_published_;
    std::atomic<size_t> total_consumed_;
    
    // 工作线程
    void messageWorker();
    
    // 分发消息给订阅者
    void dispatchMessage(const Message& message);
};

/**
 * @brief 主题管理器 - 定义系统中使用的主题
 */
class TopicManager {
public:
    // 传感器数据主题
    static const std::string SENSOR_WATER_LEVEL;
    static const std::string SENSOR_TEMPERATURE;
    static const std::string SENSOR_FLOW_RATE;
    static const std::string SENSOR_WATER_QUALITY;
    
    // 告警主题
    static const std::string ALERT_HIGH_WATER;
    static const std::string ALERT_LOW_WATER;
    static const std::string ALERT_DEVICE_FAULT;
    
    // 系统主题
    static const std::string SYSTEM_HEARTBEAT;
    static const std::string SYSTEM_STATUS;
    static const std::string SYSTEM_CONTROL;
    
    // 获取所有主题
    static std::vector<std::string> getAllTopics();
    
    // 验证主题名称
    static bool isValidTopic(const std::string& topic);
};

/**
 * @brief 消息队列工厂 - 管理全局消息队列实例
 */
class MessageQueueFactory {
public:
    static std::shared_ptr<SimpleMessageQueue> getInstance();
    static void cleanup();

private:
    static std::shared_ptr<SimpleMessageQueue> instance_;
    static std::mutex instance_mutex_;
};

} // namespace WaterIoT::Messaging
