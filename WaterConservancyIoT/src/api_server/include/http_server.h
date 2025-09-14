#pragma once

#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <functional>
#include "../../storage/include/redis_storage.h"
#include "../../threading/include/sensor_processor.h"
#include "../../messaging/include/message_queue.h"

// httplib前向声明
namespace httplib {
    class Server;
}

namespace WaterIoT::ApiServer {

/**
 * @brief HTTP API服务器
 */
class HttpServer {
public:
    HttpServer(
        std::shared_ptr<Storage::IDataStorage> storage,
        std::shared_ptr<Threading::SensorProcessor> processor,
        std::shared_ptr<Messaging::SimpleMessageQueue> mq,
        int port = 8080
    );
    
    ~HttpServer();
    
    // 服务器控制
    void start();
    void stop();
    bool isRunning() const { return running_; }
    
    // 获取服务器信息
    int getPort() const { return port_; }
    std::string getBaseUrl() const;

private:
    std::shared_ptr<Storage::IDataStorage> storage_;
    std::shared_ptr<Threading::SensorProcessor> processor_;
    std::shared_ptr<Messaging::SimpleMessageQueue> message_queue_;
    
    std::unique_ptr<httplib::Server> server_;
    std::thread server_thread_;
    std::atomic<bool> running_;
    int port_;
    
    // 路由处理函数
    void setupRoutes();
    void setupCORS();
    
    // API端点
    void handleGetSystemStatus();
    void handleGetSensorData();
    void handleGetRecentData();
    void handleGetStatistics();
    void handlePostControl();
    
    // 静态文件服务
    void setupStaticFiles();
    
    // 工具函数
    std::string createJsonResponse(const std::string& status, const std::string& message, const std::string& data = "{}");
    std::string getCurrentTimestamp();
};

} // namespace WaterIoT::ApiServer
