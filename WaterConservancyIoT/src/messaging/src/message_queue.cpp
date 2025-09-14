#include "../include/message_queue.h"
#include <iostream>
#include <iomanip>

namespace WaterIoT::Messaging {

// SimpleMessageQueue implementation
SimpleMessageQueue::SimpleMessageQueue() 
    : running_(false), total_published_(0), total_consumed_(0) {
    std::cout << "📨 消息队列初始化" << std::endl;
}

SimpleMessageQueue::~SimpleMessageQueue() {
    stop();
    std::cout << "📨 消息队列已销毁" << std::endl;
}

bool SimpleMessageQueue::publish(const std::string& topic, const std::string& content, const std::string& senderId) {
    return publish(Message(topic, content, senderId));
}

bool SimpleMessageQueue::publish(const Message& message) {
    if (!running_) return false;
    
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        message_queue_.push(message);
        total_published_++;
    }
    
    queue_condition_.notify_one();
    return true;
}

bool SimpleMessageQueue::subscribe(const std::string& topic, MessageHandler handler) {
    std::lock_guard<std::mutex> lock(subscribers_mutex_);
    subscribers_[topic].push_back(handler);
    
    std::cout << "📬 新订阅者注册主题: " << topic 
              << " (当前订阅者: " << subscribers_[topic].size() << ")" << std::endl;
    return true;
}

bool SimpleMessageQueue::unsubscribe(const std::string& topic) {
    std::lock_guard<std::mutex> lock(subscribers_mutex_);
    auto it = subscribers_.find(topic);
    if (it != subscribers_.end()) {
        subscribers_.erase(it);
        std::cout << "📪 取消订阅主题: " << topic << std::endl;
        return true;
    }
    return false;
}

void SimpleMessageQueue::start() {
    if (running_) return;
    
    running_ = true;
    worker_thread_ = std::thread([this] { messageWorker(); });
    
    std::cout << "🚀 消息队列启动" << std::endl;
}

void SimpleMessageQueue::stop() {
    if (!running_) return;
    
    running_ = false;
    queue_condition_.notify_all();
    
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
    
    std::cout << "🛑 消息队列停止" << std::endl;
}

void SimpleMessageQueue::messageWorker() {
    std::cout << "📨 消息处理线程启动" << std::endl;
    
    while (running_) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        
        queue_condition_.wait(lock, [this] {
            return !message_queue_.empty() || !running_;
        });
        
        while (!message_queue_.empty()) {
            Message message = message_queue_.front();
            message_queue_.pop();
            lock.unlock();
            
            // 分发消息
            dispatchMessage(message);
            total_consumed_++;
            
            lock.lock();
        }
    }
    
    std::cout << "📨 消息处理线程结束" << std::endl;
}

void SimpleMessageQueue::dispatchMessage(const Message& message) {
    std::lock_guard<std::mutex> lock(subscribers_mutex_);
    
    auto it = subscribers_.find(message.topic);
    if (it != subscribers_.end()) {
        for (const auto& handler : it->second) {
            try {
                handler(message);
            } catch (const std::exception& e) {
                std::cout << "❌ 消息处理错误 [" << message.topic << "]: " 
                          << e.what() << std::endl;
            }
        }
    } else {
        std::cout << "⚠️ 没有订阅者处理主题: " << message.topic << std::endl;
    }
}

SimpleMessageQueue::Statistics SimpleMessageQueue::getStatistics() const {
    std::lock_guard<std::mutex> stats_lock(stats_mutex_);
    std::lock_guard<std::mutex> queue_lock(queue_mutex_);
    std::lock_guard<std::mutex> sub_lock(subscribers_mutex_);
    
    Statistics stats;
    stats.totalPublished = total_published_;
    stats.totalConsumed = total_consumed_;
    stats.currentQueueSize = message_queue_.size();
    stats.subscriberCount = subscribers_.size();
    
    return stats;
}

void SimpleMessageQueue::printStatistics() const {
    auto stats = getStatistics();
    
    std::cout << "\n📊 消息队列统计:" << std::endl;
    std::cout << "  已发布消息: " << stats.totalPublished << std::endl;
    std::cout << "  已消费消息: " << stats.totalConsumed << std::endl;
    std::cout << "  队列大小: " << stats.currentQueueSize << std::endl;
    std::cout << "  订阅主题数: " << stats.subscriberCount << std::endl;
}

// TopicManager implementation
const std::string TopicManager::SENSOR_WATER_LEVEL = "sensor.water.level";
const std::string TopicManager::SENSOR_TEMPERATURE = "sensor.temperature";
const std::string TopicManager::SENSOR_FLOW_RATE = "sensor.flow.rate";
const std::string TopicManager::SENSOR_WATER_QUALITY = "sensor.water.quality";

const std::string TopicManager::ALERT_HIGH_WATER = "alert.water.high";
const std::string TopicManager::ALERT_LOW_WATER = "alert.water.low";
const std::string TopicManager::ALERT_DEVICE_FAULT = "alert.device.fault";

const std::string TopicManager::SYSTEM_HEARTBEAT = "system.heartbeat";
const std::string TopicManager::SYSTEM_STATUS = "system.status";
const std::string TopicManager::SYSTEM_CONTROL = "system.control";

std::vector<std::string> TopicManager::getAllTopics() {
    return {
        SENSOR_WATER_LEVEL, SENSOR_TEMPERATURE, SENSOR_FLOW_RATE, SENSOR_WATER_QUALITY,
        ALERT_HIGH_WATER, ALERT_LOW_WATER, ALERT_DEVICE_FAULT,
        SYSTEM_HEARTBEAT, SYSTEM_STATUS, SYSTEM_CONTROL
    };
}

bool TopicManager::isValidTopic(const std::string& topic) {
    auto topics = getAllTopics();
    return std::find(topics.begin(), topics.end(), topic) != topics.end();
}

// MessageQueueFactory implementation
std::shared_ptr<SimpleMessageQueue> MessageQueueFactory::instance_ = nullptr;
std::mutex MessageQueueFactory::instance_mutex_;

std::shared_ptr<SimpleMessageQueue> MessageQueueFactory::getInstance() {
    std::lock_guard<std::mutex> lock(instance_mutex_);
    
    if (!instance_) {
        instance_ = std::make_shared<SimpleMessageQueue>();
        instance_->start();
    }
    
    return instance_;
}

void MessageQueueFactory::cleanup() {
    std::lock_guard<std::mutex> lock(instance_mutex_);
    
    if (instance_) {
        instance_->stop();
        instance_.reset();
    }
}

} // namespace WaterIoT::Messaging
