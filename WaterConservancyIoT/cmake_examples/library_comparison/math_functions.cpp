#include "math_functions.h"

namespace MathLib {

int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

double power(double base, int exp) {
    double result = 1.0;
    for (int i = 0; i < exp; ++i) {
        result *= base;
    }
    return result;
}

} // namespace MathLib
