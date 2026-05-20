#pragma once
#include "Effect.hpp"

class Overdrive : public Effect {
public:
    Overdrive(float drive) : drive(drive) {}
    Overdrive() = default;
    float apply(float signal) override;
    const char* name() const override;
    const char* describe(const char* descr = nullptr) override;
    Overdrive* clone() override;
private:
    float drive = 3;
};