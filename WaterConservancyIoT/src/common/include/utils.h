#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <random>

namespace WaterIoT::Common {

class Utils {
public:
    // 时间相关工具
    static std::string getCurrentTimeString();
    static std::int64_t getCurrentTimestamp();
    static std::string timestampToString(std::int64_t timestamp);
    static std::int64_t stringToTimestamp(const std::string& timeStr);
    
    // 字符串处理工具
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string trim(const std::string& str);
    static std::string toLowerCase(const std::string& str);
    static std::string toUpperCase(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    
    // 文件系统工具
    static bool fileExists(const std::string& filename);
    static bool createDirectory(const std::string& path);
    static std::string getFileExtension(const std::string& filename);
    static std::string getBasename(const std::string& path);
    static std::string getDirname(const std::string& path);
    
    // 数值工具
    static double clamp(double value, double min, double max);
    static int randomInt(int min, int max);
    static double randomDouble(double min, double max);
    static double roundToDecimal(double value, int decimalPlaces);
    
    // 数据验证工具
    static bool isValidEmail(const std::string& email);
    static bool isValidIP(const std::string& ip);
    static bool isValidPort(int port);
    
    // 编码工具
    static std::string base64Encode(const std::string& data);
    static std::string base64Decode(const std::string& encoded);
    static std::string urlEncode(const std::string& data);
    static std::string urlDecode(const std::string& encoded);
    
    // 哈希工具
    static std::string md5Hash(const std::string& data);
    static std::string sha256Hash(const std::string& data);

private:
    static std::random_device s_randomDevice;
    static std::mt19937 s_randomGenerator;
};

} // namespace WaterIoT::Common
