#include <gtest/gtest.h>
#include <string>
#include <vector>

// 🧮 测试数学运算
TEST(BasicAssertions, MathOperations) {
    // ✅ 相等性断言
    EXPECT_EQ(4, 2 + 2);              // 期望 2+2 等于 4
    EXPECT_EQ(10, 5 * 2);             // 期望 5*2 等于 10
    
    // ✅ 不等性断言
    EXPECT_NE(5, 2 + 2);              // 期望 2+2 不等于 5
    
    // ✅ 大小比较断言
    EXPECT_GT(10, 5);                 // 期望 10 > 5 (Greater Than)
    EXPECT_GE(10, 10);                // 期望 10 >= 10 (Greater Equal)
    EXPECT_LT(3, 8);                  // 期望 3 < 8 (Less Than)
    EXPECT_LE(5, 5);                  // 期望 5 <= 5 (Less Equal)
}

// 🔤 测试字符串操作
TEST(BasicAssertions, StringOperations) {
    std::string hello = "Hello";
    std::string world = "World";
    std::string greeting = hello + ", " + world + "!";
    
    // ✅ 字符串相等
    EXPECT_EQ("Hello, World!", greeting);
    
    // ✅ 字符串不等
    EXPECT_NE("Hi there!", greeting);
    
    // ✅ C字符串比较
    const char* msg = "Test";
    EXPECT_STREQ("Test", msg);         // 字符串相等
    EXPECT_STRNE("Hello", msg);        // 字符串不等
}

// 💯 测试浮点数（重要！）
TEST(BasicAssertions, FloatingPointNumbers) {
    double result = 0.1 + 0.2;
    
    // ❌ 这个会失败！因为浮点数精度问题
    // EXPECT_EQ(0.3, result);
    
    // ✅ 正确的浮点数比较
    EXPECT_DOUBLE_EQ(0.3, result);    // 自动处理精度误差
    EXPECT_NEAR(0.3, result, 1e-10);  // 允许误差范围
    
    // ✅ 浮点数范围测试
    double sensor_value = 15.7;
    EXPECT_GT(sensor_value, 15.0);    // 传感器值应该大于15
    EXPECT_LT(sensor_value, 16.0);    // 传感器值应该小于16
}

// ✅❌ 测试布尔值
TEST(BasicAssertions, BooleanValues) {
    bool is_online = true;
    bool has_error = false;
    
    EXPECT_TRUE(is_online);            // 期望为真
    EXPECT_FALSE(has_error);           // 期望为假
    
    // ✅ 实际应用示例
    std::vector<int> data = {1, 2, 3};
    EXPECT_FALSE(data.empty());        // 期望数据不为空
    EXPECT_TRUE(data.size() > 0);      // 期望有数据
}

// 📍 测试指针
TEST(BasicAssertions, PointerValues) {
    int* valid_ptr = new int(42);
    int* null_ptr = nullptr;
    
    EXPECT_NE(nullptr, valid_ptr);     // 期望指针不为空
    EXPECT_EQ(nullptr, null_ptr);      // 期望指针为空
    
    EXPECT_EQ(42, *valid_ptr);         // 期望指针指向的值为42
    
    delete valid_ptr;  // 记得释放内存
}

// 🎯 运行主函数
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    std::cout << "🧪 开始运行Google Test示例..." << std::endl;
    return RUN_ALL_TESTS();
}
