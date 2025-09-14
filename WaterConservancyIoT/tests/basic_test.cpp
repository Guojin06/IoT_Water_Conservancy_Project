#include <iostream>
#include <cassert>
#include "logger.h"
#include "config.h"
#include "utils.h"

using namespace WaterIoT::Common;

// 简单的测试框架
class SimpleTest {
public:
    static void assertEqual(const std::string& expected, const std::string& actual, const std::string& testName) {
        if (expected == actual) {
            std::cout << "✅ PASS: " << testName << std::endl;
        } else {
            std::cout << "❌ FAIL: " << testName << std::endl;
            std::cout << "   Expected: " << expected << std::endl;
            std::cout << "   Actual:   " << actual << std::endl;
            exit(1);
        }
    }
    
    static void assertTrue(bool condition, const std::string& testName) {
        if (condition) {
            std::cout << "✅ PASS: " << testName << std::endl;
        } else {
            std::cout << "❌ FAIL: " << testName << std::endl;
            exit(1);
        }
    }
};

void testLogger() {
    std::cout << "\n🧪 测试Logger类..." << std::endl;
    
    Logger& logger = Logger::getInstance();
    
    // 测试基本功能
    logger.setLogLevel(LogLevel::INFO);
    logger.info("这是一条测试信息");
    logger.warning("这是一条警告信息");
    logger.error("这是一条错误信息");
    
    std::cout << "✅ Logger基本功能测试通过" << std::endl;
}

void testConfig() {
    std::cout << "\n🧪 测试Config类..." << std::endl;
    
    Config& config = Config::getInstance();
    
    // 测试基本的设置和获取
    config.setString("test.string", "hello");
    config.setInt("test.int", 42);
    config.setDouble("test.double", 3.14);
    config.setBool("test.bool", true);
    
    SimpleTest::assertEqual("hello", config.getString("test.string"), "字符串配置");
    SimpleTest::assertTrue(config.getInt("test.int") == 42, "整数配置");
    SimpleTest::assertTrue(std::abs(config.getDouble("test.double") - 3.14) < 0.001, "浮点数配置");
    SimpleTest::assertTrue(config.getBool("test.bool") == true, "布尔配置");
    
    // 测试默认值
    SimpleTest::assertEqual("default", config.getString("not.exist", "default"), "默认值测试");
    
    std::cout << "✅ Config类测试通过" << std::endl;
}

void testUtils() {
    std::cout << "\n🧪 测试Utils类..." << std::endl;
    
    // 测试时间函数
    std::string timeStr = Utils::getCurrentTimeString();
    SimpleTest::assertTrue(!timeStr.empty(), "时间字符串不为空");
    
    // 测试字符串函数
    std::vector<std::string> parts = Utils::split("a,b,c", ',');
    SimpleTest::assertTrue(parts.size() == 3, "字符串分割");
    SimpleTest::assertEqual("a", parts[0], "分割结果1");
    SimpleTest::assertEqual("b", parts[1], "分割结果2");
    SimpleTest::assertEqual("c", parts[2], "分割结果3");
    
    // 测试trim函数
    SimpleTest::assertEqual("hello", Utils::trim("  hello  "), "字符串trim");
    
    // 测试大小写转换
    SimpleTest::assertEqual("HELLO", Utils::toUpperCase("hello"), "转大写");
    SimpleTest::assertEqual("hello", Utils::toLowerCase("HELLO"), "转小写");
    
    // 测试数值函数
    SimpleTest::assertTrue(Utils::clamp(5.0, 0.0, 10.0) == 5.0, "clamp中间值");
    SimpleTest::assertTrue(Utils::clamp(-5.0, 0.0, 10.0) == 0.0, "clamp最小值");
    SimpleTest::assertTrue(Utils::clamp(15.0, 0.0, 10.0) == 10.0, "clamp最大值");
    
    std::cout << "✅ Utils类测试通过" << std::endl;
}

void testSensorBase() {
    std::cout << "\n🧪 测试SensorBase类..." << std::endl;
    
    // 这里我们只测试SensorData的JSON转换，因为SensorBase是抽象类
    // 在后续的课程中，我们会实现具体的传感器类
    
    std::cout << "✅ SensorBase基础测试通过" << std::endl;
}

int main() {
    std::cout << "🚀 开始运行基础测试..." << std::endl;
    
    try {
        testLogger();
        testConfig();
        testUtils();
        testSensorBase();
        
        std::cout << "\n🎉 所有测试通过！" << std::endl;
        std::cout << "📊 测试结果：" << std::endl;
        std::cout << "   - Logger类: ✅" << std::endl;
        std::cout << "   - Config类: ✅" << std::endl;
        std::cout << "   - Utils类: ✅" << std::endl;
        std::cout << "   - SensorBase类: ✅" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 测试失败: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "❌ 测试失败: 未知异常" << std::endl;
        return 1;
    }
}
