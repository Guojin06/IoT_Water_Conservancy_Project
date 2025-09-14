#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <limits>
#include <nlohmann/json.hpp>

namespace WaterIoT::Sensors {

enum class SensorType {
    WATER_LEVEL,
    WATER_QUALITY,
    RAINFALL,
    GATE_STATUS,
    FLOW_RATE,
    TEMPERATURE
};

enum class SensorStatus {
    ONLINE,
    OFFLINE,
    ERROR,
    MAINTENANCE
};

struct SensorData {
    std::string sensorId;
    SensorType type;
    double value;
    std::string unit;
    std::chrono::system_clock::time_point timestamp;
    SensorStatus status;
    std::string location;
    nlohmann::json metadata;
    
    // 转换为JSON
    nlohmann::json toJson() const;
    
    // 从JSON构造
    static SensorData fromJson(const nlohmann::json& json);
};

class ISensor {
public:
    virtual ~ISensor() = default;
    
    // 纯虚函数接口
    virtual SensorData readData() = 0;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual SensorStatus getStatus() const = 0;
    virtual std::string getSensorId() const = 0;
    virtual SensorType getSensorType() const = 0;
    virtual std::string getLocation() const = 0;
    
    // 可选的配置接口
    virtual bool configure(const nlohmann::json& config) { return true; }
    virtual nlohmann::json getConfiguration() const { return nlohmann::json{}; }
    
    // 校准接口
    virtual bool calibrate() { return true; }
    virtual bool needsCalibration() const { return false; }
    
    // 诊断接口
    virtual bool selfTest() { return true; }
    virtual std::string getLastError() const { return ""; }
};

class SensorBase : public ISensor {
public:
    SensorBase(const std::string& sensorId, 
               SensorType type, 
               const std::string& location,
               const std::string& unit = "");
    
    virtual ~SensorBase() = default;
    
    // 实现基础接口
    std::string getSensorId() const override { return m_sensorId; }
    SensorType getSensorType() const override { return m_type; }
    std::string getLocation() const override { return m_location; }
    SensorStatus getStatus() const override { return m_status; }
    std::string getLastError() const override { return m_lastError; }
    
    // 配置管理
    bool configure(const nlohmann::json& config) override;
    nlohmann::json getConfiguration() const override;
    
    // 基础功能
    virtual bool initialize() override;
    virtual void shutdown() override;
    virtual bool selfTest() override;
    virtual SensorData readData() override;

protected:
    // 子类需要实现的核心读取逻辑
    virtual double readRawValue() = 0;
    
    // 工具方法
    void setStatus(SensorStatus status);
    void setError(const std::string& error);
    void addMetadata(const std::string& key, const nlohmann::json& value);
    
    // 数据验证和处理
    virtual bool validateData(double value) const;
    virtual double processRawValue(double rawValue) const;
    
    std::string m_sensorId;
    SensorType m_type;
    std::string m_location;
    std::string m_unit;
    SensorStatus m_status{SensorStatus::OFFLINE};
    std::string m_lastError;
    nlohmann::json m_config;
    nlohmann::json m_metadata;
    
    // 校准参数
    double m_calibrationOffset{0.0};
    double m_calibrationScale{1.0};
    
    // 数据范围验证
    double m_minValue{-std::numeric_limits<double>::max()};
    double m_maxValue{std::numeric_limits<double>::max()};
};

} // namespace WaterIoT::Sensors
