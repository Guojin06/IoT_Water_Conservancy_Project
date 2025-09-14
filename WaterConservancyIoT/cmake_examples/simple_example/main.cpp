#include <iostream>
#include "calculator.h"

int main() {
    std::cout << "=== 简单CMake示例 ===" << std::endl;
    
    Calculator calc;
    
    int a = 10, b = 5;
    std::cout << a << " + " << b << " = " << calc.add(a, b) << std::endl;
    std::cout << a << " - " << b << " = " << calc.subtract(a, b) << std::endl;
    std::cout << a << " * " << b << " = " << calc.multiply(a, b) << std::endl;
    std::cout << a << " / " << b << " = " << calc.divide(a, b) << std::endl;
    
    std::cout << "CMake编译成功！" << std::endl;
    return 0;
}
