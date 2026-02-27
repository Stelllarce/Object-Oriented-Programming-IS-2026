#include <iostream>
#include "math_utils.h"  // Include only declarations
#include "config.h"       // Gets its own internal-linkage copies of the constants

// #include "math_utils.cpp" // NEVER DO THIS – causes multiple definition errors during linking

int main() {
    std::cout << "3 + 4 = " << add(3, 4) << "\n";
    std::cout << "3 * 4 = " << multiply(3, 4) << "\n";

    std::cout << "Max retries: " << MAX_RETRIES << "\n";

    for (int i = 0; i < COLORS_COUNT; i++) {
        std::cout << "Color: " << COLORS[i] << "\n";
    }

    return 0;
}
