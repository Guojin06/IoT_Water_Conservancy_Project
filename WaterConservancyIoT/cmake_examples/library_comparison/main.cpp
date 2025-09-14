#include <iostream>
#include "math_functions.h"

int main() {
    std::cout << "=== 库使用示例 ===" << std::endl;
    
    // 使用数学库
    std::cout << "5 + 3 = " << MathLib::add(5, 3) << std::endl;
    std::cout << "4 * 6 = " << MathLib::multiply(4, 6) << std::endl;
    std::cout << "2的8次方 = " << MathLib::power(2, 8) << std::endl;
    
    std::cout << "库链接成功！" << std::endl;
    return 0;
}
