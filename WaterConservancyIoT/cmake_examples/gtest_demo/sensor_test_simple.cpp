#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>

// 🌊 简化的传感器类（不依赖外部头文件）
class SimpleSensor {
public:
    SimpleSensor(const std::string& id, double baseValue) 
        : m_id(id), m_baseValue(baseValue), m_isOnline(false) {}
    
    bool start() {
        m_isOnline = true;
        return true;
    }
    
    void stop() {
        m_isOnline = false;
    }
    
    double readValue() {
        if (!m_isOnline) return -999.0;  // 错误值
        return m_baseValue + (rand() % 10) / 10.0;  // 模拟读取
    }
    
    bool isOnline() const { return m_isOnline; }
    std::string getId() const { return m_id; }
    double getBaseValue() const { return m_baseValue; }

private:
    std::string m_id;
    double m_baseValue;
    bool m_isOnline;
};

// 🏗️ 测试夹具类 - 自动管理传感器
class SensorTest : public ::testing::Test {
protected:
    // ⬆️ 每个测试开始前运行
    void SetUp() override {
        std::cout << "🔧 设置传感器测试环境..." << std::endl;
        
        // 创建传感器
        waterSensor = std::make_unique<SimpleSensor>("WL_001", 15.0);
        tempSensor = std::make_unique<SimpleSensor>("TE_001", 25.0);
        
        // 启动传感器
        ASSERT_TRUE(waterSensor->start());
        ASSERT_TRUE(tempSensor->start());
        
        testCount++;
        std::cout << "📊 这是第 " << testCount << " 个测试" << std::endl;
    }
    
    // ⬇️ 每个测试结束后运行
    void TearDown() override {
        std::cout << "🧹 清理传感器测试环境..." << std::endl;
        
        if (waterSensor) {
            waterSensor->stop();
        }
        if (tempSensor) {
            tempSensor->stop();
        }
        
        std::cout << "✅ 测试环境清理完成" << std::endl;
    }
    
    // 🛠️ 辅助方法
    bool isWaterLevelNormal(double level) {
        return level >= 14.0 && level <= 16.0;
    }
    
    bool isTempNormal(double temp) {
        return temp >= 24.0 && temp <= 26.0;
    }

protected:
    std::unique_ptr<SimpleSensor> waterSensor;
    std::unique_ptr<SimpleSensor> tempSensor;
    static int testCount;  // 测试计数器
};

// 静态成员初始化
int SensorTest::testCount = 0;

// 🧪 使用测试夹具的测试用例
TEST_F(SensorTest, SensorInitialization) {
    // 传感器已经在SetUp中创建并启动了！
    
    EXPECT_TRUE(waterSensor->isOnline());
    EXPECT_TRUE(tempSensor->isOnline());
    
    EXPECT_EQ("WL_001", waterSensor->getId());
    EXPECT_EQ("TE_001", tempSensor->getId());
    
    EXPECT_DOUBLE_EQ(15.0, waterSensor->getBaseValue());
    EXPECT_DOUBLE_EQ(25.0, tempSensor->getBaseValue());
    
    std::cout << "✅ 传感器初始化测试通过！" << std::endl;
}

TEST_F(SensorTest, ReadSingleValue) {
    double waterLevel = waterSensor->readValue();
    double temperature = tempSensor->readValue();
    
    // 验证读取的值在合理范围内
    EXPECT_TRUE(isWaterLevelNormal(waterLevel));
    EXPECT_TRUE(isTempNormal(temperature));
    
    // 验证值不是错误值
    EXPECT_NE(-999.0, waterLevel);
    EXPECT_NE(-999.0, temperature);
    
    std::cout << "📊 水位: " << waterLevel << "m, 温度: " << temperature << "°C" << std::endl;
    std::cout << "✅ 单次读取测试通过！" << std::endl;
}

TEST_F(SensorTest, ReadMultipleValues) {
    std::vector<double> waterReadings;
    std::vector<double> tempReadings;
    
    // 读取5次数据
    for (int i = 0; i < 5; ++i) {
        double water = waterSensor->readValue();
        double temp = tempSensor->readValue();
        
        waterReadings.push_back(water);
        tempReadings.push_back(temp);
        
        EXPECT_TRUE(isWaterLevelNormal(water));
        EXPECT_TRUE(isTempNormal(temp));
        
        // 模拟等待间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // 验证读取了正确数量的数据
    EXPECT_EQ(5, waterReadings.size());
    EXPECT_EQ(5, tempReadings.size());
    
    std::cout << "✅ 多次读取测试通过！读取了 " << waterReadings.size() << " 次数据" << std::endl;
}

TEST_F(SensorTest, SensorStartStop) {
    // 测试传感器的启停功能
    
    // 先停止传感器
    waterSensor->stop();
    EXPECT_FALSE(waterSensor->isOnline());
    
    // 停止状态下读取应该返回错误值
    double value = waterSensor->readValue();
    EXPECT_DOUBLE_EQ(-999.0, value);
    
    // 重新启动
    bool result = waterSensor->start();
    EXPECT_TRUE(result);
    EXPECT_TRUE(waterSensor->isOnline());
    
    // 现在应该能正常读取
    value = waterSensor->readValue();
    EXPECT_NE(-999.0, value);
    EXPECT_TRUE(isWaterLevelNormal(value));
    
    std::cout << "✅ 传感器启停测试通过！" << std::endl;
}

// 🎯 不使用夹具的简单测试
TEST(SimpleSensorTest, BasicFunctionality) {
    SimpleSensor sensor("TEST_001", 100.0);
    
    // 初始状态应该是离线
    EXPECT_FALSE(sensor.isOnline());
    
    // 启动传感器
    bool result = sensor.start();
    EXPECT_TRUE(result);
    EXPECT_TRUE(sensor.isOnline());
    
    // 读取数据
    double value = sensor.readValue();
    EXPECT_GE(value, 100.0);  // 值应该 >= 100.0
    EXPECT_LE(value, 101.0);  // 值应该 <= 101.0
    
    std::cout << "✅ 基础功能测试通过！传感器值: " << value << std::endl;
}
