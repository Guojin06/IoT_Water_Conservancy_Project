#include "logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <filesystem>

namespace WaterIoT::Common {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_logLevel = level;
}

void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_fileStream.is_open()) {
        m_fileStream.close();
    }
    
    m_logFile = filename;
    
    // 创建日志目录
    std::filesystem::path logPath(filename);
    std::filesystem::path logDir = logPath.parent_path();
    
    if (!logDir.empty() && !std::filesystem::exists(logDir)) {
        std::error_code ec;
        std::filesystem::create_directories(logDir, ec);
        if (ec) {
            std::cerr << "Failed to create log directory: " << logDir << std::endl;
            return;
        }
    }
    
    m_fileStream.open(filename, std::ios::app);
    if (!m_fileStream.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::fatal(const std::string& message) {
    log(LogLevel::FATAL, message);
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < m_logLevel) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string timestamp = getCurrentTime();
    std::string levelStr = levelToString(level);
    
    std::ostringstream logMessage;
    logMessage << "[" << timestamp << "] [" << levelStr << "] " << message;
    
    // 输出到控制台
    if (level >= LogLevel::ERROR) {
        std::cerr << logMessage.str() << std::endl;
    } else {
        std::cout << logMessage.str() << std::endl;
    }
    
    // 输出到文件
    if (m_fileStream.is_open()) {
        m_fileStream << logMessage.str() << std::endl;
        m_fileStream.flush();
    }
}

std::string Logger::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO ";
        case LogLevel::WARNING: return "WARN ";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::FATAL:   return "FATAL";
        default:                return "UNKNW";
    }
}

// 模板方法的显式实例化
template<typename... Args>
void Logger::debug(const std::string& format, Args&&... args) {
    if (LogLevel::DEBUG >= m_logLevel) {
        std::ostringstream oss;
        ((oss << args << " "), ...);
        debug(format + " " + oss.str());
    }
}

template<typename... Args>
void Logger::info(const std::string& format, Args&&... args) {
    if (LogLevel::INFO >= m_logLevel) {
        std::ostringstream oss;
        ((oss << args << " "), ...);
        info(format + " " + oss.str());
    }
}

template<typename... Args>
void Logger::warning(const std::string& format, Args&&... args) {
    if (LogLevel::WARNING >= m_logLevel) {
        std::ostringstream oss;
        ((oss << args << " "), ...);
        warning(format + " " + oss.str());
    }
}

template<typename... Args>
void Logger::error(const std::string& format, Args&&... args) {
    if (LogLevel::ERROR >= m_logLevel) {
        std::ostringstream oss;
        ((oss << args << " "), ...);
        error(format + " " + oss.str());
    }
}

} // namespace WaterIoT::Common
