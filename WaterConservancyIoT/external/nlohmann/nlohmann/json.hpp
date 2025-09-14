// 简化版nlohmann/json库头文件
// 实际项目中应该下载完整版本

#pragma once

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>

namespace nlohmann {

class json {
public:
    json() = default;
    json(const std::string& str) : m_string(str), m_type(Type::STRING) {}
    json(int val) : m_int(val), m_type(Type::INT) {}
    json(double val) : m_double(val), m_type(Type::DOUBLE) {}
    json(bool val) : m_bool(val), m_type(Type::BOOL) {}
    
    // 数组和对象构造
    json(std::initializer_list<json> list) {
        m_type = Type::OBJECT;
        // 简化实现
    }
    
    // 赋值操作
    json& operator=(const std::string& str) {
        m_string = str;
        m_type = Type::STRING;
        return *this;
    }
    
    json& operator=(int val) {
        m_int = val;
        m_type = Type::INT;
        return *this;
    }
    
    json& operator=(double val) {
        m_double = val;
        m_type = Type::DOUBLE;
        return *this;
    }
    
    json& operator=(bool val) {
        m_bool = val;
        m_type = Type::BOOL;
        return *this;
    }
    
    // 下标操作
    json& operator[](const std::string& key) {
        if (m_type != Type::OBJECT) {
            m_type = Type::OBJECT;
        }
        return m_object[key];
    }
    
    const json& operator[](const std::string& key) const {
        static json null_json;
        auto it = m_object.find(key);
        return (it != m_object.end()) ? it->second : null_json;
    }
    
    // 类型转换
    template<typename T>
    T get() const {
        if constexpr (std::is_same_v<T, std::string>) {
            return m_string;
        } else if constexpr (std::is_same_v<T, int>) {
            return m_int;
        } else if constexpr (std::is_same_v<T, double>) {
            return m_double;
        } else if constexpr (std::is_same_v<T, bool>) {
            return m_bool;
        }
        return T{};
    }
    
    // 检查是否包含键
    bool contains(const std::string& key) const {
        return m_type == Type::OBJECT && m_object.find(key) != m_object.end();
    }
    
    // 获取值（带默认值）
    template<typename T>
    T value(const std::string& key, const T& defaultVal) const {
        if (contains(key)) {
            return operator[](key).get<T>();
        }
        return defaultVal;
    }
    
    // 类型检查
    bool is_object() const { return m_type == Type::OBJECT; }
    bool is_string() const { return m_type == Type::STRING; }
    bool is_number() const { return m_type == Type::INT || m_type == Type::DOUBLE; }
    
    // 输入输出流操作
    friend std::istream& operator>>(std::istream& is, json& j) {
        // 简化的JSON解析
        std::string line;
        std::getline(is, line);
        // 这里应该有完整的JSON解析逻辑
        return is;
    }
    
    friend std::ostream& operator<<(std::ostream& os, const json& j) {
        // 简化的JSON输出
        os << "{}"; // 占位符
        return os;
    }

private:
    enum class Type {
        OBJECT,
        ARRAY,
        STRING,
        INT,
        DOUBLE,
        BOOL,
        NULL_TYPE
    };
    
    Type m_type = Type::NULL_TYPE;
    std::map<std::string, json> m_object;
    std::vector<json> m_array;
    std::string m_string;
    int m_int = 0;
    double m_double = 0.0;
    bool m_bool = false;
};

// 辅助函数
inline json setw(int width) {
    // 格式化输出宽度设置
    return json{};
}

} // namespace nlohmann
