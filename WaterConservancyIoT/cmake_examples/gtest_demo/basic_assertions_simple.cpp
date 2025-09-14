#include <gtest/gtest.h>
#include <string>

// 🧮 简单的数学函数（我们要测试的代码）
int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

double divide(double a, double b) {
    if (b == 0) return 0.0;  // 简单处理除零
    return a / b;
}

// 🧪 基础断言测试
TEST(BasicMath, Addition) {
    // ✅ 相等性测试
    EXPECT_EQ(4, add(2, 2));          // 2 + 2 = 4
    EXPECT_EQ(0, add(-5, 5));         // -5 + 5 = 0
    EXPECT_EQ(-3, add(-1, -2));       // -1 + (-2) = -3
    
    std::cout << "✅ 加法测试通过！" << std::endl;
}

TEST(BasicMath, Multiplication) {
    // ✅ 相等性测试
    EXPECT_EQ(6, multiply(2, 3));     // 2 * 3 = 6
    EXPECT_EQ(0, multiply(0, 100));   // 0 * 100 = 0
    EXPECT_EQ(-10, multiply(-2, 5));  // -2 * 5 = -10
    
    std::cout << "✅ 乘法测试通过！" << std::endl;
}

TEST(BasicMath, Division) {
    // ✅ 浮点数测试（重要！）
    EXPECT_DOUBLE_EQ(2.0, divide(6.0, 3.0));   // 6 / 3 = 2
    EXPECT_DOUBLE_EQ(0.5, divide(1.0, 2.0));   // 1 / 2 = 0.5
    
    // ✅ 边界情况测试
    EXPECT_DOUBLE_EQ(0.0, divide(5.0, 0.0));   // 除零处理
    
    std::cout << "✅ 除法测试通过！" << std::endl;
}

// 🔤 字符串测试
TEST(StringTest, BasicOperations) {
    std::string hello = "Hello";
    std::string world = "World";
    std::string greeting = hello + ", " + world + "!";
    
    // ✅ 字符串断言
    EXPECT_EQ("Hello, World!", greeting);
    EXPECT_NE("Hi there!", greeting);
    
    // ✅ 字符串长度测试
    EXPECT_EQ(13, greeting.length());
    
    std::cout << "✅ 字符串测试通过！" << std::endl;
}

// 📊 比较测试
TEST(ComparisonTest, Numbers) {
    int big = 100;
    int small = 50;
    
    // ✅ 大小比较
    EXPECT_GT(big, small);       // big > small
    EXPECT_GE(big, 100);         // big >= 100
    EXPECT_LT(small, big);       // small < big
    EXPECT_LE(small, 50);        // small <= 50
    
    std::cout << "✅ 比较测试通过！" << std::endl;
}

// ✅❌ 布尔测试
TEST(BooleanTest, TrueFalse) {
    bool success = true;
    bool error = false;
    
    EXPECT_TRUE(success);        // 期望为真
    EXPECT_FALSE(error);         // 期望为假
    
    // ✅ 实际应用
    std::string text = "Hello";
    EXPECT_FALSE(text.empty());  // 字符串不为空
    EXPECT_TRUE(text.size() > 0); // 字符串有内容
    
    std::cout << "✅ 布尔测试通过！" << std::endl;
}
