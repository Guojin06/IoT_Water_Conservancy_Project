#include "temperature_sensor.h"
#include "logger.h"
#include <cmath>

namespace WaterIoT::Sensors {

TemperatureSensor::TemperatureSensor(const std::string& sensorId, const std::string& location)
    : SensorBase(sensorId, SensorType::TEMPERATURE, location, "°C") {
    
    // 设置数据范围
    m_minValue = -40.0;    // 最低温度 -40°C
    m_maxValue = 80.0;     // 最高温度 80°C
    
    // 添加传感器元数据
    addMetadata("sensor_model", "TEMP-DS18B20");
    addMetadata("accuracy", "±0.5°C");
    addMetadata("resolution", "0.1°C");
    addMetadata("response_time", "750ms");
}

bool TemperatureSensor::initialize() {
    if (!SensorBase::initialize()) {
        return false;
    }
    
    LOG_INFO("温度传感器初始化成功: " + m_sensorId);
    LOG_INFO("  位置: " + m_location);
    LOG_INFO("  正常范围: " + std::to_string(m_normalMin) + "-" + std::to_string(m_normalMax) + "°C");
    
    return true;
}

void TemperatureSensor::shutdown() {
    LOG_INFO("关闭温度传感器: " + m_sensorId);
    SensorBase::shutdown();
}

void TemperatureSensor::setTemperatureRange(double minTemp, double maxTemp) {
    if (minTemp >= maxTemp) {
        setError("无效的温度范围设置");
        return;
    }
    
    m_normalMin = minTemp;
    m_normalMax = maxTemp;
    
    LOG_INFO("更新温度范围 [" + m_sensorId + "]: " + 
             std::to_string(minTemp) + "-" + std::to_string(maxTemp) + "°C");
}

bool TemperatureSensor::isTemperatureNormal() const {
    if (m_status != SensorStatus::ONLINE) {
        return false;
    }
    
    double currentTemp = getCelsius();
    return (currentTemp >= m_normalMin && currentTemp <= m_normalMax);
}

double TemperatureSensor::getCelsius() const {
    if (m_status != SensorStatus::ONLINE) {
        return 0.0;
    }
    
    return const_cast<TemperatureSensor*>(this)->readRawValue();
}

double TemperatureSensor::getFahrenheit() const {
    double celsius = getCelsius();
    return celsius * 9.0 / 5.0 + 32.0;
}

double TemperatureSensor::readRawValue() {
    // 模拟温度传感器数据
    // 基于时间的周期性变化（模拟昼夜温差）
    
    static double timeCounter = 0.0;
    timeCounter += 0.1;  // 时间步进
    
    // 模拟一天24小时的温度变化（简化为正弦波）
    double hourlyVariation = std::sin(timeCounter * 0.05) * m_dailyVariation;
    
    // 季节性变化（长周期）
    double seasonalVariation = std::sin(timeCounter * 0.001) * 5.0;
    
    // 随机噪声（模拟测量误差）
    double noise = m_distribution(m_rng) * 0.2;
    
    // 偶尔的突变（模拟天气变化）
    double weatherChange = 0.0;
    if (std::uniform_real_distribution<double>(0.0, 1.0)(m_rng) < 0.005) {  // 0.5%概率
        weatherChange = std::uniform_real_distribution<double>(-3.0, 3.0)(m_rng);
        LOG_DEBUG("温度突变: " + std::to_string(weatherChange) + "°C");
    }
    
    double temperature = m_baseTemperature + hourlyVariation + seasonalVariation + noise + weatherChange;
    
    // 四舍五入到0.1度精度
    temperature = std::round(temperature * 10.0) / 10.0;
    
    return temperature;
}

} // namespace WaterIoT::Sensors
