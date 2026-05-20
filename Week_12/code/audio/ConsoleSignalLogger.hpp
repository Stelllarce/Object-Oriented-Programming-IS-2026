#pragma once
#include "ISignalLogger.hpp"

class ConsoleSignalLogger : public ISignalLogger {
    virtual void log_stage(const char* effect_name, 
        float input, 
        float output) override;
    void log_chain_result(float final_signal) override;
};