#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <nlohmann/json.hpp>

namespace WaterIoT::Common {

class Config {
public:
    static Config& getInstance();
    
    bool loadFromFile(const std::string& configFile);
    bool loadFromJson(const nlohmann::json& jsonConfig);
    
    // 获取配置值
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    // 设置配置值
    void setString(const std::string& key, const std::string& value);
    void setInt(const std::string& key, int value);
    void setDouble(const std::string& key, double value);
    void setBool(const std::string& key, bool value);
    
    // 嵌套配置访问
    std::string getString(const std::string& section, const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& section, const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& section, const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& section, const std::string& key, bool defaultValue = false) const;
    
    // 检查配置是否存在
    bool hasKey(const std::string& key) const;
    bool hasSection(const std::string& section) const;
    
    // 获取所有配置
    nlohmann::json getAllConfig() const;
    
    // 保存配置到文件
    bool saveToFile(const std::string& configFile) const;

private:
    Config() = default;
    ~Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    nlohmann::json m_config;
};

} // namespace WaterIoT::Common
