#include "../include/http_server.h"
#include "../../external/httplib/httplib.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace WaterIoT::ApiServer {

HttpServer::HttpServer(
    std::shared_ptr<Storage::IDataStorage> storage,
    std::shared_ptr<Threading::SensorProcessor> processor,
    std::shared_ptr<Messaging::SimpleMessageQueue> mq,
    int port
) : storage_(storage), processor_(processor), message_queue_(mq), 
    running_(false), port_(port) {
    
    server_ = std::make_unique<httplib::Server>();
    setupCORS();
    setupRoutes();
    setupStaticFiles();
    
    std::cout << "🌐 HTTP服务器初始化完成，端口: " << port_ << std::endl;
}

HttpServer::~HttpServer() {
    stop();
    std::cout << "🌐 HTTP服务器已销毁" << std::endl;
}

void HttpServer::start() {
    if (running_) return;
    
    running_ = true;
    
    server_thread_ = std::thread([this] {
        std::cout << "🚀 HTTP服务器启动: http://localhost:" << port_ << std::endl;
        server_->listen("0.0.0.0", port_);
    });
    
    // 等待服务器启动
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void HttpServer::stop() {
    if (!running_) return;
    
    running_ = false;
    server_->stop();
    
    if (server_thread_.joinable()) {
        server_thread_.join();
    }
    
    std::cout << "🛑 HTTP服务器已停止" << std::endl;
}

std::string HttpServer::getBaseUrl() const {
    return "http://localhost:" + std::to_string(port_);
}

void HttpServer::setupCORS() {
    // 设置CORS，允许前端跨域访问
    server_->set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        return httplib::Server::HandlerResponse::Unhandled;
    });
    
    // 处理OPTIONS请求
    server_->Options(".*", [](const httplib::Request&, httplib::Response& res) {
        return;
    });
}

void HttpServer::setupRoutes() {
    // API路由
    server_->Get("/api/status", [this](const httplib::Request& req, httplib::Response& res) {
        auto stats = processor_->getStatistics();
        auto mq_stats = message_queue_->getStatistics();
        
        std::ostringstream data;
        data << "{"
             << "\"system\":\"running\","
             << "\"timestamp\":\"" << getCurrentTimestamp() << "\","
             << "\"sensors\":{"
             << "\"processed\":" << stats.totalProcessed << ","
             << "\"errors\":" << stats.totalErrors << ","
             << "\"queue_size\":" << stats.queueSize << ","
             << "\"avg_time\":" << std::fixed << std::setprecision(2) << stats.averageProcessingTime
             << "},"
             << "\"messaging\":{"
             << "\"published\":" << mq_stats.totalPublished << ","
             << "\"consumed\":" << mq_stats.totalConsumed << ","
             << "\"queue_size\":" << mq_stats.currentQueueSize
             << "}"
             << "}";
        
        res.set_content(createJsonResponse("success", "系统状态正常", data.str()), "application/json");
    });
    
    server_->Get("/api/sensors/recent", [this](const httplib::Request& req, httplib::Response& res) {
        std::string sensorId = req.get_param_value("id");
        if (sensorId.empty()) sensorId = "WL001";
        
        auto recentData = storage_->getRecentData(sensorId, 10);
        
        std::ostringstream data;
        data << "{\"sensor_id\":\"" << sensorId << "\",\"data\":[";
        
        for (size_t i = 0; i < recentData.size(); ++i) {
            if (i > 0) data << ",";
            auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                recentData[i].timestamp.time_since_epoch()).count();
            data << "{"
                 << "\"value\":" << recentData[i].value << ","
                 << "\"timestamp\":" << timestamp << ","
                 << "\"status\":\"" << (recentData[i].status == Sensors::SensorStatus::ONLINE ? "online" : "offline") << "\""
                 << "}";
        }
        
        data << "]}";
        res.set_content(createJsonResponse("success", "数据获取成功", data.str()), "application/json");
    });
    
    server_->Get("/api/cache/(.*)", [this](const httplib::Request& req, httplib::Response& res) {
        std::string key = req.matches[1];
        auto value = storage_->getCache(key);
        
        if (value) {
            std::string data = "{\"key\":\"" + key + "\",\"value\":\"" + *value + "\"}";
            res.set_content(createJsonResponse("success", "缓存获取成功", data), "application/json");
        } else {
            res.set_content(createJsonResponse("error", "缓存未找到"), "application/json");
        }
    });
    
    server_->Post("/api/control", [this](const httplib::Request& req, httplib::Response& res) {
        // 模拟控制指令
        std::string command = req.body;
        
        message_queue_->publish(Messaging::TopicManager::SYSTEM_CONTROL, command, "web_api");
        
        res.set_content(createJsonResponse("success", "控制指令已发送"), "application/json");
    });
    
    std::cout << "🛣️ API路由设置完成" << std::endl;
}

void HttpServer::setupStaticFiles() {
    // 提供静态文件服务
    server_->set_mount_point("/", "./frontend/dist");
    server_->set_mount_point("/static", "./frontend/dist/static");
    
    // 默认页面
    server_->Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_redirect("/index.html");
    });
    
    std::cout << "📁 静态文件服务设置完成" << std::endl;
}

std::string HttpServer::createJsonResponse(const std::string& status, const std::string& message, const std::string& data) {
    std::ostringstream json;
    json << "{"
         << "\"status\":\"" << status << "\","
         << "\"message\":\"" << message << "\","
         << "\"timestamp\":\"" << getCurrentTimestamp() << "\"";
    
    if (!data.empty() && data != "{}") {
        json << ",\"data\":" << data;
    }
    
    json << "}";
    return json.str();
}

std::string HttpServer::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace WaterIoT::ApiServer
