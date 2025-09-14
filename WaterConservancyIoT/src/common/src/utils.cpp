#include "utils.h"
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <cmath>

namespace WaterIoT::Common {

// 静态成员初始化
std::random_device Utils::s_randomDevice;
std::mt19937 Utils::s_randomGenerator(s_randomDevice());

std::string Utils::getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::int64_t Utils::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
}

std::string Utils::timestampToString(std::int64_t timestamp) {
    auto time_point = std::chrono::system_clock::from_time_t(timestamp / 1000);
    auto time_t = std::chrono::system_clock::to_time_t(time_point);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::int64_t Utils::stringToTimestamp(const std::string& timeStr) {
    std::tm tm = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    if (ss.fail()) {
        return 0;
    }
    
    auto time_point = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        time_point.time_since_epoch()).count();
}

std::vector<std::string> Utils::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    
    return result;
}

std::string Utils::trim(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v";
    
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

std::string Utils::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string Utils::toUpperCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool Utils::startsWith(const std::string& str, const std::string& prefix) {
    return str.length() >= prefix.length() && 
           str.compare(0, prefix.length(), prefix) == 0;
}

bool Utils::endsWith(const std::string& str, const std::string& suffix) {
    return str.length() >= suffix.length() &&
           str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool Utils::fileExists(const std::string& filename) {
    return std::filesystem::exists(filename);
}

bool Utils::createDirectory(const std::string& path) {
    std::error_code ec;
    return std::filesystem::create_directories(path, ec);
}

std::string Utils::getFileExtension(const std::string& filename) {
    std::filesystem::path path(filename);
    return path.extension().string();
}

std::string Utils::getBasename(const std::string& path) {
    std::filesystem::path p(path);
    return p.filename().string();
}

std::string Utils::getDirname(const std::string& path) {
    std::filesystem::path p(path);
    return p.parent_path().string();
}

double Utils::clamp(double value, double min, double max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

int Utils::randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(s_randomGenerator);
}

double Utils::randomDouble(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(s_randomGenerator);
}

double Utils::roundToDecimal(double value, int decimalPlaces) {
    double factor = std::pow(10.0, decimalPlaces);
    return std::round(value * factor) / factor;
}

bool Utils::isValidEmail(const std::string& email) {
    const std::regex emailPattern(
        R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})"
    );
    return std::regex_match(email, emailPattern);
}

bool Utils::isValidIP(const std::string& ip) {
    const std::regex ipPattern(
        R"(^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$)"
    );
    
    std::smatch match;
    if (!std::regex_match(ip, match, ipPattern)) {
        return false;
    }
    
    for (int i = 1; i <= 4; ++i) {
        int octet = std::stoi(match[i].str());
        if (octet < 0 || octet > 255) {
            return false;
        }
    }
    
    return true;
}

bool Utils::isValidPort(int port) {
    return port > 0 && port <= 65535;
}

std::string Utils::base64Encode(const std::string& data) {
    // 简化版base64编码实现
    const std::string chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    std::string result;
    int i = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    
    for (char c : data) {
        char_array_3[i++] = c;
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            
            for (i = 0; i < 4; i++) {
                result += chars[char_array_4[i]];
            }
            i = 0;
        }
    }
    
    if (i) {
        for (int j = i; j < 3; j++) {
            char_array_3[j] = '\0';
        }
        
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
        
        for (int j = 0; j < i + 1; j++) {
            result += chars[char_array_4[j]];
        }
        
        while (i++ < 3) {
            result += '=';
        }
    }
    
    return result;
}

std::string Utils::base64Decode(const std::string& encoded) {
    // 简化实现，实际项目中可使用专门的base64库
    return ""; // 占位符实现
}

std::string Utils::urlEncode(const std::string& data) {
    std::ostringstream oss;
    for (char c : data) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            oss << c;
        } else {
            oss << '%' << std::hex << std::uppercase << (unsigned char)c;
        }
    }
    return oss.str();
}

std::string Utils::urlDecode(const std::string& encoded) {
    std::string result;
    for (size_t i = 0; i < encoded.length(); ++i) {
        if (encoded[i] == '%' && i + 2 < encoded.length()) {
            int value = std::stoi(encoded.substr(i + 1, 2), nullptr, 16);
            result += static_cast<char>(value);
            i += 2;
        } else if (encoded[i] == '+') {
            result += ' ';
        } else {
            result += encoded[i];
        }
    }
    return result;
}

// 注意：这些哈希函数需要OpenSSL库，为了简化，我们先提供占位符实现
std::string Utils::md5Hash(const std::string& data) {
    // 占位符实现，实际项目中需要链接OpenSSL
    return "md5_placeholder";
}

std::string Utils::sha256Hash(const std::string& data) {
    // 占位符实现，实际项目中需要链接OpenSSL
    return "sha256_placeholder";
}

} // namespace WaterIoT::Common
