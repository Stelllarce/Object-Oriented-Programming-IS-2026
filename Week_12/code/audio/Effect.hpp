#pragma once

class Effect {
public:
    virtual float apply(float signal) = 0;
    virtual const char* name() const = 0;
    virtual const char* describe(const char* descr = nullptr) = 0;
    virtual Effect* clone() = 0;
};