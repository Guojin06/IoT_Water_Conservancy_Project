#pragma once

#include "sensor_base.h"
#include <random>

namespace WaterIoT::Sensors {

class WaterLevelSensor : public SensorBase {
public:
    WaterLevelSensor(const std::string& sensorId, const std::string& location);
    virtual ~WaterLevelSensor() = default;

    // 实现基类接口
    bool initialize() override;
    void shutdown() override;

    // 水位传感器特有功能
    void setNormalRange(double minLevel, double maxLevel);
    bool isLevelNormal() const;
    double getWaterDepth() const;

protected:
    // 实现核心读取逻辑
    double readRawValue() override;

private:
    // 模拟参数
    double m_baseLevel{50.0};       // 基准水位 (cm)
    double m_variation{10.0};       // 变化幅度 (cm)
    double m_normalMin{30.0};       // 正常水位下限 (cm)
    double m_normalMax{80.0};       // 正常水位上限 (cm)
    
    // 随机数生成器
    mutable std::mt19937 m_rng{std::random_device{}()};
    mutable std::normal_distribution<double> m_distribution{0.0, 1.0};
};

} // namespace WaterIoT::Sensors