#include <gtest/gtest.h>
#include <memory>

// 🌊 简单的传感器类（模拟）
class WaterLevelSensor {
public:
    WaterLevelSensor(const std::string& id, double baseLevel) 
        : m_id(id), m_baseLevel(baseLevel), m_isInitialized(false) {}
    
    bool initialize() {
        m_isInitialized = true;
        return true;
    }
    
    void shutdown() {
        m_isInitialized = false;
    }
    
    double readValue() {
        if (!m_isInitialized) return -1.0;
        return m_baseLevel + (rand() % 100) / 100.0;  // 模拟读取
    }
    
    bool isInitialized() const { return m_isInitialized; }
    std::string getId() const { return m_id; }

private:
    std::string m_id;
    double m_baseLevel;
    bool m_isInitialized;
};

// 🏗️ 测试夹具类
class SensorTest : public ::testing::Test {
protected:
    // ⬆️ SetUp: 每个测试用例开始前调用
    void SetUp() override {
        std::cout << "🔧 设置测试环境..." << std::endl;
        
        // 创建传感器
        sensor = std::make_unique<WaterLevelSensor>("WL_001", 15.0);
        
        // 初始化传感器
        bool initResult = sensor->initialize();
        ASSERT_TRUE(initResult);  // 如果初始化失败，测试没法继续
        
        // 记录开始时间
        startTime = std::chrono::steady_clock::now();
    }
    
    // ⬇️ TearDown: 每个测试用例结束后调用
    void TearDown() override {
        std::cout << "🧹 清理测试环境..." << std::endl;
        
        // 关闭传感器
        if (sensor) {
            sensor->shutdown();
            sensor.reset();
        }
        
        // 记录测试时间
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << "⏱️ 测试用时: " << duration.count() << "ms" << std::endl;
    }
    
    // 🛠️ 辅助方法（测试用例可以使用）
    bool isValueInValidRange(double value) {
        return value >= 14.0 && value <= 16.0;
    }
    
    void waitOneSecond() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

protected:
    // 📊 测试数据（所有测试用例共享）
    std::unique_ptr<WaterLevelSensor> sensor;
    std::chrono::steady_clock::time_point startTime;
};

// 🧪 使用测试夹具的测试用例
TEST_F(SensorTest, BasicInitialization) {
    // sensor 已经在 SetUp 中创建和初始化了！
    
    EXPECT_TRUE(sensor->isInitialized());
    EXPECT_EQ("WL_001", sensor->getId());
}

TEST_F(SensorTest, ReadSingleValue) {
    // sensor 已经准备好了
    
    double value = sensor->readValue();
    
    EXPECT_GT(value, 0);                    // 值应该大于0
    EXPECT_TRUE(isValueInValidRange(value)); // 使用辅助方法
}

TEST_F(SensorTest, ReadMultipleValues) {
    std::vector<double> readings;
    
    // 读取10次数据
    for (int i = 0; i < 10; ++i) {
        double value = sensor->readValue();
        readings.push_back(value);
        
        EXPECT_TRUE(isValueInValidRange(value));
        waitOneSecond();  // 使用辅助方法
    }
    
    // 验证读取了10个值
    EXPECT_EQ(10, readings.size());
    
    // 验证所有值都不同（因为有随机性）
    bool allSame = true;
    for (size_t i = 1; i < readings.size(); ++i) {
        if (readings[i] != readings[0]) {
            allSame = false;
            break;
        }
    }
    EXPECT_FALSE(allSame);  // 期望不是所有值都相同
}

TEST_F(SensorTest, ShutdownAndRestart) {
    // 先关闭传感器
    sensor->shutdown();
    EXPECT_FALSE(sensor->isInitialized());
    
    // 尝试读取应该失败
    double value = sensor->readValue();
    EXPECT_EQ(-1.0, value);
    
    // 重新初始化
    bool result = sensor->initialize();
    EXPECT_TRUE(result);
    EXPECT_TRUE(sensor->isInitialized());
    
    // 现在应该能正常读取了
    value = sensor->readValue();
    EXPECT_TRUE(isValueInValidRange(value));
}

// 🎯 主函数
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    std::cout << "🧪 开始运行传感器测试夹具示例..." << std::endl;
    return RUN_ALL_TESTS();
}
