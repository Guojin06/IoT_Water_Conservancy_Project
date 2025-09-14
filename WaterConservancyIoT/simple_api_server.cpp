#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iomanip>
#include <atomic>
#include <queue>
#include <mutex>
#include <random>

// Include httplib
#include "external/httplib/httplib.h"

// Simplified API server that runs on port 8888 to match frontend
namespace ApiServer {

// Mock sensor data with timestamp
struct SensorData {
    std::string sensorId;
    double value;
    std::time_t timestamp;
    std::string status;
    
    SensorData(const std::string& id, double val, const std::string& stat = "online") 
        : sensorId(id), value(val), status(stat) {
        timestamp = std::time(nullptr);
    }
};

// Mock storage with historical data
class MockStorage {
private:
    std::vector<SensorData> dataStore_;
    std::map<std::string, std::string> cacheStore_;
    std::mutex mutex_;

public:
    bool storeSensorData(const SensorData& data) {
        std::lock_guard<std::mutex> lock(mutex_);
        dataStore_.push_back(data);
        
        // Update cache
        std::string cacheKey = "latest:" + data.sensorId;
        cacheStore_[cacheKey] = std::to_string(data.value);
        
        std::cout << "[STORAGE] Store data: " << data.sensorId << " = " << data.value << std::endl;
        return true;
    }
    
    std::vector<SensorData> getRecentData(const std::string& sensorId, int count) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<SensorData> result;
        
        for (auto it = dataStore_.rbegin(); it != dataStore_.rend() && result.size() < count; ++it) {
            if (it->sensorId == sensorId) {
                result.push_back(*it);
            }
        }
        
        return result;
    }
    
    std::string getCache(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = cacheStore_.find(key);
        return (it != cacheStore_.end()) ? it->second : "";
    }
    
    size_t getDataCount() {
        std::lock_guard<std::mutex> lock(mutex_);
        return dataStore_.size();
    }
    
    size_t getCacheCount() {
        std::lock_guard<std::mutex> lock(mutex_);
        return cacheStore_.size();
    }
};

// Mock sensor data generator
class DataGenerator {
private:
    std::shared_ptr<MockStorage> storage_;
    std::atomic<bool> running_;
    std::mt19937 gen_;
    
public:
    DataGenerator(std::shared_ptr<MockStorage> storage) 
        : storage_(storage), running_(false), gen_(std::random_device{}()) {}
    
    void start() {
        running_ = true;
        std::thread([this] {
            std::normal_distribution<double> wl_dist(15.0, 2.0);  // Water level: 15±2m
            std::normal_distribution<double> temp_dist(24.0, 3.0); // Temperature: 24±3°C
            
            while (running_) {
                // Generate water level data
                double wl1 = std::max(0.0, wl_dist(gen_));
                double wl2 = std::max(0.0, wl_dist(gen_));
                
                storage_->storeSensorData(SensorData("WL001", wl1));
                storage_->storeSensorData(SensorData("WL002", wl2));
                
                // Generate temperature data  
                double temp1 = temp_dist(gen_);
                double temp2 = temp_dist(gen_);
                
                storage_->storeSensorData(SensorData("TEMP001", temp1));
                storage_->storeSensorData(SensorData("TEMP002", temp2));
                
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }
        }).detach();
        
        std::cout << "[GENERATOR] Data generator started" << std::endl;
    }
    
    void stop() {
        running_ = false;
        std::cout << "[GENERATOR] Data generator stopped" << std::endl;
    }
};

// HTTP API Server
class HttpApiServer {
private:
    std::shared_ptr<MockStorage> storage_;
    std::shared_ptr<DataGenerator> generator_;
    httplib::Server server_;
    int port_;
    std::atomic<bool> running_;
    
public:
    HttpApiServer(int port = 8888) : port_(port), running_(false) {
        storage_ = std::make_shared<MockStorage>();
        generator_ = std::make_shared<DataGenerator>(storage_);
        setupRoutes();
    }
    
    void setupRoutes() {
        // Enable CORS
        server_.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
            return httplib::Server::HandlerResponse::Unhandled;
        });
        
        // Handle OPTIONS requests
        server_.Options(".*", [](const httplib::Request&, httplib::Response& res) {
            return;
        });
        
        // API: Get system status
        server_.Get("/api/status", [this](const httplib::Request& req, httplib::Response& res) {
            res.set_content(R"({
    "status": "success",
    "message": "System running normally",
    "timestamp": ")" + getCurrentTimestamp() + R"(",
    "data": {
        "system": "running",
        "sensors": {
            "processed": )" + std::to_string(storage_->getDataCount()) + R"(,
            "errors": 0,
            "queue_size": 0,
            "avg_time": 5.2
        },
        "messaging": {
            "published": )" + std::to_string(storage_->getDataCount()) + R"(,
            "consumed": )" + std::to_string(storage_->getDataCount()) + R"(,
            "queue_size": 0
        }
    }
})", "application/json");
        });
        
        // API: Get recent sensor data
        server_.Get("/api/sensors/recent", [this](const httplib::Request& req, httplib::Response& res) {
            std::string sensorId = req.get_param_value("id");
            if (sensorId.empty()) {
                res.status = 400;
                res.set_content(R"({"status": "error", "message": "Missing sensor id parameter"})", "application/json");
                return;
            }
            
            auto recentData = storage_->getRecentData(sensorId, 10);
            
            std::ostringstream json;
            json << R"({
    "status": "success",
    "message": "Data retrieved successfully",
    "data": {
        "sensor_id": ")" << sensorId << R"(",
        "data": [)";
            
            for (size_t i = 0; i < recentData.size(); ++i) {
                if (i > 0) json << ",";
                json << R"(
            {
                "value": )" << recentData[i].value << R"(,
                "timestamp": )" << recentData[i].timestamp << R"(,
                "status": ")" << recentData[i].status << R"("
            })";
            }
            
            json << R"(
        ]
    }
})";
            
            res.set_content(json.str(), "application/json");
        });
        
        // API: Get cache data
        server_.Get("/api/cache/(.*)", [this](const httplib::Request& req, httplib::Response& res) {
            std::string key = req.matches[1];
            std::string value = storage_->getCache(key);
            
            if (!value.empty()) {
                res.set_content(R"({"status": "success", "data": ")" + value + R"("})", "application/json");
            } else {
                res.status = 404;
                res.set_content(R"({"status": "error", "message": "Cache key not found"})", "application/json");
            }
        });
        
        // API: Control commands
        server_.Post("/api/control", [this](const httplib::Request& req, httplib::Response& res) {
            std::string command = req.body;
            
            std::cout << "[CONTROL] Received command: " << command << std::endl;
            
            res.set_content(R"({
    "status": "success",
    "message": "Command ')" + command + R"(' executed successfully",
    "timestamp": ")" + getCurrentTimestamp() + R"("
})", "application/json");
        });
        
        // Serve static files
        server_.set_mount_point("/", "./frontend");
    }
    
    void start() {
        running_ = true;
        generator_->start();
        
        std::cout << "[HTTP] Starting API server on port " << port_ << std::endl;
        std::cout << "[HTTP] Frontend: http://localhost:" << port_ << std::endl;
        std::cout << "[HTTP] API Base: http://localhost:" << port_ << "/api/" << std::endl;
        
        // Start server in a separate thread
        std::thread([this] {
            server_.listen("0.0.0.0", port_);
        }).detach();
        
        // Wait a moment for server to start
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "[HTTP] Server started successfully!" << std::endl;
    }
    
    void stop() {
        running_ = false;
        generator_->stop();
        server_.stop();
        std::cout << "[HTTP] Server stopped" << std::endl;
    }
    
    bool isRunning() const { return running_; }
    
private:
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::ostringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

} // namespace ApiServer

int main() {
    using namespace ApiServer;
    
    std::cout << "=== Water Reservoir IoT API Server ===" << std::endl;
    
    try {
        auto server = std::make_unique<HttpApiServer>(8888);
        server->start();
        
        std::cout << "\n[SUCCESS] System ready!" << std::endl;
        std::cout << "Frontend URL: http://localhost:8888" << std::endl;
        std::cout << "\n[INFO] API Endpoints:" << std::endl;
        std::cout << "  GET  /api/status" << std::endl;
        std::cout << "  GET  /api/sensors/recent?id=WL001" << std::endl;
        std::cout << "  GET  /api/cache/latest:WL001" << std::endl;
        std::cout << "  POST /api/control" << std::endl;
        
        std::cout << "\n[WAIT] Press Ctrl+C to stop..." << std::endl;
        
        // Keep running
        while (server->isRunning()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        server->stop();
        
    } catch (const std::exception& e) {
        std::cout << "[ERROR] Server failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
