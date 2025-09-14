#include <gtest/gtest.h>
#include <stdexcept>
#include <string>

// 🧮 一个会抛出异常的计算器类
class Calculator {
public:
    double divide(double a, double b) {
        if (b == 0.0) {
            throw std::invalid_argument("除数不能为零！");
        }
        return a / b;
    }
    
    int factorial(int n) {
        if (n < 0) {
            throw std::invalid_argument("阶乘参数不能为负数！");
        }
        if (n > 12) {
            throw std::overflow_error("阶乘结果太大，会溢出！");
        }
        
        int result = 1;
        for (int i = 2; i <= n; ++i) {
            result *= i;
        }
        return result;
    }
    
    double sqrt(double x) {
        if (x < 0) {
            throw std::domain_error("负数不能开平方根！");
        }
        
        // 简单的平方根计算（牛顿法）
        if (x == 0) return 0;
        
        double result = x;
        for (int i = 0; i < 10; ++i) {
            result = (result + x / result) / 2.0;
        }
        return result;
    }
};

// 🧪 异常测试
TEST(ExceptionTest, DivisionByZero) {
    Calculator calc;
    
    // ✅ 测试应该抛出特定类型的异常
    EXPECT_THROW(calc.divide(10.0, 0.0), std::invalid_argument);
    
    // ✅ 测试应该抛出任何异常
    EXPECT_ANY_THROW(calc.divide(5.0, 0.0));
    
    // ✅ 测试不应该抛出异常
    EXPECT_NO_THROW(calc.divide(10.0, 2.0));
    
    std::cout << "✅ 除零异常测试通过！" << std::endl;
}

TEST(ExceptionTest, FactorialErrors) {
    Calculator calc;
    
    // ✅ 测试负数参数异常
    EXPECT_THROW(calc.factorial(-1), std::invalid_argument);
    EXPECT_THROW(calc.factorial(-10), std::invalid_argument);
    
    // ✅ 测试溢出异常
    EXPECT_THROW(calc.factorial(20), std::overflow_error);
    
    // ✅ 测试正常情况不抛异常
    EXPECT_NO_THROW(calc.factorial(5));
    EXPECT_NO_THROW(calc.factorial(0));  // 0! = 1
    
    // ✅ 测试正常结果
    EXPECT_EQ(1, calc.factorial(0));
    EXPECT_EQ(1, calc.factorial(1));
    EXPECT_EQ(120, calc.factorial(5));
    
    std::cout << "✅ 阶乘异常测试通过！" << std::endl;
}

TEST(ExceptionTest, SqrtDomainError) {
    Calculator calc;
    
    // ✅ 测试负数开方异常
    EXPECT_THROW(calc.sqrt(-1.0), std::domain_error);
    EXPECT_THROW(calc.sqrt(-100.0), std::domain_error);
    
    // ✅ 测试正常情况
    EXPECT_NO_THROW(calc.sqrt(4.0));
    EXPECT_NO_THROW(calc.sqrt(0.0));
    
    // ✅ 测试结果精度
    double result = calc.sqrt(4.0);
    EXPECT_NEAR(2.0, result, 0.001);  // 允许0.001的误差
    
    result = calc.sqrt(9.0);
    EXPECT_NEAR(3.0, result, 0.001);
    
    std::cout << "✅ 平方根异常测试通过！" << std::endl;
}

// 🎯 复合异常测试
TEST(ExceptionTest, CombinedOperations) {
    Calculator calc;
    
    // ✅ 测试复合操作的异常处理
    
    // 先计算阶乘，再开方
    EXPECT_NO_THROW({
        int fact = calc.factorial(4);  // 4! = 24
        double sqrtResult = calc.sqrt(static_cast<double>(fact));  // √24
        EXPECT_GT(sqrtResult, 4.0);  // √24 > 4
        EXPECT_LT(sqrtResult, 5.0);  // √24 < 5
    });
    
    // 测试异常链
    EXPECT_THROW({
        try {
            calc.factorial(-5);  // 这会抛异常
        } catch (const std::invalid_argument&) {
            // 捕获异常后抛出新异常
            throw std::runtime_error("处理阶乘异常时出错");
        }
    }, std::runtime_error);
    
    std::cout << "✅ 复合异常测试通过！" << std::endl;
}

// 🛡️ 异常信息测试
TEST(ExceptionTest, ExceptionMessages) {
    Calculator calc;
    
    // ✅ 测试异常消息内容
    try {
        calc.divide(10.0, 0.0);
        FAIL() << "应该抛出异常，但没有抛出！";
    } catch (const std::invalid_argument& e) {
        std::string message = e.what();
        EXPECT_TRUE(message.find("除数") != std::string::npos);
        EXPECT_TRUE(message.find("零") != std::string::npos);
        std::cout << "捕获到异常消息: " << message << std::endl;
    }
    
    try {
        calc.factorial(-3);
        FAIL() << "应该抛出异常，但没有抛出！";
    } catch (const std::invalid_argument& e) {
        std::string message = e.what();
        EXPECT_TRUE(message.find("负数") != std::string::npos);
        std::cout << "捕获到异常消息: " << message << std::endl;
    }
    
    std::cout << "✅ 异常消息测试通过！" << std::endl;
}