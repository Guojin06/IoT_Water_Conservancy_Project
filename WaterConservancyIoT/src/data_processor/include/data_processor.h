#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include "../../sensors/include/sensor_base.h"

namespace WaterIoT::DataProcessor {

// 数据处理状态
enum class ProcessStatus {
    RAW,        // 原始数据
    CLEANED,    // 已清洗
    FILTERED,   // 已过滤
    AGGREGATED  // 已聚合
};

// 处理结果数据
struct ProcessedData {
    std::vector<WaterIoT::Sensors::SensorData> data;
    ProcessStatus status;
    std::chrono::system_clock::time_point processTime;
    std::string processInfo;
    
    ProcessedData(std::vector<WaterIoT::Sensors::SensorData> d, 
                  ProcessStatus s, 
                  const std::string& info = "")
        : data(std::move(d)), status(s), 
          processTime(std::chrono::system_clock::now()), 
          processInfo(info) {}
};

// 聚合统计结果
struct AggregatedResult {
    double average;
    double minimum;
    double maximum;
    size_t count;
    double standardDev;
    std::string summary;
    
    AggregatedResult() 
        : average(0.0), minimum(0.0), maximum(0.0), 
          count(0), standardDev(0.0), summary("空数据") {}
};

// 数据处理器接口
class IDataProcessor {
public:
    virtual ~IDataProcessor() = default;
    
    // 数据清洗
    virtual ProcessedData cleanData(const std::vector<WaterIoT::Sensors::SensorData>& rawData) = 0;
    
    // 数据过滤
    virtual ProcessedData filterData(const ProcessedData& data) = 0;
    
    // 数据聚合
    virtual AggregatedResult aggregateData(const ProcessedData& data) = 0;
    
    // 完整处理流程
    virtual AggregatedResult processDataPipeline(const std::vector<WaterIoT::Sensors::SensorData>& rawData) = 0;
};

// 水利数据处理器具体实现
class WaterDataProcessor : public IDataProcessor {
private:
    double minValidValue_;
    double maxValidValue_;
    double outlierThreshold_;  // 异常值阈值

public:
    WaterDataProcessor(double minVal = 0.0, double maxVal = 100.0, double threshold = 2.0);
    
    ProcessedData cleanData(const std::vector<WaterIoT::Sensors::SensorData>& rawData) override;
    ProcessedData filterData(const ProcessedData& data) override;
    AggregatedResult aggregateData(const ProcessedData& data) override;
    AggregatedResult processDataPipeline(const std::vector<WaterIoT::Sensors::SensorData>& rawData) override;

private:
    bool isValidData(const WaterIoT::Sensors::SensorData& data) const;
    bool isOutlier(const WaterIoT::Sensors::SensorData& data, double mean, double stdDev) const;
    double calculateMean(const std::vector<WaterIoT::Sensors::SensorData>& data) const;
    double calculateStdDev(const std::vector<WaterIoT::Sensors::SensorData>& data, double mean) const;
};

} // namespace WaterIoT::DataProcessor
