#include "sensor_base.h"
#include "logger.h"

using namespace WaterIoT::Common;

namespace WaterIoT::Sensors {

// SensorData 方法实现
nlohmann::json SensorData::toJson() const {
    nlohmann::json json;
    json["sensor_id"] = sensorId;
    json["type"] = static_cast<int>(type);
    json["value"] = value;
    json["unit"] = unit;
    json["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()).count();
    json["status"] = static_cast<int>(status);
    json["location"] = location;
    json["metadata"] = metadata;
    return json;
}

SensorData SensorData::fromJson(const nlohmann::json& json) {
    SensorData data;
    data.sensorId = json.value("sensor_id", "");
    data.type = static_cast<SensorType>(json.value("type", 0));
    data.value = json.value("value", 0.0);
    data.unit = json.value("unit", "");
    
    auto timestamp_ms = json.value("timestamp", 0L);
    data.timestamp = std::chrono::system_clock::time_point(
        std::chrono::milliseconds(timestamp_ms));
    
    data.status = static_cast<SensorStatus>(json.value("status", 0));
    data.location = json.value("location", "");
    data.metadata = json.value("metadata", nlohmann::json{});
    
    return data;
}

// SensorBase 实现
SensorBase::SensorBase(const std::string& sensorId, 
                       SensorType type, 
                       const std::string& location,
                       const std::string& unit)
    : m_sensorId(sensorId)
    , m_type(type)
    , m_location(location)
    , m_unit(unit) {
}

bool SensorBase::configure(const nlohmann::json& config) {
    try {
        m_config = config;
        
        // 读取校准参数
        if (config.contains("calibration_offset")) {
            m_calibrationOffset = config["calibration_offset"];
        }
        
        if (config.contains("calibration_scale")) {
            m_calibrationScale = config["calibration_scale"];
        }
        
        // 读取数据范围
        if (config.contains("min_value")) {
            m_minValue = config["min_value"];
        }
        
        if (config.contains("max_value")) {
            m_maxValue = config["max_value"];
        }
        
        LOG_INFO("传感器配置更新: " + m_sensorId);
        return true;
    } catch (const std::exception& e) {
        setError("配置失败: " + std::string(e.what()));
        return false;
    }
}

nlohmann::json SensorBase::getConfiguration() const {
    return m_config;
}

bool SensorBase::initialize() {
    try {
        LOG_INFO("初始化传感器: " + m_sensorId);
        setStatus(SensorStatus::ONLINE);
        return true;
    } catch (const std::exception& e) {
        setError("初始化失败: " + std::string(e.what()));
        setStatus(SensorStatus::ERROR);
        return false;
    }
}

void SensorBase::shutdown() {
    LOG_INFO("关闭传感器: " + m_sensorId);
    setStatus(SensorStatus::OFFLINE);
}

bool SensorBase::selfTest() {
    try {
        LOG_DEBUG("传感器自检: " + m_sensorId);
        
        // 基础自检逻辑
        double testValue = readRawValue();
        if (!validateData(testValue)) {
            setError("自检失败: 数据验证不通过");
            return false;
        }
        
        LOG_DEBUG("传感器自检通过: " + m_sensorId);
        return true;
    } catch (const std::exception& e) {
        setError("自检异常: " + std::string(e.what()));
        return false;
    }
}

void SensorBase::setStatus(SensorStatus status) {
    m_status = status;
}

void SensorBase::setError(const std::string& error) {
    m_lastError = error;
    LOG_ERROR("传感器错误 [" + m_sensorId + "]: " + error);
}

void SensorBase::addMetadata(const std::string& key, const nlohmann::json& value) {
    m_metadata[key] = value;
}

bool SensorBase::validateData(double value) const {
    if (std::isnan(value) || std::isinf(value)) {
        return false;
    }
    
    if (value < m_minValue || value > m_maxValue) {
        return false;
    }
    
    return true;
}

double SensorBase::processRawValue(double rawValue) const {
    // 应用校准参数
    return (rawValue + m_calibrationOffset) * m_calibrationScale;
}

SensorData SensorBase::readData() {
    SensorData data;
    data.sensorId = m_sensorId;
    data.type = m_type;
    data.location = m_location;
    data.unit = m_unit;
    data.status = m_status;
    data.timestamp = std::chrono::system_clock::now();
    data.metadata = m_metadata;
    
    try {
        if (m_status != SensorStatus::ONLINE) {
            data.value = 0.0;
            return data;
        }
        
        // 读取原始值
        double rawValue = readRawValue();
        
        // 处理和校准
        double processedValue = processRawValue(rawValue);
        
        // 验证数据
        if (!validateData(processedValue)) {
            setError("数据验证失败");
            setStatus(SensorStatus::ERROR);
            data.value = 0.0;
            data.status = SensorStatus::ERROR;
        } else {
            data.value = processedValue;
        }
        
    } catch (const std::exception& e) {
        setError("读取数据异常: " + std::string(e.what()));
        setStatus(SensorStatus::ERROR);
        data.value = 0.0;
        data.status = SensorStatus::ERROR;
    }
    
    return data;
}

} // namespace WaterIoT::Sensors
