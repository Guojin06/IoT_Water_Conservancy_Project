#include "config.h"
#include <fstream>
#include <iostream>

namespace WaterIoT::Common {

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

bool Config::loadFromFile(const std::string& configFile) {
    try {
        std::ifstream file(configFile);
        if (!file.is_open()) {
            std::cerr << "Cannot open config file: " << configFile << std::endl;
            return false;
        }
        
        file >> m_config;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing config file " << configFile << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromJson(const nlohmann::json& jsonConfig) {
    try {
        m_config = jsonConfig;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading JSON config: " << e.what() << std::endl;
        return false;
    }
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    try {
        if (m_config.contains(key)) {
            return m_config[key].get<std::string>();
        }
    } catch (const std::exception&) {
        // 类型转换失败，返回默认值
    }
    return defaultValue;
}

int Config::getInt(const std::string& key, int defaultValue) const {
    try {
        if (m_config.contains(key)) {
            return m_config[key].get<int>();
        }
    } catch (const std::exception&) {
        // 类型转换失败，返回默认值
    }
    return defaultValue;
}

double Config::getDouble(const std::string& key, double defaultValue) const {
    try {
        if (m_config.contains(key)) {
            return m_config[key].get<double>();
        }
    } catch (const std::exception&) {
        // 类型转换失败，返回默认值
    }
    return defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
    try {
        if (m_config.contains(key)) {
            return m_config[key].get<bool>();
        }
    } catch (const std::exception&) {
        // 类型转换失败，返回默认值
    }
    return defaultValue;
}

void Config::setString(const std::string& key, const std::string& value) {
    m_config[key] = value;
}

void Config::setInt(const std::string& key, int value) {
    m_config[key] = value;
}

void Config::setDouble(const std::string& key, double value) {
    m_config[key] = value;
}

void Config::setBool(const std::string& key, bool value) {
    m_config[key] = value;
}

std::string Config::getString(const std::string& section, const std::string& key, const std::string& defaultValue) const {
    try {
        if (m_config.contains(section) && m_config[section].contains(key)) {
            return m_config[section][key].get<std::string>();
        }
    } catch (const std::exception&) {
        // 类型转换失败，返回默认值
    }
    return defaultValue;
}

int Config::getInt(const std::string& section, const std::string& key, int defaultValue) const {
    try {
        if (m_config.contains(section) && m_config[section].contains(key)) {
            return m_config[section][key].get<int>();
        }
    } catch (const std::exception&) {
        // 类型转换失败，返回默认值
    }
    return defaultValue;
}

double Config::getDouble(const std::string& section, const std::string& key, double defaultValue) const {
    try {
        if (m_config.contains(section) && m_config[section].contains(key)) {
            return m_config[section][key].get<double>();
        }
    } catch (const std::exception&) {
        // 类型转换失败，返回默认值
    }
    return defaultValue;
}

bool Config::getBool(const std::string& section, const std::string& key, bool defaultValue) const {
    try {
        if (m_config.contains(section) && m_config[section].contains(key)) {
            return m_config[section][key].get<bool>();
        }
    } catch (const std::exception&) {
        // 类型转换失败，返回默认值
    }
    return defaultValue;
}

bool Config::hasKey(const std::string& key) const {
    return m_config.contains(key);
}

bool Config::hasSection(const std::string& section) const {
    return m_config.contains(section) && m_config[section].is_object();
}

nlohmann::json Config::getAllConfig() const {
    return m_config;
}

bool Config::saveToFile(const std::string& configFile) const {
    try {
        std::ofstream file(configFile);
        if (!file.is_open()) {
            std::cerr << "Cannot create config file: " << configFile << std::endl;
            return false;
        }
        
        file << std::setw(2) << m_config << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config file " << configFile << ": " << e.what() << std::endl;
        return false;
    }
}

} // namespace WaterIoT::Common
