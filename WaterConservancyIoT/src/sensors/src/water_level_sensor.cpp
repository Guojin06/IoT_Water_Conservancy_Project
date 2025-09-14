#include "water_level_sensor.h"
#include "logger.h"
#include <cmath>

namespace WaterIoT::Sensors {

WaterLevelSensor::WaterLevelSensor(const std::string& sensorId, const std::string& location)
    : SensorBase(sensorId, SensorType::WATER_LEVEL, location, "cm") {
    
    // 设置数据范围
    m_minValue = 0.0;      // 最低水位 0cm
    m_maxValue = 200.0;    // 最高水位 200cm
    
    // 添加传感器元数据
    addMetadata("sensor_model", "WL-2024-Pro");
    addMetadata("accuracy", "±0.1cm");
    addMetadata("resolution", "0.01cm");
}

bool WaterLevelSensor::initialize() {
    if (!SensorBase::initialize()) {
        return false;
    }
    
    LOG_INFO("水位传感器初始化成功: " + m_sensorId);
    LOG_INFO("  位置: " + m_location);
    LOG_INFO("  正常范围: " + std::to_string(m_normalMin) + "-" + std::to_string(m_normalMax) + "cm");
    
    return true;
}

void WaterLevelSensor::shutdown() {
    LOG_INFO("关闭水位传感器: " + m_sensorId);
    SensorBase::shutdown();
}

void WaterLevelSensor::setNormalRange(double minLevel, double maxLevel) {
    if (minLevel >= maxLevel) {
        setError("无效的水位范围设置");
        return;
    }
    
    m_normalMin = minLevel;
    m_normalMax = maxLevel;
    
    LOG_INFO("更新水位范围 [" + m_sensorId + "]: " + 
             std::to_string(minLevel) + "-" + std::to_string(maxLevel) + "cm");
}

bool WaterLevelSensor::isLevelNormal() const {
    if (m_status != SensorStatus::ONLINE) {
        return false;
    }
    
    double currentLevel = getWaterDepth();
    return (currentLevel >= m_normalMin && currentLevel <= m_normalMax);
}

double WaterLevelSensor::getWaterDepth() const {
    if (m_status != SensorStatus::ONLINE) {
        return 0.0;
    }
    
    // 这里返回最后读取的处理后的值
    // 在实际应用中，可能需要缓存最后的读数
    return const_cast<WaterLevelSensor*>(this)->readRawValue();
}

double WaterLevelSensor::readRawValue() {
    // 模拟水位传感器数据
    // 基于正弦波加噪声的模拟方式
    
    static double timeCounter = 0.0;
    timeCounter += 0.1;  // 时间步进
    
    // 基础水位：基于正弦波的缓慢变化（模拟潮汐等自然变化）
    double timeVariation = std::sin(timeCounter * 0.1) * m_variation * 0.5;
    
    // 随机噪声（模拟测量误差和环境干扰）
    double noise = m_distribution(m_rng) * 0.5;
    
    // 偶尔的突发变化（模拟降雨等事件）
    double burstChange = 0.0;
    if (std::uniform_real_distribution<double>(0.0, 1.0)(m_rng) < 0.01) {  // 1%概率
        burstChange = std::uniform_real_distribution<double>(-5.0, 15.0)(m_rng);
        LOG_DEBUG("水位突发变化: " + std::to_string(burstChange) + "cm");
    }
    
    double rawLevel = m_baseLevel + timeVariation + noise + burstChange;
    
    // 确保在物理可能的范围内
    rawLevel = std::max(0.0, std::min(200.0, rawLevel));
    
    return rawLevel;
}

} // namespace WaterIoT::Sensors
