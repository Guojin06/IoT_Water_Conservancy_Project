#include "math_operations.h"
#include <cmath>

namespace MathLib {
    
double power(double base, int exponent) {
    return std::pow(base, exponent);
}

double sqrt_approx(double number) {
    return std::sqrt(number);
}

bool is_prime(int number) {
    if (number < 2) return false;
    for (int i = 2; i * i <= number; ++i) {
        if (number % i == 0) return false;
    }
    return true;
}

double factorial(int n) {
    if (n <= 1) return 1.0;
    double result = 1.0;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

} // namespace MathLib
