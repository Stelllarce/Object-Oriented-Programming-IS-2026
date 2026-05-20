#include "ConsoleSignalLogger.hpp"
#include <iostream>
void ConsoleSignalLogger::log_stage(const char *effect_name, float input, float output) {
    std::cout << effect_name
            << " " << input
            << " " << output << '\n';
}


void ConsoleSignalLogger::log_chain_result(float final_signal) {
    std::cout << final_signal << '\n';
}