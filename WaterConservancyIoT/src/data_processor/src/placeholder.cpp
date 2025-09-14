#include "../include/data_processor.h"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace WaterIoT::DataProcessor {

WaterDataProcessor::WaterDataProcessor(double minVal, double maxVal, double threshold)
    : minValidValue_(minVal), maxValidValue_(maxVal), outlierThreshold_(threshold) {}

ProcessedData WaterDataProcessor::cleanData(const std::vector<WaterIoT::Sensors::SensorData>& rawData) {
    std::vector<WaterIoT::Sensors::SensorData> cleanedData;
    
    for (const auto& data : rawData) {
        if (isValidData(data)) {
            cleanedData.push_back(data);
        }
    }
    
    std::string info = "数据清洗完成: 原始" + std::to_string(rawData.size()) + 
                      "条, 有效" + std::to_string(cleanedData.size()) + "条";
    
    return ProcessedData(std::move(cleanedData), ProcessStatus::CLEANED, info);
}

ProcessedData WaterDataProcessor::filterData(const ProcessedData& inputData) {
    if (inputData.data.empty()) {
        return ProcessedData({}, ProcessStatus::FILTERED, "无数据需要过滤");
    }
    
    // 计算平均值和标准差用于异常值检测
    double mean = calculateMean(inputData.data);
    double stdDev = calculateStdDev(inputData.data, mean);
    
    std::vector<WaterIoT::Sensors::SensorData> filteredData;
    for (const auto& data : inputData.data) {
        if (!isOutlier(data, mean, stdDev)) {
            filteredData.push_back(data);
        }
    }
    
    std::string info = "异常值过滤完成: 过滤前" + std::to_string(inputData.data.size()) +
                      "条, 过滤后" + std::to_string(filteredData.size()) + "条";
    
    return ProcessedData(std::move(filteredData), ProcessStatus::FILTERED, info);
}

AggregatedResult WaterDataProcessor::aggregateData(const ProcessedData& inputData) {
    AggregatedResult result;
    
    if (inputData.data.empty()) {
        result.summary = "无数据可聚合";
        return result;
    }
    
    // 提取所有数值
    std::vector<double> values;
    for (const auto& data : inputData.data) {
        values.push_back(data.value);
    }
    
    // 计算统计值
    result.count = values.size();
    result.average = calculateMean(inputData.data);
    result.minimum = *std::min_element(values.begin(), values.end());
    result.maximum = *std::max_element(values.begin(), values.end());
    result.standardDev = calculateStdDev(inputData.data, result.average);
    
    result.summary = "聚合完成: 平均值=" + std::to_string(result.average) +
                    ", 范围[" + std::to_string(result.minimum) + 
                    "," + std::to_string(result.maximum) + "]";
    
    return result;
}

AggregatedResult WaterDataProcessor::processDataPipeline(const std::vector<WaterIoT::Sensors::SensorData>& rawData) {
    // 完整的数据处理流水线
    auto cleaned = cleanData(rawData);
    auto filtered = filterData(cleaned);
    return aggregateData(filtered);
}

// 私有辅助方法
bool WaterDataProcessor::isValidData(const WaterIoT::Sensors::SensorData& data) const {
    return !std::isnan(data.value) && 
           !std::isinf(data.value) && 
           data.value >= minValidValue_ && 
           data.value <= maxValidValue_;
}

bool WaterDataProcessor::isOutlier(const WaterIoT::Sensors::SensorData& data, double mean, double stdDev) const {
    double deviation = std::abs(data.value - mean);
    return deviation > (outlierThreshold_ * stdDev);
}

double WaterDataProcessor::calculateMean(const std::vector<WaterIoT::Sensors::SensorData>& data) const {
    if (data.empty()) return 0.0;
    
    double sum = 0.0;
    for (const auto& item : data) {
        sum += item.value;
    }
    return sum / data.size();
}

double WaterDataProcessor::calculateStdDev(const std::vector<WaterIoT::Sensors::SensorData>& data, double mean) const {
    if (data.empty()) return 0.0;
    
    double variance = 0.0;
    for (const auto& item : data) {
        variance += std::pow(item.value - mean, 2);
    }
    return std::sqrt(variance / data.size());
}

} // namespace WaterIoT::DataProcessor
