#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <memory>

namespace WaterIoT::Common {

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    FATAL = 4
};

class Logger {
public:
    static Logger& getInstance();
    
    void setLogLevel(LogLevel level);
    void setLogFile(const std::string& filename);
    
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);
    
    // 模板方法支持格式化输出
    template<typename... Args>
    void debug(const std::string& format, Args&&... args);
    
    template<typename... Args>
    void info(const std::string& format, Args&&... args);
    
    template<typename... Args>
    void warning(const std::string& format, Args&&... args);
    
    template<typename... Args>
    void error(const std::string& format, Args&&... args);

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    void log(LogLevel level, const std::string& message);
    std::string getCurrentTime() const;
    std::string levelToString(LogLevel level) const;
    
    LogLevel m_logLevel{LogLevel::INFO};
    std::string m_logFile;
    std::ofstream m_fileStream;
    std::mutex m_mutex;
};

// 便利宏定义
#define LOG_DEBUG(msg) WaterIoT::Common::Logger::getInstance().debug(msg)
#define LOG_INFO(msg) WaterIoT::Common::Logger::getInstance().info(msg)
#define LOG_WARNING(msg) WaterIoT::Common::Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) WaterIoT::Common::Logger::getInstance().error(msg)
#define LOG_FATAL(msg) WaterIoT::Common::Logger::getInstance().fatal(msg)

} // namespace WaterIoT::Common
