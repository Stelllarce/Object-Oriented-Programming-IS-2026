#include "config.h"
#include <iostream>
/**
 * config.cpp includes config.h and gets its own internal-linkage copies of
 * MAX_RETRIES, COLORS, DIRECTIONS – the same values, but separate objects from
 * any other .cpp that also includes config.h. This is perfectly fine.
 */
void printConfig() {
    std::cout << "Max retries: " << MAX_RETRIES << "\n";

    for (int i = 0; i < COLORS_COUNT; i++) {
        std::cout << "Color: " << COLORS[i] << "\n";
    }

    for (int i = 0; i < DIRECTIONS_COUNT; i++) {
        std::cout << "Direction: " << DIRECTIONS[i] << "\n";
    }

    // COLORS[0] = "yellow"; // Error: pointer is const – cannot be reassigned
}
