#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

namespace WaterIoT::Threading {

/**
 * @brief 线程池 - 管理多个工作线程
 */
class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
    ~ThreadPool();
    
    // 提交任务到线程池
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    
    // 获取线程池状态
    size_t getThreadCount() const { return workers_.size(); }
    size_t getQueueSize() const;
    bool isRunning() const { return !stop_; }
    
    // 等待所有任务完成
    void waitForAll();

private:
    std::vector<std::thread> workers_;           // 工作线程
    std::queue<std::function<void()>> tasks_;    // 任务队列
    
    std::mutex queue_mutex_;                     // 队列互斥锁
    std::condition_variable condition_;          // 条件变量
    std::atomic<bool> stop_;                     // 停止标志
};

/**
 * @brief 生产者消费者模式 - 用于传感器数据处理
 */
template<typename T>
class ProducerConsumer {
public:
    explicit ProducerConsumer(size_t maxQueueSize = 1000);
    ~ProducerConsumer();
    
    // 生产者接口
    bool produce(const T& item);
    bool produce(T&& item);
    
    // 消费者接口
    bool consume(T& item, std::chrono::milliseconds timeout = std::chrono::milliseconds(100));
    
    // 状态查询
    size_t size() const;
    bool empty() const;
    bool full() const;
    
    // 控制接口
    void stop();
    bool isStopped() const { return stopped_; }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
    
    const size_t max_size_;
    std::atomic<bool> stopped_;
};

// Template implementation
template<typename T>
ProducerConsumer<T>::ProducerConsumer(size_t maxQueueSize)
    : max_size_(maxQueueSize), stopped_(false) {}

template<typename T>
ProducerConsumer<T>::~ProducerConsumer() {
    stop();
}

template<typename T>
bool ProducerConsumer<T>::produce(const T& item) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    // 等待队列不满
    if (!not_full_.wait_for(lock, std::chrono::milliseconds(100), 
                           [this] { return queue_.size() < max_size_ || stopped_; })) {
        return false; // 超时
    }
    
    if (stopped_) return false;
    
    queue_.push(item);
    not_empty_.notify_one();
    return true;
}

template<typename T>
bool ProducerConsumer<T>::produce(T&& item) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    if (!not_full_.wait_for(lock, std::chrono::milliseconds(100), 
                           [this] { return queue_.size() < max_size_ || stopped_; })) {
        return false;
    }
    
    if (stopped_) return false;
    
    queue_.push(std::move(item));
    not_empty_.notify_one();
    return true;
}

template<typename T>
bool ProducerConsumer<T>::consume(T& item, std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    if (!not_empty_.wait_for(lock, timeout, [this] { return !queue_.empty() || stopped_; })) {
        return false; // 超时
    }
    
    if (stopped_ && queue_.empty()) return false;
    
    item = std::move(queue_.front());
    queue_.pop();
    not_full_.notify_one();
    return true;
}

template<typename T>
size_t ProducerConsumer<T>::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

template<typename T>
bool ProducerConsumer<T>::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}

template<typename T>
bool ProducerConsumer<T>::full() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size() >= max_size_;
}

template<typename T>
void ProducerConsumer<T>::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stopped_ = true;
    }
    not_empty_.notify_all();
    not_full_.notify_all();
}

} // namespace WaterIoT::Threading
