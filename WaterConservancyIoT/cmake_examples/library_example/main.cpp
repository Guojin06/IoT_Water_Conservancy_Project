#include <iostream>
#include "math_operations.h"  // 来自math库
#include "string_utils.h"     // 来自string库

int main() {
    std::cout << "=== CMake库使用示例 ===" << std::endl;
    
    // 🧮 使用数学库
    std::cout << "\n📊 数学运算:" << std::endl;
    std::cout << "2的10次方 = " << MathLib::power(2, 10) << std::endl;
    std::cout << "16的平方根 = " << MathLib::sqrt_approx(16) << std::endl;
    std::cout << "17是质数吗? " << (MathLib::is_prime(17) ? "是" : "否") << std::endl;
    std::cout << "5的阶乘 = " << MathLib::factorial(5) << std::endl;
    
    // 🔤 使用字符串库
    std::cout << "\n📝 字符串处理:" << std::endl;
    std::string text = "Hello, CMake World!";
    std::cout << "原文: " << text << std::endl;
    std::cout << "大写: " << StringLib::to_upper(text) << std::endl;
    std::cout << "小写: " << StringLib::to_lower(text) << std::endl;
    
    std::string csv = "apple,banana,orange";
    auto fruits = StringLib::split(csv, ',');
    std::cout << "分割结果: ";
    for (const auto& fruit : fruits) {
        std::cout << "[" << fruit << "] ";
    }
    std::cout << std::endl;
    
    std::cout << "重新连接: " << StringLib::join(fruits, " | ") << std::endl;
    std::cout << "是否以Hello开头? " << (StringLib::starts_with(text, "Hello") ? "是" : "否") << std::endl;
    
    std::cout << "\n✅ 库链接成功！CMake工作正常！" << std::endl;
    return 0;
}
