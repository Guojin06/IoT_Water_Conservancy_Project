#include "../include/thread_pool.h"
#include <iostream>

namespace WaterIoT::Threading {

ThreadPool::ThreadPool(size_t numThreads) : stop_(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers_.emplace_back([this] {
            for (;;) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                    
                    if (stop_ && tasks_.empty()) {
                        return;
                    }
                    
                    task = std::move(tasks_.front());
                    tasks_.pop();
                }
                
                task();
            }
        });
    }
    
    std::cout << "🧵 线程池启动完成，工作线程数: " << numThreads << std::endl;
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }
    
    condition_.notify_all();
    
    for (std::thread &worker : workers_) {
        worker.join();
    }
    
    std::cout << "🧵 线程池已关闭" << std::endl;
}

size_t ThreadPool::getQueueSize() const {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    return tasks_.size();
}

void ThreadPool::waitForAll() {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    condition_.wait(lock, [this] { return tasks_.empty(); });
}

// Template specialization for submit method
template<typename F, typename... Args>
auto ThreadPool::submit(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
    
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
    std::future<return_type> res = task->get_future();
    
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        
        if (stop_) {
            throw std::runtime_error("向已停止的线程池提交任务");
        }
        
        tasks_.emplace([task]() { (*task)(); });
    }
    
    condition_.notify_one();
    return res;
}

} // namespace WaterIoT::Threading
