#pragma once

class ISignalLogger {
public:
    virtual void log_stage(const char* effect_name, 
        float input, 
        float output) = 0;
    virtual void log_chain_result(float final_signal) = 0;
};