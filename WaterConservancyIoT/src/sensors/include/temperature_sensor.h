#pragma once

#include "sensor_base.h"
#include <random>

namespace WaterIoT::Sensors {

class TemperatureSensor : public SensorBase {
public:
    TemperatureSensor(const std::string& sensorId, const std::string& location);
    virtual ~TemperatureSensor() = default;

    // 实现基类接口
    bool initialize() override;
    void shutdown() override;

    // 温度传感器特有功能
    void setTemperatureRange(double minTemp, double maxTemp);
    bool isTemperatureNormal() const;
    double getCelsius() const;
    double getFahrenheit() const;

protected:
    // 实现核心读取逻辑
    double readRawValue() override;

private:
    // 模拟参数
    double m_baseTemperature{20.0};    // 基准温度 (°C)
    double m_dailyVariation{8.0};      // 日温差 (°C)
    double m_normalMin{15.0};          // 正常温度下限 (°C)
    double m_normalMax{35.0};          // 正常温度上限 (°C)
    
    // 随机数生成器
    mutable std::mt19937 m_rng{std::random_device{}()};
    mutable std::normal_distribution<double> m_distribution{0.0, 1.0};
};

} // namespace WaterIoT::Sensors
